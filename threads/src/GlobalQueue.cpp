#include <memory>
#include <mutex>

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
    std::unique_ptr<Node> old_head = PopHead();
    return old_head ? old_head->data : std::shared_ptr<T>();
  }

  void Push(T new_value) {
    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<Node> p(new Node);
    Node* const new_tail = p.get();
    std::unique_lock<std::mutex> tail_lock(tail_mutex);
    tail->data = new_data;
    tail->next = std::move(p);
    tail = new_tail;
  }

  bool TryPop(T& value) {}
  void Empty() {}

 private:
  std::unique_ptr<Node> TryPopHead() { std::unique_lock<std::mutex> head_lock(head_mutex); }

  std::unique_ptr<Node> TryPopHead(T& value) {}

  std::mutex head_mutex, tail_mutex;
  std::unique_ptr<Node> head;
  Node* tail;

  Node* GetTail() {
    std::unique_lock<std::mutex> tail_lock(tail_mutex);
    return tail;
  }

  std::unique_ptr<Node> PopHead() {
    std::unique_ptr<Node> head_lock(head_mutex);
    if (head.get() == GetTail()) return nullptr;
    std::unique_ptr<Node> old_head = std::move(head);
    head = std::move(old_head->next);
    return old_head;
  }
};