cmake_minimum_required(VERSION 2.8.8)

project(hfsm-exec)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

#define version
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(${PROJECT_NAME}_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/inc PARENT_SCOPE)

####################
# target
####################
set(SOURCES src/logger.cpp
            src/application.cpp
            src/httpserver.cpp
            src/api.cpp
            src/statemachine.cpp
            src/builder.cpp
            src/parameter.cpp
            src/plugins.cpp)

set(HEADERS inc/logger.h
            inc/application.h
            inc/httpserver.h
            inc/api.h
            inc/statemachine.h
            inc/builder.h
            inc/parameter.h
            inc/plugins.h)

set(INCLUDE_DIRS inc
                 ext/easyloggingpp/inc
                 ${easyloggingpp_INCLUDE_DIR}
                 ${Qt5Core_INCLUDE_DIRS}
                 ${Qt5Network_INCLUDE_DIRS})

set(LIBRARIES ${Qt5Core_LIBRARIES}
              ${Qt5Network_LIBRARIES}
              pugixml
              jsoncpp
              yaml-cpp
              microhttpd)

include_directories(${INCLUDE_DIRS})

qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_executable(${PROJECT_NAME} ${SOURCES}
                               ${MOC_SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

####################
# export
####################
export(PACKAGE ${PROJECT_NAME})
