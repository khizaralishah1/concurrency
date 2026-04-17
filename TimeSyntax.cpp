#include <iostream>
using namespace std::chrono_literals;  // enables 200ms, 1s, etc.

int main() {
  auto t1 = 20ms;
  auto t2 = 400ms;

  auto dt = t2 - t1;

  std::cout << "dt: " << dt.count() << std::endl;

  return EXIT_SUCCESS;
}