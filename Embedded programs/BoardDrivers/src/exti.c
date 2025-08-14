#include "exti.h"
#include "main.h"

uint8_t first_cycle=1;  //��־�����ɼ���֮����������������
uint8_t start_transmit=0;  //��ʼ�ɼ�ָ���·�֮��ĵ�һ������������֮����һ�����豸ͬʱ��ʼ�ɼ�
uint32_t DMA_num = 0;
uint32_t LastDMA_num = 0;
uint16_t LastRemain = 0;
uint16_t NDTR_num=0;
uint8_t LastTime[3]={0x00};
uint8_t cross=0;


void GPIO_EXTI9_Init(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //����GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //����SYSCFGʱ��
	
	//��ʼ��PB9
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;  //PB9��Ӧ����·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  //ʹ��EXTI��·
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI9_5_IRQHandler(void) //�ⲿ�жϻص�����
{  
	//�ж��Ƿ�ΪEEXTI9
	if (EXTI_GetITStatus(EXTI_Line9)!=RESET)
  {
    if (protocol.isLaunchCS5340)
    {
      if(first_cycle==1)
      {
        start_transmit=1;
        LastDMA_num=0;
        LastRemain = pcmStream.pcmBufLength/2;  //��һ��ѭ�������������һ��buffer����852����
      }
      else
      {
        NDTR_num=DMA1_Stream3->NDTR;
        LastDMA_num=DMA_num;
        
        //�������buffer_point_num=1705��Ϊ10ms�����Ծ������һ��buffer��ʱ��ΪLastRemain/buffer_point_num*10ms
        if (NDTR_num>(pcmStream.pcmBufLength/2))   
        {
          LastRemain = NDTR_num-pcmStream.pcmBufLength/2;
        }
        else
        {
          LastRemain = NDTR_num;
        }
      }
      memcpy(LastTime, Time, sizeof(Time));
    }
//		STM_EVAL_LEDToggle(LED3);
		EXTI_ClearITPendingBit(EXTI_Line9);  //���EXTI9�жϹ���λ
	}
} 

void MasterTransferCallback(void)  //DMA�жϻص�����
{
	/* Half Transfer Complete.*/
	if(DMA_GetITStatus(MASTER_SPIx_RX_DMA_STREAM, MASTER_SPIx_DMA_STREAM_IT_HTIFx) != RESET)
	{                  
		pcmStream.upLoading = TRANSFER_BUFFER_HALF;  //TRANSFER_BUFFER_HALF=1

		DMA_ClearITPendingBit(MASTER_SPIx_RX_DMA_STREAM, MASTER_SPIx_DMA_STREAM_IT_HTIFx);
	}
	/* Transfer Complete.*/
	else if(DMA_GetITStatus(MASTER_SPIx_RX_DMA_STREAM, MASTER_SPIx_DMA_STREAM_IT_TCIFx) != RESET)
	{
		pcmStream.upLoading = TRANSFER_BUFFER_FULL;  //TRANSFER_BUFFER_FULL=2

		DMA_ClearITPendingBit(MASTER_SPIx_RX_DMA_STREAM, MASTER_SPIx_DMA_STREAM_IT_TCIFx);
	}
  DMA_num++;
  if (cross>0)
  {
    cross--;
  }
}



