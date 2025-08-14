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


void GPIO_EXTI9_Init(void);  //初始化PB9引脚接收秒脉冲，未开启
void EXTI9_5_IRQHandler(void);  //秒脉冲中断处理函数
void MasterTransferCallback(void); //DMA中断处理函数
#endif