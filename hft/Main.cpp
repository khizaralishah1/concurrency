// #include <vector>
// #include <thread>
// #include <chrono>
// #include <iostream>
// #include <random>

// double GetPrice() {
//   static std::default_random_engine eng(std::random_device{}());
//   static std::normal_distribution<double> dist(100.0, 0.5);  // mean, std_dev
//   return dist(eng);
// }

// double MovingAverage(const std::vector<double>& prices) {
//   double sum = std::accumulate(prices.begin(), prices.end(), 0.0);
//   return sum / prices.size();
// }

// int GenerateSignal(const std::vector<double>& prices) {
//   double price = prices.back();
//   double moving_avg = MovingAverage(prices);
//   if (price < moving_avg) return 1;
//   if (price > moving_avg) return -1;
//   return 0;
// }

// void SendOrder(int signal, double price) {
//   if (signal == 1) {
//     std::cout << "[SendOrder]: Buy at " << price << "\r\n";
//   } else if (signal == -1) {
//     std::cout << "[SendOrder]: Sell at " << price << "\r\n";
//   }
// }

// int main() {
//   std::vector<double> prices;

//   while (true) {
//     double price = GetPrice();
//     prices.push_back(price);

//     if (prices.size() > 50) prices.erase(prices.begin());  // Erases first element

//     if (prices.size() >= 10) {
//       int signal = GenerateSignal(prices);
//       SendOrder(signal, price);
//     }

//     std::this_thread::sleep_for(std::chrono::milliseconds(10));
//   }

//   return 0;
// }