#include "include.h"
#include "FS_OV7620.h"

unsigned char Pixel_Data[IMG_ROW_ALL+1][IMG_COL_ALL];
unsigned char flags = 0x06;                    //��ʼ����ֵ���Բ��ܸ���,Ϊ�˱�֤���ҽ�����ȡһ����ͼƬʱ�ſ�ʼ����ͼƬ
unsigned char sendfinished = 1;
unsigned char ROW_Count = 0;
unsigned char(*Image_Data)[IMG_COL_ALL] = (unsigned char(*)[IMG_COL_ALL])Pixel_Data[0];
void OV7620_Init()
{
	gpio_init(PTC0, GPO, 1);
	DELAY_MS(500);
	SCCB_GPIO_init();
	int test = 0;
	test += sccb_regWrite(0x42, 0x11, 0x01);   //device---�豸��  ��д������  address---д���ݵļĴ���  data---д������
	test += sccb_regWrite(0x42, 0x13, 0x01);
	test += sccb_regWrite(0x42, 0x14, 0x24);
	test += sccb_regWrite(0x42, 0x16, 0x03);
	test += sccb_regWrite(0x42, 0x2D, 0xD5);
	test += sccb_regWrite(0x42, 0x28, 0x00);
	test += sccb_regWrite(0x42, 0x70, 0x01);
	gpio_set(PTC0, 0);

	//DMAͨ��0��ʼ����PTA27����Դ(Ĭ��������)��Դ��ַΪPTB_B0_IN��Ŀ�ĵ�ַΪ��Pixel_Data��ÿ�δ���1Byte
	dma_portx2buff_init(CAMERA_DMA_CH, (void *)&INPUT_PORT, (void *)Pixel_Data, PT_PCLK, DMA_BYTE1, IMG_COL_ALL, DADDR_KEEPON);

	disable_irq(PT_CAMERA_IRQ);                        //�ر�PT_IRQ���ж�
	//DMA_IRQ_EN(CAMERA_DMA_CH);					   //���򿪴�������жϡ���Ϊ��ɺ�Ӳ�����Զ�ֹͣӲ������

	port_init(PT_PCLK, ALT1 | DMA_RISING | PULLUP);         //PCLK
	port_init(PT_HRFF, ALT1 | IRQ_RISING | PULLDOWN);
	port_init(PT_VSYNC, ALT1 | IRQ_FALLING | PULLUP);     //���жϣ������������ش����ж�
	
	flags = 0x00;
        //set_vector_handler(PT_CAMERA_IRQ,portc_irq_handler);
	enable_irq(PT_CAMERA_IRQ);
	//DELAY_MS(50);
	flags = 0x06;                    //��ʼ����ֵ���Բ��ܸ���,Ϊ�˱�֤���ҽ�����ȡһ����ͼƬʱ�ſ�ʼ����ͼƬ
										 //����������ģʽ�Ļ�����ģʽʱ��Ҫ������жϱ�־λȻ�������������¸�ֵ��0x06
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
	//if (ROW_Count == IMG_ROW_ALL)//ͼ��ɼ������Ժ�
	//{
	//	ROW_Count = 0; //�м�������
	//	DMA_DIS(DMA_CH4);
	//	disable_irq(PT_CAMERA_IRQ);  //�ر�PORTE���ж�
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
