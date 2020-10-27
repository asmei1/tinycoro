cmake_minimum_required(VERSION 3.18)
project (TinyCoro LANGUAGES CXX)
#set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_FLAGS "-fcoroutines -pthread")

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(include/tinycoro)
add_subdirectory(tests)

