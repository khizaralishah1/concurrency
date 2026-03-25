#include "InterruptFlag.h"

template <typename Lockable>
CustomLock<Lockable>::CustomLock(InterruptFlag* self_, std::condition_variable_any& cv,
                                 Lockable& lock_)
    : self(self_), lock(lock_) {
  self->set_clear_mutex.lock();  // 1
  self->thread_cond_any = &cv;   // 2
}

template <typename Lockable>
void CustomLock<Lockable>::Unlock() {  // 3
  lock.unlock();
  self->set_clear_mutex.unlock();
}

template <typename Lockable>
void CustomLock<Lockable>::Lock() {
  std::lock(self->set_clear_mutex, lock);  // 4
}

template <typename Lockable>
CustomLock<Lockable>::~CustomLock() {
  self->thread_cond_any = nullptr;  // 5
  self->set_clear_mutex.unlock();
}