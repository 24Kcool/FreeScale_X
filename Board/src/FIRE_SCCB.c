#include "include.h"
#include "FIRE_SCCB.h"




void SCCB_GPIO_init(void)
{
	port_init(SCCB_SCL, ALT1);
	port_init(SCCB_SDA, ALT1/* | PULLDOWN*/);
}

/************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：sccb_wait
*  功能说明：SCCB延时，不应太小
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void sccb_wait(void)
{
	unsigned int i;

	for (i = 0; i<18 * 4; i++)		//对应内核时钟为180mhz.用其它内核时钟的话要相应更改
	{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}
/************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：sccb_start
*  功能说明：SCCB启动位
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void sccb_start(void)
{
	SCL_OUT();
	SDA_OUT();

	SDA_HIGH();
	SCL_HIGH();
	sccb_wait();
	sccb_wait();
	SDA_LOW();
	sccb_wait();
	SCL_LOW();
	sccb_wait();
}

/************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：sccb_stop
*  功能说明：SCCB停止位
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void sccb_stop(void)
{
	SCL_OUT();
	SDA_OUT();

	//SDA_LOW();
	//sccb_wait();
	SCL_HIGH();
	sccb_wait();
	SDA_HIGH();
	sccb_wait();
}

/************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：sccb_sendByte
*  功能说明：在SCCB总线上发送一个字节
*  参数说明：data 要发送的字节内容
*  函数返回：无
*
*************************************************************************/
uint8 sccb_sendByte(uint8 data)
{
	uint8 i;
	uint8 ack;
	SDA_OUT();
	for (i = 0; i<8; i++)
	{
		if (data & 0x80)
			SDA_HIGH();
		else
			SDA_LOW();
		data <<= 1;
		sccb_wait();
		SCL_HIGH();
		sccb_wait();
		SCL_LOW();
		sccb_wait();
	}
	SDA_LOW();//00
	SDA_IN();
	sccb_wait();
	SCL_HIGH();
	sccb_wait();
	ack = SDA_DATA();
	SCL_LOW();
	sccb_wait();
	return ack;
}


/************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：sccb_regWrite
*  功能说明：通过SCCB总线向指定设备的指定地址发送指定内容
*  参数说明：device---设备号  读写有区别
*            address---写数据的寄存器
*            data---写的内容
*  函数返回：ack=1未收到应答(失败)    ack=0收到应答(成功)
*
*************************************************************************/
uint8 sccb_regWrite(uint8 device, uint8 address, uint8 data)
{
	uint8 i;
	uint8 ack;
	for (i = 0; i<200; i++)
	{
		sccb_wait();

		sccb_start();
		ack = sccb_sendByte(device);
		if (ack == 1)
		{
			sccb_stop();
			continue;
		}

		ack = sccb_sendByte(address);
		if (ack == 1)
		{
			sccb_stop();
			continue;
		}

		ack = sccb_sendByte(data);
		if (ack == 1)
		{
			sccb_stop();
			continue;
		}

		sccb_stop();
		if (ack == 0)
			return 0;

	}
	return 1;
}