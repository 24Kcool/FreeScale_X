#include  "common.h"
#include  "include.h"
//#include  "MXH_Image.h"

int Last_value = 0;
int Think1(unsigned char a[ROWM][COLM], int Last_value,int*s,int* wait, int T)
{
/*       unsigned char a[ROWM][COLM];
        for (int i = 0; i < ROWM; i++)
	{
		for (int j = 0; j < COLM; j++)
		{
			a[i][j] = y[i][j];
		}
	}
*/
	Threshold(a,T);
       Stop( s  ,a, wait) ;
	int timed = 0;
	int Line = ROWM * 0.46;
	int Middle = COLM / 2;
	int left = Middle, right = Middle;
	if (a[Line][Middle] == 255)
	{
		for (int i = Middle; i > 0 + MARGIN; i--)	//向左扫描
		{
			right = i;
			if ((a[Line][i + 1] == 255) && (a[Line][i - 1] == 0))
				break;
		}

		for (int i1 = Middle; i1 < COLM - MARGIN; i1++)	//向右扫描
		{
			left = i1;
			if ((a[Line][i1 - 1] == 255) && (a[Line][i1 + 1] == 0))
				break;
		}
		int direction = ((right + left) / 2 - Middle);
		return direction;
	}
	else
	{
		int i2 = Line;
		while (a[i2][Middle] == 0)
		{
			i2++;
			timed = i2;
			if (i2 > (ROWM - MARGIN - 2))
				break;
		}
		if (a[timed + MARGIN][Middle] == 255)
		{
			for (int i = Middle; i > 0 + MARGIN; i--)	//向左扫描
			{
				right = i;
				if ((a[timed + MARGIN][i + 1] == 255) && (a[timed + MARGIN][i - 1] == 0))
					break;
			}

			for (int i1 = Middle; i1 < COLM - MARGIN; i1++)	//向右扫描
			{
				left = i1;
				if ((a[timed + MARGIN][i1 - 1] == 255) && (a[timed + MARGIN][i1 + 1] == 0))
					break;
			}
			int direction = ((right + left) / 2 - Middle);
			return direction;
		}
		else
		{
			int open = 0;
			int r, l;
			for (int i3 = 0; i3 < COLM - 2; i3++)
			{
				if ((a[timed][i3] == 255) && (a[timed][i3 + 2] == 255))
				{
					l = i3;
					if (open == 0)
					{
						r = l;
						open = 1;
					}
				}
			}
//			return (l + r) / 2 - Middle;
                        if ((l == 0) && (r == 0))
			{
				return Last_value;
			}
			else
			{
				return (l + r) / 2 - Middle;
			}
		}
	}
}


void Threshold(unsigned char a[ROWM][COLM],int T)
{
	for (size_t i = 0; i < ROWM; i++)
	{
		for (size_t j = 0; j < COLM; j++)
		{
			if (a[i][j] < T)
			{
				a[i][j] = 0;
			}
			else
			{
				a[i][j] = 255;
			}
		}
	}
       /* for (int i = MARGIN_1; i < ROWM - MARGIN_1; i++)
	{
		for (int j = MARGIN_1; j < COLM - MARGIN_1; j++)//对于每个像素进行处理,下面有15*15的最小值滤波处理
		{
			int he = 0;
			for (int n = i - MARGIN_1; n <= i +MARGIN_1; n++)
			{
				for (int b = j - MARGIN_1; b <= j + MARGIN_1; b++)//15*15的最小值滤波
				{
					he = he + a[n][b];
				}
			}

			a[i][j] = he / ((2 * MARGIN_1 + 1)*(2 * MARGIN_1 + 1));
		}
	}*/
}

int Amplify(int c)
{
	int b = c;
	if (b > 0)
	{
		b = b*RATIO_TURN_R;
	}
	else if (b == 0)
	{
		b = b;
	}
	else
	{
		b = b*RATIO_TURN_L;
	}
	return b;
}

int Bias(int b,int Last_value)
{
	int bias = 0;
	if ((b*Last_value) > 0)
	{
		bias = Last_value*RATIO_BIAS;
	}
	return bias;
}


//int Think(char b[ROWM][COLM], int* s, int wait)
int Stop(int *s ,unsigned char a[ROWM][COLM],int* wait)    //需要声明一个全局变量
{
	int index = 0;
	if ((*wait) <= 0)
	{
		for (size_t i6 = 0; i6 < COLM-3; i6++)
		{
			if (a[ROWM / 2][i6] == 0 && a[ROWM / 2][i6 + 3] == 255)
			{
				index++;
			}
		}
		if (index >= 6)
		{
			s[1] = s[1] - 1;
			(*wait) = 300;
		}
	}
	else
	{
		*wait = *wait - 1;
	}
        //printf("wait  %d   ",*wait);
	//printf("s[1]  %d   \n",s[1]);
}
/*
int16 Think(unsigned char a[][COLM])	//	判断函数，返回值标志为：正为右转负为左转
{
  //unsigned char a[ROWM][COLM];
  //a = b;
	/*
        for (int i = 0; i < ROWM; i++)
	{
		for (int j = 0; j < COLM; j++)
		{
			a[i][j] = b[i][j];
		}
	}
        */
/*	Threshold(a);	//二值化函数 
        int16 timed = 0;
	int16 Line = ROWM / 2;
	int16 Middle = COLM / 2;
	int16 left = Middle, right = Middle;
	if (a[Line][Middle] == MAX)
	{
		for (int16 i = Middle; i > 0 + MARGIN; i--)	//向左扫描
		{
			right = i;
			if ((a[Line][i + 1] == MAX) && (a[Line][i - 1] == MIN))
				break;
		}
		for (int16 i1 = Middle; i1 < COLM - MARGIN; i1++)	//向右扫描
		{
			left = i1;
			if ((a[Line][i1 - 1] == MAX) && (a[Line][i1 + 1] == MIN))
				break;
		}
		int16 direction = ((right + left) / 2 - Middle);
		return -direction;
	}
	else
	{
		for (int16 i = Line; (a[i][Middle] == MAX) && (i < (Middle - MARGIN - 1)); i++)
		{
			timed = i;
		}
		if (a[timed + MARGIN][Middle] == MAX)
		{
			for (int16 i = Middle; i > 0 + MARGIN; i--)	//向左扫描
			{
				right = i;
				if ((a[Line][i + 1] == MAX) && (a[Line][i - 1] == MIN))
					break;
			}
			for (int16 i1 = Middle; i1 < COLM - MARGIN; i1++)	//向右扫描
			{
				left = i1;
				if ((a[Line][i1 - 1] == MAX) && (a[Line][i1 + 1] == MIN))
					break;
			}
			int16 direction = ((right + left) / 2 - Middle);
			return -direction;
		}
		else
		{
			for (int16 i = 1; i < (COLM - 1); i++)
			{
				if (a[timed][i - 1] = !a[timed][i + 1]);
				{
					int16 direction = Middle;
					return ( Middle -i);
				}
			}
		}
	}
        
/*
        int left = 0, right = 0;	//赛道的左边界与右边界坐标
	int Line = (ROWM / 3)*2;		//只取中间的一行判断
	int Middle = COLM / 2;		//从中间的一列向两边开始扫描
        int direction = 0;
	for (int i = Middle; i > (0 + MARGIN); i--)	//向左扫描
	{
		if ((a[Line][i + 1] == 255) && (a[Line][i - 1] == 0))
		{
			left = i;
			break;
		}

	}
	for (int i1 = Middle; i1 < (COLM - MARGIN); i1++)	//向右扫描
	{
		if ((a[Line][i1 - 1] == 255) && (a[Line][i1 + 1] == 0))
		{
			right =y 1
                          i1;
			break;
		}
	}
	direction = ((right + left) / 2 - Middle);		//右正左负
        //direction = 0;
	return direction;	//返回值
*/
//}
/*

void Filter2(unsigned char a[ROWM][COLM])
{
	for (size_t i0 = INDEX1; i0 < ROWM - INDEX1; i0++)
	{
		for (size_t j0 = INDEX1; j0 < COLM - INDEX1; j0++)//对每一个像素处理
		{
			if (a[i0][j0] > 0)
			{
				if ((a[i0 - INDEX1][j0 - 1] > 0) || (a[i0 - INDEX1][j0] > 0) || (a[i0 - INDEX1][j0 + 1] > 0))
				{
					if ((a[i0 + INDEX1][j0 - 1] > 0) || (a[i0 + INDEX1][j0] > 0) || (a[i0 + INDEX1][j0 + 1] > 0))
					{
						a[i0][j0] = 255;
					}
					else
					{
						a[i0][j0] = 0;
					}
				}
				else
				{
					a[i0][j0] = 0;
				}
			}

		}
	}

	for (size_t i0 = INDEX1; i0 < ROWM - INDEX2; i0++)
	{
		for (size_t j0 = INDEX1; j0 < COLM - INDEX2; j0++)//对每一个像素处理
		{
			if (a[i0][j0] > 0)
			{
				if ((a[i0 - INDEX2][j0 - 1] > 0) || (a[i0 - INDEX2][j0] > 0) || (a[i0 - INDEX2][j0 + 1] > 0))
				{
					if ((a[i0 + INDEX2][j0 - 1] > 0) || (a[i0 + INDEX2][j0] > 0) || (a[i0 + INDEX2][j0 + 1] > 0))
					{
						a[i0][j0] = 255;
					}
					else
					{
						a[i0][j0] = 0;
					}
				}
				else
				{
					a[i0][j0] = 0;
				}
			}

		}
	}
}
void Threshold(unsigned char a[ROWM][COLM])
{
	for (size_t i = 0; i < ROWM; i++)
	{
		for (size_t j = 0; j < COLM; j++)
		{
			if (a[i][j] < THRESHOLD)
			{
				a[i][j] = MIN;
			}
			else
			{
				a[i][j] = MAX;
			}
		}
	}
        //vcan_sendimg(a,240*320);
}*/