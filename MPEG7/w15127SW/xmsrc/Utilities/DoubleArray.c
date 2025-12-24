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
 
 Applicable File Name:  DoubleArray.c

-------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "DoubleVector.h"
#include "DoubleArray.h"


#undef  CONT_ARRAY
#define CONT_ARRAY 1

#ifndef RETURN_SUCCESS
#define RETURN_SUCCESS 0
#endif

#ifndef RETURN_FAILURE
#define RETURN_FAILURE -1
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
//double rint(double x);

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayCreate(
  int loLimX,
  int upLimX,
  int loLimY,
  int upLimY)
{
  DoubleArray *A;
  int sizeX = upLimX - loLimX + 1;
  int sizeY = upLimY - loLimY + 1;
  /* fprintf(stderr, "%d %d %d %d\n", loLimX, upLimX, loLimY, upLimY);
  * fflush(stderr); */
  A = (DoubleArray *) malloc(sizeof(DoubleArray));
  if (A == (DoubleArray *) NULL)
  {
    perror("doubleArrayCreate(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *)NULL);
#endif    
  }
  
  A->loLimX = loLimX;
  A->upLimX = upLimX;
  A->loLimY = loLimY;
  A->upLimY = upLimY;
  A->sizeX = upLimX - loLimX + 1;
  A->sizeY = upLimY - loLimY + 1;
  
  /*----- y -----*/
  A->array = (double **) malloc(sizeY * sizeof(int));
  if (A->array == (double **) NULL)
  {
    perror("doubleArrayCreate(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *)NULL);
#endif    
  }
  
  A->array = A->array - loLimY;
#ifdef CONT_ARRAY
  {
    double *arr = (double *) malloc(sizeX * sizeY * sizeof(double));
    if (arr == (double *) NULL)
    {
      perror("doubleArrayCreate(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *)NULL);
#endif    
    }
    /*----- This array can be accessed as a continuous block! -----*/
    A->block = arr;
    
    /*----- x -----*/
    {
      int y;
      for (y = loLimY; y <= upLimY; y++)
      {
	A->array[y] = arr + (y - loLimY) * sizeX;
	A->array[y] = A->array[y] - loLimX;
      }
    }
  }
#else
  /*----- x -----*/
  {
    int y;
    for (y = loLimY; y <= upLimY; y++)
    {
      A->array[y] = (double *) malloc(sizeX * sizeof(double));
      if (A->array[y] == (double *) NULL)
      {
	perror("doubleArrayCreate(): malloc() fails!");
        fprintf(stderr,"Attempt failed to malloc() %d x %d = %d bytes\n",
          sizeX, sizeof(double), sizeX * sizeof(double));
#ifndef RETURN_AT_ERROR
        exit(EXIT_FAILURE);
#else
        return((DoubleArray *)NULL);
#endif    
      }
      A->array[y] = A->array[y] - loLimX;
    }
  }
  /*----- This array can not be considered as a continuous block! -----*/
  A->block = (double *) NULL;
#endif
  return (A);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayInit(DoubleArray * A)
{
  double **arr = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  int x;
  int y;
  for (y = loLimY; y <= upLimY; y++)
  {
    for (x = loLimX; x <= upLimX; x++)
    {
      ARRAY(arr, x, y) = 0.0;
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayInitLine(DoubleArray * A, int y)
{
  double **arr = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  int x;
  
    for (x = loLimX; x <= upLimX; x++)
    {
      ARRAY(arr, x, y) = 0.0;
    }
  return (RETURN_SUCCESS);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayInitValue(DoubleArray * A, double val)
{
  double **arr = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  int x;
  int y;
  for (y = loLimY; y <= upLimY; y++)
  {
    for (x = loLimX; x <= upLimX; x++)
    {
      ARRAY(arr, x, y) = val;
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayDestroy(DoubleArray * A)
{
  if(A==(DoubleArray *)NULL)
  {
    perror("doubleArrayDestroy(): pointer to array is NULL!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
#ifdef CONT_ARRAY
  {
    /*
    int y;
    */
    
    double **arr = A->array;
    int loLimX = A->loLimX;
    int loLimY = A->loLimY;
    /*
    int upLimY = A->upLimY;
    */
    free(&ARRAY(arr, loLimX, loLimY));
    free(arr + loLimY);
    free(A);
    A = (DoubleArray *) NULL;
    return (RETURN_SUCCESS);
  }
#else
  {
    int y;
    
    double **arr = A->array;
    if (A != (DoubleArray *) NULL)
    {
      int loLimX = A->loLimX;
      int loLimY = A->loLimY;
      int upLimY = A->upLimY;
      for (y = loLimY; y <= upLimY; y++)
      {
        free(arr[y] + loLimX);
      }
      free(arr + loLimY);
      free(A);
      A = (DoubleArray *) NULL;
    }
    return (RETURN_SUCCESS);
  }
#endif
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayDestroy2Arg(void *link, void *parameter)
{

  if (link != (void *)NULL)
    {
      return (doubleArrayDestroy((DoubleArray *)link));
    }
  else
    return (RETURN_FAILURE);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

DoubleArray *doubleArraySubArray(
				 DoubleArray *A,
				 int loSubX,
				 int upSubX,
				 int loSubY,
				 int upSubY)
{
  double **arr_A = A->array;
  double **arr_B;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  DoubleArray *B;
  int x;
  int y;

  if (loSubX < loLimX || upSubX > upLimX 
      || loSubY < loLimY || upSubY > upLimY)

  {
    perror("doubleArraySubArray(): Array size is not correct !");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *)NULL);
#endif    
  }
  B = doubleArrayCreate(loSubX, upSubX, loSubY, upSubY);
  arr_B = B->array;
  
  for (y = loSubY; y <= upSubY; y++)
  {
    for (x = loSubX; x <= upSubX; x++)
    {
	{
	  ARRAY(arr_B, x, y) =  ARRAY(arr_A, x, y);
	}
    }
  }
  
  return(B);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayShow(DoubleArray * A)
{
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  double max;
  double min;
  
  if (doubleArrayMaxMin(A,&max,&min) != RETURN_SUCCESS)
  {
    perror("doubleArrayShow(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif        
  }
  
  fprintf(stderr,"===== DoubleArray =====\n");
  fprintf(stderr,"minX: %d\n",minX);
  fprintf(stderr,"maxX: %d\n",maxX);
  fprintf(stderr,"minY: %d\n",minY);
  fprintf(stderr,"maxY: %d\n",maxY);
  fprintf(stderr,"max: %24.24f\n",max);
  fprintf(stderr,"min: %24.24f\n",min);
  fprintf(stderr,"arr: %p\n",a);
  return (RETURN_SUCCESS);
}  
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayPrint(DoubleArray * A)
{
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  int x, y;
  
  for (x=minX; x<=maxX; x++)
  {
    for (y=minY; y<=maxY; y++)
      printf("%f  ", ARRAY(a, x, y));
    printf("\n");
  }
  return (RETURN_SUCCESS);

}  
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
http://www.teleport.com/~sabre/graphpro/

PROCEDURE FixLine(X1, Y1, X2, Y2 : INTEGER; Color : BYTE); 
VAR 
DeltaX, DeltaY,     { Change in X and Y   } 
Y     : INTEGER;    { Current position    } 
X, Slope : LONGINT;  
BEGIN 
DeltaX := X2-X1+1; DeltaY := Y2-Y1+1; 
Slope := (LONGINT(DeltaX) SHL 16) DIV DeltaY; 

X := LONGINT(X1) SHL 16; 
FOR Y := Y1 TO Y2 DO 
BEGIN 
SetPixel(X SHR 16, Y, Color); 
X := X + Slope; 
END; 
END;
-----------------------------------------------------------------------------*/
int doubleArrayDrawLine2(DoubleArray * A, 
                          int x1, int y1, int x2, int y2, double c)
{
  /*-----  -----*/
  int deltaX;
  int deltaY;
  int x;
  int y;
  int slope;
  
  /*-----  -----*/
  double **a = A->array;
  
  deltaX = x2-x1+1; 
  deltaY = y2-y1+1; 
  
  slope = (deltaX << 16) / deltaY; 
  
  x = x1 << 16; 
  for (y = y1; y1 <= y2; y++) 
  {
    ARRAY(a, (x << 16), y) = c;
    x += slope; 
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayDrawLine(DoubleArray * A, 
                         int x1, int y1, int x2, int y2, double c)
{
  /*-----  -----*/
  double **a = A->array;
  
  int minX = A->loLimX;
  int minY = A->loLimY;
  int maxX = A->upLimX;
  int maxY = A->upLimY;
  
  /*-----  -----*/
  int runcount;
  int dx;
  int dy;
  int xinc = 0;
  int yinc = 0;
  unsigned int xplot;
  unsigned int yplot;
  runcount = 0;
  dx = abs((int) (x1) - (int) (x2));
  if (x2 > x1)
  {
    xinc = 1;
  }
  if (x2 == x1)
  {
    xinc = 0;
  }
  if (x2 < x1)
  {
    xinc = -1;
  }
  dy = abs((int) (y1) - (int) (y2));
  if (y2 > y1)
  {
    yinc = 1;
  }
  if (y2 == y1)
  {
    yinc = 0;
  }
  if (y2 < y1)
  {
    yinc = -1;
  }
  xplot = x1;
  yplot = y1;
  if (dx > dy)
  {
    /*----- iterate x -----*/
    if((
      (xplot >= minX) && (xplot <= maxX) && 
      (yplot >= minY) && (yplot <= maxY)
      ))
    {
      ARRAY(a, xplot, yplot) = c;
    }
    while (xplot != x2)
    {
      xplot += xinc;
      runcount += dy;
      if (runcount >= (dx - runcount))
      {
	yplot += yinc;
	runcount -= dx;
      }
      /*-----  -----*/
      if((
        (xplot >= minX) && (xplot <= maxX) && 
        (yplot >= minY) && (yplot <= maxY)
        ))
      {
        ARRAY(a, xplot, yplot) = c;
      }
    }
  }
  else
  {
    /*----- iterate y -----*/    
    if((
      (xplot >= minX) && (xplot <= maxX) && 
      (yplot >= minY) && (yplot <= maxY)
      ))
    {
      ARRAY(a, xplot, yplot) = c;
    }
    while (yplot != y2)
    {
      yplot += yinc;
      runcount += dx;
      if (runcount >= (dy - runcount))
      {
        xplot += xinc;
        runcount -= dy;
      }
      /*-----  -----*/
      if((
        (xplot >= minX) && (xplot <= maxX) && 
        (yplot >= minY) && (yplot <= maxY)
        ))
      {
        ARRAY(a, xplot, yplot) = c;
      }
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int dAwritePGM(FILE * file_p, DoubleArray * A)
{
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  int sizeX = A->sizeX;
  int sizeY = A->sizeY;
  
  double max;
  double min;
  
  if (doubleArrayMaxMin(A, &max, &min) != RETURN_SUCCESS)
    {
      perror("dAwritePGM(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    

    }
  
  /*----- Write Raw PGM image -----*/
  {
    double dm = max - min;
    if (dm != 0.0)
    {
      int x;
      int y;
      fprintf(file_p, "P5\n");
      fprintf(file_p, "%d %d\n", sizeY, sizeX);
      fprintf(file_p, "255\n");	
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  char c = (char)
            (int)
            (255.0 * (ARRAY(a, x, y) - min) / (dm));
	  fwrite(&c, 1, 1, file_p);
	}
      }
    }
    else
    {
      char c = 0;
      int size = sizeX*sizeY;
      int n;
      fprintf(file_p, "P5\n");
      fprintf(file_p, "%d %d\n", sizeY, sizeX);
      fprintf(file_p, "1\n");	
      for (n = 1; n <= size; n++)
      {
	fwrite(&c, 1, 1, file_p);
      }
    }      
  }
  
  /*
  startTime = time(NULL);
  endTime = time(NULL);
  
  fprintf(stderr, "doubleArrayWritePGM(): %lu s.\n",
  endTime - startTime);
  fflush(stderr);
  */
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int dAwritePPM(FILE * file_p, 
	       DoubleArray * R, 
	       DoubleArray * G, 
	       DoubleArray * B)
{
  double **r = R->array;
  double **g = G->array;
  double **b = B->array;
  int loLimX = R->loLimX;
  int upLimX = R->upLimX;
  int loLimY = R->loLimY;
  int upLimY = R->upLimY;
  
  int sizeX = R->sizeX;
  int sizeY = R->sizeY;
  
  double rmax;
  double rmin;
  double gmax;
  double gmin;
  double bmax;
  double bmin;
  
  double min;
  double max;
  
  if (doubleArrayMaxMin(R, &rmax, &rmin) != RETURN_SUCCESS)
  {
    perror("doubleArrayXdisplay(): problem in doubleArrayMaxMin for R");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  if (doubleArrayMaxMin(G, &gmax, &gmin) != RETURN_SUCCESS)
  {
    perror("doubleArrayXdisplay(): problem in doubleArrayMaxMin for G");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  if (doubleArrayMaxMin(B, &bmax, &bmin) != RETURN_SUCCESS)
  {
    perror("doubleArrayXdisplay(): problem in doubleArrayMaxMin for B");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }

#define MAX(a,b) ((a)>(b)?(a):(b)) 
  max = MAX(rmax,gmax);
  max = MAX(max,bmax);
#undef MAX
  
#define MIN(a,b) ((a)<(b)?(a):(b)) 
  min = MIN(rmin,gmin);
  min = MIN(min,bmin);
#undef MIN  
  /*----- Write Raw PGM image -----*/
  {
    double dm = max - min;
    if (dm != 0.0)
    {
      int x;
      int y;
      fprintf(file_p, "P6\n");
      fprintf(file_p, "%d %d\n", sizeY, sizeX);
      fprintf(file_p, "255\n");	
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  char c[3];
          
	  c[0] = (char) (int) (255.0 * (ARRAY(r, x, y) - min) / (dm));
	  c[1] = (char) (int) (255.0 * (ARRAY(g, x, y) - min) / (dm));
	  c[2] = (char) (int) (255.0 * (ARRAY(b, x, y) - min) / (dm));
          
	  fwrite(c, 3, 1, file_p);
	}
      }
    }
    else
    {
      char c = 0;
      int size = 3*sizeX*sizeY;
      int n;
      fprintf(file_p, "P6\n");
      fprintf(file_p, "%d %d\n", sizeY, sizeX);
      fprintf(file_p, "1\n");	
      for (n = 1; n <= size; n++)
      {
	fwrite(&c, 1, 1, file_p);
      }
    }      
  }
  
  /*
  startTime = time(NULL);
  endTime = time(NULL);
  
  fprintf(stderr, "doubleArrayWritePGM(): %lu s.\n",
  endTime - startTime);
  fflush(stderr);
  */
  return (RETURN_SUCCESS);
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWriteASCII(DoubleArray *A, char *fileName)
{
  FILE *file_p;
  file_p = fopen(fileName, "wb");
  if (file_p == (FILE *) NULL)
  {
    perror("doubleArrayWriteASCII(): fopen() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  /*-----  -----*/
  {
    double **a = A->array;
    int loLimX = A->loLimX;
    int upLimX = A->upLimX;
    int loLimY = A->loLimY;
    int upLimY = A->upLimY;
    
    int sizeX = A->sizeX;
    
    /*-----  -----*/
    {
      int x;
      int y;
      fprintf(file_p, "%d\n", sizeX);
      
      for (y = loLimY; y <= upLimY; y++)
      {
	for (x = loLimX; x <= upLimX; x++)
	{
	  fprintf(file_p,"%f ",ARRAY(a,x,y));
	}
	fprintf(file_p,"\n");
      }
    }
  }
  
  fclose(file_p);
  return (RETURN_SUCCESS);
}




/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

DoubleArray *doubleArrayReadASCII(
  char *fileName,
  int loLimX,
  int upLimX,
  int loLimY,
  int upLimY
  )
{

  int sizeX;
  int sizeY;
  
  DoubleArray *A;
  double **a;
  {
    FILE *file_p;
    file_p = fopen(fileName, "rb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadRaw(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    
    }
    A = doubleArrayCreate(loLimX, upLimX, loLimY, upLimY);
    if (A == (DoubleArray *)NULL)
    {
      perror("doubleArrayRead(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    

    }

    a = A->array;
    sizeX = A->sizeX;
    sizeY = A->sizeY;
    {
      int x;
      int y;
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  float f;
	  char s[256];
	  fgets(s, 256, file_p);
	  sscanf(s, "%f", &f);
	  ARRAY(a, x, y) = (double) f;
	}
      }
    }
    
    fclose(file_p);
  }
  return (A);
} 



/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWritePGM(char *fileName, DoubleArray *A)
{
  FILE *file_p;
  file_p = fopen(fileName, "wb");
  if (file_p == (FILE *) NULL)
  {
    perror("doubleArrayWritePGM(): fopen() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  dAwritePGM(file_p, A);
  fclose(file_p);
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWritePPM(char *fileName, 
			 DoubleArray *R,
			 DoubleArray *G,
			 DoubleArray *B)
{
  FILE *file_p;
  file_p = fopen(fileName, "wb");
  if (file_p == (FILE *) NULL)
  {
    perror("doubleArrayWritePGM(): fopen() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  dAwritePPM(file_p, R, G, B);
  fclose(file_p);
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

#ifdef HAS_PGM_LIB
DoubleArray *doubleArrayReadPGM(char *fileName)
{
  int pid;
  time_t startTime = time(NULL);
  time_t endTime;
  
  int loLimX;
  int upLimX;
  int loLimY;
  int upLimY;
  DoubleArray *A;
  double **a;
  int sizeX;
  int sizeY;
  {
    gray **image;
    gray maxvalP;
    FILE *file_p;
    file_p = fopen(fileName, "rb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadPGM(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    
    }
    
    /*----- PBMPLUS library function -----*/
    image = pgm_readpgm(file_p, &sizeX, &sizeY, &maxvalP);
    
    doubleArray = doubleArrayCreate(1, sizeX, 1, sizeY);
    if (doubleArray == (DoubleArray *)NULL)
    {
      perror("doubleArrayRead(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    

    }    
    a = A->array;
    sizeX = A->sizeX;
    sizeY = A->sizeY;
    loLimX = A->loLimX;
    upLimX = A->upLimX;
    loLimY = A->loLimY;
    upLimY = A->upLimY;
    {
      int x;
      int y;
      int n = 0;
      int m = 0;
      for (x = loLimX; x <= upLimX; x++)
      {
	m = 0;
	for (y = loLimY; y <= upLimY; y++)
	{
	  ARRAY(a, x, y) = ARRAY(image, m, n);
	  m++;
	}
	n++;
      }
    }
    
    fclose(file_p);
    /*----- PBMPLUS library function -----*/
    pm_freearray(image, sizeY);
  }
  endTime = time(NULL);
  fprintf(stderr, "doubleArrayRead(): \"%s\" %lu s.\n",
	  fileName,
	  endTime - startTime);
  fflush(stderr);
  return (A);
}
#endif				/* HAS_PGM_LIB */

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadDoubleRaw(
  char *fileName,
  int loLimX,
  int upLimX,
  int loLimY,
  int upLimY
  )
{
  int sizeX;
  int sizeY;
  
  DoubleArray *A;
  double **a;
  {
    FILE *file_p;
    file_p = fopen(fileName, "rb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadRaw(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    
    }
    A = doubleArrayCreate(loLimX, upLimX, loLimY, upLimY);
    if (A == (DoubleArray *)NULL)
    {
      perror("doubleArrayRead(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    

    }

    a = A->array;
    sizeX = A->sizeX;
    sizeY = A->sizeY;
    {
      int x;
      int y;
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  float c;
	  /*fread(&c, sizeof(double), 1, file_p);*/
	  fscanf(file_p,"%f",&c);
	  ARRAY(a, x, y) =  (double) c;
	}
      }
    }
    
    fclose(file_p);
  }
  return (A);
}



/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadRaw(
  char *fileName,
  int loLimX,
  int upLimX,
  int loLimY,
  int upLimY
  )
{
  int sizeX;
  int sizeY;
  
  DoubleArray *A;
  double **a;
  {
    FILE *file_p;
    file_p = fopen(fileName, "rb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadRaw(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    
    }
    A = doubleArrayCreate(loLimX, upLimX, loLimY, upLimY);
    if (A == (DoubleArray *)NULL)
    {
      perror("doubleArrayRead(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    

    }

    a = A->array;
    sizeX = A->sizeX;
    sizeY = A->sizeY;
    {
      int x;
      int y;
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  unsigned char c;
	  fread(&c, 1, 1, file_p);
	  ARRAY(a, x, y) = (double) c;
	}
      }
    }
    
    fclose(file_p);
  }
  return (A);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWriteRaw(DoubleArray *A, char *fileName)
{
  int sizeX;
  int sizeY;
  
  
  double **a;
  {
    int loLimX = A->loLimX;
    int loLimY = A->loLimY;
    int upLimX = A->upLimX;
    int upLimY = A->upLimY;
    
    FILE *file_p;
    file_p = fopen(fileName, "wb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadRaw(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit (EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif
    }
    a = A->array;
    sizeX = A->sizeX;
    sizeY = A->sizeY;
    {
      int x;
      int y;
      for (x = loLimX; x <= upLimX; x++)
      {
	for (y = loLimY; y <= upLimY; y++)
	{
	  double tmp = ARRAY(a, x, y);
	  fwrite(&tmp, sizeof(double), 1, file_p);
          
	}
      }
    }
    
    fclose(file_p);
  }
  return(RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
This function reads raw PGM files -- and nothing else. I don't
understand why someone might want to use this function. The PBMPLUS
package or the NETPBM package have much more sophisticated functions
for reading and writing PBM, PGM and PPM.

- Almost no error checking is done.
- The maximum number of gray levels is not used.
- The treatment of comments in the PGM file is quite clumsy.
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadRAWPGM(char *fileName)
{
  /*-----  -----*/
  FILE *file_p;
  
  /*-----  -----*/
  DoubleArray *A;
  
  /*-----  -----*/
  int minX;
  int maxX;
  int minY;
  int maxY;
  
  int maxVal;
  
  /*----- Open File -----*/
  file_p = fopen(fileName, "rb");
  if (file_p == (FILE *) NULL)
  {
    char msg[255];
    sprintf(msg, "doubleArrayReadRAWPGM(%s): fopen() fails!", fileName);
    perror(msg);
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  /*----- Magic number -----*/
  {
    char s[512];
    fgets(s, 512, file_p);
    if(strncmp(s,"P5",2)!=0)
    {
      fprintf(stderr,"doubleArrayReadRAWPGM(): Wrong magic number!\n");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    
    }
  }
  
  /*----- Dimensions -----*/
  {
    char s[512];
    do
    {
      fgets(s, 512, file_p);
    }
    while(s[0] == '#');
    sscanf(s,"%d%d",&maxY,&maxX);
  }
  /*
  fprintf(stderr,"%d %d\n",maxY,maxX);
  */
  
  /*----- Largest gray value -----*/
  {
    char s[512];
    do
    {
      fgets(s, 512, file_p);
    }
    while(s[0] == '#');
    sscanf(s,"%d",&maxVal); /* This value is read but ignored */
  }
  
  /*----- Read the rest -----*/
  {
    double **a;
    /*    A = doubleArrayCreate(1, maxX, 1, maxY);
    */
    A = doubleArrayCreate(0, maxX-1, 0, maxY-1);
    if (A == (DoubleArray *)NULL)
    {
      perror("doubleArrayReadRAWPGM(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *) NULL);
#endif    

    }

    a = A->array;
    
    minX = A->loLimX;
    minY = A->loLimY;
    maxX = A->upLimX;
    maxY = A->upLimY;
    {
      int x;
      int y;
      
      for (x = minX; x <= maxX; x++)
      {
	for (y = minY; y <= maxY; y++)
	{
	  unsigned char c;
	  int num;
	  num = fread(&c, 1, 1, file_p);
#ifdef DEBUG_FREAD
	  if(num != 1)
	  {
            perror("doubleArrayReadRAWPGM(): fread() fails!");
#ifndef RETURN_AT_ERROR
            exit(EXIT_FAILURE);
#else
            return((DoubleArray *) NULL);
#endif    
	  }
#endif
	  ARRAY(a, x, y) = (double) c;
	}
      }
    }
  }
  
  /*-----  -----*/
  fclose(file_p);
  
  return (A);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayReadRAWPPM(char *fileName, 
			   DoubleArray **RR,
			   DoubleArray **GG,
			   DoubleArray **BB)
{
  int sizeX;
  int sizeY;
  
  int minX;
  int maxX;
  int minY;
  int maxY;
  
  int maxVal;
  
  DoubleArray *R;
  DoubleArray *G;
  DoubleArray *B;
  
  double **r;
  double **g;
  double **b;
  
  FILE *file_p;
  
  /*----- Open File -----*/
  file_p = fopen(fileName, "rb");
  if (file_p == (FILE *) NULL)
  {
    perror("doubleArrayReadRAWPPM(): fopen() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  /*----- Magic number -----*/
  {
    char s[512];
    fgets(s, 512, file_p);
    if(strncmp(s,"P6",2)!=0)
    {
      fprintf(stderr,"doubleArrayReadRAWPPM(): Wrong magic number!\n");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
    }
  }
  
  /*----- Dimensions -----*/
  {
    char s[512];
    do
    {
      fgets(s, 512, file_p);
    }
    while(s[0] == '#');
    sscanf(s,"%d%d",&maxY,&maxX);
  }
  
  /*----- Largest gray value -----*/
  {
    char s[512];
    do
    {
      fgets(s, 512, file_p);
    }
    while(s[0] == '#');
    sscanf(s,"%d",&maxVal); /* This value is read but ignored */
  }
  
  /*----- Read the rest -----*/
  {
    R = doubleArrayCreate(1, maxX, 1, maxY);
    if (R == (DoubleArray *)NULL)
    {
      perror("doubleArrayReadRAWPPM(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    

    }
    G = doubleArrayCreate(1, maxX, 1, maxY);
    if (G == (DoubleArray *)NULL)
    {
      perror("doubleArrayReadRAWPPM(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }

    B = doubleArrayCreate(1, maxX, 1, maxY);
    if (B == (DoubleArray *)NULL)
    {
      perror("doubleArrayReadRAWPPM(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    

    }

    r = R->array;
    g = G->array;
    b = B->array;
    
    sizeX = R->sizeX;
    sizeY = R->sizeY;
    minX = R->loLimX;
    minY = R->loLimY;
    maxX = R->upLimX;
    maxY = R->upLimY;
    
    /*-----  -----*/
    {
      int x;
      int y;
      
      for (x = minX; x <= maxX; x++)
      {
	for (y = minY; y <= maxY; y++)
	{
	  unsigned char c[3];
	  fread(c, 3, 1, file_p);
	  ARRAY(r, x, y) = (double) c[0];
	  ARRAY(g, x, y) = (double) c[1];
	  ARRAY(b, x, y) = (double) c[2];
	}
      }
    }
  }
  
  /*-----  -----*/
  *RR = R;
  *GG = G;
  *BB = B;
  
  /*-----  -----*/
  fclose(file_p);
  
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadChar(
  char *fileName,
  int minX,
  int maxX,
  int minY,
  int maxY)
{
  time_t startTime = time(NULL);
  time_t endTime;
  int sizeX;
  int sizeY;
  
  DoubleArray *doubleArray;
  double **arr;
  doubleArray = doubleArrayCreate(minX, maxX, minY, maxY);
  if (doubleArray == (DoubleArray *)NULL)
  {
    perror("doubleArrayReadChar(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
    
  }

  sizeX = doubleArray->sizeX;
  sizeY = doubleArray->sizeY;
  {
    FILE *file_p;
    file_p = fopen(fileName, "rb");
    if (file_p == (FILE *) NULL)
    {
      perror("doubleArrayReadChar(): fopen() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return((DoubleArray *)NULL);
#endif    
    }
    
    arr = doubleArray->array;
    {
      int x;
      int y;
      for (x = minX; x <= maxX; x++)
      {
	for (y = minY; y <= maxY; y++)
	{
	  char c;
	  fread(&c, 1, 1, file_p);
	  ARRAY(arr, x, y) = (double) (int) c;
	}
      }
    }
    
    fclose(file_p);
  }
  endTime = time(NULL);
  fprintf(stderr, "doubleArrayReadChar(): \"%s\" %lu s.\n",
	  fileName,
	  endTime - startTime);
  fflush(stderr);
  return (doubleArray);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayMaxMin(DoubleArray * A, double *max, double *min)
{
  int x;
  int y;
  
  double value;
  
  double **arr = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  *max = ARRAY(arr, minX, minY);
  *min = ARRAY(arr, minX, minY);
  
  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      value = ARRAY(arr, x, y);
      if (value > *max)
      {
	*max = value;
      }
      if (value < *min)
      {
	*min = value;
      }
    }
  }
  return (RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayCopy(DoubleArray * dest, DoubleArray * source)
{
  double **des = dest->array;
  double **src = source->array;
  int dMinX = dest->loLimX;
  int dMaxX = dest->upLimX;
  int dMinY = dest->loLimY;
  int dMaxY = dest->upLimY;
  
  int sMinX = source->loLimX;
  int sMinY = source->loLimY;
  
  int x;
  int y;
  
  int xs;
  int ys;
  for (y = dMinY, ys = sMinY; y <= dMaxY; y++, ys++)
  {
    for (x = dMinX, xs = sMinX; x <= dMaxX; x++, xs++)
    {
      ARRAY(des, x, y) = ARRAY(src, xs, ys);
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayCopy90(DoubleArray * A)
{
  int x;
  int y;
  
  double **arr = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  DoubleArray *B = doubleArrayCreate(loLimY, upLimY, loLimX, upLimX);
  double **newarray = B->array;
  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayCopy90(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(newarray, y, x) = ARRAY(arr, x, y);
    }
  }
  return (B);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayRot90(DoubleArray * A)
{
  int x;
  int y;
  
  double **arr = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  DoubleArray *B = doubleArrayCreate(loLimY, upLimY, loLimX, upLimX);
  double **newarray = B->array;
  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayCopy90(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }


  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(newarray, y, x) = ARRAY(arr, x, y);
    }
  }
#ifdef CONT_ARRAY
  {
    /* 
    int y;
    */
    
    double **arr = A->array;
    int loLimX = A->loLimX;
    int loLimY = A->loLimY;
    /* 
    int upLimY = A->upLimY;
    */
    free(&ARRAY(arr, loLimX, loLimY));
    free(arr + loLimY);
  }
#else
  {
    int y;
    
    double **arr = A->array;
    if (A != (DoubleArray *) NULL)
    {
      int loLimX = A->loLimX;
      int loLimY = A->loLimY;
      int upLimY = A->upLimY;
      for (y = loLimY; y <= upLimY; y++)
      {
	free(arr[y] + loLimX);
      }
      free(arr + loLimY);
    }
  }
#endif
  A->loLimX = B->loLimX;
  A->loLimY = B->loLimY;
  A->upLimX = B->upLimX;
  A->upLimY = B->upLimY;
  A->sizeX = B->sizeX;
  A->sizeY = B->sizeY;
  A->array = B->array;
  
  free(B);
  return (A);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayOrigin(DoubleArray * A, int x0, int y0)
{
  int loLimX = A->loLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  int sizeX = A->sizeX;
  int sizeY = A->sizeY;
  {
    int y;
    for (y = loLimY; y <= upLimY; y++)
    {
      A->array[y] = A->array[y] + loLimX - x0;
    }
  }
  A->array = A->array + loLimY - y0;
  
  A->loLimX = x0;
  A->upLimX = x0 + sizeX - 1;
  A->loLimY = y0;
  A->upLimY = y0 + sizeY - 1;
  return (RETURN_SUCCESS);
}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayNorm(DoubleArray * aa)
{
  int x;
  int y;
  
  double min;
  double max;
  
  double **a = aa->array;
  int loLimX = aa->loLimX;
  int upLimX = aa->upLimX;
  int loLimY = aa->loLimY;
  int upLimY = aa->upLimY;
  if (doubleArrayMaxMin(aa, &max, &min) != RETURN_SUCCESS)
  {
    perror("doubleArrayNorm(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(a, x, y) = (ARRAY(a, x, y) - min) / (max - min);
    }
  }
  return (RETURN_SUCCESS);
}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayMean(DoubleArray * aa)
{
  int x;
  int y;
  
  double mean = 0.0;
  double square = 0.0;
  
  double **a = aa->array;
  int loLimX = aa->loLimX;
  int upLimX = aa->upLimX;
  int loLimY = aa->loLimY;
  int upLimY = aa->upLimY;
  int sizeX = aa->sizeX;
  int sizeY = aa->sizeY;
  
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      double val = ARRAY(a, x, y);
      mean += val;
      square += val*val;
    }
  }
  mean /= (double)(sizeX*sizeY);
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(a, x, y) -= mean;
      ARRAY(a, x, y) /= square;
    }
  }
  return (RETURN_SUCCESS);
}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayNormalize(DoubleArray * aa)
{
  int x;
  int y;
  
  double mean = 0.0;
  double square = 0.0;
  
  double **a = aa->array;
  int loLimX = aa->loLimX;
  int upLimX = aa->upLimX;
  int loLimY = aa->loLimY;
  int upLimY = aa->upLimY;
  int sizeX = aa->sizeX;
  int sizeY = aa->sizeY;
  
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      double val = ARRAY(a, x, y);
      mean += val;
      square += val*val;
    }
  }
  mean /= (double)(sizeX*sizeY);

  square = square/(double)(sizeX*sizeY) - mean*mean;
  square = sqrt(square);
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(a, x, y) -= mean;
      ARRAY(a, x, y) /= square;
    }
  }
  return (RETURN_SUCCESS);
}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayScale(DoubleArray * A, double s)
{
  double **a = A->array;
  
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  int x;
  int y;
  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      ARRAY(a, x, y) *= s;
    }
  }
  return (RETURN_SUCCESS);

}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayOperator(
  DoubleArray * A, 
  DoubleArray * B, 
  DoubleArray * C,
  double (*operation)(double, double)
  )
{
  double **a = A->array;
  double **b = B->array;
  double **c = C->array;
  
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  int x;
  int y;
  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      ARRAY(c, x, y) = operation(ARRAY(a, x, y),ARRAY(b, x, y));
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayDuplicate(DoubleArray * A)
{
  double **b;
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *B;
  B = doubleArrayCreate(minX, maxX, minY, maxY);
  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayDuplicate(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  b = B->array;
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(b, x, y) = ARRAY(a, x, y);
      }
    }
  }
  return (B);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayAdd(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(minX, maxX, minY, maxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayAdd(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  c = C->array;
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(c, x, y) = ARRAY(a, x, y) + ARRAY(b, x, y);
      }
    }
  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArraySub(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(minX, maxX, minY, maxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArraySub(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  c = C->array;
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(c, x, y) = ARRAY(a, x, y) - ARRAY(b, x, y);
      }
    }
  }
  return (C);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayMinusEqual(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  if ((minX != B->loLimX) || (maxX != B->upLimX) || (minY != B->loLimY) || (maxY != B->upLimY))
  {
    perror("doubleArrayMinusEqual(): size of input matrices not compatible");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(a, x, y) -= ARRAY(b, x, y);
      }
    }
  }
  return (RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayPlusEqual(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  if ((minX != B->loLimX) || (maxX != B->upLimX) || (minY != B->loLimY) || (maxY != B->upLimY))
  {
    perror("doubleArrayMinusEqual(): size of input matrices not compatible");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(a, x, y) += ARRAY(b, x, y);
      }
    }
  }
  return (RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayHypot(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(minX, maxX, minY, maxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayHypot(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif      
  }

  c = C->array;
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(c, x, y) = hypot(ARRAY(a, x, y), ARRAY(b, x, y));
      }
    }
  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayAtan2(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(minX, maxX, minY, maxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayAtan2(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif      
  }
  
  c = C->array;
  {
    int x;
    int y;
    for (y = minY; y <= maxY; y++)
    {
      for (x = minX; x <= maxX; x++)
      {
	ARRAY(c, x, y) = atan2(ARRAY(a, x, y), ARRAY(b, x, y));
      }
    }
  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayMultiply(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  
  int aMinX = A->loLimX;
  int aMaxX = A->upLimX;
  int aMinY = A->loLimY;
  int aMaxY = A->upLimY;
  
  int bMinX = B->loLimX;
  int bMaxX = B->upLimX;
  
  int x;
  int y;
  int k;
  
  DoubleArray *C;
  double **c;

  if (B->sizeY != A->sizeX)
  {
    char msg[255];
    sprintf(msg, "doubleArrayMultiply(): A(%d,%d) and B(%d,%d) of uncompatible size!\n", A->sizeX, A->sizeY, B->sizeX, B->sizeY);
    perror(msg);
    exit(EXIT_FAILURE);
  }
  
  if (doubleArrayOrigin(B, B->loLimX, aMinX) != RETURN_SUCCESS)
  {
    perror("doubleArrayMultiply(): problem in doubleArrayOrigin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(NULL);
#endif    
  }
  
  C = doubleArrayCreate(bMinX, bMaxX, aMinY, aMaxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayMultiply(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
    }

  if (doubleArrayInit(C) != RETURN_SUCCESS)
  {
    perror("doubleArrayMultiply(): doubleArray not initialised");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  c = C->array;
  
  for (x = bMinX; x <= bMaxX; x++)
  {
    for (y = aMinY; y <= aMaxY; y++)
    {
      for (k = aMinX; k <= aMaxX; k++)
      {
	ARRAY(c, x, y) += ARRAY(a, k, y) * ARRAY(b, x, k);
      }
    }
  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayMult(DoubleArray * A, DoubleArray * B)
{
  double **a = A->array;
  double **b = B->array;
  
  int aMinX = A->loLimX;
  int aMaxX = A->upLimX;
  int aMinY = A->loLimY;
  int aMaxY = A->upLimY;
  
  int bMinX = B->loLimX;
  int bMaxX = B->upLimX;
  
  int x;
  int y;
  int k;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(bMinX, bMaxX, aMinY, aMaxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayMult(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  if (doubleArrayInit(C) != RETURN_SUCCESS)
  {
    perror("doubleArrayMult(): problem in doubleArrayInit");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  
  c = C->array;
  
  for (x = bMinX; x <= bMaxX; x++)
  {
    for (y = aMinY; y <= aMaxY; y++)
    {
      for (k = aMinX; k <= aMaxX; k++)
      {
        double aa = ARRAY(a, k, y);
	double bb = ARRAY(b, x, k);
	if (aa!=0.0 && bb!=0.0)
	{
	  ARRAY(c, x, y) += aa*bb;
	}       
      }
    }
  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayPower(DoubleArray * A, double m)
{
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *C;
  double **c;
  C = doubleArrayCreate(minX, maxX, minY, maxY);
  if (C == (DoubleArray *)NULL)
  {
    perror("doubleArrayPower(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  c = C->array;
  {
    int x;
    int y;

    /* test only if m <0 */
    if (m<0.0)
    {
      for (y = minY; y <= maxY; y++)
      {
        for (x = minX; x <= maxX; x++)
        {
          double tmp = ARRAY(a, x, y);
          if (tmp >= 1.0e-6)
            ARRAY(c, x, y) = pow(tmp, m);
          else
            ARRAY(c, x, y) = 0.0;
        }
      }
    }
    else
    {
      for (y = minY; y <= maxY; y++)
      {
        for (x = minX; x <= maxX; x++)
        {
          ARRAY(c, x, y) = pow(ARRAY(a, x, y), m);
          
        }
      }
    }

  }
  return (C);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayPointMultEqual(DoubleArray *A, DoubleArray *B)
{
  double **a = A->array;
  double **b = B->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  int x;
  int y;

  if ((B->loLimX != minX) ||(B->upLimX != maxX) || (B->loLimY != minY) ||
    (B->upLimY != maxY))
  {
    perror("doubleArrayPointMultEqual(): dimension or offsets of matrices A and B do not match!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      ARRAY(a, x, y) *= ARRAY(b, x, y);
    }
  }

  return (RETURN_SUCCESS);
    
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayMultScalarEqual(DoubleArray *A, double b)
{
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  int x;
  int y;


  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      ARRAY(a, x, y) *= b;
    }
  }

  return (RETURN_SUCCESS);
 
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayTranspose(DoubleArray * A)
{
  double **a = A->array;
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *AT = doubleArrayCreate(minY, maxY, minX, maxX);
  double **at = AT->array;

  int x;
  int y;

  if (AT == (DoubleArray *)NULL)
  {
    perror("doubleArrayTranspose(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  for (x = minX; x <= maxX; x++)
  {
    for (y = minY; y <= maxY; y++)
    {
      ARRAY(at, y, x) = ARRAY(a, x, y);
    }
  }
  return (AT);
}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayBlur(DoubleArray * A)
{
  DoubleArray *B;
  
  double **a = A->array;
  double **b;
  
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  B = doubleArrayCreate(minX,maxX,minY,maxY);
  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayBlur(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }

  b = B->array;
  
  {
    int x;
    int y;
    int n;
    int m;
    
    /*----- Copy Matrix -----*/
    for (x = minX; x <= maxX; x++)
    {
      for (y = minY; y <= maxY; y++)
      {
        ARRAY(b, x, y) = ARRAY(a, x, y);
      }
    }
    
    /*----- Blur matrix and write result to original matrix -----*/
    for (x = minX + 1; x <= maxX - 1; x++)
    {
      for (y = minY + 1; y <= maxY - 1; y++)
      {
	double s = 0;
	for (n = -1; n <= 1; n++)
	{
	  for (m = -1; m <= 1; m++)
	  {
	    s += ARRAY(b, x + n, y + m);
	  }
	}
	ARRAY(a, x, y) = s / 9.0;
      }
    }
  }
  if (doubleArrayDestroy(B) != RETURN_SUCCESS)
  {
    perror("doubleArrayBlur(): problem in doubleArrayDestroy");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }

  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
Example for usage of "doubleArrayHistogram()":
{
int n;
int buckets = 64;
double histo[64];

doubleArrayHistogram(V1,buckets,histo);
for (n = 1; n <= 64; n++)
{		
  printf("%f\n",histo[n-1]);
}
}
-----------------------------------------------------------------------------*/
int doubleArrayHistogram(DoubleArray *A, int buckets, double *histo)
{
  int x;
  int y;
  int n;
  
  double min;
  double max;
  
  double dm;
  
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  
  double size = (double) (A->sizeX * A->sizeY);
  
  if (doubleArrayMaxMin(A, &max, &min) != RETURN_SUCCESS)
  {
    perror("doubleArrayHistogram(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  for (n = 0; n < buckets; n++)
  {
    histo[n] = 0.0;
  }
  
  dm = (max - min)/((double) buckets);
  
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      for (n = 1; n <= buckets; n++)
      {
	double val = ARRAY(a, x, y);
	if(
          (
	    (val >= min + ((double) (n-1))*dm)
	    &&
	    (val < min + ((double) n)*dm)
	    )
          )
	{
	  histo[n-1] += 1.0;
	}
      }
    }
  }
  
  for (n = 0; n < buckets; n++)
  {
    histo[n] /= size;
  }
  
  return (RETURN_SUCCESS);

}
/*--------------------------------------------------------------------------
--------------------------------------------------------------------------*/
int doubleArrayDiscretize(DoubleArray * A, int buckets)
{
  int x;
  int y;
  
  double min;
  double max;
  
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  if (doubleArrayMaxMin(A, &max, &min) != RETURN_SUCCESS)
  {
    perror("doubleArrayXdisplay(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }  

  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(a, x, y) = (double)(
			(int)(((double)buckets * (ARRAY(a, x, y) - min) /
			(max - min))+0.5))	;
    }
  }
  return (RETURN_SUCCESS);

}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayLocalHistogramEgalisation(
  DoubleArray *A, 
  int buckets,
  int localSize)
{
  int x;
  int y;
  int n;
  
  double* histo;
  double* lut;
  
  double dm;
  
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  DoubleArray *B = doubleArrayCreate(loLimX,upLimX,loLimY,upLimY);
  double **b = B->array;

  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayLocalHistogramEgalisation(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  if (doubleArrayDiscretize(A,buckets) != RETURN_SUCCESS)
  {
    perror("doubleArrayLocalHistogramEgalisation(): problem in doubleArrayDiscretise");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  if (doubleArrayInit(B) != RETURN_SUCCESS)
  {
    perror("doubleArrayLocalHistogramEgalisation(): problem in doubleArrayInit");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  histo = (double *)malloc(buckets*sizeof(double));
  if (histo == (double *) NULL)
  {
    perror("doubleArrayLocalHistogramEgalisation(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  lut = (double *)malloc(buckets*sizeof(double));
  if (lut == (double *) NULL)
  {
    perror("doubleArrayLocalHistogramEgalisation(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  for (x = loLimX; x <= upLimX-localSize; x+=localSize)
  {
    for (y = loLimY; y <= upLimY-localSize; y+=localSize)
    {
      int s;
      int t;
      
      /*----- Calculate local maximum and minimum -----*/
      double max = ARRAY(a, x, y);
      double min = ARRAY(a, x, y);
      
      for (s = x; s < x+localSize; s++)
      {
	for (t = y; t < y+localSize; t++)
	{
	  double value = ARRAY(a, s, t);
	  if (value > max)
	  {
	    max = value;
	  }
	  if (value < min)
	  {
	    min = value;
	  }
	}
      }
      
      /*----- Initialize local histogram -----*/
      for (n = 0; n < buckets; n++)
      {
	histo[n] = 0.0;
      }
      
      /*----- Calculate width of bins -----*/
      dm = (max - min)/((double) buckets);
      
      /*-----  -----*/
      for (s = x; s < x+localSize; s++)
      {
	for (t = y; t < y+localSize; t++)
	{
	  for (n = 1; n <= buckets; n++)
	  {
	    double val = ARRAY(a, s, t);
	    if(
              (
		(val >= min + ((double) (n-1))*dm)
		&&
		(val < min + ((double) n)*dm)
		)
              )
	    {
	      histo[n-1] += 1.0;
	    }
	  }
	}
      }
      
      /*----- Normalize histogram -----*/
      for (n = 0; n < buckets; n++)
      {
	histo[n] /= (double)(localSize*localSize);
      }      
      
      /*----- Calculate cumulative histogram -----*/
      for (n = 1; n < buckets; n++)
      {
	histo[n] += histo[n-1];
      }
      
      /*----- Calculate Lookup Table -----*/
      for (n = 0; n < buckets; n++)
      {
	lut[n] += (double)buckets * histo[n];
      }
      
      /*-----  -----*/
      for (s = x; s < x+localSize; s++)
      {
	for (t = y; t < y+localSize; t++)
	{
	  ARRAY(b, s, t) = lut[(int)ARRAY(a, s, t) - (int)min];
	}
      }
    }
  }
  free(histo);
  free(lut);
  
  return(B);
}
/*-----------------------------------------------------------------------------
the dynamic range of output image is between 0 and buckets-1
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayHistogramEqualization(
  DoubleArray *A, 
  int buckets)
{
  int x;
  int y;
  int n;
  
  double* histo;
  double* lut;
  
  double dm;
  
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  DoubleArray *B = doubleArrayCreate(loLimX,upLimX,loLimY,upLimY);
  double **b = B->array;
  double min = 0.0;
  double max = 0.0;

  double size = (double) (A->sizeX * A->sizeY);

  if (B == (DoubleArray *)NULL)
  {
    perror("doubleArrayHistogramEqualization(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  /* histo allocates one more bin, for the maximal value may fall outside
  histo[buckets-1] */
  histo = (double *)malloc((buckets+1)*sizeof(double));
  if (histo == (double *) NULL)
  {
    perror("doubleArrayHistogramEqualisation(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  /* lut allocates one more bin, for the maximal value may fall outside
  lut[buckets-1] if the pixel value is max */
  lut = (double *)malloc((buckets+1)*sizeof(double));
  if (lut == (double *) NULL)
  {
    perror("doubleArrayHistogramEqualisation(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  if (doubleArrayMaxMin(A, &max, &min) != RETURN_SUCCESS)
  {
    perror("doubleArrayXdisplay(): problem in doubleArrayMaxMin");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }
  
  for (n = 0; n < buckets; n++)
  {
    histo[n] = 0.0;
  }
  
  dm = (max - min)/((double) buckets);
  
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      double val = ARRAY(a, x, y);
      int bin = (int) ((val-min)/dm);
      histo[bin] += 1.0;
    }
  } 
  /* put values outside the last bin in the last bin: this happened for
  val = max */
  histo[buckets-1] += histo[buckets];
  
  for (n = 0; n < buckets; n++)
  {
    histo[n] /= size;
  }

  /*----- Calculate cumulative histogram -----*/
  for (n = 1; n < buckets; n++)
  {
    histo[n] += histo[n-1];
  }

  /*----- Calculate Lookup Table -----*/
  for (n = 0; n < buckets; n++)
  {
    lut[n] += (double)buckets * (histo[n]-histo[0])/(histo[buckets-1]-histo[0]);
  }
  /* put values outside the last bin in the last bin: this happened for
  val = max */
  lut[buckets] = 1.0;
  
  /*-----  -----*/
  for (x = loLimX; x <= upLimX; x++)
  {
    for (y = loLimY; y <= upLimY; y++)
    {
      ARRAY(b, x, y) = lut[(int)((ARRAY(a, x, y) - min)/dm)];
    }
  }

  free(histo);
  free(lut);
  
  return(B);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayGenerate(DoubleArray *A)
{
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  DoubleArray *B;
  B = doubleArrayCreate(minX, maxX, minY, maxY);
  return (B);
}
/*-----------------------------------------------------------------------------
LCD
-----------------------------------------------------------------------------*/
#ifdef XXX
int threshold(
  DoubleArray *A, 
  int minX, int maxX, int minY, int maxY, 
  void *parameter)
{
  double **a = A->array;
  int sizeX = minX - maxX + 1;
  int sizeY = minY - maxY + 1;
  
  int x;
  int y;
  
  double sum = 0.0;
  double n = (double)sizeX*(double)sizeY;
  double mean;
  
  /*
  printf("%d %d %d %d\n",minX,maxX,minY,maxY);
  */
  
  for(x=minX;x<=maxX;x++)
  {    
    for(y=minY;y<=maxY;y++)
    {
      sum += ARRAY(a,x,y);
    }
  }
  mean = sum/n;
  for(x=minX;x<=maxX;x++)
  {    
    for(y=minY;y<=maxY;y++)
    {
      if(ARRAY(a,x,y) > 0.6*mean)
      {
	ARRAY(a,x,y) = 1.0;
      }
      else
      {
	ARRAY(a,x,y) = 0.0;
      }
      /*
      ARRAY(a,x,y) -= mean;
      */
    }
  }
  return (RETURN_SUCCESS);
}
#endif
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

int doubleArrayTriangle(DoubleArray *A)
{
  double **a = A->array;
  int minX = A->loLimX;
  int minY = A->loLimY;
  
  int sizeX = A->sizeX;
  int sizeY = A->sizeY;
  
  if (doubleArrayInitValue(A,0.0) != RETURN_SUCCESS)
  {
    perror("doubleArrayTriangle(): problem in doubleArrayInitValue");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }

  
  
  /*-----  -----*/
  {
    int x;  
    
    double l=(4*sizeY/7);
    
    double x0=sizeX/2+sqrt(3)*l/4;
    double y0=sizeY/2-l/2;
    double x2=x0-sqrt(3)*l/2;
    
    {
      int n;
      int m;
      
      for (n=0;n<=sqrt(3.0)*l/2.0;n++)
      {
	x = (int)(((double) n)/tan(M_PI/3.0));
	for (m = x; m<= l-x; m++)
	{
	  ARRAY(a,minX+(int)(x2)-n+(int)(sqrt(3.0)*l/2.0),minY+(int)y0+m) = 1.0;
	}
      }
    } 
  }
  return (RETURN_SUCCESS);
}
#ifdef YYY
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayConvolve(
  DoubleArray *signal, 
  DoubleArray *filter,
  DoubleArray *response) 
{
  /*-----  -----*/
  int xMinFil = filter->loLimX;
  int xMaxFil = filter->upLimX;
  int yMinFil = filter->loLimY;
  int yMaxFil = filter->upLimY;
  
  /*-----  -----*/
  int xMinRes = response->loLimX;
  int xMaxRes = response->upLimX;
  int yMinRes = response->loLimY;
  int yMaxRes = response->upLimY;
  
  /*-----  -----*/
  int x;
  int y;
  double **res;
  double **fil;
  double **sig;
  res = response->array;
  sig = signal->array;
  fil = filter->array;
  
  for (x = xMinRes; x <= xMaxRes; x++)
  {
    for (y = yMinRes; y <= yMaxRes; y++)
    {
      double sum = 0.0;
      int fx;
      int fy;
      for (fx = xMinFil; fx <= xMaxFil; fx++)
      {
	for (fy = yMinFil; fy <= yMaxFil; fy++)
	{
	  sum += ARRAY(fil, fx, fy) * ARRAY(sig, x - fx, y - fy);
	}
      }
      ARRAY(res, x, y) = sum;
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
double convolvePoint(
  DoubleArray * signal, 
  DoubleArray * filter, 
  int x,
  int y,
  double *response)
{
  /*-----  -----*/
  int xMinFil = filter->loLimX;
  int xMaxFil = filter->upLimX;
  int yMinFil = filter->loLimY;
  int yMaxFil = filter->upLimY;
  
  /*-----  -----*/
  double sum = 0.0;
  
  /*-----  -----*/
  double **fil;
  double **sig;
  sig = signal->array;
  fil = filter->array;
  
  {
    int fx;
    int fy;
    for (fx = xMinFil; fx <= xMaxFil; fx++)
    {
      for (fy = yMinFil; fy <= yMaxFil; fy++)
      {
	sum += ARRAY(fil, fx, fy) * ARRAY(sig, x - fx, y - fy);
      }
    }
  }
  return(sum);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int gaborFunction(
  DoubleArray * realFilter,
  DoubleArray * imagFilter,
  double sigma,
  double alpha,
  double omega,
  double sigmaPhi)
{
  double cosAlpha = cos(alpha);
  double sinAlpha = sin(alpha);
  double sigmaSquare = sigma * sigma;
  double sigmaPhiSquare = sigmaPhi * sigmaPhi;
  double norm = 1.0;
  
  double **re = realFilter->array;
  double **im = imagFilter->array;
  int xMin = realFilter->loLimX;
  int xMax = realFilter->upLimX;
  int yMin = realFilter->loLimY;
  int yMax = realFilter->upLimY;
  
  
  {
    int x;
    int y;
    for (x = xMin; x <= xMax; x++)
    {
      for (y = yMin; y <= yMax; y++)
      {
	double dx = (double) x;
	double dy = (double) y;
	double tx = dx * cosAlpha - dy * sinAlpha;
	double ty = dx * sinAlpha + dy * cosAlpha;
	double arg = exp(-0.5 * tx * tx / sigmaSquare) *
	  exp(-0.5 * ty * ty / sigmaPhiSquare);
	double phi = omega * tx;
	ARRAY(re, x, y) = cos(phi) * arg / norm;
	ARRAY(im, x, y) = sin(phi) * arg / norm;
      }
    }
  }
  return (RETURN_SUCCESS);
}
#ifdef XXX
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayFFT(
  DoubleArray * realArr,
  DoubleArray * imagArr)
{
  int x;
  int y;
  
  extern int fft(int, double *, double *);
  double **re = realArr->array;
  double **im = imagArr->array;
  int loLimX = realArr->loLimX;
  int upLimX = realArr->upLimX;
  int loLimY = realArr->loLimY;
  int upLimY = realArr->upLimY;
  int sizeX = upLimX - loLimX + 1;
  int sizeY = upLimY - loLimY + 1;
  int n;
  
  /*-----  -----*/
  {
    double *reVec;
    double *imVec;
    reVec = (double *) malloc(sizeY * sizeof(double));
    if (reVec == (double *) NULL)
    {
      perror("$Signal:$: fft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }
    imVec = (double *) malloc(sizeY * sizeof(double));
    if (imVec == (double *) NULL)
    {
      perror("$Signal:$: fft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
    }
    for (x = loLimX; x <= upLimX; x++)
    {
      for (y = loLimY, n = 0; y <= upLimY; y++, n++)
      {
        reVec[n] = (double) ARRAY(re, x, y);
        imVec[n] = (double) ARRAY(im, x, y);
      }
      fft(sizeY, reVec, imVec);
      for (y = loLimY, n = 0; y <= upLimY; y++, n++)
      {
        ARRAY(re, x, y) = (double) reVec[n];
        ARRAY(im, x, y) = (double) imVec[n];
      }
    }
    free(reVec);
    free(imVec);
  }
  
  /*-----  -----*/
  {
    double *reVec;
    double *imVec;
    reVec = (double *) malloc(sizeX * sizeof(double));
    if (reVec == (double *) NULL)
    {
      perror("$Signal:$: fft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
    }
    imVec = (double *) malloc(sizeX * sizeof(double));
    if (imVec == (double *) NULL)
    {
      perror("$Signal:$: fft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
    }
    for (y = loLimY; y <= upLimY; y++)
    {
      for (x = loLimX, n = 0; x <= upLimX; x++, n++)
      {
        reVec[n] = (double) ARRAY(re, x, y);
        imVec[n] = (double) ARRAY(im, x, y);
      }
      fft(sizeX, reVec, imVec);
      for (x = loLimX, n = 0; x <= upLimX; x++, n++)
      {
        ARRAY(re, x, y) = (double) reVec[n];
        ARRAY(im, x, y) = (double) imVec[n];
      }
    }
    free(reVec);
    free(imVec);
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayIFFT(
  DoubleArray * realArr,
  DoubleArray * imagArr)
{
  int x;
  int y;
  
  extern int ifft(int, double *, double *);
  double **re = realArr->array;
  double **im = imagArr->array;
  int loLimX = realArr->loLimX;
  int upLimX = realArr->upLimX;
  int loLimY = realArr->loLimY;
  int upLimY = realArr->upLimY;
  int sizeX = upLimX - loLimX + 1;
  int sizeY = upLimY - loLimY + 1;
  int n;
  
  /*-----  -----*/
  {
    double *reVec;
    double *imVec;
    reVec = (double *) malloc(sizeY * sizeof(double));
    if (reVec == (double *) NULL)
    {
      perror("$Signal:$: ifft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }
    imVec = (double *) malloc(sizeY * sizeof(double));
    if (imVec == (double *) NULL)
    {
      perror("$Signal:$: ifft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }
    for (x = loLimX; x <= upLimX; x++)
    {
      for (y = loLimY, n = 0; y <= upLimY; y++, n++)
      {
        reVec[n] = (double) ARRAY(re, x, y);
        imVec[n] = (double) ARRAY(im, x, y);
      }
      ifft(sizeY, reVec, imVec);
      for (y = loLimY, n = 0; y <= upLimY; y++, n++)
      {
        ARRAY(re, x, y) = (double) reVec[n];
        ARRAY(im, x, y) = (double) imVec[n];
      }
    }
    free(reVec);
    free(imVec);
  }
  
  /*-----  -----*/
  {
    double *reVec;
    double *imVec;
    reVec = (double *) malloc(sizeX * sizeof(double));
    if (reVec == (double *) NULL)
    {
      perror("$Signal:$: ifft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }
    imVec = (double *) malloc(sizeX * sizeof(double));
    if (imVec == (double *) NULL)
    {
      perror("$Signal:$: ifft2d(): malloc() fails!");
#ifndef RETURN_AT_ERROR
      exit(EXIT_FAILURE);
#else
      return(RETURN_FAILURE);
#endif    
    }
    for (y = loLimY; y <= upLimY; y++)
    {
      for (x = loLimX, n = 0; x <= upLimX; x++, n++)
      {
        reVec[n] = (double) ARRAY(re, x, y);
        imVec[n] = (double) ARRAY(im, x, y);
      }
      ifft(sizeX, reVec, imVec);
      for (x = loLimX, n = 0; x <= upLimX; x++, n++)
      {
        ARRAY(re, x, y) = (double) reVec[n];
        ARRAY(im, x, y) = (double) imVec[n];
      }
    }
    free(reVec);
    free(imVec);
  }
  return (RETURN_SUCCESS);
}
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArraySquare(DoubleArray *A, int l)
{
  double **a = A->array;
  
  int sizeX = A->sizeX;
  int sizeY = A->sizeY;
  
  if (doubleArrayInitValue(A,0.0) != RETURN_SUCCESS)
  {
    perror("doubleArraySquare(): problem in doubleArrayInitValue");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }

  
  /*-----  -----*/
  {
    int x;
    int y;
    
    for (x = -l; x <= l; x++)
    {
      for (y = -l; y <= l; y++)
      {
        ARRAY(a,sizeX/2+x,sizeY/2+y) = 1.0;
      }	 
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayHessian(
  DoubleArray * A, /* IN */
  DoubleArray * B  /* OUT */
  )
{
  /*-----  -----*/
  double **a = A->array;
  double **b = B->array;
  
  /*-----  -----*/
  int minX = A->loLimX;
  int maxX = A->upLimX;
  int minY = A->loLimY;
  int maxY = A->upLimY;
  
  /*-----  -----*/
  {
    int x;
    int y;
    
    for (x = minX; x <= maxX; x++)
    {
      for (y = minY; y <= maxY; y++)
      {
	ARRAY(b,x,y) = (
          ARRAY(a,x+1,y+1) -
          ARRAY(a,x+1,y-1) -
          ARRAY(a,x-1,y+1) +
          ARRAY(a,x-1,y-1)
          )/4.0;
      }
    }
  }
  return (RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayPatches(
  int minX,
  int maxX,
  int minY,
  int maxY,
  int margin)
{
  
  double drand48(void);
  
  int n;  
  int m;
  
  DoubleArray *A = doubleArrayCreate(minX, maxX, minY, maxY);
  double **a = A->array;
  if (A == (DoubleArray *)NULL)
  {
    perror("doubleArrayPatches(): doubleArray not allocated");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return((DoubleArray *) NULL);
#endif    
  }

  if (doubleArrayInit(A) != RETURN_SUCCESS)
  {
    perror("doubleArrayPatches(): problem in doubleArrayInit");
#ifndef RETURN_AT_ERROR
    exit(EXIT_FAILURE);
#else
    return(RETURN_FAILURE);
#endif    
  }
  
  /*-----  -----*/  
  for(n=0;n<=3;n++)
  {
    for(m=0;m<=3;m++)
    {
      
      int x;
      int y;
      
      double dn = ((double)maxX - (double)minX - (double)2.0*margin)/4.0;
      double dm = ((double)maxY - (double)minY - (double)2.0*margin)/4.0;
      
      double alpha = drand48()*2.0*M_PI;
      
      double cosAlpha = cos(alpha);
      double sinAlpha = sin(alpha);
      
      double mx = minX+margin+n*dn+dn/2.0;
      double my = minY+margin+m*dm+dm/2.0;
      
      double omega = (0.5 + drand48()*(1.0));
      
      for (x = minX+margin+n*dn; x <= minX+margin+(n+1)*dn; x++)
      {
        for (y = minY+margin+m*dm; y <= minY+margin+(m+1)*dm; y++)
        {
          double dx = (double) x - mx;
          double dy = (double) y - my;
          double tx = dx * cosAlpha - dy * sinAlpha;
          /* double ty = dx * sinAlpha + dy * cosAlpha; */
          ARRAY(a, x, y) = cos(omega*tx);
        }
      }
    }  
  }
  
  return(A);
}
/*-----------------------------------------------------------------------------
####### #     # ######
#       ##    # #     #
#       # #   # #     #
#####   #  #  # #     #
#       #   # # #     #
#       #    ## #     #
####### #     # ######
-----------------------------------------------------------------------------*/
#ifdef LCD
/*
gcc -DLCD DoubleArray.c -lm
a.out ~/images/test.pgm 
*/
int main (const int argc, char *const *argv)
{
  DoubleArray *A  = doubleArrayReadRAWPGM(argv[1]);
  
  doubleArrayLineGNUPLOTPS(A, "test1.ps", 261, 1);
  /*
  doubleArrayLineGNUPLOTPS(A, "test2.ps", 377, 0);
  */
  
  doubleArrayBlock(A, 16, 16, threshold, (void *)NULL);
  
  doubleArrayLineGNUPLOTPS(A, "test3.ps", 261, 1);
  /*
  doubleArrayLineGNUPLOTPS(A, "test4.ps", 377, 0);
  */
  
  /*
  DoubleArray *B = doubleArrayLocalBackground(A,64);
  doubleArrayLineGNUPLOT(A, 10, 1);
  doubleArrayLineGNUPLOT(A, 10, 0);
  */
  doubleArrayXV(A);
  wait(0);
  return(0);
}
#endif

#ifdef TEST2
int main()
{
  DoubleArray *A  = doubleArrayCreate(1,128,1,128);
  doubleArrayTriangle(A);
  doubleArrayXV(A);
}
#endif
#ifdef TEST1
int main()
{
  DoubleArray *A  = doubleArrayReadRaw("turing.raw",1,128,1,128);
  DoubleArray *B  = doubleArrayReadRaw("turing.raw",1,128,1,128);
  DoubleArray *R = doubleArrayCreate(1,128,1,128);
  DoubleArray *I = doubleArrayCreate(1,128,1,128);
  
  double alpha = M_PI/3.0;
  
  doubleArrayInit(I);
  doubleArrayInit(R); 
  doubleArrayXV(A);
  
  doubleArrayAffineTransform(A, B,
			     0.1*cos(alpha), -sin(alpha), 0.0, 
			     sin(alpha),  cos(alpha), 0.0);
  
  doubleArrayXV(B);
  wait(0);
  
  /*-----  -----*/
  {
    int r;
    int o;  
    
    for(o = 0; o < 6; o++)
    {
      for(r = 0; r < 2; r++)
      {
	createGaborResponse(
          A, R, I,
          4.0,12.0,
          2,6,
          r,o);
	doubleArrayXV(I);
	doubleArrayXV(R);
      }
    }
  }
  
  wait(0);
  
  doubleArrayDestroy(A);
  doubleArrayDestroy(B);
  doubleArrayDestroy(R);
  doubleArrayDestroy(I);  
  
}
#endif

#ifdef WALSH
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
h_walshtr_F(image,nr,nc)
  double *image;
  int nr,nc;
{
  int i,j,logrows,logcols;
  
  logrows = logcols = -1;
  i = 0; j = 1;
  while (1) 
  {
    if (j == nr)
      logrows = i;
    else if (j > nr)
      break;
    if (j == nc)
      logcols = i;
    else if (j > nc)
      break;
    if (logrows >= 0 && logcols >= 0)
      break;
    i++; j <<= 1;
  }
  if (logrows == -1 || logcols == -1)
    return(RETURN_FAILURE);
  return(h_fwt_f(image,logrows+logcols));
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
h_fwt_f(vec,loglen)
  double *vec;
  int loglen;
{
  int n,nv2,nm1,i,j,k,l,le,le1;
  double t,*veci,*vecj;
  
  n = 1<<loglen;
  nv2 = n >> 1;
  nm1 = n-1;
  j = 1;
  for (i=1;i<=nm1;i++) 
  {
    if (i<j) 
    {
      veci = vec+i-1;
      vecj = vec+j-1;
      t = (*vecj);
      *vecj = (*veci);
      *veci = t;
    }
    k = nv2;
    while (k<j) 
    {
      j -= k;
      k >>= 1;
    }
    j += k;
  }
  le = 1;
  for (l=1;l<=loglen;l++) 
  {
    le1 = le;
    le += le;
    for (j=0;j<le1;j++)
    {
      for (i=j;i<n;i+=le) 
      {
        veci = vec+i;
        vecj = veci+le1;
        t = (*vecj);
        *vecj = (*veci-t);
        *veci+=t;
      }
    }
  }
  return(RETURN_SUCCESS);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void doubleArrayWalsh(DoubleArray * A)
{
  int x;
  int y;
  
  double **a = A->array;
  int loLimX = A->loLimX;
  int upLimX = A->upLimX;
  int loLimY = A->loLimY;
  int upLimY = A->upLimY;
  int sizeX = A->sizeX;
  int sizeY = A->sizeY;
  int n;
  double *vec;
  
  /*-----  -----*/
  {
    vec = (double *) malloc(sizeY * sizeX * sizeof(double));
    if (vec == (double *) NULL)
    {
      perror("DoubleArray.c: doubleArrayWalsh(): malloc() fails!");
      exit(EXIT_FAILURE);
    }
    
    n = 0;
    for (x = loLimX; x <= upLimX; x++)
    {
      for (y = loLimY; y <= upLimY; y++)
      {
        vec[n] = (double) ARRAY(a, x, y);
        n++;
      }
    }
    
    h_walshtr_F(vec, sizeX, sizeY);
    
    n = 0;
    for (x = loLimX; x <= upLimX; x++)
    {
      for (y = loLimY; y <= upLimY; y++)
      {
        ARRAY(a, x, y) = (double) vec[n];
        n++;
      }
    }
    free((double *)vec);
  }
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int main()
{
  DoubleArray *A  = doubleArrayCreate(1,256,1,256);
  doubleArrayTriangle(A);
  doubleArrayXV(A);
  doubleArrayWalsh(A);  
  doubleArrayXV(A);
  doubleArrayWalsh(A);  
  doubleArrayXV(A);
  wait(0);
}  
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#endif
#endif
