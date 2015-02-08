cmake_minimum_required(VERSION 2.8.8)

project(root)

include(ExternalProject)

#define compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

#output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)

#dependency
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

add_subdirectory(main)
add_subdirectory(plugins/communication)
add_subdirectory(plugins/importer)
add_subdirectory(plugins/exporter)
