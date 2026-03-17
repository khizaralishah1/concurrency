// From lectures of 'Karan IITgn'
// At https://www.youtube.com/watch?v=ItI2qQVeLcU

/* Rationale:

  ONLY USE WHEN WE NEED TO CREATE/DESTROY OBJECTS REALLY REALLY FAST (HOT PATh )

  Problem: Latency issues when we create too many objects (and destroy) of some class X

  Solution: At program startup, if I know max objects needed, I create memory of N number of such
  objects
        => When we want a "new Object" then I can just pick an object from the pool instead of
  creating new ones
        => Delete also is similar

*/

#include <chrono>
#include <iostream>
#include <vector>

class MemoryManager {
 public:
  MemoryManager(int num_objects_, int size_) {
    memory = new char[num_objects_ * size_];

    for (int i = 0; i < num_objects_; ++i) {
      free_list.push_back(memory + i * size_);
    }
  }

  ~MemoryManager() { delete[] memory; }

  void* AllocateMemory() {
    void* obj_addr = free_list.back();
    free_list.pop_back();
    return obj_addr;
  }

  void DeallocateMemory(void* obj_addr) { free_list.push_back(obj_addr); }

 private:
  char* memory;
  std::vector<void*> free_list;
};

class Order1 {   // Allocated using memory manager
  int quantity;  // 4 bytes

 public:
  static MemoryManager memory_manager;

  void* operator new(const size_t size) { return memory_manager.AllocateMemory(); }

  void operator delete(void* memory_address) {
    return memory_manager.DeallocateMemory(memory_address);
  }

  Order1(int quantity_) : quantity(quantity_) {}

  void LogOrder() const {
    std::cout << "[Order]: Object Address:\t" << this << ", quantity = " << quantity << std::endl;
  }
};

class Order {    // Allocated using new
  int quantity;  // 4 bytes

 public:
  Order(int quantity_) : quantity(quantity_) {}

  void LogOrder() const {
    std::cout << "[Order]: Object Address:\t" << this << ", quantity = " << quantity << std::endl;
  }
};

int num_objects = 10;
MemoryManager Order1::memory_manager(num_objects, sizeof(Order1));

int main() {
  for (int i = 0; i < 100000; ++i) {
    int iterations = 100'000;

    auto t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(
                  std::chrono::high_resolution_clock::now().time_since_epoch())
                  .count();

    for (int i = 0; i < iterations; ++i) {
      Order* obj1 = new Order(i + 100);
      Order* obj2 = new Order(i + 200);
      Order* obj3 = new Order(i + 100);
      Order* obj4 = new Order(i + 200);
      Order* obj5 = new Order(i + 100);
      Order* obj6 = new Order(i + 200);
      Order* obj7 = new Order(i + 100);
      Order* obj8 = new Order(i + 200);
      delete obj1;
      delete obj2;
      delete obj3;
      delete obj4;
      delete obj5;
      delete obj6;
      delete obj7;
      delete obj8;
    }

    auto t2 = std::chrono::duration_cast<std::chrono::nanoseconds>(
                  std::chrono::high_resolution_clock::now().time_since_epoch())
                  .count();
    double t_a = t2 - t1;
    // std::cout << "[main]: Without memory manager. T = " << t_a << std::endl;

    for (int i = 0; i < iterations; ++i) {
      Order1* obj1 = new Order1(i + 100);
      Order1* obj2 = new Order1(i + 200);
      Order1* obj3 = new Order1(i + 100);
      Order1* obj4 = new Order1(i + 200);
      Order1* obj5 = new Order1(i + 100);
      Order1* obj6 = new Order1(i + 200);
      Order1* obj7 = new Order1(i + 100);
      Order1* obj8 = new Order1(i + 200);
      delete obj1;
      delete obj2;
      delete obj3;
      delete obj4;
      delete obj5;
      delete obj6;
      delete obj7;
      delete obj8;
    }

    t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::high_resolution_clock::now().time_since_epoch())
             .count();

    // std::cout << "[main]: With    memory manager. T = " << t1 - t2 << std::endl;

    // std::cout << "Difference: " << (t1 - t2) / (t_a) << " times faster\r\n";
  }
}