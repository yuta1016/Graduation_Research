/*--------------------------------------------------------------------------------------
 This software module was originally developed by

 EPFL (Yousri Abdeljaoued,abdel@ltssg3.epfl.ch)
 Ericsson Radio Systems AB,(Charilaos Christopoulos, ch.christopoulos@era.ericsson.se)
 (contributing organizations names)
 
 in the course of development of the MPEG-7 Experimentation Model.

 This software module is an implementation of a part of one or more MPEG-7
 Experimentation Model tools as specified by the MPEG-7 Requirements.

 ISO/IEC gives users of MPEG-7 free license to this software module or
 modifications thereof for use in hardware or software products claiming
 conformance to MPEG-7.

 Those intending to use this software module in hardware or software products
 are advised that its use may infringe existing patents. The original
 developers of this software module and their companies, the subsequent
 editors and their companies, and ISO/IEC have no liability for use of this
 software module or modifications thereof in an implementation.
 
 Copyright is not released for non MPEG-7 conforming products. The
 organizations named above retain full right to use the code for their own
 purpose, assign or donate the code to a third party and inhibit third parties
 from using the code for non MPEG-7 conforming products.
 
 Copyright (c) 1998-1999-2000.
 
 This notice must be included in all copies or derivative works.
 
 Applicable File Name:  DoubleArrayMatrix.c

-------------------------------------------------------------------------------------*/
#include "DoubleArrayMatrix.h"

#ifndef RETURN_SUCCESS
#define RETURN_SUCCESS 0
#endif

#ifndef RETURN_FAILURE
#define RETURN_FAILURE -1
#endif

extern double hypot (double, double);

/*-----------------------------------------------------------------------------
Inverse of a Matrix -- Matrix must be square!!
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayInverse(DoubleArray *A)
{
  /*-----  -----*/
  int minX;
  int maxX;
  int minY;
  int maxY;
  int sizeX;
  int sizeY;
  
  /*-----  -----*/
  double **s;
  double **s_1;

  /*-----  -----*/
  DoubleArray *A_1;
  DoubleArray *U;
  DoubleArray *V;
  DoubleArray *S;
  
  DoubleArray *UT;
  DoubleArray *S_1;
  DoubleArray *VS_1;
  
  /*-----  -----*/
  minX = A->loLimX;
  maxX = A->upLimX;
  minY = A->loLimY;
  maxY = A->upLimY;
  sizeX = A->sizeX;
  sizeY = A->sizeY;
  
  /*-----  -----*/
  U = doubleArrayCreate(minX, maxX, minY, maxY);
  S = doubleArrayCreate(minX, maxX, minY, maxY);
  S_1 = doubleArrayCreate(minX, maxX, minY, maxY);
  V = doubleArrayCreate(minX, maxX, minY, maxY);
  
  /*-----  -----*/
  doubleArraySVD(A, U, S, V);
  
  /*-----  -----*/
  {
    int minX = S->loLimX;
    int maxX = S->upLimX;
  
    int x;
    
    s = S->array;
    s_1 = S_1->array;

    doubleArrayInit(S_1);
    for (x = minX; x <= maxX; x++)
    {
	ARRAY(s_1, x, x) = 1.0/(ARRAY(s, x, x));
    }
  }

  /*-----  -----*/
  UT = doubleArrayTranspose(U);
  VS_1 = doubleArrayMult(V, S_1);
  A_1 = doubleArrayMult(VS_1, UT);

  /*-----  -----*/
  doubleArrayDestroy(S);
  doubleArrayDestroy(S_1);
  doubleArrayDestroy(VS_1);
  doubleArrayDestroy(UT);
  return(A_1);
}
/*-----------------------------------------------------------------------------
Inverse of a Matrix -- Matrix must be square!!
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayInverseRobust(DoubleArray *A, int k)
{
  /*-----  -----*/
  int minX;
  int maxX;
  int minY;
  int maxY;
  int sizeX;
  int sizeY;
  
  /*-----  -----*/
  double **s;
  double **s_1;

  /*-----  -----*/
  DoubleArray *A_1;
  DoubleArray *U;
  DoubleArray *V;
  DoubleArray *S;
  
  DoubleArray *UT;
  DoubleArray *S_1;
  DoubleArray *VS_1;
  
  /*-----  -----*/
  doubleArrayOrigin(A, 0, 0);

  /*-----  -----*/
  minX = A->loLimX;
  maxX = A->upLimX;
  minY = A->loLimY;
  maxY = A->upLimY;
  sizeX = A->sizeX;
  sizeY = A->sizeY;
  
  /*-----  -----*/
  U = doubleArrayCreate(minX, maxX, minY, maxY);
  S = doubleArrayCreate(minX, maxX, minY, maxY);
  S_1 = doubleArrayCreate(minX, maxX, minY, maxY);
  V = doubleArrayCreate(minX, maxX, minY, maxY);
  
  /*-----  -----*/
  doubleArraySVD(A, U, S, V);
  
  /*-----  -----*/
  {
    int minX = S->loLimX;
    int maxX = S->upLimX;
    int x;
    
    s = S->array;
    s_1 = S_1->array;

    doubleArrayInit(S_1);
    for (x = minX; x <= maxX; x++)
    {
      ARRAY(s_1, x, x) = 1.0/(ARRAY(s, x, x));
    }

/*
    ARRAY(s_1, maxX, maxX) = 0.0;
    ARRAY(s_1, minX, minX) = 0.0;
*/
    
/*
    for (x = maxX-k; x <= maxX; x++)
    {
      ARRAY(s_1, x, x) = 0.0;
    }
    for (x = minX; x <= minX+k; x++)
    {
      ARRAY(s_1, x, x) = 0.0;
    }
*/
  }

  /*-----  -----*/
  UT = doubleArrayTranspose(U);
  VS_1 = doubleArrayMult(V, S_1);
  A_1 = doubleArrayMult(VS_1, UT);

  /*-----  -----*/
  doubleArrayDestroy(S);
  doubleArrayDestroy(S_1);
  doubleArrayDestroy(VS_1);
  doubleArrayDestroy(UT);
  doubleArrayDestroy(V);
  doubleArrayDestroy(U);

  return(A_1);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
/*
 *       This is a translation of the Singular Value Decomposition algorithm of
 *       Golub and Reinsch (Numerische Mathematik 14(1970) pp403-470) from
 *       Algol 60 to C. The routine takes a single matrix (A) and computes two
 *       additional matrices (U and V) and a vector W such that
 *               A = UWV'
 *       The calling sequence is
 *       svd(a,m,n,u,w,v,eps,flags)
 *       where
 *       a is the original, m by n matrix
 *       u is the upper resultant
 *       w is the vector of singular values
 *       v is the lower resultant
 *       eps is a convergence test constant
 *       flags tells what to do, values:-
 *                                       0 produce only w
 *                                       1 produce w and u
 *                                       2 produce w and v
 *                                       3 produce u, w, and v.
 *       NOTE m must be greater than n or an error is signaled
 *
 *       BC 5/11/87
 *       Moved to 0->n-1 indices for vectors BC 5/12/87
 */
#define WITHV   1
#define WITHU   2

void dAsvd(
	  double **a,
	  int m,
	  int n,
	  double **u,
	  double *w,
	  double **v,
	  double eps,
	  int flags,
	  double *temp)
{
  int i,
  j,
  k,
  l,
  l1;			/*----- Mostly loop variables -----*/
  /*
    double tol = MINDOUBLE/eps;
    */
  double tol = eps;
  double c,
  f,
  g,
  h,
  s,
  x,
  y,
  z;			/*----- Temporaries -----*/
  if (m < n)
  {
    fprintf(stderr,"m must be greater than n!\n");
    fflush(stderr);
    exit(-1);
  }
  /*----- First copy a into u. -----*/
  for (i = 0; i < m; i++)
  {
    for (j = 0; j < n; ++j)
    {
      u[j][i] = a[j][i];
    }
  }
  /*----- Reduce the u matrix to bidiagonal form with Householder
    transforms. -----*/
  g = (x = 0.0);
  for (i = 0; i < n; i++)
  {
    temp[i] = g;
    s = 0.0;
    l = i + 1;
    for (j = i; j < m; j++)
    {
      s += u[j][i] * u[j][i];
    }
    if (s < tol)
    {
      g = 0.0;
    }
    else
    {
      f = u[i][i];
      g = (f < 0.0) ? sqrt(s) : -sqrt(s);
      h = f * g - s;
      u[i][i] = f - g;
      for (j = l; j < n; j++)
      {
	s = 0.0;
	for (k = i; k < m; k++)
	{
	  s += u[k][i] * u[k][j];
	}
	f = s / h;
	for (k = i; k < m; k++)
	{
	  u[k][j] += f * u[k][i];
	}
      }
    }
    w[i] = g;
    s = 0.0;
    for (j = l; j < n; j++)
    {
      s += u[i][j] * u[i][j];
    }
    if (s < tol)
    {
      g = 0.0;
    }
    else
    {
      f = u[i][i + 1];
      g = (f < 0.0) ? sqrt(s) : -sqrt(s);
      h = f * g - s;
      u[i][i + 1] = f - g;
      for (j = l; j < n; j++)
      {
	temp[j] = u[i][j] / h;
      }
      for (j = l; j < m; j++)
      {
	s = 0.0;
	for (k = l; k < n; k++)
	{
	  s += u[j][k] * u[i][k];
	}
	for (k = l; k < n; k++)
	{
	  u[j][k] += s * temp[k];
	}
      }
    }
    y = fabs(w[i]) + fabs(temp[i]);
    if (y > x)
    {
      x = y;
    }
  }
  /*----- Now accumulate right-hand transforms if we are building v
    too. -----*/
  if (flags & WITHV)
  {
    for (i = n - 1; i >= 0; --i)
    {
      if (g != 0.0)
      {
	h = u[i][i + 1] * g;
	for (j = l; j < n; j++)
	{
	  v[j][i] = u[i][j] / h;
	}
	for (j = l; j < n; j++)
	{
	  s = 0.0;
	  for (k = l; k < n; k++)
	  {
	    s += u[i][k] * v[k][j];
	  }
	  for (k = l; k < n; k++)
	  {
	    v[k][j] += s * v[k][i];
	  }
	}
      }
      for (j = l; j < n; j++)
      {
	v[i][j] = (v[j][i] = 0.0);
      }
      v[i][i] = 1.0;
      g = temp[i];
      l = i;
    }
  }
  /*----- Now accumulate the left-hand transforms. -----*/
  if (flags & WITHU)
  {
    for (i = n - 1; i >= 0; --i)
    {
      l = i + 1;
      g = w[i];
      for (j = l; j < n; j++)
      {
	u[i][j] = 0.0;
      }
      if (g != 0.0)
      {
	h = u[i][i] * g;
	for (j = l; j < n; j++)
	{
	  s = 0.0;
	  for (k = l; k < m; k++)
	  {
	    s += u[k][i] * u[k][j];
	  }
	  f = s / h;
	  for (k = i; k < m; k++)
	  {
	    u[k][j] += f * u[k][i];
	  }
	}
	for (j = i; j < m; j++)
	{
	  u[j][i] /= g;
	}
      }
      else
      {
	for (j = i; j < m; j++)
	{
	  u[j][i] = 0.0;
	}
      }
      u[i][i] += 1.0;
    }
  }
  /*----- Now diagonalise the bidiagonal form. BEWARE GOTO's IN THE
    LOOP!! -----*/
  eps = eps * x;
  for (k = n - 1; k >= 0; --k)
  {
    testsplitting:
    for (l = k; l >= 0; --l)
    {
      if (fabs(temp[l]) <= eps)
      goto testconvergence;
      if (fabs(w[l - 1]) <= eps)
      goto cancellation;
    }
    /*----- Cancellation of temp[l] if l > 0; -----*/
    cancellation:
    c = 0.0;
    s = 1.0;
    l1 = l - 1;
    for (i = l; i <= k; i++)
    {
      f = s * temp[i];
      temp[i] *= c;
      if (fabs(f) <= eps)
      goto testconvergence;
      g = w[i];
      h = (w[i] = hypot(f, g));
      c = g / h;
      s = -f / h;
      if (flags & WITHU)
      {
	for (j = 0; j < m; j++)
	{
	  y = u[j][l1];
	  z = u[j][i];
	  u[j][l1] = y * c + z * s;
	  u[j][i] = -y * s + z * c;
	}
      }
    }
    testconvergence:
    z = w[k];
    if (l == k)
    goto convergence;
    /*----- Shift from bottom 2x2 minor. -----*/
    x = w[l];
    y = w[k - 1];
    g = temp[k - 1];
    h = temp[k];
    f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2 * h * y);
    g = hypot(f, 1.0);
    f = ((x - z) * (x + z) + h * (y / ((f < 0.0) ? f - g : f + g) - h)) / x;
    /*----- Next QR transformation. -----*/
    c = (s = 1);
    for (i = l + 1; i <= k; i++)
    {
      g = temp[i];
      y = w[i];
      h = s * g;
      g *= c;
      temp[i - 1] = (z = hypot(f, h));
      c = f / z;
      s = h / z;
      f = x * c + g * s;
      g = -x * s + g * c;
      h = y * s;
      y *= c;
      if (flags & WITHV)
      {
	for (j = 0; j < n; j++)
	{
	  x = v[j][i - 1];
	  z = v[j][i];
	  v[j][i - 1] = x * c + z * s;
	  v[j][i] = -x * s + z * c;
	}
      }
      w[i - 1] = (z = hypot(f, h));
      c = f / z;
      s = h / z;
      f = c * g + s * y;
      x = -s * g + c * y;
      if (flags & WITHU)
      {
	for (j = 0; j < m; j++)
	{
	  y = u[j][i - 1];
	  z = u[j][i];
	  u[j][i - 1] = y * c + z * s;
	  u[j][i] = -y * s + z * c;
	}
      }
    }
    temp[l] = 0.0;
    temp[k] = f;
    w[k] = x;
    goto testsplitting;
    
    convergence:
    if (z < 0.0)
    {
      /*----- w[k] is made non-negative. -----*/
      w[k] = -z;
      if (flags & WITHV)
      {
	for (j = 0; j < n; j++)
	{
	  v[j][k] = -v[j][k];
	}
      } 
    }
  }
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArraySVD(
		   DoubleArray * A,
		   DoubleArray * U,
		   DoubleArray * S,
		   DoubleArray * V)
{
  /*-----  -----*/
  int sizeX;
  int sizeY;
  int minX;
  int maxX;
  int minY;
  int maxY;
  
  /*-----  -----*/
  double **a;
  double **u;
  double **s;
  double **v;
  
  /*-----  -----*/
  double *w;
  double *t;

  /*-----  -----*/
  doubleArrayOrigin(V, 0, 0);
  doubleArrayOrigin(A, 0, 0);
  doubleArrayOrigin(U, 0, 0);
  doubleArrayOrigin(S, 0, 0);
  
 /*-----  -----*/
  sizeX = A->sizeX;
  sizeY = A->sizeY;
  minX = A->loLimX;
  maxX = A->upLimX;
  minY = A->loLimY;
  maxY = A->upLimY;
  
  /*-----  -----*/
  a = A->array;
  s = S->array;
  v = V->array;
  u = U->array;
  
  /*-----  -----*/
  w = (double *) malloc(sizeX * sizeof(double));
  if (w == (double *) NULL)
  {
    perror("doubleArraySVD(): malloc() fails!");
    exit(EXIT_FAILURE);
  }
  
  t = (double *) malloc(sizeX * sizeof(double));
  if (t == (double *) NULL)
  {
    perror("doubleArraySVD(): malloc() fails!");
    exit(EXIT_FAILURE);
  }
  
  /*-------------------------------------------------------
  SVD
  -------------------------------------------------------*/
  dAsvd(a, sizeX, sizeY, u, w, v, 1.0e-24, 3, t);

  /*-----  -----*/
  /*#ifdef XXX*/
  doubleArrayInit(S);
  {
    int x;
    for (x = minY; x <= maxY; x++)
    {
      ARRAY(s, x, x) = w[x];
    }
  }
  
  free(w);
  /*#endif*/
  free(t);
  
  return (RETURN_SUCCESS);
}
#ifdef XXX
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void svd(
   DoubleArray *opm,
   DoubleArray *u,
   DoubleVector *d,
   DoubleArray *v,
   double tol)
{
/*
  // Returns the two orthogonal matrices and a vector of singular values. Thus
  // the COLUMNS of u and v are the left and right singular vectors of opm.
  // The original matrix may be reconstructed by u^diag(sv)^adjoint(v).
  // u and v must be identity matrices to start. Here class Real should be of
  // some variant of a real number.
  */
   if(tol<=(Real)0.0) 
      tol=(Real)SVD_TINY;
   if(opm.rsize()<opm.csize()) 
   {
      cerr << "svd(): Underdetermined matrix.";
      exit(1);
   }
   size_t ortho=u.rsize();   /* flag for accum. the orthog matrices */
   /* ortho=0 means u and v are null matrices and accum. is turned off */
   bidiag(opm,1);          /* reduce to bidiagonal form; */
   if(ortho) 
   {
      col_accum(opm,u,opm.csize(),opm.csize());    /* accum the left orthog matrix */
      row_accum(opm,v,opm.csize()-2,opm.csize()-1); /* accum right orthog matrix */
   }
   /* the original matrix may now be reconstructed as u^bidiag^adjoint(v) */
   Real tmp;
   T tmpc;      /* change to real bidiag */
   for(size_t i=1;i<opm.csize();i++) 
   {          /* with one element changed */
      tmp=abs(opm(i,i));
      if(tmp!=(Real)0.0) 
      {
	 tmpc=conj(opm(i,i))/tmp;
	 opm(i,i)*=tmpc;          /* reduce (i,i) to real */
	 opm(i,i+1)*=tmpc;
	 if(ortho) 
	    u.col(i)*=conj(tmpc);     /* same oper on entire col of u */
      }
      tmp=abs(opm(i,i+1));
      if(tmp!=(Real)0.0) 
      {
	 tmpc=conj(opm(i,i+1))/tmp;    /* reduce (i,i+1) to real */
	 opm(i,i+1)*=tmpc;
	 opm(i+1,i+1)*=tmpc;
	 if(ortho) 
	    v.col(i+1)*=tmpc; /* same oper on entire col of v */
      }
   }
   tmp=abs(opm(opm.csize(),opm.csize())); /* reduce (col,col) to real */
   if(tmp!=(Real)0.0) 
   {
      tmpc=conj(opm(opm.csize(),opm.csize()))/tmp;
      opm(opm.csize(),opm.csize())*=tmpc;
      if(ortho) 
	 u.col(opm.csize())*=conj(tmpc);    /* same oper on entire col of u */
   }
   /* bidiag is now REAL and positive */
   /* the original matrix may still be reconstructed as u^bidiag^adjoint(v) */
   mVector<Real,allocator<Real> > f(opm.csize()-1);  
   f = doubleVectorCreate(opm->);
   /* d=diagonal, f=upper band */ 
   for(size_t i=1;i<opm.csize();i++) 
   {
      d(i)=real(opm(i,i));
      f(i)=real(opm(i,i+1));
   }
   d(opm.csize())=real(opm(opm.csize(),opm.csize()));
   Real aa,bb,cc,c,s,mu;
   size_t bottom=opm.csize(), top=1;   /* initially look at the bidiag matrix */
   for(size_t k=0;k<SVD_IT_MAX*opm.csize();k++) 
   {
      if(k==SVD_IT_MAX*opm.csize()) 
      {
	 cerr << "svd(): No convergence.\n";
	 exit(1);
      }
      for(size_t i=1;i<=bottom-1;i++)  /* search for zeroes on the super-diagonal*/ 
	 if(abs(f(i))<=tol*(abs(d(i))+abs(d(i+1)))) 
	    f(i)-=f(i);
      /* deflate first: the bottom is the first diag. element which */
      while(bottom>1) 
      {
	 /* bottom diag. elem. with nonzero superdiag. above */
	 if(f(bottom-1)!=(Real)0.0) 
	    break;
	 bottom--;     /* move bottom up one */
      }
      if(bottom==1) break;       /* if at the top then we're done */
      /* set the top of the block */
      top=bottom-1;
      while(top>=1) 
      {   /* search for zero diagonal elements */
	 if(d(top)!=(Real)0.0) top--;
	 else 
	 {
	    if(f(top)!=(Real)0.0) 
	    { /* if row is not zero then zero it */
	       tmp=f(top);         /* element to be zeroed */
	       f(top)-=f(top);        /* zero it */
	       for(size_t j=top+1;j<=bottom;j++) 
	       {  /* chase the zero out the row */
		  c=tmp;            /* element to zero */
		  s=d(j);          /* diag element below */
		  givens1(c,s);     /* calc. the rotation to zero the first arg. */
		  /* note that  d(top)*=c  and f(top)=c*f(top)-s*d(top+1) are identically 0 */
		  d(j)=s*tmp+c*d(j);    /* a Givens row rotation */
		  if(j<bottom) 
		  {
		     tmp=-s*f(j);          /* same as row_rot(top,j,c,s) */
		     f(j)*=c;
		  }
		  if(ortho) col_rot(u,top,j,c,s); /* left orthog. matrix */
	       }
	    }
	    break;  /* move on */
	 }
      }   /* top-loop */
      top++;  /* drop back down one row */
      size_t j=bottom;
      while(j>top) 
      {       /* search the block for zero super-diagonal elements */
	 if(f(j-1)==(Real)0.0) break;
	 j--;
      }  /* j-loop */
      top=j;    /* the diagonal block bounded by top and bottom is now unreduced */
      if(j==bottom) 
	 continue;     /* if the block is trivial start again */
      aa=d(bottom-1)*d(bottom-1);     /* the shift (mu) is the eigenvalue of the */
      bb=d(bottom)*d(bottom);       /* trailing 2X2 matrix of the block which is */
      cc=aa*f(bottom-1)*f(bottom-1);  /* closer to abs_sq(d(bottom)) */
      cc=sqrt((aa-bb)*(aa-bb)+4.0*cc);
      mu=(aa>bb) ? aa+bb-cc : aa+bb+cc;
      mu/=(Real)2.0;
      c=d(top)*d(top)-mu;        /* shift */
      s=f(top)*d(top);  /* element of B.adjoint()*B to be implicitly zeroed */
      for(size_t i=top;i<bottom;i++) 
      {
	 givens2(c,s);      /* calc. the column rotation rotation */
	 /* if(i!=top) then tmp=(i-1,i+1) is to be zeroed by col_rot(i,i+1,c,s) */
	 if(i!=top) 
	    f(i-1)=c*f(i-1)-s*tmp;    /* (i-1,i) */
	 tmp=d(i);              /* now tmp=(i,i) */
	 d(i)=c*tmp-s*f(i);     /* (i,i)         tmp=(i,i) */
	 f(i)=s*tmp+c*f(i);     /* (i,i+1)       tmp=(i,i) */
	 tmp=-s*d(i+1);         /* (i+1,i),  now tmp=(i+1,i) */
	 d(i+1)*=c;             /* (i+1,i+1) */
	 if(ortho) col_rot(v,i,i+1,c,s); /* also rotate the cols of v */
	 c=d(i);
	 s=tmp;                 /* tmp=(i+1,i),  which is to be zeroed */
	 givens2(c,s);      /* calc. rotation,  row_rot(i,i+1,c,s) */
	 /* opm.row(i)=c*opm.row(i)-s*opm.row(j);*/
	    /* opm.row(j)= s*opm.row(i)+c*opm.row(j); */
	 /* note that (i+1,i) is zeroed by the rotation */
	 d(i)=c*d(i)-s*tmp;           /* (i,i)    tmp=(i+1,i) */
	 tmp=f(i);          /* now tmp=(i,i+1) */
	 f(i)=c*tmp-s*d(i+1);         /* (i,i+1) */
	 d(i+1)=s*tmp+c*d(i+1);       /* (i+1,i+1) */
	 if(ortho) col_rot(u,i,i+1,c,s); /* also rotate the columns of u */
	 if(i<(bottom-1)) 
	 {      /* if not at the bottom prepare for next round */
	    tmp=-s*f(i+1);               /* (i,i+2),  f(bottom)=0 */
	    f(i+1)*=c;                  /* (i+1,i+2) */
	    c=f(i);                     /* (i,i+1) */
	    s=tmp;                  /* tmp=(i,i+2),  which is to be zeroed */
	 }
      }  /* i-loop */
   }  /* k-loop */
   for(size_t i=1;i<=opm.csize();i++) 
   {
      if(d(i)<(Real)0.0) 
      {      /* change sing. val. to positive */
	 d(i)=-d(i);
	 if(ortho)   /* apply the same oper. to the col of u */
	    for(size_t j=1;j<=opm.rsize();j++) 
	       u(j,i)=-u(j,i);
      }
   }
   for(size_t i=1;i<opm.csize();i++) 
   { /* sort the singular values, greatest first */
      for(size_t j=i+1;j<=opm.csize();j++) 
      {
	 if(d(j)>d(i)) 
	 {
	    tmp=d(i);         /* swap the sing. vals. */
	    d(i)=d(j);
	    d(j)=tmp;
	    if(ortho) 
	    {
	       for(size_t k=1;k<=u.rsize();k++) 
	       {    /* and the columns of u and v */
		  tmpc=u(k,i);
		  u(k,i)=u(k,j);
		  u(k,j)=tmpc;
		  tmpc=v(k,i);
		  v(k,i)=v(k,j);
		  v(k,j)=tmpc;
	       }  /* k-loop */
	    }
	 }
      }   /* j-loop */
   }  /* i-loop */
      /* Finally, set all sing. vals less than specified precision to zero */
   for(size_t j=1;j<=d.size();j++) 
      if(d(j)<tol*d(1)) 
	 d(j)-=d(j);
   return d;   /* can still reconstruct original matrix as u^diag(d)^adjoint(v) */ 
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/template<class T,class S,class Real,class S2> 
mVector<Real,S2>& 
singular_values(mMatrix<T,S> &opm,mVector<Real,S2> &sv,Real tol)
{
/* Same as svd(), but does not accumulate the left and right singular matrices.
  mMatrix<T,allocator<T> > u,v;
  return svd(opm,u,sv,v,tol);
  } end singular_values()*/

#endif
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#ifdef SVD
/*
  gcc -pedantic -ansi -Wall -Wtraditional -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -finline-functions -Winline -DSVD DoubleArray.c DoubleArrayMatrix.c -lm
  a.out fischer1.pgm
*/
int main(const int argc, char *const *argv)
{
  /*-----  -----*/
  DoubleArray *A  = doubleArrayReadRAWPGM(argv[1]);
  
  /*-----  -----*/
  /*
    int sizeX = A->sizeX;
    int sizeY = A->sizeY;
    */
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  /*-----  -----*/
  DoubleArray *U = doubleArrayCreate(minX, maxX, minX, maxX);
  DoubleArray *S = doubleArrayCreate(minX, maxX, minX, maxX);
  DoubleArray *V = doubleArrayCreate(minY, maxY, minY, maxY);
  
  /*-----  -----*/
  doubleArrayShow(A);

  /*-----  -----*/
  doubleArraySVD(A,U,S,V);

  /*-----  -----*/
  doubleArrayXV(A);
  doubleArrayXV(U);
  /* doubleArrayXV(S); */
  doubleArrayXV(V);

  /*-----  -----*/
  doubleArrayDestroy(A);
  doubleArrayDestroy(U);
  doubleArrayDestroy(S);
  doubleArrayDestroy(V);

  return(0);
}
#endif
/*-----------------------------------------------------------------------------
END
-----------------------------------------------------------------------------*/
