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

template <typename T>
class ThreadSafeQueue {};

class ThreadPool {
 public:
  ThreadPool() : done(false), joiner(threads) {
    unsigned const thread_count = std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; ++i)
        threads.push_back(std::thread(&ThreadPool::Worker, this));
    } catch (...) {
      done = true;
      throw;
    }
  }

  ~ThreadPool() { done = false; }

  template <typename FunctionType>
  void Submit(FunctionType function) {
    work_queue.push(std::function<void()>(f));
  }

 private:
  std::atomic_bool done;
  ThreadSafeQueue<std::function<void()>> work_queue;
  std::vector<std::thread> threads;
  JoinThreads joiner;

  void Worker() {
    while (!done) {
      std::function<void()> task;
      if (work_queue.TryPop(task))
        task();
      else
        std::this_thread::yield();
    }
  }
};