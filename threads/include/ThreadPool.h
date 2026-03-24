#include <t

#include "InterruptibleThread.h"

class ThreadPool {
 public:
  typedef FunctionWrapper task_type;

  std::atomic_bool done;
  ThreadSafeQueue<task_type> pool_work_queue;
  std::vector<std::unique_ptr<WorkStealingQueue>> queues;
  std::vector<InterruptibleThread> threads;
  JoinThreads joiner;

  static thread_local WorkStealingQueue* local_work_queue;
  static thread_local unsigned my_index;

  ThreadPool();
  ~ThreadPool();

  template <typename F>
  std::future<typename std::result_of<F()>::type> Submit(F f);

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