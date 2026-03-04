#include <memory>

template <typename T>
class Stack {
 private:
  struct Node {
    std::shared_ptr<T> data;
    Node* next;
    Node(T const& data_) : data(std::make_shared<T>(data_)) {}
  };

  std::atomic<Node*> head;

 public:
  void Push(T const& data) {
    Node* const new_node = new Node(data);

    // 1.
    new_node->next = head.load();

    // 2. New head
    //      Make sure head has same value as new_node->next ===> head.compare_and_exchange_weak(OLD
    //      VALUE, NEW VALUE)
    while (!head.compare_and_exchange_weak(new_node->next, new_node));
    // A.compare_and_exchange_weak(B, C) ===> means, if A==B then set it to C
    // so if A is not modified by someone else, it can safely be set to something in this thread

    // Concern: what if context-switch happens after 1. Other thread makes a new node and sets its
    // next to head Now, new_node 1 and new_node 2 both are pointing towards the head

    // MOST IMPORTANT: EXPECTED UPDATES! head.compare_and_exchange(expected, desired)
    // FAILURE OVERWRITES THE EXPECTED!
  }

  void Pop(std::shared_ptr<T>& result) {
    // Elements: A, B, C, D
    // Let's say 12 threads come here.
    // They take the same head and want to delete it
    Node* old_head = head.load(); // A

    // 1. Thread i will make the second element as new head (B)
    // 2. Context-Switch. Thread j sees that the value of old_head (A) is different from the head now (B)
    // ... Thread k, l, m... all see that. They repeat
    // 3. Thread x updates the old_head and points to the new head properly. (B) That that new head is the same one
    // that Thread i is pointing to (B)
    // All threads are pointing to B but while loop is still running
    // If thread j sees that head is B now, it can make the head C. Other threads will follow
    // Meanwhile, thread i will return A data. J will return B data, etc
    while (old_head && !head.compare_and_exchange_weak(old_head, old_head->next));
    result = old_head ? old_head->data : std::shared_ptr<T>();
  }
};

// template <typename T>
// class Stack {
//  private:
//   struct Node {
//     T data;
//     Node* next;
//     Node(T const& data_) : data(data_) {}
//   };

//   std::atomic<Node*> head;

//  public:
//   void Push(T const& data) {
//     Node* const new_node = new Node(data);

//     // 1.
//     new_node->next = head.load();

//     // 2. New head
//     while (!head.compare_and_exchange_weak(new_node->next, new_node));
//   }

//   void Pop(T& result) {
//     Node* old_head = head.load();
//     while (!head.compare_and_exchange_weak(old_head, old_head->next));
//     result = old_head->data;
//   }
// };