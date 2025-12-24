cmake_minimum_required(VERSION 3.14)
project(tsdb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tsdb 
    src/series.cpp
    src/database.cpp
    src/encoding.cpp
)
target_include_directories(tsdb PUBLIC include)

add_executable(demo examples/demo.cpp)
target_link_libraries(demo PRIVATE tsdb)

add_subdirectory(bench)
add_subdirectory(tests)