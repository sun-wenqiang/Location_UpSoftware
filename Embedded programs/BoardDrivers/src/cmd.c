#include "cmd.h"
#include "adc.h"
#include "dac.h"
#include "gain.h"
#include "uart.h"
#include <string.h>
#include "signal_process.h"


#define VERINFO_ADDR_BASE   (0x08050000) //??FLASH???
const uint8_t Software_Ver_1 __attribute__((at(VERINFO_ADDR_BASE + 0x00)))  = 0x02;
const uint8_t Software_Ver_2 __attribute__((at(VERINFO_ADDR_BASE + 0x01)))  = 0x02;
const uint8_t Software_Ver_3 __attribute__((at(VERINFO_ADDR_BASE + 0x02)))  = 0x0C;
const char Software_Ver[] __attribute__((at(VERINFO_ADDR_BASE + 0x03)))  = "V03.01.01" __DATE__ __TIME__;
extern wiz_NetInfo  NetConf_reset;
extern uint8_t start_flag;
uint16_t threshold = 50;
int send_point = 0;
extern PtlTypeDef protocol;
extern uint16_t freq_detect;
uint8_t para_temp[2]={0, 0};
extern para_signal_process Para;
uint8_t process_state = 0;

////////////////// ADD CRC FOR IP CONFIG 20220414 ///////////////////////
//unsigned int GetCRC16(unsigned char *inPtr, unsigned int len);

/////////////////////////////////////////////////////////////////////	

static void ClearCmdBuffer(CmdTypeDef *cmd)
{
	uint32_t cmd_index = 0;
	while(cmd_index < CMD_BUF_LEN)
	{
		cmd->cmdBuffer[cmd_index] = 0;
		cmd_index++;
	}
}

static void BytesMemeryCopy(uint8_t len, uint8_t src_addr[], uint8_t net_conf_addr[])  //���ȡ�Դ��ַ��Ŀ���ַ
{
	uint8_t msg_i;
	for(msg_i = 0; msg_i< len; msg_i++)
	{
		net_conf_addr[msg_i] = src_addr[msg_i];
	}
}

void Protocol_Init(PtlTypeDef *ptl, CmdTypeDef *cmd)  //PtlTypeDefΪͨ��Э��ṹ��
{
	uint8_t ii = 0;
	for(ii=0; ii<FIRM_EDITION_LEN; ii=ii+1)  //FIRM_EDITION_LEN=31
	{
		ptl->firmversion[ii] = Read4Byte(VERINFO_ADDR_BASE + ii);  //ֻ���ֵ��8λ������ᱻ�ض�
	}
	
	ReadInfo(0x00,ptl->romBuffer,STORE_PARA_TOTAL);  //��0x00��ʼ��ȡ����ΪSTORE_PARA_TOTAL��28�������ݣ��浽ptl->romBuffer�У���STORE_PARA_TOTAL���ֽڣ��պ���EEPROMʹ�ñ��е�����
	BytesMemeryCopy(4, &(ptl->romBuffer[ ROM_SEVER_IP ]), NetConf.ip);  //��ptl->romBuffer[ ROM_SEVER_IP ]�����ĸ��ֽ����ݿ�����NetConf.ip��

	/* configure port.*/
	LocalPort = *(uint16_t *)(&(ptl->romBuffer[ ROM_SEVER_PORT ]));
	
	/* configure gw.*/
	BytesMemeryCopy(4, &(ptl->romBuffer[ ROM_SEVER_GW ]), NetConf.gw);
	
	/* configure mac.*/
	BytesMemeryCopy(6, &(ptl->romBuffer[ ROM_SEVER_MAC ]), NetConf.mac);
	
		/* configure mask.*/
	BytesMemeryCopy(4, &(ptl->romBuffer[ ROM_SEVER_MASK ]), NetConf.sn);
	
	cmd->cmdFormat.fmtHeader = CMD_HEADER;  //CMD_HEADER=0xaa
	ptl->isDisconnectSocket = CMD_DISCONNECT_N;
	ptl->isResponseClient = CMD_RESPONSE_N;
	ptl->isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
	ptl->currentStatusSys = CMD_STATUS_READY;
	ClearCmdBuffer(cmd);  //ȫ������
}  

static uint8_t BufCmp(uint8_t * buf, uint8_t dat, uint8_t l)
{
	uint8_t buf_ci;
	
	for(buf_ci = 0; buf_ci < l; buf_ci++)
	{
		if(*(buf + buf_ci) != dat)
		{
			break;
		}
	}
	return buf_ci;
}

uint32_t ParseCmd(PtlTypeDef *ptl, CmdTypeDef *cmd)
{
	uint8_t parse_temp = 0;
	uint16_t port_temp;
	uint32_t ret;
	
	if(cmd->cmdFormat.fmtHeader != CMD_HEADER)
	{
		ptl->isResponseClient = CMD_RESPONSE_Y;
		return CMD_FMT_ERR;
	}
	
	
	//////////////ADD CRC FOR IP CONFIG 20220414//////////////////////
	if(cmd->cmdFormat.fmtType == CMD_DEVICE_CONF)
	{
		if(cmd->cmdFormat.fmtLength != ptl->recv_len)
		{
			ptl->isResponseClient = CMD_RESPONSE_Y;
			return CMD_LENGTH_ERR;
		}
		else
		{
			uint16_t targetCrc = GetCRC16((unsigned char *)((char*)(&cmd->cmdBuffer)+1),(ptl->recv_len-1-2));
			uint16_t crcIndex = cmd->cmdFormat.fmtLength-2;
			char* msgBodyPtr = (char*)(&cmd->cmdBuffer);
			uint16_t currentCrc = *(uint16_t*)(msgBodyPtr + crcIndex);
			if(targetCrc != currentCrc)
			{
				ptl->isResponseClient = CMD_RESPONSE_Y;
				return  CMD_CRC_ERR;
//				cmd->cmdFormat.fmtType = CMD_NONE;
			}
			else
			{;}
			ret =  CMD_CPT_OK;
		}	
	}
	else
	{
		if(ptl->recv_len != cmd->cmdFormat.fmtLength)
		{
			return CMD_FMT_ERR;
		}
		
		ret =  CMD_CPT_OK;
	}
	////////////////////////////////////////////////////////////////////
	
	
	switch(cmd->cmdFormat.fmtType)
	{
		/****************************** configure device's network information **********************/
		case(CMD_DEVICE_CONF):
		  /* configure ip.*/
		  BytesMemeryCopy(4, cmd->cmdFormat.fmtParameter.devConfigMsg.paramIP, &(ptl->romBuffer[ROM_SEVER_IP]));
			
		  /* configure port.*/
		  port_temp = *(uint16_t *)(cmd->cmdFormat.fmtParameter.devConfigMsg.paramPort);
		  if(port_temp != 0)
		  {
			  ptl->romBuffer[ ROM_SEVER_PORT+0 ] =  cmd->cmdFormat.fmtParameter.devConfigMsg.paramPort[0];
			  ptl->romBuffer[ ROM_SEVER_PORT+1 ] =  cmd->cmdFormat.fmtParameter.devConfigMsg.paramPort[1];			 
		  }

			/* configure gw.*/
      BytesMemeryCopy(4, cmd->cmdFormat.fmtParameter.devConfigMsg.paramGW, &(ptl->romBuffer[ROM_SEVER_GW]));
			
			/* configure mac.*/
			if(BufCmp(&(cmd->cmdBuffer[13]), 0, 6) != 6)
			{
        BytesMemeryCopy(6, cmd->cmdFormat.fmtParameter.devConfigMsg.paramMAC, &(ptl->romBuffer[ROM_SEVER_MAC]));				
			}
			
			/* configure mask.*/
      BytesMemeryCopy(4, cmd->cmdFormat.fmtParameter.devConfigMsg.paramMask, &(ptl->romBuffer[ROM_SEVER_MASK]));			
			
			ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
			ptl->isResponseClient = CMD_RESPONSE_Y;
			if(ptl->currentStatusSys == CMD_STATUS_STARTED)
			{
				ptl->isResponseClient = CMD_RESPONSE_N;
			}
			
			if(WriteInfo(0x00,ptl->romBuffer,STORE_PARA_TOTAL) == AbNormal)
			{
				/* store err.*/
				ret =  CMD_STORE_ERR;
			}
			ptl->isReplyInfo = CMD_REPLY_INFO_N;
			ptl->isReplyFirm = CMD_REPLY_INFO_N;
			break;
		
		/************************************* start or stop daq ***********************************/
		case(CMD_DEVICE_SWITCH):
			parse_temp = cmd->cmdFormat.fmtParameter.devSwitch.paramValve;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
			if(parse_temp == CMD_PARAM_VALVE_START)
			{
				if(ptl->currentStatusSys != CMD_STATUS_STARTED)
				{
					ptl->currentStatusSys = CMD_STATUS_STARTED;
					ptl->isResponseClient = CMD_RESPONSE_N;
					AudioStart();
				}
				else
				{
					ptl->isResponseClient = CMD_RESPONSE_N;
				}
			}
			else if(parse_temp == CMD_PARAM_VALVE_STOP)
			{
				if(ptl->currentStatusSys == CMD_STATUS_STARTED)
				{
					AudioStop();
				}
				ptl->isResponseClient = CMD_RESPONSE_Y;
				ptl->currentStatusSys = CMD_STATUS_STOPED;
			}
			else
			{
				ret =  CMD_PARAM_ERR;
			}
			break;
			
		/************************************* setting sample frequency ****************************/	
		case(CMD_DEVICE_SETTING):
			ptl->romBuffer[ ROM_SAMPLE_FREQ_TAB ] = cmd->cmdFormat.fmtParameter.freqTabNbr;
			ptl->isResponseClient = CMD_RESPONSE_Y;
			ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
		
			if(CheckAT24C02(ROM_SAMPLE_FREQ_TAB, ptl->romBuffer[ ROM_SAMPLE_FREQ_TAB ]) == AbNormal)
			{
				/* store err.*/
				ret = CMD_STORE_ERR;
			}
			break;
			
		/************************************* obtain rom info ************************************/
		case CMD_REPLY_ROM_INFO:
			ptl->isResponseClient = CMD_RESPONSE_Y;
		  ptl->isReplyInfo = CMD_REPLY_INFO_Y;
			ptl->isReplyFirm = CMD_REPLY_FIRM_N;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
			break;

		/************************************* obtain system status *******************************/
		case CMD_DEVICE_STATUS:
			ptl->isResponseClient = CMD_RESPONSE_Y;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
		  ret = CMD_SYSTEM_STAUS;
		  break;

		/************************************* application upgrad ********************************/
		case (CMD_DEVICE_UPGRAD):
			ptl->isResponseClient = CMD_RESPONSE_Y;
			if(CheckAT24C02(ROM_UPGRAD_FLAG, CMD_PERMIT_APP_UPGRAD) == AbNormal)
			{
				/* store err.*/
				ret = CMD_STORE_ERR;
			}
		  ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
		  break;
		/************************************* obtain Firmware edition  **************************************/	
		case (CMD_REPLY_FIRMWARE_EDITION):
			ptl->isResponseClient = CMD_RESPONSE_Y;
			ptl->isReplyInfo = CMD_REPLY_INFO_N;
		  ptl->isReplyFirm = CMD_REPLY_FIRM_Y;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
		  break;			
		/************************************* command error **************************************/
		default:
			ptl->isResponseClient = CMD_RESPONSE_N;
			ptl->isReplyInfo = CMD_REPLY_INFO_N;
		  ptl->isReplyFirm = CMD_REPLY_FIRM_N;			
			ret = CMD_TYPE_ERR;
		  break;
	}
	return ret;
}


// *************   protocol V2   start    ******************** //
uint32_t ParseCmd_P2(PtlTypeDef *ptl, CmdTypeDef_P2 *cmd)
{
  uint8_t ID=ptl->romBuffer[ROM_ID];  
  uint8_t ID_temp = cmd->cmdFormat_P2.fmtDeviceID;
  if (ID==ID_temp)
  {
    uint8_t parse_temp = 0;  //��ʱ���ָ������
    uint32_t ret;  //Э�鷵��ֵ
    uint16_t port_temp;  //��ʱ��Ŷ˿�
    uint8_t fs;
    // length
    if(ptl->recv_len != cmd->cmdFormat_P2.fmtLength_P2)
    {
      ptl->isResponseClient = CMD_RESPONSE_Y;
      cmd->cmdFormat_P2.fmtLength_P2 = 8;  //��ID��������8�ֽ�
      cmd->cmdBuffer_P2[5] = CMD_LENGTH_ERR;  //���ID֮��������Ҫ����һ���ֽ�
      return CMD_LENGTH_ERR;
    }
    // CRC	
    uint16_t targetCrc = GetCRC16((unsigned char *)((char*)(&cmd->cmdFormat_P2)),(ptl->recv_len-2));
    uint16_t crcIndex = cmd->cmdFormat_P2.fmtLength_P2-2;
    char* msgBodyPtr = (char*)(&cmd->cmdBuffer_P2);  //ָ��cmdbuffer
    uint16_t currentCrc = *(uint16_t*)(msgBodyPtr + crcIndex);  //���ֽ�
    if(targetCrc != currentCrc)
    {
      ptl->isResponseClient = CMD_RESPONSE_Y;
      cmd->cmdFormat_P2.fmtLength_P2 = 8;
      cmd->cmdBuffer_P2[5] = CMD_CRC_ERR;
      return  CMD_CRC_ERR;
    }			
    // right protocol
    ret =  CMD_CPT_OK;	
        
    /*��ʼ����ָ���ִ�в���*/
    switch(cmd->cmdFormat_P2.fmtType_P2)
    {
      /****************************** obtain Firmware edition    **********************/
      case(CMD_GET_VOLTAGE_P2):  //��ȡ��ѹ
        ptl->isResponseClient = CMD_RESPONSE_Y;  //�ظ��ͻ���
        ADC_Result voltage = Get_Adc_Average(5);  //ͨ��0�����ƽ��ֵ
        if (voltage.status!=0)
        {
          cmd->cmdFormat_P2.fmtLength_P2 = 0x09;   //�ظ����ݳ���Ϊ8
          memcpy( (char *)&(cmd->cmdFormat_P2.fmtParameter_P2), (char *)&(voltage), 2);
        }
        else
        {
          cmd->cmdFormat_P2.fmtLength_P2 = 0x08;
          cmd->cmdBuffer_P2[5]=CMD_OVERTIME_ERR;
        }
        break;
      
      /************************************* configure device's network information ***********************************/
      case(CMD_CFG_NET_P2):  //�����������
        /* configure ip.*/
        BytesMemeryCopy(4, cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramIP, &(ptl->romBuffer[ROM_SEVER_IP]));
        
        /* configure port.*/
        port_temp = *(uint16_t *)(cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramPort);
        if(port_temp != 0)
        {
          ptl->romBuffer[ ROM_SEVER_PORT+0 ] =  cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramPort[0];
          ptl->romBuffer[ ROM_SEVER_PORT+1 ] =  cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramPort[1];			 
        }

        /* configure gw.*/
        BytesMemeryCopy(4, cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramGW, &(ptl->romBuffer[ROM_SEVER_GW]));
        
        /* configure mask.*/
        BytesMemeryCopy(4, cmd->cmdFormat_P2.fmtParameter_P2.devConfigMsg_P2.paramMask, &(ptl->romBuffer[ROM_SEVER_MASK]));			
        
        ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        ptl->isResponseClient = CMD_RESPONSE_Y;
        if(ptl->currentStatusSys == CMD_STATUS_STARTED)
        {
          ptl->isResponseClient = CMD_RESPONSE_N;
        }
        
        if(WriteInfo(0x00,ptl->romBuffer,STORE_PARA_TOTAL) == AbNormal)
        {
          /* store err.*/
          ret =  CMD_STORE_ERR;
          ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
        
      /************************************* start or stop daq  ****************************/	
      case(CMD_SAMPLE_CTL_P2):  //��ʼ/ֹͣ�ɼ�
        ptl->isResponseClient = CMD_RESPONSE_Y; // default to reply
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (cmd->cmdFormat_P2.fmtLength_P2 == 8)
        {
          parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ�һ���ֽڣ�AA/BB����ʼ/ֹͣ��
          if(parse_temp == CMD_PARAM_VALVE_START_P2 && validStatus==1)   //0xAA����GPS��λ���ſ��Կ���
          {
            if(ptl->currentStatusSys != CMD_PARAM_VALVE_START_P2)  //�����ϴ���״̬
            {
              ptl->currentStatusSys = CMD_PARAM_VALVE_START_P2;
              AudioStart();
              process_state = PROCESS_ARRIVAL_TIME;
            }
            ret = CMD_CPT_OK;
          }
          else if(parse_temp == CMD_PARAM_VALVE_START_P2  && validStatus==0)   //0xAA����GPSû�ж�λ���򷵻ض�Ӧ��ֵ
          {
            ret = CMD_GPS_ERROR;  //GPSû�ж�λ��
          }
          else if(parse_temp == CMD_PARAM_VALVE_STOP_P2)  //0xBB
          {
            if(ptl->currentStatusSys == CMD_PARAM_VALVE_START_P2)
            {
              AudioStop();
            }
            ptl->currentStatusSys = CMD_PARAM_VALVE_STOP_P2; // if already stop, just stop
            ret = CMD_CPT_OK;
          }
          else
          {
            ret =  CMD_PARAM_ERR;   //ָ�������д�
          }
        } //cmd->cmdFormat_P2.fmtLength_P2 == 8
        else
        {
          para_temp[0] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve; //ָ�����ݣ���ȷΪCC
          para_temp[1] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.reserv[0];
          if(para_temp[0] == CMD_PARAM_VALUE_START_P2_EMIT_SIGNAL && validStatus==1)   //0xCC����GPS��λ���ſ��Կ���
          {
            if(ptl->currentStatusSys != CMD_PARAM_VALVE_START_P2)  //�����ϴ���״̬
            {
              ptl->currentStatusSys = CMD_PARAM_VALVE_START_P2;
              AudioStart();
              process_state = PROCESS_SIGNAL;
              if (para_temp[1]>30)
              {
                para_temp[1]=30;
              }
              else if(para_temp[1]<3)
              {
                para_temp[1]=3;
              }
              Timer3_Start(para_temp[1]);
            }
            ret = CMD_CPT_OK;
          }
          else if(para_temp[0] == CMD_PARAM_VALUE_START_P2_EMIT_SIGNAL && validStatus==0)   //0xCC����GPSû�ж�λ���򷵻ض�Ӧ��ֵ
          {
            ret = CMD_GPS_ERROR;  //GPSû�ж�λ��
          }
          else
          {
            ret =  CMD_PARAM_ERR;   //ָ�������д�
          }
        }//cmd->cmdFormat_P2.fmtLength_P2 == 9
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;

        
      /*���÷Ŵ���*/
      case CMD_MAGNIFICATION_P2:  
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ����Ŵ���
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if  (parse_temp<0 || parse_temp>7)
        {
          ret = CMD_PARAM_ERR;
        }
        else if(WriteInfo(ROM_GAIN,&(parse_temp),1) == AbNormal)  //WriteInfo(uint8_t Ŀ���ַ��uint8_t * Դ��ַ��uint8_t д���ֽ���)
        {
            /* store err.*/
            ret =  CMD_STORE_ERR;
        }
        else
        {
          ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        }
        cmd->cmdBuffer_P2[5] = ret;          //MCU->PCָ���еķ���ֵ
        cmd->cmdFormat_P2.fmtLength_P2 = 8;  //����ָ�8�ֽ�
        break;
      
      /*����ID*/
      case CMD_CHANGE_ID_P2:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ��豸��ID
        ptl->isResponseClient = CMD_RESPONSE_Y;
        if(WriteInfo(ROM_ID,&(parse_temp),1) == AbNormal)  //WriteInfo(uint8_t Ŀ���ַ��uint8_t * Դ��ַ��uint8_t д���ֽ���)
        {
          /* store err.*/
          ret =  CMD_STORE_ERR;
          ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        }
        else{
        ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
      
      /*���ü����ֵ*/
      case CMD_THRESHOLD_P2:
        para_temp[0] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve; //ָ�����ݣ�Ƶ�ʣ�ǰ��Ϊ�Ͱ�λ������Ϊ�߰�λ
        para_temp[1] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.reserv[0];
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if(WriteInfo(ROM_THRESHOLD,para_temp,2) == AbNormal)  //WriteInfo(uint8_t Ŀ���ַ��uint8_t * Դ��ַ��uint8_t д���ֽ���)
        {
          /* store err.*/
          ret =  CMD_STORE_ERR;
        }
        else{
          ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
        
        /*���GPS�Ƿ���Ч*/
      case CMD_GPS_VALID:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (parse_temp != 0x00)
        {
          ret =  CMD_PARAM_ERR;
        }
        else
        {
          if (validStatus==0)
          {
            ret=CMD_GPS_ERROR;
          }
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
        
       /*��ȡ��γ��*/
      case CMD_GET_LONLAT:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (parse_temp != 0x00)
        {
          ret =  CMD_PARAM_ERR;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        else
        {
          ptl->isResponseClient = CMD_RESPONSE_N;
          if (validStatus==0)   //GPS��Ч����ʱ��;�γ��ȫΪ0
          {
            GPS_message.Lat = 0.0f;
            GPS_message.Long = 0.0f;
            GPS_message.PPS_time[0] = 0;
            GPS_message.PPS_time[1] = 0;
            GPS_message.PPS_time[2] = 0;
          }
          else                   //GPS��Ч
          {
            GPS_message.Lat = Latitude;
            GPS_message.Long = Longitude;
            GPS_message.PPS_time[0] = Time[0];
            GPS_message.PPS_time[1] = Time[1];
            GPS_message.PPS_time[2] = Time[2];
          }
          GPS_message.crc = GetCRC16((uint8_t *)GPS_message.header, GPS_message.len-2);
          if(send(SOCKET_N, GPS_message.header, GPS_message.len)!=GPS_message.len) {};
        }
        break;
        
        /*�����豸*/
      case CMD_RESTART:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (parse_temp != 0x00)
        {
          ret =  CMD_PARAM_ERR;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        else
        {
          ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        break;
        
      /*��ȡ27k����*/
      case CMD_GET_ENERGY:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (parse_temp != 0x00)
        {
          ret =  CMD_PARAM_ERR;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        else
        {
          ptl->isResponseClient = CMD_RESPONSE_N;
          Energy.energy = energy_27k;
          Energy.energy_3 = energy_;
          Energy.crc = GetCRC16((uint8_t *)Energy.header, Energy.len-2);
          if(send(SOCKET_N, Energy.header, Energy.len)!=Energy.len) {};
        }
        break;
        
      /*��ȡԭʼ����*/
      case CMD_GET_AUDIO:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (protocol.isLaunchCS5340 == CMD_LAUNCH_CS5340_Y)
        {
          ret =  CMD_PARAM_ERR;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        else
        {
          send_point = parse_temp*85250;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        break;
        
        /*���ü��Ƶ��*/
      case CMD_SET_FREQ:
        para_temp[0] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve; //ָ�����ݣ�Ƶ�ʣ�ǰ��Ϊ�Ͱ�λ������Ϊ�߰�λ
        para_temp[1] = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.reserv[0];
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if(WriteInfo(ROM_FREQ,para_temp,2) == AbNormal)  //WriteInfo(uint8_t Ŀ���ַ��uint8_t * Դ��ַ��uint8_t д���ֽ���)
        {
          /* store err.*/
          ret =  CMD_STORE_ERR;
        }
        else{
          ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
        
         /*��ȡ����*/
      case CMD_GET_PARA:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ���ȷΪ0
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        if (parse_temp != 0x00)
        {
          ret =  CMD_PARAM_ERR;
          cmd->cmdFormat_P2.fmtLength_P2 = 8;
          cmd->cmdBuffer_P2[5] = ret;
        }
        else
        {
          ptl->isResponseClient = CMD_RESPONSE_N;
          Para.crc = GetCRC16((uint8_t *)Para.header, Para.len-2);
          if(send(SOCKET_N, Para.header, Para.len)!=Para.len) {};
        }
        break;
        
        
        /*���ü���ֵ*/
      case CMD_RATIO_PARA:
        parse_temp = cmd->cmdFormat_P2.fmtParameter_P2.devSwitch.paramValve;  //ָ�����ݣ������ֵ�ı�ֵ
        ptl->isResponseClient = CMD_RESPONSE_Y;
        if(WriteInfo(ROM_RATIO,&(parse_temp),1) == AbNormal)  //WriteInfo(uint8_t Ŀ���ַ��uint8_t * Դ��ַ��uint8_t д���ֽ���)
        {
          /* store err.*/
          ret =  CMD_STORE_ERR;
          ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        }
        else{
          ptl->isDisconnectSocket = CMD_DISCONNECT_Y;
        }
        cmd->cmdFormat_P2.fmtLength_P2 = 8;
        cmd->cmdBuffer_P2[5] = ret;
        break;
      
      /************************************* command error **************************************/
      default:
        ptl->isResponseClient = CMD_RESPONSE_Y;
        ptl->isDisconnectSocket = CMD_DISCONNECT_N;
        cmd->cmdFormat_P2.fmtLength_P2 = 0x08;
        cmd->cmdBuffer_P2[5] = CMD_TYPE_ERR;  //�����ڵ�ָ��ţ���Ӧ03
        break;
    }		
  }
  else
  {
    ptl->isResponseClient = CMD_RESPONSE_N;
    ptl->isDisconnectSocket = CMD_DISCONNECT_N;
  }	
}



uint32_t ParseCmd_P2_Start(PtlTypeDef *ptl, CmdTypeDef_P2 *cmd)  //�����ʹ�����յ���P2���͵�����
{
	uint8_t parse_temp = 0;
	uint32_t ret;
	// length
	if(ptl->recv_len != cmd->cmdFormat_P2.fmtLength_P2)
	{
		ptl->isResponseClient = CMD_RESPONSE_Y;
		cmd->cmdFormat_P2.fmtLength_P2 = 7;
		cmd->cmdBuffer_P2[4] = CMD_LENGTH_ERR;
		return CMD_LENGTH_ERR;
	}
	else
	{;}
	// CRC	
	uint16_t targetCrc = GetCRC16((unsigned char *)((char*)(&cmd->cmdFormat_P2)),(ptl->recv_len-2));
	uint16_t crcIndex = cmd->cmdFormat_P2.fmtLength_P2-2;
	char* msgBodyPtr = (char*)(&cmd->cmdBuffer_P2);
	uint16_t currentCrc = *(uint16_t*)(msgBodyPtr + crcIndex);
	if(targetCrc != currentCrc)
	{
		ptl->isResponseClient = CMD_RESPONSE_Y;
		cmd->cmdFormat_P2.fmtLength_P2 = 7;
		cmd->cmdBuffer_P2[4] = CMD_CRC_ERR;
		return  CMD_CRC_ERR;
	}
	else
	{;}				
	// right protocol
	ret =  CMD_CPT_OK;	
			
	switch(cmd->cmdFormat_P2.fmtType_P2)
	{
		/****************************** reset net config **********************/
		case(CMD_RESET_P2):
			ptl->isResponseClient = CMD_RESPONSE_Y;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
		
      if(WriteInfo(ROM_SEVER_IP,  NetConf_reset.ip,4) == AbNormal)
			{
				ret =  CMD_STORE_ERR;
			}
			if(WriteInfo(ROM_SEVER_GW,  NetConf_reset.gw,4) == AbNormal)
			{
				ret =  CMD_STORE_ERR;
			}
			uint16_t LocalPort = 5000;  //port
			if(WriteInfo(ROM_SEVER_PORT, (uint8_t *)&LocalPort, 2) == AbNormal)
			{
				ret =  CMD_STORE_ERR;
			}
		  if(WriteInfo(ROM_SEVER_MASK,NetConf_reset.sn,4) == AbNormal)
			{
				ret =  CMD_STORE_ERR;
			}
			start_flag = 1;
			break;		
		/************************************* command error **************************************/
		default:
			ptl->isResponseClient = CMD_RESPONSE_Y;
		  ptl->isDisconnectSocket = CMD_DISCONNECT_N;
		  cmd->cmdFormat_P2.fmtLength_P2 = 0x07;
			cmd->cmdBuffer_P2[4] = CMD_TYPE_ERR;
		  break;
	}			
}
// *************   protocol V2   end    ******************** //
uint32_t Read4Byte(uint32_t addr)
{
	return *(volatile uint32_t *)addr;
}



uint16_t GetCRC16(uint8_t *inPtr, unsigned int len)
{
  uint16_t crc = 0xffff;
  uint8_t index;
  while (len > 0) {
    crc ^= *inPtr;
    for (index = 0; index < 8; index++) {
      if ((crc & 0x0001) == 0)
        crc = crc >> 1;
      else {
        crc = crc >> 1;
        crc ^= 0xa001;
      }
    }
    len -= 1;
    inPtr++;
  }
  return ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
}