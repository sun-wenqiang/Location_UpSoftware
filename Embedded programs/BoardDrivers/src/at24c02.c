#include "at24c02.h"

/* SDA(PB7)����ģʽ */
void SDA_InputMode(void)
{
	
	GPIOB->MODER&=~(3<<(7*2));
	
	GPIOB->MODER|=0<<(7*2);
	
}

/* SDA(PB7)���ģʽ */
void SDA_OutputMode(void)
{
	
	GPIOB->MODER&=~(3<<(7*2));
	
	GPIOB->MODER|=0x00000001<<(7*2);
	
}

/* IIC����д */
#define IICPinWrite(iicport,iicpin,status)                    \
do{                                                           \
	if(0 == status){                                          \
		GPIO_ResetBits(iicport,iicpin);                       \
	}else{                                                    \
		GPIO_SetBits(iicport,iicpin);                         \
	}                                                         \
}while(0u)

/* IIC���Ŷ� */
#define SDAPinRead GPIO_ReadInputDataBit(SDA_Port,SDA_Pin)

/* SDA��SCL���ų�ʼ�� */
void AT24C02_Init(void)
{
	
	GPIO_InitTypeDef AT24C02_GPIO_Struct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	AT24C02_GPIO_Struct.GPIO_Mode    = GPIO_Mode_OUT;
	AT24C02_GPIO_Struct.GPIO_OType   = GPIO_OType_PP;
	AT24C02_GPIO_Struct.GPIO_Pin     = SCL_Pin|SDA_Pin;  // PIN6/PIN7���˿ھ�ΪPB
	AT24C02_GPIO_Struct.GPIO_PuPd    = GPIO_PuPd_UP;
	AT24C02_GPIO_Struct.GPIO_Speed   = GPIO_Speed_100MHz;
	
	/* SCL��ʼ�� */
	GPIO_Init(GPIOB,&AT24C02_GPIO_Struct);
	
	IICPinWrite(SCL_Port,SCL_Pin,1);  //д��ߵ�ƽ����ΪI2C�ڿ���ʱ��Ӧ��Ϊ�ߵ�ƽ
	IICPinWrite(SDA_Port,SDA_Pin,1);  
}

/****************************************************************************************
 *                                    ģ��IIC                                           *
 ****************************************************************************************/
#include "delay.h"

/* ��ʼ�ź� */
void StartSignal(void)
{
	SDA_OutputMode();
	
	IICPinWrite(SCL_Port,SCL_Pin,1);
	
	IICPinWrite(SDA_Port,SDA_Pin,1);
	
	Delay_us(5);
	
	IICPinWrite(SDA_Port,SDA_Pin,0);
	
	Delay_us(5);
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
}

/* �����ź� */
void StopSignal(void)
{
	SDA_OutputMode();
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
	IICPinWrite(SDA_Port,SDA_Pin,0);
	
	Delay_us(5);
	
	IICPinWrite(SCL_Port,SCL_Pin,1);
	
	Delay_us(5);
	
	IICPinWrite(SDA_Port,SDA_Pin,1);	
	
}

/* �ȴ�Ӧ�� */
uint8_t WaitAckSignal(void)
{
	uint8_t err = 0;
	
	SDA_InputMode();
	
	IICPinWrite(SDA_Port,SDA_Pin,1);
	
	Delay_us(4);
	
	IICPinWrite(SCL_Port,SCL_Pin,1);
	
	Delay_us(4);
	
	while(SDAPinRead)
	{
		err++;
		
		if(err > 250)
		{
			StopSignal();
			
			/* ����Ӧ��ʧ�� */
			return 1;
		}
		
		Delay_ms(1);
	}
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
	/* ����Ӧ��ɹ� */
	return 0;
	
}

void ReplyAckSignal(void)
{
	SDA_OutputMode();
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
	IICPinWrite(SDA_Port,SDA_Pin,0);
	
	Delay_us(2);
	
	IICPinWrite(SCL_Port,SCL_Pin,1);
	
	Delay_us(2);
	
	IICPinWrite(SCL_Port,SCL_Pin,0);	
}

void NReplyAckSignal(void)
{
	SDA_OutputMode();
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
	IICPinWrite(SDA_Port,SDA_Pin,1);
	
	Delay_us(2);
	
	IICPinWrite(SCL_Port,SCL_Pin,1);
	
	Delay_us(2);
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
}

void Send1Byte(uint8_t dat)
{
	uint8_t temp = 0;
	
	SDA_OutputMode();
	
	IICPinWrite(SCL_Port,SCL_Pin,0);
	
	for(uint8_t index = 0;index < 8;index++)
	{
		temp = (dat&0x80)>>7;
		
		IICPinWrite(SDA_Port,SDA_Pin,temp);
		
		dat<<=1;
		
		Delay_us(2);
		
		IICPinWrite(SCL_Port,SCL_Pin,1);
		
		Delay_us(2);
		
		IICPinWrite(SCL_Port,SCL_Pin,0);
		
		Delay_us(2);
	}
	
}

uint8_t Receive1Byte(uint8_t ack)
{
	uint8_t temp = 0;
	
	SDA_InputMode();
	
	for(uint8_t index = 0;index < 8;index++)
	{
		IICPinWrite(SCL_Port,SCL_Pin,0);
		
		Delay_us(2);
		
		IICPinWrite(SCL_Port,SCL_Pin,1);
		
		temp<<=1;
		
		if(SDAPinRead){
			temp += 1;
		}
			
		Delay_us(1);
	}
	
	if(!ack){
		NReplyAckSignal();
	}else{
		ReplyAckSignal();
	}
	
	return temp;
}
/************************************* End of IIC ****************************************/



/****************************************************************************************
 *                                    AT24C02                                           *
 ****************************************************************************************/

uint8_t AT24C02_ReadByte(uint8_t addr)
{
	uint8_t temp = 0;
	
	__set_PRIMASK(1);
	
	StartSignal();
	
	/* AT24C02д���� */
	Send1Byte(0xA0);
	
	WaitAckSignal();
	
	Send1Byte(addr);
	
	WaitAckSignal();
	
	StartSignal();
	
	/* AT24C02������ */	
	Send1Byte(0xA1);
	
	WaitAckSignal();
	
	temp = Receive1Byte(0);
	
	StopSignal();
	
	__set_PRIMASK(0);
	
	return temp;
}


void AT24C02_WriteByte(uint8_t addr,uint8_t dat)
{
	__set_PRIMASK(1);
	
	StartSignal();
	
	/* AT24C02д���� */
	Send1Byte(0xA0);
	
	WaitAckSignal();	
	
	Send1Byte(addr);
	
	WaitAckSignal();
	
	Send1Byte(dat);
	
	WaitAckSignal();
	
	StopSignal();
	
	__set_PRIMASK(0);
	
	Delay_ms(10);
	
}
/* ���AT24C02�Ƿ��쳣 */
AT24C02Status CheckAT24C02(uint8_t addr,uint8_t dat)
{
	/* ��д */
	AT24C02_WriteByte(addr,dat);
	
	/* �Զ� */
	if(dat != AT24C02_ReadByte(addr))
	{
		/* AT24C02�쳣�򲻴��� */
		return AbNormal;
	}
	
	return Normal;
}

AT24C02Status WriteInfo(uint8_t addr,uint8_t *str,uint8_t len)  //Ŀ���ַ��Դ��ַ��д���ֽ���
{
	for(uint8_t offset = 0; offset < len; offset++)
	{	
		if(AbNormal == CheckAT24C02(addr+offset,*(str+offset)))
		{
			return AbNormal;
		}
	}
	
	return Normal;
}
void ReadInfo(uint8_t addr,uint8_t *str,uint8_t len)  //��addr��ʼ��ȡ���ݣ�����ȡ����浽str�У���ȡ�ĳ�����len����
{
	for(uint8_t offset = 0; offset < len; offset++)
	{
		*(str+offset) = AT24C02_ReadByte(addr+offset);
	}
}
/************************************* End of the .c file ************************************/
