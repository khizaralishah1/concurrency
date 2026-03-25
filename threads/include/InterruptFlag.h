#ifndef INTERRUPT_FLAG_H
#define INTERRUPT_FLAG_H

#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <future>

void InterruptionPoint();

template <typename T>
void InterruptibleWait(std::future<T>& uf);

template <typename Lockable, typename Predicate>
void InterruptibleWait(std::condition_variable& cv, Lockable& lock, Predicate predicate);

template <typename Predicate>
void InterruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lock,
                       Predicate predicate);

struct ThreadInterrupted : std::exception {
  const char* what() const noexcept override { return "Thread interrupted"; }
};

class InterruptFlag;

/**************** CUSTOM LOCK *****************/

template <typename Lockable>
struct CustomLock {
  InterruptFlag* self;
  Lockable& lock;

  CustomLock(InterruptFlag* self_, std::condition_variable_any& cv, Lockable& lock_);
  ~CustomLock();

  void Unlock();
  void Lock();
};

extern thread_local InterruptFlag this_thread_interrupt_flag;

class InterruptFlag {
 public:
  InterruptFlag();
  void Set();

  bool IsSet() const;
  void SetCV(std::condition_variable& cv);

  void ClearCV();

  struct ClearCVOnDestruct;

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

    CustomLock<Lockable> custom_lock(this, cv, lock);
    // here: we still have the lock. no interruption is paused
    InterruptionPoint();
    cv.wait(custom_lock, predicate);  // calls unlock (releases set_clear_mutex... Set can run)
    InterruptionPoint();
  }

 private:
  std::atomic<bool> flag;
  std::condition_variable* thread_cond;
  std::condition_variable_any* thread_cond_any;
  std::mutex set_clear_mutex;
};

#endif  // INTERRUPT_FLAG_H