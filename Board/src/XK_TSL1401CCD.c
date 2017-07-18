/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,ɽ��Ƽ�
 *     All rights reserved.
 *     �������ۣ�ɽ����̳ http://www.vcan123.com
 *
 *     ��ע�������⣬�����������ݰ�Ȩ����ɽ��Ƽ����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣��ɽ��Ƽ��İ�Ȩ������
 *
 * @file       XK_TSL1401CCD.c
 * @brief      С������ ����CCDģ��
 * @author     ����
 * @version    v8.11
 * @date       2017-05-23
 */

#include "common.h"
#include "include.h"
#include "XK_tsl1401ccd.h"

uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //����ccd�ɼ����ݵ�����


/*!
 *  @brief      main����
 *  @since      v5.0
 *  @note       ɽ�� ����CCD ����ʵ��
                �޸� PIT0 �Ķ�ʱʱ�伴���޸��ع�ʱ��
 */

void  tsl1401ccd_init(void)
{

    uint8 time = 100;

    //��ʼ�� ����CCD
    tsl1401_set_addrs(TSL1401_MAX,(uint8 *)& CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);
    tsl1401_init(time);                         //��ʼ�� ����CCD ������ �ж�ʱ��Ϊ time
    //tsl1401_led_en(TSL1401_MAX);                //���������
    //tsl1401_led_dis(TSL1401_MAX);             //�رղ����

    //���� �жϺ��� �� ʹ���ж�
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);
    
            tsl1401_get_img();         //�ɼ� ����CCD ͼ��


        //�������ֵ
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

        //���β��
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);


        //���ݲ�ֲ��ζ�ֵ��ͼ��
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        vcan_sendccd((uint8 *)&CCD_BUFF[0],1*TSL1401_SIZE);

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


/*!
 *  @brief      �����־���ֵ
 *  @since      v5.0
 *  @note       ɽ���ַ�����˵����������ֵmaxval �� ���ƽ��ֵavgval ������
                ������Ϊ�˱��ڶ���ȷ����ֵ������ģ���ɾ�������ƽ��ֵ��һ�㷵�ؽ��
                ���ǳ�С����˶����ò��ϣ�����ɾ�����˴�������Ϊ�˸������֤��
 */


void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval)
{
    int8 tmp;
    uint8 max = 0;
    uint32 sum = 0;
    uint16 lentmp = len;
    while(--lentmp)                 //��ѭ�� len-1 ��
    {
        tmp = *(src+1)- *src;
        tmp = ABS(tmp) ;
        if(tmp > max )
        {
             max = tmp;
        }

        sum += tmp;
        *dst = tmp;
        src++;
        dst++;
    }
    *dst = 0;               // ���һ�� ������Ϊ 0
    *maxval = max;           // ���������Բ�ֵ
    *avgval = (uint8)(sum/(len-1));  //ǰ len -1 ������ƽ��ֵ
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
*   ����ccd��ʼ��
*/
/*
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
      //uint8  max[TSL1401_SIZE];     //ccd���ݴ���
      //uint8  avg[TSL1401_SIZE];     //ccd���ݴ���
      tsl1401_get_img();         //�ɼ� ����CCD ͼ��

        //�������ֵ
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        //maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

      filter_xk((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      filter_xk((uint8 *)CCD_BUFF[1], TSL1401_SIZE);
      
      //qiudao((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      //qiudao((uint8 *)CCD_BUFF[1], TSL1401_SIZE);

      filter_p((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      filter_p((uint8 *)CCD_BUFF[1], TSL1401_SIZE);
      
      
     
        //���β��
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);

        //��ֵ������
        //bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
        //bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
        //bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

        temp_d = tiaojie(CCD_BUFF,70);
        //printf("cal %d     ",temp_d);

        //���ݲ�ֲ��ζ�ֵ��ͼ��
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        
        vcan_sendccd((uint8 *)&CCD_BUFF[0],2*TSL1401_SIZE);
        return temp_d;
}
*/


/*!
 *  @brief      PIT0�жϷ�����
 *  @since      v5.0
 *  @note       ���� TSL1401_INT_TIME ���� Ϊ PIT0 �����ʹ�� PIT0
 */
/*
void PIT0_IRQHandler()
{
    tsl1401_time_isr();
    PIT_Flag_Clear(PIT0);
}
*/
/*
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
*/
/*
*  ȫ����ֵ����
*
*
*
*/

/*
 void bin_xk(uint8 *buf, uint16 len)
 {
   //uint8 threshold = max-max_d-22;  //��������
   uint8 threshold = 200-max_d;
     
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

void qiudao(uint8 *src, uint16 len)
{
  int i=0;
  for(i=0;i<len-1;i++)
  {
    src[i] = src[i+1]-src[i];
  }
  src[127] = src[126]; 
}


void filter_p(uint8 *src, uint16 len)
{
  int i=0;
  int edge1=0,edge2=0,edge3=0,edge4=0;
  int num=0,num1=0;
  uint8 ma[128]={0};
  
  for(i=0;i<128;i++)
  {
    
    if(i==127)
      {
      num1 = num;
      num = 0;
	if(num1>15)
        { edge1 = i + 1 - num1;
        edge2 = 127;
        }
        else
        {
          edge1 = i + 1 - num1;
        edge2 = edge1+1;
        }
      }
    else
    {
    if( (src[i+1] - src[i] )== 0 && (src[i]>=190) )
    {
      num++;     
    }
    
    else
    {
      num1=num;
      num = 0;
      if(num1 > 15)
      {
        edge2 = i;
        edge1 = i+1-num1;
        i=127;
      }
    }
    }
  }


  
  for(i=0;i<128;i++)//�����
  {
    if(i>=0&&i<=edge1)
      src[i]=0;
    else if(i>edge1 && i<edge2)
      src[i]=200;
    else if(i>=edge2)
      src[i]=0;
  }

  
}
*/

/*!
 *  @brief      �����־���ֵ
 *  @since      v5.0
 *  @note       ɽ���ַ�����˵����������ֵmaxval �� ���ƽ��ֵavgval ������
                ������Ϊ�˱��ڶ���ȷ����ֵ������ģ���ɾ�������ƽ��ֵ��һ�㷵�ؽ��
                ���ǳ�С����˶����ò��ϣ�����ɾ�����˴�������Ϊ�˸������֤��
 */

/*
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

    //printf("max%d     max_d%d\n",max,max_d);
}
*/
/*!
 *  @brief      �򵥵�һ����ֵ�� �㷨�����ȶ�,�����ԣ�
 *  @since      v5.0
 */

/*
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

*/