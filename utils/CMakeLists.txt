cmake_minimum_required(VERSION 3.20)

project(CoordConverter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)

# set(CMAKE_BUILD_TYPE Debug)


add_library(CoordConverter SHARED CoordConverter.cpp)
