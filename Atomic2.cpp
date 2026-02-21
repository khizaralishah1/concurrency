#include <vector>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

std::vector<int> data;
std::atomic<bool> data_ready(false);

void ReaderThread() {
  while (!data_ready.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::cout << "[ReaderThread]: The answer: " << data[0] << "\r\n";
}

void WriterThread() {
  data.push_back(42);
  data_ready = true;
}

int main() {
  std::thread t1(WriterThread);
  std::thread t2(ReaderThread);

  t1.join();
  t2.join();

  return EXIT_SUCCESS;
}