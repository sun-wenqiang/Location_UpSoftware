#include "delay.h"

static uint16_t us_base = 0;

static uint16_t ms_base = 0;

void Delay_Initialize(void)
{
	/* HCLK/8 */
	SysTick->CTRL &= ~(1<<2);
	
	/* 分秒时基 计算1us所要加载到Systick的数值 */
	us_base = SysClockFreq/8;
	
	/* 毫秒时基 计算1ms所要加载到Systick的数值 */
	ms_base = (SysClockFreq*1000)/8;
	
}

/**
  * @brief  基本毫秒级延时
  * @note   n的最大值为 16777215（0xffffff）/ms_base(21000) = 798.915 =798 ms（SysCoreClk为168MHZ）
  * @param  n:延时的毫秒数
  * @retval None
  */
static void Delay_ms_base(uint32_t n)
{
	uint32_t temp; 
	
	/* 超出范围 */
	if (n > 798)
	return;
	
	/* SysTick 重加载数值 */
	SysTick->LOAD = n*ms_base;
	
	/* SysTick 清零 */
	SysTick->VAL = 0;
	
	/* 使能SysTick */
	SysTick->CTRL |= (1<<0);
	
	/* 开始计数 */
	do{
		
		temp = SysTick->CTRL;
		
	}while(!(temp&(1<<16)));
	
	/* 关闭SysTick */
	SysTick->CTRL &= ~(1<<0);
	
	/* SysTick 清零 */
	SysTick->VAL = 0;
}

void Delay_ms(uint32_t n)
{
	
	uint8_t div;
	
	uint16_t remain;
	
	div = n/798;
	
	remain = n%798;
	
	while(div--){
		
		Delay_ms_base(798);
		
	}
	
	if(remain){
		
		Delay_ms_base(remain);
		
	}
	
}

void Delay_us(uint32_t n)
{
	uint32_t temp; 
	
	/* 超出范围 */
	if (n > 798915)
	return;
	
	/* SysTick 重加载数值 */
	SysTick->LOAD = n*us_base;
	
	/* SysTick 清零 */
	SysTick->VAL = 0;
	
	/* 使能SysTick */
	SysTick->CTRL |= (1<<0);
	
	/* 开始计数 */
	do{
		
		temp = SysTick->CTRL;
		
	}while(!(temp&(1<<16)));
	
	/* 关闭SysTick */
	SysTick->CTRL &= ~(1<<0);
	
	/* SysTick 清零 */
	SysTick->VAL = 0;
}
