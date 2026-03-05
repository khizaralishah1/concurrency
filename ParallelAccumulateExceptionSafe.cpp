#include <thread>
#include <functional>
#include <future>

/*
  Before: exception in worker causes std::terminate. BAD

  NOW: exception in worker propagates to the outside (... future.get())

*/

static constexpr unsigned long const hardware_threads = std::thread::hardware_concurrency();
static constexpr unsigned long const min_per_thread = 25;

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

template <typename Iteartor, typename T>
struct AccumulateBlock {
  T operator()(Iterator first, Iterator last) {  // 1
    return std::accumulate(first, last, T());    // 2
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

  std::vector<std::future<T>> futures(num_threads - 1);  // 3
  std::vector<std::thread> threads(num_threads - 1);
  JoinThreads joiner(threads);

  Iterator block_start = first;

  // Catch any exception that may arise between spawning first thread and joining them all
  
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    // Give task to the thread i
    Iterator block_end = block_start;
    std::advance(block_end, block_size);

    // EXCEPTION SAFETY
    std::packaged_task<T>(Iterator, Iterator) > task(AccumulateBlock(<Iterator, T>()));  // 4
    futures[i] = task.get_future();                                                      // 5
    threads[i] = std::thread(std::move(task), block_start, block_end);                   // 6
    block_start = block_end;
  }
  // Give task (last block) to the main thread
  T last_result = AccumulateBlock()(block_start, last);  // 7
  T result = init;  // 8
  for (unsigned long i = 0; i < (num_threads - 1); ++i) {
    result += futures[i].get();  // 9
  }
  result += last_result;  // 10

  return result;
}