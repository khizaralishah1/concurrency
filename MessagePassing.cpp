#include <string>

struct CardInserted {
  std::string account;
};

class ATM {
public:
  messaging::receiver incoming;
  messaging::sender bank;
  messaging::sender interface_hardware;
  void (ATM::*state) ();

private:
  std::string account;
  std::string pin;
};