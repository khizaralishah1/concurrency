#include <iostream>


double SomeFunction() {
  return 1;
}

int main() {



  double lambda = [&]() { return SomeFunction(); }
}