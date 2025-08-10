#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)  // 参数是父窗口指针的构造函数
    : QWidget(parent)    // 调用父类的构造函数用来设置父对象
    , ui(new Ui::Widget)   // 初始化成员变量ui，是一个指向Ui::Widget实例的指针
{
    ui->setupUi(this);   // 将UI界面设置为当前的widget对象
}

Widget::~Widget()
{
    delete ui;    // 析构函数用于销毁当前的ui
}
