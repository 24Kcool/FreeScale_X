#include "include.h"
#include "FS_CCD.h"

#define TSL_SI   PTE3_OUT   //E3
#define TSL_CLK  PTE2_OUT   //E2

unsigned char ccd_data[128];

int CCD_AVG_E=0;
unsigned int CCD_MAX=0,CCD_MIN,CCD_AVG=0;
unsigned char TimerFlag20ms,IntegrationTime;

void Dly_us(unsigned int us)
{
  while(us--)
  {
    for(uint16 i=0;i<180;i++)
     asm("nop");  
  }
}

void ccd_init(void) 
{
  gpio_init (PTE2, GPO,0);
  gpio_init (PTE3, GPO,0);

  adc_init(ADC0_DM1);//ADC0_DM1
}


void Integration(void)   //曝光函数
{
    unsigned char i;
    
    TSL_SI=1;            /* SI  = 1 */
    Dly_us(1);
    TSL_CLK = 1;         /* CLK = 1 */
    Dly_us(1);
    TSL_SI=0;            /* SI  = 0 */
    Dly_us(1);
    TSL_CLK = 0;         /* CLK = 0 */

    for(i=0; i<127; i++) 
    {
        Dly_us(1);
        TSL_CLK = 1;       /* CLK = 1 */
        Dly_us(1);
        TSL_CLK = 0;       /* CLK = 0 */
    }
    Dly_us(1);
    TSL_CLK = 1;           /* CLK = 1 */
    Dly_us(1);
    TSL_CLK = 0;           /* CLK = 0 */
}



void ccd_tsl1401(unsigned char *p) 
{
	unsigned char i;

	TSL_SI=1;;            //SI  = 1 
	Dly_us(1);
	TSL_CLK = 1;           // CLK = 1 
	Dly_us(1);
	TSL_SI=0;           // SI  = 0 
	Dly_us(1);

	for(i=0; i<128; i++) 
	{
		Dly_us(1);
		TSL_CLK = 1;       // CLK = 1 
		Dly_us(1);
		//Sampling Pixel 2~128
		p[i] = adc_once(ADC0_DM1, ADC_8bit); 
		TSL_CLK = 0;       // CLK = 0 
	}
	Dly_us(1);
	TSL_CLK = 1;           // CLK = 1 
	Dly_us(1);
	TSL_CLK = 0;           // CLK = 0 
}
void CalculateIntegrationTime(void)
{
    unsigned char i;
    unsigned int CCD_SUM=0;
    // 计算像素点的平均AD值 
    for(i=PIX_START;i<PIX_END;i++)
    {
    	CCD_SUM += ccd_data[i];
    }
    CCD_AVG = (unsigned short int) (CCD_SUM / PIX_NUM);   //求平均值

    CCD_AVG_E = (int)2500 - (int)CCD_AVG;
    
    if(CCD_AVG_E < -500)
        IntegrationTime--;
    if(CCD_AVG_E > 200)
        IntegrationTime++;
    if(IntegrationTime <= 1)
        IntegrationTime = 1;
    if(IntegrationTime >= 20)
        IntegrationTime = 20;
}

void read_ccd(void)
{
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
	}
}