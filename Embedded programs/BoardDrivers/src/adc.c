#include "main.h"
#include "stm32f4xx_adc.h"


  



void ADC_Init_Config(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;
  ADC_InitTypeDef          ADC_InitStructure;
  ADC_CommonInitTypeDef    ADC_CommonInitStructure;  
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //GPIO口，使能AHB1时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  //ADC1挂在APB2上 
  
  //将IO口配置为模拟输入模式
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = ADC_Pin;
  GPIO_Init(ADC_Port, &GPIO_InitStructure);  //PB0，ADC1的CH8

  //配置ADC通用参数
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  //独立模式
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;  //APB2总线时钟除以这个分频系数得到ADC的时钟（不能超过36MHz）
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  //配置ADC1的参数
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //禁止连续转换
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //数据右对齐
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //禁用外部触发
  ADC_InitStructure.ADC_NbrOfConversion = 1;  //转换数量为1
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //单次转换模式
  ADC_Init(ADC1, &ADC_InitStructure);
  
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_144Cycles);  //通道8(PB0)，采样时间144周期
}


ADC_Result Get_Adc(void)
{
  ADC_Result Result = {0,1};  //值，状态

  ADC_SoftwareStartConv(ADC1);  //启动ADC转换
  
  uint32_t temp = 0;
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))  //等待转换完成
  {
    if (++temp>100000)
    {
      Result.status=0;  //异常
      break;
    }
  }
  if (Result.status!=0)
  {
    Result.result = ADC_GetConversionValue(ADC1);
  }
  return Result;   //返回转换结果
}


ADC_Result Get_Adc_Average(uint8_t num)
{
  uint32_t value = 0;  //这里防止好几个32位值加起来超出16位，所以使用32位
  ADC_Result ADC_result;
  
  ADC_Cmd(ADC1, ENABLE);   //ENABLE！！！！！！
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_15Cycles);  //配置需要转换的ADC通道
  
  for (uint8_t i=0; i<num; i++)
  {
    ADC_result = Get_Adc();
    if (ADC_result.status!=0)
    {
      value = value + ADC_result.result;   //返回转换结果
      Delay_us(10);
    }
    else
    {
      return ADC_result;
    }
  }
  ADC_result.result = (uint16_t)(value/num);
  ADC_Cmd(ADC1, DISABLE);
  return ADC_result;
}