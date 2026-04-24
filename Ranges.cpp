#include <ranges>
#include <vector>
#include <view>

int main() {
  // Traditional
  std::vector<int> numbers(10);
  std::vector<int> result;
  for (int i = 0; i < numbers.size(); ++i) {
    if (numbers[i] % 2 == 0) {
      result.push_back(numbers[i] * numbers[i]);
    }
  }

  /* C++20 Ranges

  std::views::filter produces a view of elements from the underlying range
  that satisfy a predicate, skipping those that do nog

  */
  

  auto result = numbers | std::views::filter(func) | 


  return EXIT_SUCCESS;
}