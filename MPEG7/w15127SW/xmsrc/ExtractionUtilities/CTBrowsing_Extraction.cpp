//////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  CTBrowsing_Extraction.cpp
//

//#include "stdafx.h"
#include "CTBrowsing_Extraction.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#define ABSVALUE(a)	((a<0) ? (a*-1) : a)

//----------------------------------------------------------------------------
//-- sRGB -> XYZ conversion matrix
double	SRGB2XYZ[3][3] = 
	{	0.4124,0.3576,0.1805,
		0.2126,0.7152,0.0722,
		0.0193,0.1192,0.9505};


//-- temperature related to isotemperature lines

double isoTemp[27]={25000, 20000,
			16667, 14286, 12500,
			11111, 10000, 8000,
			6667, 5714, 5000,
			4444, 4000, 3636,
			3333, 3077, 2857,
			2677, 2500, 2353,
			2222, 2105, 2000,
			1905, 1818, 1739,
			1667};


//-- isotemperature lines: u, v, slope for 31 color temperature ranged 1667K ~ infinity.
double isouv[27][3] = {
			0.18293,	0.27407,	-0.30470,
			0.18388,	0.27709,	-0.32675,
			0.18494,	0.28021,	-0.35156,
			0.18611,	0.28342,	-0.37915,
			0.18740,	0.28668,	-0.40955,
			0.18880,	0.28997,	-0.44278,
			0.19032,	0.29326,	-0.47888,
			0.19462,	0.30141,	-0.58204,
			0.19962,	0.30921,	-0.70471,
			0.20525,	0.31647,	-0.84901,
			0.21142,	0.32312,	-1.0182,
			0.21807,	0.32909,	-1.2168,
			0.22511,	0.33439,	-1.4512,
			0.23247,	0.33904,	-1.7298,
			0.24010,	0.34308,	-2.0637,
			0.24702,	0.34655,	-2.4681,
			0.25591,	0.34951,	-2.9641,
			0.26400,	0.35200,	-3.5841,
			0.27218,	0.35407,	-4.3633,
			0.28039,	0.35577,	-5.3762,
			0.28863,	0.35714,	-6.7262,
			0.29685,	0.35823,	-8.5955,
			0.30505,	0.35907,	-11.324,
			0.31320,	0.35968,	-15.628,
			0.32129,	0.36011,	-23.325,
			0.32931,	0.36038,	-40.770,
			0.33724,	0.36051,	-116.45};

//----------------------------------------------------------------------------
void rgb2xyz_marklabel(unsigned char* imagebuff, double** xyzbuff, unsigned char** nlabel, int nW, int nH)
{
	int i, j;
	double r_srgb, g_srgb, b_srgb;
/*
	for(i = 0; i<nH; i++)
		for (j=0; j<nW; j++)
			nlabel[i][j]=0;
*/
	for(i = 0; i < nH; i++)
	{
		for(j = 0; j < nW; j++)
		{
			inversegamma((int)imagebuff[(i*(nW*3))+j*3+0], (int)imagebuff[(i*(nW*3))+j*3+1], (int)imagebuff[(i*(nW*3))+j*3+2], &r_srgb, &g_srgb, &b_srgb);

			xyzbuff[i][j*3+0] = SRGB2XYZ[0][0]*r_srgb+SRGB2XYZ[0][1]*g_srgb+SRGB2XYZ[0][2]*b_srgb;
			xyzbuff[i][j*3+1] = SRGB2XYZ[1][0]*r_srgb+SRGB2XYZ[1][1]*g_srgb+SRGB2XYZ[1][2]*b_srgb;
			xyzbuff[i][j*3+2] = SRGB2XYZ[2][0]*r_srgb+SRGB2XYZ[2][1]*g_srgb+SRGB2XYZ[2][2]*b_srgb;

			if((xyzbuff[i][j*3+0]<0)||(xyzbuff[i][j*3+1]<0)||(xyzbuff[i][j*3+2]<0))
			{
				xyzbuff[i][j*3+0] = 0.0;
				xyzbuff[i][j*3+1] = 0.0;
				xyzbuff[i][j*3+2] = 0.0;
			}
			if((xyzbuff[i][j*3+0]>1.0)||(xyzbuff[i][j*3+1]>1.0)||(xyzbuff[i][j*3+2]>1.0))
			{
				nlabel[i][j] = 255;
			}
		}
	}
}

//--  inverse gamma correction: nonlinear digital input RGB -> sRGB
void inversegamma(int r, int g, int b, double* r_srgb, double* g_srgb, double* b_srgb)
{
	double r_srgbprm, g_srgbprm, b_srgbprm;

	r_srgbprm = r / 255.0;
	g_srgbprm = g / 255.0;
	b_srgbprm = b / 255.0;

	//-- red(R)
	if(r_srgbprm <= 0.03928)
		*r_srgb = r_srgbprm / 12.92;
	else
		*r_srgb = pow(((r_srgbprm + 0.055)/1.055), 2.4);

	//-- green(G)
	if(g_srgbprm <= 0.03928)
		*g_srgb = g_srgbprm / 12.92;
	else
		*g_srgb = pow(((g_srgbprm + 0.055)/1.055), 2.4);
	
	//-- blue(B)
	if(b_srgbprm <= 0.03928)
		*b_srgb = b_srgbprm / 12.92;
	else
		*b_srgb = pow(((b_srgbprm + 0.055)/1.055), 2.4);
}

void perc_ill(double** xyzbuff, unsigned char** nlabel, int nW, int nH, double* pix, double* piy)
{
	int		i,j,k,flag,loop_cnt;
	double	p_th[3];
	double	sxyz[3],ks,txyz[3];
	long	pix_cnt;
	long	lowlevel_pix_cnt;
	double	argb[3];
	double	lowlevelpercent;

	lowlevelpercent = 0.05;

	flag = 1;
	p_th[0] = 0.0;	p_th[1] = 0.0;	p_th[2] = 0.0;
	loop_cnt = 0;
	ks = 3.0;
	lowlevel_pix_cnt = 0;


	//-- remove low luminace pixels
	for(i=0; i<nH; i++){
		for(j=0; j<nW; j++){
			//-- initialize the label data
			nlabel[i][j] = 255;
			for(k=0; k<3; k++) txyz[k] = xyzbuff[i][j*3+k];
			if(txyz[1]<lowlevelpercent){
				xyzbuff[i][j*3+0] = 0;
				xyzbuff[i][j*3+1] = 0;
				xyzbuff[i][j*3+2] = 0;
				lowlevel_pix_cnt++;
				nlabel[i][j] = 0;
			}
		}
	}	

	while((loop_cnt<5)&&(flag==1)){
		argb[0] = 0.0;	argb[1] = 0.0;	argb[2] = 0.0;
		pix_cnt = 0;
		for(i=0; i<nH; i++){
		  for(j=0; j<nW; j++){
			  if(nlabel[i][j] != 0){
				argb[0] += xyzbuff[i][j*3+0];
				argb[1] += xyzbuff[i][j*3+1];
				argb[2] += xyzbuff[i][j*3+2];
				pix_cnt++;
			  }
			}
		  }
		argb[0] /= (double)(pix_cnt);
		argb[1] /= (double)(pix_cnt);
		argb[2] /= (double)(pix_cnt);

		for(i=0; i<3; i++) sxyz[i] = ks*argb[i];

		for(i=0; i<nH; i++)
		{
		  for(j=0; j<nW; j++)
		  {
			for(k=0; k<3; k++) txyz[k] = xyzbuff[i][j*3+k];
			if(txyz[0] > sxyz[0]) nlabel[i][j] = 0;
			if(txyz[1] > sxyz[1]) nlabel[i][j] = 0;
			if(txyz[2] > sxyz[2]) nlabel[i][j] = 0;
		  }
		}
		if((sxyz[0]==p_th[0])&&(sxyz[1]==p_th[1])&&(sxyz[2]==p_th[2])){
			flag = 0;
		}
		for(i=0; i<3; i++){
			p_th[i] = sxyz[i];
		}
		loop_cnt++;
	}
	
	*pix = argb[0]/(argb[0]+argb[1]+argb[2]);
	*piy = argb[1]/(argb[0]+argb[1]+argb[2]);

}

void convert_xy2temp(double pix, double piy, int* ctemperature)
{
	double u, v;

	xy2uv(pix, piy, &u, &v);
	*ctemperature = uv2ColorTemperature(u, v);
}


//-- Reference : Color Science 2nd Edition, pp. 828
void xy2uv(double x, double y, double *u, double *v)
{
	*u = (4 * x) / (-2*x + 12*y+3);
	*v = (6 * y) / (-2*x + 12*y+3);
}

//-- Reference : Color Science 2nd Edition, pp. 227~228
int uv2ColorTemperature(double iu, double iv)
{
	int i, idx1, idx2, nCT;
	double d1, d2, sqt1, sqt2;
	double invT1, invT2, invD;
	double mindist;
	int mindistIdx;

	idx1 = nCT = 0;
	idx2 = 1000;

	mindist = 10000.0;

	//	Bug fix, which is reported in N8023 document. (by Samsung AIT)
	//	For the colors with temperature outside the range 1667K-25000K,
	//	calculate distances to the first and last isotemperature line and choose the closer one.
	if( (iu < isouv[0][0] && iv < isouv[0][1]) || (iu > isouv[26][0] && iv < isouv[26][1]) )
	{
		sqt1 = sqrt(1.0 + isouv[0][2]*isouv[0][2]);
		d1 = ((iv-isouv[0][1]) - isouv[0][2]*(iu-isouv[0][0])) / sqt1;

		sqt2 = sqrt(1.0 + isouv[26][2]*isouv[26][2]);
		d2 = ((iv-isouv[26][1]) - isouv[26][2]*(iu-isouv[26][0])) / sqt2;

		if(ABSVALUE(d1) < ABSVALUE(d2)) mindistIdx = 0;
		else		mindistIdx = 26;
	}
	else 
	{
		//-- for 31 isotemperature lines: notice i, i+1 considered at one step
		//for(i=0; i<30; i++) {
		//-- for 27 isotemperature lines: notice i, i+1 considered at one step
		for(i=0; i<26; i++) {
			sqt1 = sqrt(1.0 + isouv[i][2]*isouv[i][2]);
			d1 = ((iv-isouv[i][1]) - isouv[i][2]*(iu-isouv[i][0])) / sqt1;
			
			sqt2 = sqrt(1.0 + isouv[i+1][2]*isouv[i+1][2]);
			d2 = ((iv-isouv[i+1][1]) - isouv[i+1][2]*(iu-isouv[i+1][0])) / sqt2;
			
			if((d1 * d2) <= 0) {
				idx1 = i;
				idx2 = i+1;
				break;
			}
			
			if(mindist > ABSVALUE(d1)) {
				mindist = ABSVALUE(d1);
				mindistIdx = i;
				if(mindist > ABSVALUE(d2)) {
					mindist = ABSVALUE(d2);
					mindistIdx = i+1;
				}
			}
			else if(mindist > ABSVALUE(d2)) {
				mindist = ABSVALUE(d2);
				mindistIdx = i+1;
			}
		}
	}

	//-- using reciprocal temperature
	if(idx2 == 1000) {
		nCT = (int)isoTemp[mindistIdx];
	}
	else {
		invT1 = 1.0/isoTemp[idx1];
		invT2 = 1.0/isoTemp[idx2];
		invD = d1 / ( d1 - d2);
		nCT = (int)(1.0 / (invT1 + invD * (invT2 - invT1)));
	}

	return nCT;
}

//----------------------------------------------------------------
void PCTBC_Extraction(int* ctemperature,int *pctbc_out)
{
	double firstRCT, secondRCT, RCTemperature;
	int exit=0, index=0;

	RCTemperature = double(1000000)/double(*ctemperature);

	if (*ctemperature < HotMaxTempK) {
		pctbc_out[0] = 0;		// this is color temperature category
		firstRCT = rctHotMin;
		secondRCT = firstRCT - IncrRCTHotDiff;
		while ((secondRCT >= rctHotMax)&&(!exit)){
			if ((RCTemperature <= firstRCT)&&(RCTemperature > secondRCT)){
//				pctbc_out[1] = index;
				exit = 1;
			}
			else {
				firstRCT = secondRCT;
				secondRCT -= IncrRCTHotDiff;
				index++;
			}
		}
		pctbc_out[1] = index;	// this is subrange index
	}
	else if ( (*ctemperature >= WarmMinTempK)&&(*ctemperature < WarmMaxTempK) ) {
		pctbc_out[0] = 1;
		firstRCT = rctWarmMin;
		secondRCT = firstRCT - IncrRCTWarmDiff;
		while ((secondRCT >= rctWarmMax)&&(!exit)){
			if ((RCTemperature <= firstRCT)&&(RCTemperature > secondRCT)){
//				pctbc_out[1] = index;
				exit = 1;
			}
			else {
				firstRCT = secondRCT;
				secondRCT -= IncrRCTWarmDiff;
				index++;
			}
		}
		pctbc_out[1] = index;	// this is subrage index
	}
	else if ( (*ctemperature >= ModerateMinTempK)&&(*ctemperature < ModerateMaxTempK) ) {
		pctbc_out[0] = 2;
		firstRCT = rctModerateMin;
		secondRCT = firstRCT - IncrRCTModerateDiff;
		while ((secondRCT >= rctModerateMax)&&(!exit)){
			if ((RCTemperature <= firstRCT)&&(RCTemperature > secondRCT)){
//				pctbc_out[1] = index;
				exit = 1;
			}
			else {
				firstRCT = secondRCT;
				secondRCT -= IncrRCTModerateDiff;
				index++;
			}
		}		
		pctbc_out[1] = index;	// this is subrage index
	}
	else {
		pctbc_out[0] = 3;
		firstRCT = rctCoolMin;
		secondRCT = firstRCT - IncrRCTCoolDiff;
		while ((secondRCT >= rctCoolMax)&&(!exit)){
			if ((RCTemperature <= firstRCT)&&(RCTemperature > secondRCT)){
//				pctbc_out[1] = index;
				exit = 1;
			}
			else {
				firstRCT = secondRCT;
				secondRCT -= IncrRCTCoolDiff;
				index++;
			}
		}		
		pctbc_out[1] = index;	// this is subrage index
	}
}

//-----------------------------------------------------------------
unsigned char **cBuffAlloc2(int height, int width)
{
	int	i;
	unsigned char	**mem;

	mem = (unsigned char **)malloc(height * sizeof(unsigned char *));
	if(!mem) {
		printf("Error : Cannot allocate memory!!\n");
	    exit(0);
	}

	for(i = 0; i < height; i++) {
		mem[i] = (unsigned char *)malloc(width * sizeof(unsigned char));
		if( !mem[i] ) {
			printf("Error : Cannot allocate memory!!\n");
			exit(0);
		}
	} 

	return	mem;
}

//-----------------------------------------------------------------------
double **dBuffAlloc2(int height, int width)
{
	int	i;
	double	**mem;

	mem = (double **)malloc(height * sizeof(double *));
	if(!mem) {
		printf("Error : Cannot allocate memory!!\n");
	    exit(0);
	}

	for(i = 0; i < height; i++) {
		mem[i] = (double *)malloc(width * sizeof(double));
		if( !mem[i] ) {
			printf("Error : Cannot allocate memory!!\n");
			exit(0);
		}
	} 

	return	mem;
}

//-----------------------------------------------------------------------------
double distance_PCTBC(int *Ref, int* Query)
{
/*	double	buf;
	double	dist;

	buf = Query*pow(2.0,8) + 
*/
	return (abs((Ref[0]*64 + Ref[1]) - (Query[0]*64 + Query[1])));
}
