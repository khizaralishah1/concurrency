
template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args...>> ThreadPool::Submit(F&& f, Args&&... args) {
  using result_type = std::invoke_result_t<F, Args...>;

  auto task = std::packaged_task<result_type()>(
      [func = std::forward<F>(f), ... captured_args = std::forward<Args>(args)]() mutable {
        return std::invoke(std::move(func), std::move(captured_args)...);
      });

  auto result = task.get_future();

  if (local_queue) {
    std::cout << "[ThreadPool::Submit]: Task added to local queue" << std::endl;
    local_queue->Push(std::move(task));
  } else {
    std::cout << "[ThreadPool::Submit]: Task added to global queue" << std::endl;
    global_work_queue.Push(std::move(task));
  }

  return result;
}