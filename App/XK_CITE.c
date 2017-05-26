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
	return out;//����ֵ����߿����С��ȥ�ұ߿����С����ֵ����ת����ֵ����ת
}

int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
	int i = 0, p = 0;
	int r=0;
	int l=0;
	//int out=0;
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
		return 1;//û�����
	}
        
	return 0;//�����
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
		return f;//��������
	}
	f = 0;
	return f;
}


//����ʱ��������֮��������Ҫ����CCD���ľ����ߵľ��룬֮�����һ��

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
	out = 63 - p;//�����ֵ����ƫת�������С��0�������ң�����0��������
//printf("zhuanwan%d     ",out);
	return out;
}

int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
  int L1=105;
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
	return zhidaowengding(a);
}

void filter_xk(uint8 *buf, uint16 len)
{
  int sum=0, avr=0;
  int temp=0,i=0;
  uint8 matrix[127]={0};
  for(i=0;i<len;i++)
  {
    if(len==127)
    {
      sum = buf[len] + buf[len] + buf[len-1];
    }
    else if(len == 0)
    {
      sum = buf[len] + buf[len] + buf[len+1];
    }
    else
    {
      sum = buf[len-1] + buf[len] + buf[len+1];
    }
    avr = sum/3;
    matrix[i] = avr;
  }
  buf = matrix;
}