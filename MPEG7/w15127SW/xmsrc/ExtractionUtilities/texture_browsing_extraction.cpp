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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <time.h>

#include "texture_browsing_extraction.h"

#define DEBUG_DIRECTION		0

//-----------------------------------------------------------------------------
// matrix.c
void nrerror(char *error_text) 

{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}

//-----------------------------------------------------------------------------
double *dvector(int nl, int nh)
{
	double *v;

	v = (double *) calloc((unsigned) (nh-nl+1), sizeof(double));
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl;
}

//-----------------------------------------------------------------------------
float *my_vector(int nl,int nh)
{
	float *v;

	v = (float *) calloc((unsigned) (nh-nl+1), sizeof(float));
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl;
}

//-----------------------------------------------------------------------------
void free_dvector(double *v, int nl,int nh)
{
	free((char*) (v+nl));
}

//-----------------------------------------------------------------------------
void free_vector(float *v,int  nl,int nh)
{
	free((char*) (v+nl));
}

//-----------------------------------------------------------------------------
float **matrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	float **m;

	m=(float **) malloc((unsigned) (nrh-nrl+1)*sizeof(float*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(float *) malloc((unsigned) (nch-ncl+1)*sizeof(float));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}

//-----------------------------------------------------------------------------
void free_matrix(float**m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}

//-----------------------------------------------------------------------------
double **dmatrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	double **m;

	m = (double **) calloc((unsigned) (nrh-nrl+1), sizeof(double*));
	if (!m) nrerror("allocation failure 1 in dmatrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i] = (double *) calloc((unsigned) (nch-ncl+1), sizeof(double));
		if (!m[i]) nrerror("allocation failure 2 in dmatrix()");
		m[i] -= ncl;
	}
	return m;
}

//-----------------------------------------------------------------------------
void free_dmatrix(double **m, int nrl, int nrh, int ncl, int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}

//-----------------------------------------------------------------------------
double log2( double a)
{
	return log10(a)/log10(2.0);
}

//-----------------------------------------------------------------------------
void sort(double *Y, int *I, double *A, int length)
{
	int i, j;
	double max, *tmp;

	tmp = (double *) calloc(length, sizeof(double));

	for (i=0;i<length;i++) 
		tmp[i] = A[i];

	max = tmp[0];
	for (i=1;i<length;i++) {
		if (tmp[i] > max) 
			max = tmp[i];
	}

	max = fabs(10*max);

	for (i=0;i<length;i++) {
		Y[i] = tmp[0];
		I[i] = 0;
		for (j=1;j<length;j++) {
			if (tmp[j] < Y[i]) {
				Y[i] = tmp[j];
				I[i] = j;
			}
		}

		tmp[I[i]] = max;
	}

	free(tmp);
}

//-----------------------------------------------------------------------------
void minimun(double *Y, int *I, double *A, int length)
{
	int i, index;
	double min;

	min = A[0];
	index = 0;
	for (i=1;i<length;i++) 
		if (A[i] < min) {
			min = A[i];
			index = i;
		}

	*Y = min;
	*I = index;
}

//-----------------------------------------------------------------------------
void Mat_Abs(Matrix *A)
{
	int h, w;

	for (h=0;h<A->height;h++)
		for (w=0;w<A->width;w++) {
			if (A->data[h][w] < 0)
			    A->data[h][w] = -1.0*(A->data[h][w]);
		}
}

//-----------------------------------------------------------------------------
void Mat_Mean(double *mean,Matrix *A)
{
	int h, w;
	double tmp;

	tmp = 0.0;
	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			tmp += A->data[h][w];
		}
	}

	*mean = tmp/(double) (A->height*A->width);
}

//-----------------------------------------------------------------------------
void Mat_Vector(Matrix *A, float *a)
{
	int h, w;

	for (h=0;h<A->height;h++)
		for (w=0;w<A->width;w++)
			a[h*A->width+w] = (float) A->data[h][w];
}

//-----------------------------------------------------------------------------
void Mat_Shift(Matrix *A, Matrix *B, int side)
{
	int h, w;

	for (h=side;h<B->height;h++)
		for (w=side;w<B->width;w++)
			A->data[h-side][w-side] = B->data[h][w];
		
	for (h=side;h<B->height;h++)
		for (w=0;w<side;w++)
			A->data[h-side][B->width-side+w] = B->data[h][w];

	for (h=0;h<side;h++)
		for (w=side;w<B->width;w++)
			A->data[B->height-side+h][w-side] = B->data[h][w];

	for (h=0;h<side;h++)
		for (w=0;w<side;w++)
			A->data[B->height-side+h][B->width-side+w] = B->data[h][w];
}

//-----------------------------------------------------------------------------
void Mat_Zeros(Matrix *A)
{
	int h, w;

	for (h=0;h<A->height;h++)
		for (w=0;w<A->width;w++)
			A->data[h][w] = 0;
}

//-----------------------------------------------------------------------------
void Mat_Zeros_uc(uc_Matrix *A)
{
	int h, w;

	for (h=0;h<A->height;h++)
		for (w=0;w<A->width;w++)
			A->data[h][w] = 0;
}

//-----------------------------------------------------------------------------
void Mat_Zeros_i(i_Matrix *A)
{
	int h, w;

	for (h=0;h<A->height;h++)
		for (w=0;w<A->width;w++)
			A->data[h][w] = 0;
}

//-----------------------------------------------------------------------------
void CreateMatrix(Matrix **M, int hei,int wid)
{
	int h;

	Matrix *tmp;

	tmp = (Matrix *) calloc(1, sizeof(Matrix));
	tmp->data = (double **) calloc(hei, sizeof(double *));

	if (!(tmp->data)) {
		nrerror("allocation failure in CreateMatrix()");
		exit(1);
	}

	for (h=0; h<hei; h++) {
		tmp->data[h] = (double *) calloc(wid, sizeof(double));
		if (!(tmp->data[h])) {
			nrerror("allocation failure in CreateMatrix()");
			exit(1);
		}
	}

	tmp->height = hei;
	tmp->width = wid;
	
	*M = tmp;
}

//-----------------------------------------------------------------------------
void FreeMatrix(Matrix *M)
{
	int h;

	for (h=0; h<M->height; h++) 
	     free(M->data[h]);
	free(M->data);
}

//-----------------------------------------------------------------------------
void Create_i_Matrix(i_Matrix **M, int hei, int wid)
{
	int h;

	i_Matrix *tmp;

	tmp = (i_Matrix *) calloc(1, sizeof(i_Matrix));
	tmp->data = (int **) calloc(hei, sizeof(int *));
	if (!(tmp->data)) {
		nrerror("allocation failure in Create_i_Matrix()");
		exit(1);
	}

	for (h=0; h<hei; h++) {
		tmp->data[h] = (int *) calloc(wid, sizeof(int));
		if (!(tmp->data[h])) {
			nrerror("allocation failure in Create_i_Matrix()");
			exit(1);
		}
	}

	tmp->height = hei;
	tmp->width = wid;
	
	*M = tmp;
}

//-----------------------------------------------------------------------------
void Free_i_Matrix(i_Matrix *M)
{
	int h;

	for (h=0; h<M->height; h++) 
	     free(M->data[h]);
	free(M->data);
}

//-----------------------------------------------------------------------------
void Create_uc_Matrix(uc_Matrix **M, int hei,int wid)
{
	int h;

	uc_Matrix *tmp;

	tmp = (uc_Matrix *) calloc(1, sizeof(uc_Matrix));
	tmp->data = (unsigned char **) calloc(hei, sizeof(unsigned char *));
	if (!(tmp->data)) {
		nrerror("allocation failure in Create_uc_Matrix()");
		exit(1);
	}

	for (h=0; h<hei; h++) {
		tmp->data[h] = (unsigned char *) calloc(wid, sizeof(unsigned char));
		if (!(tmp->data[h])) {
			nrerror("allocation failure in Create_uc_Matrix()");
			exit(1);
		}
	}

	tmp->height = hei;
	tmp->width = wid;
	
	*M = tmp;
}

//-----------------------------------------------------------------------------
void Free_uc_Matrix(uc_Matrix *M)
{
	int h;

	for (h=0; h<M->height; h++) 
	     free(M->data[h]);
	free(M->data);
}

//-----------------------------------------------------------------------------
void Mat_FFT2(Matrix *Output_real, Matrix *Output_imag, Matrix *Input_real, Matrix *Input_imag)
{
	int xs, ys, i, j;
	double **R, **I, **Fr, **Fi;

	xs = Input_real->height;
	ys = Input_real->width;

        R  = dmatrix(1,xs,1,ys);
        I  = dmatrix(1,xs,1,ys);
        Fr = dmatrix(1,xs,1,ys);
        Fi = dmatrix(1,xs,1,ys);
			
        for (i=1;i<=Input_real->height;i++) 
            for (j=1;j<=Input_real->width;j++) {
                R[i][j] = Input_real->data[i-1][j-1];
                I[i][j] = Input_imag->data[i-1][j-1];
            }

        four2(Fr, Fi, R, I, xs, ys, 1);         /* 2-D FFT */

        for (i=1;i<=Input_real->height;i++) 
            for (j=1;j<=Input_real->width;j++) {
                Output_real->data[i-1][j-1] = Fr[i][j];
                Output_imag->data[i-1][j-1] = Fi[i][j];
            }

        free_dmatrix(R,1,xs,1,ys);
        free_dmatrix(I,1,xs,1,ys);   
        free_dmatrix(Fr,1,xs,1,ys);
        free_dmatrix(Fi,1,xs,1,ys);   
}

//-----------------------------------------------------------------------------
void Mat_IFFT2(Matrix *Output_real, Matrix *Output_imag, Matrix *Input_real, Matrix *Input_imag)
{
	int xs, ys, i, j;
	double **R, **I, **Fr, **Fi, NN;

	xs = Input_real->height;
	ys = Input_real->width;

        R  = dmatrix(1,xs,1,ys);
        I  = dmatrix(1,xs,1,ys);
        Fr = dmatrix(1,xs,1,ys);
        Fi = dmatrix(1,xs,1,ys);

        for (i=1;i<=Input_real->height;i++) 
            for (j=1;j<=Input_real->width;j++) {
                R[i][j] = Input_real->data[i-1][j-1];
                I[i][j] = Input_imag->data[i-1][j-1];
            }

        four2(Fr, Fi, R, I, xs, ys, -1);         /* 2-D IFFT */

	NN = (double) (xs*ys);

        for (i=1;i<=Input_real->height;i++) 
            for (j=1;j<=Input_real->width;j++) {
                Output_real->data[i-1][j-1] = Fr[i][j]/NN;
                Output_imag->data[i-1][j-1] = Fi[i][j]/NN;
            }

        free_dmatrix(R,1,xs,1,ys);
        free_dmatrix(I,1,xs,1,ys);   
        free_dmatrix(Fr,1,xs,1,ys);
        free_dmatrix(Fi,1,xs,1,ys);   
}

//-----------------------------------------------------------------------------
void four2(double **fftr, double **ffti, double **rdata, double **idata, int rs, int cs, int isign)

/************************************************************ 

   2-D fourier transform of data with real part stored in
   "rdata" and imaginary part in "idata" with size "rs" x
   "cs". The result is in "fftr" and "ffti". The isign is
   "isign" =  1 forward, and "isign" = -1 inverse 

*************************************************************/
{
        double **T, *tmp1, *tmp2;
        int i, j;

        tmp1 = dvector(1,2*cs);
        tmp2 = dvector(1,2*rs);
        T = dmatrix(1,2*rs,1,cs);

        for (i=1;i<=rs;i++) {
            for (j=1;j<=cs;j++) {
                tmp1[j*2-1] = rdata[i][j];
                tmp1[j*2] = idata[i][j];
            }
            four1(tmp1, cs, isign);
            for (j=1;j<=cs;j++) {
                T[i*2-1][j] = tmp1[j*2-1];
                T[i*2][j] = tmp1[j*2];
            }
        }

        for (i=1;i<=cs;i++) {
            for (j=1;j<=rs;j++) {
                tmp2[j*2-1] = T[j*2-1][i];
                tmp2[j*2] = T[j*2][i];
            }
            four1(tmp2,rs,isign);
            for (j=1;j<=rs;j++) {
                fftr[j][i] = tmp2[j*2-1];
                ffti[j][i] = tmp2[j*2];
            }
        }
        free_dvector(tmp1, 1, 2*cs);
        free_dvector(tmp2, 1, 2*rs);
        free_dmatrix(T, 1, 2*rs, 1, cs); 
}

//-----------------------------------------------------------------------------
void four1(double *data, int nn, int isign)
{
	int n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	n = nn << 1;
	j = 1;
	for (i=1;i<n;i+=2) {
		if (j > i) {

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
#undef SWAP
		}
		m = n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax) {
		istep = 2*mmax;
		theta = 6.28318530717959/(isign*mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j = i+mmax;
				tempr = wr*data[j]-wi*data[j+1];
				tempi = wr*data[j+1]+wi*data[j];
				data[j] = data[i]-tempr;
				data[j+1] = data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr = (wtemp=wr)*wpr-wi*wpi+wr;
			wi = wi*wpr+wtemp*wpi+wi;
		}
		mmax = istep;
	}
}

//-----------------------------------------------------------------------------
void Mat_Copy(Matrix *A, Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end)
{
	int i, j, h, w, h_done, w_done;

	if ((h_target >= 0)&&(h_target < A->height)&&(w_target >= 0)&&(w_target < A->width)) {
		if ((h_begin >= 0)&&(h_begin < B->height)&&(w_begin >= 0)&&(w_begin < B->width)) {
			h = h_end-h_begin+1;
			w = w_end-w_begin+1;
			if ((h >= 1)&&(w >= 1)) {
				h_done = h_target+h-1;
				w_done = w_target+w-1;
				if ((h_done < A->height)&&(w_done < A->width)) {
					for (i=0;i<h;i++) {
						for (j=0;j<w;j++) {
							A->data[i+h_target][j+w_target] = B->data[i+h_begin][j+w_begin];
						}
					}
				}
			}
		}
	}
	else {
		printf("matrix dimension error!\n");
		exit(1);
	}
}

//-----------------------------------------------------------------------------
void Mat_uc_Copy(uc_Matrix *A, uc_Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end)
{
	int i, j, h, w, h_done, w_done;

	if ((h_target >= 0)&&(h_target < A->height)&&(w_target >= 0)&&(w_target < A->width)) {
		if ((h_begin >= 0)&&(h_begin < B->height)&&(w_begin >= 0)&&(w_begin < B->width)) {
			h = h_end-h_begin+1;
			w = w_end-w_begin+1;
			if ((h >= 1)&&(w >= 1)) {
				h_done = h_target+h-1;
				w_done = w_target+w-1;
				if ((h_done < A->height)&&(w_done < A->width)) {
					for (i=0;i<h;i++) {
						for (j=0;j<w;j++) {
							A->data[i+h_target][j+w_target] = B->data[i+h_begin][j+w_begin];
						}
					}
				}
			}
		}
	}
	else {
		printf("matrix dimension error!\n");
		exit(1);
	}
}
	
//-----------------------------------------------------------------------------
void Mat_i_Copy(i_Matrix *A, i_Matrix *B, int h_target, int w_target, int h_begin, int w_begin, int h_end, int w_end)
{
	int i, j, h, w, h_done, w_done;

	if ((h_target >= 0)&&(h_target < A->height)&&(w_target >= 0)&&(w_target < A->width)) {
		if ((h_begin >= 0)&&(h_begin < B->height)&&(w_begin >= 0)&&(w_begin < B->width)) {
			h = h_end-h_begin+1;
			w = w_end-w_begin+1;
			if ((h >= 1)&&(w >= 1)) {
				h_done = h_target+h-1;
				w_done = w_target+w-1;
				if ((h_done < A->height)&&(w_done < A->width)) {
					for (i=0;i<h;i++) {
						for (j=0;j<w;j++) {
							A->data[i+h_target][j+w_target] = B->data[i+h_begin][j+w_begin];
						}
					}
				}
			}
		}
	}
	else {
		printf("matrix dimension error!\n");
		exit(1);
	}
}

//----------------------------------------------------------------------------
void Mat_Product(Matrix *A, Matrix *B, Matrix *C)
{
	int h, w;

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = B->data[h][w]*C->data[h][w];
		}
	}
}

//-----------------------------------------------------------------------------
void Mat_Sum(Matrix *A, Matrix *B, Matrix *C)
{
	int h, w;

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = B->data[h][w]+C->data[h][w];
		}
	}
}

//-----------------------------------------------------------------------------
void Mat_Substract(Matrix *A, Matrix *B, Matrix *C)
{
	int h, w;

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = B->data[h][w]-C->data[h][w];
		}
	}
}

//-----------------------------------------------------------------------------
void Mat_Fliplr(Matrix *A)
{
	Matrix *tmp;
	int h, w;

	CreateMatrix(&tmp, A->height, A->width);
	
	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			tmp->data[h][w] = A->data[h][(A->width)-w-1];
		}
	}

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = tmp->data[h][w];
		}
	}

	FreeMatrix(tmp);
}

//-----------------------------------------------------------------------------
void Mat_Flipud(Matrix *A)
{
	Matrix *tmp;
	int h, w;

	CreateMatrix(&tmp, A->height, A->width);
	
	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			tmp->data[h][w] = A->data[(A->height)-h-1][w];
		}
	}

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = tmp->data[h][w];
		}
	}

	FreeMatrix(tmp);
}

//-----------------------------------------------------------------------------
void Mat_uc_Fliplr(uc_Matrix *A)
{
	uc_Matrix *tmp;
	int h, w;

	Create_uc_Matrix(&tmp, A->height, A->width);
	
	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			tmp->data[h][w] = A->data[h][(A->width)-w-1];
		}
	}

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = tmp->data[h][w];
		}
	}

	Free_uc_Matrix(tmp);
}

//-----------------------------------------------------------------------------
void Mat_uc_Flipud(uc_Matrix *A)
{
	uc_Matrix *tmp;
	int h, w;

	Create_uc_Matrix(&tmp, A->height, A->width);
	
	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			tmp->data[h][w] = A->data[(A->height)-h-1][w];
		}
	}

	for (h=0;h<A->height;h++) {
		for (w=0;w<A->width;w++) {
			A->data[h][w] = tmp->data[h][w];
		}
	}

	Free_uc_Matrix(tmp);
}

// end of matrix.c

//-----------------------------------------------------------------------------
// direction.c
int *DominantDirection(float **histo)
{

 int i,j,k;
 double *temp_ori, *out_ori;
 int *temp_index;
 struct Peak histo_peak[4][2];
 int 	candi_size=2;
 int    temp_flag;
 int    lable,count;
 int	*index_value;
 double *contrast;
 double sum,*temp_contrast;
 int    *final_index, peak_count;
 struct Peak *temp_peak;
 double *temp_value;
 int *temp_c_index;

 temp_index=(int *)malloc(ORIENTATION*sizeof(int));
 temp_ori=(double *)malloc(ORIENTATION*sizeof(double));
 out_ori=(double *)malloc(ORIENTATION*sizeof(double));
 final_index=(int *)malloc(2*sizeof(int));

 for(i=0;i<SCALE;i++)
 {
  for(j=0;j<ORIENTATION;j++)
  {
   temp_ori[j]=(double)histo[i][j];
  }
  
  temp_peak=(struct Peak *)malloc(ORIENTATION*sizeof(struct Peak));

  peak_count=0;
  for(j=0;j<ORIENTATION;j++)
  {
   if(j==0)
    if(temp_ori[j]>temp_ori[j+1]) 
    {
     temp_peak[peak_count].index=j;
     temp_peak[peak_count].value=temp_ori[j];
     peak_count++;
     }

    if(j==(ORIENTATION-1))
     if(temp_ori[j]>temp_ori[j-1])
     {
      temp_peak[peak_count].index=j;
      temp_peak[peak_count].value=temp_ori[j];
      peak_count++;
      }

     if(j>0 && j<(ORIENTATION-1))
       if(temp_ori[j]>=temp_ori[j-1] && temp_ori[j]>=temp_ori[j+1])
       {
        temp_peak[peak_count].index=j;
        temp_peak[peak_count].value=temp_ori[j];
        peak_count++;
        }
       
   }

 if(DEBUG_DIRECTION)
 {
  printf("peaks:\n");
  printf("peak_count=%d\n",peak_count);
  for(j=0;j<peak_count;j++)
   printf("%d ",temp_peak[j].index);
  printf("\n");
 }

  temp_value=(double *)malloc(peak_count*sizeof(double));
  for(j=0;j<peak_count;j++)
   temp_value[j]=temp_peak[j].value;
    
  sort(out_ori,temp_index,temp_value,peak_count);

  if(peak_count==1)
  {
   for(j=0;j<candi_size;j++)
   {
    histo_peak[i][j].index=temp_peak[peak_count-1].index;
    histo_peak[i][j].value=temp_peak[peak_count-1].value;
    histo_peak[i][j].contrast=ComputeContrast(histo_peak[i][j].index,temp_ori,ORIENTATION);
    histo_peak[i][j].flag=0;
   }
  }
  else {
   for(j=0;j<candi_size;j++)
   {
    histo_peak[i][j].index=temp_peak[temp_index[peak_count-1-j]].index;
    histo_peak[i][j].value=out_ori[peak_count-1-j];
    histo_peak[i][j].contrast=ComputeContrast(histo_peak[i][j].index,temp_ori,ORIENTATION);
    histo_peak[i][j].flag=0;
   }
  }
   
  free(temp_value);
  free(temp_peak); 
 }


 for(i=0;i<(SCALE-1);i++)
 {
  for(j=0;j<candi_size;j++)
  {
   temp_flag=JudgeContinuity(histo_peak[i][j].index,j,histo_peak[i+1],candi_size);
   if(histo_peak[i][j].flag==0) histo_peak[i][j].flag=temp_flag;
  }
 }

 if(DEBUG_DIRECTION)
 {
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<candi_size;j++)
		printf("%d %f %f %d\n",histo_peak[i][j].index,histo_peak[i][j].value,histo_peak[i][j].contrast,histo_peak[i][j].flag);
		printf("\n");
	}	 
 }

 sum=0.0;
 for(i=0;i<SCALE;i++)
  for(j=0;j<candi_size;j++)
    sum+=histo_peak[i][j].flag;

 if(sum==0.0)
 {
	 final_index[0]=6;
	 final_index[1]=6;
 }
 else {

  count=0;
  index_value=(int *)malloc(ORIENTATION*sizeof(int));
  for(i=0;i<SCALE;i++)
  {
   lable=0;
   for(j=0;j<candi_size;j++)
    if(histo_peak[i][j].flag==1)
    {
     index_value[count]=histo_peak[i][j].index;
     lable=1;
     break;
    }
    if(lable==1) break;
   }

   for(i=0;i<SCALE;i++)
    for(j=0;j<candi_size;j++)
    {
     if(histo_peak[i][j].flag==1)
     {
      lable=0;
      for(k=0;k<count+1;k++)
      {
       if(histo_peak[i][j].index==index_value[k])
       {
        lable=1;
        break;
       }
      }
      if(lable==0) 
      {
       count++;
       index_value[count]=histo_peak[i][j].index;
      }
     }
   } 

  if(count==0)
  {
	  final_index[0]=index_value[0]; 
	  final_index[1]=6;
  }
  else {

  if(DEBUG_DIRECTION)
  {
   for(i=0;i<count+1;i++)
    printf("%d ",index_value[i]);
   printf("\n"); 
  }
  
   contrast=(double *)malloc((count+1)*sizeof(double));
   for(i=0;i<count+1;i++)
   {
    contrast[i]=0.0;
    for(j=0;j<SCALE;j++)
     for(k=0;k<candi_size;k++)
     {
      if(histo_peak[j][k].flag==1 && histo_peak[j][k].index==index_value[i])
      {
       if(histo_peak[j][k].contrast>contrast[i]) contrast[i]=histo_peak[j][k].contrast;
      }
     }
    }

  if(DEBUG_DIRECTION)
  {
   printf("contrast:\n");
   for(i=0;i<count+1;i++)
    printf("%f ",contrast[i]);
   printf("\n");
  }

   temp_contrast=(double *)malloc((count+1)*sizeof(double));	 
   
   temp_c_index=(int *)malloc((count+1)*sizeof(int));
   sort(temp_contrast,temp_index,contrast,count+1);
   
   for(j=0;j<2;j++)
   {
    for(i=0;i<count+1;i++)
     if(contrast[i]==temp_contrast[count-j]) 
	 {
     final_index[j]=index_value[i];
     break;
     }
   }
  
   free(contrast);
   free(temp_contrast);
   free(temp_c_index);
  }
  free(index_value); 
 }    
    

 free(temp_index);
 free(temp_ori);
 free(out_ori);

 return final_index;
 
}

//-----------------------------------------------------------------------------
int JudgeContinuity(int index,int order,struct Peak *peak, int size)
{
 int status;
 int i;

 status=0; 
 for(i=0;i<size;i++)
 {
  if(index==peak[i].index)
  {
   peak[i].flag=1;
   status=1;
   break;
  }
 }

 return status;

}

//-----------------------------------------------------------------------------
double ComputeContrast(int index,double *histo,int len)
{
 double slope;
 double temp_slop1,temp_slop2;

 if(index==0)
  slope=histo[index]-histo[index+1];
 else
  if(index==len-1)
   slope=histo[index]-histo[index-1];
  else
  {
   temp_slop1=histo[index]-histo[index-1];
   temp_slop2=histo[index]-histo[index+1];
  slope=(temp_slop1+temp_slop2)/2;
  }

 return slope;
}
  
// end of direction.c

//-----------------------------------------------------------------------------
// Gabor.c
void Gabor(Matrix *Gr, Matrix *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int flag)
{
	double base, a, u0, var, X, Y, G, t1, t2, m;
	int x, y, side;

	base = Uh/Ul;
	a = pow(base, 1.0/(double)(scale-1));

	u0 = Uh/pow(a, (double) scale-s);

        var = pow(0.6/Uh*pow(a, (double) scale-s), 2.0);

	t1 = cos((double) M_PI/orientation*(n-1.0));
	t2 = sin((double) M_PI/orientation*(n-1.0));

	side = (int) (Gr->height-1)/2;

	for (x=0;x<2*side+1;x++) {
		for (y=0;y<2*side+1;y++) {
			X = (double) (x-side)*t1+ (double) (y-side)*t2;
			Y = (double) -(x-side)*t2+ (double) (y-side)*t1;
			G = 1.0/(2.0*M_PI*var)*pow(a, (double) scale-s)*exp(-0.5*(X*X+Y*Y)/var);
			Gr->data[x][y] = G*cos(2.0*M_PI*u0*X);
			Gi->data[x][y] = G*sin(2.0*M_PI*u0*X);
		}
	}

	/* if flag == 1, then remove the DC from the real part of Gabor */

	if (flag == 1) {
		m = 0;
		for (x=0;x<2*side+1;x++)
			for (y=0;y<2*side+1;y++)
				m += Gr->data[x][y];

		m /= pow((double) 2.0*side+1, 2.0);

		for (x=0;x<2*side+1;x++)
			for (y=0;y<2*side+1;y++)
				Gr->data[x][y] -= m;
	}	
}

// end of Gabor.c

//-----------------------------------------------------------------------------
// imagerotate.c
int billinear(Matrix *img,float a,float b,int ii,int jj)
{
	double y;

	if( (ii<0) || (ii>=img->height-1) ) return 255;
	if( (jj<0) || (jj>=img->width-1) ) return 255;
	if( (a==0.0) && (b==0.0) ) return (int)img->data[ii][jj];

	if(a==0.0) {
		y=(1-b)*img->data[ii][jj]+b*img->data[ii][jj+1];
		return (int)(y+0.5);
	}

	if(b==0.0) {
		y=(1-a)*img->data[ii][jj]+a*img->data[ii+1][jj];
		return (int)(y+0.5);
	}

	y=(1-a)*(1-b)*img->data[ii][jj]+(1-a)*b*img->data[ii][jj+1]+
		a*(1-b)*img->data[ii+1][jj]+a*b*img->data[ii+1][jj+1];

	return (int)(y+0.5);

}

//-----------------------------------------------------------------------------
void ImgRotate(Matrix *inImg,float angle)
{
	int i,j,ci,cj;
	float sina,cosa,oldi,oldj;
	float alpha,beta;
	int NN,MM,ii,jj;
	Matrix *rImg;

	CreateMatrix(&rImg,inImg->height,inImg->width);

	angle=(float)angle*(3.1415926535/180.0);
	MM=inImg->width; NN=inImg->height;

	sina=(float)sin(angle); cosa=(float)cos(angle);
	ci=NN/2; cj=MM/2;

	for(i=0;i<NN;i++)
		for(j=0;j<MM;j++)
		{
			oldi=(i-ci)*cosa-(j-cj)*sina+ci;
			oldj=(i-ci)*sina+(j-cj)*cosa+cj;
			ii=(int)oldi;
			jj=(int)oldj;
			alpha=oldi-(float)ii;
			beta=oldj-(float)jj;
			rImg->data[i][j]=(unsigned char)billinear(inImg,alpha,beta,ii,jj);

		}

	for(i=0;i<NN;i++)
		for(j=0;j<MM;j++)
			inImg->data[i][j]=rImg->data[i][j];

	FreeMatrix(rImg);

	
}

// end of imagerotate.c

//-----------------------------------------------------------------------------
// loadGrayImage.c
void LoadGrayImg(Matrix *I, char *fn)
{
	FILE *fp;
	unsigned char *tmp;
	int i, j, hei = I->height, wid = I->width;

	tmp = (unsigned char *) calloc(hei*wid, sizeof(unsigned char));

	if ((fp = fopen(fn,"r")) == NULL) {
		printf("%s can not be open!\n", fn);
		exit(0);
	}
	fread(tmp, sizeof(unsigned char), hei*wid, fp);
	fclose(fp);

	for (i=0;i<hei;i++)
		for (j=0;j<wid;j++) 
			I->data[i][j] = (double) tmp[i*wid+j];

	free(tmp);
}

// end of loadgrayimage.c

//-----------------------------------------------------------------------------
// pbc.c
void swap_double( double *data_ptr ) {
  union fl_union {
    unsigned char ch[8];
    double fl;
  };
  union fl_union tmp;
  unsigned char c1, c2, c3, c4, c5, c6, c7, c8;

  tmp.fl = *data_ptr;
  c1 = tmp.ch[0];
  c2 = tmp.ch[1];
  c3 = tmp.ch[2];
  c4 = tmp.ch[3];
  c5 = tmp.ch[4];
  c6 = tmp.ch[5];
  c7 = tmp.ch[6];
  c8 = tmp.ch[7];
  tmp.ch[7] = c1;
  tmp.ch[6] = c2;
  tmp.ch[5] = c3;
  tmp.ch[4] = c4;
  tmp.ch[3] = c5;
  tmp.ch[2] = c6;
  tmp.ch[1] = c7;
  tmp.ch[0] = c8;
  *data_ptr = tmp.fl;
}

//-----------------------------------------------------------------------------
void pbcmain( struct pbc_struct *pbc, int size)
{
  char c5[200];
  float row_credit[3],column_credit[3],image_credit;

  int img_size;

  img_size = (int)(size*0.625);

  sprintf(c5,"default");
  
//printf("%s\n",c5);
  proj_analysis(c5,1,row_credit, pbc,img_size);
  proj_analysis(c5,2,column_credit, pbc,img_size);
//printf("row_credit=%f column_credit=%f\n",row_credit[2],column_credit[2]);
  image_credit=row_credit[2]+column_credit[2];
  
  // For returning to calling function.
  pbc->structuredness = image_credit;
		

}

//-----------------------------------------------------------------------------
//proj_type ==1 for Row, proj_type==2 is used for Column
void proj_analysis(char *image_name,int proj_type,float *credit, struct pbc_struct *pbc,int img_size)
{
  int Flag_Auto_Analysis=1, Flag_Proj_Credit=1, Flag_Contrast_Out=1;
  char c6[200],c5[200];
  float **Proj_Candi_Valu,**contrast,*Peak,peak_diff,dis_ratio,dis_peak,var_dis;
  float **picked_cand,max_contrast;
  int Candi_Avail,count_candi,leng_B,size_peak,*PeakI,**Proj_Candi_Posi,flag;
  int *refine_cand,pick_num,**picked_cand_posi;
  int i,j,k;
  double *A,*B;
  FILE *fp;
  int my_debug;

  //img_size=PROJ_SIZE;
  my_debug=DEBUGLEV;
  
// SHOULD BE CHANGED: Input image name conventions
  if (proj_type==1) sprintf(c6,"%s_Row",image_name);
  else if (proj_type==2) sprintf(c6,"%s_Column",image_name);
 
  if (my_debug==2) printf("%s\n",c6);

  if (Flag_Auto_Analysis == 1)
  {
    Proj_Candi_Posi = (int **)imatrix(24,2);
    Proj_Candi_Valu = (float **)tb_fmatrix(24,2);
    Candi_Avail=0;
    count_candi=0;
    contrast= (float **)tb_fmatrix(4,6);
  }

// I guess A is used for the storage of the projections
  A = (double *)calloc(img_size,sizeof(double));
  for (i=0;i<4;i++)
  {
    for (j=0;j<6;j++)
    {
// THIS understands the input file name for the corresponding scale and orientation (i,j)
      if (proj_type==1)
        sprintf(c5,"%s_Row_%d%d",image_name,i,j);
      else if (proj_type==2)
        sprintf(c5,"%s_Column_%d%d",image_name,i,j);
      
// Opens the suitable projection file	  
	  fp = fopen(c5,"rb");
      if (!fp)  { printf("unable to open %s\n",c5); exit (-1); }
      fread(A,sizeof(double),img_size,fp);
      fclose(fp);

// fixme Swap the byte order.
//	  for( k=0; k<img_size; k++ ) {
//		swap_double( &(A[k]) );
//	  }

      leng_B=auto1_radon(A,img_size,&B);
      if (my_debug==2)
      {
        //printf("B = %d \n",leng_B);
        //for (k=0;k<leng_B;k++) printf("%d %f ",k,B[k]);
        //printf("\n");
      }

      if (Flag_Auto_Analysis==1)
      {
        PeakI = (int *)calloc(leng_B,sizeof(int));
        Peak = (float *)calloc(leng_B,sizeof(float));
        size_peak=peak_proj(B,leng_B,PeakI,Peak);
        if (size_peak>1) 
          contrast[i][j] = proj_contrast(B,leng_B,PeakI,Peak,size_peak);
        if (my_debug==2) printf("%f \n",contrast[i][j]);

        if (size_peak>1)
        {
          if (size_peak == 2)
          {
            peak_diff = (float)fabs(Peak[0]-Peak[1]);
            if (peak_diff<=0.13 && contrast[i][j]>0.5) 
            {
              dis_peak = (float)(PeakI[1]-PeakI[0]);
              var_dis = 0.0;

              Proj_Candi_Posi[count_candi][0]=i;
              Proj_Candi_Posi[count_candi][1]=j;
              Proj_Candi_Valu[count_candi][0]=dis_peak;
              Proj_Candi_Valu[count_candi][1]=var_dis;

              Candi_Avail=1; 
              count_candi ++;
            }
          }
          else 
          {
            peak_summary(&dis_peak,&var_dis,PeakI,Peak,size_peak);
            dis_ratio = var_dis/dis_peak;
            if (dis_ratio<0.14)
            {
              Proj_Candi_Posi[count_candi][0]=i;
              Proj_Candi_Posi[count_candi][1]=j;
              Proj_Candi_Valu[count_candi][0]=dis_peak;
              Proj_Candi_Valu[count_candi][1]=var_dis;

              Candi_Avail=1;
              count_candi ++;
            }
          }
        }
        free(Peak); free(PeakI);
      }
      free(B);
      Candi_Avail=0;
    }
  }
  free(A);

  if (my_debug==3) 
  {
    printf("Proj_Candi\n");
    for (k=0;k<count_candi;k++) 
      printf("%d %d\n",Proj_Candi_Posi[k][0]+1,Proj_Candi_Posi[k][1]+1);
    for (k=0;k<count_candi;k++) 
      printf("%7.4f %7.4f\n",Proj_Candi_Valu[k][0],Proj_Candi_Valu[k][1]);
	printf("count_candi=%d\n",count_candi);
  }

//  printf("count_candi=%d\n",count_candi);
  if (Flag_Auto_Analysis == 1)
  {
    flag = 0;
    if (count_candi>0)
    {
      refine_cand = (int *)calloc(count_candi,sizeof(int));
      flag = cand_cluster(Proj_Candi_Valu,count_candi,2,refine_cand,&pick_num);
      if (my_debug==2)
      {
        printf("refine_cand\n");
        for (k=0;k<pick_num;k++) printf("%d ",refine_cand[k]);
        printf("flag %d\n",flag);
      }

      picked_cand = (float **)tb_fmatrix(pick_num,2);
      picked_cand_posi = (int **)imatrix(pick_num,2);

      for (i=0;i<pick_num;i++) 
      {
        picked_cand[i][0]=Proj_Candi_Valu[refine_cand[i]][0];
        picked_cand[i][1]=Proj_Candi_Valu[refine_cand[i]][1];
        picked_cand_posi[i][0]=Proj_Candi_Posi[refine_cand[i]][0];
        picked_cand_posi[i][1]=Proj_Candi_Posi[refine_cand[i]][1];
      }

      if (my_debug==3)
      {
        printf("picked_cand \n");
        for (k=0;k<pick_num;k++)
          printf("%d %d\n",picked_cand_posi[k][0]+1,picked_cand_posi[k][1]+1);
        for (k=0;k<pick_num;k++)
          printf("%7.4f %7.4f\n",picked_cand[k][0],picked_cand[k][1]);
      }

      if (Flag_Contrast_Out==1)
      {
        max_contrast = -100000.0;
        for (i=0;i<pick_num;i++) 
        {
          if (max_contrast<contrast[picked_cand_posi[i][0]][picked_cand_posi[i][1]])
          {
            max_contrast=contrast[picked_cand_posi[i][0]][picked_cand_posi[i][1]];
            k=i;
          }
        }
          
		if (proj_type==1) {
			pbc->element[0] = picked_cand_posi[k][0]+1;
			//pbc->element[1] = picked_cand_posi[k][1]+1;
		}
		else if (proj_type==2) {
			pbc->element[2] = picked_cand_posi[k][0]+1;
			//pbc->element[3] = picked_cand_posi[k][1]+1;
		}

      }

      if (Flag_Proj_Credit==1)
        proj_credit_modi(picked_cand_posi,pick_num,count_candi,flag,credit);

      free_imatrix(picked_cand_posi,pick_num);
      tb_free_fmatrix(picked_cand,pick_num);
      free (refine_cand);
    }
  }

  if (Flag_Auto_Analysis == 1)
  {
    free_imatrix(Proj_Candi_Posi,24);
    tb_free_fmatrix(Proj_Candi_Valu,24); 
    tb_free_fmatrix(contrast,4);
  }
}

//-----------------------------------------------------------------------------
void proj_credit_modi(int **candidate,int size_cand,int size_count_candi,int flag,
    float *cand_credit)
{
  int i,j,**cand_pair,**cand_class_map,**scale_map,**direc_map,num_scales;
  int scale_num[4],direc_num[6],num_direc,**direc_cand,pbc_class[3];
  float value[3];
  int my_debug;

  my_debug=DEBUGLEV;

  for (i=0;i<3;i++) cand_credit[i]=0;
  if (size_cand==1) 
  {
    cand_credit[0]=(float)0.2; 
	cand_credit[1]=(float)0.0; 
	cand_credit[2]=(float)0.2;
  }
  else if (size_cand==size_count_candi && flag==1)
  {
    cand_credit[2]=(float)(size_cand*0.2);
  }
  else
  {
    cand_class_map = (int **)imatrix(4,6);
    scale_map = (int **)imatrix(4,6);
    direc_map = (int **)imatrix(4,6);
    cand_pair=(int **)imatrix(size_cand,2);
    for (i=0;i<size_cand;i++)
    {
      cand_pair[i][0]=candidate[i][0];
      cand_pair[i][1]=candidate[i][1];
    }
    num_scales=get_num(cand_pair,size_cand,scale_num,4);
    get_map(cand_pair,size_cand,num_scales,scale_num,1,scale_map);
    direc_cand = (int **)imatrix(size_cand,2);
    convertcand(cand_pair,direc_cand,size_cand);
    num_direc=get_num(direc_cand,size_cand,direc_num,6);
    get_map(direc_cand,size_cand,num_direc,direc_num,2,direc_map);
    for (i=0;i<4;i++)
      for (j=0;j<6;j++) cand_class_map[i][j]=MIN(scale_map[i][j],direc_map[i][j]);
    if (my_debug==3)
    {
      printf("cand_class_map\n");
      for (i=0;i<4;i++)
      {
        for (j=0;j<6;j++) printf("%d ",cand_class_map[i][j]);
        printf("\n");
      }
    }

    value[0]=(float)1.0; value[1]=(float)0.5; value[2]=(float)0.2;
    pbc_class[0]=0; pbc_class[1]=0; pbc_class[2]=0;
    for (i=0;i<4;i++)
    {
      for (j=0;j<6;j++)
      {
        if (cand_class_map[i][j]==1) pbc_class[0]++;
        else if (cand_class_map[i][j]==2) pbc_class[1]++;
        else if (cand_class_map[i][j]==3) pbc_class[2]++;
      }
    }
    for (i=0;i<3;i++) cand_credit[2] += pbc_class[i]*value[i];

    free_imatrix(direc_cand,size_cand);
    free_imatrix(cand_class_map,4);
    free_imatrix(scale_map,4);
    free_imatrix(direc_map,4);
    free_imatrix(cand_pair,size_cand);
  }
}

//-----------------------------------------------------------------------------
void convertcand(int **cand_pair,int **direc_cand,int size_cand)
{
  int i,*index,*tmpcand;
  int my_debug;

  my_debug=DEBUGLEV;

  tmpcand = (int *)calloc(size_cand,sizeof(int));
  index = (int *)calloc(size_cand,sizeof(int));
  for (i=0;i<size_cand;i++) tmpcand[i]=cand_pair[i][1];
  piksrtintS2B(size_cand,tmpcand,index);
  for (i=0;i<size_cand;i++)
  {
    direc_cand[i][0] = cand_pair[index[i]][1];
    direc_cand[i][1] = cand_pair[index[i]][0];
  }
  free(index); free(tmpcand);
  if (my_debug==1)
  {
    for (i=0;i<size_cand;i++)
    {
      printf("%d %d\n",direc_cand[i][0]+1,direc_cand[i][1]+1);
    }
  }
}

//-----------------------------------------------------------------------------
void get_map(int **cand_pair,int size_cand,int total,int *num,int type,int **map)
{
  int start,i,j,**in_pair,*sing;
  int my_debug;

  my_debug=DEBUGLEV;

  start=0;
  for (i=0;i<total;i++)
  {
    in_pair = (int **)imatrix(num[i],sizeof(int));
    for (j=start;j<start+num[i];j++)
    {
      in_pair[j-start][0]=cand_pair[j][0];
      in_pair[j-start][1]=cand_pair[j][1];
    }
    sing = (int *)calloc(num[i],sizeof(int));
    sing_map(in_pair,num[i],sing,type);
    if (type==1)
    {
      for (j=0;j<num[i];j++)
        map[in_pair[j][0]][in_pair[j][1]] = sing[j];
    }
    else if (type==2)
    {
      for (j=0;j<num[i];j++)
        map[in_pair[j][1]][in_pair[j][0]] = sing[j];
    }
    free(sing);
    free_imatrix(in_pair,num[i]);
    start += num[i];
  }
  if (my_debug==1)
  {
    printf("map type %d\n",type);
    for (i=0;i<4;i++)
    {
      for (j=0;j<6;j++) printf("%d ",map[i][j]);
      printf("\n");
    }
  }
}

//-----------------------------------------------------------------------------
void sing_map(int **inpair,int size_in,int *sing,int type)
{
  int i;
  int my_debug;

  my_debug=DEBUGLEV;

  if (size_in==1) sing[0]=3;
  else
  {
    if (type==1 && inpair[0][1]==0 && inpair[size_in-1][1]==5)
    {
      sing[0]=1; sing[size_in-1]=1;
      if (size_in>2)
      {
        for (i=1;i<size_in-1;i++)
        {
          if (inpair[i][1]==inpair[i-1][1]+1 || inpair[i][1]==inpair[i+1][1]-1)
            sing[i]=1;
          else sing[i]=2;
        }
      }
    }
    else
    {
      if (size_in==2)
      {
        if (inpair[0][1]==inpair[1][1]-1) { sing[0]=1; sing[1]=1; }
        else { sing[0]=2; sing[1]=2; }
      }
      else
      {
        for (i=1;i<size_in-1;i++)
        {
          if (inpair[i][1]==inpair[i-1][1]+1 || inpair[i][1]==inpair[i+1][1]-1)
            sing[i]=1;
          else sing[i]=2;
        }

        if (inpair[0][1]==inpair[1][1]-1) sing[0]=1;
        else sing[0]=2;

        if (inpair[size_in-1][1]==inpair[size_in-2][1]+1) sing[size_in-1]=1;
        else sing[size_in-1]=2;
      }
    }
  }
  if (my_debug==1)
  {
    printf("sing ");
    for (i=0;i<size_in;i++) printf("%d ",sing[i]);
    printf("\n");
  }
}

//-----------------------------------------------------------------------------
int get_num(int **cand_pair,int size_cand,int *num,int len)
{
  int i,temp_scale,total;
  int my_debug;

  my_debug=DEBUGLEV;

  for (i=0;i<len;i++) num[i]=0;
  total=1;
  temp_scale = cand_pair[0][0];
  num[0]=1;
  for (i=1;i<size_cand;i++)
  {
    if (cand_pair[i][0]!=temp_scale)
    {
      total++;
      temp_scale = cand_pair[i][0];
    }
    num[total-1]++;
  }
  if (my_debug==1)
  {
    printf("num %d\n",total);
    for (i=0;i<total;i++) printf("%d ",num[i]);
    printf("\n");
  }
  return total;
}

//-----------------------------------------------------------------------------
int cand_cluster(float **candi,int size_in,int ndim,int *y,int *pick_num)
{
  int flag=0,*cluster,*count,i,newN,maxcount,domi_num;
  float temp;
  int my_debug;

  my_debug=DEBUGLEV;

  if (size_in<=2)
  {
    if(size_in==1)
	{
		y[0]=0;
		if(candi[0][1]>2) flag=1;
		*pick_num=1;
	}
    if (size_in==2)
    {
	  y[0]=0; y[1]=1;
      temp = var_vectors(candi,size_in,ndim);
      if (temp>2) flag=1;
	  *pick_num = 2;
    }
    
  } 
  else
  {
    cluster = (int *)calloc(size_in,sizeof(int));
    count = (int *)calloc(size_in,sizeof(int));
    newN = agglom(candi,cluster,ndim,2.0,count,size_in,2);
    if (newN==size_in) flag=1;
    if (my_debug==2)
    {
      for (i=0;i<size_in;i++) printf("%d ",cluster[i]+1);
      printf("\n");
      for (i=0;i<newN;i++) printf("%d ",count[i]);
      printf("\n");
    }

    if (newN==size_in) 
    {
      *pick_num=newN;
      for (i=0;i<newN;i++) y[i]=i; 
    }
    else
    {
      maxcount=0;
      for (i=0;i<newN;i++)
      {
        if (maxcount<count[i]) { maxcount=count[i]; domi_num=i; }
      }
      *pick_num=0;
      for (i=0;i<size_in;i++) 
      { 
        if (cluster[i]==domi_num) { y[(*pick_num)]=i; (*pick_num)++; }
      }
    }
    free (count); free(cluster);
  }
  return flag;
}

//-----------------------------------------------------------------------------
int agglom(float **candi,int *label,int N,float thresh,int *count,int Nlabel,int fN)
{
  int i,j,newNlabel,mini,minj,total;
  float **dist,mindist,**labelcf;

  labelcf= (float **)tb_fmatrix(Nlabel,N);
  for (i=0;i<Nlabel;i++) 
    for (j=0;j<N;j++) labelcf[i][j]=candi[i][j];
  for (i=0;i<Nlabel;i++) { label[i]=i; count[i]=1; }

  dist=(float **)tb_fmatrix(Nlabel,Nlabel);
  mindist=100000;
  for (i=0;i<Nlabel-1;i++)
  {
    for (j=i+1;j<Nlabel;j++)
    {
      dist[i][j] = tb_distance(labelcf[i],labelcf[j],N);
      dist[j][i] = dist[i][j];
      if (dist[i][j]<mindist) { mindist=dist[i][j]; mini=i; minj=j; }
    }
  }

  newNlabel=Nlabel;
  while (mindist<thresh && newNlabel>fN)
  {
    for (i=0;i<Nlabel;i++)
    {
      if (label[i]==minj) label[i]=mini;
      else if (label[i]>minj) label[i]--;
    }

    total=count[mini]+count[minj];
    for (j=0;j<N;j++)
      labelcf[mini][j] = (count[mini]*labelcf[mini][j] + count[minj]*labelcf[minj][j])                         /total;
    count[mini]=total;
    for (i=minj;i<newNlabel-1;i++)
    {
      count[i]=count[i+1];
      for (j=0;j<N;j++) labelcf[i][j] = labelcf[i+1][j];
    }
    for (i=minj;i<newNlabel-1;i++)
    {
      for (j=0;j<minj;j++) dist[i][j]=dist[i+1][j];
    }
    for (j=minj;j<newNlabel-1;j++)
    {
      for (i=0;i<minj;i++) dist[i][j]=dist[i][j+1];
    }
    for (i=minj;i<newNlabel-1;i++)
    {
      for (j=minj;j<newNlabel-1;j++) dist[i][j]=dist[i+1][j+1];
    }

    newNlabel--;
    for (j=0;j<newNlabel;j++)
    {
      dist[mini][j] = tb_distance(labelcf[mini],labelcf[j],N);
      dist[j][mini] = dist[mini][j];
    }
    mindist=100000;
    for (i=0;i<newNlabel-1;i++)
    {
      for (j=i+1;j<newNlabel;j++)
      {
        if (dist[i][j]<mindist) { mindist=dist[i][j]; mini=i; minj=j; }
      }
    }
  }
  tb_free_fmatrix(dist,Nlabel);
  tb_free_fmatrix(labelcf,Nlabel);
  return newNlabel;
}

//-----------------------------------------------------------------------------
float var_vectors(float **vector,int nvec,int ndim)
{
  float *mean,var;
  int i,j;

  mean = (float *)calloc(ndim,sizeof(float));
  for (i=0;i<nvec;i++)
  {
    for (j=0;j<ndim;j++) mean[j] += vector[i][j];
  }
  for (j=0;j<ndim;j++) mean[j] /= nvec;

  var = 0;
  for (i=0;i<nvec;i++)
  {
    for (j=0;j<ndim;j++) var += (vector[i][j]-mean[j])*(vector[i][j]-mean[j]);
  }
  var =(float)sqrt(var/nvec);
  free(mean);
  return var;
}

//-----------------------------------------------------------------------------
void peak_summary(float *dis_peak,float *var_dis,int *PeakI,float *Peak,int size_peak)
{
  float *dis_array,dis,sum;
  int len,i;

  len = size_peak-1;
  dis_array = (float *)calloc(len,sizeof(float));
  dis = 0;
  for (i=1;i<size_peak;i++) 
  {
    dis_array[i-1] = (float)(PeakI[i]-PeakI[i-1]);
    dis += dis_array[i-1];
  }
  dis /= len;

  sum = 0;
  for (i=0;i<len;i++) sum += (dis_array[i]-dis)*(dis_array[i]-dis);
  *var_dis = (float)sqrt(sum/len);
  *dis_peak = dis;
  free(dis_array);
}

//-----------------------------------------------------------------------------
float proj_contrast(double *B,int leng_B,int *PeakI,float *Peak,int num_peak)
{
  int count,i,j,*index_valley;
  float *valley_value,meanp,meanv;

  count = num_peak-1;
  index_valley = (int *)calloc(count,sizeof(int));
  valley_value = (float *) calloc(count,sizeof(float));

  meanv = 0;
  for (i=0;i<count;i++)
  {
    for (j=PeakI[i]+1;j<=PeakI[i+1]-1;j++)
    {
      if (B[j]<=B[j-1] && B[j]<=B[j+1])
      {
        index_valley[i]=j;
        valley_value[i]= (float) B[j];
        break;
      }
    }
    meanv += valley_value[i];
  }
  meanv /= count;

  meanp = 0;
  for (i=0;i<num_peak;i++) meanp += Peak[i];
  meanp /= num_peak;

  free(index_valley); free(valley_value);
  return (meanp-meanv);
}

//-----------------------------------------------------------------------------
int peak_proj(double *B,int leng_B,int *PeakI,float *Peak)
{
  int count=0,i;
  int my_debug;

  my_debug=DEBUGLEV;

  for (i=1;i<leng_B-1;i++)
  {
    if (B[i]>B[i-1] && B[i]>B[i+1])
    {
      PeakI[count]=i; Peak[count]=(float) B[i];
      count ++;
    }
  }
  if (my_debug==2)
  {
    printf("peak\n");
    for (i=0;i<count;i++) printf("%d %f\n",PeakI[i],Peak[i]);
  }
  return count;
}

//-----------------------------------------------------------------------------
int auto1_radon(double *x_in,int x_long,double **y)
{
  int x_start,x_end,long0,long1,i,j,k,cut;
  double *x,*sum,*A,*C,temp1,temp2,temp3,*temp_out;

  x_start=1;
  for (i=0;i<x_long;i++)
  {
    if(x_in[i]!=0) { x_start=i; break; }
  }
  x_end=x_long-1;
  for (i=x_long-1;i>=0;i--)
  {
    if (x_in[i]!=0) { x_end=i; break; }
  }

  long0 =x_end-x_start+1;
  long1 = long0 -1;

  x = (double *) calloc(long0,sizeof(double));
  for (i=0;i<long0;i++) x[i] = x_in[i+x_start];
  sum = (double *) calloc(long1,sizeof(double));
  for (k=0;k<long1;k++)
  {
    sum[k]=0;
    A = (double *)calloc(long0-k,sizeof(double));
    C = (double *)calloc(long0-k,sizeof(double));
    for (j=k;j<long0;j++) A[j-k]=x[j];
    for (j=0;j<long0-k;j++) C[j]=x[j]; 
    temp1=0.0; temp2=0.0; temp3=0.0;
    for (j=0;j<long0-k;j++)
    {
      temp1 += A[j]*C[j];
      temp2 += A[j]*A[j];
      temp3 += C[j]*C[j];
    }
    if (temp2==0 || temp3==0) sum[k]=0;
    else  sum[k] = temp1/(sqrt(temp2)*sqrt(temp3));
    free(A); free(C);
  }

  temp_out = (double *)calloc(2*long1-1,sizeof(double));
  for (i=0;i<long1-1;i++) temp_out[i] = sum[long1-i-1];
  for (i=long1-1;i<2*long1-1;i++) temp_out[i] = sum[i-long1+1];

  cut = round((float)((long1-1)*0.75));
  *y = (double *) calloc(2*cut+1,sizeof(double)); 
  for (i=0;i<cut;i++) (*y)[i]= temp_out[long1-1-cut+i];
  for (i=0;i<cut;i++) (*y)[i+cut+1] = temp_out[long1+i];
  (*y)[cut] = temp_out[long1-1];

  free(temp_out);
  free(sum);
  free(x);

  return 2*cut+1;
}

// VS2013-MODS
//-----------------------------------------------------------------------------
#if (_MSC_VER < 1800)
int round(float x)
{
  int i;
  float y;

  y=(float)floor(x);
  if ((x-y)>=0.5) x = (float)ceil(x);
  else x = (float)floor(x);
  i = (int) x;
  return i;
}
#endif
//-----------------------------------------------------------------------------
float tb_distance(float *a,float *b,int dim)
{
  int i;
  float dist=0.0;

  for (i=0;i<dim;i++) dist+=sqr(a[i]-b[i]);
  dist = (float)sqrt(dist);
  return dist;
}

//-----------------------------------------------------------------------------
void piksrtintS2B(int n, int *num, int *index)
{
  int i,j;
  int indextmp;
  int numtmp;

  for (i=0;i<n;i++) index[i]= i;
  for (j=1;j<n;j++)
  {
    numtmp=num[j];
    indextmp=index[j];
    i=j-1;
    while (i>=0 && num[i]>numtmp)
    {
      num[i+1]=num[i];
      index[i+1]=index[i];
      i--;
    }
    num[i+1]=numtmp;
    index[i+1]=indextmp;
  }
}

//-----------------------------------------------------------------------------
float **tb_fmatrix(int nr, int nc)
{
  int i,j;
  float **m;

  m=(float **) malloc(nr*sizeof(float *));
  if (!m) return NULL;
  for(i=0;i<nr;i++)
  {
    m[i]=(float *) malloc(nc*sizeof(float));
    if (!m[i]) return NULL;
    for (j=0;j<nc;j++) m[i][j]=0;
  }
  return m;
}

//-----------------------------------------------------------------------------
void tb_free_fmatrix(float **m, int nr)
{
  int i;

  for (i=0;i<nr;i++) free(m[i]);
  free(m);
}

//-----------------------------------------------------------------------------
int **imatrix(int nr, int nc)
{
  int i,j;
  int **m;

  m=(int **) malloc(nr*sizeof(int *));
  if (!m) return NULL;
  for(i=0;i<nr;i++)
  {
    m[i]=(int *) malloc(nc*sizeof(int));
    if (!m[i]) return NULL;
    for (j=0;j<nc;j++) m[i][j]=0;
  }
  return m;
}

//-----------------------------------------------------------------------------
void free_imatrix(int **m, int nr)
{
  int i;

  for (i=0;i<nr;i++) free(m[i]);
  free(m);
}

// end of pbc.c

//-----------------------------------------------------------------------------
// threshold.c
void threshold( struct pbc_struct *pbc ) {

	pbc->tstructuredness = 0;

	if( (pbc->structuredness > 0) && (pbc->structuredness <= BOUNDARY1) ) {
		pbc->tstructuredness = 1;
	}

	else if( (pbc->structuredness > BOUNDARY1) && (pbc->structuredness <= BOUNDARY2) ) {
		pbc->tstructuredness = 2;
	}

	else if( (pbc->structuredness > BOUNDARY2) && (pbc->structuredness <= BOUNDARY3) ) {
		pbc->tstructuredness = 3;
	}

	else if( pbc->structuredness > BOUNDARY3 ) {
		pbc->tstructuredness = 4;
	}

}

// endof threshold.c

//-----------------------------------------------------------------------------
// GaborFeature
int GaborFeature(Matrix *img, int side, double Ul, double Uh, int scale, int orientation, int flag,int *pbc)
{
//	FILE *outim;
	char  str[200];
	unsigned char dummy2;
	FILE *fp;
#ifdef WRITE_FILTERED
	FILE *FilteredImage;
#endif
	int i, h, w, wid, hei, imgwid, imghei, s, n, base;
	int border,r1,r2,r3,r4;
	double dummy1;
	double sum_double;

	double **rowProjections, **columnProjections;

	double  duration;
	double  xshift=0,yshift=0;
	double	his_mean,his_std,his_threshold;
	float	**histo;
	int		*d_direction;

	char	OutBuffer[]="RotatedImage";	
	double  *temp_proj;
	double  angle[2];
	float   *temp_histo;
	int		count_histo;
	double 	fmin,fmax;
	clock_t start, finish; 

	Matrix *IMG, *IMG_imag, *Gr, *Gi, *Tmp_1, *Tmp_2, *F_1, *F_2,*G_real, *G_imag, 
		*F_real, *F_imag, *temp_FilteredImage, *temp1,*temp2;
	
	Matrix *FilteredImageBuffer[4][6];
	
	int ProjectionSize;
	
	
	imghei = img->height;
	imgwid = img->width;
	border=side;

	ProjectionSize=(int)(imgwid*0.625);

	hei = (int) pow(2.0, ceil(log2((double)(img->height+2.0*border))));
    wid = (int) pow(2.0, ceil(log2((double)(img->width+2.0*border))));

//	printf("%d\t%d (Real Size)\n",imghei,imgwid);
//	printf("%d\t%d (Extended Size)\n",hei,wid);

	rowProjections = (double **) malloc(orientation*scale*sizeof(double *));
	for(i=0;i<orientation*scale;i++)
		rowProjections[i]=(double *)malloc(ProjectionSize*sizeof(double));

	columnProjections = (double **) malloc(orientation*scale*sizeof(double *));
	for(i=0;i<orientation*scale;i++)
		columnProjections[i]=(double *)malloc(ProjectionSize*sizeof(double));

// BEGIN: Allocations for all the ImageHeaders, imageData and Matrices. Newly created images are all initialized to 0 by default
    CreateMatrix(&IMG, hei,wid);

    r1=img->width+border;
	r2 = img->width+2*border;
	for (h=0;h<border;h++) {
	 for (w=0;w<border;w++)
	   IMG->data[h][w] = img->data[border-1-h][border-1-w];
	 for (w=border;w<r1;w++)
	  IMG->data[h][w] = img->data[border-1-h][w-border];
	 for (w=r1;w<r2;w++)
	  IMG->data[h][w] = img->data[border-1-h][2*img->width-w+border-1];
    }
	
	r1 = img->height+border;
    r2 = img->width+border;
	r3 = img->width+2*border;
	for (h=border;h<r1;h++) {
		for (w=0;w<border;w++)
			IMG->data[h][w] = img->data[h-border][border-1-w];
		for (w=border;w<r2;w++)
			IMG->data[h][w] = img->data[h-border][w-border];
		for (w=r2;w<r3;w++)
			IMG->data[h][w] = img->data[h-border][2*img->width-w+border-1];
	}
	
	r1 = img->height+border;
	r2 = img->height+2*border;
	r3 = img->width+border;
	r4 = img->width+2*border;
	for (h=r1;h<r2;h++) {
		for (w=0;w<border;w++)
			IMG->data[h][w] = img->data[2*img->height-h+border-1][border-1-w];
		for (w=border;w<r3;w++)
		 	IMG->data[h][w] = img->data[2*img->height-h+border-1][w-border];
		for (w=r3;w<r4;w++)
			IMG->data[h][w] = img->data[2*img->height-h+border-1][2*img->width-w+border-1];
	}


 
// Allocate memory
 
	CreateMatrix(&F_real, hei, wid);
    CreateMatrix(&F_imag, hei, wid);
    CreateMatrix(&IMG_imag, hei, wid);
    CreateMatrix(&Gr, 2*side+1, 2*side+1);
    CreateMatrix(&Gi, 2*side+1, 2*side+1);
    CreateMatrix(&Tmp_1, hei, wid);
    CreateMatrix(&Tmp_2, hei, wid);
    CreateMatrix(&F_1, hei, wid);
    CreateMatrix(&F_2, hei, wid);
    CreateMatrix(&G_real, hei, wid);
    CreateMatrix(&G_imag, hei, wid);

	CreateMatrix(&temp_FilteredImage,imghei,imgwid);

	histo=(float **)malloc(scale*sizeof(float *));
	for(s=0;s<scale;s++)
		histo[s]=(float *)malloc(orientation*sizeof(float));

	for(s=0;s<scale;s++)
		for(n=0;n<orientation;n++)
			histo[s][n]=0.0;


	for(s=0;s<4;s++)
		for(n=0;n<6;n++)
		{
			CreateMatrix(&FilteredImageBuffer[s][n],imghei,imgwid);
		}

//	END: of Allocations
	

	/* ----------- compute the Gabor filtered output ------------- */

	base = scale*orientation;

	start=clock();
	
 
    Mat_FFT2(F_real, F_imag, IMG, IMG_imag);

    //printf("\n Gabor Filtering the image...\n");	
// compute filtered images
	for (s=0;s<scale;s++) {
		for (n=0;n<orientation;n++) {
	
		  //printf("s=%d n=%d\n",s,n);
			Gabor(Gr, Gi, s+1, n+1, Ul, Uh, scale, orientation, flag);

            Mat_Copy(F_1, Gr, 0, 0, 0, 0, 2*side, 2*side);
            Mat_Copy(F_2, Gi, 0, 0, 0, 0, 2*side, 2*side);
            Mat_FFT2(G_real, G_imag, F_1, F_2);

 
            Mat_Product(Tmp_1, G_real, F_real);
            Mat_Product(Tmp_2, G_imag, F_imag);
            Mat_Substract(IMG, Tmp_1, Tmp_2);
 
            Mat_Product(Tmp_1, G_real, F_imag);
            Mat_Product(Tmp_2, G_imag, F_real);
            Mat_Sum(IMG_imag, Tmp_1, Tmp_2);
 
            Mat_IFFT2(Tmp_1, Tmp_2, IMG, IMG_imag);
                   
            CreateMatrix(&temp1,imghei,imgwid);
			CreateMatrix(&temp2,imghei,imgwid);

			Mat_Copy(temp1,Tmp_1,0,0,2*side,2*side,imghei+2*side-1,imgwid+2*side-1);
			Mat_Copy(temp2,Tmp_2,0,0,2*side,2*side,imghei+2*side-1,imgwid+2*side-1);
            
			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=sqrt(temp1->data[h][w]*temp1->data[h][w]+
					temp2->data[h][w]*temp2->data[h][w]);
					FilteredImageBuffer[s][n]->data[h][w]=dummy1;
				}


			FreeMatrix(temp1);
			FreeMatrix(temp2);


			sprintf(str,"IRow_%d%d",s,n);
#ifdef WRITE_FILTERED

			if ((FilteredImage = fopen(str,"wb")) == NULL) {
				printf("%s can not be openned!\n", str);
				exit(0);
			}
	
			for (h=0;h<imghei;h++){
				for (w=0;w<imgwid;w++){
					iplGetPixel(OutShift,w,h,&dummy1);
					fwrite(&dummy1,sizeof(float), 1, FilteredImage);
				}
			}
			fclose(FilteredImage);
#endif			
			
	
		}
	}


	// start to detect dominant directions
	printf("Dominant Orientation Estimation\n");
	for(s=0;s<scale;s++)
	{	
		sum_double=0.0;
		for(n=0;n<orientation;n++)
		{
			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=FilteredImageBuffer[s][n]->data[h][w];
					sum_double+=dummy1;
				}
		}
		
		his_mean=sum_double/(orientation*imghei*imgwid);


		sum_double=0.0;
		for(n=0;n<orientation;n++)
		{
			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=FilteredImageBuffer[s][n]->data[h][w];
					sum_double+=pow((dummy1-his_mean),2);
				}
		}
		
		his_std=sqrt(sum_double/(orientation*imghei*imgwid));
		his_threshold=his_mean+his_std;

		for(n=0;n<orientation;n++)
		{
			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=FilteredImageBuffer[s][n]->data[h][w];
					
					if(dummy1>his_threshold) histo[s][n]++;
				}
		}

					
	}

	if(DEBUG_DIRECTION)
	{
		for(s=0;s<scale;s++)
		{
			for(n=0;n<orientation;n++)
				printf("%f ",histo[s][n]);
			printf("\n");
		}

		fp=fopen("Histogram.data","wb");
		temp_histo=(float *)malloc(scale*orientation*sizeof(float));
		count_histo=0;
		for(s=0;s<scale;s++)
			for(n=0;n<orientation;n++)
			{
				temp_histo[count_histo]=histo[s][n];
				count_histo++;
			}
		fwrite(temp_histo,sizeof(float),scale*orientation,fp);
		fclose(fp);
		free(temp_histo);
	}


	d_direction=(int *)malloc(D_DirectionSize*sizeof(int));
	d_direction=DominantDirection(histo);

	
	if(d_direction[0]==6 && d_direction[1]==6)
	{
		angle[0]=0; angle[1]=90;
		pbc[0]=0; pbc[1]=0;
	}
	else if(d_direction[0]!=6 && d_direction[1]==6)
	{
		angle[0]=d_direction[0]*30; angle[1]=angle[0]+90;
		if(angle[1]>180) angle[1]=angle[1]-180;
		pbc[0]=d_direction[0]+1; 
		if(pbc[0]>=4) pbc[1]=0;
		else pbc[1]=0;
	}
	else
	{
		angle[0]=d_direction[0]*30; angle[1]=angle[0]+90;
		if(angle[1]>180) angle[1]=angle[1]-180;
		pbc[0]=d_direction[0]+1; pbc[1]=d_direction[1]+1;
	}

	
	free(d_direction);
	for(s=0;s<D_DirectionSize;s++)
	 printf("dominant direction is %d\n",pbc[s]);
	//printf("angle1=%3.0f \t angle2=%3.0f\n\n",angle[0],angle[1]);
	


	// compute projections of rotated filtered images
	for(s=0;s<scale;s++)
		for(n=0;n<orientation;n++)
		{
			fmin=FilteredImageBuffer[s][n]->data[0][0]; fmax=fmin;
			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					if(FilteredImageBuffer[s][n]->data[h][w]>fmax) fmax=FilteredImageBuffer[s][n]->data[h][w];
					if(FilteredImageBuffer[s][n]->data[h][w]<fmin) fmin=FilteredImageBuffer[s][n]->data[h][w];
				}



			//printf("min=%f max=%f\n",fmin,fmax);

			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=FilteredImageBuffer[s][n]->data[h][w];
					dummy2=(unsigned char)((dummy1-fmin)/(fmax-fmin)*255);
				
					temp_FilteredImage->data[h][w]=dummy2;
					
				}
				
			ImgRotate(temp_FilteredImage,(float)angle[0]);

			temp_proj=(double *)malloc(ProjectionSize*sizeof(double));
			ComputeProjection(temp_FilteredImage,imgwid,imghei,angle[0],ProjectionSize,temp_proj);
		

			for(h=0;h<ProjectionSize;h++)
				rowProjections[s*orientation+n][h] = temp_proj[h];

				
			sprintf(str,"default_Row_%d%d",s,n);
			if ((fp = fopen(str,"wb")) == NULL) {
				printf("%s can not be openned!\n", str);
				exit(0);
			}
			fwrite(rowProjections[s*orientation+n],sizeof(double),ProjectionSize, fp );
			fclose(fp);

			for(h=0;h<imghei;h++)
				for(w=0;w<imgwid;w++)
				{
					dummy1=FilteredImageBuffer[s][n]->data[h][w];
					dummy2=(unsigned char)((dummy1-fmin)/(fmax-fmin)*255);
				
					temp_FilteredImage->data[h][w]=dummy2;
					
				}

			ImgRotate(temp_FilteredImage,(float)angle[1]);
				
			ComputeProjection(temp_FilteredImage,imgwid,imghei,angle[1],ProjectionSize,temp_proj);
			
			for(h=0;h<ProjectionSize;h++)
				columnProjections[s*orientation+n][h] = temp_proj[h];

			free(temp_proj);

			sprintf(str,"default_Column_%d%d",s,n);
			if ((fp = fopen(str,"wb")) == NULL) {
				printf("%s can not be openned!\n", str);
				exit(0);
			}
			fwrite(columnProjections[s*orientation+n],sizeof(double),ProjectionSize, fp );
			fclose(fp);



		}


//CHANGED	
	
	finish=clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	printf( "\n%f seconds\n\n", duration );

			
	for(i=0;i<orientation*scale;i++)
		free(rowProjections[i]);
	free(rowProjections);
	

	for(i=0;i<orientation*scale;i++)
		free(columnProjections[i]);
	free(columnProjections);


	for(s=0;s<scale;s++)
		free(histo[s]);
	free(histo);

	
	for(s=0;s<4;s++)
		for(n=0;n<6;n++)
		{
			FreeMatrix(FilteredImageBuffer[s][n]);
		}

    FreeMatrix(Gr);
    FreeMatrix(Gi);
    FreeMatrix(Tmp_1);
    FreeMatrix(Tmp_2);
    FreeMatrix(F_1);
    FreeMatrix(F_2);
    FreeMatrix(G_real);
    FreeMatrix(G_imag);
    FreeMatrix(F_real);
    FreeMatrix(F_imag);
    FreeMatrix(IMG);
    FreeMatrix(IMG_imag);
	

	return 1;
}

//-----------------------------------------------------------------------------
void ComputeProjection(Matrix *inputImage,int xsize,int ysize,double angle,int proj_size,double *proj)
{

	int xcenter,ycenter;
	int j,l,count_pixel;
	double sum_pixel;
	unsigned char dummy;

	xcenter=xsize/2;
	ycenter=ysize/2;

	// KW 2013/07/24 - set the values to 0
	for(j=0; j < proj_size; j++) proj[j] = 0.0;
	int proj_start = xcenter-proj_size/2;
	int proj_stop = proj_start + proj_size;

	//for(j=(xcenter-proj_size/2);j<(xcenter+proj_size/2);j++)
	for(j=proj_start;j<proj_stop;j++)
	{
		count_pixel=0;
		sum_pixel=0.0;
		for(l=0;l<ysize;l++)
		{
			dummy=0;
			if(sqrt(pow((double)j-xcenter,2)+pow((double)l-ycenter,2))<=127.0)
			{
				// probably wrong
				dummy=(unsigned char)inputImage->data[j][l];
				sum_pixel+=(double)dummy;
				count_pixel++;
			}
		}
		// KW 2013/07/24 fix divide by zero
		if(count_pixel > 0)
			proj[j-proj_start]=sum_pixel/count_pixel;
			//proj[j-(xcenter-proj_size/2)]=sum_pixel/count_pixel;

	}
	
}

//-----------------------------------------------------------------------------
// PBC_Extraction.c
void PBC_Extraction(Matrix *img,int width,int height, int *pbc_out)
{
	int *temp_direction;
	struct pbc_struct pbc;
	int imgsize;

	temp_direction=(int *)malloc(2*sizeof(int));
	GaborFeature(img, XM_SIDE, XM_UL, XM_UH, XM_SCALE, XM_ORIENTATION, XM_FLAG,temp_direction);
	//printf("Extracted Texture Browsing Feature ... ");

	pbc.element[1]=temp_direction[0];
	pbc.element[3]=temp_direction[1];

	free(temp_direction);

	imgsize=width;
	pbcmain( &pbc,imgsize );
	threshold( &pbc );

	pbc_out[0]=pbc.tstructuredness;
	pbc_out[2]=pbc.element[0];
//	pbc_out[2]=pbc.element[1];
	pbc_out[4]=pbc.element[2];
//	pbc_out[3]=pbc.element[2];
	pbc_out[1]=pbc.element[1];
	pbc_out[3]=pbc.element[3];
	
	
}

//-----------------------------------------------------------------------------
// R channel to Matrix
void Convert2Matrix(unsigned char *R, int width,int height, Matrix *image)
{
	int i,j;
	int count;

	count=0;
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			image->data[i][j]=R[count];
			count++;
		}

}

//-----------------------------------------------------------------------------
double distance_PBC(int flag, int *Ref, int* Query)
{
	double dis, temp1, temp2;
    temp1 = abs(Ref[1]-Query[1]);
	temp1 = (temp1>3)? (6-temp1):temp1;
	dis=0.6*abs(Ref[0]-Query[0])+0.2*(0.5*(temp1+abs(Ref[2]-Query[2])));
		
	if (flag != 0) {
		temp2 = abs(Ref[3]-Query[3]);
		temp2 = (temp2>3)? (6-temp2):temp2;
		dis = dis + 0.2*(0.5*(temp2+abs(Ref[4]-Query[4])));
	}

	return dis;
}

//------------------------------------------------------------------------------------
//Arbitrary Shape routine//

void min_test(int a, int& min) {
	if (a<min) min = a;
}

//-----------------------------------------------------------------------------
bool max_test(int a, int& max) {
	if (a>max) {
		max = a;
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------------
void ArbitraryShape(unsigned char *a_image, unsigned char *y_image, int image_height, int image_width) {	

	int i, j, x, y;
	int flag, a_min, a_max;
	int center_x, center_y;
	int a_size, pad_height_count, pad_width_count;
	int **m_arbitraryShape, **m_arbitraryShape_temp;
	unsigned char **m_arbitraryShape_patch;

	m_arbitraryShape = (int **)calloc(image_height+2, sizeof(int *));
	m_arbitraryShape_temp = (int **)calloc(image_height+2, sizeof(int *));
	m_arbitraryShape_patch = (unsigned char **)calloc(image_height, sizeof(unsigned char *));

	for(i=0;i<image_height+2;i++) {
		m_arbitraryShape[i] = (int *)calloc(image_width+2, sizeof(int));
		m_arbitraryShape_temp[i] = (int *)calloc(image_width+2, sizeof(int));
	}
	
	for(i=0;i<image_height;i++)
		m_arbitraryShape_patch[i] = (unsigned char *)calloc(image_width, sizeof(unsigned char));

	for(i=1;i<image_height+1;i++) {
		for(j=1;j<image_width+1;j++) {
			if(a_image[(i-1)*image_width+(j-1)]!=(unsigned char)255) { // the pixel is white
				m_arbitraryShape[i][j]= 1;
				m_arbitraryShape_temp[i][j]= 1;
			}
		}
	}

	flag=1;
	a_max=0;

	while(flag) {
		flag=0;
		for(i=0;i<image_height+2;i++) {
			for(j=0;j<image_width+2;j++) {
				m_arbitraryShape[i][j] = m_arbitraryShape_temp[i][j];
				if (max_test(m_arbitraryShape[i][j],a_max)) {
					center_y = i-1;
					center_x = j-1;
				}
			}
		}
		
		for(i=1;i<image_height+1;i++) {
			for(j=1;j<image_width+1;j++) {
				if (m_arbitraryShape[i][j] != 0) {
					a_min = m_arbitraryShape[i][j];
					min_test(m_arbitraryShape[i-1][j],a_min);
					min_test(m_arbitraryShape[i+1][j],a_min);
					min_test(m_arbitraryShape[i][j-1],a_min);
					min_test(m_arbitraryShape[i+1][j-1],a_min);
					min_test(m_arbitraryShape[i-1][j-1],a_min);			
					min_test(m_arbitraryShape[i][j+1],a_min);
					min_test(m_arbitraryShape[i+1][j+1],a_min);
					min_test(m_arbitraryShape[i-1][j+1],a_min);
	
					if(m_arbitraryShape[i][j]==a_min) {
						m_arbitraryShape_temp[i][j] = m_arbitraryShape[i][j]+1;
						flag = 1;
					}
					else
						m_arbitraryShape_temp[i][j]=m_arbitraryShape[i][j];
				}
			}
		}

	}	//while(flag ==1)

	a_size= a_max-1;
    pad_height_count = (int)( ((float)image_height) / (2.0*((float)a_size)) ) + 1;
	pad_width_count = (int)( ((float)image_width) / (2.0*((float)a_size)) ) + 1;
	
	for(y=0;y<pad_height_count;y++) {
		for(x=0;x<pad_width_count;x++) {
			for(i=0;i<2*a_size+1;i++) {
				for(j=0;j<2*a_size+1;j++) {
					if( ((i+y*(2*a_size+1))<image_height) && ((j+x*(2*a_size+1))<image_width) )
						m_arbitraryShape_patch[i+y*(2*a_size+1)][j+x*(2*a_size+1)] = y_image[(center_y-a_size+i)*image_width + center_x-a_size+j];
				}
			}
		}
	}

	for(i=0;i<image_height;i++)
		for(j=0;j<image_width;j++)
			y_image[i*image_width+j]=m_arbitraryShape_patch[i][j];

	free(m_arbitraryShape);
	free(m_arbitraryShape_temp);
	free(m_arbitraryShape_patch);

}
