cmake_minimum_required(VERSION 3.20)

project(Location_upSoftware)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)   # 打开QT的元对象编译器自动（MOC）处理，槽机制需要MOC预处理头文件
set(CMAKE_AUTOUIC ON)   # 自动处理QT的UI界面
set(CMAKE_AUTORCC ON)   # 自动处理QT的资源文件（.qrc），包含图片、图标等等

find_package(Qt5 REQUIRED COMPONENTS Widgets)   # 查找QT库，且需要Widgets模块
file(GLOB SRC ${CMAKE_SOURCE_DIR}/app/*.cpp)
include_directories(${CMAKE_SOURCE_DIR}/app)

add_executable(Location_upSoftware main.cpp ${SRC})  # 生成可执行文件，前面是文件名，后面是源文件

target_link_libraries(Location_upSoftware Qt5::Widgets)  # 将Widgets库链接到可执行文件上
