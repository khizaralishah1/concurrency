#include <future>
#include <functional>
#include <iostream>

#include "ThreadPool.h"

thread_local ThreadTasksQueue* ThreadPool::local_queue = nullptr;
thread_local unsigned ThreadPool::my_index = 0;

ThreadPool::ThreadPool() : done(false), joiner(threads) {
  unsigned const thread_count = std::thread::hardware_concurrency();

  try {
    for (unsigned i = 0; i < thread_count; ++i) {
      queues.push_back(std::unique_ptr<ThreadTasksQueue>(new ThreadTasksQueue));
      // threads.push_back(std::thread(&ThreadPool::Worker, this, i));
    }
    for (unsigned i = 0; i < thread_count; ++i) {
      threads.emplace_back(&ThreadPool::Worker, this, i);
    }
    std::cout << "[ThreadPool::ThreadPool]" << std::endl;
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
    // if (queues[index]->TrySteal(task)) return true;
  }
  return false;
}

ThreadPool::~ThreadPool() { done = true; }