class Testing {
 public:
  Testing();

  void Print();

  void SetData(int* p);

 private:
  int data[16];
  std::string names;
  bool orders[100];

  char padding[256 - 200];  // avoids false sharing
};

// 2nd example
struct ProtectedData {
  std::mutex mutex;
  char padding[1024];  // if a thread takes a mutex and tries to unlock it, it might fill up cache
  // line... causing other thread which already had the mutex...to update its
  // data that was just picked up to cache line by thread trying to lock mutex
  MyData some_data;
};