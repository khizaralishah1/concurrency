#include "ThreadTasksQueue.h"

ThreadTasksQueue::ThreadTasksQueue() {}

void ThreadTasksQueue::Push(DataType data) {
  // Latest task -> front
  std::unique_lock<std::mutex> lock(mutex);
  queue.push_front(std::move(data));
}

bool ThreadTasksQueue::Empty() const {
  std::unique_lock<std::mutex> lock(mutex);
  return queue.empty();
}

bool ThreadTasksQueue::TryPop(DataType& popped_data) {
  std::unique_lock<std::mutex> lock(mutex);
  if (queue.empty()) return false;
  popped_data = std::move(queue.front());
  queue.pop_back();
  return true;
}

bool ThreadTasksQueue::TrySteal(DataType& stolen_data) {
  std::unique_lock<std::mutex> lock(mutex);
  if (queue.empty()) return false;
  stolen_data = std::move(queue.back());
  queue.pop_back();
  return true;
}