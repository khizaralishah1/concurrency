#include <queue>
#include <memory>
#include <future>

/*
  ThreadPool: multiple threads

    work_queues:
      thread_local_queue: w1, w2, w3, work4       [AVOIDS CACHE PING PONG]
      pool_queue: [TAKE WORK FROM GLOBAL QUEUE ONLY IF THERE IS NO WORK IN THREAD'S OWN QUEUE]


      * work stealing: if thread has no work in local queue, none in global (pool) if can steal
  work from other's queue
*/
class FunctionWrapper {};

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

class WorkStealingQueue {
 public:
  typedef FunctionWrapper data_type;

  WorkStealingQueue() {}
  WorkStealingQueue(const WorkStealingQueue& other) = delete;
  WorkStealingQueue& operator=(const WorkStealingQueue& other) = delete;

  void Push(data_type data) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push_front(std::move(data));
  }

  bool Empty() const {
    std::unique_lock<std::mutex> lock(mutex);
    return queue.empty();
  }

  bool TryPop(data_type& result) {
    std::unique_lock<std::mutex> lock(mutex);
    if (queue.empty()) return false;
    result = std::move(queue.front());
    queue.pop_back();
    return true;
  }

  bool TrySteal(data_type& result) {
    std::unique_lock<std::mutex> lock(mutex);
    if (queue.empty()) return false;
    result = std::move(queue.back());
    queue.pop_back();
    return true;
  }

 private:
  std::deque<data_type> queue;
  mutable std::mutex mutex;
};

template <typename T>
class ThreadSafeQueue {};

class ThreadPool {
 public:
  typedef FunctionWrapper task_type;

  std::atomic_bool done;
  ThreadSafeQueue<task_type> pool_work_queue;
  std::vector<std::unique_ptr<WorkStealingQueue>> queues;
  std::vector<std::thread> threads;
  JoinThreads joiner;

  static thread_local WorkStealingQueue* local_work_queue;
  static thread_local unsigned my_index;

  ThreadPool() : done(false), joiner(threads) {
    unsigned const thread_count = std::thread::hardware_concurrency();

    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        queues.push_back(std::unique_ptr<WorkStealingQueue>(new WorkStealingQueue));
        threads.push_back(std::thread(&ThreadPool::Worker, this, i));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }

  ~ThreadPool() { done = true; }

  template <typename F>
  std::future<typename std::result_of<F()>::type> Submit(F f) {
    typedef typename std::result_of<F()>::type result_type;
    std::packaged_task<result_type()> task(f);
    std::future<result_type> result(task.get_future());
    if (local_work_queue)
      local_work_queue->Push(std::move(task));
    else
      pool_work_queue.Push(std::move(task));
    return result;
  }

  void Worker(unsigned my_index_) {
    my_index = my_index_;
    local_work_queue = queues[my_index].get();
    while (!done) RunPendingTask();
  }

  bool PopTaskFromLocalQueue(task_type& task) {
    return local_work_queue && local_work_queue->TryPop(task);
  }

  bool PopTaskFromPoolQueue(task_type& task) { return pool_work_queue.TryPop(task); }

  bool StealTaskFromQueue(task_type& task) {
    for (unsigned i = 0; i < queues.size(); ++i) {
      unsigned const index = (my_index + i + 1) % queues.size();
      if (queues[index]->TrySteal(task)) return true;
    }
    return false;
  }

  void RunPendingTask() {
    task_type task;
    if (PopTaskFromLocalQueue(task) || PopTaskFromPoolQueue(task) || StealTaskFromQueue(task)) {
      task();
    } else
      std::this_thread::yield();
  }

 private:
};