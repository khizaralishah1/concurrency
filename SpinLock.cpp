#include <atomic>

class SpinLockMutex {
 public:
  SpinLockMutex() : flag(ATOMIC_FLAG_INIT) {}

  void Lock() {
    // Everything afterwards will see the changes done properly
    while (flag.test_and_set(std::memory_order_acquire));
  }

  void Unlock() {
    // Everything you did before, is visible
    flag.clear(std::memory_order_release);
  }

 private:
  std::atomic_flag flag;
};