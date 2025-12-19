///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CTBrowsing_Extraction.h
//

//-- temperature range for each classification
#define HotMinTempK	1667
#define HotMaxTempK 2251
#define WarmMinTempK 2251
#define WarmMaxTempK 4171
#define ModerateMinTempK 4171
#define ModerateMaxTempK 8061
#define CoolMinTempK 8061
#define CoolMaxTempK 25001
#define rctHotMin double(1000000)/double(HotMinTempK)
#define rctHotMax double(1000000)/double(HotMaxTempK)
#define rctWarmMin double(1000000)/double(WarmMinTempK)
#define rctWarmMax double(1000000)/double(WarmMaxTempK)
#define rctModerateMin double(1000000)/double(ModerateMinTempK)
#define rctModerateMax double(1000000)/double(ModerateMaxTempK)
#define rctCoolMin double(1000000)/double(CoolMinTempK)
#define rctCoolMax double(1000000)/double(CoolMaxTempK)
#define IncrRCTHotDiff (rctHotMin-rctHotMax)/64.0
#define IncrRCTWarmDiff (rctWarmMin-rctWarmMax)/64.0
#define IncrRCTModerateDiff (rctModerateMin-rctModerateMax)/64.0
#define IncrRCTCoolDiff  (rctCoolMin-rctCoolMax)/64.0

// function prototype 
unsigned char **cBuffAlloc2(int height, int width);
double	**dBuffAlloc2(int height, int width);
void rgb2xyz_marklabel(unsigned char* imagebuff, double** xyzbuff, unsigned char** nlabel, int nW, int nH);
void inversegamma(int r, int g, int b, double* r_srgb, double* g_srgb, double* b_srgb);
void perc_ill(double** xyzbuff, unsigned char** nlabel, int nW, int nH, double* pix, double* piy);
void convert_xy2temp(double pix, double piy, int* ctemperature);
void xy2uv(double x, double y, double *u, double *v);
int uv2ColorTemperature(double iu, double iv);
void PCTBC_Extraction(int* ctemperature,int *pctbc_out);
double distance_PCTBC(int *Ref, int* Query);
