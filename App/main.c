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
 * @data-revise 2017-7-8
 */

#include "common.h"
#include "include.h"

//#define BIN_MAX 0xC8   //ccdʹ��
//#define CCD_EXPOSURE 50  //ccd�ع�ʱ��
#define CAMERA_SIZE IMG_ROW_ALL*IMG_COL
#define MAX_VEL 30  //������Ƴ���
#define MID_DIR 5.23  //FUTABA S3010


//uint8 CCD_BUFF[TSL1401_MAX*3][TSL1401_SIZE];  //����ccd�ɼ����ݵ�����
float dir = MID_DIR;   //С�����������  ��ʼ������5����
int val=0;     //����������ɼ�����

  float temp_dir=0;   //�������÷��� 
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
  int stop[2] = { 0,2 };    //��stop[1]Ϊ0��ʱ��С��ͣ��������Ӧ�ڴӰ����ߺ��淢�������
  int wait = 100;
  extern int T2=0;
void PIT_VAL();
void S3010_IRQHandler();
void isr_priority_init();

void main(void)
{

  // �²����Ƕ���������Ҫ�������ֵ�Ķ��弰��ʼ��
  
  float vel = 0;   //С���ٶȿ��� 


  float kp_encoder=1;
  float ki_encoder=0.01;
  float encoder_least=0;


/*******/
  float encoder_target=200;



  float encoder=0;
  float encoder_integral=0;

 
  uint32 time;
 // �����Ƕ����нӿ��Լ�����ĳ�ʼ��
    
  led_init(LED0);      //PTD15
  led_init(LED1);      //PTA17
  led_init(LED2);      //PTC0
  led_init(LED3);      //PTE26
  key_init(KEY_A);     //PTB20
  key_init(KEY_B);     //PTB21
  key_init(KEY_C);     //PTB22
  OV7620_Init();       //����ͷ��ʼ��
  ftm_quad_init(FTM1); //512·��������ʼ��
  motion(vel,0);
  control(dir);
  //uart_init(UART4, 115200);	//������debugģʽ�Զ�����UART4�����������
  isr_priority_init();  

// ������ĸ���ϵ���ָʾ��

    led(LED0,LED_ON);     DELAY_MS(100);
    led(LED1,LED_ON);     DELAY_MS(100);
    led(LED2,LED_ON);     DELAY_MS(100);
    led(LED3,LED_ON);     DELAY_MS(100);
    led(LED0,LED_OFF);  led(LED1,LED_OFF);  led(LED2,LED_OFF);  led(LED3,LED_OFF);
    
    //�趨������ÿ��50ms��ȡһ������
    pit_init_ms(PIT1,50);
    set_vector_handler(PIT1_VECTORn,PIT_VAL);
    enable_irq(PIT1_IRQn);
    
    //�趨������ms����һ��
    pit_init_ms(PIT2,50);
    set_vector_handler(PIT2_VECTORn,S3010_IRQHandler);
    enable_irq(PIT2_IRQn);
    
    

//  ��������Ҫ����ѭ������
   
  
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
 //control(dir);    //����Ŀ��Ʒ�Χ0-5-10 ��Ӧ ��-��-��  
 
  //vcan_sendimg(Image_Data,CAMERA_SIZE);   //����ͼ������ 
  sendfinished = 1;                         //ͼ������ɣ�������ע��
    
 //DELAY_MS(20);
 
  led_turn(LED0); //�������ʵ�  
 // time = lptmr_time_get_ms();
  //lptmr_time_close();
  //printf("time : %d\n",time);
//  printf("dir %f\n",dir);
  }
   
  
  
    /*// ttl���ڵ��Ա���
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
  
   
  
   val = ftm_quad_get(FTM1);          //��ȡFTM �������� ��������(������ʾ������)
   ftm_quad_clean(FTM1);              //����
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
  temp_dir = Amplify(temp_dir);             //���÷Ŵ���
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
	NVIC_SetPriorityGrouping(0);	//��7λ��ʾ��ռ���ȼ���1λ��ʾ�����ȼ�������128����ռ���ȼ��飬ÿһ����2���ж�
	NVIC_SetPriority(PT_CAMERA_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));	//����ͷ�ж�ȡ�����ȼ�0
	NVIC_SetPriority(PIT2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));		//PIT�ж�ȡ�����ȼ�2
	NVIC_SetPriority(PIT1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
}
