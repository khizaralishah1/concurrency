#include <thread>
#include <vector>
#include <future>

unsigned long const min_per_thread = 25;
unsigned long const hardware_threads = std::thread::hardware_concurrency();

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

template <typename Iterator, typename MatchType>
struct FindElement {
  void operator()(Iterator begin, Iterator end, MatchType match, std::promise<Iterator>* result,
                  std::atomic<bool>* done_flag) {
    try {
      for (; begin != end && !done_flag->load(); ++begin) {
        if (*begin == match) {
          result->set_value(begin);
          done_flag->store(true);
          return;
        }
      }
    } catch (...) {
      try {
        result->set_exception(std::current_exception());
        done_flag->store(true);
      } catch (...) {
      }
    }
  }
};

template <typename Iterator, typename MatchType>
void ParallelFind(Iterator first, Iterator last, MatchType match) {
  unsigned long const length = std::distance(first, last);
  if (!length) return;

  unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

  unsigned long const num_threads =
      std::min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);

  unsigned long const block_size = length / num_threads;

  std::promise<Iterator> result;
  std::atomic<bool> done_flag(false);
  std::vector<std::thread> threads(num_threads - 1);
  JoinThreads joiner(threads);

  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);
    threads[i] = std::thread(FindElement(), block_start, block_end, match, &result, &done_flag);
    block_start = block_end;
  }

  FindElement()(block_start, last, match, &result, &done_flag);

  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    futures[i].get();
  }
}

// SECOND IMPLEMENTATION
template <typename Iterator, typename MatchType>
Iterator ParallelFindImpl(Iterator first, Iterator last, MatchType match,
                          std::atomic<bool>& done) {
  try {
    unsigned long const length = std::distance(first, last);
    if (length < (2*min_per_thread)) {
      for(;(first != last) && !done.load(); ++first) {
        if (*first == match) {
          done = true;
          return first;
        }
      }
      return last;
    } else {
      Iterator const mid = first + length / 2;
      std::future<Iterator> async_result = std::async(&ParallelFindImpl<Iterator, MatchType>, mid, last, match, std::ref(done));
      Iterator const direct_result = ParallelFindImpl(first, mid, match, done);
      return (direct_result == mid) ? async_result.get() : direct_result;
    }

  } catch (...) {
    done = true;
    throw;
  }
}