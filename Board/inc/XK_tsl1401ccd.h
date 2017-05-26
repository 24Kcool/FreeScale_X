/*!
 *
 * @file       XK_ls1401ccd.h
 * @brief      小车控制 线性CCD模块的声明与宏定义
 * @author     许珂
 * @version    v8.11
 * @date       2017-05-21
 *
 */

#ifndef _XK_LS1401CCD_H_
#define _XK_LS1401CCD_H_

void PIT0_IRQHandler();
void bin(uint8 *bin,uint8 * img,uint8 * difimg,uint16 len,uint8 maxdif);

void abs_diff(uint8 *dst,uint8 *src,uint16 len,uint8 * maxval,uint8 * avgval);
void maxvar(uint8 *buf,uint16 len,uint8  maxval);

void tsl1401ccd_init();

#endif  //__XK_BIANMA_H__