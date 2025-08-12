#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);   // QT应用对象
    Widget w;   // 顶级窗口
    w.show();   // 设为可见
    return a.exec();   // 运行程序（主线程），w开始响应用户操作
}
