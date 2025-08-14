#ifndef _ADC_H
#define _ADC_H

#include "stm32f4xx.h"

#define ADC_Pin           GPIO_Pin_0
#define ADC_Port          GPIOB

typedef struct{
  uint16_t result;
  uint8_t status;
}ADC_Result;

void ADC_GPIO_Init(void);
void ADC_Init_Config(void);
ADC_Result Get_Adc(void);
ADC_Result Get_Adc_Average(uint8_t num);
#endif