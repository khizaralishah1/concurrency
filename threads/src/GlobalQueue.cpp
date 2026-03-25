#include <memory>
#include <mutex>
#include <condition_variable>

#include "GlobalQueue.h"

template <typename T>
struct Node {
  std::shared_ptr<T> data;
  std::unique_ptr<Node> next;
};

template <typename T>
class GlobalQueue {
 public:
  GlobalQueue() : head(new Node), tail(head.get()) {}

  // No copy
  GlobalQueue(const Queue& other) = delete;
  GlobalQueue& operator=(const Queue& other) = delete;

  std::shared_ptr<T> TryPop() {
    std::unique_ptr<Node> old_head = WaitPopHead();
    return old_head ? old_head->data : std::shared_ptr<T>();
  }

  void Push(T new_value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<Node> p(new Node);
    Node* const new_tail = p.get();
    {
      std::unique_lock<std::mutex> tail_lock(tail_mutex);
      tail->data = new_data;
      tail->next = std::move(p);
      tail = new_tail;
    }
    data_cv.notify_one();
  }

  bool TryPop(T& value) {
    std::unique_ptr<Node> old_head = WaitPopHead(value);
    return old_head ? true : false;
  }

  bool Empty() {
    std::unique_lock<std::mutex> head_lock(head_mutex);
    if (head.get() == GetTail()) return true;
    return false;
  }

 private:
  std::mutex head_mutex, tail_mutex;
  std::unique_ptr<Node> head;
  Node* tail;
  std::condition_variable data_cv;

  std::unique_ptr<Node> WaitPopHead(T& value) {
    // std::unique_lock<std::mutex> head_lock(WaitForData());
    std::unique_lock<std::mutex> head_lock(head_mutex);
    value = std::move(*head->data);
    return PopHead();
  }

  Node* GetTail() {
    std::unique_lock<std::mutex> tail_lock(tail_mutex);
    return tail;
  }

  std::unique_ptr<Node> PopHead() {
    std::unique_ptr<Node> old_head = std::move(head);
    head = std::move(old_head->next);
    return old_head;
  }
};