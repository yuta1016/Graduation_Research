/* 
function [likelihood,forward,backward,occupation]= ...
  fb_analysis(Transitions,Starts,P_obs_giv_state,timesteps,first);
*/

/* Matlab V4 types should be changed as follows:
  Matrix -> mxArray
  REAL -> mxREAL
  mxCreateFull ->  mxCreateDoubleMatrix
  */

#define IN_MATLAB
#ifdef IN_MATLAB
#define V4_COMPAT
#include <matrix.h>  /* Matlab matrices */
#endif

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <strings.h>
#include <alloca.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>

#define notDblMtx(it) (!mxIsNumeric(it) || !mxIsDouble(it) || mxIsSparse(it) || mxIsComplex(it))

/* how matlab does matrix indices */
#define INDEX(base,row,col,nrows) ((base)+(row)-1+((col)-1)*(nrows))
#define REALMAX DBL_MAX
#define REALMIN DBL_MIN

void mprint(double *m,int const rows,int nrows,int const ncols) {
  while (nrows-->0) {
    register double *p=m; int c=ncols;
    while (c-->0) {
      fprintf(stderr," % 7g",*p); p+=rows;
    }
    fprintf(stderr, "\n");
    m++;
  }
  fprintf(stderr, "\n");
}
 
#pragma inline vecset
void vecset(int count, register double *dst, int const d, double value) {
  /* DST=SRC*factor */
  while (count-->0) {
    (*dst)=value;
    dst+=d;
  }
}
#pragma inline veccopy
void veccopy(int count, register double *dst, int const d, 
	     register double *src, int const s) {
  /* DST=SRC */
  while (count-->0) {
    (*dst)=(*src);
    dst+=d;src+=s;
  }
}
#pragma inline vecscale
void vecscale(int count, register double *dst, int const d, 
	      double const factor) {
  /* DST=SRC*factor */
  while (count-->0) {
    (*dst)*=factor;
    dst+=d;
  }
}
#pragma inline vecmultscalar
void vecmultscalar(int count, register double *dst, int const d, 
		   register double *src, int const s, double const factor) {
  /* DST=SRC*factor */
  if (1==s) {
    while (count-->0) {
      (*dst)=(*src++)*factor;
      dst+=d;
    }
  } else {
    while (count-->0) {
      (*dst)=(*src)*factor;
      dst+=d;src+=s;
    }
  }
}
#pragma inline vecmult
void vecmult(int count, 
	     register double *dst, int const d, 
	     register double *src1, int const s1, 
	     register double *src2, int const s2) {
  /* DST=SRC1*SRC2 */
  while (count-->0) {
    (*dst)=(*src1)*(*src2);
    dst+=d;src1+=s1;src2+=s2;
  }
}
#pragma inline vecmult2
void vecmult2(int count, 
	      register double *dst, int const d, 
	      register double *src1, int const s1, 
	      register double *src2, int const s2, double const factor) {
  /* DST=SRC1*SRC2*factor */
  while (count-->0) {
    (*dst)=(*src1)*(*src2)*factor;
    dst+=d;src1+=s1;src2+=s2;
  }
}
#pragma inline vecmult2andsum
double vecmult2andsum(int count, 
		      register double *dst, int const d, 
		      register double *src1, int const s1, 
		      register double *src2, int const s2, 
		      double const factor) {
  /* DST=SRC1*SRC2*factor, return sum */
  register double q,sum=0.0;
  while (count-->0) {
    q=(*src1)*(*src2)*factor; *dst=q;
    sum+=q;dst+=d;src1+=s1;src2+=s2;
  }
  return sum;
}
#pragma inline vecmultandsum
double vecmultandsum(int count, 
		     register double *dst, int const d, 
		     register double *src1, int const s1, 
		     register double *src2, int const s2) {
  /* DST=SRC1*SRC2, return SUM(DST) */
  register double q,sum=0.0;
  while (count-->0) {
    q=(*src1)*(*src2);*dst=q;
    sum+=q;dst+=d;src1+=s1;src2+=s2;
  }
  return sum;
}
#pragma inline vecmultsum
double vecmultsum(register int count, 
		  register double *src1, int const s1, 
		  register double *src2, int const s2) {
  /* return SUM(SRC1*SRC2) */
  register double sum=0;
  if (1==s1) {
    while (count-->0) {
      register double const q=*src1++;
      if (0!=q) sum+=q*(*src2);
      src2+=s2;
    }
  } else if (1==s2) {
    while (count-->0) {
      register double const q=*src1;
      if (0!=q) sum+=q*(*src2);
      src1+=s1;src2++;
    }
  } else {
    while (count-->0) {
      register double const q=*src1;
      if (0!=q) sum+=q*(*src2);
      src1+=s1;src2+=s2;
    }
  }
  return sum;
}

void fb_analysis(double * const T,	/* transition matrix */
		 double * const S,	/* initial state probabilities */
		 double * const PO,	/* P(output|state) */
		 int const states,	/* # states */
		 int const frames,	/* # time steps */
		 int const lengthPO,	/* # rows in PO */
		 double *likelihood, /* where to put log-likelihood result */
		 double *forward, /* matrix for forward variable */
		 double *backward,
		 double *occupation, /* state occupation matrix */
		 double *scale, /* per-frame likelihoods */
		 double *p_states /* state occupation sums */
		 )	
{
  int warned=0;
  int const wants_scale=(scale!=0);
  unsigned char *fix;
#ifdef __ALLOCA_H
  double *tmp=(double*)alloca(states*sizeof(double));
  unsigned char *bad_outputs=(unsigned char*)alloca(frames*sizeof(unsigned char));
  if (!scale)     scale=(double*)alloca(frames*sizeof(double));
  if (!forward) forward=(double*)alloca(states*frames*sizeof(double));
#else
  double *tmp=(double*)calloc(states,sizeof(double));
  unsigned char *bad_outputs=(unsigned char*)calloc(frames,sizeof(unsigned char));
  int const flag_scale=!scale;
  int const flag_forward=!forward;
  if (flag_scale) scale=(double*)calloc(frames,sizeof(double));
  if (flag_forward) forward=(double*)calloc(states*frames,sizeof(double));
#endif

/*  mprint(S,1,1,states);		
    mprint(T,states,states,states);
    mprint(PO,lengthPO,frames,states); */
  fix = bad_outputs;
  {
    /* Do initial probabilities of 1st state */
    double q=vecmultandsum(states,tmp,1,S,1,PO,lengthPO);
    double *s=scale;
    /*mprint(tmp,1,1,states);*/
/*    fprintf(stderr,"%g\n",q);*/
    if (q>0) {
      q=1/q; /* take inverse */
      vecmultscalar(states,forward,frames,tmp,1,q); /* rescale forward #s */
      *fix++=0;
    } else {			/* problem due to underflow */
      q=REALMAX/(256*256); /* use default awful value */
      veccopy(states,forward,frames,S,1); /* copy in start probs */
      *fix++=1;
      if (!warned) {
	char buf[200];
	sprintf(buf, "fb_analysis: nonsense value at t=%d; numerical error or bad data",1);
#ifdef IN_MATLAB
	mexWarnMsgTxt(buf);
#else
	fprintf(stderr,"%s\n",buf);
#endif
	warned=1;
      }
    }
    *s++=q;
    /*    fprintf(stderr,"%g\n",q);*/
    /*    mprint(forward,frames,1,states);*/
    /* now iterate over each subsequent frame */
    {
      int t=frames-1; double *pbase=PO+1; double *fbase=forward; 
      while (t-->0) {
	int i=states; double sum=0.0; double *dst=tmp; double *tbase=T;
	double *p=pbase;
	for(;i>0; i--,p+=lengthPO,tbase+=states) { /* for each new state */
	  q=(*p)*vecmultsum(states,tbase,1,fbase,frames);
	  *dst++=q; sum=sum+q;
	}
	if (sum>0.0) {
	  q=1/sum; 
	  if (q>0.0) *s++=q; else *s++=REALMIN;
	  fbase++;*fix++=0;
	  vecmultscalar(states,fbase,frames,tmp,1,q);
	} else {	/* underflow error; fix: don't multiply by PO */
	  int i=states; double *dst=fbase+1; double *tbase=T;
	  for(;i>0; i--,tbase+=states,dst+=frames) 
	    *dst=vecmultsum(states,tbase,1,fbase,frames);
	  fbase++;*fix++=1;
	  q=REALMAX/(256*256);*s++=q;
	  if (!warned) {
	    char buf[200];
	    sprintf(buf, "fb_analysis: nonsense value at t=%d; numerical error or bad data",frames-t);
#ifdef IN_MATLAB
	    mexWarnMsgTxt(buf);
#else
	    fprintf(stderr,"%s\n",buf);
#endif
	    warned=1;
	  }
	}
	/* mprint(fbase,frames,1,states);*/
	pbase++;
      }
    }
  }
  /*mprint(forward,frames,frames,states);*/
  
  if (likelihood) {  /* Calculate likelihood */
    int t=frames; double *s=scale; double sum=0.0; 
    while (t-->0) 
      sum-=log(*s++);
    *likelihood=sum;
  }

  /*fprintf(stderr,"back");*/
  if (backward) {
    double *s=scale+frames-1; double *bbase=backward+frames-1; double q=*s--;
    /*fprintf(stderr,"%g\n",q);*/
    /* Do last time frame */
    {int i=states; double *b=bbase; for(;i>0; i--, b+=frames) *b=q;}
    /*mprint(bbase,frames,1,states);*/
    {
      int t=frames-1; double *pbase=INDEX(PO,frames,1,lengthPO); 
      fix--;
      for(; t>0; t--,pbase--,s--) { /* for each frame */
	if (*--fix) {
	  veccopy(states,tmp,1,bbase,frames);
	} else 
	  vecmult(states,tmp,1,bbase,frames,pbase,lengthPO);
	/*mprint(tmp,1,1,states);*/
	q=*s;
	bbase--;
	{
	  int j=states; double *b=bbase; double *tbase=T;
	  for(; j>0; j--,b+=frames,tbase++) { /* for each new state */
	    double const bb=q*vecmultsum(states,tbase,states,tmp,1);
	    *b=(bb>REALMAX)?REALMAX:bb;
	  }
	}
	/*mprint(bbase,frames,1,states);*/
      }
    }
  } else return;
  /*mprint(backward,frames,frames,states);*/
  
  if (occupation) {
    int t; double *fbase=forward, *bbase=backward, *obase=occupation, *s=scale;
    for (t=frames; t>0; t--) {
      double q=1.0/(*s++);
      if (*fix++) q=1./vecmultsum(states,fbase,frames,bbase,frames);
      q=vecmult2andsum(states,obase++,frames,fbase++,frames,bbase++,frames,q);
      if ((q!=1.0)&&(q!=0.0)) vecscale(states,obase-1,frames,1.0/q);
    }
  }
  /*fprintf(stderr,"occupation done\n");*/

  if (wants_scale) {
    double *s=scale; int t;
    for (t=frames; t>0; t--) { *s=1/(*s); s+=1; }
  }

  if (p_states) {
    {int i=states; double *dst=tmp;
    for(;i>0; i--) *dst++=0.0;
    }
    {int t; double *fbase=forward; double *bbase=backward; double *s=scale;
#pragma parallel local(t)
#pragma pfor
    for(t=frames;t>0; t--) {
      double q=*s;
      {int i=states; double *f=fbase; double *b=bbase; double *dst=tmp;
      for(; i>0; i--,f+=frames,b+=frames,dst++) 
	*dst+=(*f)*(*b)*q;
      }
      fbase++,bbase++,s++;}
    }}
  
#ifndef __ALLOCA_H
  /*fprintf(stderr,"all but freed\n");*/
  free(tmp); 		/* clean up */
  free(bad_outputs);
  if (flag_scale) free(scale);
  if (flag_forward) free (forward);
  /*fprintf(stderr,"freed\n");*/
  /*fprintf(stderr,"");*/	/* THIS IS HERE TO FIX SOME COMPILER ERROR */
#endif

}
#ifdef IN_MATLAB
void mexFunction(int nlhs,	     /* Num return vals on lhs */
		 Matrix *plhs[],    /* Matrices on lhs      */
		 int nrhs,	     /* Num args on rhs    */
		 Matrix *prhs[]     /* Matrices on rhs */
		 )
{
  Matrix *arg;
  double *Transitions=0,*Starts=0,*Pobservations=0,*likelihood=0,*forward=0,*backward=0,*occupancy=0,*scale=0;
  int T,frames,states,lengthPO,first;
  
  if (nrhs < 3 ) mexErrMsgTxt("requires at least 3 input arguments.");
  
  /* ARG 1: MATRIX  */
  arg = prhs[0];
  if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
  Transitions = mxGetPr(arg);
  states = (int) mxGetM(arg);
  if (states!=(int) mxGetM(arg))
    mexErrMsgTxt("1st arg must be NxN real non-sparse matrix.");

  /* ARG 2: VECTOR */
  arg = prhs[1];
  if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
  Starts = mxGetPr(arg);
  if (states!=(int) mxGetM(arg)*(int) mxGetN(arg))
    mexErrMsgTxt("1st arg must be NxN matrix; 2nd arg must be 1xN vector.");

  /* ARG 3: MATRIX  */
  arg = prhs[2];
  if notDblMtx(arg) mexErrMsgTxt("MTX arg must be a real non-sparse matrix.");
  Pobservations = mxGetPr(arg);
  lengthPO = (int) mxGetM(arg);
  if ((lengthPO<1)||
      (states!=(int) mxGetN(arg)))
    mexErrMsgTxt("1st arg must be NxN matrix; 3rd arg must be TxN matrix.");

  /* ARG 4: frames */
  if (nrhs>3) {
    arg = prhs[3];
    if (notDblMtx(arg)||
	(mxGetM(arg) * mxGetN(arg) != 1))
      mexErrMsgTxt("optional 4th arg must be an integer.");
    frames=(int)*mxGetPr(arg);
    /* fprintf(stderr,"frames=%d\n",frames); */
    if (frames>lengthPO)
      mexErrMsgTxt("more frames requested than data available.");
  } else frames=lengthPO;

  first=0;
  /* ARG 5: first */
  if (nrhs>4) {
    arg = prhs[4];
    if (notDblMtx(arg)||
	(mxGetM(arg) * mxGetN(arg) != 1))
      mexErrMsgTxt("optional 5th arg must be a positive integer.");
    first=(int)*mxGetPr(arg)-1;
    /* fprintf(stderr,"first=%d\n",first); */
    if (first<0) 
      mexErrMsgTxt("requested begin frame is must be >=1.");
    if (first+frames>lengthPO)
      mexErrMsgTxt("requested begin frame is too big.");
  }

  /* OUT 1: likelihood */
  plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
  likelihood=mxGetPr(plhs[0]);

  /* OUT 2: forward */
  if (nlhs>1) {
    plhs[1]=mxCreateDoubleMatrix(frames,states,mxREAL);
    forward=mxGetPr(plhs[1]);
  }
  /* OUT 3: backward */
  if (nlhs>2) {
    plhs[2]=mxCreateDoubleMatrix(frames,states,mxREAL);
    backward=mxGetPr(plhs[2]);
  }
  /* OUT 4: occupancy */
  if (nlhs>3) {
    plhs[3]=mxCreateDoubleMatrix(frames,states,mxREAL);
    occupancy=mxGetPr(plhs[3]);
  }

  /* OUT 5: scale */
  if (nlhs>4) {
    plhs[4]=mxCreateDoubleMatrix(1,frames,mxREAL);
    scale=mxGetPr(plhs[4]);
  }

  fb_analysis(Transitions,Starts,Pobservations+first,states,frames,lengthPO,likelihood,forward,backward,occupancy,scale,(double*)0);
  
  /* fprintf(stderr,"done\n"); */
}


#else
main() {
  double *const t=(double*)calloc(256*256,sizeof(double));
  double *const s=(double*)calloc(1*256,sizeof(double));
  double *const p=(double*)calloc(512*256,sizeof(double));
  double *const f=(double*)calloc(512*256,sizeof(double));
  double *const b=(double*)calloc(512*256,sizeof(double));
  double *const o=(double*)calloc(512*256,sizeof(double));
  double *const sc=(double*)calloc(512*1,sizeof(double));
  double like;
  int i;
  double *z = t;
  for (i=0;i<256*256;i++) *z++=((double)rand())/((double)RAND_MAX);
  z=s;
  for (i=0;i<256;i++) *z++=((double)rand())/((double)RAND_MAX);
  z=p;
  for (i=0;i<512*256;i++) *z++=((double)rand())/((double)RAND_MAX);
  
  fb_analysis(t,s,p,256,512,512,&like,f,b,o,sc,(double*)0);
  printf("%g\n",*o);
}
#endif
