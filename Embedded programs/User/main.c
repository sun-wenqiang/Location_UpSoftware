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
__align(4) AudioTypeDef pcmStream;  //__align(4)用来强制将pcmStream在内存中按四字节对齐方式存储，这个变量用来配置采集芯片cs5340，包含采样率/采样精度/麦克风数量等等
extern char httpRecvBuffer[HTTP_RX_BUFEER_SIZE];  //HTTP_RX_BUFEER_SIZE=2048
extern char httpTransBuffer[HTTP_TX_BUFEER_SIZE];  //HTTP_TX_BUFEER_SIZE=2048
extern ModifyFsDef fsReferList[maxfs];  //可能用来配置音频采样率
__align(4) CmdTypeDef    command;  //定义命令
CmdTypeDef_P2 command_P2;
PtlTypeDef protocol;  //定义协议
uint16_t cmd_rxlen;  //W5500已经接收和保存的数据大小
uint8_t cmd_status;
uint8_t socketStatus;
uint32_t ledNetBaseTime;
uint8_t ledNetSwitch;
uint8_t fs_index;
uint8_t start_flag  = 0;  //全局作用域
extern uint16_t LocalPort ;
extern uint8_t socket_reset_flag;
extern para_signal_process Para;
extern uint8_t C;
extern uint8_t process_state;

/* Private function prototypes -----------------------------------------------*/
static void NVIC_Administrate(void);  //只可以在被定义的文件中被调用
static void reset_collect(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
  
uint32_t iii=0;  //取出单通道数据所用到的参数
int jjj=0;       //取出单通道数据所用到的参数
size_t element_count[2] = {buffer1_point_num, buffer2_point_num};   //前一半buffer和后一半buffer中ch2的采样点个数
uint8_t address_offset[2]={2,0};
uint8_t storage=1;
uint8_t output=0;
uint8_t find_27k=0;
float DataOut[buffer2_point_num]={0};   //用来存放pcm转为float类型之后的信号
float data_arrival[buffer_point_num] = {0};  //1705个数据，存放用于寻找到达时间的数据
size_t first_length = 0;            // 第一次检测到的buffer长度
uint16_t freq_detect = 27000;
static uint16_t socketWatchdog = 0;
static uint32_t end_num = 0;
uint8_t detect_ratio = 0;
int main(void)
{
	/* move vector table.*/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x20000);  //设置中断向量表地址
	
	/* Cortex-Mx's NVIC administrate and management*/
	NVIC_Administrate();  //配置 NVIC 中断优先级和使能
	
	/* initialize delay function(Cortex-Mx's SysTick).*/
	Delay_Initialize();  //延时初始化
	
  /***********************************************************************************************/
  /***********************************************************************************************/
  /***********************************************************************************************/
  STM_EVAL_LEDInit(LED6);  //初始化LED的GPIO口，分别为板子上的6543LED
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED3);

	STM_EVAL_LEDOn(LED6);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED3);
  
  
  GPIO_EXTI9_Init();
  UART3_Init();
  //DAC_Init_Config();  //初始化DAC
  ADC_Init_Config();  //初始化ADC
  GAIN_GPIO_Init();   //放大倍数初始化，默认为放大10倍    
  /***********************************************************************************************/
  /***********************************************************************************************/
  /***********************************************************************************************/
  
	/* initialize device at24c02.*/
	AT24C02_Init();  //与AT24C02（E2PROM）通过I2C通信，这一步初始化I2C的引脚（SCL/SDL）
	
	/* initialize protocol.*/
	Protocol_Init(&protocol, &command);  //协议初始化，配置MAC，IP等
	
	/* initialize SPI1 for NetWork.*/
	sFLASH_Init();    //配置SPI和MDA来为与外部闪存进行通信做准备
	/* Reset W5500 before operate it.*/
	W5500_Reset();  //配置W5500的复位引脚并进行复位
	/* Configure server's local information, for instance ip/gw/mac...*/
	W5500_Init();  //配置W5500的各种信息，IP/MAC/收发缓冲区/超时时间/重连次数等
	
	
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
			fs_index = 3;   //20240925添加80K
			break;	
    case FS_852500:
      fs_index = 4;   //20241021添加85.25K
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

///////////////////  开机等待 ///////////////////////////////
	LocalPort = 5000; // port
	TIM_START_Config();
	start_flag = 0;	
	W5500_Reset();
	W5500_Init_start();
	
	LaunchWatchDog(IWDG_Prescaler_64, 5000); // (64/40'000)*625 = 1s,  (64/40'000)*5000 = 8s
	while (1)  // 开机等待循环
	{
		IWDG_Feed();
		if(start_flag == 1)  
		{
			start_flag = 0;
			TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);  //禁用TIM2的定时器更新中断
			break;
		}
		else if(start_flag == 2)  //触发了TIM2的定时器中断
		{
			start_flag = 0;
			/* Reset W5500 before operate it.*/
			W5500_Reset();
			/* Configure server's local information, for instance ip/gw/mac...*/
			W5500_Init();		
      TIM_Cmd(TIM2, DISABLE);  //开启TIM2没啥用，直接关掉
			break;
		}
		else if(start_flag == 0)
		{
			switch(getSn_SR(SOCKET_N))  //获取状态寄存器
			{
				IWDG_Feed();
				case SOCK_ESTABLISHED :  //此状态下可以使用SEND或RECV进行数据包传输
					cmd_rxlen = getSn_RX_RSR(SOCKET_N);  //RX_RSR显示socket_n中已经接收和保存的数据大小
					if(cmd_rxlen > 0)
					{
						memset(httpRecvBuffer, 0, HTTP_RX_BUFEER_SIZE);  //HTTP_RX_BUFEER_SIZE=2048，将httpRecvBuffer中的数据全部置零
						memset(httpTransBuffer, 0, HTTP_TX_BUFEER_SIZE);  //HTTP_TX_BUFEER_SIZE=2048，将httpTransBuffer中的数据全部置零
						cmd_rxlen = recv(SOCKET_N, (uint8_t *)httpRecvBuffer, cmd_rxlen);  //从socket_n中接收并存到缓冲区
						if( !strncmp((const char *)httpRecvBuffer, (const char *)"GET", 3) || \
								!strncmp((const char *)httpRecvBuffer, (const char *)"POST", 4))  //处理HTTP请求
						{  
							protocol.currentStatusSys = CMD_STATUS_STOPED;
							protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
							httpParseAndResponse(SOCKET_N, cmd_rxlen);  //  ？？？？解析并回复HTTP请求
						}//http request
						else  //159行if   不是HTTP请求？
						{
							IWDG_Feed();
							/* copy httpRecvBuffer to cmdBuffer.*/
							memcpy(command.cmdBuffer, httpRecvBuffer,CMD_BUF_LEN);  //将接收到的数据(httpRecvBuffer)复制到cmdBuffer
							memcpy(command_P2.cmdBuffer_P2, httpRecvBuffer,CMD_BUF_LEN);
							protocol.recv_len = (uint8_t)cmd_rxlen;  //socket_n中已经接收和保存的数据大小
							TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);  //禁用TIM2定时器更新
							if(command_P2.cmdFormat_P2.fmtHeader_P2 == CMD_HEADER_P2) //P2
							{
									cmd_status = ParseCmd_P2_Start(&protocol, &command_P2);  //处理解析command_P2
									
									if(protocol.isResponseClient != CMD_RESPONSE_N)  //需要响应客户端
									{
										protocol.isResponseClient = CMD_RESPONSE_N;
										// calculate CRC
										uint16_t currentCrc = GetCRC16((unsigned char *)((char*)(&command_P2.cmdFormat_P2)),(command_P2.cmdFormat_P2.fmtLength_P2-2));
										memcpy((char*)&command_P2.cmdBuffer_P2[command_P2.cmdFormat_P2.fmtLength_P2-2], (char*)(&currentCrc), 2);
										// send whole protocol 
										if(send(SOCKET_N, command_P2.cmdBuffer_P2, command_P2.cmdFormat_P2.fmtLength_P2) == STORE_PARA_TOTAL)  //MCU对PC的回复
										{;}
										start_flag = 1;
										W5500_Reset();
										W5500_Init_reset();													
										Delay_ms(5);
									}//response client
							}  //处理P2协议
						}//cmd request  166行
					}//rxstrlen  154行  cmd_rxlen <= 0
					ledNetSwitch = 1;
					if(wizphy_getphylink() == PHY_LINK_OFF)  //断开连接
					{
						disconnect(SOCKET_N);
					}//link off
				break;
			
				case SOCK_CLOSE_WAIT :  //socket_n接收到了来自连接对方发来的断开链接的请求，这个状态下仍然可以进行数据传输
					close(SOCKET_N);  //关闭socket
				break;
			
				case SOCK_INIT :  //表示socket打开并处于TCP工作模式
					listen(SOCKET_N);  //等待客户端连接请求
				break;
			
				case SOCK_CLOSED :  //指示socket_n处于关闭状态，资源被释放
					ledNetSwitch = 0;
					socket(SOCKET_N, Sn_MR_TCP,LocalPort,SF_TCP_NODELAY|SF_IO_NONBLOCK);  //建立一个套接字连接
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
  data_stream.id=protocol.romBuffer[ROM_ID];   //设备ID
  data_stream.crc = 0x0102;
  GPS_message.id=protocol.romBuffer[ROM_ID];   //设备ID
  Energy.id = protocol.romBuffer[ROM_ID];   //设备ID
  Set_Gain(protocol.romBuffer[ROM_GAIN]);     //设置放大倍数
  threshold = ((uint16_t)protocol.romBuffer[ROM_THRESHOLD + 1] << 8) | protocol.romBuffer[ROM_THRESHOLD];
  freq_detect = ((uint16_t)protocol.romBuffer[ROM_FREQ + 1] << 8) | protocol.romBuffer[ROM_FREQ];
  Para.id = data_stream.id;
  Para.frequency = freq_detect;
  Para.magnification = protocol.romBuffer[ROM_GAIN];
  Para.threshold = threshold;
  Para.ratio = protocol.romBuffer[ROM_RATIO];
  detect_ratio = Para.ratio;
  /* ************************处理循环 ************************/ 
  /* ************************处理循环 ************************/ 
  /* ************************处理循环 ************************/ 
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
				STM_EVAL_LEDToggle(LED5);  //翻转电平
			}//ledNetBaseTime
		}//ledNetSwitch
		else  //ledNetSwitch != 0   SOCK_ESTABLISHED
		{
			STM_EVAL_LEDOn(LED5);  //表示连接网络成功
		}
  /***************************************************/
 
		switch(getSn_SR(SOCKET_N))
		{
			IWDG_Feed();
			case SOCK_ESTABLISHED :  
				cmd_rxlen = getSn_RX_RSR(SOCKET_N);  //RX_RSR显示socket_n中已经接收和保存的数据大小，大于0表示接收到了数据，即有指令下达
      /***************************************************/
				if(cmd_rxlen > 0)  //接收到指令
				{
					memset(httpRecvBuffer, 0, HTTP_RX_BUFEER_SIZE);  //HTTP_RX_BUFEER_SIZE=2048
	        memset(httpTransBuffer, 0, HTTP_TX_BUFEER_SIZE);  //HTTP_TX_BUFEER_SIZE=2048
					cmd_rxlen = recv(SOCKET_N, (uint8_t *)httpRecvBuffer, cmd_rxlen);   //会将接收到的数据存放到httpRecvBuffer，长度是cmd_rxlen
					if( !strncmp((const char *)httpRecvBuffer, (const char *)"GET", 3) || \
						  !strncmp((const char *)httpRecvBuffer, (const char *)"POST", 4))   //HTTP请求
					{
						protocol.currentStatusSys = CMD_STATUS_STOPED;
						protocol.isLaunchCS5340 = CMD_LAUNCH_CS5340_N;
						httpParseAndResponse(SOCKET_N, cmd_rxlen);
					}//http request
					else  //协议请求  298行if
					{
						IWDG_Feed();
						/* copy httpRecvBuffer to cmdBuffer.*/
						memcpy(command_P2.cmdBuffer_P2, httpRecvBuffer,CMD_BUF_LEN);		
						protocol.recv_len = (uint8_t)cmd_rxlen;
						if(command_P2.cmdFormat_P2.fmtHeader_P2 == CMD_HEADER_P2) //P2
						{
								cmd_status = ParseCmd_P2(&protocol, &command_P2);   //处理P2协议 
								
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

						if(protocol.isDisconnectSocket == CMD_DISCONNECT_Y)  //断开socket_n连接
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
					}//cmd request  305行else
				}//rxstrlen  293行if
        /***************************************************/
				IWDG_Feed();
/*************************************获取数据****************************************/     
        if (send_point>0)
        {
          Stop_DMA_I2S();
          Config_DMA_I2S(pcmStream.pcmBuffer, pcmStream.pcmBufLength);
          I2S_Cmd(MASTER_I2Sx, ENABLE);
          DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, ENABLE);
          while (send_point>0)
          {
            IWDG_Feed();
            if(pcmStream.upLoading != TRANSFER_PROGRESSING)//如果装满一个buffer，则取出一个通道数据并发送
            {                  
              uint8_t ch_index=pcmStream.upLoading;       
  /*********************************************将ch2的数据取出来*******************************************************************/
                if (fs_index==4)  //85.25KHz
                {
                  for (iii=0;iii<element_count[ch_index-1];iii++)
                  {
                    uint8_t *ptr = (uint8_t *)(pcmStream.pcmBufferAddr[ch_index]+address_offset[ch_index-1]+4*iii);
                    uint8_t second = *ptr;
                    uint8_t first = *(ptr+1);
                    uint8_t fouth = *(ptr+2);
                    uint8_t third = *(ptr+3);
                    //初始地址加2将uint16指针移到ch2，之后4*iii是移动到下一个ch2采样点uint16起始地址
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
        
/*************************************信号处理****************************************/
				if(protocol.isLaunchCS5340 == CMD_LAUNCH_CS5340_Y)  
				{
          if (first_cycle==1)  //判断是否为第一个采集循环
          {
            Stop_DMA_I2S();
            Config_DMA_I2S(pcmStream.pcmBuffer, pcmStream.pcmBufLength);
            while(start_transmit==0) IWDG_Feed();  //等待下一个秒脉冲到来，打开DMA开始传输
            I2S_Cmd(MASTER_I2Sx, ENABLE);
            DMA_Cmd(MASTER_SPIx_RX_DMA_STREAM, ENABLE);
            first_cycle = 0;
            STM_EVAL_LEDOn(LED6);  //开始采集标志
          }
                
          if(pcmStream.upLoading != TRANSFER_PROGRESSING)//如果装满一个buffer，则取出一个通道数据并发送
          {                  
            uint8_t ch_index=pcmStream.upLoading;       
            if (cross==0)  //不需要跨过不看
            {
/*********************************************将ch2的数据取出来*******************************************************************/
              if (fs_index==4)  //85.25KHz
              {
                for (iii=0;iii<element_count[ch_index-1];iii++)
                {
                  uint8_t *ptr = (uint8_t *)(pcmStream.pcmBufferAddr[ch_index]+address_offset[ch_index-1]+4*iii);
                  uint8_t second = *ptr;
                  uint8_t first = *(ptr+1);
                  uint8_t fouth = *(ptr+2);
                  uint8_t third = *(ptr+3);
                  //初始地址加2将uint16指针移到ch2，之后4*iii是移动到下一个ch2采样点uint16起始地址
                  pcmStream.SingleCh_Buffer[iii]=(fouth<<0)|(third<<8)|(second<<16)|(first<<24);
                }  
                Pcm2Vol_RemoveDC(pcmStream.SingleCh_Buffer, DataOut, 31, 3, element_count[ch_index-1]);
                memset(pcmStream.SingleCh_Buffer, 0, buffer2_point_num*4);  
              }
/*********************************************取ch2的数据完成********************************************************************/              
/*********************************************信号检测模块********************************************************************/  
              IWDG_Feed();
              output=Detect_Signal(DataOut,element_count[ch_index-1],1024,85250,(uint32_t)freq_detect,threshold);
              if (output==0)
              {
                storage=1;
                find_27k=0;
                memset(data_arrival, 0, buffer_point_num*4);
                memset(DataOut, 0, buffer2_point_num*4);
              }
              else  //检测到信号
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
                if (find_27k==2)  //连续两次检测到，可以进行到达时间检测
                { 
                  uint32_t DMA_num_now = DMA_num;
                  cross=70;
                  find_27k=0;
                  storage=1;
                  if (process_state == PROCESS_SIGNAL)
                  {
                    if (send(SOCKET_N, (uint8_t *)data_arrival, buffer_point_num*4)!=buffer_point_num*4) {};   //只发送前1278点
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
				}  //启动CS5340
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
          STM_EVAL_LEDOff(LED6);  //没有采集标志
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

void NVIC_Administrate(void)  //配置中断信息
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* four priority groups.*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* initialize Master SPI2 rx interrupt.*/     //这里配置SPI2上的DMA1中断
	NVIC_InitStructure.NVIC_IRQChannel                   = MASTER_SPIx_RX_DMA_STREAM_IRQ;//#define MASTER_SPIx_RX_DMA_STREAM_IRQ   DMA1_Stream3_IRQn
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置外部中断
	NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;  //PB9对应的中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
  
  //配置串口中断
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


