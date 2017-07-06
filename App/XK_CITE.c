/*!
 *
 * @file       XK_CITE.c
 * @brief      С������ �������빦�ܺ���
 * @author     ����
 * @version    v8.11
 * @date       2017-05-26
 */
 
#include "common.h"
#include "include.h"
#include "XK_cite.h"

#define TSL1401_SIZE 128
#define TSL1401_MAX 1
#define CCD_NEAR 1
#define CCD_FAR 0
#define CURT_TURN 40 //��������ʱ�ļ�ת�ȣ�����ֵ��

/*
��˼���������������Դ���
2017.05.26�ʼ�
���ߣ�������
�������ֳ������ķ���ѭ��
���������ص㣨��ε��������м��ɫ�������Ǻ�ɫ·�غ���ɫ���򣩣�������Ҫ������CCD�ļ�⺯����һ��������ͨ����ֵ��������
�õ���ͼ��Ӧ�����м������ߺڵģ�������Ǽ���·���������ߵ�ʱ��Ӧ���ò�ͬ�ķ�����
����������������أ��ұ������½��أ�ÿһ��ѭ��Ӧ���������ж����̡�
���⣬
Ӧ��ʹ����������CCD����֤С���ڽ��������ܳ�����
���������ǽ������������ж�
��һ���ж��Ƿ����CCD��Lֵ��Rֵ���������ľ��������ص����ظ����������趨����Сֵ
�ڶ����ж��Ǽ���Ƿ���������
�������ж��Ǽ���Ƿ���������������򿴴��룩
���о���ʮ��·����Ҫ����һ���жϣ���û��д��
*/


//�޸Ĺ����ж��صĺ������������ж��������߹涨�ĵ�·
int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])//��⵽ֱ������ȶ�����
{

	//�汾2.0���жϰ�ɫ��·������ȫ�ڵ����
	int i = 2;
	int r = 0;
	int l = 0;
	int out = 0;
	while (i < 123)
	{
		if (a[CCD_FAR][i] < a[CCD_FAR][i + 1])
		{
			l = 63 - i+1;
		}
		if (a[CCD_FAR][i] > a[CCD_FAR][i + 1])
		{
			r = i - 63;
		}
		i = i + 1;
	}
	out = l - r;
	return out;//����ֵ����߿����С��ȥ�ұ߿����С����ֵ����ת����ֵ����ת
}

int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
/*	�汾1.0 
	int i = 0, p = 0;
	int r=0;
	int l=0;
	int out=0;
	int L = 0;
	while (i < 125)
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
	return L;
*/
	//�汾2.0
	int i = 2;
	int r = 0;
	int l = 0;
	int out = 0;
	int L = 0;
	while (i < 123)
	{
		if (a[CCD_FAR][i] < a[CCD_FAR][i + 1])
		{
			l = 63 - i;
		}
		if (a[CCD_FAR][i] > a[CCD_FAR][i + 1])
		{
			r = i - 63;
		}
		i = i + 1;
	}
	L = l + r;
	return L;
}

int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0)
{
/*
	int l;
	l = jisuanLdechangdu(a);
	if (l > (L0 - 5) && l < (L0 + 5))
	{
		return 1;//û�����
	}
	return 0;//�����
*/
	//�汾2.0
	int l = 0;
	l = jisuanLdechangdu(a);
	if (l > (L0 - 5) && l < (L0 + 5))
	{
		return 0;//û�����
	}
	return 1;//�����
}

int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
//�汾2.0
	int num = 0;
	int i = 2;
	int out = 0;
	while (i < 123)
	{
		if (a[CCD_FAR][i] != a[CCD_FAR][i + 1])
		{
			num = num + 1;
		}
		i = i + 1;
	}
	if (num == 2)
	{
		out = 1;
		return out;//��⵽��������·��
	}
	return out;//û��
}


//����ʱ��������֮��������Ҫ����CCD���ľ����ߵľ��룬֮�����һ��

int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{

	//�汾2.0
	int i = 2;
	int position = 0;
	int out = 0;
	while (i < 123)
	{
		if (a[CCD_FAR][i] != a[CCD_FAR][i + 1])
		{
			position = i;
		}
                i++;
	}
	out = 63 - position;//�����ֵ����ƫת�������С��0�������ң�����0��������
	if(out <0)
        {
          out =out -50;
        }
          if(out >0)
          out = out +30;
        return out;
}

int lingjinluyanbaohu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int min)//�ٽ�·�ر��������������ٽ�ֵmin
{
	int i = 15;
	int l = 0;
	int r = 126;
	int out = 0;
	while (i < 111)
	{
		if (a[CCD_NEAR][i] < a[CCD_NEAR][i + 1])
		{
			out = -1;
			return out;
		}
		if (a[CCD_NEAR][i] > a[CCD_NEAR][i + 1])
		{
			out = 1;
			return out;
		}
		i = i + 1;
	}
	return out;//��û��̫����·��
}

int lingjiezhuanwan(int which)//1��-1��
{
	if (which == 1)
	{
		return CURT_TURN;
	}
	if (which == -1)
	{
		return CURT_TURN*(-1);
	}
}

int panduanshizi(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int i = 2;
	int out = 0;
	int p = 0;

	while (i < 123)
	{
		if (a[CCD_FAR][i] != a[CCD_FAR][i + 1])
		{
			p = 1;
		}
                i++;
	}
	if (p == 0)
	{
		out = 1;
		return out;//��⵽ʮ��
	}
	return out;//û��
}
int shizishuchu(void)
{
	return 0;
}

int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L1)
{

	//�汾2.0
/*
	int p = 0;
	if ((p = lingjinluyanbaohu(a,10)) != 0)
	{
		return lingjiezhuanwan(p);
	}
	if (panduanshizi(a))
	{
		return 0;
	}*/
	if (shifouyouliangtiaoxian(a) == 1)//��⵽����ֱ��
	{
		if (panduanshifouyouwandao(a, L1) == 0)//��⵽���
		{
			//�ж�ֱ������
			//����CCD���е����
			if (shifouyouliangtiaoxian(a) == 1)
			{
				return zhidaowengding(a);
			}
			return zhuanwan(a);
		}
		//ֱ������
		return zhidaowengding(a);
	}
	return zhuanwan(a);
}

void filter_xk(uint8 *buf, uint16 len)
{
  int sum=0, avr=0;
  int i=0;
  uint8 matrix[128]={0};
  for(i=2;i<len-2;i++)
  {
    sum = buf[i-2] + buf[i-1] + buf[i] + buf[i+1] + buf[i+2] ;
    /*
    if(len==127)
    {
      sum = buf[len] + buf[len] + buf[len-1]+buf[len-2]+buf[len-3]+buf[len-4]+buf[len-5];
    }
    else if(len == 3)
    {
      sum = buf[len] + buf[len] + buf[len+1]+buf[len+2]+buf[len+3]+buf[len+4]+buf[len+5];
    }
    else
    {
      sum = buf[len]buf[len-2] + buf[len-1] + buf[len] + buf[len+1] + buf[len-2];
    }*/
    avr = sum/5;
    matrix[i] = avr;
  }
  matrix[0] = matrix[1] = matrix[2];
  matrix[127] = matrix[126] = matrix[125];
  for(i=0;i<128;i++)
    buf[i]=matrix[i];
}

