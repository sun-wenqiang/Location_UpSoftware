#include "iwdg.h"




void LaunchWatchDog(uint8_t iwdgpsc,uint16_t iwdgrlr)
{
	/* 解除IWDG_KR保护 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* 设置预分频系数 */
	IWDG_SetPrescaler(iwdgpsc);
	
	/* 装载初值 */
	IWDG_SetReload(iwdgrlr);
	
	/* 加载初值 */
	IWDG_ReloadCounter();
	
	/* 使能独立看门狗 */
	IWDG_Enable();
}

//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}

