#ifndef _FS_CCD_H_
#define _FS_CCD_H_
#define PIX_START	28       //ÏÔÊ¾·¶Î§
#define PIX_END		100
#define PIX_NUM		(PIX_END-PIX_START)
extern int CCD_AVG_E;
extern unsigned int CCD_MAX, CCD_MIN, CCD_AVG;
extern unsigned char TimerFlag20ms, IntegrationTime;
extern unsigned char ccd_data[128];
extern void read_ccd(void);
extern void ccd_init(void); 
extern void Integration(void);   //ÆØ¹âº¯Êý
extern void ccd_tsl1401(unsigned char *p);
extern void CalculateIntegrationTime(void); 
#endif