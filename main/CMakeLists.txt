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

set(EXT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext)
set(EXT_INSTALL_DIR ${CMAKE_SOURCE_DIR}/ext/bin)

ExternalProject_Add(ext_gtest
                    URL http://googletest.googlecode.com/files/gtest-1.7.0.zip
                    PREFIX ${EXT_SOURCE_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy libgtest.a ${EXT_INSTALL_DIR}/lib/libgtest.a &&
                                    ${CMAKE_COMMAND} -E copy libgtest_main.a ${EXT_INSTALL_DIR}/lib/libgtest_main.a &&
                                    ${CMAKE_COMMAND} -E copy_directory include/ ${EXT_INSTALL_DIR}/include/
)

ExternalProject_Add(ext_jsoncpp
                    URL https://github.com/open-source-parsers/jsoncpp/archive/0.9.4.tar.gz
                    PREFIX ${EXT_SOURCE_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_yamlcpp
                    URL https://yaml-cpp.googlecode.com/files/yaml-cpp-0.5.1.tar.gz
                    PREFIX ${EXT_SOURCE_DIR}
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_pugixml
                    URL https://github.com/zeux/pugixml/archive/v1.5.tar.gz
                    PREFIX ${EXT_SOURCE_DIR}
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR} scripts
                    BUILD_COMMAND make
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_libmicrohttpd
                    URL http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.39.tar.gz
                    PREFIX ${EXT_SOURCE_DIR}
                    CONFIGURE_COMMAND ./configure --prefix=${EXT_INSTALL_DIR}
                    BUILD_IN_SOURCE 1
)

ExternalProject_Add(ext_easyloggingpp
                    URL https://github.com/easylogging/easyloggingpp/releases/download/v9.80/easyloggingpp_v9.80.tar.gz
                    PREFIX ${EXT_SOURCE_DIR}
                    CONFIGURE_COMMAND ""
                    BUILD_IN_SOURCE 1
                    BUILD_COMMAND ""
                    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy easylogging++.h ${EXT_INSTALL_DIR}/include/easylogging++.h
)

add_custom_target(ext DEPENDS ext_gtest
                              ext_jsoncpp
                              ext_yamlcpp
                              ext_pugixml
                              ext_libmicrohttpd
                              ext_easyloggingpp)

#exclude dependencies from all
set_target_properties(ext_gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_jsoncpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_yamlcpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_pugixml PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_libmicrohttpd PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext_easyloggingpp PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(ext PROPERTIES EXCLUDE_FROM_ALL TRUE)

####################
# target
####################
#define binary output directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

#define compiler flags
set(DEFINITIONS "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFINITIONS}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#define source + headers
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

#define include directories
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/inc
                 ${EXT_INSTALL_DIR}/include
                 ${Qt5Core_INCLUDE_DIRS}
                 ${Qt5Network_INCLUDE_DIRS}
                 ${Qt5Script_INCLUDE_DIRS})

include_directories(${INCLUDE_DIRS})

#define libraries
set(LIBRARIES_DIR ${EXT_INSTALL_DIR}/lib)

set(LIBRARIES ${Qt5Core_LIBRARIES}
              ${Qt5Network_LIBRARIES}
              ${Qt5Script_LIBRARIES}
              pugixml
              jsoncpp
              yaml-cpp
              microhttpd)

link_directories(${LIBRARIES_DIR})

#create object files for main program
qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_library(${PROJECT_NAME}-obj OBJECT ${SOURCES}
                                       ${MOC_SOURCES})

#create main executable
add_executable(${PROJECT_NAME} src/main.cpp
                               $<TARGET_OBJECTS:${PROJECT_NAME}-obj>)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

####################
# install
####################
set(INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME})
set(PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/plugins)

#config for build tree
set(CONFIG_INCLUDE_DIRS ${INCLUDE_DIRS})

configure_file(${PROJECT_NAME}Config.cmake.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)

#config for install tree
set(CONFIG_INCLUDE_DIRS ${INSTALL_DIR}/include
                        ${Qt5Core_INCLUDE_DIRS}
                        ${Qt5Network_INCLUDE_DIRS}
                        ${Qt5Script_INCLUDE_DIRS})

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

################################
# test
################################
enable_testing()

set(TEST_LIBRARIES gtest
                   gtest_main
                   pthread)

#test value
add_executable(test_value test/test_value.cpp
                          $<TARGET_OBJECTS:${PROJECT_NAME}-obj>)

target_link_libraries(test_value ${TEST_LIBRARIES}
                                 ${LIBRARIES})

add_test(test_value ${EXECUTABLE_OUTPUT_PATH}/test_value)
