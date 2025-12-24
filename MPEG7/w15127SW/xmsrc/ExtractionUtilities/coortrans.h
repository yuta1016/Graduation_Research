///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// S. Herrmann TU-Munich, Institute for Integrated Circuits
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  coortrans.h
//
//

/******* INCLUDES ************************************************************/
#include "address.h"

/******* DEFINES *************************************************************/
#ifndef _COORTRANS_H_
#define _COORTRANS_H_

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
extern float *_tr_parameters;   /* par1,par2,..par12*/
extern float *_tr_origins;      /* xin,yin,xout,yout*/

/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/

//===========================================================================
/*coordinate transformation routines*/
void tranlational(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);
void rotational(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);
void affine(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);
void perspective(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);
void parabolic(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);

int invertaffine(float *transpar_in,  float *origins_in,
		 float *transpar_out, float *origins_out);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
