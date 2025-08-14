#ifndef _EXTI_H
#define _EXTI_H
#include "main.h"

extern uint8_t first_cycle;
extern uint8_t start_transmit;
extern __align(4) AudioTypeDef pcmStream;
extern PtlTypeDef protocol;
extern uint32_t DMA_num;
extern uint32_t LastDMA_num;
extern uint16_t LastRemain;
extern uint8_t LastTime[3];
extern uint8_t cross;


void GPIO_EXTI9_Init(void);  //��ʼ��PB9���Ž��������壬δ����
void EXTI9_5_IRQHandler(void);  //�������жϴ�����
void MasterTransferCallback(void); //DMA�жϴ�����
#endif