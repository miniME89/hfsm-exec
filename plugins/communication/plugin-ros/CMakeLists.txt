cmake_minimum_required(VERSION 2.8.8)

project(plugin-ros)

find_package(Qt5 REQUIRED COMPONENTS Core Network)
find_package(hfsm-exec REQUIRED)

#define compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${hfsm-exec_DEFINITIONS}")

####################
# target
####################
set(SOURCES src/plugin_ros.cpp)

set(HEADERS inc/plugin_ros.h)

set(INCLUDE_DIRS inc
                 ${hfsm-exec_INCLUDE_DIRS})

set(LIBRARIES ${Qt5Core_LIBRARIES}
			  ${Qt5Network_LIBRARIES})

include_directories(${INCLUDE_DIRS})

qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_library(${PROJECT_NAME} SHARED ${SOURCES}
                                   ${MOC_SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

####################
# install
####################
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${hfsm-exec_PLUGIN_DIR})
