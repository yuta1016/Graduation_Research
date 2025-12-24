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
#include <strings.h>
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

void mahalonobis(double *X,		/* data */
	 double *I,		/* square symmetric matrix */
	 double *M,		/* mean */
	 int n,			/* # points */
	 int d,			/* # dimensions */
	 double *res		/* where to put result X*I*X'  */
	  )	
{
  int i,j,k;
  register double *p;
  int tri=((d*(d+1))/2);
  double totalweight=0;
  double *buffer,*mbuffer;
  int nonzeromean=0;
#ifndef __ALLOCA_H
  int used_calloc=0;
#endif
#ifdef __ALLOCA_H
  buffer=(double*)alloca(tri*sizeof(double));
  mbuffer=(double*)alloca(d*sizeof(double));
#else
  buffer=(double*)calloc(tri,sizeof(double));
  mbuffer=(double*)calloc(d,sizeof(double));
#endif

  p=buffer;
  /* copy diagonals into buffer */
  for (j=d;j>0;j--) {
    *p++=*I; I+=d+1;
  }
  I-=2*tri;
  /* copy lower triangle into buffer */
  for (j=d-1;j>0;j--) {
    I+=d-j;
    for (k=j;k>0;k--) 
      *p++=*I++;
  }
  /* mprint(buffer,1,1,tri); */
  /* for each point */
  for (i=n;i>0;i--) {
    register double sum=0.0, dsum=0.0, *ipd=buffer, *ip=buffer+d;
    p=X++;
    if (M) {
      register double *p2=mbuffer,*p3=M;
      for (j=d;j>0;j--) { *p2++=*p-(*p3++);p+=n; }
    } else {
      register double *p2=mbuffer;
      for (j=d;j>0;j--) { *p2++=*p; p+=n; }
    }
    p=mbuffer;
    for (j=d;j>0;j--) {
      register double isum=0.0, pval=*p++, *p2=p;
      for (k=j-1;k>0;k--) 
	isum+=(*p2++)*(*ip++);
      dsum+=pval*pval*(*ipd++);
      sum+=isum*pval;
      /* fprintf(stderr, "%g %g\n",pval,isum); */
    }
    /* fprintf(stderr, "%g %g\n",dsum,sum); */
    *res++=sum+sum+dsum;
  }    
#ifndef __ALLOCA_H
  free(buffer); free(mbuffer);
#endif
}

void mexFunction(int nlhs,	     /* Num return vals on lhs */
		 Matrix *plhs[],    /* Matrices on lhs      */
		 int nrhs,	     /* Num args on rhs    */
		 Matrix *prhs[]     /* Matrices on rhs */
		 )
{
  Matrix *arg;
  double *X=0,*Icov=0,*Mean=0,*Res=0;
  int n,d,flag;
  
  if (nrhs < 2 ) mexErrMsgTxt("requires at least 2 input arguments.");
  
  /* ARG 1: MATRIX  */
  arg = prhs[0];
  if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
  X = mxGetPr(arg);
  n = (int) mxGetM(arg);
  d = (int) mxGetN(arg);
  if (n*d<1)
    mexErrMsgTxt("1st arg must be TxD real non-sparse matrix.");
#ifdef DEBUG
  fprintf(stderr,"arg1: %dx%d matrix  ",n,d);
#endif

  /*  ARG 2: Square matrix */
  if (nrhs>1) {
    int tmp;
    arg = prhs[1];
    if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
    Icov = mxGetPr(arg);
#ifdef DEBUG
    fprintf(stderr,"arg2: %dx%d matrix  ",(int) mxGetN(arg),(int) mxGetM(arg));
#endif
    tmp=(int) mxGetN(arg)*(int) mxGetM(arg);
    if (d*d!=tmp)
      mexErrMsgTxt("2nd arg must be DxD symmetric matrix.");
  }

  /*  OPTIONAL ARG 3: Vector */
  if (nrhs>2) {
    int tmp;
    arg = prhs[2];
    if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
    Mean = mxGetPr(arg);
#ifdef DEBUG
    fprintf(stderr,"arg2: %dx%d matrix  ",(int) mxGetN(arg),(int) mxGetM(arg));
#endif
    tmp=(int) mxGetN(arg)*(int) mxGetM(arg);
    if (d!=tmp)
      mexErrMsgTxt("optional 3rd arg must be 1xD vector.");
  }

  /* OUT: result */
  plhs[0]=mxCreateDoubleMatrix(n,1,mxREAL);
  Res=mxGetPr(plhs[0]);

  mahalonobis(X,Icov,Mean,n,d,Res);
}

