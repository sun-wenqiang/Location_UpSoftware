include "protocal.h"


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


void sendCommand(QTcpSocket* tcpclient, uint8_t device_id, uint8_t cmd_id, const std::vector<uint8_t>& cmd_data)
{
    
}