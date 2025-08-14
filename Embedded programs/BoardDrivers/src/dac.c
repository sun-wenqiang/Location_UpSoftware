#include "main.h"

void GPIO_Init_DAC(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //DAC1输出在PA4上
  
  GPIO_InitTypeDef GPIO_InitStructure;  //配置IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //模拟模式
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_Init_Config(void)
{
  GPIO_Init_DAC();
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);  //使能DAC的时钟
  
  DAC_InitTypeDef DAC_InitStructure;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1;  //不使用LFSR（用来生成噪声）和三角波
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);  //初始化DAC通道1
  DAC_Cmd(DAC_Channel_1, ENABLE);  //开启DAC通道1
}

void DAC_Set_Value(uint16_t value)  //输出电压
{
  if (value>=4096)
  {
    value = 4095;
  }
  DAC_SetChannel1Data(DAC_Align_12b_R, value);
}

