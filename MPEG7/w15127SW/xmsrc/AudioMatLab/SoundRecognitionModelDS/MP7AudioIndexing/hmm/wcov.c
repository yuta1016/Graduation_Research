/* 
*/

/* Matlab V4 types should be changed as follows:
  Matrix -> mxArray
  REAL -> mxREAL
  mxCreateFull ->  mxCreateDoubleMatrix
  */

#undef DEBUG

#define V4_COMPAT
#include <matrix.h>  /* Matlab matrices */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <alloca.h>
#include <stdlib.h>

#define notDblMtx(it) (!mxIsNumeric(it) || !mxIsDouble(it) || mxIsSparse(it) || mxIsComplex(it))

/* how matlab does matrix indices */
#define INDEX(base,row,col,nrows) ((base)+(row)-1+((col)-1)*(nrows))
#define REALMAX DBL_MAX
#define REALMIN DBL_MIN

void mprint(double *m,int rows,int nrows,int ncols) {
  while (nrows-->0) {
    double *p=m; int c=ncols;
    while (c-->0) {
      fprintf(stderr," % 7g",*p); p+=rows;
    }
    fprintf(stderr, "\n");
    m++;
  }
  fprintf(stderr, "\n");
}

void wcov(double *X,	/* data */
	  double *W,	/* weights */
	  double *M,	/* mean */
	  int n,	/* # points */
	  int d,	/* # dimensions */
	  int flag,	/* whether or not to normalize */
	  double *covariance, /* where to put covariance result */
	  double *mean /* where to put mean */
	  )	
{
  int i,j,k;
  register double *p;
  double totalweight=0;
  double *buffer;
  int nonzeromean=0;
#ifndef __ALLOCA_H
  int used_calloc=0;
#endif
  if (!mean) {
#ifdef DEBUG
    fprintf(stderr,"allocating mean\n");
#endif
#ifdef __ALLOCA_H
    mean=(double*)alloca(d*sizeof(double));
#else
    used_calloc=1;
    mean=(double*)calloc(d,sizeof(double));
#endif
  }
#ifdef __ALLOCA_H
    buffer=(double*)alloca(d*sizeof(double));
#else
    buffer=(double*)calloc(d,sizeof(double));
#endif

  if (W) {
    p=W;
    for (j=n;j>0;j--) totalweight+=*p++;
  } else totalweight = n;
#ifdef DEBUG
  fprintf(stderr," totalweight=%g\n",totalweight);
#endif
  
  if (M) {
    register double *dst=mean;
    register double *src=M;
    int i=d;
    while (i-->0) { nonzeromean|=(*src!=0.0); *dst++=*src++;}
  } else {	
    register double *m=mean;
    p=X;
    if (W) {			/* compute weighted sample mean  */
      for (i=d;i>0;i--) {
	register double *p1=W;
	register double sum=0.0;
	for (j=n;j>0;j--) sum+=*p++**p1++;
	nonzeromean|=(sum!=0.0);
	*m++=sum/totalweight;
      }
    } else {			/* compute unweighted sample Mean */
#ifdef DEBUG
    fprintf(stderr,"calculating mean\n");
#endif
      for (i=d;i>0;i--) {
	register double sum=0.0;
	for (j=n;j>0;j--) sum+=*p++;
	nonzeromean|=(sum!=0.0);
	*m++=sum/totalweight;
      }
    }
  }
#ifdef DEBUG
  fprintf(stderr,"mean:");mprint(mean,1,1,d);
  if (nonzeromean) fprintf(stderr,"nonzero mean\n");
#endif

  /* accumulate covariances in the lower diagonal */
  for (i=n;i>0;i--) {		/* for each row */
    register double *p1=buffer;
    register double *p2=mean;
    register double weight;
    if (W) weight=*W++; 
#ifdef DEBUG3
    if (W) fprintf(stderr,"weight: %g ", weight);
#endif
    p=X++;
    if ((0==W)||(0.0<weight)) {
      if (nonzeromean)
	for (j=d;j>0;j--,p+=n)	/* subtract mean from data point */
	  *p1++=*p-*p2++;
      else
	for (j=d;j>0;j--,p+=n)	/* copy data point into buffer */
	  *p1++=*p;
#ifdef DEBUG3
      mprint(buffer,1,1,d);
#endif
      p1=buffer;
      p=covariance;
      for (j=d;j>0;j--) {		/* for each element */
	register double factor;
	p2=p1;
	if (W) { factor=*p1++ * weight; } else { factor=*p1++; }
	for (k=j;k>0;k--) 	/* for each subsequent element */
	  *p+++=*p2++ * factor;
	p+=(d-j+1);
      }
#ifdef DEBUG
      mprint(covariance,d,d,d);
#endif
    }
  }
  
#ifdef DEBUG
  fprintf(stderr,"flag=%d  \n", flag);
#endif
  /* now copy the lower diagonal into the upper diagonal */
  p=covariance;
  if (!flag) {			/* with normalization */
    double norm=1.0/((double)totalweight-1.0);
    for (j=d;j>0;j--) {
      register double *dst=p;
#ifdef DEBUG2
      fprintf(stderr, "%g;", *p*norm);
#endif
      *p++*=norm;
      dst+=d;
      for (k=j-1;k>0;k--) {
	register double tmp=*p*norm;
#ifdef DEBUG3
	fprintf(stderr, "%g;", tmp);
#endif
	*p++=tmp;
	*dst=tmp;
	dst+=d;
      }
      p+=(d-j+1);
#ifdef DEBUG2
      fprintf(stderr,"(%d)",p-covariance);
#endif
    }
  } else {			/* without normalization */
    for (j=d;j>0;j--) {
      register double *dst=p;
#ifdef DEBUG
      fprintf(stderr,":%g ", *p);
#endif
      p++;
      dst+=d;
      for (k=j-1;k>0;k--) {
#ifdef DEBUG3
	fprintf(stderr,":%g ", *p);
#endif
	*dst=*p++;
	dst+=d;
      }
      p+=(d-j+1);
#ifdef DEBUG2
      fprintf(stderr,"(%d)",p-covariance);
#endif
    }
  }
#ifndef __ALLOCA_H
  free(buffer);
  if (used_calloc) {
#ifdef DEBUG
    fprintf(stderr,"freeing mean\n");
#endif
    free (mean);
  }
#endif
}

void mexFunction(int nlhs,	     /* Num return vals on lhs */
		 Matrix *plhs[],    /* Matrices on lhs      */
		 int nrhs,	     /* Num args on rhs    */
		 Matrix *prhs[]     /* Matrices on rhs */
		 )
{
  Matrix *arg;
  double *X=0,*Weights=0,*Mean=0,*Covariance=0,*Mean2=0;
  int n,d,flag;
  
  if (nrhs < 1 ) mexErrMsgTxt("requires at least 1 input arguments.");
  
  /* ARG 1: MATRIX  */
  arg = prhs[0];
  if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
  X = mxGetPr(arg);
  n = (int) mxGetM(arg);
  d = (int) mxGetN(arg);
  if (n*d<1)
    mexErrMsgTxt("1st arg must be TxD real non-sparse matrix.");
  if (n==1) { n=d;d=1; }
#ifdef DEBUG
  fprintf(stderr,"arg1: %dx%d matrix  ",n,d);
#endif

  /* OPTIONAL ARG 2: VECTOR */
  if (nrhs>1) {
    int tmp;
    arg = prhs[1];
    if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
    Weights = mxGetPr(arg);
#ifdef DEBUG
    fprintf(stderr,"arg2: %dx%d matrix  ",(int) mxGetN(arg),(int) mxGetM(arg));
#endif
    tmp=(int) mxGetN(arg)*(int) mxGetM(arg);
    if (n!=tmp&&tmp!=0)
      mexErrMsgTxt("optional 2nd arg must be Tx1 vector or [].");
  }

  /* OPTIONAL ARG 3: VECTOR */
  if (nrhs>2) {
    int tmp;
    arg = prhs[2];
    if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
    Mean = mxGetPr(arg);
#ifdef DEBUG
    fprintf(stderr,"arg3: %dx%d matrix  ",(int) mxGetN(arg),(int) mxGetM(arg));
#endif
    tmp=(int) mxGetN(arg)*(int) mxGetM(arg);
    if (d!=tmp&&tmp!=0)
      mexErrMsgTxt("optional 3rd arg must be 1xN vector or [].");
  }

  /* OPTIONAL ARG 4: SCALAR */
  if (nrhs>3) {
    arg = prhs[3];
    if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
    flag = (int)*(mxGetPr(arg));
#ifdef DEBUG
    fprintf(stderr,"arg4: %dx%d matrix  ",(int) mxGetN(arg),(int) mxGetM(arg));
#endif
    if ((1!=(int) mxGetN(arg))||(1!=(int) mxGetM(arg)))
      mexErrMsgTxt("optional 4th arg must be a flag (boolean).");
  } else flag=0;

  /* OUT 1: covariance */
  plhs[0]=mxCreateDoubleMatrix(d,d,mxREAL);
  Covariance=mxGetPr(plhs[0]);

  /* OPTIONAL OUT 2: mean */
  if (nlhs>1) {
    plhs[1]=mxCreateDoubleMatrix(1,d,mxREAL);
    Mean2=mxGetPr(plhs[1]);
  }

  wcov(X,Weights,Mean,n,d,flag,Covariance,Mean2);
}

