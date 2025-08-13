#include "manager.h"


Manager::Manager(QObject *parent)   
{
    for (int i = 0; i < 6; i++)
    {
        clients[i] = new TcpClient(QString("192.168.1.%1").arg(160+i), 5000 + i, i, this);
        waitingResponseTimers[i] = new QTimer(this);
    }
}

Manager::~Manager()
{
    for (auto client:clients)
    {
        delete clients;
    }
}

bool Manager::connectToServer(uint8_t id)
{
    if (clients[id]->connectToServer())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Manager::disconnectFromServer(uint8_t id)
{
    if (clients[id]->disconnectFromServer())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Manager::sendMessage(uintt_t id, uint8_t cmd_id, const std::vector<uint8_t> &cmd_data)
{
    if (!clients[id])
    {
        qWarning()<< "Tcp client is not exist";
        return false;
    }
    if (clients[id]->sendCommand(cmd_id, cmd_data))
    {
        return true;
    }
    else
    {
        return false;
    }
}

ClientState Manager::getStatus(uint8_t id)
{
    return clients[id]->getStatus();
}



