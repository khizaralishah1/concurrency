#include <future>

#include "ThreadPool.h"

ThreadPool::ThreadPool() : done(false), joiner(threads) {
  unsigned const thread_count = std::thread::hardware_concurrency();

  try {
    for (unsigned i = 0; i < thread_count; ++i) {
      queues.push_back(std::unique_ptr<ThreadTasksQueue>(new ThreadTasksQueue));
      // threads.push_back(std::thread(&ThreadPool::Worker, this, i));
      threads.emplace_back(&ThreadPool::Worker, this, i);
    }
  } catch (...) {
    done = true;
    throw;
  }
}

void ThreadPool::Worker(unsigned my_index_) {
  my_index = my_index_;
  local_queue = queues[my_index].get();
  while (!done) RunPendingTask();
}

void ThreadPool::RunPendingTask() {
  TaskType task;
  if (PopTaskFromLocalQueue(task) || PopTaskFromGlobalQueue(task) || StealTaskFromQueue(task)) {
    task();
  } else
    std::this_thread::yield();
}

bool ThreadPool::PopTaskFromLocalQueue(TaskType& task) {
  return local_queue && local_queue->TryPop(task);
}

bool ThreadPool::PopTaskFromGlobalQueue(TaskType& task) { return global_work_queue.TryPop(task); }

bool ThreadPool::StealTaskFromQueue(TaskType& task) {
  for (unsigned i = 0; i < queues.size(); ++i) {
    unsigned const index = (my_index + i + 1) % queues.size();
    if (queues[index]->TrySteal(task)) return true;
  }
  return false;
}

// std::result_of was depreciated
template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args...>> ThreadPool::Submit(F&& f, Args&&... args) {
  using result_type = std::invoke_result_t<F, Args...>;

  std::packaged_task<result_type()> task(f);

  auto task = std::packaged_task<result_type()>(
      [func = std::forward<F>(f), ... captured_args = std::forward<Args>(args)]() mutable {
        return std::invoke(std::move(func), std::move(captured_args)...);
      });

  auto result = task.get_future();

  if (local_queue)
    local_queue->Push(std::move(task));
  else
    global_work_queue.Push(std::move(task));

  return result;
}

ThreadPool::~ThreadPool() { done = true; }