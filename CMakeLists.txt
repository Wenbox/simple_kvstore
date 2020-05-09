cmake_minimum_required(VERSION 3.7)
project(simple_kvstore)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(Threads)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#find_package(Boost 1.64.0 COMPONENTS system serialization REQUIRED)
include_directories(include)
include_directories(./libs/asio-1.16.1/include)



set(SERVER_SOURCE_FILES
        src/server.cpp
        src/server.cpp src/message.cpp )
set(CLIENT_SOURCE_FILES
        src/client.cpp src/message.cpp)
#configure_file(config.txt ${CMAKE_CURRENT_BINARY_DIR}/bin/config.txt COPYONLY)

add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})
#add_library()

#add_subdirectory(tests)

#target_link_libraries(simple_kvstore_lib)