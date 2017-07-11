/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_SCCB.h
 * @brief      OV摄像头配置总线SCCB函数库
 * @author     野火科技
 * @version    v5.0
 * @date       2013-09-01
 */


#ifndef _FIRE_SCCB_H_
#define _FIRE_SCCB_H_



#define SCL_HIGH()	GPIO_PDOR_REG(GPIOX_BASE(SCCB_SCL)) |= (1 << PTn(SCCB_SCL))
#define SCL_LOW()	GPIO_PDOR_REG(GPIOX_BASE(SCCB_SCL)) &= ~(1 << PTn(SCCB_SCL))
#define SCL_OUT()	GPIO_PDDR_REG(GPIOX_BASE(SCCB_SCL)) |= (1 << PTn(SCCB_SCL))
#define SCL_IN()	GPIO_PDDR_REG(GPIOX_BASE(SCCB_SCL)) &= ~(1 << PTn(SCCB_SCL))

#define SDA_HIGH()	GPIO_PDOR_REG(GPIOX_BASE(SCCB_SDA)) |= (1 << PTn(SCCB_SDA))
#define SDA_LOW()	GPIO_PDOR_REG(GPIOX_BASE(SCCB_SDA)) &= ~(1 << PTn(SCCB_SDA))
#define SDA_OUT()	GPIO_PDDR_REG(GPIOX_BASE(SCCB_SDA)) |= (1 << PTn(SCCB_SDA))
#define SDA_IN()	GPIO_PDDR_REG(GPIOX_BASE(SCCB_SDA)) &= ~(1 << PTn(SCCB_SDA))
#define SDA_DATA()	(GPIO_PDIR_REG(GPIOX_BASE(SCCB_SDA)) >> PTn(SCCB_SDA)) & 0x01

#define ADDR_OV7725   0x42
#define ADDR_OV7620   0x42

#define DEV_ADR  ADDR_OV7725             /*设备地址定义*/

#define SCCB_DELAY()    SCCB_delay(750)


extern void SCCB_GPIO_init(void);
//extern void sccb_wait();
extern uint8 sccb_regWrite(uint8 device, uint8 address, uint8 data);

#endif      //_FIRE_SCCB_H_
