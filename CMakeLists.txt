cmake_minimum_required(VERSION 3.21)
project(test_task)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

add_subdirectory(lib)
include_directories(lib/googletest/include)


set(CMAKE_CXX_STANDARD 17)

add_executable(test_task tests.cpp RouteManager.cpp)

target_link_libraries(test_task gtest gtest_main)

