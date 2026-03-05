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

template <typename Iterator, typename Func>
void ParallelForEach(Iterator first, Iterator last, Func func) {
  unsigned long const length = std::distance(first, last);
  if (!length) return;
  unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

  unsigned long const num_threads =
      std::min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);

  unsigned long const block_size = length / num_threads;

  std::vector<std::future<void>> futures(num_threads - 1);
  std::vector<std::thread> threads(num_threads - 1);
  JoinThreads joiner(threads);

  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);
    std::packaged_task<void> task([=]() { std::for_each(block_start, block_end, func) });
    futures[i] = task.get_future();
    threads[i] = std::thread(std::move(task));
    block_start = block_end;
  }

  std::for_each(block_start, last, func);
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    futures[i].get();
  }
}

// SECOND IMPLEMENTATION
template <typename Iterator, typename Func>
void ParallelForEach(Iterator first, Iterator last, Func func) {
  unsigned long const len = std::distance(first, last);
  if (!len) return;

  if (len < (2 * min_per_thread))
    std::for_each(first, last, func);
  else {
    Iterator const mid = first + len / 2;
    std::future<void> first_half = std::async(&ParallelForEach<Iterator, Func>, first, mid, func);
    ParallelForEach(mid, last, func);
    first_half.get();
  }
}