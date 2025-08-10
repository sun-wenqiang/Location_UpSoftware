cmake_minimum_required(VERSION 3.20)

project(protocol)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../lib)

# set(CMAKE_BUILD_TYPE Debug)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_library(protocol SHARED protocol.cpp)

target_link_libraries(protocol 
                    Qt5::Network)  