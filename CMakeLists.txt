cmake_minimum_required(VERSION 3.20)

project(Hello)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)   # 打开QT的元对象编译器自动（MOC）处理，槽机制需要MOC预处理头文件
set(CMAKE_AUTOUIC ON)   # 自动处理QT的UI界面
set(CMAKE_AUTORCC ON)   # 自动处理QT的资源文件（.qrc），包含图片、图标等等

find_package(Qt5 REQUIRED COMPONENTS Widgets)   # 查找QT库，且需要Widgets模块

add_executable(HelloApp main.cpp)  # 生成可执行文件，前面是文件名，后面是源文件

target_link_libraries(HelloApp Qt5::Widgets)  # 将Widgets库链接到我的可执行文件上
