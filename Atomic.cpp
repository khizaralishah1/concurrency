// #include <atomic>
// #include <cassert>

// class Foo {};

// int main() {
//   Foo some_array[5];
//   std::atomic<Foo*> p(some_array);

//   // Returns old value
//   Foo* x = p.fetch_add(2);  // uses std::memory_order_release
//   assert(x == some_array && "Should return old val\r\n");

//   // Returns new value
//   assert(p.load() == &some_array[2]);
//   x = (p -= 1);
//   assert(x == &some_array[1]);
//   assert(p.load() == &some_array[1]);

//   return 0;
// }

/*
  Requirement for user defined type (T) for std::atomic<T>
  * Has trivial (compiler-generated) copy-assignment op
  * It's members should also have same criteria
  * [allows memcpy]
  * Must be bitwise equality comparable (uses memcmp)
*/

// HOW SLOW IS ATOMIC? ~5 to 9 times
#include <chrono>
#include <atomic>
#include <iostream>

int main() {
  int iter = 0;
  int iterations = 100'000'000;
  double t = 0, t2 = 0;
  while (++iter < 10'000) {
    volatile int count = 0;
    std::atomic<int> count2{0};
    auto start = std::chrono::high_resolution_clock::now();
    while (count < iterations) {
      char x = 'a';
      ++count;
    }
    auto end = std::chrono::high_resolution_clock::now();
    t += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();

    while (count2.load(std::memory_order_relaxed) < iterations) {
      // ++count2;
      // count2.fetch_add(1);
      count2.fetch_add(1, std::memory_order_relaxed);
    }

    end = std::chrono::high_resolution_clock::now();
    t2 += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << std::fixed << "t: " << t << ", " << t2 << ". div: " << t2 / t << std::endl;
  }
}