#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

#include "ThreadPool.h"

void Function() { std::cout << "[Function]" << std::endl; }

int main() {
  ThreadPool thread_pool;

  auto dt = std::chrono::seconds(1);
  while (1) {
    std::this_thread::sleep_for(dt);
    thread_pool.Submit(Function);
    thread_pool.
  }

  std::cout << "[main]: Exit" << std::endl;
  return 0;
}