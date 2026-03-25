#include "GlobalQueue.h"

template <typename T>
GlobalQueue<T>::GlobalQueue() : head(new Node<T>), tail(head.get()) {}

template <typename T>
std::shared_ptr<T> GlobalQueue<T>::TryPop() {
  std::unique_ptr<Node<T>> old_head = WaitPopHead();
  return old_head ? old_head->data : std::shared_ptr<T>();
}

template <typename T>
void GlobalQueue<T>::Push(T new_value) {
  std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
  std::unique_ptr<Node<T>> p(new Node<T>);
  Node<T>* const new_tail = p.get();
  {
    std::unique_lock<std::mutex> tail_lock(tail_mutex);
    tail->data = new_data;
    tail->next = std::move(p);
    tail = new_tail;
  }
  data_cv.notify_one();
}

template <typename T>
bool GlobalQueue<T>::TryPop(T& value) {
  std::unique_ptr<Node<T>> old_head = WaitPopHead(value);
  return old_head ? true : false;
}

template <typename T>
bool GlobalQueue<T>::Empty() {
  std::unique_lock<std::mutex> head_lock(head_mutex);
  if (head.get() == GetTail()) return true;
  return false;
}

template <typename T>
std::unique_ptr<Node<T>> GlobalQueue<T>::WaitPopHead(T& value) {
  // std::unique_lock<std::mutex> head_lock(WaitForData());
  std::unique_lock<std::mutex> head_lock(head_mutex);
  if (head.get() == GetTail()) return nullptr;
  value = std::move(*head->data);
  return PopHead();
}

template <typename T>
Node<T>* GlobalQueue<T>::GetTail() {
  std::unique_lock<std::mutex> tail_lock(tail_mutex);
  return tail;
}

template <typename T>
std::unique_ptr<Node<T>> GlobalQueue<T>::PopHead() {
  std::unique_ptr<Node<T>> old_head = std::move(head);
  head = std::move(old_head->next);
  return old_head;
}
