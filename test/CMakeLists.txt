cmake_minimum_required(VERSION 3.20)

project(test)

add_definitions(-DEBUG_MODE)

find_package(Eigen3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/../algorithm ${EIGEN3_INCLUDE_DIR})
file(GLOB SRC "${CMAKE_SOURCE_DIR}/../algorithm/*.cpp")

add_executable(app test.cpp ${SRC})

target_link_libraries(app PRIVATE Eigen3::Eigen)
