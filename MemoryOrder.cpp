/*
  Three distinct memory ordering models
  1. Relaxed
  2. Acquire and release
        Consume:
        Acquire:
        Release:
        Acq_Rel:
  3. Sequentially consistent:
      -> store SYNCHRONIZES-WITH load
*/

#include <iostream>
#include <atomic>
#include <thread>
#include <cassert>

std::atomic<bool> x, y;
std::atomic<int> z;

void WriteX() { x.store(true, std::memory_order_seq_cst); }

void WriteY() { y.store(true, std::memory_order_seq_cst); }

void ReadXThenY() {
  // 1. Read x
  while (!x.load(std::memory_order_seq_cst));

  // 2. Read y
  if (y.load(std::memory_order_seq_cst)) ++z;
}

void ReadYThenX() {
  // 1. Read y
  while (!y.load(std::memory_order_seq_cst));

  // 2. Read x
  if (x.load(std::memory_order_seq_cst)) ++z;
}

int main() {
  // 1. Init
  x = false;
  y = false;
  z = 0;

  // 2. Threads
  std::thread a(WriteX);
  std::thread b(WriteY);
  std::thread c(ReadXThenY);
  std::thread d(ReadYThenX);
  a.join();
  b.join();
  c.join();
  d.join();
  assert(z.load() != 0);

  return EXIT_SUCCESS;
}