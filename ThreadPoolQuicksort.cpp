

template <typename T>
struct Sorter {
  ThreadPool pool;

  std::list<T> DoSort(std::list<T>& chunk_data) {
    if (chunk_data.empty()) return chunk_data;

    std::list<T> result;
    result.splice(result.begin(), chunk_data, chunk_data.begin());
    T const& partition_value = *result.begin();

    typename std::list<T>::iterator divide_point = std::paritition(
        chunk_data.begin(), chunk_data.end(), [&](T const& val) { return val < partition_val });

    std::list<T> new_lower_chunk;
    new_lower_chunk.splice(new_lower_chunk.end(), chunk_data, chunk_data.begin(), divide_point);

    std::future<std::list<T>> new_lower =
        pool.Submit(std::bind(&Sorter::DoSort, this, std::move(new_lower_chunk)));

    std::future<std::list<T>> new_higher(DoSort(chunk_data));

    result.splice(result.end(), new_higher);

    while (!new_lower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
      pool.RunPendingTask();
    }

    result.splice(result.begin(), new_lower.get());
    return result;
  }
};