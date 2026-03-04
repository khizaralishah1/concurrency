/*
   I haven't properly understood the reclamation technique using hazard pointers. Have to come back
   to it later
*/

#include <thread>
#include <atomic>
#include <functional>
#include <memory>

unsigned const max_hazard_pointers = 100;

struct HazardPointer {
  std::atomic<std::thread::id> id;
  std::atomic<void*> pointer;
};

HazardPointer hazard_pointers[max_hazard_pointers];

class HpOwner {
 public:
  HpOwner(HpOwner const&) = delete;
  HpOwner operator=(HpOwner const&) = delete;

  HpOwner() : hp(nullptr) {
    for (unsigned i = 0; i < max_hazard_pointers; ++i) {
      std::thread::id old_id;
      if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
        hp = &hazard_pointers[i];
        break;
      }
    }
    if (!hp) {
      throw std::runtime_error("No hazard pointers available");
    }
  }

  std::atomic<void*>& GetPointer() { return hp->pointer; }

  ~HpOwner() {
    hp->pointer.store(nullptr);
    hp->id.store(std::thread::id());
  }

 private:
  HazardPointer* hp;
};

std::atomic<void*>& GetHazardPointerForCurrentThread() {
  thread_local static HpOwner hazard;  // will be created once per thread. not once overall
  return hazard.GetPointer();
}

bool OutstandingHazardPointersFor(void* p) {
  for (unsigned i = 0; i < max_hazard_pointers; ++i) {
    if (hazard_pointers[i].pointer.load() == p) return true;
  }

  return false;
}

template <typename T>
void DoDelete(void* p) {
  delete static_cast<T*>(p);
}

struct DataToReclaim {
  void* data;
  std::function<void(void*)> deleter;
  DataToReclaim* next;

  template <typename T>
  DataToReclaim(T* p) : data(p), deleter(&DoDelete<T>), next(0) {}

  ~DataToReclaim() { deleter(data); }
};

std::atomic<DataToReclaim*> nodes_to_reclaim;

void AddToReclaimList(DataToReclaim* node) {
  node->next = nodes_to_reclaim.load();
  while (!nodes_to_reclaim.compare_exchange_weak(node->next, node));
}

template <typename T>
void ReclaimLater(T* data) {
  AddToReclaimList(new DataToReclaim(data));
}

void DeleteNodesWithNoHazards() {
  DataToReclaim* current = nodes_to_reclaim.exchange(nullptr);

  while (current) {
    DataToReclaim* const next = current->next;
    if (!OutstandingHazardPointersFor(current->data)) {
      delete current;
    } else {
      AddToReclaimList(current);
    }
    current = next;
  }
}

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

  std::atomic<void*>& GetHazardPointerForCurrentThread() {}

  void Push(T const& data) {
    Node* const new_node = new Node(data);
    new_node->next = head.load();
    while (!head.compare_and_exchange_weak(new_node->next, new_node));
  }

  std::shared_ptr<T> Pop() {
    ++num_threads;

    // 1. Hazard pointer
    std::atomic<void*>& hp = GetHazardPointerForCurrentThread();

    Node* old_head = head.load();

    // 2. Set hazard pointer to head
    do {
      Node* temp;
      do {
        temp = old_head;
        hp.store(old_head);
        old_head = head.load();
      } while (old_head != temp);
    }

    while (old_head && !head.compare_and_exchange_strong(
                           old_head, old_head->next));  // if "weak" then spurious failure results
                                                        // in setting the hp unncessarily

    // 3. Clear the hazard pointer. Means you may delete the head
    hp.store(nullptr);

    // Now we need to delete old_head
    std::shared_ptr<T> result;
    if (old_head) {
      result.swap(old_head->data);  // we will eventually return it, but first delete old_head

      if (OutStandingHpFor(old_head))
        ReclaimLater(old_head);
      else
        delete old_head;
      DeleteNodeswithNoHazards();
    }
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