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
 * @data-revise 2017-7-8
 */

#include "common.h"
#include "include.h"

//#define BIN_MAX 0xC8   //ccd使用
//#define CCD_EXPOSURE 50  //ccd曝光时间
#define CAMERA_SIZE IMG_ROW_ALL*IMG_COL
#define MAX_VEL 30  //最高限制车速
#define MID_DIR 5.23  //FUTABA S3010


//uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //定义ccd采集数据的数组
float dir = MID_DIR;   //小车方向控制量  初始量定义5归中
int val=0;     //定义编码器采集数据

  float temp_dir=0;   //计算所得方向 
  float last_temp_dir=0;
  float last_dir=5.23;
  float delta_dir=0;
  float delta_temp_dir=0;
  float kp=1;
  float kd=0;
  float kv=40;
  float k=0;
  float k2 = 0;
  int Last_value1=0;
  int stop[2] = { 0,2 };    //当stop[1]为0的时候小车停下来，对应于从斑马线后面发车的情况
  int wait = 100;
  extern int T2=0;
void PIT_VAL();
void S3010_IRQHandler();
void isr_priority_init();

void main(void)
{

  // 下部分是对于所有需要处理的数值的定义及初始化
  
  float vel = 0;   //小车速度控制 


  float kp_encoder=1;
  float ki_encoder=0.01;
  float encoder_least=0;


/*******/
  float encoder_target=200;



  float encoder=0;
  float encoder_integral=0;

 
  uint32 time;
 // 以下是对所有接口以及外设的初始化
    
  led_init(LED0);      //PTD15
  led_init(LED1);      //PTA17
  led_init(LED2);      //PTC0
  led_init(LED3);      //PTE26
  key_init(KEY_A);     //PTB20
  key_init(KEY_B);     //PTB21
  key_init(KEY_C);     //PTB22
  OV7620_Init();       //摄像头初始化
  ftm_quad_init(FTM1); //512路编码器初始化
  motion(vel,0);
  control(dir);
  //uart_init(UART4, 115200);	//已设置debug模式自动开启UART4作调试输出用
  isr_priority_init();  

// 以下是母板上电后的指示灯

    led(LED0,LED_ON);     DELAY_MS(100);
    led(LED1,LED_ON);     DELAY_MS(100);
    led(LED2,LED_ON);     DELAY_MS(100);
    led(LED3,LED_ON);     DELAY_MS(100);
    led(LED0,LED_OFF);  led(LED1,LED_OFF);  led(LED2,LED_OFF);  led(LED3,LED_OFF);
    
    //设定编码器每隔50ms读取一次数据
    pit_init_ms(PIT1,50);
    set_vector_handler(PIT1_VECTORn,PIT_VAL);
    enable_irq(PIT1_IRQn);
    
    //设定舵机输出ms控制一次
    pit_init_ms(PIT2,50);
    set_vector_handler(PIT2_VECTORn,S3010_IRQHandler);
    enable_irq(PIT2_IRQn);
    
    

//  以下是主要程序循环部分
   
  
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
   
    
int max=0;
int min=255;
int M1=0;
int M2=0;
int m1=0;
int m2=0;
int last_T=0;
int delta_T=0;

int judge=50;
int gm1;
int gm2;
int p=0;
for(int i = 80; i < 160; i=i+2)
    {
      for(int j = 0; j < 320; j=j+2)
        {
          if(Image_Data[i][j]>max)
            max=Image_Data[i][j];
          else
            max=max;
          
          if(Image_Data[i][j]<min)
            min = Image_Data[i][j];
          else
            min=min;
        }
     }

T2=(max+min)<<1;
last_T=T2;
  while((judge>5)||(p>5))
  {
    for(int i = 80; i < 160; i=i+2)
    {
      for(int j = 0; j < 320; j=j+2)
        {
        if(Image_Data[i][j]<T2)
          { 
            m1=m1+Image_Data[i][j];
            M1=M1+1;
          }
        else   
          {
          m2=m2+Image_Data[i][j];
            M2=M2+1;
          }
        }
     }
  gm1=m1/M1;
  gm2=m2/M2;
  T2=(gm1+gm2)<<1;
  delta_T=T2-last_T;
  last_T=T2;
  judge=(abs(delta_T));
  p=p+1;
}


  while(1)
  { 
    //printf("delta  %d     ",delta_T);
    //printf("T %d\n",T);
    //lptmr_time_start_ms();
    if (key_check(KEY_B) == KEY_DOWN)
      k++;
    else if (key_check(KEY_C) == KEY_DOWN) 
      k--;
    else
      k=k;
    
    if (key_check(KEY_A) == KEY_DOWN)
      k2++;
    if (k2>=100)
      k2 = 0;
    
    //printf("k:   %f    ",k);
    //printf("k2:  %f    ",k2);

      

  /*obtain_bianma=val;
  delta_bianma=target_bianma-obtain_bianma;
  ki_delta_bianma=ki_delta_bianma+(1+0*0.1)*delta_bianma;

 vel=(target_bianma+ki_delta_bianma)/256;*/

  
encoder_least=val-0;
encoder *=0.2;
encoder += encoder_least*0.8;
encoder_integral +=encoder;
encoder_integral=encoder_integral-encoder_target;
//if(encoder_integral>2100000000)
//encoder_integral=2100000000;
//if(encoder_integral<-2100000000)
//encoder_integral=-2100000000;
vel=-(encoder*kp_encoder+encoder_integral*ki_encoder)/7.68;

//printf("encoder_integral %f \t",encoder_integral); 

  
  
  //vel = kv* (0.1 + sqrt(abs(temp_dir)));
  if(vel > MAX_VEL)
    vel = MAX_VEL;
  else if (vel < 0)
    vel = 0;
  printf("vel %f \t",vel); 
  printf("val %d \n",val); 
  
  
  motion(vel,0);    //40 2160  30  1580
 //control(dir);    //舵机的控制范围0-5-10 对应 右-中-左  
 
  //vcan_sendimg(Image_Data,CAMERA_SIZE);   //发送图像数据 
  sendfinished = 1;                         //图像发送完成，不可以注释
    
 //DELAY_MS(20);
 
  led_turn(LED0); //运行速率灯  
 // time = lptmr_time_get_ms();
  //lptmr_time_close();
  //printf("time : %d\n",time);
//  printf("dir %f\n",dir);
  }
   
  
  
    /*// ttl串口调试备用
  uart_putchar(UART4, 0xff);
  for (int i = 0; i < IMG_ROW_ALL; ++i)
    for (int j = 0; j < IMG_COL; ++j)
    {
      uint8 data = Image_Data[i][j] == 255 ? 254 : Image_Data[i][j];
      uart_putchar(UART4, data);
    }*/
}

void PIT_VAL()
{
  
   
  
   val = ftm_quad_get(FTM1);          //获取FTM 正交解码 的脉冲数(负数表示反方向)
   ftm_quad_clean(FTM1);              //清零
   PIT_Flag_Clear(PIT1);
   
}

void S3010_IRQHandler()
{
  
 temp_dir = Think1(Image_Data, Last_value1,stop,&wait,T2); 
  //printf("temp_dir %f\t",temp_dir);
  
  
  if((temp_dir < -180) || (temp_dir > 180))
    temp_dir = Last_value1;
  if(abs(Last_value1 - temp_dir)>40)
    temp_dir = ((Last_value1/3*2) + (temp_dir/3));
  Last_value1 = temp_dir;
  temp_dir = Amplify(temp_dir);             //放置放大函数
  temp_dir = -(temp_dir-6)*(1+50*0.01) / 50.0;
  
  //temp_dir = -temp_dir / 50.0;


 /*
  if(stop[1]==0)
  {
    motion(0,0);
    break;
  }
 */
  //dir = 5+0.01*k;
  
  
  //******/
  delta_temp_dir =temp_dir - last_temp_dir;
  temp_dir=temp_dir*(1-0.005*5)-0.01*5*delta_temp_dir+0.05*0.04*abs(temp_dir)*temp_dir;
  dir = MID_DIR+temp_dir;//+delta_dir*abs(delta_dir);
  //*********/
  
  if(dir >=6.5)//6.4+9
    dir = 6.5;
  if(dir <= 4.2)//3.5+9
    dir = 4.2;
  
  
  last_temp_dir = temp_dir;
  control(dir);
  PIT_Flag_Clear(PIT2);
  
}

void clear_port_isr_flags()
{
	PORTA_ISFR = 0xFFFFFFFF;
	PORTB_ISFR = 0xFFFFFFFF;
	PORTC_ISFR = 0xFFFFFFFF;
	PORTD_ISFR = 0xFFFFFFFF;
	PORTE_ISFR = 0xFFFFFFFF;
}

void isr_priority_init()
{
	NVIC_SetPriorityGrouping(0);	//用7位表示抢占优先级，1位表示子优先级。即有128个抢占优先级组，每一个组2个中断
	NVIC_SetPriority(PT_CAMERA_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));	//摄像头中断取组优先级0
	NVIC_SetPriority(PIT2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));		//PIT中断取组优先级2
	NVIC_SetPriority(PIT1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
}
