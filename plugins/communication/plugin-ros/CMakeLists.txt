cmake_minimum_required(VERSION 2.8.8)

project(plugin-ros)

find_package(Qt5 REQUIRED COMPONENTS Core Network Script)
find_package(hfsm-exec REQUIRED)

####################
# target
####################
set(SOURCES src/plugin_ros.cpp)

set(HEADERS inc/plugin_ros.h)

set(INCLUDE_DIRS inc
                 ${hfsm-exec_INCLUDE_DIR}
                 ${Qt5Core_INCLUDE_DIRS}
                 ${Qt5Network_INCLUDE_DIRS}
                 ${Qt5Script_INCLUDE_DIRS})

set(LIBRARIES ${Qt5Core_LIBRARIES}
              ${Qt5Network_LIBRARIES})


include_directories(${INCLUDE_DIRS})

qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

add_library(${PROJECT_NAME} SHARED ${SOURCES}
                                   ${MOC_SOURCES})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
