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

  template <typename Lockable>
  void Wait(std::condition_variable_any& cv, Lockable& lock) {
    struct CustomLock {
      InterruptFlag* self;
      Lockable& lock;

      CustomLock(InterruptFlag* self_, std::condition_variable_any& cv, Lockable& lock_)
          : self(self_), lock(lock_) {
        self->set_clear_mutex.lock();
        self->thread_cond_any = &cv;
      }

      void Unlock() {
        lock.unlock();
        self->set_clear_mutex.unlock();
      }

      void Lock() { std::lock(self->set_clear_mutex, lock); }

      ~CustomLock() {
        self->thread_cond_any = nullptr;
        self->set_clear_mutex.unlock();
      }
    };

    CustomLock custom_lock(this, cv, lock);
    InterruptionPoint();
    cv.wait(custom_lock);
    InterruptionPoint();
  }

  template <typename Lockable>
  void InterruptibleWait(std::condition_variable& cv, Lockable& lock) {
    this_thread_interrupt_flag.Wait(cv, lock);
  }

  template <typename Predicate>
  void InterruptibleWait(std::condition_variable& cv, std::unique_lock<std::mutex>& lock,
                         Predicate predicate) {
    InterruptionPoint();
    this_thread_interrupt_flag.SetCV(v);
    interrupt_flag::ClearCVOnDestruct guard;
    while (!this_thread_interrupt_flag.IsSet() && !predicate()) {
      cv.wait_for(lock, std::chrono::milliseconds(1););
    }
    this_thread_interrupt_flag.ClearCV();
    InterruptionPoint();
  }

 private:
  std::atomic<bool> flag;
  std::condition_variable* thread_cond;
  std::condition_variable_any* thread_cond_any;
  std::mutex set_clear_mutex;
};

thread_local InterruptFlag this_thread_interrupt_flag;

class InterruptibleThread {
 public:
  template <typename F>
  InterruptibleThread(F f) {
    std::promise<InterruptFlag*> p;
    internal_thread = std::thread([f, &p] {
      p.set_value(&this_thread_interrupt_flag);  // set value of promise to flag
      f();
    });

    // Gurantess that flag points to this_thread_interrupt_flag. Will only continue once promise
    // has set the value
    flag = p.get_future().get();
  }

  void Join();
  void Detach();
  bool Joinable() const;

  void Interrupt() {
    if (flag) flag->Set();
  }

  void InterruptionPoint();

 private:
  std::thread internal_thread;
  InterruptFlag* flag;
};
