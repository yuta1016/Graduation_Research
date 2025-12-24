/*--------------------------------------------------------------------------------------
 This software module was originally developed by

 EPFL (Yousri Abdeljaoued,abdel@ltssg3.epfl.ch)
 Ericsson Radio Systems AB,(Charilaos Christopoulos, ch.christopoulos@era.ericsson.se)
 EPFL (Touradj Ebrahimi, Touradj.Ebrahimi@epfl.ch)
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
 
 Applicable File Name:  DoubleArray.h

-------------------------------------------------------------------------------------*/
#ifndef __DOUBLE_ARRAY_H__
#define __DOUBLE_ARRAY_H__

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
 

extern double drand48 (void);





/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
/*----- *(*(arr_p+y)+x) _is_equal_to_ arr_p[y][x] -----*/
#define ARRAY(arr_p,x,y)    (*(*(arr_p+y)+x))

#define SQUARE(x) ((x) * (x))


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
typedef struct 
{
  double re, im;
} DoubleComplex;
/*-----------------------------------------------------------------------------
  DoubleArray
-----------------------------------------------------------------------------*/
typedef struct
{
  double **array;
  double *block; /* The array can also be addressed as a continuous 
		    block if this pointer is non nil */
  int loLimX;
  int upLimX;
  int loLimY;
  int upLimY;
  int sizeX;
  int sizeY;
}      DoubleArray;		/* */
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#define doubleArrayCreateTemp(arr,minX,maxX,minY,maxY)                        \
{                                                                             \
  int y;                                                                      \
  arr = (DoubleArray *) alloca(sizeof(DoubleArray));                          \
  arr->loLimX = minX;                                                         \
  arr->upLimX = maxX;                                                         \
  arr->loLimY = minY;                                                         \
  arr->upLimY = maxY;                                                         \
  arr->sizeX = maxX - minX + 1;                                               \
  arr->sizeY = maxY - minY + 1;                                               \
  arr->array = (double **) alloca(arr->sizeY * sizeof(int));                  \
  arr->array = arr->array - minY;                                             \
  for (y = minY; y <= maxY; y++)                                              \
  {                                                                           \
      arr->array[y] = (double *) alloca(arr->sizeX * sizeof(double));         \
      arr->array[y] = arr->array[y] - minX;                                   \
  }                                                                           \
}               


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayCreate(int loLimX, int upLimX, int loLimY, int upLimY);
int doubleArrayInit(DoubleArray *A);
int doubleArrayInitLine(DoubleArray * A, int y);
int doubleArrayInitValue(DoubleArray *A, double val);
int doubleArrayDestroy(DoubleArray *A);
int doubleArrayDestroy2Arg(void *link, void *parameter);
DoubleArray *doubleArraySubArray(DoubleArray *A, int loSubX, int upSubX, int loSubY, int upSubY);
int doubleArrayWrite(DoubleArray *A, char *fileName);
DoubleArray *doubleArrayRead(char *fileName);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadRAWPGM(char *fileName);
int doubleArrayReadRAWPPM(char *fileName, 
			   DoubleArray **RR,
			   DoubleArray **GG,
			   DoubleArray **BB);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayReadRaw(char *fileName, int loLimX, int upLimX, int loLimY, int upLimY);
DoubleArray *doubleArrayReadDoubleRaw(char *fileName, int loLimX, int upLimX, int loLimY, int upLimY);
int doubleArrayWriteRaw(DoubleArray *A, char *fileName);
int doubleArrayMaxMin(DoubleArray *A, double *max, double *min);
/* int doubleArrayXdisplay(DoubleArray * arr_p); */ 
DoubleArray *doubleArrayReadChar(char *fileName, int minX, int maxX, int minY, int maxY); 
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/

int doubleArrayXLOADIMAGE(DoubleArray *A); 
int doubleArrayWritePGM(char *fileName, DoubleArray *A); 
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayCopy90(DoubleArray *A);
DoubleArray *doubleArrayCopy180(void); 
DoubleArray *doubleArrayCopy270(void); 
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayRot90(DoubleArray *A); 
DoubleArray *doubleArrayRot180(void); 
DoubleArray *doubleArrayRot270(void);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayOrigin(DoubleArray *A, int x0, int y0);
int doubleArrayNorm(DoubleArray *aa);
int doubleArrayMean(DoubleArray *aa);
int doubleArrayNormalize(DoubleArray *aa);
int doubleArrayScale(DoubleArray *A, double s);
int doubleArrayCopy(DoubleArray *dest, DoubleArray *source);
int doubleArrayBlur(DoubleArray *A);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayMultiply(DoubleArray *A, DoubleArray *B);
DoubleArray *doubleArrayMult(DoubleArray *A, DoubleArray *B);
DoubleArray *doubleArrayInvert(DoubleArray *A);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayDuplicate(DoubleArray *A);
DoubleArray *doubleArrayTranspose(DoubleArray *A);
DoubleArray *doubleArrayAdd(DoubleArray *A, DoubleArray *B);
DoubleArray *doubleArraySub(DoubleArray * A, DoubleArray * B);
int doubleArrayMinusEqual(DoubleArray *A, DoubleArray *B);
int doubleArrayPlusEqual(DoubleArray *A, DoubleArray *B);
DoubleArray *doubleArrayHypot(DoubleArray *A, DoubleArray *B);
DoubleArray *doubleArrayPower(DoubleArray *A, double m);
int doubleArrayPointMultEqual(DoubleArray *A, DoubleArray *B);
int doubleArrayMultScalarEqual(DoubleArray *A, double b);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayOperator( DoubleArray * A, DoubleArray * B,
DoubleArray * C, double (*op)(double, double));
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayTriple(DoubleArray *A);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayShow(DoubleArray *A);
int doubleArrayPrint(DoubleArray *A);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayDrawLine(DoubleArray *A, int x1, int y1, int x2, int
y2, double c);
int doubleArrayDrawLine2(DoubleArray * A, int x1, int
y1, int x2, int y2, double c);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayTriangle(DoubleArray *A);
int doubleArraySquare(DoubleArray *A, int l);
DoubleArray *doubleArrayPatches(int minX, int maxX, int minY, int maxY, int
margin);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int dAwritePGM(FILE *file_p, DoubleArray *A); 
int doubleArrayDISPLAY(DoubleArray *A); 
DoubleArray *doubleArrayAtan2(DoubleArray *A, DoubleArray *B);
int doubleArrayHistogram(DoubleArray *A, int buckets, double *histo);
int doubleArrayDiscretize(DoubleArray *A, int buckets); 
DoubleArray *doubleArrayLocalHistogramEgalisation(DoubleArray *A, int buckets, int localSize); 
DoubleArray *doubleArrayHistogramEqualization(DoubleArray *A, int buckets); 
int doubleArrayLineGNUPLOT(DoubleArray *A, int s, int direction); 

int doubleArrayXPlot(DoubleArray *A, int x); 
int doubleArrayYPlot(DoubleArray *A, int y); 

int doubleArrayConvolve(DoubleArray *signal, DoubleArray *filter,
DoubleArray *response); 

double convolvePoint(DoubleArray *signal,
DoubleArray *filter, int x, int y, double *response); 

int
gaborFunction(DoubleArray *realFilter, DoubleArray *imagFilter, double
sigma, double alpha, double omega, double sigmaPhi);

 int rotation(double theta, double *x, double *y); 

int
doubleArrayAffineTransform(DoubleArray *A, DoubleArray *B, double ax,
double bx, double cx, double ay, double by, double cy);

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWriteASCII(DoubleArray *A, char *fileName);

DoubleArray *doubleArrayReadASCII(char *fileName, int loLimX, int upLimX, int loLimY,
int upLimY);

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayWritePPM(char *fileName, DoubleArray *R, DoubleArray
*G, DoubleArray *B); int dAwritePPM(FILE * file_p, DoubleArray * R,
DoubleArray * G, DoubleArray * B);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayGenerate(DoubleArray *A); 
int threshold( DoubleArray *A, int minX, int maxX, int minY, int
maxY, void *parameter);
int background( DoubleArray *A, int minX, int maxX, int minY, int
maxY, void *parameter);
int doubleArrayBlock(
  DoubleArray *A, 
  int localSizeX,
  int localSizeY,
  void (*action)(DoubleArray *A, 
		 int minX, int maxX, int minY, int maxY, 
		 void *parameter),
  void *parameter);
DoubleArray *doubleArrayBackground(
			 DoubleArray *A, 
			 int localSize);
DoubleArray *doubleArrayBackground(
			 DoubleArray *A, 
			 int localSize);
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleArrayLineGNUPLOT(DoubleArray *A, int s, int direction);
int doubleArrayLineGNUPLOTPS(DoubleArray *A, char *psName, 
			     int s, int direction);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
/*-----------------------------------------------------------------------------
####### #     # ######
#       ##    # #     #
#       # #   # #     #
#####   #  #  # #     #
#       #   # # #     #
#       #    ## #     #
####### #     # ######
-----------------------------------------------------------------------------*/
