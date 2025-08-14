/**
  ******************************************************************************
  * @file    SPI/SPI_FLASH/spi_flash.h
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   This file contains all the functions prototypes for the spi_flash
  *          firmware driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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
#ifndef NET_CONF_H
#define NET_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "w5500.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "spi.h"
/* Exported constants --------------------------------------------------------*/
#if defined(STM32F40_41xxx) || defined(STM32F40XX)
#define WIZ_RST_Pin_CLK                      RCC_AHB1Periph_GPIOE
#define WIZ_RST_Pin                          GPIO_Pin_1
#define WIZ_RST_Port                         GPIOE
#endif

#if defined(STM32F429_439xx)
#define WIZ_RST_Pin_CLK                      RCC_AHB1Periph_GPIOE
#define WIZ_RST_Pin                          GPIO_Pin_3
#define WIZ_RST_Port                         GPIOE
#endif
	 
#define SOCKET_N 0
/* Exported types ------------------------------------------------------------*/
extern uint16_t LocalPort;
extern wiz_NetInfo NetConf;
/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void W5500_Init(void);
void W5500_Reset(void);
void W5500_Init_start(void);
void W5500_Init_reset(void);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
