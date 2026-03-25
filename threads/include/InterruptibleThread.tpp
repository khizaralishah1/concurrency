template <typename F, typename... Args>
InterruptibleThread::InterruptibleThread(F&& f, Args&&... args) {
  std::promise<InterruptFlag*> p;
  // Forward the external f and args properly (rvalues are forwarded using std::foward)
  internal_thread = std::thread(
      [func = std::forward<F>(f), ... CapturedArgs = std::forward<Args>(args), &p]() mutable {
        p.set_value(&this_thread_interrupt_flag);

        // Call function with args
        std::invoke(std::move(func), std::move(CapturedArgs)...);
      });

  // Guarantees that flag points to this_thread_interrupt_flag. Will only continue once promise
  // has set the value
  flag = p.get_future().get();
}