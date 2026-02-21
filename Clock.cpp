#include <chrono>
#include <future>
using namespace std::chrono;

void SomeTask() {
  
}

std::future<int> f = std::async(SomeTask);

std::future_status status = f.wait_for(milliseconds(35));

if (status == std::future_status::ready) {
  DoSomething(f.get());
} else if (status == std::future_status::timeout) {
} else if (status == std::future_status::deferred) {
}
