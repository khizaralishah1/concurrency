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
  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType()>::type> Submit(FunctionType f) {
    typedef typename std::result_of<FunctionType()>::type result_type;

    std::packaged_task<result_type()> task(f);
    std::future<result_type> result(task.get_future());
    if (local_work_queue)
      local_work_queue->push(std::move(task));
    else
      pool_work_queue.push(std::move(task));
    return result;
  }

  void RunPendingTask() {
    FunctionWrapper task;

    if (local_work_queue && !local_work_queue->empty()) {
      task = std::move(local_work_queue->front());
      local_work_queue->pop();
      task();
    } else if (pool_work_queue.TryPop(task)) {
      task();
    } else {
      std::this_thread::yield();
    }
  }

 private:
  bool done = false;

  ThreadSafeQueue<FunctionWrapper> pool_work_queue;

  typedef std::queue<FunctionWrapper> local_queue_type;

  static thread_local std::unique_ptr<local_queue_type> local_work_queue;

  void Worker() {
    local_work_queue.reset(new local_queue_type);

    while (!done) {
      RunPendingTask();
    }
  }
};