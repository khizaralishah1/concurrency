#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

#include "ThreadPool.h"

void Function() { std::cout << "[Function]" << std::endl; }

void Function2(int i, int j) { std::cout << "[Function] " << i << " " << j << std::endl; }

int main() {
  ThreadPool thread_pool;

  auto dt = std::chrono::seconds(1);
  int i = 0;
  unsigned int num_threads = std::thread::hardware_concurrency();

  while (1) {
    thread_pool.Submit(Function);
    thread_pool.Submit(Function);
    thread_pool.Submit(Function);

    int thread_id = i % num_threads;
    ++i;

    // thread_pool.SubmitToThread(thread_id, Function2, thread_id, 10);
    std::this_thread::sleep_for(dt);
    // thread_pool.
  }

  std::cout << "[main]: Exit" << std::endl;
  return 0;
}