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