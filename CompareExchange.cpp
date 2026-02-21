#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> sync(0);

void thread_1() {
  sync.store(1, std::memory_order_release);
}

void thread_2() {
  int expected = 1;

  while(!sync.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
    expected = 1;
  }
  std::cout << "[Thread2]:\r\n";
}

void thread_3() {
  while(sync.load(std::memory_order_acquire) < 2);
  std::cout << "[Thread3]:\r\n";
}

int main() {
  std::thread t1(thread_1);
  std::thread t2(thread_2);
  std::thread t3(thread_3);

  return 0;
}