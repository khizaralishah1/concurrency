#ifndef GLOBAL_QUEUE_H
#define GLOBAL_QUEUE_H

#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
struct Node {
  std::shared_ptr<T> data;
  std::unique_ptr<Node> next;
};

template <typename T>
class GlobalQueue {
 public:
  GlobalQueue();

  // No copy
  GlobalQueue(const GlobalQueue& other) = delete;
  GlobalQueue& operator=(const GlobalQueue& other) = delete;

  std::shared_ptr<T> TryPop();
  void Push(T new_value);
  bool TryPop(T& value);
  bool Empty();

 private:
  std::mutex head_mutex, tail_mutex;
  std::unique_ptr<Node<T>> head;
  Node<T>* tail;
  std::condition_variable data_cv;

  std::unique_ptr<Node<T>> WaitPopHead(T& value);
  std::unique_ptr<Node<T>> PopHead();
  Node<T>* GetTail();
};

#include "GlobalQueue.tpp"

#endif  // GLOBAL_QUEUE_H