// also known as RVO = Return-Value Optimization
#include <iostream>

int g = 0;

struct S {
  int x;

  S(int x_) : x(x_) {
    static int i = 0;
    std::cout << "construct " << x << " and " << g++ << std::endl;
  }

  S(const S& other, std::string a = "") {
    std::cout << "copy " << g++ << ". a = " << a << std::endl;
  }

  S(const S&& other, std::string a = "") {
    std::cout << "move " << g++ << ". a = " << a << std::endl;
  }
};

S MakeStruct(S in) {
  std::cout << "MakeStruct 1" << std::endl;
  return std::move(S(in, "bbbbb"));
}

int main() {
  S s(MakeStruct(S(0)), "aaaa");

  /*
    -fno-elide-constructors

    1. S(0) constructed
    2. Copy to function argument
    3. Copy and created in the return statement
    4. Return value copied to the outside
    5. Main s is created through copy

  */

  return EXIT_SUCCESS;
}
