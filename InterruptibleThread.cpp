#include <thread>
#include <future>
#include <condition_variable>

class InterruptFlag {
 public:
  InterruptFlag() : thread_cond(nullptr), thread_cond_any(nullptr) {}

  void Set() {
    flag.store(true, std::memory_order_relaxed);
    std::unique_lock<std::mutex> lock(set_clear_mutex);
    if (thread_cond) {
      thread_cond->notify_all();
    } else if (thread_cond_any) {
      thread_cond_any->notify_all();
    }
  }

  bool IsSet() const { return flag.load(std::memory_order_relaxed); }

  void SetCV(std::condition_variable& cv) {
    std::unique_lock<std::mutex> lock(set_clear_mutex);
    thread_cond = &cv;
  }

  void ClearCV() {
    std::unique_lock<std::mutex> lock(set_clear_mutex);
    thread_cond = nullptr;
  }

  struct ClearCVOnDestruct {
    ~ClearCVOnDestruct() { this_thread_interrupt_flag.ClearCV(); }
  };

  template <typename Lockable, typename Predicate>
  void Wait(std::condition_variable_any& cv, Lockable& lock, Predicate predicate) {
    struct CustomLock {
      InterruptFlag* self;
      Lockable& lock;

      CustomLock(InterruptFlag* self_, std::condition_variable_any& cv, Lockable& lock_)
          : self(self_), lock(lock_) {
        self->set_clear_mutex.lock();  // 1
        self->thread_cond_any = &cv;   // 2
      }

      void Unlock() {  // 3
        lock.unlock();
        self->set_clear_mutex.unlock();
      }

      void Lock() {
        std::lock(self->set_clear_mutex, lock);  // 4
      }

      ~CustomLock() {
        self->thread_cond_any = nullptr;  // 5
        self->set_clear_mutex.unlock();
      }
    };

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
    cv.wait(custom_lock, predicate); // calls unlock (releases set_clear_mutex... Set can run)
    InterruptionPoint();
  }

 private:
  std::atomic<bool> flag;
  std::condition_variable* thread_cond;
  std::condition_variable_any* thread_cond_any;
  std::mutex set_clear_mutex;
};

thread_local InterruptFlag this_thread_interrupt_flag;

struct ThreadInterrupted : std::exception {
  const char* what() const noexcept override { return "Thread interrupted"; }
};

class InterruptibleThread {
 public:
  template <typename F>
  InterruptibleThread(F f) {
    std::promise<InterruptFlag*> p;
    internal_thread = std::thread([f, &p] {
      p.set_value(&this_thread_interrupt_flag);  // set value of promise to flag
      f();
    });

    // Guarantees that flag points to this_thread_interrupt_flag. Will only continue once promise
    // has set the value
    flag = p.get_future().get();
  }

  void Join();
  void Detach();
  bool Joinable() const;

  void Interrupt() {
    if (flag) flag->Set();
  }

 private:
  std::thread internal_thread;
  InterruptFlag* flag;
};

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

template <typename Predicate>
void InterruptibleWait(std::condition_variable& cv, Lockable& lock, Predicate predicate) {
  this_thread_interrupt_flag.Wait(cv, lock, predicate);
}

void InterruptionPoint() {
  if (this_thread_interrupt_flag.IsSet()) {
    // Throw thread interrupted, custom
    throw ThreadInterrupted{};
  }
}