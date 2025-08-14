#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx.h"

/* HCLK */
#define SysClockFreq  168

/**
  * @brief  SysTick��ʼ��
  * @retval None
  */
void Delay_Initialize(void);

/**
  * @brief  ���뼶��ʱ
  * @note   n�����ֵΪ 0xffffffff
  * @param  n:��ʱ�ĺ�����
  * @retval None
  */
void Delay_ms(uint32_t n);

/**
  * @brief  ΢�뼶��ʱ
  * @note   n�����ֵΪ 16777215��0xffffff��/us_base(21) = 798915 us
  * @param  n:��ʱ��΢����
  * @retval None
  */
void Delay_us(uint32_t n);

#endif

