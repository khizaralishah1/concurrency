#include <vector>

#include "InterruptibleThread.h"
#include "JoinThreads.h"
#include "FunctionWrapper.h"
#include "ThreadTasksQueue.h"

class ThreadPool {
 public:
  static thread_local ThreadTasksQueue* local_work_queue;
  static thread_local unsigned my_index;

  typedef FunctionWrapper TaskType;

  std::atomic_bool done;
  // TODO: Implement
  ThreadSafeQueue<TaskType> global_work_queue;

  ThreadPool();
  ~ThreadPool();

  // TODO: Do we have to call F() to see the invoked result?
  template <typename F, typename... Args>
  std::future<std::invoke_result_t<F, Args...>> Submit(F&& f, Args&&... args);

  void Worker(unsigned my_index_);
  void RunPendingTask();
  bool PopTaskFromLocalQueue(TaskType& task);
  bool PopTaskFromGlobalQueue(TaskType& task);
  bool StealTaskFromQueue(TaskType& task);

 private:
  std::vector<std::unique_ptr<ThreadTasksQueue>> queues;
  std::vector<InterruptibleThread> threads;
  JoinThreads<InterruptibleThread> joiner;
};