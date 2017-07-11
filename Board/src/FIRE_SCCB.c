#include "include.h"
#include "FIRE_SCCB.h"




void SCCB_GPIO_init(void)
{
	port_init(SCCB_SCL, ALT1);
	port_init(SCCB_SDA, ALT1/* | PULLDOWN*/);
}

/************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�sccb_wait
*  ����˵����SCCB��ʱ����Ӧ̫С
*  ����˵������
*  �������أ���
*
*************************************************************************/
void sccb_wait(void)
{
	unsigned int i;

	for (i = 0; i<18 * 4; i++)		//��Ӧ�ں�ʱ��Ϊ180mhz.�������ں�ʱ�ӵĻ�Ҫ��Ӧ����
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
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�sccb_start
*  ����˵����SCCB����λ
*  ����˵������
*  �������أ���
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
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�sccb_stop
*  ����˵����SCCBֹͣλ
*  ����˵������
*  �������أ���
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
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�sccb_sendByte
*  ����˵������SCCB�����Ϸ���һ���ֽ�
*  ����˵����data Ҫ���͵��ֽ�����
*  �������أ���
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
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�sccb_regWrite
*  ����˵����ͨ��SCCB������ָ���豸��ָ����ַ����ָ������
*  ����˵����device---�豸��  ��д������
*            address---д���ݵļĴ���
*            data---д������
*  �������أ�ack=1δ�յ�Ӧ��(ʧ��)    ack=0�յ�Ӧ��(�ɹ�)
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