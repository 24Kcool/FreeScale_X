/*!
 *
 * @file       XK_CITE.c
 * @brief      小车控制 其他必须功能函数
 * @author     许珂
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
#define CURT_TURN 40 //靠近边沿时的急转度（绝对值）

/*
飞思卡尔光电四轮组测试代码
2017.05.26笔记
作者：马晓航
将赛道分成条带的方法循迹
根据赛道特点（这次的赛道是中间白色，两边是黑色路沿和蓝色外域），我们需要对线阵CCD的检测函数做一点修正，通过二值化函数，
得到的图像应该是中间亮两边黑的，因此我们检测道路的左右两边的时候应该用不同的方法，
我想左边沿是上升沿，右边沿是下降沿，每一个循环应该是两个判断流程。
另外，
应该使用两个线阵CCD来保证小车在近处不会跑出赛道
初步构想是建立三个条件判断
第一个判断是否近处CCD的L值与R值（线阵中心距离左右沿的像素个数）大于设定的最小值
第二个判断是检测是否有两条沿
第三个判断是检测是否有弯道（具体排序看代码）
还有就是十字路口需要进行一个判断（还没有写）
*/


//修改过的判断沿的函数不能用于判断两条黑线规定的道路
int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])//检测到直道后的稳定函数
{

	//版本2.0，判断白色道路，两边全黑的情况
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
	return out;//返回值，左边空余大小减去右边空余大小，正值往左转，负值往右转
}

int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
/*	版本1.0 
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
	//版本2.0
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
		return 1;//没有弯道
	}
	return 0;//有弯道
*/
	//版本2.0
	int l = 0;
	l = jisuanLdechangdu(a);
	if (l > (L0 - 5) && l < (L0 + 5))
	{
		return 0;//没有弯道
	}
	return 1;//有弯道
}

int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{
//版本2.0
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
		return out;//检测到左右两个路沿
	}
	return out;//没有
}


//当延时函数结束之后，我们需要计算CCD中心距离线的距离，之后跟着一条

int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE])
{

	//版本2.0
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
	out = 63 - position;//由这个值计算偏转量，如果小于0则是向右，大于0则是向左。
	if(out <0)
        {
          out =out -50;
        }
          if(out >0)
          out = out +30;
        return out;
}

int lingjinluyanbaohu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int min)//临近路沿保护函数，大于临界值min
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
	return out;//并没有太靠近路沿
}

int lingjiezhuanwan(int which)//1左-1右
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
		return out;//检测到十字
	}
	return out;//没有
}
int shizishuchu(void)
{
	return 0;
}

int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L1)
{

	//版本2.0
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

