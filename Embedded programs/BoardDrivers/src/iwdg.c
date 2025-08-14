#include "iwdg.h"




void LaunchWatchDog(uint8_t iwdgpsc,uint16_t iwdgrlr)
{
	/* ���IWDG_KR���� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* ����Ԥ��Ƶϵ�� */
	IWDG_SetPrescaler(iwdgpsc);
	
	/* װ�س�ֵ */
	IWDG_SetReload(iwdgrlr);
	
	/* ���س�ֵ */
	IWDG_ReloadCounter();
	
	/* ʹ�ܶ������Ź� */
	IWDG_Enable();
}

//ι�������Ź�
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}

