#include <atomic>
#include <thread>
#include <iomanip>
#include <chrono>
#include <iostream>

constexpr uint64_t iterations = 1'000'000'000ull;


struct Counters {
  std::atomic<uint64_t> counter1{0};
  std::atomic<uint64_t> counter2{0};
};

struct alignas(64) PaddedCounters {
  std::atomic<uint64_t> counter1{0};  // 8 bytes
  char _pad1[56];
  std::atomic<uint64_t> counter2{0};
  char _pad2[56];
};

template <typename T>
void RunBenchmark(const char* name) {
  T counters;

  const auto start = std::chrono::high_resolution_clock::now();

  std::thread t1([&] {
    for (uint64_t i = 0; i < iterations; ++i) {
      counters.counter1.fetch_add(1, std::memory_order_relaxed);
    }
  });

  std::thread t2([&] {
    for (uint64_t i = 0; i < iterations; ++i) {
      counters.counter2.fetch_add(1, std::memory_order_relaxed);
    }
  });

  t1.join();
  t2.join();

  const auto end = std::chrono::high_resolution_clock::now();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << std::left << std::setw(40) << name << ": " << std::right << std::setw(6) << ms
            << " ms. " << "(c1 = " << counters.counter1.load() << ", " << counters.counter2.load()
            << ")\r\n";
}

int main() {
  for (int i = 0; i < 10; ++i) {
    RunBenchmark<Counters>("Without padding (false sharing): ");
    RunBenchmark<PaddedCounters>("With padding: ");
    std::cout << "\r\n";
  }
}