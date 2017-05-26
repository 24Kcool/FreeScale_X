/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,山外科技
 *     All rights reserved.
 *     技术讨论：山外论坛 http://www.vcan123.com
 *
 *     除注明出处外，以下所有内容版权均属山外科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留山外科技的版权声明。
 *
 * @file       main.c
 * @brief      智能车  主控循环
 * @author     许珂
 * @version    v8.11
 * @date       2017-5-24
 */

#include "common.h"
#include "include.h"

#define BIN_MAX 0x80   //ccd使用

uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //定义ccd采集数据的数组
float direction=5;   //小车方向控制量  初始量定义5归中

/*
*  线性ccd处理部分函数声明
*/
void PIT0_IRQHandler();
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif);
void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);
void bin_xk(uint8 *buf, uint16 len);
void tsl1401ccd_init();
int ccd_cal();

int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //直到保持稳定1
int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]); //计算两根标识线之间的距离
int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0);  //判断是否又弯道
int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //是否有两条线
int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);     //转弯
int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);      //调节函数

void IRQ_CCD_cal();


void main(void)
{
/*
*   下部分是对于所有需要处理的数值的定义及初始化
*/
    int val=0;     //定义编码器采集数据
    uint8 state=0;   //定义状态
    int velocity=0;//小车速度控制 
    int a=0;
/*
*  以下是对所有接口以及外设的初始化
*/    
    led_init(LED0);      //PTD15
    led_init(LED1);      //PTA17
    led_init(LED2);      //PTC0
    led_init(LED3);      //PTE26
    key_init(KEY_A);     //PTB20
    key_init(KEY_B);     //PTB21
    key_init(KEY_C);     //PTB22
    tsl1401ccd_init();   //ccd的初始化
    ftm_quad_init(FTM1); //512路编码器初始化
    motion(velocity,velocity);
    control(direction);
    
/*
*  以下是母板上电后的指示灯
*/
    led(LED0,LED_ON);     DELAY_MS(200);
    led(LED1,LED_ON);     DELAY_MS(200);
    led(LED2,LED_ON);     DELAY_MS(200);
    led(LED3,LED_ON);     DELAY_MS(200);
    led(LED0,LED_OFF);  led(LED1,LED_OFF);  led(LED2,LED_OFF);  led(LED3,LED_OFF);

    
//    pit_init_ms(PIT1,5);
//    set_vector_handler(PIT1_VECTORn,IRQ_CCD_cal);
//    enable_irq(PIT1_IRQn);
/*
*  以下是主要程序循环部分
*   
*/  
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
    
    while(state)
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
     // int(direction);
      //direction = direction/10+2.1;
      //printf("direction %f\n",direction);
      
      val = ftm_quad_get(FTM1);          //获取FTM 正交解码 的脉冲数(负数表示反方向)
      ftm_quad_clean(FTM1);              //清零
      printf("%d     ",val);
      
      
      if (val<400)
        velocity = velocity + 2;
      else
        velocity = velocity - 2;
      
      

      if (val<0)
        velocity = 0;
    
      printf("%d\n",velocity);
      motion(velocity,velocity);  //40 2160  30  1580
      control(direction);    //舵机的控制范围0-6-12 对应 左-中-右
      //DELAY_MS(0);
      led_turn(LED0);

   
    }
   
    
}




/*
*   线性ccd初始化
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
        maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);
        
        //二值化处理
        bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
        bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
        bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

        temp_d = tiaojie(CCD_BUFF);
        //printf("cal %d     ",temp_d);
        
        //求波形差分
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],TSL1401_SIZE,&max[0],&avg[0]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],TSL1401_SIZE,&max[1],&avg[1]);
        //abs_diff((uint8 *)&CCD_BUFF[TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],TSL1401_SIZE,&max[2],&avg[2]);


        //根据差分波形二值化图像
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+0],(uint8 *)&CCD_BUFF[0],(uint8 *)&CCD_BUFF[TSL1401_MAX+0],TSL1401_SIZE,max[0]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+1],(uint8 *)&CCD_BUFF[1],(uint8 *)&CCD_BUFF[TSL1401_MAX+1],TSL1401_SIZE,max[1]);
        //bin((uint8 *)&CCD_BUFF[2*TSL1401_MAX+2],(uint8 *)&CCD_BUFF[2],(uint8 *)&CCD_BUFF[TSL1401_MAX+2],TSL1401_SIZE,max[2]);
        
        
        //vcan_sendccd((uint8 *)&CCD_BUFF[0],2*TSL1401_SIZE);
        return temp_d;
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

/*
*  全局阈值处理
*
*
*
*/
 void bin_xk(uint8 *buf, uint16 len)
 {
   uint8 threshold = 100;
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

int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int i = 0 ;
	int r=0,p = 0;
	int l=0;
	int out = 0;
	while (i < 126)
	{
		if (a[1][i] < a[1][i+1])
		{
			if (p == 0)
			{
				r = i;
				p = 1;
			}
			l = i;
		}
		i = i + 1;
	}
	l = 128 - l;
	out = l - r;
        //printf("zhidao:%d     ",out);
	return out;//返回值，左边空余大小减去右边空余大小，正值往左转，负值往右转
}

int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int i = 0, p = 0;
	int r=0;
	int l=0;
	int out=0;
	int L = 0;
	while (i < 126)
	{
		if (a[1][i] < a[1][i + 1])
		{
			if (p == 0)
			{
				r = i;
				p = 1;
			}
			l = i;
		}
		i = i + 1;
	}
	L = l - r;
        //printf("changdu%d     ",L);
	return L;
}

int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0)
{
	int l=0;
	l = jisuanLdechangdu(a);
	if (l > (L0 - 5) && l < (L0 + 5))
	{
		return 1;//没有弯道
	}
        
	return 0;//有弯道
}

int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int ge = 0;
	int i = 0;
	int f = 0;
	while (i < 126)
	{
		if (a[1][i] < a[1][i + 1])
		{
			ge = ge + 1;
		}
		i = i + 1;
	}
	if (ge == 2)
	{
		f = 1;
		return f;//有两根线
	}
	f = 0;
	return f;
}


//当延时函数结束之后，我们需要计算CCD中心距离线的距离，之后跟着一条

int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int i = 0;
	int p = 0;
	int out = 0;
	while (i < 126)
	{
		if (a[1][i] != a[1][i + 1])
		{
			p = i;
		}
	}
	out = 63 - p;//由这个值计算偏转量，如果小于0则是向右，大于0则是向左。
//printf("zhuanwan%d     ",out);
	return out;
}

int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
  int L1=105;
	if (shifouyouliangtiaoxian(a) == 1)//检测到两条直线
	{
		if (panduanshifouyouwandao(a, L1) == 0)//检测到弯道
		{
			//判断直线条数
			//计算CCD到中点距离
			if (shifouyouliangtiaoxian(a) == 1)
			{
				return zhidaowengding(a);
			}
			return zhuanwan(a);
		}
		//直道保持
		return zhidaowengding(a);
	}
	return zhidaowengding(a);
}


void IRQ_CCD_cal()
{
  PIT_Flag_Clear(PIT1);
  
  tsl1401_get_img();         //采集 线性CCD 图像


  //限制最大值
   maxvar((uint8 *)&CCD_BUFF[0],TSL1401_SIZE,BIN_MAX);
   maxvar((uint8 *)&CCD_BUFF[1],TSL1401_SIZE,BIN_MAX);
   maxvar((uint8 *)&CCD_BUFF[2],TSL1401_SIZE,BIN_MAX);
        
  //二值化处理
   bin_xk((uint8 *)&CCD_BUFF[0],TSL1401_SIZE);
   bin_xk((uint8 *)&CCD_BUFF[1],TSL1401_SIZE);
   bin_xk((uint8 *)&CCD_BUFF[2],TSL1401_SIZE);

   direction = tiaojie(CCD_BUFF);
   //printf("%d\n",direction);
  
}