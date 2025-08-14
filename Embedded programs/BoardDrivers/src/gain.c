#include "gain.h"


void GAIN_GPIO_Init(void)
{
  RCC_AHB1PeriphClockCmd(GAIN_Clock, ENABLE);
  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = G0_Pin|G1_Pin|G2_Pin;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  
  GPIO_Init(GAIN_GPIO_Port, &GPIO_InitStructure);
  
  GPIO_WriteBit(GAIN_GPIO_Port, G0_Pin, 0);  
  GPIO_WriteBit(GAIN_GPIO_Port, G1_Pin, 0); 
  GPIO_WriteBit(GAIN_GPIO_Port, G2_Pin, 1);
}

void Set_Gain(uint8_t value)
{
  GPIO_WriteBit(GAIN_GPIO_Port, G0_Pin, ((value>>0)&1));  
  GPIO_WriteBit(GAIN_GPIO_Port, G1_Pin, ((value>>1)&1)); 
  GPIO_WriteBit(GAIN_GPIO_Port, G2_Pin, ((value>>2)&1)); 
}