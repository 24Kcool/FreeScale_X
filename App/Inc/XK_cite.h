/*!
 *
 * @file       XK_CITE.h
 * @brief      С������ �������빦�ܺ���
 * @author     ����
 * @version    v8.11
 * @date       2017-05-26
 */

#ifndef _XK_CITE_H_
#define _XK_CITE_H_

int zhidaowengding(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //ֱ�������ȶ�1
int jisuanLdechangdu(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]); //����������ʶ��֮��ľ���
int panduanshifouyouwandao(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE],int L0);  //�ж��Ƿ������
int shifouyouliangtiaoxian(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);   //�Ƿ���������
int zhuanwan(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);     //ת��
extern int tiaojie(uint8 a[TSL1401_MAX * 3][TSL1401_SIZE]);      //���ں���
extern void filter_xk(uint8 *buf, uint16 len);

#endif  //__XK_CITE_H__