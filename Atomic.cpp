#include <atomic>
#include <cassert>

class Foo {};

int main() {
  Foo some_array[5];
  std::atomic<Foo*> p(some_array);

  // Returns old value
  Foo* x = p.fetch_add(2);  // uses std::memory_order_release
  assert(x == some_array && "Should return old val\r\n");

  // Returns new value
  assert(p.load() == &some_array[2]);
  x = (p -= 1);
  assert(x == &some_array[1]);
  assert(p.load() == &some_array[1]);

  return 0;
}

/*
  Requirement for user defined type (T) for std::atomic<T>
  * Has trivial (compiler-generated) copy-assignment op
  * It's members should also have same criteria
  * [allows memcpy]
  * Must be bitwise equality comparable (uses memcmp)
*/