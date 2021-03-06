/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       XK_TSL1401CCD.c
 * @brief      小车控制 线性CCD模块
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-23
 */

#include "common.h"
#include "include.h"
#include "XK_tsl1401ccd.h"

uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //定义ccd采集数据的数组


/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       山外 线性CCD 测试实验
                修改 PIT0 的定时时间即可修改曝光时间
 */

void  tsl1401ccd_init(void)
{

    uint8 time = 100;

    //初始化 线性CCD
    tsl1401_set_addrs(TSL1401_MAX,(uint8 *)& CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);
    tsl1401_init(time);                         //初始化 线性CCD ，配置 中断时间为 time
    //tsl1401_led_en(TSL1401_MAX);                //开启补光灯
    //tsl1401_led_dis(TSL1401_MAX);             //关闭补光灯

    //配置 中断函数 和 使能中断
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);
    
            tsl1401_get_img();         //采集 线性CCD 图像


        //限制最大值
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

        //求波形差分
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);


        //根据差分波形二值化图像
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        vcan_sendccd((uint8 *)&CCD_BUFF[0],1*TSL1401_SIZE);

}

/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 *  @note       由于 TSL1401_INT_TIME 配置 为 PIT0 ，因而使用 PIT0
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
 *  @brief      计算差分绝对值
 *  @since      v5.0
 *  @note       山外差分法补充说明：差分最大值maxval 和 差分平均值avgval 这两个
                参数是为了便于定义确定阈值而加入的，可删除。差分平均值，一般返回结果
                都非常小，因此顶层用不上，建议删掉（此处保留是为了给大家验证）
 */


void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval)
{
    int8 tmp;
    uint8 max = 0;
    uint32 sum = 0;
    uint16 lentmp = len;
    while(--lentmp)                 //仅循环 len-1 次
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
    *dst = 0;               // 最后一个 点配置为 0
    *maxval = max;           // 返回最大绝对差值
    *avgval = (uint8)(sum/(len-1));  //前 len -1 个数的平均值
}

/*!
 *  @brief      简单的一个二值化 算法（不稳定,仅测试）
 *  @since      v5.0
 */


// diff_threshold 差分阈值 ,不同的角度，不同的环境而有所不同
//可根据 maxdif 最大差分值来配置，或者直接固定阈值
#define diff_threshold    ((maxdif> 12) ? ((maxdif*80)/100) :10)     // 差分阈值
//#define diff_threshold    10
#define safe_isolation    3
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif)
{
    uint16 tmplen = len;
    uint8  thldnum = 0;        //阈值次数
    uint8  thresholdimg;
    uint8  tmpnum;

    memset(bin,0xFF,len);  //全部当作

    while(tmplen--)
    {
        if((tmplen == 0)|| (tmplen > len))
        {
            return;
        }

        if(difimg[tmplen] > diff_threshold)                  //找到 差分阈值
        {
            thldnum++;

            //寻找最大差分阈值
            while(tmplen--)
            {
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                if(difimg[tmplen] < difimg[tmplen+1] )    //tmplen+1 为最大阈值
                {
                     break;
                }
            }

            //tmplen + 1 是 差分最大值 ，切换到 颜色扫描
            if((img[tmplen] <= img[tmplen+1]) ||(img[tmplen+1] <= img[tmplen+2]) )  // 前面 黑色 ，后面 白色
            {
                //选择 差分值最大值的前一个 作为 阈值
                thresholdimg = (img[tmplen+1] + img[tmplen+2])/2;

                //扫描下一个 高于 此阈值 (比此点更白)
                while(img[tmplen] <= thresholdimg)
                {
                    bin[tmplen] = 0;                //黑色
                    tmplen--;
                    if(tmplen == 0)      //结尾了 ,直接退出
                    {
                        if(img[tmplen] <= thresholdimg)
                        {
                             bin[tmplen] = 0;                //黑色
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

                //等待差分值降低
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
                //前白 后 黑
                if(thldnum == 1)
                {
                    //后面的 内容都是 黑色的
                    tmpnum  = tmplen + 1;
                    while(tmpnum < len)
                    {
                        bin[tmpnum] = 0;                //黑色
                        tmpnum ++;
                    }
                }
            }


        }
    }
}


/*
*   线性ccd初始化
*/
/*
void  tsl1401ccd_init(void)
{
    uint8 time = CCD_EXPOSURE;

    //初始化 线性CCD
    tsl1401_set_addrs(TSL1401_MAX,(uint8 *)& CCD_BUFF[0],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[2]);
    tsl1401_init(time);                         //初始化 线性CCD ，配置 中断时间为 time
    //tsl1401_led_en(TSL1401_MAX);                //开启补光灯
    //tsl1401_led_dis(TSL1401_MAX);             //关闭补光灯

    //配置 中断函数 和 使能中断
    set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);
    enable_irq(PIT0_IRQn);
}

int ccd_cal()
{
      int temp_d=0;
      //uint8  max[TSL1401_SIZE];     //ccd数据处理
      //uint8  avg[TSL1401_SIZE];     //ccd数据处理
      tsl1401_get_img();         //采集 线性CCD 图像

        //限制最大值
        maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
        maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
        //maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);

      filter_xk((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      filter_xk((uint8 *)CCD_BUFF[1], TSL1401_SIZE);
      
      //qiudao((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      //qiudao((uint8 *)CCD_BUFF[1], TSL1401_SIZE);

      filter_p((uint8 *)CCD_BUFF[0], TSL1401_SIZE);
      filter_p((uint8 *)CCD_BUFF[1], TSL1401_SIZE);
      
      
     
        //求波形差分
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);

        //二值化处理
        //bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
        //bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
        //bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

        temp_d = tiaojie(CCD_BUFF,70);
        //printf("cal %d     ",temp_d);

        //根据差分波形二值化图像
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        
        vcan_sendccd((uint8 *)&CCD_BUFF[0],2*TSL1401_SIZE);
        return temp_d;
}
*/


/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 *  @note       由于 TSL1401_INT_TIME 配置 为 PIT0 ，因而使用 PIT0
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
*  全局阈值处理
*
*
*
*/

/*
 void bin_xk(uint8 *buf, uint16 len)
 {
   //uint8 threshold = max-max_d-22;  //经验数据
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


  
  for(i=0;i<128;i++)//左遍历
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
 *  @brief      计算差分绝对值
 *  @since      v5.0
 *  @note       山外差分法补充说明：差分最大值maxval 和 差分平均值avgval 这两个
                参数是为了便于定义确定阈值而加入的，可删除。差分平均值，一般返回结果
                都非常小，因此顶层用不上，建议删掉（此处保留是为了给大家验证）
 */

/*
void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval)
{
    int8 tmp,tmp1;
    //uint8 max_d = 0;   //该变量设置为全局变量定义在最前端
    uint32 sum = 0;
    uint16 lentmp = len;
    while(--lentmp)                 //仅循环 len-1 次
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
    *dst = 0;               // 最后一个 点配置为 0
    *maxval = max_d;           // 返回最大绝对差值
    *avgval = (uint8)(sum/(len-1));  //前 len -1 个数的平均值

    //printf("max%d     max_d%d\n",max,max_d);
}
*/
/*!
 *  @brief      简单的一个二值化 算法（不稳定,仅测试）
 *  @since      v5.0
 */

/*
// diff_threshold 差分阈值 ,不同的角度，不同的环境而有所不同
//可根据 maxdif 最大差分值来配置，或者直接固定阈值
#define diff_threshold    ((maxdif> 12) ? ((maxdif*80)/100) :10)     // 差分阈值
//#define diff_threshold    10
#define safe_isolation    3
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif)
{
    uint16 tmplen = len;
    uint8  thldnum = 0;        //阈值次数
    uint8  thresholdimg;
    uint8  tmpnum;

    memset(bin,0xFF,len);  //全部当作

    while(tmplen--)
    {
        if((tmplen == 0)|| (tmplen > len))
        {
            return;
        }

        if(difimg[tmplen] > diff_threshold)                  //找到 差分阈值
        {
            thldnum++;

            //寻找最大差分阈值
            while(tmplen--)
            {
                if((tmplen == 0)|| (tmplen > len))
                {
                    return;
                }

                if(difimg[tmplen] < difimg[tmplen+1] )    //tmplen+1 为最大阈值
                {
                     break;
                }
            }

            //tmplen + 1 是 差分最大值 ，切换到 颜色扫描
            if((img[tmplen] <= img[tmplen+1]) ||(img[tmplen+1] <= img[tmplen+2]) )  // 前面 黑色 ，后面 白色
            {
                //选择 差分值最大值的前一个 作为 阈值
                thresholdimg = (img[tmplen+1] + img[tmplen+2])/2;

                //扫描下一个 高于 此阈值 (比此点更白)
                while(img[tmplen] <= thresholdimg)
                {
                    bin[tmplen] = 0;                //黑色
                    tmplen--;
                    if(tmplen == 0)      //结尾了 ,直接退出
                    {
                        if(img[tmplen] <= thresholdimg)
                        {
                             bin[tmplen] = 0;                //黑色
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

                //等待差分值降低
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
                //前白 后 黑
                if(thldnum == 1)
                {
                    //后面的 内容都是 黑色的
                    tmpnum  = tmplen + 1;
                    while(tmpnum < len)
                    {
                        bin[tmpnum] = 0;                //黑色
                        tmpnum ++;
                    }
                }
            }


        }
    }
}

*/