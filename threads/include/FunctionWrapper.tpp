// TODO: We sometimes send packaged_task. Have to erase its type
// TODO: Problems on std::future of that packaged task? CHECKED: NO! Shared state is preserved
template <typename F>
FunctionWrapper::FunctionWrapper(F&& f) : impl(new ImplType<std::decay_t<F>>(std::move(f))) {}
