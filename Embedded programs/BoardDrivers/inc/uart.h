#ifndef _UART_H
#define _UART_H

#define UART3_CLOCK              RCC_APB1Periph_USART3
#define UART3_PORT_CLOCK         RCC_AHB1Periph_GPIOB
#define UART3_PORT               GPIOB
#define UART3_RX_PIN             GPIO_Pin_11
#define UART3_TX_PIN             GPIO_Pin_10
#define rxBuffersize             80

#include "stm32f4xx.h"

typedef struct 
{
  uint8_t header[2];        //两个字节，包头
  uint8_t len;              //一个字节，长度
  uint8_t id;               //一个字节，设备id
  uint8_t order_number;     //一个字节，指令号
  uint8_t PPS_time[3];      //三个字节，秒脉冲对应的时间，时分秒
  float Long;               //四字节，经度
  float Lat;                //四字节，纬度
  uint16_t crc;             //两个字节，校验码
}GPS_stream1;

extern GPS_stream1 GPS_message;

extern volatile uint8_t rxIndex;
extern volatile char rxBuffer[rxBuffersize];
extern uint8_t Time[3];
extern uint8_t TimeStamp[6];
extern uint16_t head_restart[2];
extern uint8_t validStatus;
extern float Latitude;
extern float Longitude;

void UART3_Init(void);
void USART3_IRQHandler(void);
void parseGPRMC(char *GPRMCsentence);
void UART3_SendString(const char * str);
void copyVolatileToNormal(char* dest, volatile const char* src, int size);
#endif