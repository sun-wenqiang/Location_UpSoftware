#include "protocol.h"

/**
 * @brief   计算 CRC16-MODBUS 校验值
 * 
 * 本函数使用标准的 CRC16-MODBUS 算法计算输入数据的 16 位 CRC 校验值，
 * 并返回高字节在前、低字节在后的结果（大端格式）。
 * 
 * @param   inPtr  指向输入数据缓冲区的指针
 * @param   len    输入数据的字节长度
 * @return  uint16_t  计算得到的 CRC 校验值（高字节在前）
 * 
 * @note    该算法初始值为 0xFFFF，多项式为 0xA001。
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
uint16_t getCRC16(uint8_t* inPtr, size_t len)
{
    uint16_t crc = 0xffff;
    uint8_t index;
    while (len > 0) {
        crc ^= *inPtr;
        for (index = 0; index < 8; index++) {
            if ((crc & 0x0001) == 0)
                crc = crc >> 1;
            else {
                crc = crc >> 1;
                crc ^= 0xa001;
            }
        }
        len -= 1;
        inPtr++;
    }
    return ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
}

/**
 * @brief   向指定设备发送 TCP 协议命令
 * 
 * 本函数按照自定义协议格式，向指定设备 ID 发送命令数据。
 * 协议格式为：
 *   [帧头 0xFF 0xA5][数据总长度][设备ID][命令ID][命令数据...][CRC16低字节][CRC16高字节]
 * 其中 CRC16 使用 getCRC16(...) 计算，初始值为 0xFFFF，多项式 0xA001。
 * 
 * @param   tcpclient   指向已连接的 QTcpSocket 对象的指针
 * @param   device_id   目标设备 ID
 * @param   cmd_id      命令 ID
 * @param   cmd_data    命令数据内容（不含协议头和 CRC）
 * @return  bool        发送成功返回 true，发送失败返回 false
 * 
 * @note    函数内部会检查 TCP 连接状态，若未连接则直接返回 false。
 *          等待写入超时时间为 10 毫秒。
 * 
 * @see     getCRC16()
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
bool sendCommand(QTcpSocket* tcpclient, uint8_t device_id, uint8_t cmd_id, const std::vector<uint8_t>& cmd_data)
{
    if (!tcpcliend || tcpclient->state() != QAbstractSocket::ConnectedState) {
        qWarning << "TCP client is not connected.";
        return false;
    }

    QByteArray packet;
    packet.append(0XFF, 0XA5); // Protocol header
    uint8_t len = static_cast<uint8_t>(cmd_data.size() + 7); // Length of the command data + device_id and cmd_id
    packet.append(len);
    packet.append(device_id);
    packet.append(cmd_id);

    if (!cmd_data.empty()) {
        packet.append(reinterpret_cast<const char*>(cmd_data.data()), static_cast<int>(cmd_data.size()));
    }

    uint16_t crc = getCRC16(reinterpret_cast<uint8_t*>(packet.constData()), static_cast<size_t>(packet.size()));
    packet.append(static_cast<char>(crc & 0xFF));
    packet.append(static_cast<char>((crc >> 8) & 0xFF));
    
    qint64 bytesWritten = tcpclient->write(packet);
    if (bytesWritten == -1) {
        qWarning() << "Failed to write to TCP socket:" << tcpclient->errorString();
        return false;
    } 

    if (!tcpclient->waitForBytesWritten(10)) {
        qWarning() << "Timeout while waiting for bytes to be written:" << tcpclient->errorString();
        return false;
    }

    qDebug() << "Command sent successfully." << bytesWritten;
    return true;
}

/**
 * @brief   解析 TCP 接收到的响应数据帧
 * 
 * 本函数从原始 TCP 接收数据中查找帧头 (0xFF 0xA5)，校验数据长度和 CRC，
 * 若校验通过则解析出设备 ID、命令 ID 和数据负载，并分发到对应的处理函数。
 * 
 * 数据帧结构：
 *   [帧头 0xFF 0xA5][总长度][设备ID][命令ID][数据负载...][CRC16低字节][CRC16高字节]
 * 
 * @param   rawData  从 TCP 套接字接收到的原始字节数组
 * @return  int      成功返回 0，失败返回 -1（无数据 / 帧头错误 / 长度不足 / CRC 错误）
 * 
 * @note    函数会根据命令 ID 调用 handleGetPower、handleStatus、handleGetEnergy、
 *          handleGetCoordinates、handleArrivalTime、handleMultiParams 等对应的业务处理函数。
 * 
 * @see     getCRC16(), handleGetPower(), handleStatus(), handleGetEnergy(),
 *          handleGetCoordinates(), handleArrivalTime(), handleMultiParams()
 * 
 * @warning CRC 使用小端格式解析 (qFromLittleEndian)
 * 
 * @todo    若未来增加新的命令 ID，需要在 switch 中添加相应的 case 分支。
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
int ParseResponse(const QByteArray& rawData)
{
    if (rawData.isEmpty())
    {
        return -1;
    }

    int headIdx = -1;
    for (int i = 0; i < rawData.size() - 1; i++)
    {
        if ((uint8_t)rawData[i] == 0xFF && (uint8_t)rawData[i+1] == 0xA5)
        {
            headIdx = i;
        }
    }

    if (headIdx == -1 || headIdx + 2 > rawData.size())
    {
        qDebug("Header wrong!!!");
        return -1;
    }

    uint8_t totalLen = (uint8_t)rawData[headIdx + 2];
    if (headIdx + totalLen > rawData.size()) {
        qDebug() << "The length of raw data is not enough!!!";
        return -1;
    }

    QByteArray frame = rawData.mid(headIdx, totalLen);
    uint16_t crcReceived = qFromLittleEndian<uint16_t>((const uchar*)frame.constData() + totalLen - 2);
    uint16_t crcCalculated = getCRC16((uint8_t*)frame.constData(), totalLen - 2);
    if (crcReceived != crcCalculated) {
        qDebug() << "Cheeck CRC failed!!!";
        return -1;
    }

    // 字段解析
    uint8_t id = (uint8_t)frame[3];
    uint8_t cmd = (uint8_t)frame[4];
    QByteArray payload = frame.mid(5, totalLen - 7); // 去掉包头、长度、ID、cmd、CRC(2B)

    qDebug() << QString("Receive response of node %1").arg(id);

    switch (cmd)
    {
        case CMD_GET_POWER:
            handleGetPower(id, payload, totalLen);
            break;
        case CMD_COLLECT_CONTROL:
        case CMD_QUERY_GPS_STATUS:
        case CMD_REBOOT:
        case CMD_GET_RAW_SIGNAL:
            handleStatus(id, payload);
            break;
        case CMD_SET_AMPLIFICATION:
        case CMD_SET_DEVICE_ID:
        case CMD_SET_ENERGY_THRESHOLD:
        case CMD_SET_DETECT_FREQUENCY:
        case CMD_SET_RATIO_THRESHOLD:
            handleStatus(id, payload);
            break;
        case CMD_GET_ENERGY:
            handleGetEnergy(id, payload);
            break;
        case CMD_GET_COORDINATES:
            handleGetCoordinates(id, payload);
            break;
        case CMD_SIGNAL_ARRIVAL_TIME:
            handleArrivalTime(id, payload);
            break;
        case CMD_GET_MULTI_PARAMS:
            handleMultiParams(id, payload);
            break;
        default:
            qDebug() << QString("Unknown cmd id  0x%1").arg(cmd, 2, 16, QLatin1Char('0'));
            break;
    }

}

/**
 * @brief   处理“获取电量”指令的响应数据
 * 
 * 根据响应帧长度解析电量 ADC 值并计算对应电压，或报告错误码。
 * 
 * @param   id         设备节点 ID
 * @param   payload    指令内容数据（不包含包头、长度、ID、命令号和 CRC）
 * @param   totalLen   当前响应帧的总长度
 * 
 * @note    当 totalLen 为 9 时，payload 前 2 字节为 ADC 值，计算电压；
 *          当 totalLen 为 8 时，payload 第 1 字节为错误码，调用 reportError() 处理。
 * 
 * @return  void
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleGetPower(uint8_t id, QByteArray payload, uint8_t totalLen)
{
    if (totalLen == 9) {
        uint16_t adc_val = qFromLittleEndian<uint16_t>((const uchar*)payload.constData());
        double voltage = (double)adc_val / 4095.0 * 3.1 * 11 + 0.1;
        qDebug() << QString("ADC = %1, voltage = %2 V").arg(adc_val).arg(voltage, 0, 'f', 2);
    } else if (totalLen == 8) {
        reportError((uint8_t)payload[0]);
    } else {
        qDebug() << "Power length wrong!";
    }
}

/**
 * @brief   处理“获取能量”指令的响应数据
 * 
 * 解析并打印 27kHz 和 30kHz 频率下的能量值，数据均为 4 字节单精度浮点数。
 * 
 * @param   id       设备节点 ID
 * @param   payload  指令内容数据，包含两个连续的 float 值，分别对应 27kHz 和 30kHz 的能量
 * 
 * @return  void
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleGetEnergy(uint8_t id, QByteArray payload)
{
    qDebug()<<QString("Response from node %1: ").arg(id);
    float energy_27k;
    memcpy(&energy_27k, payload.constData(), 4);
    float energy_30k;
    memcpy(&energy_30k, payload.constData() + 4, 4);
    qDebug() << QString("Energy of 27kHz: %1, energy of 30kHz: %2")
                .arg(energy_27k, 0, 'f', 6)
                .arg(energy_30k, 0, 'f', 6);

}


/**
 * @brief   处理“获取经纬度”指令的响应数据
 * 
 * 解析 PPS 时间（时、分、秒）及经度和纬度，均为单精度浮点数，打印结果。
 * 
 * @param   id       设备节点 ID
 * @param   payload  指令内容数据，前3字节为时分秒，后8字节分别为经度和纬度（float）
 * 
 * @return  void
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleGetCoordinates(uint8_t id, QByteArray payload)
{
    qDebug()<<QString("Response from node %1: ").arg(id);
    uint8_t hour = (uint8_t)payload[0];
    uint8_t minute = (uint8_t)payload[1];
    uint8_t second = (uint8_t)payload[2];
    float lon, lat;
    memcpy(&lon, payload.constData() + 3, 4);
    memcpy(&lat, payload.constData() + 7, 4);
    qDebug() << QString("PPS: %1:%2:%3").arg(hour, 2, 10, QLatin1Char('0'))
                                                .arg(minute, 2, 10, QLatin1Char('0'))
                                                .arg(second, 2, 10, QLatin1Char('0'));
    qDebug() << QString("longitude: %1, latitude: %2").arg(lon, 0, 'f', 9).arg(lat, 0, 'f', 9);
}

/**
 * @brief   处理“信号到达时间”指令的响应数据
 * 
 * 解析时间（时、分、秒）、DMA 相关计数值、剩余计数和能量数据，
 * 并计算时间偏移量，打印到达时间、偏移量及 27kHz 能量值。
 * 
 * @param   id       设备节点 ID
 * @param   payload  指令内容数据，结构包含：
 *                  - 0-2字节：小时、分钟、秒
 *                  - 3-6字节：DMA_num_last (uint32_t)
 *                  - 7-10字节：DMA_num_now (uint32_t)
 *                  - 11-12字节：Remain_last (uint16_t)
 *                  - 13-14字节：Arrival_time (uint16_t)
 *                  - 15-18字节：27kHz能量 (float)
 * 
 * @return  void
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleArrivalTime(uint8_t id, QByteArray payload)
{
    qDebug()<<QString("Response from node %1: ").arg(id);
    uint8_t hour = (uint8_t)payload[0];
    uint8_t minute = (uint8_t)payload[1];
    uint8_t second = (uint8_t)payload[2];

    uint32_t DMA_num_last = qFromLittleEndian<uint32_t>((const uchar*)payload.constData() + 3);
    uint32_t DMA_num_now = qFromLittleEndian<uint32_t>((const uchar*)payload.constData() + 7);
    uint16_t Remain_last = qFromLittleEndian<uint16_t>((const uchar*)payload.constData() + 11);
    uint16_t Arrival_time = qFromLittleEndian<uint16_t>((const uchar*)payload.constData() + 13);
    float energy_27k;
    memcpy(&energy_27k, payload.constData() + 15, 4);

    double offset = (DMA_num_now - DMA_num_last - 3) * 0.01 +
                    Remain_last / 3410.0 * 0.01 +
                    Arrival_time / 1705.0 * 0.02;

    qDebug() << QString("Arrival time: %1:%2:%3").arg(hour, 2, 10, QLatin1Char('0'))
                                             .arg(minute, 2, 10, QLatin1Char('0'))
                                             .arg(second, 2, 10, QLatin1Char('0'));
    qDebug() << QString("With offset %1").arg(offset, 0, 'f', 5);
    qDebug() << QString("The energy of 27kHz is %1").arg(energy_27k, 0, 'f', 5);
}

/**
 * @brief   处理“获取多参数”指令的响应数据
 * 
 * 解析当前放大倍数、检测能量阈值、检测频率和检测比值阈值，并打印相关信息。
 * 
 * @param   id       设备节点 ID
 * @param   payload  指令内容数据，结构包含：
 *                  - 0字节：放大倍数索引 (uint8_t)
 *                  - 1-2字节：检测能量阈值 (uint16_t)
 *                  - 3-4字节：检测频率 (uint16_t)
 *                  - 5字节：检测比值阈值 (uint8_t)
 * 
 * @return  void
 * 
 * @note    gainValues 为预定义的放大倍数数组，索引对应放大倍数。
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleMultiParams(uint8_t id, QByteArray payload)
{
    qDebug()<<QString("Response from node %1: ").arg(id);
    uint8_t magnification = (uint8_t)payload[0];
    uint16_t threshold = qFromLittleEndian<uint16_t>((const uchar*)payload.constData() + 1);
    uint16_t frequency = qFromLittleEndian<uint16_t>((const uchar*)payload.constData() + 3);
    uint8_t ratio = (uint8_t)payload[5];

    qDebug() << QString("Magnification factor: %1 dB, detection frequency: %2 Hz, detection energy threshold: %3, detection ratio threshold: %4")
                .arg(gainValues[magnification])
                .arg(frequency)
                .arg(threshold)
                .arg(ratio);
}

/**
 * @brief   处理通用状态响应指令
 * 
 * 根据状态码打印成功信息或调用错误报告函数处理错误状态。
 * 
 * @param   id       设备节点 ID
 * @param   payload  指令内容数据，首字节为状态码 (uint8_t)
 * 
 * @return  void
 * 
 * @see     reportError()
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void handleStatus(uint8_t id, QByteArray payload)
{
    uint8_t status = (uint8_t)payload[0];
    qDebug()<<QString("Response from node %1: ").arg(id);
    switch (status)
    {
    case STATUS_SUCCESS:
        qDebug()<<"Successful.";
        break;
    default:
        reportError(status);
        break;
    }
}

/**
 * @brief   根据状态码打印错误信息
 * 
 * 解析状态码并输出对应的错误提示信息，用于命令响应错误处理。
 * 
 * @param   payload  指令内容数据，首字节为状态码 (uint8_t)
 * 
 * @return  void
 * 
 * @note    状态码定义见 CommandStatus 枚举。
 * 
 * @author  Sun Wenqiang
 * @date    2025-08-09
 * @copyright
 *          Copyright (c) 2025 Sun Wenqiang. All rights reserved.
 */
void reportError(QByteArray payload)
{
    uint8_t status = (uint8_t)payload[0];
    switch (status)
    {
    case STATUS_CMD_INVALID:
        qDebug() << "Command number error.";
        break;
    case STATUS_CONTENT_INVALID:
        qDebug() << "Command context error.";
        break;
    case STATUS_CRC_ERROR:
        qDebug() << "CRC error.";
        break;
    case STATUS_GPS_INVALID:
        qDebug() << "GPS invalid.";
        break;
    case STATUS_LENGTH_ERROR:
        qDebug() << "Command length error.";
        break;
    case STATUS_POWER_READ_TIMEOUT:
        qDebug() << "Read power timeout.";
        break;
    default:
        qDebug() << "Unknown status.";
        break;
    }
}