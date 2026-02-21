#include <future>
#include <cmath>

double SquareRoot(double x) {
  if (x < 0) throw std::out_of_range("x < 0");
  return sqrt(x);
}

int main() {
  std::future<double> f = std::async(SquareRoot, -1);
  double y = f.get();


  return EXIT_SUCCESS;
}