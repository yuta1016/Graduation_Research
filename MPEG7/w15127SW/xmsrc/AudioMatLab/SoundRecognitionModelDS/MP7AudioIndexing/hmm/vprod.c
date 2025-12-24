/* 
function res=vprod(a,b,r,c);
*/

/* Matlab V4 types should be changed as follows:
  Matrix -> mxArray
  REAL -> mxREAL
  mxCreateFull ->  mxCreateDoubleMatrix
  */

#define V4_COMPAT
#include <matrix.h>  /* Matlab matrices */

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <strings.h>
#include <stdlib.h>

#define notDblMtx(it) (!mxIsNumeric(it) || !mxIsDouble(it) || mxIsSparse(it) || mxIsComplex(it))


void vprod(double *a,double *b,register double *r,int ar,int ac,int br,int bc, int rr, int rc) {
  int aac, bbc,i;
  if (ac*ar*bc*br==1) {		/* single value */
    int i=rc*rr; double v=(*a)*(*b); 
    while(i-->0) *r++=v;
    return;
  }
#pragma parallel shared(a,b,ar,br,rr,ac,bc) local(i,aac,bbc,r) if(rr>10000)
#pragma pfor iterate(i=rc;rc;-1)
  aac=0, bbc=0;
  for (i=rc;i>0;i--) {
    register int aar=0, bbr=0, rrr=rr; 
    register double *ap=a+aac*ar, *bp=b+bbc*br;
    if (1==ar) {
      if (1==br) {		/* both are flat */
	double v=(*ap++)*(*bp++);
	while (rrr-->0) *r++=v;
      } else {		/* just a is flat */
	double av=*ap++;
	while (rrr-->0) {	
	  *r++=av*(*bp++);
	  if (++bbr>=br) { bp-=br; bbr=0; }
	}
      }
    }  else if (1==br) {
      double bv=*bp++;	/* just b is flat */
      while (rrr-->0) {	
	*r++=(*ap++)*bv;
	if (++aar>=ar) { ap-=ar; aar=0; }
      }
    }  else {
      while (rrr-->0) {	/* neither is flat */
	*r++=(*ap++)*(*bp++);
	if (++aar>=ar) { ap-=ar; aar=0; }
	if (++bbr>=br) { bp-=br; bbr=0; }
      }
    }
    if (++aac>=ac) { aac=0; }
    if (++bbc>=bc) { bbc=0; }
  }
}
      
  
void mexFunction(int nlhs,	     /* Num return vals on lhs */
		 Matrix *plhs[],    /* Matrices on lhs      */
		 int nrhs,	     /* Num args on rhs    */
		 Matrix *prhs[]     /* Matrices on rhs */
		 )
{
  Matrix *arg;
  double *a, *b, *r;
  int ar,ac,br,bc,rr,rc;
  
  if (nrhs < 2 ) mexErrMsgTxt("vprod: requires at least 3 input arguments.");
  
  /* ARG 1: MATRIX  */
  arg = prhs[0];
  if notDblMtx(arg) mexErrMsgTxt("vprod: 1st arg must be a real non-sparse matrix.");
  a = mxGetPr(arg);
  ar = (int) mxGetM(arg);
  ac = (int) mxGetN(arg);
  if (0>=ar*ac)
    mexErrMsgTxt("vprod: 1st arg must at least be a real non-sparse vector.");

  /* ARG 2: MATRIX  */
  arg = prhs[1];
  if notDblMtx(arg) mexErrMsgTxt("vprod: 2nd arg must be a real non-sparse matrix.");
  b = mxGetPr(arg);
  br = (int) mxGetM(arg);
  bc = (int) mxGetN(arg);
  if (0>=br*bc)
    mexErrMsgTxt("vprod: 2nd arg must at least be a real non-sparse vector.");

  if (0!=((ar>br)?ar%br:br%ar))
    mexWarnMsgTxt("vprod: rows do not evenly divide.");
  if (0!=((ac>bc)?ac%bc:bc%ac))
    mexWarnMsgTxt("vprod: columns do not evenly divide.");

  /* ARG 3: rows */
  if (nrhs>2) {
    arg = prhs[2];
    if (notDblMtx(arg)||
	(mxGetM(arg) * mxGetN(arg) != 1))
      mexErrMsgTxt("vprod: optional 4th arg must be an integer.");
    rr=(int)*mxGetPr(arg);
    if (rr<1)
      mexErrMsgTxt("vprod: optional 3rd & 4th args must be positive integers.");
  } else 
    rr = (ar>br)?ar:br;

  /* ARG 4: cols */
  if (nrhs>3) {
    arg = prhs[3];
    if (notDblMtx(arg)||
	(mxGetM(arg) * mxGetN(arg) != 1))
      mexErrMsgTxt("vprod: optional 4th arg must be an integer.");
    rc=(int)*mxGetPr(arg);
    if (rc<1)
      mexErrMsgTxt("vprod: optional 3rd & 4th args must be positive integers.");
  } else
    rc = (ac>bc)?ac:bc;
  
  /* OUT 1: r */
  plhs[0]=mxCreateDoubleMatrix(rr,rc,mxREAL);
  r=mxGetPr(plhs[0]);

  /*fprintf(stderr,"%dx%d + %dx%d -> %dx%d\n",ar,ac,br,bc,rr,rc);*/
  vprod(a,b,r,ar,ac,br,bc,rr,rc);
  
}
