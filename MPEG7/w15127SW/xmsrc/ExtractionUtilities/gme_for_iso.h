///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// (contributing organizations names)
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  gme_for_iso.h
//

#ifndef _GME_FOR_ISO_
#define _GME_FOR_ISO_

#include "momusys.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
#ifdef USEXML3
void GlobalMotionEstimation(MomVop *ref_vop, MomVop *curr_vop,
				       Int no_of_sprite_warping_points, double *motion_params);
#else
void GlobalMotionEstimation(MomVop *ref_vop, MomVop *curr_vop,
				       Int no_of_sprite_warping_points, Double *motion_params);
#endif // USEXML3

Int GetMomVopWidth(MomVop *vop);
Int GetMomVopHeight(MomVop *vop);
Int GetMomVopHorSpatRef(MomVop *vop);
Int GetMomVopVerSpatRef(MomVop *vop);
MomImage *GetMomVopY(MomVop *vop);
MomImage *GetMomVopA(MomVop *vop);
MomImageType GetMomImageType(MomImage *image);
Char *GetMomImageData(MomImage *image);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /*_GME_FOR_ISO_*/

