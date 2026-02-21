#include <future>
#include <functions.hpp>

template <typename F, typename A>
using ResultType = typename std::invoke_result<F, A>::type;

template <typename F, typename A>
std::future<ResultType<F, A>> SpawnTask(F&& f, A&& a) {
  // 1. Create a packaged task using the function f
  std::packaged_task<ResultType<F, A>> task(std::move(f));

  // 2. Take the future of that packaged task
  std::future<ResultType<F, A>> result_future(task.get_future());

  // 3. Run the task asynchronously
  std::thread t(std::move(f), std::move(a));
  t.detach();

  // 4. Return the future
  return result_future;
}
