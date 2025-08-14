#include "main.h"

void GPIO_Init_DAC(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //DAC1�����PA4��
  
  GPIO_InitTypeDef GPIO_InitStructure;  //����IO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  //ģ��ģʽ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DAC_Init_Config(void)
{
  GPIO_Init_DAC();
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);  //ʹ��DAC��ʱ��
  
  DAC_InitTypeDef DAC_InitStructure;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1;  //��ʹ��LFSR���������������������ǲ�
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);  //��ʼ��DACͨ��1
  DAC_Cmd(DAC_Channel_1, ENABLE);  //����DACͨ��1
}

void DAC_Set_Value(uint16_t value)  //�����ѹ
{
  if (value>=4096)
  {
    value = 4095;
  }
  DAC_SetChannel1Data(DAC_Align_12b_R, value);
}

