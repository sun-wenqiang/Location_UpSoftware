#include "iis.h"
#include "main.h"

/**
  * @brief  Configures the I2S Peripheral.
  * @param  None
  * @retval None
  */
void I2S_Config(uint32_t freq)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2S_InitTypeDef I2S_InitStructure;
	
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the I2Sx/I2Sx_ext clock */
  MASTER_I2Sx_CLK_INIT(MASTER_I2Sx_CLK, ENABLE);
	
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(MASTER_I2Sx_WS_GPIO_CLK | MASTER_I2Sx_CK_GPIO_CLK  | \
                         MASTER_I2Sx_SD_GPIO_CLK | MASTER_I2Sx_MCK_GPIO_CLK | \
	                       MASTER_SPIx_DMA_CLK, ENABLE);

  /* I2S GPIO Configuration --------------------------------------------------*/
  /* Connect I2S pins to Alternate functions */  
  GPIO_PinAFConfig(MASTER_I2Sx_WS_GPIO_PORT, MASTER_I2Sx_WS_SOURCE, MASTER_I2Sx_WS_AF);
  GPIO_PinAFConfig(MASTER_I2Sx_CK_GPIO_PORT, MASTER_I2Sx_CK_SOURCE, MASTER_I2Sx_CK_AF);
  GPIO_PinAFConfig(MASTER_I2Sx_SD_GPIO_PORT, MASTER_I2Sx_SD_SOURCE, MASTER_I2Sx_SD_AF);
  GPIO_PinAFConfig(MASTER_I2Sx_MCK_GPIO_PORT, MASTER_I2Sx_MCK_SOURCE, MASTER_I2Sx_MCK_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

  /* I2S WS pin configuration */
  GPIO_InitStructure.GPIO_Pin = MASTER_I2Sx_WS_PIN;
  GPIO_Init(MASTER_I2Sx_WS_GPIO_PORT, &GPIO_InitStructure);

  /* I2S CK pin configuration */
  GPIO_InitStructure.GPIO_Pin =  MASTER_I2Sx_CK_PIN;
  GPIO_Init(MASTER_I2Sx_CK_GPIO_PORT, &GPIO_InitStructure);
  
  /* I2S SD pin configuration */
  GPIO_InitStructure.GPIO_Pin = MASTER_I2Sx_SD_PIN;
  GPIO_Init(MASTER_I2Sx_SD_GPIO_PORT, &GPIO_InitStructure);

  /* I2S MCK pin configuration */
  GPIO_InitStructure.GPIO_Pin =  MASTER_I2Sx_MCK_PIN;
  GPIO_Init(MASTER_I2Sx_MCK_GPIO_PORT, &GPIO_InitStructure);

  /* I2S configuration -------------------------------------------------------*/
  /* Initialize  I2Sx and I2Sxext peripherals */
  SPI_I2S_DeInit(MASTER_I2Sx);

  /* Configure the Audio Frequency, Standard and the data format */
  I2S_InitStructure.I2S_AudioFreq = freq;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_24b;
  /* Configure I2Sx in Master Receive Mode */
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_Init(MASTER_I2Sx, &I2S_InitStructure);

  SPI_I2S_DMACmd(MASTER_I2Sx, SPI_I2S_DMAReq_Rx, ENABLE);
  //I2S_Cmd(MASTER_I2Sx, ENABLE);
}

void Stop_DMA_I2S(void)
{
  DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, DISABLE);
  I2S_Cmd(MASTER_I2Sx, DISABLE);
  memset(pcmStream.pcmBuffer, 0, sizeof(pcmStream.pcmBuffer));
  memset(pcmStream.SingleCh_Buffer, 0, sizeof(pcmStream.SingleCh_Buffer));
  Clear_I2S_FIFO();
}

// 配置DMA并开启
void Config_DMA_I2S(uint16_t* addr, uint32_t len)
{
  /* DMA configuration -------------------------------------------------------*/
  DMA_InitTypeDef DMA_InitStructure;
  /* Deinitialize DMA Streams */
  DMA_DeInit(MASTER_SPIx_RX_DMA_STREAM);

  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	
	/* Configure Maseter SPI2 DMA Transfer Complete and Half Transfer Complete.*/
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_BufferSize = len ;    //这个值作为CNDTR寄存器的值

  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(MASTER_I2Sx->DR));
  DMA_InitStructure.DMA_Channel = MASTER_SPIx_RX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)addr;
  DMA_Init(MASTER_SPIx_RX_DMA_STREAM, &DMA_InitStructure);

	/* Configure DMA Transfer Complete Interrupt.*/
  DMA_ITConfig(MASTER_SPIx_RX_DMA_STREAM, DMA_IT_TC | DMA_IT_HT, ENABLE);
}

void Clear_I2S_FIFO(void)
{
  while (SPI_I2S_GetFlagStatus(MASTER_I2Sx, SPI_I2S_FLAG_RXNE)==SET)
  {
    uint16_t temp = SPI_I2S_ReceiveData(MASTER_I2Sx);
    (void)temp;
  }
}
