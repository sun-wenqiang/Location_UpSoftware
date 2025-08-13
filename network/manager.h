#ifndef MANAGER_H
#define MANAGER_H
#include <iostream>
#include "TcpClient.h"

class Manager: public QObject
{
    Q_OBJECT
public:
    Manager(QObject *parent = nullptr);
    ~Manager();
    bool connectToServer(uint8_t id);
    bool disconnectFromServer(uint8_t id);
    bool sendMessage(uintt_t id, uint8_t cmd_id, const std::vector<uint8_t>& cmd_data);
    ClientState getStatus(uint8_t id);
private:
    std::array<TcpClient *, 6> clients;
    std::array<QTimer *, 6> waitingResponseTimers;
};

#endif // NODECONTROLLER_H
