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
 
 Applicable File Name:  DoubleVector.c

-------------------------------------------------------------------------------------*/
#include "DoubleVector.h"

#ifndef RETURN_SUCCESS
#define RETURN_SUCCESS 0
#endif

#ifndef RETURN_FAILURE
#define RETURN_FAILURE -1
#endif

/* extern FILE	*popen(const char *, const char *); */
/* extern int	pclose(FILE *); */

/* #define RETURN_AT_ERROR*/

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorWrite(DoubleVector * vec_p, char *fileName)
{
    int n;
    FILE *file_p;
    file_p = fopen(fileName, "wb");
    if (file_p == (FILE *) NULL)
    {
	perror("writeDoubleVector(): fopen() fails!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
	return (RETURN_FAILURE);
#endif        
    }

    if (setvbuf(file_p, NULL, _IOFBF, BUFSIZ) != 0)
    {
	perror("doubleVectorWrite(): setvbuf() fails!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
	return (RETURN_FAILURE);
#endif        
    }

    for (n = vec_p->loLim; n <= vec_p->upLim; n++)
    {
	fprintf(file_p, "%d %f\n", n, vec_p->vector[n]);
    }

    fclose(file_p);

    fprintf(stderr, "Writing \"%s\"\n", fileName);
    fflush(stderr);
    return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorRead(char *fileName)
{
    int n;
    int i;
    double *vec = NULL;
    char s[256];
    DoubleVector *vec_p;
    FILE *file_p;
    float x;
    int loLim;
    int upLim;
    file_p = fopen(fileName, "rb");
    fprintf(stderr, "%s\n",fileName);
    if (file_p == (FILE *) NULL)
    {
	perror("doubleVectorRead(): fopen() fails!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
	return ((DoubleVector *)NULL);
#endif
    }

    n=0;
    while (feof(file_p) == 0)
    {
	fgets(s, 256, file_p);
	sscanf(s, "%f", &x);

	if (n == 0)
	{
	    vec = (double *) malloc(sizeof(double));
	}
	else
	{
	    vec = (double *) realloc(vec, sizeof(double) * (n+1));
	}
	vec[n] = (double) x;
	n++;
    }
    fclose(file_p);

    loLim = -n / 2;
    upLim = n / 2 - 1;
    vec_p = doubleVectorCreate(loLim, upLim);
    for (i = 0; i <= n-1; i++)
    {
	vec_p->vector[loLim + i] = vec[i];
    }
printf("----->%d\n",loLim);
printf("----->%d\n",upLim);
printf("----->%d\n",n);
    return (vec_p);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorDestroy(DoubleVector * vec_p)
{
    int loLim = vec_p->loLim;
    free(vec_p->vector + loLim);
    free(vec_p);
    return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorCreate(int loLim, int upLim)
{
    DoubleVector *vec_p;
    vec_p = (DoubleVector *) malloc(sizeof(DoubleVector));
    if (vec_p == (DoubleVector *) NULL)
    {
	perror("initDoubleVector(): malloc() fails!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
	return((DoubleVector *) NULL);
#endif
    }

    vec_p->vector = (double *) malloc((upLim - loLim + 1) * sizeof(double));

    if (vec_p->vector == (double *) NULL)
    {
	perror("initDoubleVector(): malloc() fails!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
	return((DoubleVector *) NULL);
#endif
    }

    vec_p->loLim = loLim;
    vec_p->upLim = upLim;
    vec_p->size = upLim - loLim + 1;
    vec_p->vector = vec_p->vector - loLim;

    doubleVectorInit(vec_p);
    return (vec_p);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorInit(DoubleVector * vec_p)
{
    int loLim = vec_p->loLim;
    int upLim = vec_p->upLim;
    int x;
    for (x = loLim; x <= upLim; x++)
    {
	VECTOR(vec_p->vector, x) = 0.0;
    }

    return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorMaxMin(DoubleVector * vec_p, double *max, double *min)
{
    int n;
    double *vec;
    int loLim;
    int upLim;
    double value;
    vec = vec_p->vector;
    loLim = vec_p->loLim;
    upLim = vec_p->upLim;
    *max = VECTOR(vec, loLim);
    *min = VECTOR(vec, loLim);

    for (n = loLim + 1; n <= upLim; n++)
    {
	value = VECTOR(vec, n);

	if (value > *max)
	{
	    *max = value;
	}
	if (value < *min)
	{
	    *min = value;
	}
    }
    return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorNormalize(DoubleVector * vec_p)
{
    int n;
    double *vec = vec_p->vector; 
    int loLim = vec_p->loLim;
    int upLim = vec_p->upLim;
    double max;
    double min;
    doubleVectorMaxMin(vec_p, &max, &min);

    if ((max - min) != 0.0)
    {
	for (n = loLim; n <= upLim; n++)
	{
	    VECTOR(vec, n) = (VECTOR(vec, n) - min) / (max - min);
	}
    }
    return (RETURN_SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorOrigin(DoubleVector * A, int x0)
{
  int loLim = A->loLim;  
  int size = A->size;

  A->vector = A->vector + loLim - x0;
  
  A->loLim = x0;
  A->upLim = x0 + size - 1;

  return(RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorWriteAsciiFile(DoubleVector *vec, char *fileName)
{
  FILE *file_p = fopen(fileName, "wb");
  if (file_p == NULL)
  {
    perror("doubleVectorWriteAsciiFile(): cannot open file!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
  if (doubleVectorWriteAscii(vec, file_p) != RETURN_SUCCESS)
  {
    perror("doubleVectorWriteAsciiFile(): error writing vector!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
  return (fclose(file_p));

}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorWriteAscii(DoubleVector *vec, FILE *file_p)
{
  int i = vec->loLim;
  int upLim = vec->upLim;
  double *vector = vec->vector;
  
  for (; i<=upLim; i++)
    fprintf(file_p, "%f\n", VECTOR(vector, i));
  return (RETURN_SUCCESS);
         
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

int doubleVectorWriteBinaryFile(DoubleVector *vec, char *fileName)
{
  FILE *file_p = fopen(fileName, "wb");
  if (file_p == NULL)
  {
    perror("doubleVectorWriteBinaryFile(): cannot open file!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
  if (doubleVectorWriteBinary(vec, file_p) != RETURN_SUCCESS)
  {
    perror("doubleVectorWriteBinaryFile(): error writing vector!");
#ifndef RETURN_AT_ERROR
        exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
  return (fclose(file_p));

}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

int doubleVectorWriteBinary(DoubleVector *vec, FILE *file_p)
{

  if (fwrite(&vec->loLim, sizeof(int), 1, file_p) != 1)
  {
    perror("doubleVectorWriteBinary(): error writing loLim!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
    
  if (fwrite(&vec->upLim, sizeof(int), 1, file_p) != 1)
  {
    perror("doubleVectorWriteBinary(): error writing upLim!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
    
  if (fwrite(vec->vector+vec->loLim, sizeof(double), vec->size, file_p)
      != vec->size)
  {
    perror("doubleVectorWriteBinary(): error writing elements!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return (RETURN_FAILURE);
#endif
  }
    
  return (RETURN_SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorReadAsciiFile(char *fileName)
{
  DoubleVector *out;
  FILE *file_p = fopen(fileName, "rb");
  if (file_p == NULL)
  {
    perror("doubleVectorReadAsciiFile(): cannot open file!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return((DoubleVector *) NULL);
#endif
  }
  out = doubleVectorReadAscii(file_p);
  fclose(file_p);
  return(out);
  
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorReadAscii(FILE *file_p)
{
  DoubleVector *out;
  double data[MAX_SIZE_VECTOR];
  double tmp = 0.0;
  int size = 0;
  
  while(fscanf(file_p, "%lf", &tmp) != EOF)
  {
    if (size<MAX_SIZE_VECTOR)
    {
      data[size] = tmp;
      size++;
    }
    else
    {
      perror("doubleVectorReadAscii(): size of input data too large");
#ifndef RETURN_AT_ERROR
      exit (EXIT_FAILURE);
#else
      return((DoubleVector *) NULL);
#endif
    }
  }

  out = doubleVectorCreate(0, size-1);
  memcpy(out->vector, data, size*sizeof(double));
  return(out);
  
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorReadBinaryFile(char *fileName)
{
  DoubleVector *out;
  FILE *file_p = fopen(fileName, "rb");
  if (file_p == NULL)
  {
    perror("doubleVectorReadBinaryFile(): cannot open file!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return((DoubleVector *) NULL);
#endif
  }
  out = doubleVectorReadBinary(file_p);
  fclose(file_p);
  return(out);
  
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorReadBinary(FILE *file_p)
{
  DoubleVector *out;
  int loLim = 0;
  int upLim = 0;
  
  fread(&loLim, sizeof(int), 1, file_p);
  fread(&upLim, sizeof(int), 1, file_p);

  out = doubleVectorCreate(loLim, upLim);
  if (out == NULL)
  {
    perror("doubleVectorReadBinary(): problem in allocating out");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return((DoubleVector *) NULL);
#endif
  }
  if (fread(out->vector+loLim, sizeof(double), out->size, file_p) != out->size)
  {
    perror("doubleVectorReadBinary(): problem in reading file");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return((DoubleVector *) NULL);
#endif
  }
    
    
  return(out);
  
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int     doubleVectorInfo(DoubleVector * vec_p)
{
  printf("XMIN: %d\n",vec_p->loLim);
  printf("XMAX: %d\n",vec_p->upLim);
  printf("SIZE: %d\n",vec_p->size);
  return(RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorLineFit(
			      DoubleVector * vector, 
			      double *a0, double *a1,
			      double *chi)
{
  double n=0.0;
  int i;
  double x,y;
  double sum_x_square = 0.0;
  double sum_x        = 0.0;
  double sum_y        = 0.0;
  double sum_x_times_y  = 0.0;
  double denom;
  
  int size = vector->size;
  int loLim = vector->loLim;
  int upLim = vector->upLim;
  double *vec = vector->vector;

  for(i = loLim; i<=upLim; i++)
    { 
      x = (double) i;
      y = VECTOR(vec,i);

      sum_x_square  += x * x;
      sum_x         += x;
      sum_y         += y;
      sum_x_times_y   += x * y;
    }
  n = (double) size;

  denom=(n*sum_x_square)-(sum_x*sum_x);
  *a0 = ((sum_x_square*sum_y)-(sum_x*sum_x_times_y))/denom;
  *a1 = ((n*sum_x_times_y)-(sum_x*sum_y))/denom;

  *chi = 0.0;
  for(i = loLim; i<=upLim; i++)
    { 
      x = (double) i;
      y = VECTOR(vec,i);
      *chi += (y - *a0 - (*a1 * x)) * (y - *a0 - (*a1 * x));
    }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleVector *doubleVectorTriple(DoubleVector * vec_p)
{
  DoubleVector *newVector;
  double *newVec;
  int newLoLim;
  int newUpLim;
  double *vec = vec_p->vector;
  int size = vec_p->size;
  int loLim = vec_p->loLim;
  int upLim = vec_p->upLim;
  newLoLim=loLim-size;
  newUpLim=upLim+size;
  newVector = doubleVectorCreate(newLoLim,newUpLim);
  newVec = newVector->vector;
  {
    int x;
    for (x=loLim;x<=upLim;x++)
      {
	VECTOR(newVec,x)=VECTOR(vec,x);
	VECTOR(newVec,x-size)=VECTOR(vec,x);
	VECTOR(newVec,x+size)=VECTOR(vec,x);
      }
  }
  return(newVector);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorPlusEqual(DoubleVector *left, DoubleVector *right)
{
  int i;
  int loLim = left->loLim;
  int upLim = left->upLim;
  double *lVector = left->vector;
  double *rVector = right->vector;
  
  if ((loLim != right->loLim) || (upLim != right->upLim))
  {
    perror("doubleVectorPlusEqual(): dimensions of left and right members disagree!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif
  }

  for (i=loLim; i<=upLim; i++)
  {
    VECTOR(lVector, i) += VECTOR(rVector, i);
  }

  return(RETURN_SUCCESS);
  
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorMinusEqual(DoubleVector *left, DoubleVector *right)
{
  int i;
  int loLim = left->loLim;
  int upLim = left->upLim;
  double *lVector = left->vector;
  double *rVector = right->vector;
  
  if ((loLim != right->loLim) || (upLim != right->upLim))
  {
    perror("doubleVectorPlusEqual(): dimensions of left and right members disagree!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif
  }

  for (i=loLim; i<=upLim; i++)
  {
    VECTOR(lVector, i) -= VECTOR(rVector, i);
  }
  return(RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorPointMultEqual(DoubleVector *left, DoubleVector *right)
{
  int i;
  int loLim = left->loLim;
  int upLim = left->upLim;
  double *lVector = left->vector;
  double *rVector = right->vector;
  
  if ((loLim != right->loLim) || (upLim != right->upLim))
  {
    perror("doubleVectorPointMultEqual(): dimensions of left and right members disagree!");
#ifndef RETURN_AT_ERROR
    exit (EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif

  }

  for (i=loLim; i<=upLim; i++)
  {
    VECTOR(lVector, i) *= VECTOR(rVector, i);
  }
  return(RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorDivideScalarEqual(DoubleVector *left, double right)
{
  int i;
  int loLim = left->loLim;
  int upLim = left->upLim;
  double *lVector = left->vector;
  

  for (i=loLim; i<=upLim; i++)
  {
    VECTOR(lVector, i) /= right;
  }
  return(RETURN_SUCCESS);

}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/


int doubleVectorScale(DoubleVector *in, double scale)
{
  int x=0;
  int loLim = in->loLim;
  int upLim = in->upLim;
  double *vec = in->vector;

  for (x=loLim; x<=upLim; x++)
    VECTOR(vec, x) *= scale;

  return(RETURN_SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/


#ifdef DB_TEST
int main()
{
   DoubleVector *V = doubleVectorCreate(-10,10);
   {
      double *vec = V->vector;
      int loLim = V->loLim;
      int upLim = V->upLim;
      int x;
      for (x=loLim;x<=upLim;x++)
      {
	 VECTOR(vec,x) = (double)rand();
      }
   }
   doubleVectorGNUPLOT(V);
   doubleVectorDestroy(V);
   return(RETURN_SUCCESS);
}
#endif


#ifdef DB_TEST2
int main()
{
   DoubleVector *V = doubleVectorCreate(-10,10);
   {
      double *vec = V->vector;
      int loLim = V->loLim;
      int upLim = V->upLim;
      int x;
      for (x=loLim;x<=upLim;x++)
      {
	 VECTOR(vec,x) = (double)rand();
      }
   }
   doubleVectorWriteAscii(V, stdout);
   printf("\n");
   doubleVectorWriteBinaryFile(V, "glop");

   {
     DoubleVector *gg = doubleVectorReadBinaryFile("glop");
     doubleVectorWriteAscii(gg, stdout);
     doubleVectorDestroy(gg);
   }
   
   doubleVectorDestroy(V);
   return(EXIT_SUCCESS);
}
#endif

/*-----------------------------------------------------------------------------
END
-----------------------------------------------------------------------------*/
