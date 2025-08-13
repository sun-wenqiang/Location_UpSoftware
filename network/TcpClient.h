#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QTcpSocket>
#include <iostream>
#include <QString>
#include <QtGlobal>
#include <QTimer>
#include <QtEndian>
#include <chrono>
#include <thread>
#include "CoordConverter.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "CoordConverter.h"

/*增益表*/
extern const int gainValues[8];

enum ClientState {
    Disconnected,
    Connected,
    Idle,
    Working,
    Rebooted,
    Error,
    waitingResponse,
    ResponseTimeout
};

/**
 * 设备指令枚举
 */
typedef enum {
    CMD_GET_POWER              = 0x00,  // 获取电量
    CMD_CONFIG_NETWORK         = 0x01,  // 配置网络参数
    CMD_COLLECT_CONTROL        = 0x02,  // 采集控制
    CMD_SET_AMPLIFICATION      = 0x03,  // 设置放大倍数
    CMD_SET_DEVICE_ID          = 0x04,  // 设置设备ID
    CMD_SIGNAL_ARRIVAL_TIME    = 0x05,  // 信号到达时间
    CMD_SET_ENERGY_THRESHOLD   = 0x06,  // 设置检测能量阈值
    CMD_QUERY_GPS_STATUS       = 0x07,  // 查询GPS是否有效
    CMD_GET_COORDINATES        = 0x08,  // 获取经纬度
    CMD_REBOOT                 = 0x09,  // 重启
    CMD_GET_ENERGY             = 0x0A,  // 获取能量
    CMD_GET_RAW_SIGNAL         = 0x0B,  // 获取原始信号
    CMD_SET_DETECT_FREQUENCY   = 0x0C,  // 设置检测频率
    CMD_GET_MULTI_PARAMS       = 0x0D,  // 获取当前放大倍数、检测的能量阈值、检测的比值阈值和检测频率参数
    CMD_SET_RATIO_THRESHOLD    = 0x0E,  // 设置检测的比值阈值
} CommandID;

/**
 * 指令执行状态枚举
 */
typedef enum {
    STATUS_SUCCESS               = 0x00,  // 正确执行
    STATUS_CMD_INVALID           = 0x03,  // 指令号错误
    STATUS_CONTENT_INVALID       = 0x04,  // 指令内容错误
    STATUS_MEMORY_ERROR          = 0x05,  // 内存写入错误
    STATUS_LENGTH_ERROR          = 0x07,  // 长度错误
    STATUS_CRC_ERROR             = 0x08,  // CRC错误
    STATUS_POWER_READ_TIMEOUT    = 0x09,  // 读取电量超时（修改点）
    STATUS_GPS_INVALID           = 0x0A,  // GPS无效
} CommandStatus;

typedef enum{
    FREE                = 0x00,  // 空闲
    CONNECTED           = 0x01,  // 指令号错误
    DISCONNECTED        = 0x02,  // 指令内容错误
}TcpClient_status;

struct Time{
    int hour;
    int minute;
    int second;
};

struct Info_ArrivalTime{
    int id;
    GeoCoord corrd;
    Time time;
    double offset;
    bool used;
};

struct NodeInfo{
    int id;
    QString ip;
    quint16 port;
    double longitude;
    double latitude;
    double altitude;
};


class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    TcpClient(const QString & ip, const quint16 port, uint8_t id, QObject *parent = nullptr);
    ~TcpClient();

    bool connectToServer();
    bool disconnectFromServer();
    void checkConnection();
    bool sendCommand(uint8_t cmd_id, const std::vector<uint8_t>& cmd_data);
    uint16_t getCRC16(const uint8_t* inPtr, size_t len);
    void delay_ms(uint32_t ms);
    void setStatus(ClientState new_state);
    ClientState getStatus();


signals:
    void dataReceived(const QByteArray data);
    void heartbeatLoss();
    void receiveResponse(uint8_t cmd);
    void receiveArrivalTime(Info_ArrivalTime& arrival_info);

public slots:
    void onReadyRead();
    void handleDataReceived();
    void handleHeartbeatLoss();

private:
    CommandStatus cmd_status;
    QString ip;
    quint16 port;
    uint8_t id;
    QTimer * heartbeatTimer;   // 定期检查是否断开连接
    QByteArray receiveBuffer;
    int heartbreak_count = 0;
    ClientState status;

    int ParseResponse(QByteArray& rawData);
    void handleGetPower(uint8_t id, QByteArray payload, uint8_t totalLen);
    void handleGetEnergy(uint8_t id, QByteArray payload);
    void handleGetCoordinates(uint8_t id, QByteArray payload);
    void handleArrivalTime(uint8_t id, QByteArray payload);
    void handleMultiParams(uint8_t id, QByteArray payload);
    void handleStatus(uint8_t id, QByteArray payload);
    void reportError(uint8_t payload);
};


bool readNodeInfo(const QString& filename, int nodeID, NodeInfo& node);
bool updateNodeInfo(const QString& filename, NodeInfo& node);

#endif
