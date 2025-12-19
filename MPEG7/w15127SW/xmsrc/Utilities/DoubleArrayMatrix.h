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
 
 Applicable File Name:  DoubleArrayMatrix.h

-------------------------------------------------------------------------------------*/
#include "Utilities/DoubleArray.h"

/*----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
#ifndef __DOUBLE_ARRAY_MATRIX_H__
#define __DOUBLE_ARRAY_MATRIX_H__
/*-----------------------------------------------------------------------------
SVD
-----------------------------------------------------------------------------*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int doubleArraySVD( 
		   DoubleArray *A, 
		   DoubleArray *U, 
		   DoubleArray *S,
		   DoubleArray *VT); 
/*-----------------------------------------------------------------------------
Inverse of a Matrix -- Matrix must be square!!
-----------------------------------------------------------------------------*/
DoubleArray *doubleArrayInverse(DoubleArray *A);
DoubleArray *doubleArrayInverseRobust(DoubleArray *A, int k);

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
