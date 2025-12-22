cmake_minimum_required(VERSION 3.14)
project(tsdb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For now, just a simple test that headers compile
add_executable(test_compile tests/test_compile.cpp)
target_include_directories(test_compile PRIVATE include)