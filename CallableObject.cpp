#include <iostream>

struct Callable {
  void operator()() {
    std::cout << "[Callable] Hey!" << std::endl;
  }
};

int main() {
  Callable callable_object;
  callable_object();

  return 0;
}