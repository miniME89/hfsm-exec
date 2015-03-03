cmake_minimum_required(VERSION 2.8.8)

project(hfsm-exec)

find_package(Qt5 REQUIRED COMPONENTS Core
                                     Network
                                     Script)

#define version
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})

####################
# dependencies
####################
include(ExternalProject)

set(EXTERNAL_BUILD_DIR ${PROJECT_BINARY_DIR})
set(EXTERNAL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})

ExternalProject_Add(ext_jsoncpp
                    URL https://github.com/open-source-parsers/jsoncpp/archive/0.9.0.tar.gz
                    PREFIX ${EXTERNAL_BUILD_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_yamlcpp
                    URL https://yaml-cpp.googlecode.com/files/yaml-cpp-0.5.1.tar.gz
                    PREFIX ${EXTERNAL_BUILD_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_pugixml
                    URL https://github.com/zeux/pugixml/archive/v1.5.tar.gz
                    PREFIX ${EXTERNAL_BUILD_DIR}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_DIR} scripts
                    BUILD_COMMAND make
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_libmicrohttpd
                    URL http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.39.tar.gz
                    PREFIX ${EXTERNAL_BUILD_DIR}
                    CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_easyloggingpp
                    URL https://github.com/easylogging/easyloggingpp/releases/download/v9.80/easyloggingpp_v9.80.tar.gz
                    PREFIX ${EXTERNAL_BUILD_DIR}
                    CONFIGURE_COMMAND ""
                    BUILD_IN_SOURCE 1
                    BUILD_COMMAND ""
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy easylogging++.h ${EXTERNAL_INSTALL_DIR}/include/easylogging++.h
)

add_custom_target(ext_install DEPENDS ext_jsoncpp
                                      ext_yamlcpp
                                      ext_pugixml
                                      ext_libmicrohttpd
                                      ext_easyloggingpp)

#exclude dependencies
set_target_properties(ext_jsoncpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_yamlcpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_pugixml PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_libmicrohttpd PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_easyloggingpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_install PROPERTIES EXCLUDE_FROM_ALL TRUE)

####################
# target
####################
#define binary output directory
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#define compiler flags
set(DEFINITIONS "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFINITIONS}")

set(SOURCES src/logger.cpp
            src/application.cpp
            src/httpserver.cpp
            src/api.cpp
            src/statemachine.cpp
            src/builder.cpp
            src/plugins.cpp
            src/value.cpp)

set(HEADERS inc/logger.h
            inc/application.h
            inc/httpserver.h
            inc/api.h
            inc/statemachine.h
            inc/builder.h
            inc/plugins.h
            inc/value.h)

#include directories
set(PROJECT_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/inc)

set(EXTERNAL_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS}
                          ${Qt5Network_INCLUDE_DIRS}
                          ${Qt5Script_INCLUDE_DIRS})

set(INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS}
                 ${EXTERNAL_INCLUDE_DIRS})

#libraries
set(LIBRARIES ${Qt5Core_LIBRARIES}
              ${Qt5Network_LIBRARIES}
              ${Qt5Script_LIBRARIES}
              pugixml
              jsoncpp
              yaml-cpp
              microhttpd)

link_directories(${LINK_DIRS})

include_directories(${INCLUDE_DIRS})

qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES}
                               ${MOC_SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

####################
# install
####################
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME})
set(PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/plugins)

#config for build tree
set(CONFIG_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS}
                        ${EXTERNAL_INCLUDE_DIRS})

configure_file(${PROJECT_NAME}Config.cmake.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)

#config for install tree
set(CONFIG_INCLUDE_DIRS ${INSTALL_DIR}/include
                        ${EXTERNAL_INCLUDE_DIRS})

configure_file(${PROJECT_NAME}Config.cmake.in
               ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake)

install(FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake
        DESTINATION ${INSTALL_DIR}/cmake
        COMPONENT dev)

#install targets
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${INSTALL_DIR})

#install headers
install(FILES ${HEADERS}
        DESTINATION ${INSTALL_DIR}/include)

#export
export(PACKAGE ${PROJECT_NAME})
