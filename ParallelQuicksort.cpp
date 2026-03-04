#include <vector>

template <typename T>
struct Sorter {
  struct ChunkToSort {
    std::list<T> data;
    std::promise<std::list<T>> promise;
  };

  Stack<ChunkToSort> chunks;
  std::vector<std::thread> threads;
  unsigned const max_threads;
  std::atomic<bool> end_of_data;

  Sorter() : max_threads(std::thread::hardware_concurrency() - 1), end_of_data(false) {}

  ~Sorter() {
    end_of_data = true;
    for (unsigned i = 0; i < threads.size(); ++i) {
      threads[i].join();
    }
  }

  void TrySortChunk() {
    boost::shared_ptr<ChunkToSort> chunk = chunks.pop();
    if (chunk) SortChunk(chunk);
  }

  void SortChunk(boost::shared_ptr<ChunkToSort> const& chunk) {
    chunk->promise.set_value(DoSort(chunk->data));
  }

  std::list<T> DoSort(std::list<T>& chunk_data) {
    if (chunk_data.empty) return chunk_data;

    // result = chunk_data
    std::list<T> result;
    result.splice(result.begin(), chunk_data, chunk_data.begin());

    // partition = result[0]
    T const& partition_value = *result.begin();

    // divide where value >= partition
    std::list<T>::iterator divide_point =
        std::partition(chunk_data.begin(), chunk_data.end(),
                       [&](T const& value) { return value < partition_value });

    // Fill new lower chunk = [chunk_data, divider]
    ChunkToSort new_lower_chunk;
    new_lower_chunk.data.splice(new_lower_chunk.data.end(), chunk_data, chunk_data.begin(),
                                divide_point);
    std::future<std::list<T>> new_lower = new_lower_chunk.promise.get_future();
    chunks.push(std::move(new_lower_chunk));


    if (threads.size() < max_threads) {
      threads.push_back(std::thread(&Sorter<T>::SortThread, this));
    }

    std::list<T> new_higher(DoSort(chunk_data));

    // result = new_higher
    result.splice(result.end(), new_higher);
    while (new_lower.wait_for(std::chrono::seconds(0)) != std::future_status_ready) {
      // If new_lower is not processed due to max threads, then we will pool until thread is available
      TrySortChunk();
    }

    // result = new_lower, new_higher
    result.splice(result.begin(), new_lower.get());
    return result;
  }

  void SortThread() {
    while (!end_of_data) {
      TrySortChunk();
      std::this_thread::yield();
    }
  }
};

template<typename T>
std::list<T> ParallelSort(std::list<T> input) {
  if (input.empty()) return input;
  Sorter<T> sorter;
  return sorter.DoSort(input);
}