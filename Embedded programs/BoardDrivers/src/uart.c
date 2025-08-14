#include "main.h"
#include <string.h>
#include <stdio.h>

GPS_stream1 GPS_message={{0xff, 0xa5},18,0,8};;


volatile uint8_t rxIndex = 0;
volatile char rxBuffer[rxBuffersize];
uint8_t Time[3]={0x00};  //20个字节的时间戳，为了让时间戳与数据解析一样
uint8_t validStatus = 0;
float Latitude = 0.0f;
float Longitude = 0.0f;

void UART3_Init(void)
{
  RCC_APB1PeriphClockCmd(UART3_CLOCK,ENABLE);
  RCC_APB1PeriphClockCmd(UART3_PORT_CLOCK,ENABLE);
  
  /*配置IO口，PB10(TX)和PB11(RX)*/
  GPIO_InitTypeDef GPIO_InitStructure;  
  GPIO_InitStructure.GPIO_Pin = UART3_TX_PIN|UART3_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(UART3_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11,GPIO_AF_USART3);
  
  /*配置UART3外设参数*/
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_Init(USART3,&USART_InitStructure);

  /*开启接收中断*/
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  /*使能UART3*/
  USART_Cmd(USART3, ENABLE);
}


/*UART3接收中断回调函数*/
void USART3_IRQHandler(void)
{
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    char receivedChar = USART_ReceiveData(USART3);
    if (receivedChar == '$')
    {
      rxIndex = 0;  // 第一个字符为$，从头开始存储
    }
    
    if (rxIndex < rxBuffersize - 1) 
    {
        rxBuffer[rxIndex++] = receivedChar;
    }

    if (receivedChar == '*')  // 解析语句获取时间戳
    {
      // 添加字符串结束符
      rxBuffer[rxIndex] = '\0';
      char tempBuffer[rxBuffersize];
      strncpy(tempBuffer, (const char*)rxBuffer, sizeof(tempBuffer));
      
      // 找到第一个逗号后的字符串 (即时间戳的开始)
      char *time_ptr = strchr(tempBuffer, ',') + 1;
      
      // 将时间戳中的HH, MM, SS提取到Time数组中
      if (time_ptr != NULL)
      {
        // 假设time_ptr指向的字符串为 "HHMMSS"
        uint8_t hour = (time_ptr[0] - '0') * 10 + (time_ptr[1] - '0');   // 提取HH
        uint8_t minute = (time_ptr[2] - '0') * 10 + (time_ptr[3] - '0'); // 提取MM
        uint8_t second = (time_ptr[4] - '0') * 10 + (time_ptr[5] - '0'); // 提取SS

        // 存储到Time数组中
        Time[0] = hour;
        Time[1] = minute;
        Time[2] = second;
      }
      // 查找第二个逗号，提取有效性标识（A或V）
      char *status_ptr = strchr(time_ptr, ',') + 1;
      if (status_ptr != NULL && (*status_ptr == 'A' || *status_ptr == 'V'))
      {
        // 将A或V存储到有效性标识中
        validStatus = *status_ptr == 'A' ? 1 : 0; // 1为有效，0为无效
      }
      
      if (validStatus==1){
      // ========== 新增 GPRMC 经纬度提取逻辑 ========== 
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
            case 4: // 纬度
              strncpy(latitudeStr, token, sizeof(latitudeStr) - 1);
              break;
            case 5: // 纬度方向
              latDir = token[0];
              break;
            case 6: // 经度
              strncpy(longitudeStr, token, sizeof(longitudeStr) - 1);
              break;
            case 7: // 经度方向
              lonDir = token[0];
              break;
            default:
              break;
          }

          token = strtok(NULL, ",");
        }

        // 将 ddmm.mmmm 格式转换为十进制度数
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
      // ========== 经纬度提取结束 ==========
    }
      
    }
  }
}




/*发送字符串的函数*/
void UART3_SendString(const char * str)
{
  while (*str)
  {
    USART_SendData(USART3, (uint8_t)(*str++));
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE)==RESET);
  }
}
