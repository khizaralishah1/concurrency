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