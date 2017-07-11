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
     /*用户添加所需代码*/ 
	static unsigned char TimerCnt20ms = 0;
	unsigned char integration_piont;
	TimerCnt20ms++;
	/* 根据曝光时间计算5ms周期内的曝光点 */
	integration_piont = (unsigned char)(20 - IntegrationTime);
	if (integration_piont >= 2)
	{   /* 曝光点小于2(曝光时间大于18ms)则不进行再曝光 */
		if (integration_piont == TimerCnt20ms)
			Integration();           /* 曝光开始 */
	}
	if (TimerCnt20ms == 20)
	{
		TimerCnt20ms = 0;
		ccd_tsl1401(ccd_data);
		CalculateIntegrationTime();
		/************************发送*************************/
		static unsigned char sendcounter = 0;
		sendcounter++;
		if (sendcounter % 20)
			return;
		sendcounter = 0;
		uart_putchar(UART4, 'T');//上位机的接收头
		//uart_putchar(UART4, 0x02);
		//uart_putchar(UART4, 0xAB);
		for (unsigned char i = 0; i<128; i++)
		{
			if (ccd_data[i] == 255)
				ccd_data[i] = 254;
			uart_putchar(UART4, ccd_data[i]);
		}
		//uart_putchar(UART4, 0xff);//上位机的结束符
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