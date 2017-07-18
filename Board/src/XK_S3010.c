/*!
 *
 * @file       XK_3010.c
 * @brief      小车控制 前轮舵机控制
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-21
 */

#include "common.h"
#include "include.h"

/*
* 舵机输出口声明
*/
#define S3010_FTM   FTM2
#define S3010_CH    FTM_CH1     //FTM2_CH1    PTB19
#define S3010_HZ    (100)

void control(float angle)
{

    ftm_pwm_init(S3010_FTM, S3010_CH,S3010_HZ,100);         // FTM0_PRECISON 配置 为 100 ，即占空比 为 100%
                                                           // port_cfg.h 里 配置 FTM2_CH1 对应为 PTB19
   
    float intern = 9+angle;
    ftm_pwm_duty(S3010_FTM, S3010_CH,intern);  
   /*
    float i;
    while(1)
    {
        
        for(i = 6.5;i<7.7;i=i+0.2)
        {
            ftm_pwm_duty(S3010_FTM, S3010_CH,i);
            DELAY_MS(100);
        }
        
        for(;i>6.5;i=i-0.2)
        {
            ftm_pwm_duty(S3010_FTM, S3010_CH,i);
            DELAY_MS(100);
        }
  
    }
    */
}

