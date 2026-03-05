template <typename Iterator, typename T>
T ParallelAccumulate(Iterator first, Iterator last, T init) {
  unsigned long const length = std::distance(first, last);
  unsigned long const max_chunk_size = 25;

  if (length <= max_chunk_size) return std::accumulate(first, last, init);

  Iterator mid_point = first;
  std::advance(first, length / 2);
  std::future<T> first_half_result = std::async(ParallelAccumulate<Iterator, T>, first, mid_point,
                                                init);  // takes care of num_hardware_threads
  T second_half_result = ParallelAccumulate(mid_point, last, T()); // if it throws exception, the above future is destroyed and we will wait for async task to finish. TODO: I don't understand this
  return first_half_result.get() + second_half_result;
}