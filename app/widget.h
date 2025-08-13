#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "TcpClient.h"
#include "manager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    int get_id();

private slots:
    void on_startButton_clicked();

    void on_powerButton_clicked();

    void on_gpsButton_clicked();

    void on_configButton_clicked();

    void on_energyButton_clicked();

    void on_energythresButton_clicked();

    void on_energyratioButton_clicked();

    void on_geoButton_clicked();

    void on_pulseButton_clicked();

    void on_rebootButton_clicked();

    void on_elseButton_clicked();

    void on_node0Button_clicked();

    void on_node1Button_clicked();

    void on_node2Button_clicked();

    void on_node3Button_clicked();

    void on_node4Button_clicked();

    void on_node5Button_clicked();

private:
    Ui::Widget *ui;
    Manager * manager;
};
#endif // WIDGET_H
