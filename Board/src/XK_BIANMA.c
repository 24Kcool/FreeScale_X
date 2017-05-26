/*!
 *
 * @file       XK_BIANMA.c
 * @brief      С������ 512·������
 * @author     ����
 * @version    v8.11
 * @date       2017-05-22
 */


#include "common.h"
#include "include.h"
#include "XK_bianma.h"

extern int16 bianma_get(void);

/*!
 *  @brief      main����
 *  @since      v5.0
 *  @note       FTM �������� ����
 */
void bianma_init(void)
{

    ftm_quad_init(FTM1);                                    //FTM1 ���������ʼ�������õĹܽſɲ� port_cfg.h ��

//    pit_init_ms(PIT0, 500);                                 //��ʼ��PIT0����ʱʱ��Ϊ�� 1000ms
//    set_vector_handler(PIT0_VECTORn ,bianma_get);      //����PIT0���жϷ�����Ϊ PIT0_IRQHandler
//    enable_irq (PIT0_IRQn);                                 //ʹ��PIT0�ж�

    while(1);
}

/*!
 *  @brief      PIT0�жϷ�����
 *  @since      v5.0
 */
int16 bianma_get(void)
{

    int16 val;
    val = ftm_quad_get(FTM1);          //��ȡFTM �������� ��������(������ʾ������)
    ftm_quad_clean(FTM1);

    PIT_Flag_Clear(PIT0);       //���жϱ�־λ

    return val;
}

