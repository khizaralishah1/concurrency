#include <mutex>
#include <deque>

#include "FunctionWrapper.h"

/*
 * This queue contains functions
 * New functions can be added to the front
 * Owner thread can pick up from front
 * Other threads can steal from back
 */

class ThreadTasksQueue {
 public:
  typedef FunctionWrapper DataType;

  ThreadTasksQueue();

  // No copy
  ThreadTasksQueue(const ThreadTasksQueue& other) = delete;
  ThreadTasksQueue& operator=(const ThreadTasksQueue& other) = delete;

  // TODO: move necessary or not? Probably not. Unsure

  void Push(DataType data);
  bool Empty() const;
  bool TryPop(DataType& result);
  bool TrySteal(DataType& result);

 private:
  std::deque<DataType> queue;
  mutable std::mutex mutex;
};