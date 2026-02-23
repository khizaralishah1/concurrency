#include <mutex>

template <typename T>
class List {
  struct Node {
    std::mutex mutex;
    std::shared_ptr<T> data;
    std::unique_ptr<Node> next;

    Node() : next() {}

    Node(T const& value) : data(std::make_shared<T>(value)) {}
  };

  Node head;

 public:
  List() {}

  ~List(){RemoveIf([](Node const&) { return true; })}

  List(List const& other) = delete;

  List& operator=(List const& other) = delete;

  void PushFront(T const& value) {
    // h n 1 2 3
    std::unique_ptr<Node> new_node(new Node(value));
    std::unique_lock<std::mutex> lock(head.mutex);
    new_node->next = std::move(head.next);
    head.next = std::move(new_node);
  }

  template <typename Function>
  void ForEach(Function function) {
    // h 1 2 3 4
    Node* current = &head;
    std::unique_lock<std::mutex> lock(head.mutex);
    while (Node* const next = current->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mutex);  // 1 is locked
      lock.unlock();                                        // h in unlocked
      // use 1
      function(*next->data);
      // 1 is current
      current = next;
      lock = std::move(next_lock);
    }
  }

  template <typename Predicate>
  std::shared_ptr<T> FindFirstIf(Predicate predicate) {
    Node* current = &head;
    std::unique_lock<std::mutex> lock(head.mutex);
    while (Node* const& next = current->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mutex);
      lock.unlock();
      if (predicate(*next->data)) return next->data;
      current = next;
      lock = std::move(next_lock);
    }
    return std::shared_ptr<T>();
  }

  template <typename Predicate>
  void RemoveIf(Predicate predicate) {
    Node* current = &head;
    std::unique_lock<std::mutex> lock(head.mutex);

    while (Node* const next = current->next.get()) {
      std::unique_lock<std::mutex> next_lock(next->mutex);
      if (predicate(*next->data)) {
        std::unique_ptr<Node> old_next = std::move(current->next);
        current->next = std::move(next->next);
        next_lock.unlock();  // must unlock, otherwise 'next' will be destroyed and its member
                             // mutex will give undefined behavior because locked mutex is now
                             // going to be destroyed
      } else {
        lock.unlock();
        current = next;
        lock = std::move(next_lock);  // ? why
      }
    }
  }
};