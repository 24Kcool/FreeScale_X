/*!
 *
 * @file       XK_3010.c
 * @brief      С������ ǰ�ֶ������
 * @author     ����
 * @version    v8.11
 * @date       2017-05-21
 */

#include "common.h"
#include "include.h"

/*
* ������������
*/
#define S3010_FTM   FTM2
#define S3010_CH    FTM_CH1     //FTM2_CH1    PTB19
#define S3010_HZ    (100)

void control(float angle)
{

    ftm_pwm_init(S3010_FTM, S3010_CH,S3010_HZ,100);         // FTM0_PRECISON ���� Ϊ 100 ����ռ�ձ� Ϊ 100%
                                                           // port_cfg.h �� ���� FTM2_CH1 ��ӦΪ PTB19
   
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

