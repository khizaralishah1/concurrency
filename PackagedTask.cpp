#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <future>
#include <deque>

std::mutex mtx;
void ProcessMessage() {}
bool Shutdown() { return true; }

// 1. All the tasks that will be run by the thread
std::deque<std::packaged_task<void()>> tasks;

// 2. Thread function called by std::thread
void BackgroundThread() {
  while (!Shutdown()) {
    ProcessMessage();

    // 3. Look for pending task from tasks
    std::packaged_task<void()> task;
    {
      std::unique_lock<std::mutex> lock(mtx);
      if (tasks.empty()) continue;
      task = std::move(tasks.front());
      tasks.pop_front();
    }

    // 4. Run that task
    task();
  }
}

// 5. In another world, you are adding new tasks
template<typename Callable>
std::future<void> AddNewTask(Callable callable) {
  std::packaged_task<void()> task(callable);
  std::future<void> result = task.get_future();
  std::unique_lock<std::mutex> lock(mtx);
  tasks.push_back(std::move(task));
  return result;
}

void Add() {

}

int main() {
  std::thread background_thread(BackgroundThread);

  while(1) {
    AddNewTask(Add);
  }

  return EXIT_SUCCESS;
}