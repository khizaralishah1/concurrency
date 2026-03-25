#ifndef INTERRUPTIBLE_THREAD_H
#define INTERRUPTIBLE_THREAD_H

#include <future>

#include "InterruptFlag.h"

class InterruptibleThread {
 public:
  template <typename F, typename... Args>
  InterruptibleThread(F&& f, Args&&... args);

  // No copying
  InterruptibleThread(const InterruptibleThread&) = delete;
  InterruptibleThread& operator=(const InterruptibleThread&) = delete;

  // Allow move
  InterruptibleThread(InterruptibleThread&& other) noexcept;
  InterruptibleThread& operator=(InterruptibleThread&& other) noexcept;

  void join();
  bool joinable() const;
  void Detach();

  void Interrupt();

 private:
  std::thread internal_thread;
  InterruptFlag* flag;
};

#include "InterruptibleThread.tpp"

#endif  // INTERRUPTIBLE_THREAD_H