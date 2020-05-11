cmake_minimum_required(VERSION 3.7)
project(tests)

set(CMAKE_CXX_STANDARD 11)

include_directories(./libs/asio-1.16.1/include)

add_executable(scenario1 scenario1.cpp ../src/client.cpp)
add_test(NAME sc1 COMMAND scenario1)

