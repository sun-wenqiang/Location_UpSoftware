#ifndef PROTOCAL_H
#define PROTOCAL_H

#include <QTcpSocket>
#include <QHostAddress>
#include <vector>
#include <cstdint>
#include <iostream>

const uint8_t header[] = {0xFF, 0xA5};


uint16_t getCRC16(uint8_t* inPtr, size_t len);
void sendCommand(QTcpSocket* tcpclient, uint8_t device_id, uint8_t cmd_id, 
                 const std::vector<uint8_t>& cmd_data);




#endif