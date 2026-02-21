#include <future>
#include <list>

// 0. Run quick sort on the input
template <typename T>
std::list<T> QuickSort(std::list<T> input) {
  if (input.empty()) return input;

  // 1. Take first element as pivot
  std::list<T> result;
  result.splice(result.begin(), input, input.begin());
  T const& pivot = *result.begin();

  // 2. Input orders the elements. Predicate true are earlir. False later
  auto divide_point =
      std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });

  // 3. Input is divided into: [lower_part, input]
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

  // 0a. Run quick sort on the lower part
  std::future<std::list<T>> new_lower(std::async(QuickSort<T>, std::move(lower_part)));

  // 0b. Run quick sort on the higher part. Not using future because main can do it on its own
  std::list<T> new_higher(QuickSort<T>, std::move(input));

  // 4. Merge lower and higher parts
  result.splice(result.end(), new_higher);
  result.splice(result.begin(), new_lower.get());  // Will wait

  // IMPORTANT: if too many threads have been spawned by std::async, C++ will decide to do more
  // using deferred, run only when future is asked!

  return result;
}