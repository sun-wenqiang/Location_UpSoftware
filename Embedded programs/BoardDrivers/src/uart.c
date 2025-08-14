#include "main.h"
#include <string.h>
#include <stdio.h>

GPS_stream1 GPS_message={{0xff, 0xa5},18,0,8};;


volatile uint8_t rxIndex = 0;
volatile char rxBuffer[rxBuffersize];
uint8_t Time[3]={0x00};  //20���ֽڵ�ʱ�����Ϊ����ʱ��������ݽ���һ��
uint8_t validStatus = 0;
float Latitude = 0.0f;
float Longitude = 0.0f;

void UART3_Init(void)
{
  RCC_APB1PeriphClockCmd(UART3_CLOCK,ENABLE);
  RCC_APB1PeriphClockCmd(UART3_PORT_CLOCK,ENABLE);
  
  /*����IO�ڣ�PB10(TX)��PB11(RX)*/
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = UART3_TX_PIN|UART3_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(UART3_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11,GPIO_AF_USART3);
  
  /*����UART3�������*/
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART3,&USART_InitStructure);

  /*���������ж�*/
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  /*ʹ��UART3*/
  USART_Cmd(USART3, ENABLE);
}


/*UART3�����жϻص�����*/
void USART3_IRQHandler(void)
{
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    char receivedChar = USART_ReceiveData(USART3);
    if (receivedChar == '$')
    {
      rxIndex = 0;  // ��һ���ַ�Ϊ$����ͷ��ʼ�洢
    }
    
    if (rxIndex < rxBuffersize - 1) 
    {
        rxBuffer[rxIndex++] = receivedChar;
    }

    if (receivedChar == '*')  // ��������ȡʱ���
    {
      // ����ַ���������
      rxBuffer[rxIndex] = '\0';
      char tempBuffer[rxBuffersize];
      strncpy(tempBuffer, (const char*)rxBuffer, sizeof(tempBuffer));
      
      // �ҵ���һ�����ź���ַ��� (��ʱ����Ŀ�ʼ)
      char *time_ptr = strchr(tempBuffer, ',') + 1;
      
      // ��ʱ����е�HH, MM, SS��ȡ��Time������
      if (time_ptr != NULL)
      {
        // ����time_ptrָ����ַ���Ϊ "HHMMSS"
        uint8_t hour = (time_ptr[0] - '0') * 10 + (time_ptr[1] - '0');   // ��ȡHH
        uint8_t minute = (time_ptr[2] - '0') * 10 + (time_ptr[3] - '0'); // ��ȡMM
        uint8_t second = (time_ptr[4] - '0') * 10 + (time_ptr[5] - '0'); // ��ȡSS

        // �洢��Time������
        Time[0] = hour;
        Time[1] = minute;
        Time[2] = second;
      }
      // ���ҵڶ������ţ���ȡ��Ч�Ա�ʶ��A��V��
      char *status_ptr = strchr(time_ptr, ',') + 1;
      if (status_ptr != NULL && (*status_ptr == 'A' || *status_ptr == 'V'))
      {
        // ��A��V�洢����Ч�Ա�ʶ��
        validStatus = *status_ptr == 'A' ? 1 : 0; // 1Ϊ��Ч��0Ϊ��Ч
      }
      
      if (validStatus==1){
      // ========== ���� GPRMC ��γ����ȡ�߼� ========== 
      if (strstr(tempBuffer, "RMC") != NULL)
      {
        char *token;
        int fieldIndex = 0;
        char latitudeStr[16] = {0};
        char latDir = 0;
        char longitudeStr[16] = {0};
        char lonDir = 0;

        token = strtok(tempBuffer, ",");
        while (token != NULL)
        {
          fieldIndex++;

          switch (fieldIndex)
          {
            case 4: // γ��
              strncpy(latitudeStr, token, sizeof(latitudeStr) - 1);
              break;
            case 5: // γ�ȷ���
              latDir = token[0];
              break;
            case 6: // ����
              strncpy(longitudeStr, token, sizeof(longitudeStr) - 1);
              break;
            case 7: // ���ȷ���
              lonDir = token[0];
              break;
            default:
              break;
          }

          token = strtok(NULL, ",");
        }

        // �� ddmm.mmmm ��ʽת��Ϊʮ���ƶ���
        float lat = atof(latitudeStr);
        float lon = atof(longitudeStr);

        float latDeg = (int)(lat / 100);
        float latMin = lat - latDeg * 100;
        float latFinal = latDeg + latMin / 60.0f;
        if (latDir == 'S') latFinal = -latFinal;

        float lonDeg = (int)(lon / 100);
        float lonMin = lon - lonDeg * 100;
        float lonFinal = lonDeg + lonMin / 60.0f;
        if (lonDir == 'W') lonFinal = -lonFinal;

        Latitude = latFinal;
        Longitude = lonFinal;
      }
      // ========== ��γ����ȡ���� ==========
    }
      
    }
  }
}




/*�����ַ����ĺ���*/
void UART3_SendString(const char * str)
{
  while (*str)
  {
    USART_SendData(USART3, (uint8_t)(*str++));
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
  }
}
