#include "include.h"
#include "FS_OV7620.h"

unsigned char Pixel_Data[IMG_ROW_ALL+1][IMG_COL_ALL];
unsigned char flags = 0x06;                    //初始化数值绝对不能更改,为了保证当且仅当获取一张新图片时才开始处理图片
unsigned char sendfinished = 1;
unsigned char ROW_Count = 0;
unsigned char(*Image_Data)[IMG_COL_ALL] = (unsigned char(*)[IMG_COL_ALL])Pixel_Data[0];
void OV7620_Init()
{
	gpio_init(PTC0, GPO, 1);
	DELAY_MS(500);
	SCCB_GPIO_init();
	int test = 0;
	test += sccb_regWrite(0x42, 0x11, 0x01);   //device---设备号  读写有区别  address---写数据的寄存器  data---写的内容
	test += sccb_regWrite(0x42, 0x13, 0x01);
	test += sccb_regWrite(0x42, 0x14, 0x24);
	test += sccb_regWrite(0x42, 0x16, 0x03);
	test += sccb_regWrite(0x42, 0x2D, 0xD5);
	test += sccb_regWrite(0x42, 0x28, 0x00);
	test += sccb_regWrite(0x42, 0x70, 0x01);
	gpio_set(PTC0, 0);

	//DMA通道0初始化，PTA27触发源(默认上升沿)，源地址为PTB_B0_IN，目的地址为：Pixel_Data，每次传输1Byte
	dma_portx2buff_init(CAMERA_DMA_CH, (void *)&INPUT_PORT, (void *)Pixel_Data, PT_PCLK, DMA_BYTE1, IMG_COL_ALL, DADDR_KEEPON);

	disable_irq(PT_CAMERA_IRQ);                        //关闭PT_IRQ的中断
	//DMA_IRQ_EN(CAMERA_DMA_CH);					   //不打开传输完成中断。因为完成后硬件会自动停止硬件请求

	port_init(PT_PCLK, ALT1 | DMA_RISING | PULLUP);         //PCLK
	port_init(PT_HRFF, ALT1 | IRQ_RISING | PULLDOWN);
	port_init(PT_VSYNC, ALT1 | IRQ_FALLING | PULLUP);     //场中断，上拉，上升沿触发中断
	
	flags = 0x00;
        //set_vector_handler(PT_CAMERA_IRQ,portc_irq_handler);
	enable_irq(PT_CAMERA_IRQ);
	//DELAY_MS(50);
	flags = 0x06;                    //初始化数值绝对不能更改,为了保证当且仅当获取一张新图片时才开始处理图片
										 //将车从其它模式改回运行模式时都要先清除中断标志位然后对这个变量重新赋值成0x06
}
void Send_Img()
{
	int ind_i, ind_j;
	uart_putchar(VCAN_PORT, 0xFF);
	for (ind_i = 0; ind_i < IMG_ROW;++ind_i)
		for (ind_j = 0; ind_j < IMG_COL;++ind_j)
		{
			if (0xFF == LUT(Image_Data, ind_i, ind_j))
				uart_putchar(VCAN_PORT, 0xFE);
			else
				uart_putchar(VCAN_PORT, LUT(Image_Data, ind_i, ind_j));
		}
}

void HRFF_handler()
{
    if(ROW_Count>IMG_ROW_ALL)
        return;
    DMA_EN(CAMERA_DMA_CH);
    ROW_Count++;
//	if (ROW_Count & 0x01)
//		DMA_EN(CAMERA_DMA_CH);
//	ROW_Count++;
	/***********************************************/
	//if (ROW_Count == IMG_ROW_ALL)//图像采集够了以后
	//{
	//	ROW_Count = 0; //行计数清零
	//	DMA_DIS(DMA_CH4);
	//	disable_irq(PT_CAMERA_IRQ);  //关闭PORTE的中断
	//}
}

void VSYNC_handler()
{
	if (sendfinished == 0)
		return;
    DMA_DADDR(CAMERA_DMA_CH) = (uint32)Pixel_Data[0];
//	if (flags & 0x80)
//	{
//		DMA_DADDR(CAMERA_DMA_CH) = (uint32)Pixel_Data[0];
//		Image_Data = (unsigned char(*)[IMG_COL_ALL])Pixel_Data[IMG_ROW];
//	}
//	else
//	{
//		DMA_DADDR(CAMERA_DMA_CH) = (uint32)Pixel_Data[IMG_ROW];
//		Image_Data = (unsigned char(*)[IMG_COL_ALL])Pixel_Data[0];
//	}
	ROW_Count = 0;
	flags ^= 0x80;
	CLEAR_WAIT;
	sendfinished = 0;
}
