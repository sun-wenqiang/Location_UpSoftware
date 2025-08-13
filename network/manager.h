#ifndef MANAGER_H
#define MANAGER_H
#include <iostream>
#include "TcpClient.h"
#include <map>
#include <QString>
#include "algorithm.h"
#include "CoordConverter.h"

struct sourcePosition
{
    point2D enu;
    GeoCoord geo;
}

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

public slots:
    void handleArrivalTime(const Info_ArrivalTime& arrival_info);

signals:
    void obtainResult(const sourcePosition& ans);


private:
    std::array<TcpClient *, 6> clients;
    std::array<QTimer *, 6> waitingResponseTimers;
    std::map<QString, std::vector<Info_ArrivalTime>> ArrivalTime_dict;
    std::unordered_set<QString> used_timeArrival;   // 维护使用过的到达时间
};

#endif // NODECONTROLLER_H
