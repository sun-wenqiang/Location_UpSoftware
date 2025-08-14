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
  uint8_t header[2];        //�����ֽڣ���ͷ
  uint8_t len;              //һ���ֽڣ�����
  uint8_t id;               //һ���ֽڣ��豸id
  uint8_t order_number;     //һ���ֽڣ�ָ���
  uint8_t PPS_time[3];      //�����ֽڣ��������Ӧ��ʱ�䣬ʱ����
  float Long;               //���ֽڣ�����
  float Lat;                //���ֽڣ�γ��
  uint16_t crc;             //�����ֽڣ�У����
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