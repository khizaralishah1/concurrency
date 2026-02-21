// #include <atomic>
// #include <thread>
// #include <iostream>
// #include <assert.h>

// std::atomic<bool> x, y;
// std::atomic<int> z;

// void write_x() { x.store(true, std::memory_order_release); }

// void write_y() { y.store(true, std::memory_order_release); }

// void read_x_then_y() {
//   while (!x.load(std::memory_order_acquire));
//   if (y.load(std::memory_order_acquire)) ++z;
// }

// void read_y_then_x() {
//   while (!y.load(std::memory_order_acquire));
//   if (x.load(std::memory_order_acquire)) ++z;
// }

// int main() {
//   x = false;
//   y = false;
//   z = 0;

//   std::thread a(write_x);
//   std::thread b(write_y);
//   std::thread c(read_x_then_y);
//   std::thread d(read_y_then_x);

//   a.join();
//   b.join();
//   c.join();
//   d.join();

//   assert(z.load() != 0);

//   return EXIT_SUCCESS;
// }

#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
  x.store(true, std::memory_order_relaxed);
  y.store(true, std::memory_order_release);
}

void read_y_then_x() {
  while (!y.load(std::memory_order_acquire));

  // y.store and y.load are synchronized
  // we are 100% sure that x happened before y
  
  if (x.load(std::memory_order_relaxed)) ++z;
}

int main() {
  x = false;
  y = false;
  z = 0;

  std::thread a(write_x_then_y);
  std::thread b(read_y_then_x);

  a.join();
  b.join();

  assert(z.load() != 0);

  return 0;
}