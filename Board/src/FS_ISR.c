#include "include.h"
#include "FS_ISR.h"

void uart0_handler()
{
	uart_irq_handler();
}
void pit20ms_handler()
{
	PIT_Flag_Clear(PIT_20ms);
#ifdef DEBUG
	static int counter = 0;
	if (/*ounter % 50 ==*/ 0)
	{
		//uart_putchar(UART4, '8');
        uint16 val=ftm_quad_get(FTM1);
		printf("%u\n",val);
	}
	++counter;
#endif // DEBUG
}
void pit0_handler()
{
	PIT_Flag_Clear(PIT0);
     /*�û�����������*/ 
	static unsigned char TimerCnt20ms = 0;
	unsigned char integration_piont;
	TimerCnt20ms++;
	/* �����ع�ʱ�����5ms�����ڵ��ع�� */
	integration_piont = (unsigned char)(20 - IntegrationTime);
	if (integration_piont >= 2)
	{   /* �ع��С��2(�ع�ʱ�����18ms)�򲻽������ع� */
		if (integration_piont == TimerCnt20ms)
			Integration();           /* �ع⿪ʼ */
	}
	if (TimerCnt20ms == 20)
	{
		TimerCnt20ms = 0;
		ccd_tsl1401(ccd_data);
		CalculateIntegrationTime();
		/************************����*************************/
		static unsigned char sendcounter = 0;
		sendcounter++;
		if (sendcounter % 20)
			return;
		sendcounter = 0;
		uart_putchar(UART4, 'T');//��λ���Ľ���ͷ
		//uart_putchar(UART4, 0x02);
		//uart_putchar(UART4, 0xAB);
		for (unsigned char i = 0; i<128; i++)
		{
			if (ccd_data[i] == 255)
				ccd_data[i] = 254;
			uart_putchar(UART4, ccd_data[i]);
		}
		//uart_putchar(UART4, 0xff);//��λ���Ľ�����
	}
}
void portc_irq_handler()
{
	if ((PORTC_ISFR & (1 << 7)))
	{
		PORTC_ISFR |= (1 << 7);
		VSYNC_handler();
	}	
	if ((PORTC_ISFR & (1 << 6)))
	{
		PORTC_ISFR |= (1 << 6);
		HRFF_handler();
	}
}

void porte_irq_handler()
{
}