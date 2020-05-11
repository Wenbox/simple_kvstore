cmake_minimum_required(VERSION 3.7)
project(simple_kvstore)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package(Threads)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
include_directories(./libs/asio/include)
include_directories(src)


set(SERVER_SOURCE_FILES
        src/server.cpp  src/request_session.cpp )
set(CLIENT_SOURCE_FILES
        src/client.cpp src/run_client.cpp)
#configure_file(config.txt ${CMAKE_CURRENT_BINARY_DIR}/bin/config.txt COPYONLY)

add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})
#add_library()

add_subdirectory(tests)

#target_link_libraries(simple_kvstore_lib)