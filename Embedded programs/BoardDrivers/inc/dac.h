#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx.h"

void GPIO_Init_DAC(void);  //����GPIO(PA4)
void DAC_Init_Config(void);  //����DAC����
void DAC_Set_Value(uint16_t value);  //����DAC�����ѹ
#endif

