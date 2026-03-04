#include <memory>

/*
TODO: come back to this later. page 211 -- end of chapter

*/

template <typename T>
class Stack {
 public:
  void Push(T const& data) {
    CountedNode new_node;
    new_node.external_count = 1;
    new_node.node_ptr = new Node(data);
    // Memory ordering (1)
    new_node.node_ptr->next = head.load(std::memory_order_relaxed);
    while (!head.compare_exchange_weak(new_node.node_ptr->next, new_node,
                                       std::memory_order_release, std::memory_order_relaxed));

    // if "new_node->next is STILL POINTING to the original HEAD" ===> new node becomes the head
    while (!std::atomic_compare_exchange_weak(&head, &(new_node.node_ptr->next), new_node));
    // Will keep failing if someone else has updated the head, so we will keep updating
    // "new_node->next = updated head" until we see that we are now pointing to the updated head,
    // and then we make new node as the new head
  }

  std::shared_ptr<T> Pop() {
    CountedNode old_head = head.load(std::memory_order_relaxed);

    for (;;) {
      // 1. Increase internal reference to this node (this one will be deleted)
      CountedNode new_head;
      do {
        new_head = old_head;
        ++new_head.external_count;
      } while (!head.compare_exchange_strong(old_head, new_head, std::memory_order_acquire,
                                             std::memory_order_relaxed));

      // TODO: i don't understand why we have to update old_head now
      old_head.external_count = new_head.external_count;

      // 2.
      Node* const ptr = old_head.node_ptr;
      if (!ptr) return std::shared_ptr<T>();

      // If HEAD is still pointing to the original head, now update the head to the next
      if (head.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) {
        std::shared_ptr<T> result;
        result.swap(ptr->data);

        // remove current thread's references. result is the 'all other threads' number of
        // references'
        int const count_increase = old_head.external_count - 2;  // TODO: why?

        if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) ==
            -count_increase) {
          delete ptr;
        }

        return result;
      } else if (ptr->internal_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
        // head is pointing somewhere else .... subtract its internal reference then start again
        ptr->internal_count.load(std::memory_order_acquire);  // TODO: why?
        delete ptr;
      }
    }
  }

 private:
  std::atomic<CountedNode> head;

  struct Node {
    // Shared ptr so that Nodes are not copied, and external world can just reference them
    std::shared_ptr<T> data;
    CountedNode next;
    std::atomic<int> internal_count;

    Node(T const& data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
  };

  struct CountedNode {
    int external_count;
    Node<T>* node_ptr;
  };
};

