/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IIS_H
#define __IIS_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported typedef -----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Exported define ------------------------------------------------------------*/
/* Uncomment the line below if you will use the I2S peripheral as a Master */

/* I2Sx Communication boards Interface */
#define MASTER_I2Sx                    SPI2
#define MASTER_I2Sx_CLK                RCC_APB1Periph_SPI2
#define MASTER_I2Sx_CLK_INIT           RCC_APB1PeriphClockCmd

#define MASTER_I2Sx_MCK_PIN            GPIO_Pin_6
#define MASTER_I2Sx_MCK_GPIO_PORT      GPIOC
#define MASTER_I2Sx_MCK_GPIO_CLK       RCC_AHB1Periph_GPIOC
#define MASTER_I2Sx_MCK_SOURCE         GPIO_PinSource6
#define MASTER_I2Sx_MCK_AF             GPIO_AF_SPI2

#define MASTER_I2Sx_WS_PIN             GPIO_Pin_12
#define MASTER_I2Sx_WS_GPIO_PORT       GPIOB
#define MASTER_I2Sx_WS_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define MASTER_I2Sx_WS_SOURCE          GPIO_PinSource12
#define MASTER_I2Sx_WS_AF              GPIO_AF_SPI2

#define MASTER_I2Sx_CK_PIN             GPIO_Pin_13
#define MASTER_I2Sx_CK_GPIO_PORT       GPIOB
#define MASTER_I2Sx_CK_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define MASTER_I2Sx_CK_SOURCE          GPIO_PinSource13
#define MASTER_I2Sx_CK_AF              GPIO_AF_SPI2

#define MASTER_I2Sx_SD_PIN             GPIO_Pin_15
#define MASTER_I2Sx_SD_GPIO_PORT       GPIOB
#define MASTER_I2Sx_SD_GPIO_CLK        RCC_AHB1Periph_GPIOB
#define MASTER_I2Sx_SD_SOURCE          GPIO_PinSource15
#define MASTER_I2Sx_SD_AF              GPIO_AF_SPI2

#define MASTER_SPIx_DMA                 DMA1
#define MASTER_SPIx_DMA_CLK             RCC_AHB1Periph_DMA1
#define MASTER_SPIx_RX_DMA_CHANNEL      DMA_Channel_0
#define MASTER_SPIx_RX_DMA_STREAM       DMA1_Stream3
#define MASTER_SPIx_DMA_STREAM_IT_HTIFx DMA_IT_HTIF3
#define MASTER_SPIx_DMA_STREAM_IT_TCIFx DMA_IT_TCIF3
#define MASTER_SPIx_RX_DMA_STREAM_IRQ   DMA1_Stream3_IRQn
#define MasterTransferCallback          DMA1_Stream3_IRQHandler

#define TX_MASTER_BUFFERSIZE           (countof(aTxMasterBuffer) - 1)
#define RX_MASTER_BUFFERSIZE           TX_MASTER_BUFFERSIZE

#define TX_SLAVE_BUFFERSIZE            (countof(aTxSlaveBuffer) - 1)
#define RX_SLAVE_BUFFERSIZE            TX_SLAVE_BUFFERSIZE
/* Uncomment the line below if you will use the I2S peripheral as a Slave */
/* #define I2S_SLAVE */
/* ......*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void I2S_Config(uint32_t freq);
void Stop_DMA_I2S(void);
void Config_DMA_I2S(uint16_t* addr, uint32_t len);
void Clear_I2S_FIFO(void);  //清空I2S中残留的数据
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
