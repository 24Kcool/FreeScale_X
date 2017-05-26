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

int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //直到保持稳定1
int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]); //计算两根标识线之间的距离
int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0);  //判断是否又弯道
int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //是否有两条线
int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);     //转弯
extern int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);      //调节函数
extern void filter_xk(uint8 *buf, uint16 len);

#endif  //__XK_CITE_H__