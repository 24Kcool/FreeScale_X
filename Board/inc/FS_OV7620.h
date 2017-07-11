#ifndef _FS_OV7620_H_
#define _FS_OV7620_H_

#define IMG_ROW_ALL		240
#define IMG_ROW			120
#define IMG_COL			320
#define IMG_COL_ALL		320

#define IS_WAIT			(flags & 0x01)
#define SET_WAIT		(flags |= 0x01)
#define CLEAR_WAIT		(flags &= 0xfe)

extern unsigned char ROW_Count;
extern unsigned char Pixel_Data[IMG_ROW_ALL+1][IMG_COL_ALL];
extern unsigned char flags;
extern unsigned char sendfinished;
extern unsigned char(*Image_Data)[IMG_COL_ALL];

extern void OV7620_Init();
extern void Send_Img();


extern void VSYNC_handler();
extern void HRFF_handler();

#endif