#ifndef IWDG_H
#define IWDG_H

#include "stm32f4xx.h"


void LaunchWatchDog(uint8_t iwdgpsc,uint16_t iwdgrlr);
void IWDG_Feed(void);

#define FeedDog {IWDG->KR = 0xAAAA;}

#endif


