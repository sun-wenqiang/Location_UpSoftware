cmake_minimum_required(VERSION 3.20)

project(algorithm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)

# set(CMAKE_BUILD_TYPE Debug)

find_package(Eigen3 REQUIRED)

add_library(algorithm SHARED algorithm.cpp)
add_library(signalProcess SHARED signalProcess.cpp)

target_link_libraries(algorithm PRIVATE Eigen3::Eigen)
