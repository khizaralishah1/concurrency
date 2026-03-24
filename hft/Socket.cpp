#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <iostream>
#include <string>

int main() {
  try {
    boost::asio::io_context io_context;

    // SSL context
    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
    ctx.set_default_verify_paths();
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
        websocket{io_context, ctx};

    boost::asio::ip::tcp::resolver resolver{io_context};
    auto const results = resolver.resolve("stream.binance.com", "9443");
    boost::asio::connect(websocket.next_layer().next_layer(), results.begin(), results.end());

    // SSL handshake
    websocket.next_layer().handshake(boost::asio::ssl::stream_base::client);

    websocket.handshake("stream.binance.com", "/ws/btcusdt@trade");

    while (true) {
      boost::beast::flat_buffer buffer;
      websocket.read(buffer);

      std::string msg = boost::beast::buffers_to_string(buffer.data());
      std::cout << msg << std::endl;
    }

  } catch (std::exception const& e) {
    std::cout << "[Main]: Error: " << e.what() << std::endl;
  }
}