#include <iostream>
#include <arm_neon.h>
#include <cstdlib>
#include <chrono>
#include <vector>

/*
  vld1q_f32 = Loads 4 consecutive floats into 128-bit NEON register
  vaddq_f32 = Adds 2 such wide vectors
  vst1q_f32 = Stores this wide vector back to memory

*/

void ScalarAddition(const float* a, const float* b, float* result, int n) {
#pragma clang loop vectorize(disable) interleave(disable)
  for (int i = 0; i < n; ++i) {
    result[i] = a[i] + b[i];
  }
}

void NeonAddition(const float* a, const float* b, float* result, int n) {
  int i = 0;
  for (; i + 4 <= n; i += 4) {
    float32x4_t r = vaddq_f32(vld1q_f32(&a[i]), vld1q_f32(&b[i]));
    vst1q_f32(&result[i], r);
  }
  // Leftovers
  for (; i < n; ++i) {
    result[i] = a[i] + b[i];
  }
}

template <typename Callable>
double TimeMs(Callable&& function, int repeats) {
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < repeats; ++i) function();
  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration<double, std::milli>(end - start).count();
}

int main() {
  const int repeats = 10'000;
  const int size = 100'000;
  std::vector<float> a(size), b(size), out_scalar(size), out_neon(size);

  for (int i = 0; i < size; ++i) {
    a[i] = static_cast<float>(i);
    b[i] = static_cast<float>(i) * 2.f;
  }

  double scalar_ms =
      TimeMs([&] { ScalarAddition(a.data(), b.data(), out_scalar.data(), size); }, repeats);
  double neon_ms =
      TimeMs([&] { NeonAddition(a.data(), b.data(), out_neon.data(), size); }, repeats);

  bool match = true;
  for (int i = 0; i < size; ++i) {
    if (out_scalar[i] != out_neon[i]) {
      match = false;
      std::cout << "[Main]:: Fail iteration " << i << ". Values " << out_scalar[i] << ", "
                << out_neon[i] << std::endl;
      break;
    }
  }

  std::cout << std::fixed << "Time ms (Scalar, Neon): " << scalar_ms << ", " << neon_ms
            << std::endl;

  return EXIT_SUCCESS;
}