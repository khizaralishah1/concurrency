#include <iostream>

// Implementation of lambda: Lambda is an anonymous class with operator()
struct MyLambda {
  int x;
  int operator() (int y) const {
    return x + y;
  }
};


double SomeFunction() { return 1; }

int main() {
  int x = 10;

  auto default_lambda = [&]() {
    std::cout << "Default lambda, x: " << x << std::endl;
    return SomeFunction();
  };

  MyLambda my_lambda;

  default_lambda();

  return EXIT_SUCCESS;
}
