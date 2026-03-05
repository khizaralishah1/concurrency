#include <thread>
#include <functional>

static constexpr unsigned long const hardware_threads = std::thread::hardware_concurrency();
static constexpr unsigned long const min_per_thread = 25;

template <typename Iteartor, typename T>
struct AccumulateBlock {
  void operator()(Iterator first, Iterator last, T& result) {
    result = std::accumulate(first, last, result);  // 1
  }
};

template <typename Iterator, typename T>
T ParallelAccumulate(Iterator first, Iterator last, T init) {
  unsigned long const length = std::distance(first, last);  // 2
  if (!length) return init;

  unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
  unsigned long const num_threads =
      std::min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);
  unsigned long const block_size = length / num_threads;

  std::vector<T> results(num_threads);                // 3
  std::vector<std::thread> threads(num_threads - 1);  // 4

  Iterator block_start = first;  // 5
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    // Give task to the thread i
    Iterator block_end = block_start;  // 6
    std::advance(block_end, block_size);
    threads[i] = std::thread(AccumulateBlock<Iterator, T>(), block_start, block_end,
                             std::ref(results[i]));  // 7
    // after first thread is constructed... now if exception occurs then that thread will call
    // std::terminate
    // AccumulateBlock can throw
    block_start = block_end;  // 8
  }
  // Give task (last block) to the main thread
  AccumulateBlock()(block_start, last, results[num_threads - 1]);  // 9

  // Join
  // TODO: what is std::mem_fn?
  std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

  return std::accumulate(results.begin(), results.end(), init);  // 10
}