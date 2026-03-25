#include <thread>
#include <future>
#include <condition_variable>
#include <functional>
#include <mutex>

#include "InterruptibleThread.h"

thread_local InterruptFlag this_thread_interrupt_flag;

void InterruptibleThread::Interrupt() {
  if (flag) flag->Set();
}

void InterruptibleThread::join() { internal_thread.join(); }

bool InterruptibleThread::joinable() const { return internal_thread.joinable(); }

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
