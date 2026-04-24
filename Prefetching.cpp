#include <iostream>

inline void prefetch(const void* ptr) {
#if defined(__GNUC__)
  __builtin_prefetch(ptr);
#elif defined(_MSC_VER)
  _mm_prefetch((const char*)ptr, _MM_HINT_T0);
#endif
}

inline int HeavyCompute(int x) {
  for (int i = 0; i < 50; ++i) x = x * 1664525 + 1013904223;  // simple LCG
  return x;
}

void ExampleSlow() {
  volatile int sum = 0;
  int n = 50'000'000;
  std::vector<int> arr(n, 1);

  const auto start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i + 16 < n; ) {
    sum += HeavyCompute(arr[i]);
    i += 4;
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "[ExampleSlow]: Time taken: " << ms << "ms" << std::endl;
}

void ExamplePrefetch() {
  volatile int sum = 0;
  int n = 50'000'000;
  std::vector<int> arr(n, 1);

  const auto start = std::chrono::high_resolution_clock::now();

  for (size_t i = 0; i + 16 < n; ) {
    prefetch(&arr[i + 16]);  // look ahead... start cooking next dish
    sum += HeavyCompute(arr[i]);
    i += 4;
  }

  const auto end = std::chrono::high_resolution_clock::now();
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "[ExamplePrefetch]: Time taken: " << ms << "ms" << std::endl;
}

int main() {
  ExampleSlow();
  ExamplePrefetch();

  return EXIT_SUCCESS;
}
