#ifndef JOIN_THREADS_H
#define JOIN_THREADS_H

#include <thread>
#include <vector>

template <typename Thread>
class JoinThreads {
 public:
  JoinThreads(std::vector<Thread>& threads_);
  ~JoinThreads();

 private:
  std::vector<Thread>& threads;
};

#include "JoinThreads.tpp"

#endif // JOIN_THREADS_H