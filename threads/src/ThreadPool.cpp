#include <future>

#include "ThreadPool.h"

ThreadPool::ThreadPool() : done(false), joiner(threads) {
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

// Submit a new task
template <typename F>
std::future<typename std::result_of<F()>::type> ThreadPool::Submit(F f) {
  typedef typename std::result_of<F()>::type result_type;
  std::packaged_task<result_type()> task(f);
  std::future<result_type> result(task.get_future());
  if (local_work_queue)
    local_work_queue->Push(std::move(task));
  else
    pool_work_queue.Push(std::move(task));
  return result;
}

ThreadPool::~ThreadPool() { done = true; }