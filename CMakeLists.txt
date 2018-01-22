cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(elevator_simulator VERSION 0.0.1)

set(CMAKE_CXX_FLAGS "-Wall -std=c++14") 
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SOURCE_DIR}/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

add_custom_target(run
    COMMAND ${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)