#include <memory>

struct ImplBase {
  virtual void Call() = 0;
  virtual ~ImplBase() {}
};

template <typename F>
struct ImplType : ImplBase {
  F f;
  ImplType(F&& f_) : f(std::move(f_)) {}
  void Call() { f(); }
};

class FunctionWrapper {
 public:
  template <typename F>
  FunctionWrapper(F&& f);
  FunctionWrapper() = default;
  FunctionWrapper(FunctionWrapper&& other);
  FunctionWrapper(const FunctionWrapper&) = delete;
  FunctionWrapper(FunctionWrapper&) = delete;
  FunctionWrapper& operator=(const FunctionWrapper&) = delete;
  FunctionWrapper& operator=(FunctionWrapper&& other);

  void operator()() { impl->Call(); }

 private:
  std::unique_ptr<ImplBase> impl;
};