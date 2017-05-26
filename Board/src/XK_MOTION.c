/*!
 *
 * @file       XK_3010.c
 * @brief      小车控制 后轮电机控制
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-22
 */

#include "common.h"
#include "include.h"

/*
* 电机输出口声明
*/

#define LIFT_FTM    FTM0
#define LIFT_CH     FTM_CH0     //FTM0_CH0    PTC1
#define RIGHT_FTM   FTM0
#define RIGHT_CH    FTM_CH1     //FTM0_CH1    PTC2
#define LIFT_FTM1    FTM0
#define LIFT_CH1     FTM_CH2     //FTM0_CH0    PTC3
#define RIGHT_FTM1   FTM0
#define RIGHT_CH1    FTM_CH3     //FTM0_CH1    PTC4
#define MOTION_HZ   (1000)


void motion(uint8 left, uint8 right)
{
    //uint8 i = 0;
    ftm_pwm_init(LIFT_FTM,  LIFT_CH,  MOTION_HZ, 100);           // FTM0_PRECISON 配置 为 100 ，即占空比 为 100%
    ftm_pwm_init(RIGHT_FTM, RIGHT_CH, MOTION_HZ, 100);
    
//    ftm_pwm_init(LIFT_FTM1,  LIFT_CH1,  MOTION_HZ, 100);           // FTM0_PRECISON 配置 为 100 ，即占空比 为 100%
//    ftm_pwm_init(RIGHT_FTM1, RIGHT_CH1, MOTION_HZ, 100);
    
    ftm_pwm_duty(LIFT_FTM,  LIFT_CH,  left);       //改变 占空比 ，K60 输出 PWM 占空比 逐渐增大，电机逐渐 降速
    ftm_pwm_duty(RIGHT_FTM, RIGHT_CH, right);
//    ftm_pwm_duty(LIFT_FTM1,  LIFT_CH1,  left2);       //改变 占空比 ，K60 输出 PWM 占空比 逐渐增大，电机逐渐 降速
//    ftm_pwm_duty(RIGHT_FTM1, RIGHT_CH1, right2);
    
    /*
    while(1)
    {
        for(i= 0;i<=60;i+=10)
        {
            ftm_pwm_duty(FTM0,FTM_CH0,i);       //改变 占空比 ，K60 输出 PWM 占空比 逐渐增大，电机逐渐 降速
            ftm_pwm_duty(FTM0,FTM_CH1,i);
            DELAY_MS(50);
        }
    }*/
}