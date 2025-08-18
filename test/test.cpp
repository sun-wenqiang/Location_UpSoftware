#include "../algorithm/algorithm.h"
#include "../utils/CoordConverter.h"
#include <iostream>
#include "test.h"

int main() {
    /* 
    point2D node1 = {0, 0};
    point2D node2 = {-1.8343242536022,         0.891888149455403};
    point2D node3 = {0.324825134121168,        1.29820720407733};
    point2D node4 = {0.530398528893644,        2.03575598726156};

    std::vector<point2D> nodes_pos = {node1, node2, node3, node4};
    std::vector<double> delay = {0,  0.00126820877581185, 0.000142706262359215
    , 0.00051631964970499};

    std::vector<double> ans(2);
    ans = solveMultiNodes(nodes_pos, delay);
    if (ans.empty())
    {
        std::cerr<<"解算失败"<<std::endl;
    }
    else
    {
        std::cout<<"解算结果"<<"x = "<<ans[0]<<", y = "<<ans[1]<<std::endl;
    }

    GeoCoord ref, geo;
    ref.altitude = 528.553282;
    ref.latitude = 0.603102437039617;
    ref.longitude = 1.90024091620209;

    geo.altitude = 545.651561;
    geo.latitude = 0.603108155105235;
    geo.longitude = 1.90022986819938;

    EnuCoord enu = Geo2Enu(geo, ref);  // WGS84转化为局部东北天坐标，需要以ref为参考
    std::cout<<"转换结果"<<"x = "<<enu.x<<", y = "<<enu.y<<", z = "<<enu.z<<std::endl;
    // EcefCoord Geo2Ecef(GeoCoord geo);   // WGS84转化为地心地固坐标系
    // EnuCoord Ecef2Enu(EcefCoord ecef, GeoCoord ref);  // ECEF转化为局部的直角坐标系，需要以ref节点为参考

    GeoCoord reverse = Enu2Geo(enu, ref);  // 将局部东北天坐标转化为WGS84，需要以ref为参考
    // EcefCoord Enu2Ecef(EnuCoord enu, GeoCoord ref);  // 将局部的东北天坐标转化为ECEF坐标，需要以ref为参考
    // GeoCoord Ecef2Geo(EcefCoord ecef);    // 将ECEF坐标转化为经纬度

    std::cout<<"转换回来"<<"longitude = "<<reverse.longitude<<", latitude = "<<reverse.latitude<<", altitude = "<<reverse.altitude<<std::endl;
    */

    QString ip = "192.168.1.100";
    quint16 port = 8000;
    uint8_t id = 0;
    TcpClient client = TcpClient(ip, port, id);
    client.connectToServer();
    client.checkConnection();
    std::vector<uint8_t> cmd_data = {0xAA};
    client.sendCommand(CMD_COLLECT_CONTROL, cmd_data);

    while (1)
    {
        client.delay_ms(100);
    }

    client.disconnectFromServer();
    return 0;
}
