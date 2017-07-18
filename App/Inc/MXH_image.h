




#ifndef __MXH_IMAGE_H__
#define __MXH_IMAGE_H__

#define MARGIN_1 2
#define MARGIN 5
#define THRESHOLD 58
#define INDEX1 1
#define INDEX2 1
#define ROWM 240
#define COLM 320
#define RATIO_TURN_L 1
#define RATIO_TURN_R 1.1
#define RATIO_BIAS 0

#define MAXO 250 
#define MINO 20

extern int Stop(int *s ,unsigned char a[ROWM][COLM],int* wait);
extern int Think1(unsigned char a[ROWM][COLM], int Last_value,int* s,int* wait,int T);
extern void Filter2(unsigned char a[ROWM][COLM]);
extern void Threshold(unsigned char a[ROWM][COLM],int T);
extern int Amplify(int c);
extern int Bias(int b,int Last_value);

#endif