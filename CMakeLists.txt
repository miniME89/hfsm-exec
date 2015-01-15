cmake_minimum_required(VERSION 2.8.8)

project(root)

#define compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

#output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(main)
add_subdirectory(plugins)
