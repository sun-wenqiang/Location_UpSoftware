/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H
#define __TIM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define USE_TIMx                  TIM3
#define RCC_PERIPH_CLK_CMD        RCC_APB1PeriphClockCmd
#define RCC_PERIPH_CLK            RCC_APB1Periph_TIM3
#define TIMx_INTERRUPR_IRQ        TIM3_IRQn
/* Exported functions ------------------------------------------------------- */
void TIM_Config(void);
void TIM_START_Config(void);
void TIM5_Init(void);
void Timer3_Start(uint8_t seconds);
#endif /* __TIM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
