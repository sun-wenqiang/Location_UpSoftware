#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{

}


void Widget::on_powerButton_clicked()
{

}


void Widget::on_gpsButton_clicked()
{

}


void Widget::on_configButton_clicked()
{

}


void Widget::on_energyButton_clicked()
{

}


void Widget::on_energythresButton_clicked()
{

}


void Widget::on_energyratioButton_clicked()
{

}


void Widget::on_geoButton_clicked()
{

}


void Widget::on_pulseButton_clicked()
{

}


void Widget::on_rebootButton_clicked()
{

}


void Widget::on_elseButton_clicked()
{

}


void Widget::on_node0Button_clicked()
{
    QString ip = "172.25.96.1";
    quint16 port = 5000;
    uint8_t id = 0;
    TcpClient *client = new TcpClient(ip, port, id, this);
    client->connectToServer();
    client->checkConnection();
    std::vector<uint8_t> cmd_data = {0xAA};
    client->sendCommand(CMD_COLLECT_CONTROL, cmd_data);
}

