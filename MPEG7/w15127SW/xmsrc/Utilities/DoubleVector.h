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
 
 Applicable File Name:  DoubleVector.h

-------------------------------------------------------------------------------------*/
#ifndef __DOUBLE_VECTOR_H__
#define __DOUBLE_VECTOR_H__

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>


#ifndef VECTOR
/*----- *(vec+n) _is_equal_to_ vec[n] -----*/
#define VECTOR(vec_p,x)     (*(vec_p+x))
#endif


#define MAX_SIZE_VECTOR  2048
/*-----------------------------------------------------------------------------
DoubleVector
-----------------------------------------------------------------------------*/
typedef struct
{
    double *vector;
    int loLim;
    int upLim;
    int size;
}      DoubleVector;		/* */
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int doubleVectorWrite(DoubleVector * doubleVector, char *fileName);
DoubleVector *doubleVectorRead(char *fileName);
DoubleVector *doubleVectorCreate(int loLim, int upLim);
int doubleVectorInit(DoubleVector * vec_p);
int doubleVectorDestroy(DoubleVector * vec_p);
int doubleVectorMaxMin(DoubleVector * vec_p, double *max, double *min);
int doubleVectorNormalize(DoubleVector * vec_p);
int    doubleVectorInfo(DoubleVector * vec_p);
int doubleVectorOrigin(DoubleVector * A, int x0);

int doubleVectorWriteAsciiFile(DoubleVector *vec, char *fileName);
int doubleVectorWriteAscii(DoubleVector *vec, FILE *file_p);
DoubleVector *doubleVectorReadAsciiFile(char *fileName);
DoubleVector *doubleVectorReadAscii(FILE *file_p);

int doubleVectorWriteBinaryFile(DoubleVector *vec, char *fileName);
int doubleVectorWriteBinary(DoubleVector *vec, FILE *file_p);
DoubleVector *doubleVectorReadBinaryFile(char *fileName);
DoubleVector *doubleVectorReadBinary(FILE *file_p);


int doubleVectorScale(DoubleVector *in, double scale);


/*-----  -----*/

/*-----  -----*/
int doubleVectorLineFit(DoubleVector * vector, double *a0, double *a1, double *chi);

DoubleVector *doubleVectorTriple(DoubleVector * vec_p);

/*-----  -----*/
int doubleVectorPlusEqual(DoubleVector *left, DoubleVector *right);
int doubleVectorMinusEqual(DoubleVector *left, DoubleVector *right);
int doubleVectorPointMultEqual(DoubleVector *left, DoubleVector *right);
int doubleVectorDivideScalarEqual(DoubleVector *left, double right);

#endif
/*-----------------------------------------------------------------------------
END
-----------------------------------------------------------------------------*/





