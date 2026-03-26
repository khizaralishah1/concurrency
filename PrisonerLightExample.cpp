/*
  P Prisoners: threads
  1 Switch: bool; initially off


  First solution:
    One is counter, all others are contributors
*/

#include <thread>
#include <iostream>
#include <atomic>
#include <vector>

int num_prisoners = 100;
std::atomic<int> value{0};

std::atomic<bool> light_on{false};

void Counter() {
  value.fetch_add(1);
  int prisoners = 1;
  bool desired = false;
  while (true) {
    bool expected = true;
    if (light_on.compare_exchange_weak(expected, desired)) {
      ++prisoners;
      if (prisoners == num_prisoners) return;
    }
  }
}

void Contributor() {
  while (true) {
    if (light_on) {
      std::this_thread::yield();
    } else {
      light_on.store(true, std::memory_order_release);
      value.fetch_add(1);
      return;
    }
  }
}

int main() {
  std::vector<std::thread> prisoners(num_prisoners);
  prisoners[0] = std::thread(Counter);

  for (int i = 1; i < num_prisoners; ++i) {
    prisoners[i] = std::thread(Contributor);
  }

  for (auto& p : prisoners) {
    p.join();
  }

  std::cout << "value: " << value << std::endl;

  return EXIT_SUCCESS;
}