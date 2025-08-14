#include "manager.h"
#include "algorithm.h"

Manager::Manager(QObject *parent)   
{
    for (int i = 0; i < 6; i++)
    {
        clients[i] = new TcpClient(QString("192.168.1.%1").arg(160+i), 5000 + i, i, this);
        waitingResponseTimers[i] = new QTimer(this);
        connect(clients[i], &TcpClient::receiveArrivalTime, this, &Manager::handleArrivalTime);
    }
}

Manager::~Manager()
{
}

void Manager::handleArrivalTime(const Info_ArrivalTime& arrival_info)
{
    Time time_temp = arrival_info.time;
    QString key = QString("%1_%2_%3").arg(time_temp.hour, time_temp.minute, time_temp.second);
    if (ArrivalTime_dict.count(key) > 0)
    {
        ArrivalTime_dict[key].push_back(arrival_info);  // 键存在，添加到达时间信息
        // 去查询key的前一个键来定位
        auto It = ArrivalTime_dict.find(key);  // 迭代器
        if (It == ArrivalTime_dict.begin())
        {
            return;   // 只有一个到达时间vector
        }
        auto prevIt = std::prev(It);
        std::vector<Info_ArrivalTime>& prev_arrivalTime_vector = prevIt->second;
        
        if (prev_arrivalTime_vector.size()<3)
        {
            return;    // 不足三个节点
        }
        
        size_t node_count = prev_arrivalTime_vector.size();
        Info_ArrivalTime ref_node = prev_arrivalTime_vector[0];
        GeoCoord geo_ref = ref_node.corrd;
        std::vector<point2D> nodes_pos(node_count);
        std::vector<double> delay(node_count);
        for (int i = 0; i < prev_arrivalTime_vector.size(); i++)
        {
            Info_ArrivalTime current_node = prev_arrivalTime_vector[i];
            GeoCoord geo = current_node.corrd;
            EnuCoord enu_temp = Geo2Enu(geo, geo_ref);
            nodes_pos[i] = {enu_temp.x, enu_temp.y};
            delay[i] = current_node.offset - ref_node.offset;
        }
        std::vector<double> source_position;   //解算结果
        if (node_count == 3)
        {
            source_position = solveThreeNodes(nodes_pos, delay);
        }
        else
        {
            source_position = solveMultiNodes(nodes_pos, delay);
        }
        sourcePosition source_result;
        source_result.geo = Enu2Geo({source_position[0], source_position[1], geo_ref.altitude}, geo_ref);
        source_result.enu.x = source_position[0];
        source_result.enu.y = source_position[1];
        emit obtainResult(source_result);
    }
    else
    {
        ArrivalTime_dict[key] = std::vector<Info_ArrivalTime>{arrival_info};
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

bool Manager::sendMessage(uint8_t id, uint8_t cmd_id, const std::vector<uint8_t> &cmd_data)
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



