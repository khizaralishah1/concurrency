#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);

unsigned const loop_count = 10;

struct ReadValues {
  int x, y, z;
};

ReadValues v1[loop_count];
ReadValues v2[loop_count];
ReadValues v3[loop_count];
ReadValues v4[loop_count];
ReadValues v5[loop_count];

void Increment(std::atomic<int>* var, ReadValues* v) {
  // 1. Only start running when go is true
  while (!go) std::this_thread::yield();

  // 2. Store the global (x,y,z) into the struct
  for (unsigned int i = 0; i < loop_count; ++i) {
    v[i].x = x.load(std::memory_order_relaxed);
    v[i].y = y.load(std::memory_order_relaxed);
    v[i].z = z.load(std::memory_order_relaxed);
    var->store(i + 1, std::memory_order_relaxed);
    std::this_thread::yield();
  }
}

void ReadVals(ReadValues* v) {
  while (!go) std::this_thread::yield();

  for (unsigned int i = 0; i < loop_count; ++i) {
    v[i].x = x.load(std::memory_order_relaxed);
    v[i].y = y.load(std::memory_order_relaxed);
    v[i].z = z.load(std::memory_order_relaxed);
    std::this_thread::yield();
  }
}

void Print(ReadValues* v) {
  for (unsigned int i = 0; i < loop_count; ++i) {
    if (i) std::cout << ",";
    std::cout << "[Print]: " << v[i].x << " " << v[i].y << " " << v[i].z;
  }
  std::cout << "\r\n";
}

int main() {
  std::thread t1(Increment, &x, v1);
  std::thread t2(Increment, &y, v2);
  std::thread t3(Increment, &z, v3);
  std::thread t4(ReadVals, v4);
  std::thread t5(ReadVals, v5);

  go = true;

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();

  Print(v1);
  Print(v2);
  Print(v3);
  Print(v4);
  Print(v5);

  return EXIT_SUCCESS;
}