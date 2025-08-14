/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT24C02_H
#define __AT24C02_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
/* Exported define -----------------------------------------------------------*/
#define SCL_Pin  GPIO_Pin_6
#define SCL_Port GPIOB

#define SDA_Pin  GPIO_Pin_7
#define SDA_Port GPIOB
	 
/*****************************************************************************************
 *                                    DAQ ROM System                          
 *****************************************************************************************
 *
 *                                ¡¾AT24C02 Store List¡¿
 *  +¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-----------
 *  | AT24C02 address|       information of store
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x00          |       device ID(4 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x04          |       firmware version(2 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x06          |       application upgrad flag(1 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x07          |       ip(4 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x0b          |       port(2 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x0d          |       gw(4 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x11          |       mac(6 byte)
 *  |¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-------------
 *  |  0x17          |       fs tab(1 byte)
 *  +¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-----------
 *  |  0x18          |       net mask(4 byte)
 *  +¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-----------
 *  |  0x22          |       ID(1 byte)
 *  +¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª-----------
 *
 *****************************************************************************************/
 
#define ROM_DEVICE_ID               0x00                        
#define ROM_FW_VERSION              (ROM_DEVICE_ID      + 4) 
#define ROM_UPGRAD_FLAG             (ROM_FW_VERSION     + 2) 
#define ROM_SEVER_IP                (ROM_UPGRAD_FLAG    + 1) 
#define ROM_SEVER_PORT              (ROM_SEVER_IP       + 4) 
#define ROM_SEVER_GW                (ROM_SEVER_PORT     + 2) 
#define ROM_SEVER_MAC               (ROM_SEVER_GW       + 4) 
#define ROM_SAMPLE_FREQ_TAB         (ROM_SEVER_MAC      + 6) 
#define ROM_SEVER_MASK       			  (ROM_SAMPLE_FREQ_TAB   + 1) 
#define ROM_ID                      (ROM_SEVER_MASK     + 4)
#define ROM_THRESHOLD               (ROM_ID             + 1)
#define ROM_GAIN                    (ROM_THRESHOLD      + 2)
#define ROM_FREQ                    (ROM_GAIN           + 1)
#define ROM_RATIO                   (ROM_FREQ           + 2)


#define STORE_PARA_TOTAL            0x23  //ÔÚ¼ÓÈëID£¬ãÐÖµ£¬ÔöÒæ£¬¼ì²âÆµÂÊ£¬±ÈÖµÖ®ºó£¬´Ó1C£¨28£©¸ÄÎªÁË23£¨35£©
#define MAX_ROM_SIZE                255

/* Exported typedef ----------------------------------------------------------*/
typedef enum
{
	Normal   = 0,
	AbNormal = 1
}AT24C02Status;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void AT24C02_Init(void);
uint8_t AT24C02_ReadByte(uint8_t addr);
void AT24C02_WriteByte(uint8_t addr,uint8_t dat);
AT24C02Status CheckAT24C02(uint8_t addr,uint8_t dat);
AT24C02Status WriteInfo(uint8_t addr,uint8_t *str,uint8_t len);
void ReadInfo(uint8_t addr,uint8_t *str,uint8_t len);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

