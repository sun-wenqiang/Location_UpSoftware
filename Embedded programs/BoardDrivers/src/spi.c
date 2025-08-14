/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_FLASH
  * @{
  */  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void sFLASH_LowLevel_Init(void); 

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
	
  sFLASH_LowLevel_Init();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

//  /*!< Enable the sFLASH_SPI  */
//  SPI_Cmd(sFLASH_SPI, ENABLE);
	
	/*!<************************** SPI_DMA configuration ***********************/
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	/* Deinitialize DMA Structure */
  DMA_DeInit(sFLASH_DMA_STREAM);
	
  /* Configure DMA Initialization Structure */
//  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)(&(sFLASH_SPI->DR)) ;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = sFLASH_DMA_STREAM_CHANNEL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;
//  DMA_InitStructure.DMA_Memory0BaseAddr = 0;
  DMA_Init(sFLASH_DMA_STREAM, &DMA_InitStructure);
	
  /* Enable SPI1 Peripheral */
  SPI_Cmd(sFLASH_SPI, ENABLE);
	
  /* Clear DMA Transfer Complete Flags */
  DMA_ClearFlag(sFLASH_DMA_STREAM, sFLASH_DMA_FLAG_TCIFx);
	
  /* Enable USART DMA TX Requsts */
  SPI_I2S_DMACmd(sFLASH_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
}

/**
  * @brief  Transmit data through dma peripheral.
  * @param  buf: the first element's address of buffer.
  *         n:buffer's length.
  * @retval None.
  */
void dmaSPI_SendByte(uint8_t* buf, uint32_t n)
{
  /* Disable DMA USART TX Stream */
  DMA_Cmd(sFLASH_DMA_STREAM,DISABLE);
	
	/* Waiting for DMA is configurable */ 
	while((sFLASH_DMA_STREAM->CR & 0x00000001));
	
	/* Reload Memery and Length */
	sFLASH_DMA_STREAM->M0AR = (uint32_t)buf;
	sFLASH_DMA_STREAM->NDTR = n;
	
	/* Enable DMA USART TX Stream */
	DMA_Cmd(sFLASH_DMA_STREAM,ENABLE);

  /* Waiting the end of Data transfer */  
  while (DMA_GetFlagStatus(sFLASH_DMA_STREAM, sFLASH_DMA_FLAG_TCIFx)==RESET);
  
  /* Clear DMA Transfer Complete Flags */
  DMA_ClearFlag(sFLASH_DMA_STREAM, sFLASH_DMA_FLAG_TCIFx);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t sFLASH_ReadByte(void)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  sFLASH_SPI->DR = 0xff;

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return (sFLASH_SPI->DR);
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval None.
  */
void sFLASH_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  sFLASH_SPI->DR = byte;

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */

  sFLASH_SPI->DR;
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_LowLevel_Init(void)  //初始化与闪存芯片相关的硬件资源，GPIO/时钟等
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(sFLASH_SPI_SCK_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK | 
                         sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_CS_GPIO_CLK, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(sFLASH_SPI_SCK_GPIO_PORT, sFLASH_SPI_SCK_SOURCE, sFLASH_SPI_SCK_AF);
  GPIO_PinAFConfig(sFLASH_SPI_MISO_GPIO_PORT, sFLASH_SPI_MISO_SOURCE, sFLASH_SPI_MISO_AF);
  GPIO_PinAFConfig(sFLASH_SPI_MOSI_GPIO_PORT, sFLASH_SPI_MOSI_SOURCE, sFLASH_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MISO_PIN;
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
