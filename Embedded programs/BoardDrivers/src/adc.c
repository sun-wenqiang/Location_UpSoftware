#include "main.h"
#include "stm32f4xx_adc.h"


  



void ADC_Init_Config(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;
  ADC_InitTypeDef          ADC_InitStructure;
  ADC_CommonInitTypeDef    ADC_CommonInitStructure;  
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //GPIO�ڣ�ʹ��AHB1ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  //ADC1����APB2�� 
  
  //��IO������Ϊģ������ģʽ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = ADC_Pin;
  GPIO_Init(ADC_Port, &GPIO_InitStructure);  //PB0��ADC1��CH8

  //����ADCͨ�ò���
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;  //����ģʽ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;  //APB2����ʱ�ӳ��������Ƶϵ���õ�ADC��ʱ�ӣ����ܳ���36MHz��
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  //����ADC1�Ĳ���
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //��ֹ����ת��
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //�����Ҷ���
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  //�����ⲿ����
  ADC_InitStructure.ADC_NbrOfConversion = 1;  //ת������Ϊ1
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;  //����ת��ģʽ
  ADC_Init(ADC1, &ADC_InitStructure);
  
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_144Cycles);  //ͨ��8(PB0)������ʱ��144����
}


ADC_Result Get_Adc(void)
{
  ADC_Result Result = {0,1};  //ֵ��״̬

  ADC_SoftwareStartConv(ADC1);  //����ADCת��
  
  uint32_t temp = 0;
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))  //�ȴ�ת�����
  {
    if (++temp>100000)
    {
      Result.status=0;  //�쳣
      break;
    }
  }
  if (Result.status!=0)
  {
    Result.result = ADC_GetConversionValue(ADC1);
  }
  return Result;   //����ת�����
}


ADC_Result Get_Adc_Average(uint8_t num)
{
  uint32_t value = 0;  //�����ֹ�ü���32λֵ����������16λ������ʹ��32λ
  ADC_Result ADC_result;
  
  ADC_Cmd(ADC1, ENABLE);   //ENABLE������������
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_15Cycles);  //������Ҫת����ADCͨ��
  
  for (uint8_t i=0; i<num; i++)
  {
    ADC_result = Get_Adc();
    if (ADC_result.status!=0)
    {
      value = value + ADC_result.result;   //����ת�����
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