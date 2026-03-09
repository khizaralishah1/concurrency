#include <memory>
#include <thread>
#include <future>

/*
  std::packaged_task<> instances are NOT copyable, just movable, you can no longer use
  std::function<> because that requires stored function objects to be copy-constructable
*/

class FunctionWrapper {
 public:
  template <typename F>
  FunctionWrapper(F&& f) : impl(new ImplType<F>(std::move(f))) {}
  FunctionWrapper() = default;
  FunctionWrapper(FunctionWrapper&& other) : impl(std::move(other.impl)) {}
  FunctionWrapper(const FunctionWrapper&) = delete;
  FunctionWrapper(FunctionWrapper&) = delete;
  FunctionWrapper& operator=(const FunctionWrapper&) = delete;
  FunctionWrapper& operator=(FunctionWrapper&& other) {
    impl = std::move(other.impl);
    return *this;
  }

  void operator()() { impl->Call(); }

 private:
  struct ImplBase {
    virtual void Call() = 0;
    virtual ~ImplBase() {}
  };

  std::unique_ptr<ImplBase> impl;
  template <typename F>
  struct ImplType : ImplBase {
    F f;
    ImplType(F&& f_) : f(std::move(f_)) {}
    void Call() { f(); }
  };
};

template <typename T>
class ThreadSafeQueue {};

class ThreadPool {
 public:
  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType f) {
    typedef typename std::result_of<FunctionType()>::Type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    queue.push(std::move(task));
    return res;
  }

 private:
  bool done = false;
  ThreadSafeQueue<FunctionWrapper> queue;

  void Worker() {
    while (!done) {
      FunctionWrapper task;
      if (queue.TryPop(task))
        task();
      else
        std::this_thread::yield();
    }
  }
};

/*
  PARALLEL ACCUMULATE
*/

template <typename Iterator, typename T>
T ParallelAccumulate(Iterator first, Iterator last, T init) {
  unsigned long const length = std::distance(first, last);
  if (!length) return;

  unsigned long const block_size = 25;
  unsigned long const num_blocks = (length + block_size - 1) / block_size;

  std::vector<std::future<T>> futures(num_blocks - 1);
  ThreadPool thread_pool;

  Iterator block_start = first;
  for (unsigned long i = 0; i < (num_blocks - 1); ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);
    futures[i] = thread_pool.Submit(AccumulateBlock<Iterator, T>());
    block_start = block_end;
  }
  T last_result = AccumulateBlock<Iterator, T>()(block_start, last);
  T result = init;
  for (unsigned long i = 0; i < (num_blocks - 1); ++i) {
    result += futures[i].get();
  }
  result += last_result;
  return result;
}