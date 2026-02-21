#include <iostream>
#include <future>  // std::future

// 4.2.1 Returning values from background tasks

// std::async starts an asynchronous task whose result isn't needed right away
// Return an std::future that will eventually hold a return value

int SomeAsynchronousStep() {
  int answer = 0;
  for (int i = 0; i < 100; ++i) {
    answer += i;
  }
  return answer;
}

void SomeOtherStep() { std::cout << "[SomeOtherStep]" << std::endl; }

int main() {
  std::future<int> result_of_this_function = std::async(SomeAsynchronousStep);
  SomeOtherStep();
  std::cout << "[Main] Answer is: " << result_of_this_function.get() << std::endl;

  return EXIT_SUCCESS;
}