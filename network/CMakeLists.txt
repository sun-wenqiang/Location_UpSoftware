cmake_minimum_required(VERSION 3.20)

project(TcpClient)

set(CMAKE_CXX_STANDARD 17)
# 打开QT的元对象编译器自动（MOC）处理，槽机制需要MOC预处理头文件
set(CMAKE_AUTOMOC ON)
# 自动处理QT的UI界面   
# set(CMAKE_AUTOUIC ON) 
# 自动处理QT的资源文件（.qrc），包含图片、图标等等  
set(CMAKE_AUTORCC ON)   


# 1. 找到 Qt5 模块
find_package(Qt5 COMPONENTS Network REQUIRED)
            
# 6. 生成可执行文件，前面是文件名，后面是源文件                    
add_library(TcpClient SHARED TcpClient.cpp) 

# 7. 将需要的库链接到可执行文件上                
target_link_libraries(TcpClient 
                    Qt5::Network)  
