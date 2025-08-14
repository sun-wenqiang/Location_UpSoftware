/**
  ******************************************************************************
  * @file    User/main.c 
  * @author  ***
  * @version V1.0.0
  * @date    2020-3-25
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__align(4) AudioTypeDef pcmStream;  //__align(4)����ǿ�ƽ�pcmStream���ڴ��а����ֽڶ��뷽ʽ�洢����������������òɼ�оƬcs5340������������/��������/��˷������ȵ�
extern char httpRecvBuffer[HTTP_RX_BUFEER_SIZE];  //HTTP_RX_BUFEER_SIZE=2048
extern char httpTransBuffer[HTTP_TX_BUFEER_SIZE];  //HTTP_TX_BUFEER_SIZE=2048
extern ModifyFsDef fsReferList[maxfs];  //��������������Ƶ������
__align(4) CmdTypeDef    command;  //��������
CmdTypeDef_P2 command_P2;
PtlTypeDef protocol;  //����Э��
uint16_t cmd_rxlen;  //W5500�Ѿ����պͱ�������ݴ�С
uint8_t cmd_status;
uint8_t socketStatus;
uint32_t ledNetBaseTime;
uint8_t ledNetSwitch;
uint8_t fs_index;
uint8_t start_flag  = 0;  //ȫ��������
extern uint16_t LocalPort ;
extern uint8_t socket_reset_flag;
extern para_signal_process Para;
extern uint8_t C;
extern uint8_t process_state;

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Administrate(void);  //ֻ�����ڱ�������ļ��б�����
static void reset_collect(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
  
uint32_t iii=0;  //ȡ����ͨ���������õ��Ĳ���
int jjj=0;       //ȡ����ͨ���������õ��Ĳ���
size_t element_count[2] = {buffer1_point_num, buffer2_point_num};   //ǰһ��buffer�ͺ�һ��buffer��ch2�Ĳ��������
uint8_t address_offset[2]={2,0};
uint8_t storage=1;
uint8_t output=0;
uint8_t find_27k=0;
float DataOut[buffer2_point_num]={0};   //�������pcmתΪfloat����֮����ź�
float data_arrival[buffer_point_num] = {0};  //1705�����ݣ��������Ѱ�ҵ���ʱ�������
size_t first_length = 0;            // ��һ�μ�⵽��buffer����
uint16_t freq_detect = 27000;
static uint16_t socketWatchdog = 0;
static uint32_t end_num = 0;
uint8_t detect_ratio = 0;
int main(void)
{
	/* move vector table.*/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x20000);  //�����ж��������ַ
	
	/* Cortex-Mx's NVIC administrate and management*/
	NVIC_Administrate();  //���� NVIC �ж����ȼ���ʹ��
	
	/* initialize delay function(Cortex-Mx's SysTick).*/
	Delay_Initialize();  //��ʱ��ʼ��
	
  /***********************************************************************************************/
  /***********************************************************************************************/
  /***********************************************************************************************/
  STM_EVAL_LEDInit(LED6);  //��ʼ��LED��GPIO�ڣ��ֱ�Ϊ�����ϵ�6543LED
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED3);

	STM_EVAL_LEDOn(LED6);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED3);
  
  
  GPIO_EXTI9_Init();
  UART3_Init();
  //DAC_Init_Config();  //��ʼ��DAC
  ADC_Init_Config();  //��ʼ��ADC
  GAIN_GPIO_Init();   //�Ŵ�����ʼ����Ĭ��Ϊ�Ŵ�10��    
  /***********************************************************************************************/
  /***********************************************************************************************/
  /***********************************************************************************************/
  
	/* initialize device at24c02.*/
	AT24C02_Init();  //��AT24C02��E2PROM��ͨ��I2Cͨ�ţ���һ����ʼ��I2C�����ţ�SCL/SDL��
	
	/* initialize protocol.*/
	Protocol_Init(&protocol, &command);  //Э���ʼ��������MAC��IP��
	
	/* initialize SPI1 for NetWork.*/
	sFLASH_Init();    //����SPI��MDA��Ϊ���ⲿ�������ͨ����׼��
	/* Reset W5500 before operate it.*/
	W5500_Reset();  //����W5500�ĸ�λ���Ų����и�λ
	/* Configure server's local information, for instance ip/gw/mac...*/
	W5500_Init();  //����W5500�ĸ�����Ϣ��IP/MAC/�շ�������/��ʱʱ��/����������
	
	
	switch(protocol.romBuffer[ROM_SAMPLE_FREQ_TAB])
	{
		case FS_20K:
			fs_index = 0;
			break;
		case FS_48K:
			fs_index = 1;
			break;
		case FS_125K:
			fs_index = 2;
			break;	
    case FS_80K:
			fs_index = 3;   //20240925���80K
			break;	
    case FS_852500:
      fs_index = 4;   //20241021���85.25K
      break;
		default:
			fs_index = 0;  // default 20k
			break;
	}
	/* Initialize CS5340.*/
	pcmStream.micNbr         = 2;
	pcmStream.sampleAccuracy = 24;
  pcmStream.sampleFreq     = fsReferList[fs_index].fsRealVaule;
	CS5340_Initialize(&pcmStream);

///////////////////  �����ȴ� ///////////////////////////////
	LocalPort = 5000; // port
	TIM_START_Config();
	start_flag = 0;	
	W5500_Reset();
	W5500_Init_start();
	
	LaunchWatchDog(IWDG_Prescaler_64, 5000); // (64/40'000)*625 = 1s,  (64/40'000)*5000 = 8s
	while (1)  // �����ȴ�ѭ��
	{
		IWDG_Feed();
		if(start_flag == 1)  
		{
			start_flag = 0;
			TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);  //����TIM2�Ķ�ʱ�������ж�
			break;
		}
		else if(start_flag == 2)  //������TIM2�Ķ�ʱ���ж�
		{
			start_flag = 0;
			/* Reset W5500 before operate it.*/
			W5500_Reset();
			/* Configure server's local information, for instance ip/gw/mac...*/
			W5500_Init();		
      TIM_Cmd(TIM2, DISABLE);  //����TIM2ûɶ�ã�ֱ�ӹص�
			break;
		}
		else if(start_flag == 0)
		{
			switch(getSn_SR(SOCKET_N))  //��ȡ״̬�Ĵ���
			{
				IWDG_Feed();
				case SOCK_ESTABLISHED :  //��״̬�¿���ʹ��SEND��RECV�������ݰ�����
					cmd_rxlen = getSn_RX_RSR(SOCKET_N);  //RX_RSR��ʾsocket_n���Ѿ����պͱ�������ݴ�С
					if(cmd_rxlen > 0)
					{
						memset(httpRecvBuffer, 0, HTTP_RX_BUFEER_SIZE);  //HTTP_RX_BUFEER_SIZE=2048����httpRecvBuffer�е�����ȫ������
						memset(httpTransBuffer, 0, HTTP_TX_BUFEER_SIZE);  //HTTP_TX_BUFEER_SIZE=2048����httpTransBuffer�е�����ȫ������
						cmd_rxlen = recv(SOCKET_N, (uint8_t *)httpRecvBuffer, cmd_rxlen);  //��socket_n�н��ղ��浽������
						if( !strncmp((const char *)httpRecvBuffer, (const char *)"GET", 3) || \
								!strncmp((const char *)httpRecvBuffer, (const char *)"POST", 4))  //����HTTP����
						{  
							protocol.currentStatusSys = CMD_STATUS_STOPED;
							protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
							httpParseAndResponse(SOCKET_N, cmd_rxlen);  //  ���������������ظ�HTTP����
						}//http request
						else  //159��if   ����HTTP����
						{
							IWDG_Feed();
							/* copy httpRecvBuffer to cmdBuffer.*/
							memcpy(command.cmdBuffer, httpRecvBuffer,CMD_BUF_LEN);  //�����յ�������(httpRecvBuffer)���Ƶ�cmdBuffer
							memcpy(command_P2.cmdBuffer_P2, httpRecvBuffer,CMD_BUF_LEN);
							protocol.recv_len = (uint8_t)cmd_rxlen;  //socket_n���Ѿ����պͱ�������ݴ�С
							TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);  //����TIM2��ʱ������
							if(command_P2.cmdFormat_P2.fmtHeader_P2 == CMD_HEADER_P2) //P2
							{
									cmd_status = ParseCmd_P2_Start(&protocol, &command_P2);  //�������command_P2
									
									if(protocol.isResponseClient != CMD_RESPONSE_N)  //��Ҫ��Ӧ�ͻ���
									{
										protocol.isResponseClient = CMD_RESPONSE_N;
										// calculate CRC
										uint16_t currentCrc = GetCRC16((unsigned char *)((char*)(&command_P2.cmdFormat_P2)),(command_P2.cmdFormat_P2.fmtLength_P2-2));
										memcpy((char*)&command_P2.cmdBuffer_P2[command_P2.cmdFormat_P2.fmtLength_P2-2], (char*)(&currentCrc), 2);
										// send whole protocol 
										if(send(SOCKET_N, command_P2.cmdBuffer_P2, command_P2.cmdFormat_P2.fmtLength_P2) == STORE_PARA_TOTAL)  //MCU��PC�Ļظ�
										{;}
										start_flag = 1;
										W5500_Reset();
										W5500_Init_reset();													
										Delay_ms(5);
									}//response client
							}  //����P2Э��
						}//cmd request  166��
					}//rxstrlen  154��  cmd_rxlen <= 0
					ledNetSwitch = 1;
					if(wizphy_getphylink() == PHY_LINK_OFF)  //�Ͽ�����
					{
						disconnect(SOCKET_N);
					}//link off
				break;
			
				case SOCK_CLOSE_WAIT :  //socket_n���յ����������ӶԷ������ĶϿ����ӵ��������״̬����Ȼ���Խ������ݴ���
					close(SOCKET_N);  //�ر�socket
				break;
			
				case SOCK_INIT :  //��ʾsocket�򿪲�����TCP����ģʽ
					listen(SOCKET_N);  //�ȴ��ͻ�����������
				break;
			
				case SOCK_CLOSED :  //ָʾsocket_n���ڹر�״̬����Դ���ͷ�
					ledNetSwitch = 0;
					socket(SOCKET_N, Sn_MR_TCP,LocalPort,SF_TCP_NODELAY|SF_IO_NONBLOCK);  //����һ���׽�������
				break;
			
				default:
				break;
			}//getSn_SR			
		}
		else  //start_flag==else
		{}
	}
  IWDG_Feed();
	Protocol_Init(&protocol, &command);
  data_stream.id=protocol.romBuffer[ROM_ID];   //�豸ID
  data_stream.crc = 0x0102;
  GPS_message.id=protocol.romBuffer[ROM_ID];   //�豸ID
  Energy.id = protocol.romBuffer[ROM_ID];   //�豸ID
  Set_Gain(protocol.romBuffer[ROM_GAIN]);     //���÷Ŵ���
  threshold = ((uint16_t)protocol.romBuffer[ROM_THRESHOLD + 1] << 8) | protocol.romBuffer[ROM_THRESHOLD];
  freq_detect = ((uint16_t)protocol.romBuffer[ROM_FREQ + 1] << 8) | protocol.romBuffer[ROM_FREQ];
  Para.id = data_stream.id;
  Para.frequency = freq_detect;
  Para.magnification = protocol.romBuffer[ROM_GAIN];
  Para.threshold = threshold;
  Para.ratio = protocol.romBuffer[ROM_RATIO];
  detect_ratio = Para.ratio;
  /* ************************����ѭ�� ************************/ 
  /* ************************����ѭ�� ************************/ 
  /* ************************����ѭ�� ************************/ 
  RCC_ClocksTypeDef clocks;
  RCC_GetClocksFreq(&clocks);
  TIM5_Init();
  while (1)
  {
		IWDG_Feed();
    /***************************************************/
		if(ledNetSwitch == 0)  //SOCK_CLOSED
		{
			ledNetBaseTime++;
			if(ledNetBaseTime == 0x0000ffff)
			{
				ledNetBaseTime = 0;
				STM_EVAL_LEDToggle(LED5);  //��ת��ƽ
			}//ledNetBaseTime
		}//ledNetSwitch
		else  //ledNetSwitch != 0   SOCK_ESTABLISHED
		{
			STM_EVAL_LEDOn(LED5);  //��ʾ��������ɹ�
		}
  /***************************************************/
 
		switch(getSn_SR(SOCKET_N))
		{
			IWDG_Feed();
			case SOCK_ESTABLISHED :  
				cmd_rxlen = getSn_RX_RSR(SOCKET_N);  //RX_RSR��ʾsocket_n���Ѿ����պͱ�������ݴ�С������0��ʾ���յ������ݣ�����ָ���´�
      /***************************************************/
				if(cmd_rxlen > 0)  //���յ�ָ��
				{
					memset(httpRecvBuffer, 0, HTTP_RX_BUFEER_SIZE);  //HTTP_RX_BUFEER_SIZE=2048
	        memset(httpTransBuffer, 0, HTTP_TX_BUFEER_SIZE);  //HTTP_TX_BUFEER_SIZE=2048
					cmd_rxlen = recv(SOCKET_N, (uint8_t *)httpRecvBuffer, cmd_rxlen);   //�Ὣ���յ������ݴ�ŵ�httpRecvBuffer��������cmd_rxlen
					if( !strncmp((const char *)httpRecvBuffer, (const char *)"GET", 3) || \
						  !strncmp((const char *)httpRecvBuffer, (const char *)"POST", 4))   //HTTP����
					{
						protocol.currentStatusSys = CMD_STATUS_STOPED;
						protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
						httpParseAndResponse(SOCKET_N, cmd_rxlen);
					}//http request
					else  //Э������  298��if
					{
						IWDG_Feed();
						/* copy httpRecvBuffer to cmdBuffer.*/
						memcpy(command_P2.cmdBuffer_P2, httpRecvBuffer,CMD_BUF_LEN);		
						protocol.recv_len = (uint8_t)cmd_rxlen;
						if(command_P2.cmdFormat_P2.fmtHeader_P2 == CMD_HEADER_P2) //P2
						{
								cmd_status = ParseCmd_P2(&protocol, &command_P2);   //����P2Э�� 
								
								if(protocol.isResponseClient != CMD_RESPONSE_N)
								{
									protocol.isResponseClient = CMD_RESPONSE_N;
									// calculate CRC
									uint16_t currentCrc = GetCRC16((unsigned char *)((char*)(&command_P2.cmdFormat_P2)),(command_P2.cmdFormat_P2.fmtLength_P2-2));
									memcpy((char*)&command_P2.cmdBuffer_P2[command_P2.cmdFormat_P2.fmtLength_P2-2], (char*)(&currentCrc), 2);
									// send whole protocol 
									if(send(SOCKET_N, command_P2.cmdBuffer_P2, command_P2.cmdFormat_P2.fmtLength_P2) == STORE_PARA_TOTAL)
									{;}
									IWDG_Feed();
									Delay_ms(5);;
								}//response client
						}

						if(protocol.isDisconnectSocket == CMD_DISCONNECT_Y)  //�Ͽ�socket_n����
						{
							/* clear disconnect flag.*/
							protocol.isDisconnectSocket = CMD_DISCONNECT_N;
							disconnect(SOCKET_N);
							close(SOCKET_N);
							/* reset board.*/
							W5500_Reset();
							W5500_Init();		
							SPI_I2S_DeInit(MASTER_I2Sx);
							NVIC_SystemReset();  // reset all register
							NVIC_SetVectorTable(NVIC_VectTab_FLASH,0);
							ExecuteAPP(NVIC_VectTab_FLASH);
							
						}//CMD_DISCONNECT_Y
					}//cmd request  305��else
				}//rxstrlen  293��if
        /***************************************************/
				IWDG_Feed();
/*************************************��ȡ����****************************************/     
        if (send_point>0)
        {
          Stop_DMA_I2S();
          Config_DMA_I2S(pcmStream.pcmBuffer, pcmStream.pcmBufLength);
          I2S_Cmd(MASTER_I2Sx, ENABLE);
          DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, ENABLE);
          while (send_point>0)
          {
            IWDG_Feed();
            if(pcmStream.upLoading != TRANSFER_PROGRESSING)//���װ��һ��buffer����ȡ��һ��ͨ�����ݲ�����
            {                  
              uint8_t ch_index=pcmStream.upLoading;       
  /*********************************************��ch2������ȡ����*******************************************************************/
                if (fs_index==4)  //85.25KHz
                {
                  for (iii=0;iii<element_count[ch_index-1];iii++)
                  {
                    uint8_t *ptr = (uint8_t *)(pcmStream.pcmBufferAddr[ch_index]+address_offset[ch_index-1]+4*iii);
                    uint8_t second = *ptr;
                    uint8_t first = *(ptr+1);
                    uint8_t fouth = *(ptr+2);
                    uint8_t third = *(ptr+3);
                    //��ʼ��ַ��2��uint16ָ���Ƶ�ch2��֮��4*iii���ƶ�����һ��ch2������uint16��ʼ��ַ
                    pcmStream.SingleCh_Buffer[iii]=(fouth<<0)|(third<<8)|(second<<16)|(first<<24);
                  }  
                  Pcm2Vol_RemoveDC(pcmStream.SingleCh_Buffer, DataOut, 31, 3, element_count[ch_index-1]);
                  memset(pcmStream.SingleCh_Buffer, 0, buffer2_point_num*4);  
                }
              pcmStream.upLoading = TRANSFER_PROGRESSING;
              if(send(SOCKET_N, (uint8_t *)DataOut, element_count[ch_index-1]*4)!=element_count[ch_index-1]*4) {};
              send_point -= element_count[ch_index-1];
            }
            Delay_ms(2);
          }
          send_point=0;
          reset_collect();
        }
        
/*************************************�źŴ���****************************************/
				if(protocol.isLaunchCS5340 == CMD_LAUNCH_CS5340_Y)  
				{
          if (first_cycle==1)  //�ж��Ƿ�Ϊ��һ���ɼ�ѭ��
          {
            Stop_DMA_I2S();
            Config_DMA_I2S(pcmStream.pcmBuffer, pcmStream.pcmBufLength);
            while(start_transmit==0) IWDG_Feed();  //�ȴ���һ�������嵽������DMA��ʼ����
            I2S_Cmd(MASTER_I2Sx, ENABLE);
            DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, ENABLE);
            first_cycle = 0;
            STM_EVAL_LEDOn(LED6);  //��ʼ�ɼ���־
          }
                
          if(pcmStream.upLoading != TRANSFER_PROGRESSING)//���װ��һ��buffer����ȡ��һ��ͨ�����ݲ�����
          {                  
            uint8_t ch_index=pcmStream.upLoading;       
            if (cross==0)  //����Ҫ�������
            {
/*********************************************��ch2������ȡ����*******************************************************************/
              if (fs_index==4)  //85.25KHz
              {
                for (iii=0;iii<element_count[ch_index-1];iii++)
                {
                  uint8_t *ptr = (uint8_t *)(pcmStream.pcmBufferAddr[ch_index]+address_offset[ch_index-1]+4*iii);
                  uint8_t second = *ptr;
                  uint8_t first = *(ptr+1);
                  uint8_t fouth = *(ptr+2);
                  uint8_t third = *(ptr+3);
                  //��ʼ��ַ��2��uint16ָ���Ƶ�ch2��֮��4*iii���ƶ�����һ��ch2������uint16��ʼ��ַ
                  pcmStream.SingleCh_Buffer[iii]=(fouth<<0)|(third<<8)|(second<<16)|(first<<24);
                }  
                Pcm2Vol_RemoveDC(pcmStream.SingleCh_Buffer, DataOut, 31, 3, element_count[ch_index-1]);
                memset(pcmStream.SingleCh_Buffer, 0, buffer2_point_num*4);  
              }
/*********************************************ȡch2���������********************************************************************/              
/*********************************************�źż��ģ��********************************************************************/  
              IWDG_Feed();
              output=Detect_Signal(DataOut,element_count[ch_index-1],1024,85250,(uint32_t)freq_detect,threshold);
              if (output==0)
              {
                storage=1;
                find_27k=0;
                memset(data_arrival, 0, buffer_point_num*4);
                memset(DataOut, 0, buffer2_point_num*4);
              }
              else  //��⵽�ź�
              {
                if (storage==1)
                {
                  move_float_data(DataOut,data_arrival,element_count[ch_index-1]);
                  memset(DataOut, 0, buffer2_point_num*4);
                  first_length=element_count[ch_index-1];
                }
                else
                {
                  move_float_data(DataOut,data_arrival+first_length,element_count[ch_index-1]);
                  memset(DataOut, 0, buffer2_point_num*4);
                }
                storage=2;
                find_27k++;
                if (find_27k==2)  //�������μ�⵽�����Խ��е���ʱ����
                { 
                  uint32_t DMA_num_now = DMA_num;
                  cross=70;
                  find_27k=0;
                  storage=1;
                  if (process_state == PROCESS_SIGNAL)
                  {
                    if (send(SOCKET_N, (uint8_t *)data_arrival, buffer_point_num*4)!=buffer_point_num*4) {};   //ֻ����ǰ1278��
                  }
                  else if(process_state == PROCESS_ARRIVAL_TIME)
                  {
                    uint16_t arrival_point=time_arrival_detection(data_arrival,buffer_point_num/4*3);  //buffer_point_num/4*3=1705{852+853}/4*3=1278
                    memcpy(data_stream.lasttime, LastTime, 3);
                    data_stream.last_dma_num=LastDMA_num;
                    data_stream.last_remain=LastRemain;
                    data_stream.dma_num_now=DMA_num_now;
                    data_stream.arrival_time=arrival_point;
                    data_stream.energy_27=energy_27k;
                    data_stream.crc = GetCRC16((uint8_t *)data_stream.header, data_stream.len-2);
                    if(send(SOCKET_N, data_stream.header, data_stream.len)!=data_stream.len) {};
                  }
                  memset(data_arrival, 0, buffer_point_num*4);
                  STM_EVAL_LEDToggle(LED3);
                  IWDG_Feed();
                }
              }
            }  //cross==0
            pcmStream.upLoading = TRANSFER_PROGRESSING;
          }
				}  //����CS5340
        else
        {
          first_cycle = 1;
          start_transmit = 0;
          protocol.currentStatusSys=CMD_PARAM_VALVE_STOP_P2;
          DMA_num=0;
          pcmStream.upLoading=0;
          cross=0;
          storage=1;
          output=0;
          find_27k=0;
          DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, DISABLE);
          I2S_Cmd(MASTER_I2Sx, DISABLE);
          STM_EVAL_LEDOff(LED6);  //û�вɼ���־
        }
        /***************************************************/
				ledNetSwitch = 1;
				if(wizphy_getphylink() == PHY_LINK_OFF)
				{
					disconnect(SOCKET_N);
				}//link off
			break;
		
			case SOCK_CLOSE_WAIT :
				close(SOCKET_N);
        socket(SOCKET_N, Sn_MR_TCP,LocalPort,SF_TCP_NODELAY|SF_IO_NONBLOCK);
        listen(SOCKET_N);
			break;
		
			case SOCK_INIT :
				listen(SOCKET_N);
			break;
		
			case SOCK_CLOSED :
				ledNetSwitch = 0;
				socket(SOCKET_N, Sn_MR_TCP,LocalPort,SF_TCP_NODELAY|SF_IO_NONBLOCK);
			break;
      
      case SOCK_LISTEN:
        reset_collect();
        ledNetSwitch = 0;
        break;
			default:
        reset_collect();
			break;
		}//getSn_SR
    
    if (socket_reset_flag)
    {
        socket_reset_flag = 0;

        uint8_t status = getSn_SR(SOCKET_N);
        if (status != SOCK_ESTABLISHED && status != SOCK_LISTEN)
        {
            close(SOCKET_N);
            if (socket(SOCKET_N, Sn_MR_TCP, LocalPort, SF_TCP_NODELAY | SF_IO_NONBLOCK) == SOCKET_N)
            {
                listen(SOCKET_N);
            }
        }
    }
  }//Infinite loop
}//main

void NVIC_Administrate(void)  //�����ж���Ϣ
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* four priority groups.*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* initialize Master SPI2 rx interrupt.*/     //��������SPI2�ϵ�DMA1�ж�
	NVIC_InitStructure.NVIC_IRQChannel                   = MASTER_SPIx_RX_DMA_STREAM_IRQ;//#define MASTER_SPIx_RX_DMA_STREAM_IRQ   DMA1_Stream3_IRQn
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;  //PB9��Ӧ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
  
  //���ô����ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_Init(&NVIC_InitStructure);
}

void reset_collect(void)
{
  first_cycle = 1;
  start_transmit = 0;
  protocol.currentStatusSys=CMD_PARAM_VALVE_STOP_P2;
  pcmStream.upLoading=TRANSFER_PROGRESSING;
  DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, DISABLE);
  I2S_Cmd(MASTER_I2Sx, DISABLE);
  protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
  cross=0;
  storage=1;
  find_27k=0;
  output=0;
}


