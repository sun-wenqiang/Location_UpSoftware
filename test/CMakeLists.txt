cmake_minimum_required(VERSION 3.20)

project(test)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3)
find_package(Qt5 COMPONENTS Core Network REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/../algorithm 
                    ${CMAKE_SOURCE_DIR}/../utils 
                    ${CMAKE_SOURCE_DIR}/../network
                    ${EIGEN3_INCLUDE_DIR})

file(GLOB SRC1 "${CMAKE_SOURCE_DIR}/../algorithm/*.cpp")
file(GLOB SRC2 "${CMAKE_SOURCE_DIR}/../utils/*.cpp")
file(GLOB SRC3 "${CMAKE_SOURCE_DIR}/../network/*.cpp")

add_executable(app test.cpp ${SRC1} ${SRC2} ${SRC3})

target_include_directories(app PRIVATE ${FFTW_INCLUDE_DIRS})
target_link_libraries(app 
                    Qt5::Core
                    Qt5::Network
                    ${FFTW_LIBRARIES}
                    Eigen3::Eigen Qt5::Network)

