// #include <iostream>
// #include <atomic>
// #include <thread>

// std::atomic<int> outer_buffer{0};
// std::atomic<int> inner_buffer{0};

// std::atomic_bool data_available1{true};
// std::atomic_bool data_available2{false};

// void F1() {
//   std::cout << "[F1]: Start\r\n";
//   while (1) {
//     if (data_available1.load(std::memory_order_relaxed)) {
//       data_available1.store(false, std::memory_order_relaxed);
//       inner_buffer.fetch_add(1, std::memory_order_relaxed);
//       std::this_thread::yield();  // widen race window
//       data_available2.store(true, std::memory_order_relaxed);
//     }
//   }
//   std::cout << "[F1]: End\r\n";
// }

// void F2() {
//   std::cout << "[F2]: Start\r\n";
//   while (1) {
//     if (data_available2.load(std::memory_order_relaxed)) {
//       data_available2.store(false, std::memory_order_relaxed);
//       outer_buffer.fetch_add(1, std::memory_order_relaxed);

//       int in = inner_buffer.load(std::memory_order_relaxed);
//       int out = outer_buffer.load(std::memory_order_relaxed);

//       if (in != out) {
//         std::cout << "Outer and inner buffer are different\r\n";
//       }

//       std::this_thread::yield();  // widen race window

//       data_available1.store(true, std::memory_order_relaxed);
//     }
//   }
//   std::cout << "[F2]: End\r\n";
// }

// int main() {
//   std::thread t1(F1);
//   std::thread t2(F2);

//   t1.join();
//   t2.join();
//   std::cout << "[main]: End\r\n";
//   return EXIT_SUCCESS;
// }

// #include <atomic>
// #include <thread>
// #include <iostream>

// std::atomic<int> x{0};
// std::atomic<int> y{0};

// int r1 = 0;
// int r2 = 0;

// int main() {
//   for (long i = 0;; i++) {
//     x.store(0, std::memory_order_relaxed);
//     y.store(0, std::memory_order_relaxed);
//     r1 = r2 = 0;

//     std::thread t1([] {
//       x.store(1, std::memory_order_relaxed);
//       std::this_thread::yield();
//       r1 = y.load(std::memory_order_relaxed);
//     });

//     std::thread t2([] {
//       y.store(1, std::memory_order_relaxed);
//       std::this_thread::yield();
//       r2 = x.load(std::memory_order_relaxed);
//     });

//     t1.join();
//     t2.join();

//     if (r1 == 0 && r2 == 0) {
//       std::cout << "Reordering detected at iteration " << i << "\n";
//       break;
//     }
//   }
// }

// #include <atomic>
// #include <thread>

// std::atomic<int> x{0};
// std::atomic<int> y{0};

// int r1 = 0;
// int r2 = 0;

// int main() {
//   for (long i = 0;; i++) {
//     x.store(0, std::memory_order_relaxed);
//     y.store(0, std::memory_order_relaxed);
//     r1 = r2 = 0;

//     std::thread t1([&] {
//       int tmp = y.load(std::memory_order_relaxed);
//       x.store(tmp, std::memory_order_relaxed);
//       r1 = x.load(std::memory_order_relaxed);
//     });

//     std::thread t2([&] {
//       int tmp = x.load(std::memory_order_relaxed);
//       y.store(tmp, std::memory_order_relaxed);
//       r2 = y.load(std::memory_order_relaxed);
//     });

//     t1.join();
//     t2.join();

//     if (r1 != 0 || r2 != 0) {
//       break;
//     }
//   }
//   return -1;
// }

// #include <thread>
// #include <atomic>
// #include <iostream>

// int x = 0, y = 0;
// int r1 = 0, r2 = 0;

// void thread1() {
//   x = 1;   // write to x
//   std::this_thread::sleep_for(std::chrono::nanoseconds(1));
//   r1 = y;  // read y
// }

// void thread2() {
//   y = 1;   // write to y
//   std::this_thread::sleep_for(std::chrono::nanoseconds(1));
//   r2 = x;  // read x
// }

// int main() {
//   int count = 0;
//   while (true) {
//     x = y = r1 = r2 = 0;

//     std::thread t1(thread1);
//     std::thread t2(thread2);

//     t1.join();
//     t2.join();

//     if (r1 == 0 && r2 == 0) {
//       std::cout << "Reordering observed! " << count << "\r\n";
//       break;
//     }

//     if (++count % 100000 == 0) {
//       std::cout << "Still trying...: " << count << "\r\n";
//     }
//   }
// }

#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

int x = 0, y = 0;
int r1 = 0, r2 = 0;

void thread1() {
  x = 1;  // write to x
  std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  r1 = y;  // read y
}

void thread2() {
  y = 1;  // write to y
  std::this_thread::sleep_for(std::chrono::nanoseconds(1));
  r2 = x;  // read x
}

int main() {
  int iter = 0;
  while (++iter < 1000) {
    volatile int count = 0;
    std::atomic<int> count2{0};
    auto start = std::chrono::high_resolution_clock::now();
    while (count < 100'000'000) {
      char x = 'a';
      ++count;
    }
    auto end = std::chrono::high_resolution_clock::now();
    double t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();

    while (count2 < 100'000'000) {
      ++count2;
    }

    end = std::chrono::high_resolution_clock::now();
    double t2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << std::fixed << "t: " << t << ", " << t2 << ". div: " << t2 / t << std::endl;
  }
}