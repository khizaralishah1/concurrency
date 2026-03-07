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

class Barrier {
  std::atomic<unsigned> count;
  std::atomic<unsigned> max_size;
  std::atomic<unsigned> generation;

 public:
  // Implicit conversion error: Complex c1(10, 5); is valid, but Complex c2 = 20;
  explicit Barrier(unsigned count_) : count(count_), max_size(count_), generation(0) {}  // 1

  void Wait() {
    unsigned const my_generation = generation;  // 2
    if (!--max_size) {                          // 3
      max_size = count.load();                  // 4
      ++generation;                             // 5
    } else {
      while (generation == my_generation)  // 6
        std::this_thread::yield();         // 7
    }
  }

  void DoneWaiting() {
    --count;  // TODO: WHY??? YOU REDUCE THE COUNTER, NEXT TIME HOW WILL YOU USE BARRIER?
    if (!--max_size) {
      max_size = count.load();
      ++generation;
    }
  }
};

template <typename Iterator>
struct ProcessElement {  // 1
  typedef typename Iterator::value_type value_type;

  void operator()(Iterator first, Iterator last, std::vector<value_type>& buffer, unsigned i,
                  barrier& barrier) {
    // Logic:
    // t0     t1   t2    t3    t4    t5   t6
    // 0      1    2     3     4     5     6     7     8
    // todo; i don't understand this
    value_type& ith_element = *(first + i);
    bool update_source = false;

    for (unsigned step = 0, stride = 1; stride <= i; ++step, stride *= 2) {
      value_type const& source = (step % 2) ? buffer[i] : ith_element;
      value_type const& dest = (step % 2) ? ith_element : buffer[i];
      value_type const& addend = (step % 2) ? buffer[i - stride] : *(first + i - stride);

      dest = source + addend;
      update_source = !(step % 2);
      barrier.Wait();
    }
    if (update_source) ith_element = buffer[i];
    barrier.DoneWaiting();
  }
};

template <typename Iterator>
void ParallelPartialSum(Iterator first, Iterator last) {
  unsigned long const len = std::distance(first, last);
  if (len <= 1) return;

  std::vector<value_type> buffer(len);
  Barrier barrier(len);

  std::vector<std::thread> threads(len - 1);
  JoinThreads joiner(threads);

  for (unsigned long i = 0; i < (len - 1); ++i) {
    threads[i] =
        std::thread(ProcessElement(), first, last, std::ref(buffer), i, std::ref(barrier));
  }
  ProcessChunk(first, last, buffer, len - 1, barrier);
}