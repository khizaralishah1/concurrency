class Testing {
public:
  Testing();

  void Print();

  void SetData(int* p);

private:
  int data[16];
  std::string names;
  bool orders[100];
};