/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       main.c
 * @brief      ���ܳ�  ����ѭ��
 * @author     ����
 * @version    v8.11
 * @date       2017-5-24
 */

#include "common.h"
#include "include.h"

#define BIN_MAX 0xC8   //ccdʹ��
#define CCD_EXPOSURE 60  //ccd�ع�ʱ��

uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //����ccd�ɼ����ݵ�����
float direction=5;   //С�����������  ��ʼ������5����
uint8 max_d=0;   //ccdÿ����������ֵ
uint8 max = 0;   //ccd�вɼ�������ֵ
int val=0;     //����������ɼ�����

/*
*  ����ccd�����ֺ�������
*/
void PIT0_IRQHandler();
//void IRQ_CCD_cal();  //ccd���ݴ�����жϺ���
void PIT_VAL();

void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif);
void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);
void bin_xk(uint8 *buf, uint16 len);
void tsl1401ccd_init();
int ccd_cal();




void main(void)
{
/*
*   �²����Ƕ���������Ҫ�������ֵ�Ķ��弰��ʼ��
*/
    //int val=0;     //����������ɼ�����
    uint8 state=0;   //����״̬
    int velocity=0;//С���ٶȿ��� 
    int a=0;
/*
*  �����Ƕ����нӿ��Լ�����ĳ�ʼ��
*/    
    led_init(LED0);      //PTD15
    led_init(LED1);      //PTA17
    led_init(LED2);      //PTC0
    led_init(LED3);      //PTE26
    key_init(KEY_A);     //PTB20
    key_init(KEY_B);     //PTB21
    key_init(KEY_C);     //PTB22
    tsl1401ccd_init();   //ccd�ĳ�ʼ��
    ftm_quad_init(FTM1); //512·��������ʼ��
    motion(velocity,0);
    control(direction);
    
/*
*  ������ĸ���ϵ���ָʾ��
*/
    led(LED0,LED_ON);     DELAY_MS(200);
    led(LED1,LED_ON);     DELAY_MS(200);
    led(LED2,LED_ON);     DELAY_MS(200);
    led(LED3,LED_ON);     DELAY_MS(200);
    led(LED0,LED_OFF);  led(LED1,LED_OFF);  led(LED2,LED_OFF);  led(LED3,LED_OFF);
   
    pit_init_ms(PIT1,50);
    set_vector_handler(PIT1_VECTORn,PIT_VAL);
    enable_irq(PIT1_IRQn);
;
/*
*  ��������Ҫ����ѭ������
*   
*/  
/*
    while (1)
    {
      if(key_check(KEY_A) == KEY_DOWN)
      {
        state = 1;
        led(LED1,LED_ON);
        for(uint8 i=0;i<6;i++)
        {
          led_turn(LED1);
          led_turn(LED2);
          
          DELAY_MS(100);
        }       
        break;
      }
    }
*/    
    while(1)
    {
      if (key_check(KEY_B) == KEY_DOWN)
      {
        velocity++;
      }
      else
      {
        velocity=velocity;
      }
      
      a=ccd_cal();
      direction=a/60+5;
      //direction = 5+2;
     // int(direction);
      //direction = direction/10+2.1;
      //printf("direction %f\n",direction);
      

      //printf("%d     ",val);
      
      
      if (val<250)
        velocity = velocity + 2;
      else
        velocity = velocity - 2;
      
      
      if (velocity>=20)
        velocity = 20;
      if (velocity<0)
        velocity = 0;
    
      printf("%d\n",velocity);
      motion(velocity,0);  //40 2160  30  1580
      control(direction);    //����Ŀ��Ʒ�Χ0-6-12 ��Ӧ ��-��-��
      //DELAY_MS(0);
      led_turn(LED0);
   
    }
   
    
}

void PIT_VAL()
{
  
   val = ftm_quad_get(FTM1);          //��ȡFTM �������� ��������(������ʾ������)
   ftm_quad_clean(FTM1);              //����
   PIT_Flag_Clear(PIT1);
   
}

/*
*   ����ccd��ʼ��
*/
void  tsl1401ccd_init(void)
{
    uint8 time = CCD_EXPOSURE;

    //��ʼ�� ����CCD
    tsl1401_set_addrs(TSL1401_MAX,(uint8 *)& CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);
    tsl1401_init(time);                         //��ʼ�� ����CCD ������ �ж�ʱ��Ϊ time
    //tsl1401_led_en(TSL1401_MAX);                //���������
    //tsl1401_led_dis(TSL1401_MAX);             //�رղ����

    //���� �жϺ��� �� ʹ���ж�
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);
}




int ccd_cal()
{
      int temp_d=0;
      uint8  max[TSL1401_SIZE];     //ccd���ݴ���
      uint8  avg[TSL1401_SIZE];     //ccd���ݴ���
      tsl1401_get_img();         //�ɼ� ����CCD ͼ��


        //�������ֵ
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        //maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

      filter_xk((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      filter_xk((uint8 *)CCD_BUFF[1], TSL1401_SIZE);
                
        //���β��
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);

        //��ֵ������
        bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
        bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
        //bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

        temp_d = tiaojie(CCD_BUFF);
        //printf("cal %d     ",temp_d);

        //���ݲ�ֲ��ζ�ֵ��ͼ��
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        
        vcan_sendccd((uint8 *)&CCD_BUFF[0],2*TSL1401_SIZE);
        return temp_d;
}



/*!
 *  @brief      PIT0�жϷ�����
 *  @since      v5.0
 *  @note       ���� TSL1401_INT_TIME ���� Ϊ PIT0 �����ʹ�� PIT0
 */
void PIT0_IRQHandler()
{
    tsl1401_time_isr();
    PIT_Flag_Clear(PIT0);
}

void maxvar(uint8 *buf,uint16 len,uint8  maxval)
{
    while(len--)
    {
        if(buf[len] > maxval)
        {
            buf[len]= maxval;
        }
    }

}

/*
*  ȫ����ֵ����
*
*
*
*/
 void bin_xk(uint8 *buf, uint16 len)
 {
   //uint8 threshold = max-max_d-22;  //��������
   uint8 threshold = 200-max_d;
/*   
   int8 bigger [TSL1401_SIZE]={0};
   int8 smaller[TSL1401_SIZE]={0};
   uint8 i=0,k=0,sum1=0,avr1=0;
   uint8 j=0,l=0,sum2=0,avr2=0;
   //uint8 judge=1,z=0;
   
*/
   
  /*   
   while(len--)
   {
     if(buf[len]>=threshold)
     {
       bigger[i]=len;
       i++;
     }
     else
     {
       smaller[j]=len;
       j++;
     }
   }
   for(k=0;k<i;k++)
   {
     sum1+=buf[bigger[k]];
   }
   
   for(l=0;l<j;l++)
   {
     sum2+=buf[smaller[l]];
   }
   
   avr1=sum1/i;
   avr2=sum2/j;
   threshold=(avr1+avr2)/2;
   
   if(abs(threshold-threshold1)<10)
   {
     threshold=threshold1;
     judge=0;
   }
   else
     judge=1; */  
     
    while(len--)
   {
     if(buf[len]>=threshold)
     {
       buf[len] = 255;
     }
     else
     {
       buf[len] = 0;
     }
   }
   

 }


/*!
 *  @brief      �����־���ֵ
 *  @since      v5.0
 *  @note       ɽ���ַ�����˵����������ֵmaxval �� ���ƽ��ֵavgval ������
                ������Ϊ�˱��ڶ���ȷ����ֵ������ģ���ɾ�������ƽ��ֵ��һ�㷵�ؽ��
                ���ǳ�С����˶����ò��ϣ�����ɾ�����˴�������Ϊ�˸������֤��
 */


void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval)
{
    int8 tmp,tmp1;
    //uint8 max_d = 0;   //�ñ�������Ϊȫ�ֱ�����������ǰ��
    uint32 sum = 0;
    uint16 lentmp = len;
    while(--lentmp)                 //��ѭ�� len-1 ��
    {
        tmp1 = *(src+1)- *src;
        tmp = ABS(tmp1) ;
        if(tmp > max_d )
        {
             max_d = tmp;
             if(tmp1 >= 0)
               max = *(src+1);
             else
               max = *src;
        }

        sum += tmp;
        *dst = tmp;
        src++;
        dst++;
    }
    *dst = 0;               // ���һ�� ������Ϊ 0
    *maxval = max_d;           // ���������Բ�ֵ
    *avgval = (uint8)(sum/(len-1));  //ǰ len -1 ������ƽ��ֵ

    printf("max%d     max_d%d\n",max,max_d);
}

/*!
 *  @brief      �򵥵�һ����ֵ�� �㷨�����ȶ�,�����ԣ�
 *  @since      v5.0
 */


// diff_threshold �����ֵ ,��ͬ�ĽǶȣ���ͬ�Ļ�����������ͬ
//�ɸ��� maxdif �����ֵ�����ã�����ֱ�ӹ̶���ֵ
#define diff_threshold    ((maxdif> 12) ? ((maxdif*80)/100) :10)     // �����ֵ
//#define diff_threshold    10
#define safe_isolation    3
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif)
{
    uint16 tmplen = len;
    uint8  thldnum = 0;        //��ֵ����
    uint8  thresholdimg;
    uint8  tmpnum;

    memset(bin,0xFF,len);  //ȫ������

    while(tmplen--)
    {
        if((tmplen == 0)|| (tmplen > len))
        {
            return;
        }

        if(difimg[tmplen] > diff_threshold)                  //�ҵ� �����ֵ
        {
            thldnum++;

            //Ѱ���������ֵ
            while(tmplen--)
            {
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                if(difimg[tmplen] < difimg[tmplen+1] )    //tmplen+1 Ϊ�����ֵ
                {
                     break;
                }
            }

            //tmplen + 1 �� ������ֵ ���л��� ��ɫɨ��
            if((img[tmplen] <= img[tmplen+1]) ||(img[tmplen+1] <= img[tmplen+2]) )  // ǰ�� ��ɫ ������ ��ɫ
            {
                //ѡ�� ���ֵ���ֵ��ǰһ�� ��Ϊ ��ֵ
                thresholdimg = (img[tmplen+1] + img[tmplen+2])/2;

                //ɨ����һ�� ���� ����ֵ (�ȴ˵����)
                while(img[tmplen] <= thresholdimg)
                {
                    bin[tmplen] = 0;                //��ɫ
                    tmplen--;
                    if(tmplen == 0)      //��β�� ,ֱ���˳�
                    {
                        if(img[tmplen] <= thresholdimg)
                        {
                             bin[tmplen] = 0;                //��ɫ
                        }
                        return ;
                    }
                    else if (tmplen > len)
                    {
                         return;
                    }
                }
                tmplen -= safe_isolation;
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                //�ȴ����ֵ����
                while(difimg[tmplen] > diff_threshold)
                {
                    tmplen--;
                    if((tmplen == 0)|| (tmplen > len))
                    {
                        return;
                    }
                }
            }
            else
            {
                //ǰ�� �� ��
                if(thldnum == 1)
                {
                    //����� ���ݶ��� ��ɫ��
                    tmpnum  = tmplen + 1;
                    while(tmpnum < len)
                    {
                        bin[tmpnum] = 0;                //��ɫ
                        tmpnum ++;
                    }
                }
            }


        }
    }
}



/*
void IRQ_CCD_cal()
{
   tsl1401_get_img();            //�ɼ� ����CCD ͼ��
   uint8  max[TSL1401_SIZE];     //ccd���ݴ���
   uint8  avg[TSL1401_SIZE];     //ccd���ݴ���



   //�������ֵ
   maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
   maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
  //maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);
                
   //���β��
   abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
   abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
   //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);

   //��ֵ������
   bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
   bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
   //bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

   direction = tiaojie(CCD_BUFF)/60+5;
   //printf("cal %d     ",temp_d);

   //���ݲ�ֲ��ζ�ֵ��ͼ��
   //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
   //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
   //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        
   vcan_sendccd((uint8 *)&CCD_BUFF[0],2*TSL1401_SIZE);
   PIT_Flag_Clear(PIT1);
}*/