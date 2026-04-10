#include <ranges>
#include <vector>
#include <algorithm>
#include <iostream>

void PrintVector(const std::vector<int>& n) {
  for (int i = 0; i < n.size(); ++i) {
    std::cout << n[i] << " ";
  }
}

int main() {
  std::vector<int> numbers{1, 2, 3, 4, 5, 6, 7};

  /*
    std::views::filter produces a view of elements from the underlying range
    that satisfy a predicate, skipping those that do not
  */

  auto filtered = numbers | std::views::filter([](int v) { return v % 2 == 0; });

  // Filtered is a bidirectional common range (TODO: WHAT???)
  std::vector<int> output(filtered.begin(), filtered.end());

  std::cout << "Filtered: "; PrintVector(output);  

  return EXIT_SUCCESS;
}