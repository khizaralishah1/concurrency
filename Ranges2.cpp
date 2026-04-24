#include <iostream>
#include <ranges>
#include <vector>

int main() {
  // C++17 way
  std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<int> result;
  for (auto& x : vec)
    if (x % 2 == 0) result.push_back(x * 2);

  // C++20 way
  // logic: vec -> take even -> transform to *2
  auto result2 = vec | std::views::filter([](int x) { return x % 2 == 0; }) |
                std::views::transform([](int x) { return x * 2; });

  /*
  What I liked :
  --Lazy evaluation(no intermediate containers)
  -- Readable-- Less code,
  -- clearer intent
  */

  return EXIT_SUCCESS;
}