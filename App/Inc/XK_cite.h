/*!
 *
 * @file       XK_CITE.h
 * @brief      小车控制 其他必须功能函数
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-26
 */

#ifndef _XK_CITE_H_
#define _XK_CITE_H_

int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);//检测到直道后的稳定函数
int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);
int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0);
int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);
int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);
int lingjinluyanbaohu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int min);
int lingjiezhuanwan(int which);//1左-1右
int panduanshizi(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);
int shizishuchu(void);
int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L1);
void filter_xk(uint8 *buf, uint16 len);

#endif  //__XK_CITE_H__