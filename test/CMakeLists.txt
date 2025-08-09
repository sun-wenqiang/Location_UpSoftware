cmake_minimum_required(VERSION 3.20)

project(test)

set(CMAKE_BUILD_TYPE Debug)

find_package(Eigen3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/../algorithm 
                    ${CMAKE_SOURCE_DIR}/../utils 
                    ${EIGEN3_INCLUDE_DIR})

file(GLOB SRC1 "${CMAKE_SOURCE_DIR}/../algorithm/*.cpp")
file(GLOB SRC2 "${CMAKE_SOURCE_DIR}/../utils/*.cpp")

add_executable(app test.cpp ${SRC1} ${SRC2})

target_link_libraries(app PRIVATE Eigen3::Eigen)

target_compile_definitions(app PRIVATE DEBUG_MODE)

