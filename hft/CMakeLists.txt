find_package(Boost REQUIRED COMPONENTS system thread)
find_package(OpenSSL REQUIRED)

add_executable(hft Main.cpp Socket.cpp)

target_link_libraries(hft Boost::system Boost::thread OpenSSL::SSL OpenSSL::Crypto pthread)