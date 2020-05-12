cmake_minimum_required(VERSION 3.7)
project(tests)

set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_SOURCE_DIR}/libs/asio/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
FOREACH(dir ${dirs})
    IF(NOT EXISTS ${dir})
        message(SEND_ERROR "directory '" ${dir} "' does not exist.")
    ENDIF(NOT EXISTS ${dir})
ENDFOREACH(dir)

add_executable(scenario1 scenario1.cpp ../src/client.cpp ../src/kv_store_config.cpp)
add_executable(scenario2 scenario2.cpp ../src/client.cpp ../src/kv_store_config.cpp)
add_executable(scenario3 scenario3.cpp ../src/client.cpp ../src/kv_store_config.cpp)

add_test(NAME sc1 COMMAND scenario1)

