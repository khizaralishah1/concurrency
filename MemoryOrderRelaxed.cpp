#include <iostream>
#include <atomic>
#include <thread>
#include <cassert>

std::atomic<bool> x, y;
std::atomic<int> z;

/*
 * Has order between stores. And between loads
 * NO order between store-load
 * So, assert can fire
 */

void WriteXThenY() {
  x.store(true, std::memory_order_relaxed);
  y.store(true, std::memory_order_relaxed);
}

void ReadYThenX() {
  // 1. Read y
  while (!y.load(std::memory_order_relaxed));

  // 2. Read x
  if (x.load(std::memory_order_relaxed)) ++z;
}

int main() {
  for (int i = 0; i < 100000; ++i) {
    // 1. Init
    x = false;
    y = false;
    z = 0;

    // 2. Threads
    std::thread a(WriteXThenY);
    std::thread b(ReadYThenX);
    a.join();
    b.join();
    assert(z.load() != 0);
  }
  std::cout << "[Main] Finished\r\n";

  return EXIT_SUCCESS;
}