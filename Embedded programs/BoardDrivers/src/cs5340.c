#include "cs5340.h"
ModifyFsDef fsReferList[maxfs] =
{
	//{384,5,20000},   //HSE=12M
  {75,2,20000},  //HSE=16.368M
	{258,3,48000},
	{320,2,125000},
  {80,2,79922},
  {128,2,85250}  // �ֱ��ʾPLLN��PLLI2SR���������ʵ�Ĳ�����
};
extern uint8_t fs_index;
/**
  * @brief  Reset Cs5340.
  * @param  None
  * @retval None
  */
void CS5340_ResetPinConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(CS5340_RST_GPIO_CLK, ENABLE);
	
	/* CS5340 RST pin configuration */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Pin =  CS5340_RST_PIN;
  GPIO_Init(CS5340_RST_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initialize CS5340.
  * @param  None
  * @retval None
  */
void CS5340_Initialize(AudioTypeDef* pcm_stream)
{
	uint32_t plli2snv;
	uint32_t plli2srv;
	
	/* Reset CS5340.*/
	CS5340_ResetPinConfig();  //��ʼ��cs5340��������PB8
	CS5340_RST_PULL_LOW;  //PB8��0
	CS5340_RST_PULL_UP;  //PB8��1
	
	/* Calculate DMA's buffer length.*/
	pcm_stream->pcmBufLength       = HW_BUFFER_LEN(pcm_stream->sampleAccuracy, pcm_stream->micNbr,
	                                               pcm_stream->sampleFreq);
	pcm_stream->upLoading          = 0;
	pcm_stream->pcmBufferAddr[0]   = 0;
	pcm_stream->pcmBufferAddr[1]   = &(pcm_stream->pcmBuffer[0]);  //��������һ��Ԫ�صĵ�ַ
	pcm_stream->pcmBufferAddr[2]   = &(pcm_stream->pcmBuffer[ pcm_stream->pcmBufLength/2 ]);  //�������м�Ԫ�صĵ�ַ
  
  extern PtlTypeDef protocol;	
	/* PLLI2s as i2s clock source*/
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);  //��PLLI2Sʱ��Դ����I2S
	
	/* Obtain fs tab from ROM(at24c02).*/

	plli2snv = fsReferList[ fs_index ].plli2snVaule;
	plli2srv = fsReferList[ fs_index ].plli2srVaule;
	
	/* Configure PLLI2S */
	RCC_PLLI2SConfig(plli2snv, plli2srv);  //����PLLI2Sʱ��Դ
	
	/* Enable PLLI2s*/
	RCC_PLLI2SCmd(ENABLE);
	
  /* Wait till PLLI2S is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET)  //�ȴ�PLLI2Sʱ��Դ�ȶ�
  {
  }
	
	/* Configure IIS Peripheral.*/
  I2S_Config(pcm_stream->sampleFreq);  //����I2S
}

extern AudioTypeDef pcmStream;
extern PtlTypeDef protocol;
/**
  * @brief  Audio Start.
  * @param  None
  * @retval None
  */
void AudioStart()
{
	protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_Y;
	pcmStream.upLoading = TRANSFER_PROGRESSING;
}
/**
  * @brief  Audio Stop.
  * @param  None
  * @retval None
  */
void AudioStop()
{
	protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
	pcmStream.upLoading = TRANSFER_PROGRESSING;
}
