//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu,Shawn Newsam, Baris Newsam, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// Electrical and Computer Engineering Deptt., University of California Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TextureBrowsingExtraction.cpp
//
#ifndef TEXTRUE_BROWSING_EXTRACTION_H
#define TEXTRUE_BROWSING_EXTRACTION_H

#include <math.h>

// define parameters used for Gabor decomposition
#define XM_UL			0.05	/* lower frequency */
#define XM_UH			0.4		/* upper frequency */
#define XM_SCALE		4		/* number of scales in generating filters */
#define XM_ORIENTATION	6		/* number of orientations in generating filters */
#define XM_FLAG			1		/* remove the DC */
#define XM_SIDE			40		/* filter mask = 2*side+1 x 2*side+1 */

// define the thresholds for quantizing PBC
#define	BOUNDARY1	5.1
#define	BOUNDARY2	10.1
#define	BOUNDARY3	19.9

// for direction.c
#define SCALE		4
#define ORIENTATION 6

struct Peak {
        int index;
        double value;
		double contrast;
        int flag;
        } ;


// for pbc.c
//#define PROJ_SIZE 160
#ifndef DEBUGLEV
#define DEBUGLEV  0
#endif

struct pbc_struct {
  float structuredness;
  int tstructuredness;
  int element[4];
};

#define sqr(x) ((x)*(x))
#ifndef MAX
  #define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
  #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

// for GaborFeature.c
#define D_DirectionSize 2
#define LOG2 log(2.0)


// matrix.h
typedef struct MatrixStruct 
{
	double **data;
	int height, width;
} Matrix;

typedef struct uc_MatrixStruct 
{
	unsigned char **data;
	int height, width;
} uc_Matrix;

typedef struct i_MatrixStruct 
{
	int **data;
	int height, width;
} i_Matrix;

typedef struct RegionStruct
{
	int id;
	int number;
	int *member_i;
	int *member_j;
} TRegion;

#ifndef M_PI
#define M_PI 3.141592653589793115997963468544185161590576171875
#endif

/* tempr is a double and is defined inside the function four1()*/

void nrerror(char *error_text);


double *dvector(int nl, int nh);

//float *vector(int nl,int nh);
float *my_vector(int nl,int nh);

void free_dvector(double *v, int nl,int nh);

void free_vector(float *v,int  nl,int nh);

float **matrix(int nrl,int nrh,int ncl,int nch);

void free_matrix(float**m,int nrl,int nrh,int ncl,int nch);

double **dmatrix(int nrl,int nrh,int ncl,int nch);

void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch);

double log2( double a);

void sort(double *Y, int *I, double *A, int length);

void minimun(double *Y, int *I, double *A, int length);

void Mat_Abs(Matrix *A);

void Mat_Mean(double *mean,Matrix *A);

void Mat_Vector(Matrix *A, float *a);

void Mat_Shift(Matrix *A, Matrix *B, int side);

void Mat_Zeros(Matrix *A);
void Mat_Zeros_uc(uc_Matrix *A);

void Mat_Zeros_i(i_Matrix *A);
void CreateMatrix(Matrix **M, int hei,int wid);


void FreeMatrix(Matrix *M);

void Create_i_Matrix(i_Matrix **M, int hei, int wid);

void Free_i_Matrix(i_Matrix *M);

void Create_uc_Matrix(uc_Matrix **M, int hei,int wid);

void Free_uc_Matrix(uc_Matrix *M);


void Mat_FFT2(Matrix *Output_real, Matrix *Output_imag, Matrix *Input_real, Matrix *Input_imag);

void Mat_IFFT2(Matrix *Output_real, Matrix *Output_imag, Matrix *Input_real, Matrix *Input_imag);

void four2(double **fftr, double **ffti, double **rdata, double **idata, int rs, int cs, int isign);

void four1(double *data, int nn, int isign);

void Mat_Copy(Matrix *A, Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end);

void Mat_uc_Copy(uc_Matrix *A, uc_Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end);
	
void Mat_i_Copy(i_Matrix *A, i_Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end);
				
void Mat_Product(Matrix *A, Matrix *B, Matrix *C);

void Mat_Sum(Matrix *A, Matrix *B, Matrix *C);

void Mat_Substract(Matrix *A, Matrix *B, Matrix *C);

void Mat_Fliplr(Matrix *A);

void Mat_Flipud(Matrix *A);


void Mat_uc_Fliplr(uc_Matrix *A);

void Mat_uc_Flipud(uc_Matrix *A);


// direction.h
double ComputeContrast(int index,double *histo,int len);

int *DominantDirection(float **histo);

int JudgeContinuity(int index,int order,struct Peak *peak, int size);


// gabor.h
void Gabor();


// imageRotate.h
int billinear(Matrix *img,float a,float b,int ii,int jj);

void ImgRotate(Matrix *inImg,float angle);

// LoadGrayImg.h
void LoadGrayImg();


// pbc.h

void piksrtintS2B(int n, int *num, int *index);
// VS2013-MODS
#if (_MSC_VER < 1800)
int round(float x);
#endif
float tb_distance(float *a,float *b,int dim);
int **imatrix(int nr, int nc);
void free_imatrix(int **m, int nr);
float **tb_fmatrix(int nr, int nc);
void tb_free_fmatrix(float **m, int nr);
void proj_analysis(char *image_name,int proj_type,float *credit,struct pbc_struct *pbc,int img_size);
int auto1_radon(double *x_in,int x_long,double **B);
int peak_proj(double *B,int leng_B,int *PeakI,float *Peak);
float proj_contrast(double *B,int leng_B,int *PeakI,float *Peak,int num_peak);
void peak_summary(float *dis_peak,float *var_dis,int *PeakI,float *Peak,int size_peak);
int cand_cluster(float **candi,int size_in,int ndim,int *refine_cand,int *pick_num);
float var_vectors(float **vector,int nvec,int ndim);
int agglom(float **candi,int *label,int N,float thresh,int *count,int Nlabel,int fN);
void proj_credit_modi(int **candidate,int size_cand,int size_count_candi,int flag,
    float *credit);
int get_num(int **cand_pair,int size_cand,int *num,int len);
void get_map(int **cand_pair,int size_cand,int total,int *num,int type,int **map);
void sing_map(int **inpair,int size_in,int *sing,int type);
void convertcand(int **cand_pair,int **direc_cand,int size_cand);
void pbcmain( struct pbc_struct *pbc ,int size);

//threshold.h
void threshold( struct pbc_struct *pbc );

// GaborFeature.h
void ComputeProjection(Matrix *inputImage,int xsize,int ysize,double angle,int proj_size,double *proj);
int GaborFeature(Matrix *img, int side, double Ul, double Uh, int scale, int orientation, int flag,int *pbc);

// PBC_Extraction.h
void PBC_Extraction(Matrix *img,int width,int height, int *pbc);

// Convert2Matrix
void Convert2Matrix(unsigned char *R, int width,int height, Matrix *image);
double distance_PBC(int *Ref, int* Query);

double distance_PBC(int flag, int *Ref, int* Query);

void min_test(int a, int& min);

bool max_test(int a, int& max);

void ArbitraryShape(unsigned char *a_image, unsigned char *y_image, int image_height, int image_width);	//yjyu - 010222

#endif
