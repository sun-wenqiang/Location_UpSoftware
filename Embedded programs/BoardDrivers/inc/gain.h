#ifndef __GAIN_H
#define __GAIN_H
#include "stm32f4xx.h"

#define GAIN_GPIO_Port   GPIOE
#define GAIN_Clock       RCC_AHB1Periph_GPIOE
#define G0_Pin           GPIO_Pin_13
#define G1_Pin           GPIO_Pin_12
#define G2_Pin           GPIO_Pin_11


void GAIN_GPIO_Init(void);
void Set_Gain(uint8_t value);
#endif