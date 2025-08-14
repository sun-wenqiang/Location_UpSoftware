#include "exti.h"
#include "main.h"

uint8_t first_cycle=1;  //标志开启采集，之后的秒脉冲才起作用
uint8_t start_transmit=0;  //开始采集指令下发之后的第一个秒脉冲来了之后置一，让设备同时开始采集
uint32_t DMA_num = 0;
uint32_t LastDMA_num = 0;
uint16_t LastRemain = 0;
uint16_t NDTR_num=0;
uint8_t LastTime[3]={0x00};
uint8_t cross=0;


void GPIO_EXTI9_Init(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //启用GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  //启用SYSCFG时钟
	
	//初始化PB9
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;  //PB9对应的线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  //中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;  //使能EXTI线路
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI9_5_IRQHandler(void) //外部中断回调函数
{  
	//判断是否为EEXTI9
	if (EXTI_GetITStatus(EXTI_Line9)!=RESET)
  {
    if (protocol.isLaunchCS5340)
    {
      if(first_cycle==1)
      {
        start_transmit=1;
        LastDMA_num=0;
        LastRemain = pcmStream.pcmBufLength/2;  //第一个循环，距离存满第一个buffer还有852个点
      }
      else
      {
        NDTR_num=DMA1_Stream3->NDTR;
        LastDMA_num=DMA_num;
        
        //这里存满buffer_point_num=1705点为10ms，所以距离存满一个buffer的时间为LastRemain/buffer_point_num*10ms
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
		EXTI_ClearITPendingBit(EXTI_Line9);  //清除EXTI9中断挂起位
	}
} 

void MasterTransferCallback(void)  //DMA中断回调函数
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



