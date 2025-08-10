#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget  // 定义Widget类，继承自QWidget
{
    Q_OBJECT    // Qt的宏，用于启用信号槽机制和元对象系统

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void on_startButton_clicked();  


private:
    Ui::Widget *ui;   // 指向UI界面的指针，访问界面元素
};
#endif // WIDGET_H
