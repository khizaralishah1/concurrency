#include <future>
#include <list>

template <typename T>
std::list<T> QuickSort(std::list<T> input) {
  if (input.empty()) return input;

  // Add first element of input to the results. Deletes the first element of input
  std::list<T> result;
  result.splice(result.begin(), input, input.begin());

  // Takes that first element
  T const& pivot = *result.begin();

  // All the elements will come before --> for which predicate is true
  // All the elements will come later --> for which predicate is false
  auto divide_point =
      std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });
  // Now, the input elements are arranged: all elements below pivot come earlier, others later
  // Divide point: iterator to first element that makes the predicate fail

  // Lower part contains all the input elements from start to pivot
  std::list<T> lower_part;
  lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

  auto new_lower(QuickSort(std::move(lower_part)));

  auto new_higher(QuickSort(std::move(input)));

  result.splice(result.end(), new_higher);
}

/*
 Splice:
  arg: const iterator pos, list& source
       const iterator pos, list& source, const iterator it
       const iterator pos, list& source, const iterator first, const iterator last


  pos: position in destination containter from where the source items will be inserted

  how many items to be taken from source?
    it -> 1
    first to last


    std::list<int> list1 = {1, 2, 3};
    std::list<int> list2 = {4, 5, 6};

    list1.splice(list1.end(), list2);

    list1 = {1, 2, 3, 4, 5, 6};
    list2 = {};
*/