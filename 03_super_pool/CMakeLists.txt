cmake_minimum_required(VERSION 3.1)
project(03_super_pool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cc)
add_executable(03_super_pool ${SOURCE_FILES})