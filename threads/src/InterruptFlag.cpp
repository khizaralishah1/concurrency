#include "InterruptFlag.h"
#include "CustomLock.h"

InterruptFlag::InterruptFlag() : thread_cond(nullptr), thread_cond_any(nullptr) {}

void InterruptFlag::Set() {
  flag.store(true, std::memory_order_relaxed);
  std::unique_lock<std::mutex> lock(set_clear_mutex);
  if (thread_cond) {
    thread_cond->notify_all();
  } else if (thread_cond_any) {
    thread_cond_any->notify_all();
  }
}

bool InterruptFlag::IsSet() const { return flag.load(std::memory_order_relaxed); }

void InterruptFlag::SetCV(std::condition_variable& cv) {
  std::unique_lock<std::mutex> lock(set_clear_mutex);
  thread_cond = &cv;
}

void InterruptFlag::ClearCV() {
  std::unique_lock<std::mutex> lock(set_clear_mutex);
  thread_cond = nullptr;
}

struct ClearCVOnDestruct {
  ~ClearCVOnDestruct() { this_thread_interrupt_flag.ClearCV(); }
};

template <typename Lockable, typename Predicate>
void Wait(std::condition_variable_any& cv, Lockable& lock, Predicate predicate) {
  /*
    Case 1: Set occurs before Wait => Caught by first InterruptionPoint
    Case 2: Set occurs ater custom_lock is constructed
            => SET sets the flag, but waits on custom lock to notify the cv
            => WAIT (this function) Sees interrupt and gets out
            => custom_lock's destructor unlocks
    Case 3: Set occurs after first InterruptionPoint
            => SETS the flag but waits on custom_lock
            => WAIT does cv_wait. Checks predicate and the flag.
            => The flag is set, so it returns
  */

  CustomLock custom_lock(this, cv, lock);
  // here: we still have the lock. no interruption is paused
  InterruptionPoint();
  cv.wait(custom_lock, predicate);  // calls unlock (releases set_clear_mutex... Set can run)
  InterruptionPoint();
}
