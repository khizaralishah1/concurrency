#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <type_traits>
#include <atomic>
#include <functional>

#include "InterruptibleThread.h"
#include "JoinThreads.h"
#include "FunctionWrapper.h"
#include "ThreadTasksQueue.h"
#include "GlobalQueue.h"

// OLD: typedef FunctionWrapper TaskType;
using TaskType = FunctionWrapper;

class ThreadPool {
 public:
  static thread_local ThreadTasksQueue* local_queue;
  static thread_local unsigned my_index;

  ThreadPool();
  ~ThreadPool();

  template <typename F, typename... Args>
  std::future<std::invoke_result_t<F, Args...>> Submit(F&& f, Args&&... args);

  template <typename F, typename... Args>
  std::future<std::invoke_result_t<F, Args...>> SubmitToThread(unsigned int thread_id, F&& f, Args&&... args);

 private:
  std::atomic_bool done;
  GlobalQueue<TaskType> global_work_queue;
  std::vector<std::unique_ptr<ThreadTasksQueue>> queues;
  std::vector<InterruptibleThread> threads;
  JoinThreads<InterruptibleThread> joiner;

  void Worker(unsigned my_index_);
  void RunPendingTask();
  bool PopTaskFromLocalQueue(TaskType& task);
  bool PopTaskFromGlobalQueue(TaskType& task);
  bool StealTaskFromQueue(TaskType& task);
};

#include "ThreadPool.tpp"

#endif  // THREAD_POOL_H