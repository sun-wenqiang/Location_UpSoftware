cmake_minimum_required(VERSION 3.20)

project(Location_upSoftware)

set(CMAKE_CXX_STANDARD 17)
# 打开QT的元对象编译器自动（MOC）处理，槽机制需要MOC预处理头文件
set(CMAKE_AUTOMOC ON)
# 自动处理QT的UI界面   
# set(CMAKE_AUTOUIC ON) 
# 自动处理QT的资源文件（.qrc），包含图片、图标等等  
set(CMAKE_AUTORCC ON)   

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 1. 找到 Qt5 Widgets 模块
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# 2. 找到所有 UI 文件
file(GLOB UI_FILES ${CMAKE_SOURCE_DIR}/app/*.ui)

# 3. wrap 成 .h 文件（可见在 build/app 下）
qt5_wrap_ui(WRAP_FILES ${UI_FILES})

# 4. 找到源文件
file(GLOB SRC1 ${CMAKE_SOURCE_DIR}/app/*.cpp)
file(GLOB SRC2 ${CMAKE_SOURCE_DIR}/network/*.cpp)

# 5. 包含源码目录和构建目录（构建目录里会生成 ui_xxx.h）
include_directories(${CMAKE_SOURCE_DIR}/app
                    ${CMAKE_SOURCE_DIR}/build
                    ${CMAKE_SOURCE_DIR}/network)
            
# 6. 生成可执行文件，前面是文件名，后面是源文件                    
add_executable(Location_upSoftware main.cpp 
                ${SRC1} 
                ${WRAP_FILES}
                ${SRC2})  

# 7. 将需要的库链接到可执行文件上                
target_link_libraries(Location_upSoftware 
                    Qt5::Widgets
                    Qt5::Network)  
