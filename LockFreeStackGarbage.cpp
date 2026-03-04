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

  std::atomic<unsigned> num_threads;
  std::atomic<Node*> to_be_deleted;

 public:
  Stack() : threads(0), to_be_deleted(nullptr) {}

  void Push(T const& data) {
    Node* const new_node = new Node(data);
    new_node->next = head.load();
    while (!head.compare_and_exchange_weak(new_node->next, new_node));
  }

  std::shared_ptr<T> Pop() {
    ++num_threads;
    Node* old_head = head.load();
    while (old_head && !head.compare_and_exchange_weak(old_head, old_head->next));

    // Now we need to delete old_head
    std::shared_ptr<T> result;
    if (old_head)
      result.swap(old_head->data);  // we will eventually return it, but first delete old_head
    TryReclaim(old_head);
    return result;
  }

  void TryReclaim(Node* head) {
    if (num_threads == 1) {
      Node* first_node_to_delete = to_be_deleted.exchange(nullptr);

      // At this point, num_threads could be different, so check again
      if (!--num_threads) {  // is it still just 1 thread?
        DeleteNodes(first_node_to_delete);
      } else {
        AddNodeForDeletionFindLast(first_node_to_delete);
      }

      // DON"T UNDERSTAND THIS???
      // Deleting is time-consuming, might cause other threads to add more nodes to the list
      // So delete those nodes first, and then finally delete your own
      delete old_head;

    } else {
      AddNodeForDeletion(old_head, old_head);
      --num_threads;
    }
  }

  void AddNodeForDeletion(Node* first, Node* last) {
    last->next = to_be_deleted;
    // If to_be_deleted is not changed by anyone, then set it to first
    // I DON'T UNDERSTAND THIS PROPERLY.... OKAY GOT IT AFTER SOME THOUGHT
    while (!to_be_deleted.compare_exchange_weak(last->next, first));
  }

  void AddNodeForDeletionFindLast(Node* first) {
    Node* last = first;
    while (Node* const next = last->next) {
      last = next;
    }
    AddNodeForDeletion(first, last);
  }

  void DeleteNodes(Node* first) {
    while (first) {
      Node* next = first->next;
      delete first;
      first = next;
    }
  }

};