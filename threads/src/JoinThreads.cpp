#include "JoinThreads.h"

template <typename Thread>
JoinThreads<Thread>::JoinThreads(std::vector<Thread>& threads_) : threads(threads_) {}

template <typename Thread>
JoinThreads<Thread>::~JoinThreads() {
  for (auto& thread : threads) {
    if (thread.joinable()) threads.join();
  }
}