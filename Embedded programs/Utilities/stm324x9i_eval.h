/**
  ******************************************************************************
  * @file    STM324x7i_eval.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    19-September-2013
  * @brief   This file contains definitions for STM324x7I_EVAL's Leds, push-buttons
  *          and COM ports hardware resources.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM324X9I_EVAL_H
#define __STM324X9I_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
	 
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM324x7I_EVAL
  * @{
  */
      
/** @addtogroup STM324x7I_EVAL_LOW_LEVEL
  * @{
  */ 

/** @defgroup STM324x7I_EVAL_LOW_LEVEL_Exported_Types
  * @{
  */
typedef enum 
{
  COM1 = 0,
  COM2 = 1
} COM_TypeDef;
/**
  * @}
  */ 

/** @defgroup STM324x7I_EVAL_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** 
  * @brief  Define for STM324x7I_EVAL board  
  */ 
#if !defined (USE_STM324x7I_EVAL)
 #define USE_STM324x7I_EVAL
#endif

/** @addtogroup STM324x7I_EVAL_LOW_LEVEL_COM
  * @{
  */
#define COMn                             2

/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource9
#define EVAL_COM1_TX_AF                  GPIO_AF_USART1
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource10
#define EVAL_COM1_RX_AF                  GPIO_AF_USART1
#define EVAL_COM1_IRQn                   USART1_IRQn
#define EVAL_COMx                        EVAL_COM1

/**
  * @}
  */ 

/** @defgroup STM324x7I_EVAL_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_COMInit(COM_TypeDef COM, uint32_t baudrate);
/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif

#endif /* __STM324x7I_EVAL_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
