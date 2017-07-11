#ifndef _FS_LUT_H_
#define _FS_LUT_H_
#define LUT(arr,ind_i,ind_j)	((arr)[corr_lut[ind_i][ind_j][0]][corr_lut[ind_i][ind_j][1]])
extern const unsigned short corr_lut[120][320][2];
extern const unsigned char thr_lut[120];


#endif