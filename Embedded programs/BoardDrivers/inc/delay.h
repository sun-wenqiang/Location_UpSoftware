#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx.h"

/* HCLK */
#define SysClockFreq  168

/**
  * @brief  SysTick初始化
  * @retval None
  */
void Delay_Initialize(void);

/**
  * @brief  毫秒级延时
  * @note   n的最大值为 0xffffffff
  * @param  n:延时的毫秒数
  * @retval None
  */
void Delay_ms(uint32_t n);

/**
  * @brief  微秒级延时
  * @note   n的最大值为 16777215（0xffffff）/us_base(21) = 798915 us
  * @param  n:延时的微秒数
  * @retval None
  */
void Delay_us(uint32_t n);

#endif

