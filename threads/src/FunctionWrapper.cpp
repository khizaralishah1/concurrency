#include "FunctionWrapper.h"

FunctionWrapper::FunctionWrapper(FunctionWrapper&& other) : impl(std::move(other.impl)) {}

FunctionWrapper& FunctionWrapper::operator=(FunctionWrapper&& other) {
  impl = std::move(other.impl);
  return *this;
}

void FunctionWrapper::operator()() { impl->Call(); }