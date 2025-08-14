/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS5340_H
#define __CS5340_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes -------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "iis.h"
#include "cmd.h"
/* Exported define -----------------------------------------------------------*/
#define CS5340_RST_PIN                 GPIO_Pin_8
#define CS5340_RST_GPIO_PORT           GPIOB
#define CS5340_RST_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define CS5340_RST_PULL_LOW            GPIO_ResetBits(CS5340_RST_GPIO_PORT, CS5340_RST_PIN)
#define CS5340_RST_PULL_UP             GPIO_SetBits(CS5340_RST_GPIO_PORT, CS5340_RST_PIN)

#define MAX_MIC_NBR                    2
#define MAX_SAMPLE_BITS                24
#define MAX_SAMPLE_FREQ                85250
#define HW_BUFFER_LEN(b,n,f)           ( ( ( (b/8) + 1)/2 )*n*f )/50    //一半buffer存10ms，6820

#define TRANSFER_PROGRESSING           0
#define TRANSFER_BUFFER_HALF           1
#define TRANSFER_BUFFER_FULL           2

#define buffer1_point_num    852   //pcmbuffer中前半个buffer的ch2采样点数量
#define buffer2_point_num    853   //pcmbuffer中后半个buffer的ch2采样点数量
#define buffer_point_num      1705  //pcmbuffer中ch2采样点数量

/* Exported typedef -----------------------------------------------------------*/
enum{
	_20khz = 0,
	_48khz  = 1,
	_125khz  = 2,
  _80khz = 3,
  _85khz = 4,
	maxfs
};
typedef struct{
	uint32_t plli2snVaule;
	uint32_t plli2srVaule;
	uint32_t fsRealVaule;
}ModifyFsDef;

typedef struct{
	uint32_t sampleFreq;  //采样率
	uint8_t  sampleAccuracy;  //采样精度
	uint8_t  micNbr;  //麦克风数量
	uint8_t  upLoading;  //上传？
	uint16_t pcmBuffer[HW_BUFFER_LEN(MAX_SAMPLE_BITS, MAX_MIC_NBR, MAX_SAMPLE_FREQ)]; 
  //6820/8+1=853，存放单通道半个buffer采样点
  int32_t SingleCh_Buffer[HW_BUFFER_LEN(MAX_SAMPLE_BITS, MAX_MIC_NBR, MAX_SAMPLE_FREQ)/8+1];   
  uint32_t pcmBufLength;  //缓冲区长度
	uint16_t* pcmBufferAddr[3];  //缓冲区地址
}AudioTypeDef;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void CS5340_ResetPinConfig(void);
void CS5340_Initialize(AudioTypeDef* pcm_stream);
void AudioStart(void);
void AudioStop(void);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
