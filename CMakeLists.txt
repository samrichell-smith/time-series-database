cmake_minimum_required(VERSION 3.14)
project(tsdb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tsdb 
    src/series.cpp
    src/database.cpp
)
target_include_directories(tsdb PUBLIC include)

add_executable(test_compile tests/test_compile.cpp)
target_link_libraries(test_compile PRIVATE tsdb)