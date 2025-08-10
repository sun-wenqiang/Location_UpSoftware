#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <QTcpSocket>
#include <QHostAddress>
#include <vector>
#include <cstdint>
#include <iostream>
#include <QDebug>
#include <QByteArray>
#include <map>
#include <QtEndian>

/*增益表*/
const int gainValues[] = {-120, 0, 6, 14, 20, 26, 34, 40};

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

uint16_t getCRC16(const uint8_t* inPtr, size_t len);
bool sendCommand(QTcpSocket* tcpclient, uint8_t device_id, uint8_t cmd_id, const std::vector<uint8_t>& cmd_data);
int ParseResponse(const QByteArray& rawData);
void handleGetPower(uint8_t id, QByteArray payload, uint8_t totalLen);
void handleGetEnergy(uint8_t id, QByteArray payload);
void handleGetCoordinates(uint8_t id, QByteArray payload);
void handleArrivalTime(uint8_t id, QByteArray payload);
void handleMultiParams(uint8_t id, QByteArray payload);
void handleStatus(uint8_t id, QByteArray payload);
void reportError(uint8_t payload);

#endif