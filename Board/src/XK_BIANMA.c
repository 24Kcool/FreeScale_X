/*!
 *
 * @file       XK_BIANMA.c
 * @brief      小车控制 512路编码器
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-22
 */


#include "common.h"
#include "include.h"
#include "XK_bianma.h"

extern int16 bianma_get(void);

/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       FTM 正交解码 测试
 */
void bianma_init(void)
{

    ftm_quad_init(FTM1);                                    //FTM1 正交解码初始化（所用的管脚可查 port_cfg.h ）

//    pit_init_ms(PIT0, 500);                                 //初始化PIT0，定时时间为： 1000ms
//    set_vector_handler(PIT0_VECTORn ,bianma_get);      //设置PIT0的中断服务函数为 PIT0_IRQHandler
//    enable_irq (PIT0_IRQn);                                 //使能PIT0中断

    while(1);
}

/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 */
int16 bianma_get(void)
{

    int16 val;
    val = ftm_quad_get(FTM1);          //获取FTM 正交解码 的脉冲数(负数表示反方向)
    ftm_quad_clean(FTM1);

    PIT_Flag_Clear(PIT0);       //清中断标志位

    return val;
}

