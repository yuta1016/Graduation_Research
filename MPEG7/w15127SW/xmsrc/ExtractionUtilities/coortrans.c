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
// Applicable File Name:  coortrans.c
//
//

/******* INCLUDES ************************************************************/
#include "address.h"
#include "coortrans.h"

/******* DEFINES *************************************************************/

/******* TYPES ***************************************************************/

/******* VARIABLES ***********************************************************/
float *_tr_parameters;   /* par1,par2,..par12*/
float *_tr_origins;      /* xin,yin,xout,yout*/

/******* FUNCTIONS ***********************************************************/

/******* Code ****************************************************************/


//===========================================================================
/*coordinate transformation routines*/

void tranlational(TCoor width,TCoor height,
		  TCoor inx,TCoor iny,
		  float *outx,float *outy)
{
  *outx=(float)inx-_tr_origins[0] +_tr_parameters[0]+ _tr_origins[2];
  *outy=(float)iny-_tr_origins[1] +_tr_parameters[1]+ _tr_origins[3];
}

//----------------------------------------------------------------------------
void rotational(TCoor width,TCoor height,
		TCoor inx,TCoor iny,
		float *outx,float *outy)
{
  float tmpx=inx-_tr_origins[0];
  float tmpy=iny-_tr_origins[1];

  *outx=tmpx+ _tr_parameters[0] + tmpx* _tr_parameters[2] +
    tmpy* _tr_parameters[3];
  *outy=tmpy+ _tr_parameters[1] - tmpx* _tr_parameters[3] +
    tmpy* _tr_parameters[2];

  *outx+=_tr_origins[2];
  *outy+=_tr_origins[3];
}

//----------------------------------------------------------------------------
void affine(TCoor width,TCoor height,
	    TCoor inx,TCoor iny,
	    float *outx,float *outy)
{
  float tmpx=inx-_tr_origins[0];
  float tmpy=iny-_tr_origins[1];

  *outx=tmpx+ _tr_parameters[0] + tmpx* _tr_parameters[2] +
    tmpy* _tr_parameters[3];
  *outy=tmpy+ _tr_parameters[1] + tmpx* _tr_parameters[4] +
    tmpy* _tr_parameters[5];

  *outx+=_tr_origins[2];
  *outy+=_tr_origins[3];
}

//----------------------------------------------------------------------------
void perspective(TCoor width,TCoor height,
		 TCoor inx,TCoor iny,
		 float *outx,float *outy)
{
}

//----------------------------------------------------------------------------
void parabolic(TCoor width,TCoor height,
	       TCoor inx,TCoor iny,
	       float *outx,float *outy)
{
}

//----------------------------------------------------------------------------
int invertaffine(float *transpar_in,  float *origins_in,
		 float *transpar_out, float *origins_out)

{
  float det;

  /* invert transformation parameters*/
  origins_out[0]=origins_in[2];
  origins_out[1]=origins_in[3];
  origins_out[2]=origins_in[0];
  origins_out[3]=origins_in[1];

  det=(transpar_in[2]+1)*(transpar_in[5]+1)-
    transpar_in[4]*transpar_in[3];
  if (det == 0.0) return 1;

  transpar_out[0]=transpar_out[1]=0.0;
  transpar_out[2]=(transpar_in[5]+1)/det;
  transpar_out[3]=-transpar_in[3]/det;
  transpar_out[4]=-transpar_in[4]/det;
  transpar_out[5]=(transpar_in[2]+1)/det;

  transpar_out[0]=
    -transpar_in[0]*transpar_out[2] -
    transpar_in[1]*transpar_out[3];
  transpar_out[1]=
    -transpar_in[0]*transpar_out[4] -
    transpar_in[1]*transpar_out[5];

  transpar_out[2]-=1.0;
  transpar_out[5]-=1.0;
    
#ifdef TESTINVERT
  float deti;
  float *tmppar;
  float *tmporigin;
  int testx, testy;
  float tmpx, tmpy;

  deti=(transpar_out[2]+1)*(transpar_out[5]+1)-
    transpar_out[4]*transpar_out[3];

  tmppar=_tr_parameters;
  tmporigin=_tr_origins;
  _tr_parameters=transpar_in;
  _tr_origins=origins_in;
  affine(0,0,1000,3000,&tmpx,&tmpy);
  testx=(int)(tmpx+0.5);
  testy=(int)(tmpy+0.5);
  _tr_parameters=transpar_out;
  _tr_origins=origins_out;
  affine(0,0,testx,testy,&tmpx,&tmpy);
  _tr_parameters=tmppar;
  _tr_origins=tmporigin;
#endif

  return 0;
}
