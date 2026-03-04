template <typename T>
class Queue {
 public:
  Queue() : head(new Node), tail(head.load()) {}

  Queue(const Queue& other) = delete;
  Queue& operator=(const Queue& other) = delete;

  // b a
  // a b c d e f
  void Push(T value) {
    // std::shared_ptr<T> new_data(std::make_shared<T>(value));
    // Node* p = new Node;
    // Node* const old_tail = tail.load();
    // old_tail->data.swap(new_data);
    // old_tail->next = p;
    // tail.store(p);

    std::unique_ptr<T> new_data(new T(new_value));
    CountedNode new_next;
    new_next.ptr = new Node;
    new_next.external_count = 1;

    for (;;) {
      Node* const old_tail = tail.load();
      T* old_data = nullptr;

      // If old_tail's has data field null, it means you have the real tail now, update it.
      // Everybody else will fail because now this 'tail' is not a real tail because it's data
      // field is not null. So others will REFRESH
      // TODO: how will it refresh for others? it will just fail
      // Oh! It refreshes using next loop iteration where tail is updated
      if (old_tail->data.compare_exchange_strong(old_data, new_data.get())) {
        // Other threads are not accessing this 'fake tail' now. They could be
        // accessing....NOTHING???? Just looping until tail is updated inside
        old_tail->next = new_next;
        tail.store(new_next.ptr);  // Now others can access this tail
        new_data.release();
        break;
      }
    }
  }

  std::shared_ptr<T> Pop() {
    Node* old_head = PopHead();
    if (!old_head) return std::shared_ptr<T>();
    std::shared_ptr<T> const result(old_head->data);
    delete old_head;
    return result;
  }

  ~Queue() {
    while (Node* const old_head = head.load()) {
      head.store(old_head->next);
      delete old_head;
    }
  }

 private:
  Node* PopHead() {
    Node* const old_head = head.load();
    if (old_head == tail.load()) return nullptr;
    head.store(old_head->next);
    return old_head;
  }

  struct Node {
    std::atomic<T*> data;
    std::atomic<NodeCounter> count;
    CountedNode next;

    Node() {
      NodeCounter new_count;
      new_count.internal_count = 0;
      new_count.extern_counters = 2;
      count.store(new_count);
      next.ptr = nullptr;
      next.external_count = 0;
    }
  };

  struct CountedNode {
    int external_count;
    Node* node_ptr;
  };

  struct NodeCounter {
    unsigned internal_count : 30;
    unsigned external_counters : 2;
  }

  std::atomic<CountedNode>
      head;
  std::atomic<CountedNode> tail;
};