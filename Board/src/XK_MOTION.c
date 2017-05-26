/*!
 *
 * @file       XK_3010.c
 * @brief      С������ ���ֵ������
 * @author     ����
 * @version    v8.11
 * @date       2017-05-22
 */

#include "common.h"
#include "include.h"

/*
* ������������
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
    ftm_pwm_init(LIFT_FTM,  LIFT_CH,  MOTION_HZ, 100);           // FTM0_PRECISON ���� Ϊ 100 ����ռ�ձ� Ϊ 100%
    ftm_pwm_init(RIGHT_FTM, RIGHT_CH, MOTION_HZ, 100);
    
//    ftm_pwm_init(LIFT_FTM1,  LIFT_CH1,  MOTION_HZ, 100);           // FTM0_PRECISON ���� Ϊ 100 ����ռ�ձ� Ϊ 100%
//    ftm_pwm_init(RIGHT_FTM1, RIGHT_CH1, MOTION_HZ, 100);
    
    ftm_pwm_duty(LIFT_FTM,  LIFT_CH,  left);       //�ı� ռ�ձ� ��K60 ��� PWM ռ�ձ� �����󣬵���� ����
    ftm_pwm_duty(RIGHT_FTM, RIGHT_CH, right);
//    ftm_pwm_duty(LIFT_FTM1,  LIFT_CH1,  left2);       //�ı� ռ�ձ� ��K60 ��� PWM ռ�ձ� �����󣬵���� ����
//    ftm_pwm_duty(RIGHT_FTM1, RIGHT_CH1, right2);
    
    /*
    while(1)
    {
        for(i= 0;i<=60;i+=10)
        {
            ftm_pwm_duty(FTM0,FTM_CH0,i);       //�ı� ռ�ձ� ��K60 ��� PWM ռ�ձ� �����󣬵���� ����
            ftm_pwm_duty(FTM0,FTM_CH1,i);
            DELAY_MS(50);
        }
    }*/
}