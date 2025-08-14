#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx.h"

void GPIO_Init_DAC(void);  //配置GPIO(PA4)
void DAC_Init_Config(void);  //配置DAC并打开
void DAC_Set_Value(uint16_t value);  //控制DAC输出电压
#endif

