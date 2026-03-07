#include <future>
#include <vector>

class JoinThreads {
 public:
  explicit JoinThreads(std::vector<std::thread>& threads_) : threads(threads_) {}
  ~JoinThreads() {
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable()) threads[i].join();
    }
  }

 private:
  std::vector<std::thread>& threads;
};

template <typename Iterator>
struct ProcessChunk {  // 1
  typedef typename Iterator::value_type value_type;

  void operator()(Iterator begin, Iterator last, std::future<value_type>* previous_end_value,
                  std::promise<value_type>* end_value) {
    // Logic: Partial sum of range. Then previous is added to each element of the result. Return
    // last
    Iterator end = last;
    ++end;
    std::partial_sum(begin, end, begin);               // 2
    if (previous_end_value) {                          // 3
      value_type& addend = previous_end_value->get();  // 4
      *last += addend;  // 5 [Update 'last' first so that next chunk can proceed!]
      if (end_value) end_value->set_value(*last);                                  // 6
      std::for_each(begin, last, [addend](value_type& item) { item += addend; });  // 7
    } else if (end_value) {
      end_value->set_value(*last);  // 8
    }
  }
};

template <typename Iterator>
void ParallelPartialSum(Iterator first, Iterator last) {
  unsigned long const len = std::distance(first, last);
  if (!len) return last;

  unsigned long const min_per_thread = 25;  // 12
  unsigned long const max_threads = (len + min_per_thread - 1) / min_per_thread;
  unsigned long const hw_threads = std::thread::hardware_concurrency();
  unsigned long const num_threads = std::min(hw_threads ? hw_threads : 2, max_threads);

  unsigned long const block_size = len / num_threads;

  typedef typename Iterator::value_type value_type;

  std::vector<std::thread> threads(num_threads - 1);                  // 13
  std::vector<std::promise<value_type>> end_values(num_threads - 1);  // 14
  std::vector<std::future<value_type>> previous_end_values;           // 15
  previous_end_values.reserve(num_threads - 1);                       // 16
  JoinThreads joiner(threads);

  //         s     l
  // 0 1 2 3 4 5 6 7 8 9
  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    Iterator block_last = block_start;
    std::advance(block_last, block_size);  // 17
    threads[i] = std::thread(ProcessChunk(), block_start, block_last,
                             i ? &previous_end_values[i - 1] : 0, &end_values[i]);  // 18
    block_start = block_last;
    ++block_start;                                              // 19
    previous_end_values.push_back(end_values[i].get_future());  // 20
  }
  Iterator final_element = block_start;
  std::advance(final_element, std::distance(block_start, last) - 1);  // 21
  ProcessChunk(block_start, final_element, (num_threads > 1) ? &previous_end_values.back() : 0,
               0);  // 22
}