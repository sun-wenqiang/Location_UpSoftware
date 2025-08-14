/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_H
#define __CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "cs5340.h"	 
#include "at24c02.h"	
#include "net_conf.h"
#include <stdlib.h>
#include <stdio.h>
#include "tim.h"

extern uint16_t threshold;
extern int send_point;

#define FIRM_EDITION_LEN         31

/* Exported define -----------------------------------------------------------*/
	 /* command's format.*/
#define CMD_BUF_LEN                  255
#define CMD_HEADER                   0xaa

	 /* command's type.*/
#define CMD_DEVICE_CONF              0x01
#define CMD_DISCONNECT_N             0
#define CMD_DISCONNECT_Y             1
	 
#define CMD_RESPONSE_N               0
#define CMD_RESPONSE_Y               1
	 
#define CMD_LAUNCH_CS5340_N          0
#define CMD_LAUNCH_CS5340_Y          1

#define CMD_DEVICE_SWITCH            0x02
#define CMD_PARAM_VALVE_START        0x01
#define CMD_PARAM_VALVE_STOP         0x00

#define CMD_STATUS_READY	           0
#define CMD_STATUS_STARTED	         1
#define CMD_STATUS_STOPED	           2

#define CMD_REPLY_ROM_INFO           0x03
#define CMD_REPLY_INFO_N             0
#define CMD_REPLY_INFO_Y             1

#define CMD_DEVICE_SETTING           0x04

#define CMD_DEVICE_STATUS            0x05

#define CMD_DEVICE_UPGRAD            0x06
#define CMD_PERMIT_APP_UPGRAD        1
#define CMD_FORBIT_APP_UPGRAD        0

#define CMD_REPLY_FIRMWARE_EDITION    0x00
#define CMD_REPLY_FIRM_N             0
#define CMD_REPLY_FIRM_Y             1

#define CMD_RESET_NET_CONFIG         0xFF

#define CMD_NONE                     0x0F


// *************   protocol V2   start    ******************** //
#define CMD_HEADER_P2                   0xA5FF  //P2Э���ͷ

#define CMD_GET_VOLTAGE_P2              0x00  //��ȡ��ѹ
#define CMD_CFG_NET_P2                  0x01  //�����������
#define CMD_SAMPLE_CTL_P2               0x02  //��ʼ��ֹͣ�ɼ�
#define CMD_MAGNIFICATION_P2            0x03  //�Ŵ���
#define CMD_CHANGE_ID_P2                0x04  //����ID
#define CMD_THRESHOLD_P2                0x06  //���ü������
#define CMD_GPS_VALID                   0x07  //GPS�Ƿ���Ч
#define CMD_GET_LONLAT                  0x08  //��ȡ��γ��
#define CMD_RESTART                     0x09  //����
#define CMD_GET_ENERGY                  0x0A  //��ȡ27kHz����
#define CMD_GET_AUDIO                   0x0B  //��ȡ��Ƶ�ź�
#define CMD_SET_FREQ                    0x0C  //���ü��Ƶ��
#define CMD_GET_PARA                    0x0D  //��ȡ�Ŵ����������ֵ��Ƶ��
#define CMD_RATIO_PARA                  0x0E  //����ֵ����ֵ
#define CMD_RESET_P2                    0xFF  //��������
#define CMD_PARAM_VALVE_START_P2        0xAA
#define CMD_PARAM_VALVE_STOP_P2         0xBB
#define CMD_PARAM_VALUE_START_P2_EMIT_SIGNAL   0XCC  // ������⵽�����ź�

#define PROCESS_ARRIVAL_TIME             0X0    //���ڹ��������͵���ʱ��
#define PROCESS_SIGNAL                   0X1    //���ڹ����������ź�

#define FS_20K          0x14
#define FS_48K          0x30
#define FS_125K         0x7d
#define FS_80K          0x50
#define FS_852500       0x55
// *************   protocol V2   end    ******************** //
/* process result.*/
typedef enum
{
	CMD_CPT_OK       = 0,
	CMD_CPT_ERR      = 1,
	CMD_FMT_ERR      = 2,
	CMD_TYPE_ERR     = 3,
	CMD_PARAM_ERR    = 4,
	CMD_STORE_ERR    = 5,
	CMD_SYSTEM_STAUS = 6,
	CMD_LENGTH_ERR = 7,
	CMD_CRC_ERR    = 8,
  CMD_OVERTIME_ERR = 9,
  CMD_GPS_ERROR = 10
}CmdStatus;
 
/* Exported typedef ----------------------------------------------------------*/
typedef struct
{
	uint8_t  paramIP[4];
	uint8_t  paramPort[2];
	uint8_t  paramGW[4];
	uint8_t  paramMAC[4];
	uint8_t  paramMask[4];
}DevConfigDef;

typedef struct
{
	uint8_t  paramIP[4];
	uint8_t  paramPort[2];
	uint8_t  paramGW[4];
	uint8_t  paramMask[4];
}DevConfigDef_P2;

typedef struct
{
	uint8_t  paramValve;
	uint8_t  reserv[15];
}DevSwitchDef;

typedef struct
{
	uint8_t  paramMAC[4];
	uint8_t  reserv[12];
}MacConfigDef;

typedef union
{
	DevConfigDef    devConfigMsg;
	DevConfigDef_P2 devConfigMsg_P2;
	MacConfigDef    MacConfigMsg;
	DevSwitchDef    devSwitch;
	uint8_t         freqTabNbr;
	uint8_t         reserv[16];
}CmdParamDef;

typedef struct
{
	uint8_t fmtHeader;  //��ʾ��Ϣ��ʼ
	uint8_t fmtLength;  //��ʾ��Ϣ����
	uint8_t fmtType;  //��Ϣ���ͣ���ʾ��ͬ����������
	CmdParamDef fmtParameter;  //���ݲ�ͬ�������ͣ��洢��ͬ�������豸���á�MAC������״̬�ȣ�
}CmdFormatDef;

typedef union
{
	CmdFormatDef cmdFormat;  //�������ݽṹ
	uint8_t cmdBuffer[CMD_BUF_LEN];  //CMD_BUF_LEN=255����������Ϊ�ֽ���
}CmdTypeDef;  //P1Э��

typedef struct
{
	uint8_t recv_len;
	uint8_t isDisconnectSocket;
	uint8_t isResponseClient;
	uint8_t isLaunchCS5340;  //cs5340Ϊ��Ƶ�ɼ�оƬ
	uint8_t isReplyInfo;
	uint8_t isReplyFirm;
	uint8_t currentStatusSys;
	uint8_t romBuffer[MAX_ROM_SIZE];  //MAX_ROM_SIZE==255�����IP��MAC�����룬�����ʱ�ȵ�
	uint8_t firmversion[FIRM_EDITION_LEN];  //FIRM_EDITION_LEN=31
}PtlTypeDef;

// *************   protocol V2   start    ******************** //


typedef struct
{
	uint16_t fmtHeader_P2;
	uint8_t fmtLength_P2;
  uint8_t fmtDeviceID;  //�豸ID
	uint8_t fmtType_P2;
	CmdParamDef fmtParameter_P2;
}CmdFormatDef_P2;

typedef union
{
	CmdFormatDef_P2 cmdFormat_P2;
	uint8_t cmdBuffer_P2[CMD_BUF_LEN];
}CmdTypeDef_P2;
// *************   protocol V2   end    ******************** //

/* Exported types ------------------------------------------------------------*/
typedef void (* Funcptr)(void);

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ExecuteAPP(addr)                             \
do{                                                  \
	__set_MSP(*(uint32_t *)(addr));                    \
	((Funcptr)(*(uint32_t *)(addr+4)))();              \
}while(0u)

/* Exported functions ------------------------------------------------------- */
void Protocol_Init(PtlTypeDef *ptl, CmdTypeDef *cmd);
uint32_t ParseCmd(PtlTypeDef *ptl, CmdTypeDef *cmd);
uint32_t ParseCmd_P2(PtlTypeDef *ptl, CmdTypeDef_P2 *cmd);
uint32_t ParseCmd_P2_Start(PtlTypeDef *ptl, CmdTypeDef_P2 *cmd);
uint32_t Read4Byte(uint32_t addr);

uint16_t GetCRC16(uint8_t *inPtr, unsigned int len);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
