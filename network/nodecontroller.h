#ifndef NODECONTROLLER_H
#define NODECONTROLLER_H
#include <iostream>
#include "TcpClient.h"

class NodeController: public QObject
{
    Q_OBJECT
public:
    NodeController(TcpClient * client):m_client(client) {};
    bool connectToServer();
    bool disconnectFromServer();
    bool sendMessage(uint8_t cmd_id, const std::vector<uint8_t>& cmd_data);
    void setStatus(ClientState new_state);
    ClientState getStatus();
// signals:
//     void connectionError();
//     void waitingResponnseTimeout();

// public slots:
//     void handleConnectError();
    // void handleWaitingResponseTimeout();

private:
    TcpClient * m_client;
};

#endif // NODECONTROLLER_H
