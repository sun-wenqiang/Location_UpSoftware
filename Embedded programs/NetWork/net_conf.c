#include "net_conf.h"

/**************** Local NetWork Information ****************/
wiz_NetInfo  NetConf = 
{
	 /* Server MAC */
	{0x0c,0x29,0xab,0x7c,0x04,0x03},
	/* Server IP */
	{192,168,4,168},
	/* Subnet Mask */
	{255,255,255,0},
	/* Gateway IP Address */
	{192,168,4,1},
	/* DNS server IP Address */
	{114,114,114,114},
	/* Static IP */
	NETINFO_STATIC
};


wiz_NetInfo  NetConf_start = 
{
	 /* Server MAC */
	{0x0c,0x29,0xab,0x7c,0x04,0x03},
	/* Server IP */
	{192,168,1,250},
	/* Subnet Mask */
	{255,255,255,0},
	/* Gateway IP Address */
	{192,168,1,1},
	/* DNS server IP Address */
	{114,114,114,114},
	/* Static IP */
	NETINFO_STATIC
};

wiz_NetInfo  NetConf_reset = 
{
	 /* Server MAC */
	{0x0c,0x29,0xab,0x7c,0x04,0x03},
	/* Server IP */
	{192,168,1,168},
	/* Subnet Mask */
	{255,255,255,0},
	/* Gateway IP Address */
	{192,168,1,1},
	/* DNS server IP Address */
	{114,114,114,114},
	/* Static IP */
	NETINFO_STATIC
};


/* W5500 T/R Buffer.*/
uint8_t TX_Buffer[8] = {16,0,0,0,0,0,0,0};
uint8_t RX_Buffer[8] = {16,0,0,0,0,0,0,0};
//uint8_t TX_Buffer[8] = {2,2,2,2,2,2,2,2};
//uint8_t RX_Buffer[8] = {2,2,2,2,2,2,2,2};
wiz_NetTimeout ReTry =
{
	/* Retry Times */
	8,
	/* Limit Time 2000*100us = 200ms */
	2000
};
/*===================================================*/

/* Server Port.*/
uint16_t LocalPort = 5000;

/* Enter Critical.*/
static void Enter_Critical(void)
{
	__set_PRIMASK(1);
}

/* Exti Critical.*/
static void Exti_Critical(void)
{
	__set_PRIMASK(0);
}

/* CS pin pull-down to Select*/
static void W5500_Selcet(void)
{
  sFLASH_CS_LOW();
}

/* CS pin pull-up to delete */
static void W5500_Delete(void)
{
  sFLASH_CS_HIGH();
}

void W5500_Init(void)
{
	/* Register Critical Function.*/
	reg_wizchip_cris_cbfunc(Enter_Critical,Exti_Critical);  //ע�������˳��ٽ����Ļص�����

	/* Register CS Select Function.*/
	reg_wizchip_cs_cbfunc(W5500_Selcet,W5500_Delete);  //ע��W5500Ƭѡ��ǰ��Ϊѡ��W5500������Ϊ��ѡ��

	/* Register Write and Read Function.*/
	reg_wizchip_spi_cbfunc(sFLASH_ReadByte,sFLASH_SendByte);  //ע��SPI�Ķ�д�ص�����������ͨ��SPIʵ��MCU��W5500�����ݽ���
	
	/* Configure W5500 T/R Buffer.*/
	wizchip_init(TX_Buffer,RX_Buffer);  //ע�������շ�������
	
	/* Configure Local Information */
	ctlnetwork(CN_SET_NETINFO,&NetConf);  //���ø���������Ϣ����IP��MAC�����ص�

	/* Configure Retry Times and Limit Time.*/
	ctlnetwork(CN_SET_TIMEOUT,&ReTry);  //�������Դ����ͳ�ʱʱ��
}



void W5500_Init_start(void)
{
	/* Register Critical Function.*/
	reg_wizchip_cris_cbfunc(Enter_Critical,Exti_Critical);

	/* Register CS Select Function.*/
	reg_wizchip_cs_cbfunc(W5500_Selcet,W5500_Delete);

	/* Register Write and Read Function.*/
	reg_wizchip_spi_cbfunc(sFLASH_ReadByte,sFLASH_SendByte);
	
	/* Configure W5500 T/R Buffer.*/
	wizchip_init(TX_Buffer,RX_Buffer);
	
	/* Configure Local Information */
	ctlnetwork(CN_SET_NETINFO,&NetConf_start);

	/* Configure Retry Times and Limit Time.*/
	ctlnetwork(CN_SET_TIMEOUT,&ReTry);
}


void W5500_Init_reset(void)
{
	/* Register Critical Function.*/
	reg_wizchip_cris_cbfunc(Enter_Critical,Exti_Critical);

	/* Register CS Select Function.*/
	reg_wizchip_cs_cbfunc(W5500_Selcet,W5500_Delete);

	/* Register Write and Read Function.*/
	reg_wizchip_spi_cbfunc(sFLASH_ReadByte,sFLASH_SendByte);
	
	/* Configure W5500 T/R Buffer.*/
	wizchip_init(TX_Buffer,RX_Buffer);
	
	/* Configure Local Information */
	ctlnetwork(CN_SET_NETINFO,&NetConf_reset);

	/* Configure Retry Times and Limit Time.*/
	ctlnetwork(CN_SET_TIMEOUT,&ReTry);
}


#define DLY_1MS     42000
static void W5500_ResetDelay(uint16_t ms)
{
	uint32_t i = 0;
	uint16_t j = 0;
	for(j = 0;j < ms;j++)
	{
		for(i = 0; i < DLY_1MS; i++)
		{
			;
		}			
	}
}

#define RST_Down    GPIO_ResetBits(WIZ_RST_Port, WIZ_RST_Pin)
#define RST_Up      GPIO_SetBits(WIZ_RST_Port, WIZ_RST_Pin)
void W5500_Reset(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(WIZ_RST_Pin_CLK, ENABLE);
	
  /*!< Configure Wiz_RST pin in output nopull mode ********************/
	GPIO_InitStructure.GPIO_Mode                    = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed                   = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType                   = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd                    = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin                     = WIZ_RST_Pin;  
	GPIO_Init(WIZ_RST_Port, &GPIO_InitStructure);
	
	//ȷ��������ȷ��λ������
	RST_Up;
	W5500_ResetDelay(1);
	RST_Down;
	W5500_ResetDelay(2);
	RST_Up; 
	W5500_ResetDelay(3);
}

void dma_wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len)
{
//	uint16_t i = 0;
   uint16_t ptr = 0;
   uint32_t addrsel = 0;
	 uint8_t spi_data[3];
   if(len == 0)  
     return;
   ptr = getSn_TX_WR(sn);
   addrsel = ((uint32_t)ptr << 8) + (WIZCHIP_TXBUF_BLOCK(sn) << 3);
   //
   //WIZCHIP_WRITE_BUF(addrsel,wizdata, len);
   /*<%*/
   WIZCHIP_CRITICAL_ENTER();
   WIZCHIP.CS._select();

   addrsel |= (_W5500_SPI_WRITE_ | 0x00);

   if(!WIZCHIP.IF.SPI._write_burst) 	// byte operation
   {
		WIZCHIP.IF.SPI._write_byte((addrsel & 0x00FF0000) >> 16);
		WIZCHIP.IF.SPI._write_byte((addrsel & 0x0000FF00) >>  8);
		WIZCHIP.IF.SPI._write_byte((addrsel & 0x000000FF) >>  0);
//		for(i = 0; i < len; i++)
//			WIZCHIP.IF.SPI._write_byte(wizdata[i]);
		 dmaSPI_SendByte(wizdata, len);
   }
   else									// burst operation
   {
		spi_data[0] = (addrsel & 0x00FF0000) >> 16;
		spi_data[1] = (addrsel & 0x0000FF00) >> 8;
		spi_data[2] = (addrsel & 0x000000FF) >> 0;
		WIZCHIP.IF.SPI._write_burst(spi_data, 3);
		WIZCHIP.IF.SPI._write_burst(wizdata, len);
   }
   
   WIZCHIP.CS._deselect();
   WIZCHIP_CRITICAL_EXIT();
   SPI1->DR;

	 /*%>*/
   ptr += len;
   setSn_TX_WR(sn,ptr);
}
