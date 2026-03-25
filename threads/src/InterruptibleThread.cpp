#include <thread>
#include <future>
#include <condition_variable>

#include "InterruptibleThread.h"

thread_local InterruptFlag this_thread_interrupt_flag;

template <typename F, typename... Args>
InterruptibleThread::InterruptibleThread(F&& f, Args&&... args) {
  std::promise<InterruptFlag*> p;
  // Forward the external f and args properly (rvalues are forwarded using std::foward)
  internal_thread = std::thread(
      [func = std::forward<F>(f), ... CapturedArgs = std::forward<Args>(args), &p]() mutable {
        p.set_value(&this_thread_interrupt_flag);

        // Call function with args
        std::invoke(std::move(func), std::move(CapturedArgs)...);
      });

  // Guarantees that flag points to this_thread_interrupt_flag. Will only continue once promise
  // has set the value
  flag = p.get_future().get();
}

void InterruptibleThread::Interrupt() {
  if (flag) flag->Set();
}

void InterruptibleThread::join() {
  if (internal_thread.joinable()) internal_thread.join();
}

// Allow move
InterruptibleThread::InterruptibleThread(InterruptibleThread&& other) noexcept
    : internal_thread(std::move(other.internal_thread)), flag(other.flag) {
  other.flag = nullptr;
}

InterruptibleThread& InterruptibleThread::operator=(InterruptibleThread&& other) noexcept {
  if (this != &other) {
    internal_thread = std::move(other.internal_thread);
    flag = other.flag;
    other.flag = nullptr;
  }
  return *this;
}

/***************** THESE WILL RUN IN THE GIVEN FUNCTION, AT RELEVANT POINTS ****************/

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
  while (!this_thread_interrupt_flag.IsSet()) {
    if (uf.wait_for(lock, std::chrono::milliseconds(1)) == std::future_status::ready) {
      break;
    }
    InterruptionPoint();
  }
}

void InterruptionPoint() {
  if (this_thread_interrupt_flag.IsSet()) {
    // Throw thread interrupted, custom
    throw ThreadInterrupted{};
  }
}