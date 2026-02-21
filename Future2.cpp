#include <string>
#include <future>

struct Structure {
  void Foo(int, std::string const&);
  std::string Bar(std::string const&);
};

struct Callable {
  double operator()(double);
};

class MoveOnly {
 public:
  MoveOnly() = default;
  MoveOnly(MoveOnly&&) = default;

  // Cannot construct from l-value
  MoveOnly(MoveOnly const&) = delete;
  MoveOnly& operator=(MoveOnly const&) = delete;

  // Can construct from temporary
  MoveOnly& operator=(MoveOnly&&) = default;

  void operator()();
};

int main() {
  // 1. Member functions of an object
  Structure structure;

  // structure->Foo()
  auto result_of_foo = std::async(Structure::Foo, &structure, 10, "A string");

  // structure.Bar()
  auto result_of_bar = std::async(Structure::Bar, structure, "Hello");

  // 2. Callable object

  // Makes an object that is move-constructured from Callable, and calls its ()
  auto result_of_callable_tmp_obj = std::async(Callable(), 1.234);

  // Calls the () of the object callable
  Callable callable_obj;
  auto result_of_callable_obj = std::async(std::ref(callable_obj), 2.1);

  // 3. MoveOnly class
  auto result_of_move_only = std::async(MoveOnly());

  // 4. Two ways of launching async
  // a. In a different thread - true async
  std::future result_from_diff_thread = std::async(std::launch::async, MoveOnly());

  // b. Only when .get() is used - launch is deffered until then
  std::future result_deferred = std::async(std::launch::deferred, MoveOnly());

  // Implementation chooses
  std::future result_any =
      std::async(std::launch::async | std::launch::deferred, std::ref(callable_obj), 1.0);

  result_deferred.get();  // This is where 4b gets called

  return EXIT_SUCCESS;
}