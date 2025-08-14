#include "widget.h"
#include "ui_widget.h"
#include "TcpClient.h"
#include <QPushButton>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget), manager(new Manager)
{
    ui->setupUi(this);

    for (int i = 0; i < 6; i++)
    {
        QPushButton* bnt = nullptr;
        QLabel* label = nullptr;
        switch(i)
        {
            case 0: bnt = ui->node0Button; label = ui->node0Label; break;
            case 1: bnt = ui->node1Button; label = ui->node1Label; break;
            case 2: bnt = ui->node2Button; label = ui->node2Label; break;
            case 3: bnt = ui->node3Button; label = ui->node3Label; break;
            case 4: bnt = ui->node4Button; label = ui->node4Label; break;
            case 5: bnt = ui->node5Button; label = ui->node5Label; break;
        }

        connect(bnt, &QPushButton::clicked, this, [this, bnt, i](){
            if (manager->clients[i]->getConnectStatus())
            {
                bool result = manager->clients[i]->disconnectFromServer();
                if (result)
                {
                    bnt->setText("连接");
                    bnt->setStyleSheet("background-color: white;");
                    printConnect(QString("与节点%1断开连接成功").arg(i));
                }
                else
                {
                    bnt->setText("已连接");
                    bnt->setStyleSheet("background-color: green;");
                    printConnect(QString("与节点%1断开连接失败").arg(i));
                }
            }
            else
            {
                bool result = manager->clients[i]->connectToServer();
                if (result)
                {
                    bnt->setText("已连接");
                    bnt->setStyleSheet("background-color: green;");
                    printConnect(QString("与节点%1建立连接成功").arg(i));
                }
                else
                {
                    bnt->setText("连接");
                    bnt->setStyleSheet("background-color: white;");
                    printConnect(QString("与节点%1建立连接失败").arg(i));
                }
            }
        });
    }

    connect(manager, &Manager::obtainResult, this, &Widget::handleObtainResult);
}

Widget::~Widget()
{
    delete ui;
    delete manager;
}

void Widget::handleObtainResult(const sourcePosition& ans)
{
    printLog(QString("x: %1, y: %2").arg(ans.enu.x, ans.enu.y));
    printTrack(QString("x: %1, y: %2, longitude: %3, latitude: %4").arg(ans.enu.x, ans.enu.y, ans.geo.longitude, ans.geo.latitude));
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

void Widget::printLog(const QString& log)
{
    ui->logText->append(log);
}

void Widget::printConnect(const QString& connect_log)
{
    ui->connectText->append(connect_log);
}

void Widget::printTrack(const QString& track_log)
{
    ui->trackText->append(track_log);
}
