cmake_minimum_required(VERSION 2.8.8)

project(plugins)

#library output directory
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/plugins)

add_subdirectory(communication)
add_subdirectory(importer)
add_subdirectory(exporter)
