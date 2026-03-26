template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args...>> ThreadPool::Submit(F&& f, Args&&... args) {
  using result_type = std::invoke_result_t<F, Args...>;

  auto task = std::packaged_task<result_type()>(
      [func = std::forward<F>(f), ... captured_args = std::forward<Args>(args)]() mutable {
        return std::invoke(std::move(func), std::move(captured_args)...);
      });

  auto result = task.get_future();

  std::cout << "[ThreadPool::Submit]: Task added to global queue" << std::endl;
  global_work_queue.Push(std::move(task));

  return result;
}

template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args...>> ThreadPool::SubmitToThread(unsigned int thread_id,
                                                                         F&& f, Args&&... args) {
  using result_type = std::invoke_result_t<F, Args...>;

  auto task = std::packaged_task<result_type()>(
      [func = std::forward<F>(f), ... captured_args = std::forward<Args>(args)]() mutable {
        return std::invoke(std::move(func), std::move(captured_args)...);
      });

  auto result = task.get_future();

  if (thread_id < std::thread::hardware_concurrency()) {
    if (queues[thread_id].get()) {
      std::cout << "[ThreadPool::SubmitToThread]: " << thread_id << ". Task added to local queue"
                << std::endl;
      queues[thread_id].get()->Push(std::move(task));
    } else {
      std::cout << "[ThreadPool::SubmitToThread]: " << thread_id
                << ". Task NOT added to local queue. Queue not available!" << std::endl;
    }
  }

  return result;
}