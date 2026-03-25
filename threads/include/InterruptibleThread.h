#include <future>

#include "InterruptFlag.h"

struct ThreadInterrupted : std::exception {
  const char* what() const noexcept override { return "Thread interrupted"; }
};

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
  void Detach();
  bool Joinable() const;

  void Interrupt();

 private:
  std::thread internal_thread;
  InterruptFlag* flag;
};

void InterruptionPoint();