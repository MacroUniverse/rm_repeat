# g++ -Wall -Wno-reorder -Wno-misleading-indentation -std=c++11 -O3 -fopenmp -D NDEBUG -fmax-errors=1 rm_repeat.cpp -o rm_repeat

cmake_minimum_required(VERSION 3.10)
project(rm_repeat)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -D NDEBUG")

add_executable(rm_repeat rm_repeat.cpp)

target_compile_options(rm_repeat PRIVATE
    -Wall
    -Wno-reorder
    -Wno-misleading-indentation
    -fopenmp
    -fmax-errors=1
)
