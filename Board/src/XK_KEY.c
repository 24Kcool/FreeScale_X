/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       XK_KEY.c
 * @brief      �������ܵ�ʵ��
 * @author     ����
 * @version    v8.11
 * @date       2017-05-24
 */

#include "common.h"
#include "include.h"

//ȫ�ֱ�������
volatile uint32 irqflag = 0;                //��־λ���壬��0 ��ʾ�����ж�

//��������
void FTM0_INPUT_IRQHandler(void);        //FTM0�жϷ�����


/*!
 *  @brief      main����
 *  @since      v8.11
 *  @note       FTM ���벶׽ ����
 */

void key()
{
    ftm_input_init(FTM0, FTM_CH7, FTM_Falling,FTM_PS_1);     //��ʼ��FTM���벶׽ģʽ���½��ز�׽(FTM0_CH7 Ϊ PTD7)
    port_init_NoALT(FTM0_CH7_PIN ,PULLUP);              //���ö˿�Ϊ����������ԭ�ȵĸ������ã�

    set_vector_handler(FTM0_VECTORn ,FTM0_INPUT_IRQHandler);    //����FTM0���жϷ�����Ϊ FTM0_INPUT_IRQHandler
    enable_irq (FTM0_IRQn);                                     //ʹ��FTM0�ж�

    led_init(LED0);                 //��ʼ��LED�˿�

    while(1)
    {
        if(irqflag != 0)
        {
            led_turn(LED0);         //ͨ�� LED ��Ч��
            DELAY_MS(200);          //������ͨ���������£��鿴LED״̬�������Ҫ������ʱ����
            irqflag = 0;
        }
    }
}


void FTM0_INPUT_IRQHandler(void)
{
    uint8 s = FTM0_STATUS;          //��ȡ��׽�ͱȽ�״̬  All CHnF bits can be checked using only one read of STATUS.
    uint8 CHn;

    FTM0_STATUS = 0x00;             //���жϱ�־λ

    CHn = 7;
    if( s & (1 << CHn) )
    {
        /*     �û�����       */
        irqflag++;

        /*********************/
    }

}