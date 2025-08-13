#include "widget.h"
#include "ui_widget.h"
#include "TcpClient.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget), manager(new Manager)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
    delete manager;
}

int Widget::get_id()
{
    int id = ui->nodeIDBox->value();
    return id;
}

void Widget::on_startButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0XAA};
    manager->clients[id]->sendCommand(CMD_COLLECT_CONTROL, cmd_data);
}


void Widget::on_powerButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_GET_POWER, cmd_data);
}


void Widget::on_gpsButton_clicked()   // 查询GPS有效性
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_QUERY_GPS_STATUS, cmd_data);
}


void Widget::on_configButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_GET_MULTI_PARAMS, cmd_data);
}


void Widget::on_energyButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_GET_ENERGY, cmd_data);
}


void Widget::on_energythresButton_clicked()
{

}


void Widget::on_energyratioButton_clicked()
{

}


void Widget::on_geoButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_GET_COORDINATES, cmd_data);
}


void Widget::on_pulseButton_clicked()
{

}


void Widget::on_rebootButton_clicked()
{
    int id = get_id();
    std::vector<uint8_t> cmd_data = {0X00};
    manager->clients[id]->sendCommand(CMD_REBOOT, cmd_data);
}


void Widget::on_elseButton_clicked()
{
    
}


void Widget::on_node0Button_clicked()
{
    bool connedted = manager->clients[0]->connectToServer();
}

void Widget::on_node1Button_clicked()
{
    bool connedted = manager->clients[1]->connectToServer();
}

void Widget::on_node2Button_clicked()
{
    bool connedted = manager->clients[2]->connectToServer();
}

void Widget::on_node3Button_clicked()
{
    bool connedted = manager->clients[3]->connectToServer();
}

void Widget::on_node4Button_clicked()
{
    bool connedted = manager->clients[4]->connectToServer();
}

void Widget::on_node5Button_clicked()
{
    bool connedted = manager->clients[5]->connectToServer();
}