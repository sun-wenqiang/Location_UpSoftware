#include "delay.h"

static uint16_t us_base = 0;

static uint16_t ms_base = 0;

void Delay_Initialize(void)
{
	/* HCLK/8 */
	SysTick->CTRL &= ~(1<<2);
	
	/* ����ʱ�� ����1us��Ҫ���ص�Systick����ֵ */
	us_base = SysClockFreq/8;
	
	/* ����ʱ�� ����1ms��Ҫ���ص�Systick����ֵ */
	ms_base = (SysClockFreq*1000)/8;
	
}

/**
  * @brief  �������뼶��ʱ
  * @note   n�����ֵΪ 16777215��0xffffff��/ms_base(21000) = 798.915 =798 ms��SysCoreClkΪ168MHZ��
  * @param  n:��ʱ�ĺ�����
  * @retval None
  */
static void Delay_ms_base(uint32_t n)
{
	uint32_t temp; 
	
	/* ������Χ */
	if (n > 798)
	return;
	
	/* SysTick �ؼ�����ֵ */
	SysTick->LOAD = n*ms_base;
	
	/* SysTick ���� */
	SysTick->VAL = 0;
	
	/* ʹ��SysTick */
	SysTick->CTRL |= (1<<0);
	
	/* ��ʼ���� */
	do{
		
		temp = SysTick->CTRL;
		
	}while(!(temp&(1<<16)));
	
	/* �ر�SysTick */
	SysTick->CTRL &= ~(1<<0);
	
	/* SysTick ���� */
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
	
	/* ������Χ */
	if (n > 798915)
	return;
	
	/* SysTick �ؼ�����ֵ */
	SysTick->LOAD = n*us_base;
	
	/* SysTick ���� */
	SysTick->VAL = 0;
	
	/* ʹ��SysTick */
	SysTick->CTRL |= (1<<0);
	
	/* ��ʼ���� */
	do{
		
		temp = SysTick->CTRL;
		
	}while(!(temp&(1<<16)));
	
	/* �ر�SysTick */
	SysTick->CTRL &= ~(1<<0);
	
	/* SysTick ���� */
	SysTick->VAL = 0;
}
