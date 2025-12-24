///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
//  Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
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
// Applicable File Name:  ParametricObjectMotionUtilities.h
//

#ifndef _PARAMETRIC_OBJECT_MOTION_UTILITIES_
#define _PARAMETRIC_OBJECT_MOTION_UTILITIES_

#include "momusys.h"
#include "ExtractionUtilities/gme_for_iso.h"

#define Translational  0
#define Rotation_Scale 1
#define Affine         2
#define Perspective    3
#define Quadric        4

#define VelocityBasedRegion       0
#define VelocityBasedLittleSquare 1
#define ParameterBased            2

#define L1Norm      0
#define L2Norm      1
#define Correlation 2
#define Amplitude   3


void ComputeSpeedField(double *param1, double *param2, 
		       double **vectorx_FM1, double **vectory_FM1, 
		       double **vectorx_FM2, double **vectory_FM2, 
		       int sizex, int sizey, double xOrig1,double yOrig1,double xOrig2,double yOrig2,
		       double xg1, double yg1, double xg2, double yg2,int mCodeR,int mCodeQ);

double distFOTotal(double **fo1_vx, double **fo1_vy, double **fo2_vx, 
			      double **fo2_vy, int hh, int ww, int QueryNorm);

void  GetXYCenter(MomVop *ref_vop,double *xcenter,double *ycenter);

void ComputeSpeedFieldAllMask(double *param1,double *param2, MomVop *m_RefVop,MomVop *m_QueryVop, double **foSelected_vxD,double **foSelected_vyD, double **fo_vxD,double **fo_vyD,int mCodeR,int mCodeQ,double xOrigin1, double yOrigin1, double xOrigin2, double yOrigin2);



#endif /*_PARAMETRIC_OBJECT_MOTION_UTILITIES_*/

