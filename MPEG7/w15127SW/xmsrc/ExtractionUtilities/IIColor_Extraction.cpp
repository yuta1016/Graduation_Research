//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim
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
// Applicable File Name:  IIColor_Extraction.cpp
//

//#include "stdafx.h"
#include "IIColor_Extraction.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

// function prototype 

//----------------------------------------------------------------------------
void IIC_inversegamma(int r, int g, int b, double* r_srgb, double* g_srgb, double* b_srgb);
void gamma_init();
void getcoefficient_for_temperatureconversion(double px, double py, double Tt, double *matrix);
void CalculateTransformMatrixWithBradford(double *XYZi, double *XYZt, double *matrix);
void GetConeResponse(double *XYZ, double *RGB);
void getxyz_for_temperature(double Tp, double *xyz);
void getxy_for_xyz(double x, double y, double *xyz);

//----------------------------------------------------------------------------
static unsigned char inv_sRGB_lookup[256];

void gamma_init()
{
	double sr, ss;

	for(int i=0; i<256; i++)
	{
		sr = i / 255.0;

		if(sr <= 0.00304)
			 ss = 255.0*(12.92*sr);
		else
			ss = 255.0*(1.055*pow(sr,1.0/2.4)-0.055);
	
		if(ss > 255) inv_sRGB_lookup[i] = (unsigned char) 255;
		else if(ss < 0) inv_sRGB_lookup[i] = (unsigned char) 0;
		else inv_sRGB_lookup[i] = (unsigned char) ss;
	}
}

//----------------------------------------------------------------------------
void IIC_rgb2xyz_marklabel(unsigned char* imagebuff, double** xyzbuff, unsigned char** nlabel, int nW, int nH)
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
			IIC_inversegamma((int)imagebuff[(i*(nW*3))+j*3+0], (int)imagebuff[(i*(nW*3))+j*3+1], (int)imagebuff[(i*(nW*3))+j*3+2], &r_srgb, &g_srgb, &b_srgb);

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
void IIC_inversegamma(int r, int g, int b, double* r_srgb, double* g_srgb, double* b_srgb)
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

double **IIC_dBuffAlloc2(int height, int width)
{
	int	i;
	double	**mem;

	mem = (double **)malloc(height * sizeof(double *));
	if(!mem) {
		printf("Error : Cannot allocate memory!!\n");
		exit(-1);
	}

	for(i = 0; i < height; i++) {
		mem[i] = (double *)malloc(width * sizeof(double));
		if( !mem[i] ) {
			printf("Error : Cannot allocate memory!!\n");
			exit( -1 );
		}
	} 

	return	mem;
}

unsigned char **IIC_cBuffAlloc2(int height, int width)
{
	int	i;
	unsigned char	**mem;

	mem = (unsigned char **)malloc(height * sizeof(unsigned char *));
	if(!mem) {
		printf("Error : Cannot allocate memory!!\n");
		exit(-1);
	}

	for(i = 0; i < height; i++) {
		mem[i] = (unsigned char *)malloc(width * sizeof(unsigned char));
		if( !mem[i] ) {
			printf("Error : Cannot allocate memory!!\n");
			exit( -1 );
		}
	} 

	return	mem;
}

void calc_xy(double** xyzbuff, double** xybuff, unsigned char** nlabel, int nW, int nH)
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
			for(k=0; k<3; k++) 
				txyz[k] = xyzbuff[i][j*3+k];
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
    
	for(i=0; i<nH; i++){
		for(j=0; j<nW; j++){
			if(nlabel[i][j] != 0){
				xybuff[i][j*2+0] = xyzbuff[i][j*3+0]/(xyzbuff[i][j*3+0]+
				xyzbuff[i][j*3+1]+xyzbuff[i][j*3+2]);
				xybuff[i][j*2+1] = xyzbuff[i][j*3+1]/(xyzbuff[i][j*3+0]+
				xyzbuff[i][j*3+1]+xyzbuff[i][j*3+2]);
			}
			else {
				xybuff[i][j*2+0] = 0;
				xybuff[i][j*2+1] = 0;
			}
		}
	}

}

void removenoise_xy(double** xybuff, unsigned char** nlabel, int** xybincnt, int nW, int nH, double* pix, 
					double* piy)
{
	double tmp_x, tmp_y;
	int index_x, index_y, i, j, pix_cnt=0;
	int noise_thr = 30;
	int bin_size = 60;
	double avg_xy[2];

	//-- initialize xy bin count
	for(i=0; i<bin_size; i++){
		for(j=0; j<bin_size; j++){
			xybincnt[i][j] = 0;
		}
	}

	for(i=0; i<nH; i++){
		for(j=0; j<nW; j++){
			if(nlabel[i][j]!=0){
				tmp_x=xybuff[i][j*2+0];
				tmp_y=xybuff[i][j*2+1];
				index_x=int(tmp_x*120/2.0);
				index_y=int(tmp_y*120/2.0);
				if((index_x!=bin_size)&&(index_y!=bin_size)){
					xybincnt[index_x][index_y]++;  //wrong!!!!!!
				}
				else if(index_x==bin_size){
					xybincnt[index_x-1][index_y]++;
				}
				else xybincnt[index_x][index_y-1]++;
			}
		}
	}	

	//-- remove bin counts that are considered as noise.
	for(i=0; i<bin_size; i++){
		for(j=0; j<bin_size; j++){
			if((xybincnt[i][j]!=0) && (xybincnt[i][j]<noise_thr)){	
				xybincnt[i][j]=0;
			}
			pix_cnt+=xybincnt[i][j];
		}
	}

  avg_xy[0]=0;
  avg_xy[1]=0;
  int count=0;

  for(i=0; i<bin_size; i++){
		for(j=0; j<bin_size; j++){
			if (xybincnt[i][j]!=0){
				tmp_x=double(i+i+1)/120.0;
//                  tmp_x1=double(i+1)*2/100.0;
				tmp_y=double(j+j+1)/120.0;
//  				tmp_y1=double(j+1)*2/100.0;

				avg_xy[0]+=tmp_x;
				avg_xy[1]+=tmp_y;
				count++;
			}
		}
	}
	avg_xy[0] /= (double)(count);
	avg_xy[1] /= (double)(count);

	*pix = avg_xy[0];
	*piy = avg_xy[1];
}

void interleave_xyzbuff(double *xyzbuff1, double **xyzbuff, int nW, int nH)
{
	for(int i=0; i<nH; i++){
		for(int j=0; j<nW*3; j+=3){
			xyzbuff1[i*(nW*3)+(j+0)]=xyzbuff[i][j+0];
			xyzbuff1[i*(nW*3)+(j+1)]=xyzbuff[i][j+1];
			xyzbuff1[i*(nW*3)+(j+2)]=xyzbuff[i][j+2];
		}
	}
}

void temperature_conversion(double *xyzbuff1, double *xyzbuff2, int nSize, int nW, int nH, 
							double px, double py, double Tt)
{
	double x,y,z;
	double matrix[9];

	getcoefficient_for_temperatureconversion(px,py,Tt,matrix);

	for(int i=0; i<nSize; i+=3)
	{
		x = xyzbuff1[i];
		y = xyzbuff1[i+1];
		z = xyzbuff1[i+2];

		xyzbuff2[i]   = matrix[0]*x + matrix[1]*y + matrix[2]*z;
		xyzbuff2[i+1] = matrix[3]*x + matrix[4]*y + matrix[5]*z;
		xyzbuff2[i+2] = matrix[6]*x + matrix[7]*y + matrix[8]*z;
	}
}

void getcoefficient_for_temperatureconversion(double px, double py, double Tt, double *matrix)
{
	double xyzi[3];
	double xyzt[3];

	getxy_for_xyz(px, py, xyzi);
	getxyz_for_temperature(Tt, xyzt);

	CalculateTransformMatrixWithBradford(xyzi,xyzt,matrix);
}

void CalculateTransformMatrixWithBradford(double *XYZi, double *XYZt, double *matrix)
{
	double RGBi[3];
	double RGBt[3];
	double mtrxD[9];
	double mtrxTemp[9];

	GetConeResponse(XYZi, RGBi);
	GetConeResponse(XYZt, RGBt);

	//-- matrix D:
	mtrxD[0] = RGBt[0] / RGBi[0];
	mtrxD[1] = 0.0;
	mtrxD[2] = 0.0;

	mtrxD[3] = 0.0;
	mtrxD[4] = RGBt[1] / RGBi[1];
	mtrxD[5] = 0.0;
	
	mtrxD[6] = 0.0;
	mtrxD[7] = 0.0;
	mtrxD[8] = RGBt[2] / RGBi[2];
	//--

	int i, j;
	//-- matrix Temp = D*MBFD
	for(i=0; i<9; i+=3)
	{
		mtrxTemp[i]	  = mtrxD[i] * MBFD[0][0] + mtrxD[i+1] * MBFD[1][0] + mtrxD[i+2] * MBFD[2][0];
		mtrxTemp[i+1] = mtrxD[i] * MBFD[0][1] + mtrxD[i+1] * MBFD[1][1] + mtrxD[i+2] * MBFD[2][1];
		mtrxTemp[i+2] = mtrxD[i] * MBFD[0][2] + mtrxD[i+1] * MBFD[1][2] + mtrxD[i+2] * MBFD[2][2];
	}

	// matrix = iMBFD * D * MBFD = iMBFD * mtrxTemp
	for(i=0, j=0; i<9; i+=3, j++)
	{
		matrix[i]   = IMBFD[j][0] * mtrxTemp[0] + IMBFD[j][1]*mtrxTemp[3] + IMBFD[j][2] * mtrxTemp[6];
		matrix[i+1] = IMBFD[j][0] * mtrxTemp[1] + IMBFD[j][1]*mtrxTemp[4] + IMBFD[j][2] * mtrxTemp[7];
		matrix[i+2] = IMBFD[j][0] * mtrxTemp[2] + IMBFD[j][1]*mtrxTemp[5] + IMBFD[j][2] * mtrxTemp[8];
	}

}

void GetConeResponse(double *XYZ, double *RGB)
{
	for(int i=0; i<3; i++)
	{
		RGB[i] = MBFD[i][0] * XYZ[0] + MBFD[i][1] * XYZ[1] + MBFD[i][2] * XYZ[2];
	}
}

void getxyz_for_temperature(double Tp, double *xyz)
{
	double xd, yd;

	if(Tp < 4000){
		xd = -0.2661239*(1000000000.0/(Tp*Tp*Tp))-0.2343580*(1000000.0/(Tp*Tp))+0.8776956*(1000.0/Tp)+0.179910;
	}
	else if(Tp >= 4000 || Tp <= 25000){
		xd = -3.0258469*(1000000000.0/(Tp*Tp*Tp))+2.1070379*(1000000.0/(Tp*Tp))+0.2226347*(1000.0/Tp)+0.24039;
	}

	if(xd <= 0.38405 ) {
		yd = 3.0817580*xd*xd*xd-5.8733867*xd*xd+3.75112997*xd-0.37001483;
	}
	else if(xd <= 0.50338) {
		yd = -0.9549476*xd*xd*xd-1.37418593*xd*xd+2.09137015*xd-0.16748867;
	}
	else {
		yd = -1.1063814*xd*xd*xd-1.34811020*xd*xd+2.18555832*xd-0.20219683;
	}

	getxy_for_xyz(xd,yd,xyz);
}

void getxy_for_xyz(double x, double y, double *xyz)
{
	xyz[0] = x/y;
	xyz[1] = y/y;
	xyz[2] = (1.0-x-y)/y;
}

void xyz2rgb(unsigned char *imagebuff, double *xyzbuff2, int nW, int nH)
{
	int	   rr, gg, bb;
	double x,y,z;
	double r,g,b;

	for(int i=0; i<(nW*nH*3); i+=3)
	{
		x = xyzbuff2[i];
		y = xyzbuff2[i+1];
		z = xyzbuff2[i+2];

		r = XYZ2SRGB[0]*x+XYZ2SRGB[1]*y+XYZ2SRGB[2]*z;
		g = XYZ2SRGB[3]*x+XYZ2SRGB[4]*y+XYZ2SRGB[5]*z;
		b = XYZ2SRGB[6]*x+XYZ2SRGB[7]*y+XYZ2SRGB[8]*z;

		if(r<0) r=0;
		else if(r>1) r=1;
		if(g<0) g=0;
		else if(g>1) g=1;
		if(b<0) b=0;
		else if(b>1) b=1;

		rr = (int) (r*255);
		gg = (int) (g*255);
		bb = (int) (b*255);

		imagebuff[i+2] = inv_sRGB_lookup[rr];
		imagebuff[i+1] = inv_sRGB_lookup[gg];
		imagebuff[i]   = inv_sRGB_lookup[bb];
	}
}

int **nBuffAlloc2(int height, int width)
{
	int	i;
	int	**mem;

	mem = (int **)malloc(height * sizeof(int *));
	if(!mem) {
		printf("Error : Cannot allocate memory!!\n");
		exit(-1);
	}

	for(i = 0; i < height; i++) {
		mem[i] = (int *)malloc(width * sizeof(int));
		if( !mem[i] ) {
			printf("Error : Cannot allocate memory!!\n");
			exit( -1 );
		}
	} 

	return	mem;
}

