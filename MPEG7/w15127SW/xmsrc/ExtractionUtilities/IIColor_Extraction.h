///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  IIColor_Extraction.h
//


// function prototype 
//--------------------------------------------------------------------------
void gamma_init();
// KW 2014/10: add prefix 'IIC_' to fix conflict with duplicated functions in CTBrowsing_Extraction
void IIC_rgb2xyz_marklabel(unsigned char* imagebuff, double** xyzbuff, unsigned char** nlabel, int nW, int nH);
int **nBuffAlloc2(int height, int width);
unsigned char **IIC_cBuffAlloc2(int height, int width);
double **IIC_dBuffAlloc2(int height, int width);
void calc_xy(double** xyzbuff, double** xybuff, unsigned char** nlabel, int nW, int nH);
void removenoise_xy(double** xybuff, unsigned char** nlabel, int** xybincnt, int nW, int nH, 
						   double* pix, double* piy);
void interleave_xyzbuff(double *xyzbuff1, double **xyzbuff, int nW, int nH);
void temperature_conversion(double *xyzbuff1, double *xyzbuff2, int nSize, int nW, int nH, 
								   double px, double py, double Tt);
void xyz2rgb(unsigned char *imagebuff, double *xyzbuff2, int nW, int nH);
//--------------------------------------------------------------------------
//-- sRGB -> XYZ conversion matrix
static double	SRGB2XYZ[3][3] = 
	{	0.4124,0.3576,0.1805,
		0.2126,0.7152,0.0722,
		0.0193,0.1192,0.9505};

//-- XYZ -> sRGB conversion matrix
static double	XYZ2SRGB[9] = 
	{	3.2406,-1.5372,-0.4986,
		-0.9689,1.8758,0.0415,
		0.0557,-0.2040,1.0570};

//-- Bradford matrix
static double	MBFD[3][3] = 
	{	0.8951, 0.2664,-0.1614,
		-0.7502, 1.7135, 0.0367,
		0.0389,-0.0685, 1.0296};

//-- Inverse Bradford matrix
static double	IMBFD[3][3] = 
	{	0.9870,-0.1471, 0.1600,
		0.4323, 0.5184, 0.0493,
		-0.0085, 0.0400, 0.9685};
