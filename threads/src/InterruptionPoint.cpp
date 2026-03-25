#include "InterruptFlag.h"

template <typename Predicate>
void InterruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lock,
                       Predicate predicate) {
  InterruptionPoint();
  this_thread_interrupt_flag.SetCV(
      cv);  // When you will be interrupting this thread by setting the flag, you will see that its
  // cv was also set, so you wake cv as well
  InterruptFlag::ClearCVOnDestruct guard;
  InterruptionPoint();
  while (!this_thread_interrupt_flag.IsSet() && !predicate) {
    cv.wait_for(lock, std::chrono::milliseconds(1));
  }
  this_thread_interrupt_flag.ClearCV();
  InterruptionPoint();
}

template <typename Lockable, typename Predicate>
void InterruptibleWait(std::condition_variable& cv, Lockable& lock, Predicate predicate) {
  this_thread_interrupt_flag.Wait(cv, lock, predicate);
}

template <typename T>
void InterruptibleWait(std::future<T>& uf) {
  // while (!this_thread_interrupt_flag.IsSet()) {
  //   if (uf.wait_for(lock, std::chrono::milliseconds(1)) == std::future_status::ready) {
  //     break;
  //   }
  //   InterruptionPoint();
  // }
}

void InterruptionPoint() {
  if (this_thread_interrupt_flag.IsSet()) {
    // Throw thread interrupted, custom
    throw ThreadInterrupted{};
  }
}