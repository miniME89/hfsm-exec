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

ExternalProject_Add(googletest
                    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
                    INSTALL_COMMAND ""
                    LOG_DOWNLOAD ON
                    LOG_CONFIGURE ON
                    LOG_BUILD ON)

ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)

add_subdirectory(main)
add_subdirectory(plugins/communication)
add_subdirectory(plugins/decoder)
