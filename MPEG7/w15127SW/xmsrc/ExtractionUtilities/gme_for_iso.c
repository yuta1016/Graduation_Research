///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yoshinori Suzuki (Hitachi, Ltd.)
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// (contributing organizations names)
//
// Updated:
// Institut National des Telecommunications, ARTEMIS Project Unit,
//      Marius PREDA, Titus ZAHARIA (18/01/2000)
//
// Updated 06/14/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - bugfixes
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

/******************************************************************************
 *
 * This software module was originally developed by
 *
 *   Yoshinori Suzuki (Hitachi, Ltd.)
 *
 * in the course of development of the MPEG-4 Video (ISO/IEC 14496-2) standard.
 * This software module is an implementation of a part of one or more MPEG-4
 * Video (ISO/IEC 14496-2) tools as specified by the MPEG-4 Video (ISO/IEC
 * 14496-2) standard.
 *
 * ISO/IEC gives users of the MPEG-4 Video (ISO/IEC 14496-2) standard free
 * license to this software module or modifications thereof for use in hardware
 * or software products claiming conformance to the MPEG-4 Video (ISO/IEC
 * 14496-2) standard.
 *
 * Those intending to use this software module in hardware or software products
 * are advised that its use may infringe existing patents. The original
 * developer of this software module and his/her company, the subsequent
 * editors and their companies, and ISO/IEC have no liability for use of this
 * software module or modifications thereof in an implementation. Copyright is
 * not released for non MPEG-4 Video (ISO/IEC 14496-2) standard conforming
 * products.
 *
 * Hitachi, Ltd. retains full right to use the code for his/her own
 * purpose, assign or donate the code to a third party and to inhibit third
 * parties from using the code for non MPEG-4 Video (ISO/IEC 14496-2) standard
 * conforming products. This copyright notice must be included in all copies or
 * derivative works.
 *
 * Copyright (c) 1998
 *
 *****************************************************************************/

/***********************************************************HeaderBegin*******
 *
 * File:        gme_for_iso.c
 *
 * Author:      Yoshinori Suzuki (Hitachi, Ltd.)
 * Created:     06/08/98
 *
 * Description: utilities for global motion estimation
 * -- GlobalMotionEstimation
 * -- TranslationalGME
 * -- IsotropicGME
 * -- AffineGME
 * -- PerspectiveGME
 * -- ModifiedThreeStepSearch
 * -- ThreeTapFilter
 * -- DeltaMP
 * -- swap_line
 *
 * Notes:
 *
 * Modified:
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM

 

 ***********************************************************HeaderEnd*********/

/************************    INCLUDE FILES    ********************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "momusys.h"
#include "gme_for_iso.h"
#   define  GME_ABS(x)                (((x) < 0) ? -(x) : (x))


#ifndef Double
#define Double double
#endif // Double


struct sprite_motion
{
	Double	a1;
	Double	a2;
	Double	a3;
	Double	a4;
	Double	a5;
	Double	a6;
	Double	a7;
	Double	a8;
	Double	a9;
	Double	a10;
	Double	a11;
	Double	a12;
};
typedef struct sprite_motion Sprite_motion; 

Int
GetMomVopWidth(MomVop *vop)
{
  return(vop->width);
}

Int
GetMomVopHeight(MomVop *vop)
{
  return(vop->height);
}

Int
GetMomVopHorSpatRef(MomVop *vop)
{
  return(vop->hor_spat_ref);
}

Int
GetMomVopVerSpatRef(MomVop *vop)
{
  return(vop->ver_spat_ref);
}

MomImage *
GetMomVopY(MomVop *vop)
{
  return(vop->y_chan);
}

MomImage *
GetMomVopA(MomVop *vop)
{
  return(vop->a_chan);
}

MomImageType
GetMomImageType(MomImage *image)
{
  return(image->type);
}

Char *
GetMomImageData(MomImage *image)
{
  switch(GetMomImageType(image))
    {
    case SHORT_TYPE:
      return((Char *)image->data->s);
      break;
    case FLOAT_TYPE:
      return((Char *)image->data->f);
      break;
    case UCHAR_TYPE:
      return((Char *)image->data->u);
      break;
    default:
      printf("Image type >>%d<< not supported\n",image->type);
      return(NULL);
    }
}

void
TranslationalGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter);

void
IsotropicGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter);

void
AffineGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter);

void
PerspectiveGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter);

void
ParabolicGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter);

void ThreeTapFilter(SInt *Low, SInt *Hight, Int width, Int height);

Int
ModifiedThreeStepSearch(SInt *ref_work, SInt *curr_work, 
	SInt *ref_alpha_work, SInt *curr_alpha_work,
	Int curr_width, Int curr_height, Int ref_width, Int ref_height,
	Int offset_x, Int offset_y,
	Int *best_locationx, Int *best_locationy, Int SquareRange);

Int
DeltaMP(Double *dA, Int n, Double *db, Double *dm);

/***********************************************************CommentBegin******
 *
 * -- runerr -- Run-time error handling function
 *
 * Author :		
 *	
 *
 * Created :		
 *	
 *
 * Purpose :		
 *	Run-time error handling function
 * 
 * Arguments in : 	
 *	
 *
 * Arguments in/out :	
 *	
 *
 * Arguments out :	
 *	
 *
 * Return values :	
 *	
 *
 * Side effects :	
 *	
 *
 * Description :	
 *	Code adapted from XLIM
 *
 * See also :
 *	
 *
 * Modified :		
 *	
 *
 ***********************************************************CommentEnd********/

void
runerr(Char *error_msg, Int exit_code)
{
  printf("\n\n%s\n",error_msg);
  exit(exit_code);
}

/***********************************************************CommentBegin******
 *
 * -- emalloc -- Memory allocation with error handling
 *
 * Author :		
 *	
 *
 * Created :		
 *	
 *
 * Purpose :		
 *	Memory allocation with error handling.
 * 
 * Arguments in : 	
 *	
 *
 * Arguments in/out :	
 *	
 *
 * Arguments out :	
 *	
 *
 * Return values :	
 *	
 *
 * Side effects :	
 *	
 *
 * Description :	
 *	WARNING: There is a problem when you want to allocate more memory
 *	than size can handle.  Malloc gets as argument an unsigned.  It poses
 *	a problem when sizeof(unsigned) < sizeof(Char *)...
 *
 *	Code adapted from XLIM
 *
 * See also :
 *	
 *
 * Modified :		
 *	
 *
 ***********************************************************CommentEnd********/

Char *
emalloc(Int n)
{
  Char 	*p;
	
  if(n <= 0) runerr("ERROR(emalloc): Wrong size: <= 0.",1);

  p = (Char *) malloc((UInt)n);
  if(p == NULL)
    runerr("ERROR(emalloc): memory allocation error.",1);

  return p;
}

/***********************************************************CommentBegin******
 *
 * --    GlobalMotionEstimation
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 08-06-98
 *
 * Purpose :   generator of global motion parameters
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM
 *
 *
 ***********************************************************CommentEnd********/

/* specification of parameters
    a1 x + a2 y + a3    a4 x + a5 y + a6
    ----------------    ----------------
    a7 x + a8 y + 1     a7 x + a8 y + 1

   d=2  X'i = X + M[0], Y'i = Y + M[1]
   d=4  X'i = M[0]X + M[1]Y + M[2], Y'i = -M[1]X + M[0]Y + M[3]
   d=6  X'i = M[0]X + M[1]Y + M[2], Y'i = M[3]X + M[4]Y + M[5]
   d=8  X'i = M[0]X + M[1]Y + M[2], Y'i = M[3]X + M[4]Y + M[5]
              --------------------        --------------------
              M[6]X + M[7]Y +  1          M[6]X + M[7]Y +  1
*/
#ifdef USEXML3
void GlobalMotionEstimation(MomVop *ref_vop, MomVop *curr_vop,
					Int model_code, double *parameters)
#else
void GlobalMotionEstimation(MomVop *ref_vop, MomVop *curr_vop,
					Int model_code, Double *parameters)
#endif // USEXML3
{
Sprite_motion *m_parameter = (Sprite_motion *)emalloc(sizeof(Sprite_motion)); 

switch(model_code) {
case 0:
TranslationalGME(ref_vop, curr_vop, m_parameter);
break;
case 1:
IsotropicGME(ref_vop, curr_vop, m_parameter);
break;
case 2:
AffineGME(ref_vop, curr_vop, m_parameter);
break;
case 3:
PerspectiveGME(ref_vop, curr_vop, m_parameter);
break;
case 4:
ParabolicGME(ref_vop, curr_vop, m_parameter);
break;
default:
printf("Invalid motion model: %d\n",model_code);
exit(0);
break;
}

parameters[0]  = m_parameter->a1;
parameters[1]  = m_parameter->a2;
parameters[2]  = m_parameter->a3;
parameters[3]  = m_parameter->a4;
parameters[4]  = m_parameter->a5;
parameters[5]  = m_parameter->a6;
parameters[6]  = m_parameter->a7;
parameters[7]  = m_parameter->a8;
parameters[8]  = m_parameter->a9;
parameters[9]  = m_parameter->a10;
parameters[10] = m_parameter->a11;
parameters[11] = m_parameter->a12;

free(m_parameter);
}

/***********************************************************CommentBegin******
 *
 * --    TranslationalGME
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose : generator of global motion parameters for translational transform
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM
 *
 *
 ***********************************************************CommentEnd********/
void
TranslationalGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter)
{
UChar *UCref, *UCcurr, *UCcurr_alpha, *UCref_alpha;
SInt *ref, *curr, *curr_alpha, *ref_alpha;
SInt *ref_P[3], *curr_P[3], *ref_alpha_P[3], *curr_alpha_P[3];
SInt *ref_work, *curr_work, *curr_alpha_work, *ref_alpha_work;
Int curr_width, curr_height, curr_left, curr_top;
Int ref_width, ref_height, ref_left, ref_top;

Int i, j, best_locationx, best_locationy;
Int curr_pel, offset_x, offset_y, ref_pel;
Int ref_pel1, ref_pel2, ref_pel3;
Double dm[2], db[2], dA[4], M[2];
Double dE2 = 0;
Int stop = 0, level = 2, ite = 0;
Int x, y, x1, y1;
Double dx1, dy1, dx, dy;
Double dt, du, dk, d1mk, dl, d1ml,
	dI1, de, dI1dx, dI1dy;
Double ddedm[2];
Double I1x1y1[4];
Int error_histgram[256];
Int threshold_T=0, check=1;

// ref = (SInt *) GetMomImageData(GetMomVopY(ref_vop));
// curr = (SInt *) GetMomImageData(GetMomVopY(curr_vop));
// ref_alpha = (SInt *) GetMomImageData(GetMomVopA(ref_vop));
// curr_alpha = (SInt *) GetMomImageData(GetMomVopA(curr_vop));

//START

 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCcurr = (UChar *) GetMomImageData(GetMomVopY(curr_vop));

 if ( ref_vop->a_chan != NULL )
	 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 if ( curr_vop->a_chan != NULL ) 
	 UCcurr_alpha = (UChar *) GetMomImageData(GetMomVopA(curr_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 curr_width = (GetMomVopWidth(curr_vop));
 curr_height = (GetMomVopHeight(curr_vop));

 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 curr       = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
 curr_alpha = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

 curr_pel = 0;

 for(y=0; y<curr_height; y++)
   for(x=0; x<curr_width; x++, curr_pel++)
     {
       ref       [curr_pel] = (SInt)UCref       [curr_pel];
       curr      [curr_pel] = (SInt)UCcurr      [curr_pel];
     }

 curr_pel = 0;

 if ( ref_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)UCref_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)255;
 }

 curr_pel = 0;

 if ( curr_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)UCcurr_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)255;
 }

	 //END


ref_left = (GetMomVopHorSpatRef(ref_vop)) >> 2;
ref_top = (GetMomVopVerSpatRef(ref_vop)) >> 2;
ref_width = (GetMomVopWidth(ref_vop)) >> 2;
ref_height = (GetMomVopHeight(ref_vop)) >> 2;

curr_left = (GetMomVopHorSpatRef(curr_vop)) >> 2;
curr_top = (GetMomVopVerSpatRef(curr_vop)) >> 2;
curr_width = (GetMomVopWidth(curr_vop)) >> 2;
curr_height = (GetMomVopHeight(curr_vop)) >> 2;

ref_P[0] = ref;
curr_P[0] = curr;
ref_alpha_P[0] = ref_alpha;
curr_alpha_P[0] = curr_alpha;
ref_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
ref_alpha_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
curr_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
curr_alpha_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
ref_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
ref_alpha_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
curr_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
curr_alpha_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

ThreeTapFilter(ref_P[1], ref_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(ref_alpha_P[1], ref_alpha_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(curr_P[1], curr_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(curr_alpha_P[1], curr_alpha_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(ref_P[2], ref_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(ref_alpha_P[2], ref_alpha_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(curr_P[2], curr_P[1], curr_width*2, curr_height*2);
ThreeTapFilter(curr_alpha_P[2], curr_alpha_P[1], curr_width*2, curr_height*2);

for(i = 0; i < 2; i++)
	dm[i] = 0;
for(i = 0; i < 2; i++)
	M[i] = 0;

ref_work = ref_P[2];
ref_alpha_work = ref_alpha_P[2];
curr_work = curr_P[2];
curr_alpha_work = curr_alpha_P[2];

best_locationx = 0;
best_locationy = 0;
/* for the absolute coordinate system */
offset_x = curr_left - ref_left;
offset_y = curr_top - ref_top;

threshold_T=ModifiedThreeStepSearch(ref_work, curr_work, 
	ref_alpha_work, curr_alpha_work,
	curr_width, curr_height, ref_width, ref_height,
	offset_x, offset_y,
	&best_locationx, &best_locationy, 4); 



M[0] = (Double)best_locationx;
M[1] = (Double)best_locationy;

for(level = 2; level >= 0; level--){

ref_work = ref_P[level];
ref_alpha_work = ref_alpha_P[level];
curr_work = curr_P[level];
curr_alpha_work = curr_alpha_P[level];

for(i=0;i<256;i++)
	error_histgram[i]=0;

for(ite = 0; ite < 32; ite++){

dE2 = 0.;
stop = 0;
curr_pel = 0;

for(j = 0; j < 4; j++)
	dA[j] = 0;
for(i = 0; i < 2; i++)
	db[i] = 0;

for(y=0; y<curr_height; y++) {
dy = (Double)(y + curr_top); 
for(x=0; x<curr_width; x++, curr_pel++) {

if(curr_alpha_work[curr_pel]) {
dx = (Double)(x + curr_left); 

dt = dx + M[0] ;
du = dy + M[1] ;
dx1 = dt-ref_left;
dy1 = du-ref_top;

		if ( dx1 >= 0.0 )
		    x1 = (Int) dx1;
		else
		    x1 = (Int) dx1 - 1;

		if ( dy1 >= 0.0 )
		    y1 = (Int) dy1;
		else
		    y1 = (Int) dy1 - 1;

if(x1>=0 && (x1+1)<ref_width && y1>=0 && (y1+1)<ref_height) {
ref_pel = x1 + ref_width * y1;
ref_pel1 = x1+1 + ref_width * y1;
ref_pel2 = x1 + ref_width * (y1+1);
ref_pel3 = x1+1 + ref_width * (y1+1);
if(ref_alpha_work[ref_pel] && ref_alpha_work[ref_pel1] &&
	ref_alpha_work[ref_pel2] && ref_alpha_work[ref_pel3]) {
stop++;
dk = dx1 - x1;
d1mk = 1. - dk;
dl = dy1 - y1;
d1ml = 1. -dl;
I1x1y1[0] = ref_work[ref_pel];
I1x1y1[1] = ref_work[ref_pel1];
I1x1y1[2] = ref_work[ref_pel2];
I1x1y1[3] = ref_work[ref_pel3];
dI1 = d1mk*d1ml*I1x1y1[0] + dk*d1ml*I1x1y1[1] +
	d1mk*dl*I1x1y1[2] + dk*dl*I1x1y1[3];
de = dI1 - curr_work[curr_pel];
		if(ite==0)
		{
			if ( (Int)(fabs(de)) > 255 )
				error_histgram[255]++;
			else
				error_histgram[(Int)(fabs(de))]++;
		}
if(fabs(de) <= threshold_T) {
dI1dx = (I1x1y1[1]-I1x1y1[0])*d1ml + (I1x1y1[3]-I1x1y1[2])*dl;
dI1dy = (I1x1y1[2]-I1x1y1[0])*d1mk + (I1x1y1[3]-I1x1y1[1])*dk;
ddedm[0] = dI1dx;
ddedm[1] = dI1dy;
db[0] += -de*ddedm[0];
db[1] += -de*ddedm[1];
for(j=0; j<2; j++)
	for(i=0; i<=j; i++)
		dA[j*2+i] += ddedm[j] * ddedm[i];
dE2 += de*de;
} /* threshold */
} /* limit of ref_luma area */
} /* limit of curr_luma area */
   
} /* limit of curr_alpha value */

} /* x */
} /* y */

if(ite==0){
  stop = stop*80/100;
  j=0;
for(i=0;i<256;i++){
  j+=error_histgram[i];
  if(j > stop){
   threshold_T = i;
   break; 
  }
}
}

for(j=0; j<2; j++)
	for(i=j+1; i<2; i++)
	dA[j*2+i] = dA[i*2+j];

check = DeltaMP(dA, 2, db, dm);

if(check){
for(i=0; i<2; i++)
	M[i] += dm[i];
	if(fabs(dm[0]) < 0.001 && fabs(dm[1]) < 0.001 ) break; 
}else{
 break;
}

} /* iteration */

curr_width *= 2;
curr_height *= 2;
curr_left *= 2;
curr_top *= 2;
ref_width *= 2;
ref_height *= 2;
ref_left *= 2;
ref_top *= 2;
offset_x *= 2;
offset_y *= 2;

if(level){
M[0] *= 2;
M[1] *= 2;
}
} /* level */

//m_parameter -> a1 = 1.0;
//m_parameter -> a2 = 0.0;
//m_parameter -> a3 = M[0];
//m_parameter -> a4 = 0.0;
//m_parameter -> a5 = 1.0;
//m_parameter -> a6 = M[1];
//m_parameter -> a7 = 0.0;
//m_parameter -> a8 = 0.0;

m_parameter -> a1 = M[0];
m_parameter -> a2 = M[1];

for(i=1;i<3;i++) {
free(ref_P[i]);
free(curr_P[i]);
free(ref_alpha_P[i]);
free(curr_alpha_P[i]);
}

}

/***********************************************************CommentBegin******
 *
 * --    IsotropicGME
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose :   generator of global motion parameters for isotropic transform
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM
 *
 *
 ***********************************************************CommentEnd********/
void
IsotropicGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter)
{
UChar *UCref, *UCcurr, *UCcurr_alpha, *UCref_alpha;
SInt *ref, *curr, *curr_alpha, *ref_alpha;
SInt *ref_P[3], *curr_P[3], *ref_alpha_P[3], *curr_alpha_P[3];
SInt *ref_work, *curr_work, *curr_alpha_work, *ref_alpha_work;
Int curr_width, curr_height, curr_left, curr_top;
Int ref_width, ref_height, ref_left, ref_top;

Int i, j, best_locationx, best_locationy;
Int curr_pel, offset_x, offset_y, ref_pel;
Int ref_pel1, ref_pel2, ref_pel3;
Double dm[4], db[4], dA[16], M[4];
Double dE2 = 0;
Int stop = 0, level = 2, ite = 0;
Int x, y, x1, y1;
Double dx1, dy1, dx, dy;
Double dt, du, dk, d1mk, dl, d1ml,
	dI1, de, dI1dx, dI1dy;
Double ddedm[4];
Double I1x1y1[4];
Int error_histgram[256];
Int threshold_T=0, check = 1;

// ref = (SInt *) GetMomImageData(GetMomVopY(ref_vop));
// curr = (SInt *) GetMomImageData(GetMomVopY(curr_vop));
// ref_alpha = (SInt *) GetMomImageData(GetMomVopA(ref_vop));
// curr_alpha = (SInt *) GetMomImageData(GetMomVopA(curr_vop));

//START

 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCcurr = (UChar *) GetMomImageData(GetMomVopY(curr_vop));

 if ( ref_vop->a_chan != NULL )
	 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 if ( curr_vop->a_chan != NULL ) 
	 UCcurr_alpha = (UChar *) GetMomImageData(GetMomVopA(curr_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 curr_width = (GetMomVopWidth(curr_vop));
 curr_height = (GetMomVopHeight(curr_vop));

 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 curr       = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
 curr_alpha = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

 curr_pel = 0;

 for(y=0; y<curr_height; y++)
   for(x=0; x<curr_width; x++, curr_pel++)
     {
       ref       [curr_pel] = (SInt)UCref       [curr_pel];
       curr      [curr_pel] = (SInt)UCcurr      [curr_pel];
     }

 curr_pel = 0;

 if ( ref_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)UCref_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)255;
 }

 curr_pel = 0;

 if ( curr_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)UCcurr_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)255;
 }

	 //END


ref_left = (GetMomVopHorSpatRef(ref_vop)) >> 2;
ref_top = (GetMomVopVerSpatRef(ref_vop)) >> 2;
ref_width = (GetMomVopWidth(ref_vop)) >> 2;
ref_height = (GetMomVopHeight(ref_vop)) >> 2;

curr_left = (GetMomVopHorSpatRef(curr_vop)) >> 2;
curr_top = (GetMomVopVerSpatRef(curr_vop)) >> 2;
curr_width = (GetMomVopWidth(curr_vop)) >> 2;
curr_height = (GetMomVopHeight(curr_vop)) >> 2;

ref_P[0] = ref;
curr_P[0] = curr;
ref_alpha_P[0] = ref_alpha;
curr_alpha_P[0] = curr_alpha;
ref_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
ref_alpha_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
curr_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
curr_alpha_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
ref_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
ref_alpha_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
curr_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
curr_alpha_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

ThreeTapFilter(ref_P[1], ref_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(ref_alpha_P[1], ref_alpha_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(curr_P[1], curr_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(curr_alpha_P[1], curr_alpha_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(ref_P[2], ref_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(ref_alpha_P[2], ref_alpha_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(curr_P[2], curr_P[1], curr_width*2, curr_height*2);
ThreeTapFilter(curr_alpha_P[2], curr_alpha_P[1], curr_width*2, curr_height*2);

for(i = 0; i < 4; i++)
	dm[i] = 0;
for(i = 0; i < 4; i++)
	M[i] = 0;
M[0]=1.0;

ref_work = ref_P[2];
ref_alpha_work = ref_alpha_P[2];
curr_work = curr_P[2];
curr_alpha_work = curr_alpha_P[2];

best_locationx = 0;
best_locationy = 0;
/* for the absolute coordinate system */
offset_x = curr_left - ref_left;
offset_y = curr_top - ref_top;

threshold_T=ModifiedThreeStepSearch(ref_work, curr_work, 
	ref_alpha_work, curr_alpha_work,
	curr_width, curr_height, ref_width, ref_height,
	offset_x, offset_y,
	&best_locationx, &best_locationy,4); 

M[2] = (Double)best_locationx;
M[3] = (Double)best_locationy;

for(level = 2; level >= 0; level--){

ref_work = ref_P[level];
ref_alpha_work = ref_alpha_P[level];
curr_work = curr_P[level];
curr_alpha_work = curr_alpha_P[level];

for(i=0;i<256;i++)
	error_histgram[i]=0;

for(ite = 0; ite < 32; ite++){

dE2 = 0.;
stop = 0;
curr_pel = 0;

for(j = 0; j < 16; j++)
	dA[j] = 0;
for(i = 0; i < 4; i++)
	db[i] = 0;

for(y=0; y<curr_height; y++) {
dy = (Double)(y + curr_top); 
for(x=0; x<curr_width; x++, curr_pel++) {

if(curr_alpha_work[curr_pel]) {
dx = (Double)(x + curr_left); 

dt = M[0] * dx + M[1] * dy + M[2];
du = -M[1] * dx + M[0] * dy + M[3];
dx1 = dt-ref_left;
dy1 = du-ref_top;

		if ( dx1 >= 0.0 )
		    x1 = (Int) dx1;
		else
		    x1 = (Int) dx1 - 1;

		if ( dy1 >= 0.0 )
		    y1 = (Int) dy1;
		else
		    y1 = (Int) dy1 - 1;

if(x1>=0 && (x1+1)<ref_width && y1>=0 && (y1+1)<ref_height) {
ref_pel = x1 + ref_width * y1;
ref_pel1 = x1+1 + ref_width * y1;
ref_pel2 = x1 + ref_width * (y1+1);
ref_pel3 = x1+1 + ref_width * (y1+1);
if(ref_alpha_work[ref_pel] && ref_alpha_work[ref_pel1] &&
	ref_alpha_work[ref_pel2] && ref_alpha_work[ref_pel3]) {
stop++;
dk = dx1 - x1;
d1mk = 1. - dk;
dl = dy1 - y1;
d1ml = 1. -dl;
I1x1y1[0] = ref_work[ref_pel];
I1x1y1[1] = ref_work[ref_pel1];
I1x1y1[2] = ref_work[ref_pel2];
I1x1y1[3] = ref_work[ref_pel3];
dI1 = d1mk*d1ml*I1x1y1[0] + dk*d1ml*I1x1y1[1] +
	d1mk*dl*I1x1y1[2] + dk*dl*I1x1y1[3];
de = dI1 - curr_work[curr_pel];
		if(ite==0)
		{
			if ( (Int)(fabs(de)) > 255 )
				error_histgram[255]++;
			else
				error_histgram[(Int)(fabs(de))]++;
		}
if(fabs(de) <= threshold_T) {
dI1dx = (I1x1y1[1]-I1x1y1[0])*d1ml + (I1x1y1[3]-I1x1y1[2])*dl;
dI1dy = (I1x1y1[2]-I1x1y1[0])*d1mk + (I1x1y1[3]-I1x1y1[1])*dk;
ddedm[0] = dx * dI1dx + dy * dI1dy;
ddedm[1] = dy * dI1dx - dx * dI1dy;
ddedm[2] = dI1dx;
ddedm[3] = dI1dy;
db[0] += -de*ddedm[0];
db[1] += -de*ddedm[1];
db[2] += -de*ddedm[2];
db[3] += -de*ddedm[3];
for(j=0; j<4; j++)
	for(i=0; i<=j; i++)
		dA[j*4+i] += ddedm[j] * ddedm[i];
dE2 += de*de;
} /* threshold */
} /* limit of ref_luma area */
} /* limit of curr_luma area */
   
} /* limit of curr_alpha value */

} /* x */
} /* y */

if(ite==0){
  stop = stop*80/100;
  j=0;
for(i=0;i<256;i++){
  j+=error_histgram[i];
  if(j > stop){
   threshold_T = i;
   break; 
  }
}
}

for(j=0; j<4; j++)
	for(i=j+1; i<4; i++)
	dA[j*4+i] = dA[i*4+j];

check = DeltaMP(dA, 4, db, dm);

if(check){
for(i=0; i<4; i++)
	M[i] += dm[i];

if(fabs(dm[2]) < 0.001 && fabs(dm[3]) < 0.001 && fabs(dm[0]) < 0.00001 &&
   fabs(dm[1]) < 0.00001 ) break;
}else {
break;
}

} /* iteration */

curr_width *= 2;
curr_height *= 2;
curr_left *= 2;
curr_top *= 2;
ref_width *= 2;
ref_height *= 2;
ref_left *= 2;
ref_top *= 2;
offset_x *= 2;
offset_y *= 2;

if(level){
M[2] *= 2;
M[3] *= 2;
}
} /* level */

//m_parameter -> a1 = M[0];
//m_parameter -> a2 = M[1];
//m_parameter -> a3 = M[2];
//m_parameter -> a4 = -M[1];
//m_parameter -> a5 = M[0];
//m_parameter -> a6 = M[3];
//m_parameter -> a7 = 0.0;
//m_parameter -> a8 = 0.0;


m_parameter -> a1 = M[2];
m_parameter -> a2 = M[3];
m_parameter -> a3 = M[0]-1.0;
m_parameter -> a4 = M[1];

for(i=1;i<3;i++) {
free(ref_P[i]);
free(curr_P[i]);
free(ref_alpha_P[i]);
free(curr_alpha_P[i]);
}

}


/***********************************************************CommentBegin******
 *
 * --    AffineGME
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose :   generator of global motion parameters for affine transform
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM
 *
 *
 ***********************************************************CommentEnd********/
void
AffineGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter)
{
UChar *UCref, *UCcurr, *UCcurr_alpha, *UCref_alpha;
  SInt *ref, *curr, *curr_alpha, *ref_alpha;
  SInt *ref_P[3], *curr_P[3], *ref_alpha_P[3], *curr_alpha_P[3];
  SInt *ref_work, *curr_work, *curr_alpha_work, *ref_alpha_work;
  Int curr_width, curr_height, curr_left, curr_top;
  Int ref_width, ref_height, ref_left, ref_top;

  Int i, j, best_locationx, best_locationy;
  Int curr_pel, offset_x, offset_y, ref_pel;
  Int ref_pel1, ref_pel2, ref_pel3;
  Double dm[6], db[6], dA[36], M[6];
  Double dE2 = 0;
  Int stop = 0, level = 2, ite = 0;
  Int x, y, x1, y1;
  Double dx1, dy1, dx, dy;
  Double dt, du, dk, d1mk, dl, d1ml,
	dI1, de, dI1dx, dI1dy;
  Double ddedm[6];
  Double I1x1y1[4];
  Int error_histgram[256];
  Int threshold_T=0, check = 1;

//START

 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCcurr = (UChar *) GetMomImageData(GetMomVopY(curr_vop));

 if ( ref_vop->a_chan != NULL )
	 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 if ( curr_vop->a_chan != NULL ) 
	 UCcurr_alpha = (UChar *) GetMomImageData(GetMomVopA(curr_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 curr_width = (GetMomVopWidth(curr_vop));
 curr_height = (GetMomVopHeight(curr_vop));


 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 curr       = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
 curr_alpha = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

 curr_pel = 0;

 for(y=0; y<curr_height; y++)
   for(x=0; x<curr_width; x++, curr_pel++)
     {
       ref       [curr_pel] = (SInt)UCref       [curr_pel];
       curr      [curr_pel] = (SInt)UCcurr      [curr_pel];
     }

 curr_pel = 0;

 if ( ref_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)UCref_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)255;
 }

 curr_pel = 0;

 if ( curr_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)UCcurr_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)255;
 }

	 //END

  ref_left = (GetMomVopHorSpatRef(ref_vop)) >> 2;
  ref_top = (GetMomVopVerSpatRef(ref_vop)) >> 2;
  ref_width = (GetMomVopWidth(ref_vop)) >> 2;
  ref_height = (GetMomVopHeight(ref_vop)) >> 2;

  curr_left = (GetMomVopHorSpatRef(curr_vop)) >> 2;
  curr_top = (GetMomVopVerSpatRef(curr_vop)) >> 2;
  curr_width = (GetMomVopWidth(curr_vop)) >> 2;
  curr_height = (GetMomVopHeight(curr_vop)) >> 2;

  ref_P[0] = ref;
  curr_P[0] = curr;
  ref_alpha_P[0] = ref_alpha;
  curr_alpha_P[0] = curr_alpha;
  ref_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
  ref_alpha_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
  curr_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
  curr_alpha_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
  ref_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
  ref_alpha_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
  curr_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
  curr_alpha_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

  ThreeTapFilter(ref_P[1], ref_P[0], ref_width*4, ref_height*4);
  ThreeTapFilter(ref_alpha_P[1], ref_alpha_P[0], ref_width*4, ref_height*4);
  ThreeTapFilter(curr_P[1], curr_P[0], curr_width*4, curr_height*4);
  ThreeTapFilter(curr_alpha_P[1], curr_alpha_P[0], curr_width*4, curr_height*4);
  ThreeTapFilter(ref_P[2], ref_P[1], ref_width*2, ref_height*2);
  ThreeTapFilter(ref_alpha_P[2], ref_alpha_P[1], ref_width*2, ref_height*2);
  ThreeTapFilter(curr_P[2], curr_P[1], curr_width*2, curr_height*2);
  ThreeTapFilter(curr_alpha_P[2], curr_alpha_P[1], curr_width*2, curr_height*2);




  for(i = 0; i < 6; i++)
    dm[i] = 0;
  for(i = 0; i < 6; i++)
    M[i] = 0;
  M[0]=1.0;
  M[4]=1.0;

  ref_work = ref_P[2];
  ref_alpha_work = ref_alpha_P[2];
  curr_work = curr_P[2];
  curr_alpha_work = curr_alpha_P[2];

  best_locationx = 0;
  best_locationy = 0;
/* for the absolute coordinate system */
  offset_x = curr_left - ref_left;
  offset_y = curr_top - ref_top;

  threshold_T=ModifiedThreeStepSearch(ref_work, curr_work, 
				      ref_alpha_work, curr_alpha_work,
				      curr_width, curr_height, ref_width, ref_height,
				      offset_x, offset_y,
				      &best_locationx, &best_locationy,4); 



  M[2] = (Double)best_locationx;
  M[5] = (Double)best_locationy;


  for(level = 2; level >= 0; level--){

    ref_work = ref_P[level];
    ref_alpha_work = ref_alpha_P[level];
    curr_work = curr_P[level];
    curr_alpha_work = curr_alpha_P[level];

    for(i=0;i<256;i++)
      error_histgram[i]=0;

    for(ite = 0; ite < 32; ite++){

      dE2 = 0.;
      stop = 0;
      curr_pel = 0;

      for(j = 0; j < 36; j++)
	dA[j] = 0;
      for(i = 0; i < 6; i++)
	db[i] = 0;

      for(y=0; y<curr_height; y++) {
	dy = (Double)(y + curr_top); 
	for(x=0; x<curr_width; x++, curr_pel++) {

	  if(curr_alpha_work[curr_pel]) {
	    dx = (Double)(x + curr_left); 

	    dt = M[0] * dx + M[1] * dy + M[2];
	    du = M[3] * dx + M[4] * dy + M[5];
	    dx1 = dt-ref_left;
	    dy1 = du-ref_top;

		if ( dx1 >= 0.0 )
		    x1 = (Int) dx1;
		else
		    x1 = (Int) dx1 - 1;

		if ( dy1 >= 0.0 )
		    y1 = (Int) dy1;
		else
		    y1 = (Int) dy1 - 1;

	    if(x1>=0 && (x1+1)<ref_width && y1>=0 && (y1+1)<ref_height) {
	      ref_pel = x1 + ref_width * y1;
	      ref_pel1 = x1+1 + ref_width * y1;
	      ref_pel2 = x1 + ref_width * (y1+1);
	      ref_pel3 = x1+1 + ref_width * (y1+1);
	      if(ref_alpha_work[ref_pel] && ref_alpha_work[ref_pel1] &&
		 ref_alpha_work[ref_pel2] && ref_alpha_work[ref_pel3]) {
		stop++;
		dk = dx1 - x1;
		d1mk = 1. - dk;
		dl = dy1 - y1;
		d1ml = 1. -dl;
		I1x1y1[0] = ref_work[ref_pel];
		I1x1y1[1] = ref_work[ref_pel1];
		I1x1y1[2] = ref_work[ref_pel2];
		I1x1y1[3] = ref_work[ref_pel3];
		dI1 = d1mk*d1ml*I1x1y1[0] + dk*d1ml*I1x1y1[1] +
		  d1mk*dl*I1x1y1[2] + dk*dl*I1x1y1[3];
		de = dI1 - curr_work[curr_pel];
		if(ite==0)
		{
			if ( (Int)(fabs(de)) > 255 )
				error_histgram[255]++;
			else
				error_histgram[(Int)(fabs(de))]++;
		}
		if(fabs(de) <= threshold_T) {
		  dI1dx = (I1x1y1[1]-I1x1y1[0])*d1ml + (I1x1y1[3]-I1x1y1[2])*dl;
		  dI1dy = (I1x1y1[2]-I1x1y1[0])*d1mk + (I1x1y1[3]-I1x1y1[1])*dk;
		  ddedm[0] = dx * dI1dx;
		  ddedm[1] = dy * dI1dx;
		  ddedm[2] = dI1dx;
		  ddedm[3] = dx * dI1dy;
		  ddedm[4] = dy * dI1dy;
		  ddedm[5] = dI1dy;
		  db[0] += -de*ddedm[0];
		  db[1] += -de*ddedm[1];
		  db[2] += -de*ddedm[2];
		  db[3] += -de*ddedm[3];
		  db[4] += -de*ddedm[4];
		  db[5] += -de*ddedm[5];
		  for(j=0; j<6; j++)
		    for(i=0; i<=j; i++)
		      dA[j*6+i] += ddedm[j] * ddedm[i];
		  dE2 += de*de;
		} /* threshold */
	      } /* limit of ref_luma area */
	    } /* limit of curr_luma area */
	  } /* limit of curr_alpha value */
	} /* x */
      } /* y */

      if(ite==0){
	stop = stop*80/100;
	j=0;
	for(i=0;i<256;i++){
	  j+=error_histgram[i];
	  if(j > stop){
	    threshold_T = i;
	    break; 
	  }
	}
      }

      for(j=0; j<6; j++)
	for(i=j+1; i<6; i++)
	  dA[j*6+i] = dA[i*6+j];

      check = DeltaMP(dA, 6, db, dm);

      if(check){ 
	for(i=0; i<6; i++)
	  M[i] += dm[i];

	if(fabs(dm[2]) < 0.001 && fabs(dm[5]) < 0.001 && fabs(dm[0]) < 0.00001 &&
	   fabs(dm[1]) < 0.00001 && fabs(dm[3]) < 0.00001 && 
	   fabs(dm[4]) < 0.00001 ) break;
      }else {
	break;
      }
    } /* iteration */

    curr_width *= 2;
    curr_height *= 2;
    curr_left *= 2;
    curr_top *= 2;
    ref_width *= 2;
    ref_height *= 2;
    ref_left *= 2;
    ref_top *= 2;
    offset_x *= 2;
    offset_y *= 2;

    if(level){
      M[2] *= 2;
      M[5] *= 2;
    }
  } /* level */

  m_parameter -> a1 = M[2];
  m_parameter -> a2 = M[5];
  m_parameter -> a3 = M[0]-1.0;
  m_parameter -> a4 = M[1];
  m_parameter -> a5 = M[3];
  m_parameter -> a6 = M[4]-1.0;


  for(i=0;i<3;i++) {
    free(ref_P[i]);
    free(curr_P[i]);
    free(ref_alpha_P[i]);
    free(curr_alpha_P[i]);
  }
}

/***********************************************************CommentBegin******
 *
 * --    PerspectiveGME
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose :   generator of global motion parameters for perspective transform
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *	11/11/99, Aljoscha Smolic, HHI, adaptation to MPEG-7-XM
 *
 *
 ***********************************************************CommentEnd********/
void
PerspectiveGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter)
{
UChar *UCref, *UCcurr, *UCcurr_alpha, *UCref_alpha;
SInt *ref, *curr, *curr_alpha, *ref_alpha;
SInt *ref_P[3], *curr_P[3], *ref_alpha_P[3], *curr_alpha_P[3];
SInt *ref_work, *curr_work, *curr_alpha_work, *ref_alpha_work;
Int curr_width, curr_height, curr_left, curr_top;
Int ref_width, ref_height, ref_left, ref_top;

Int i, j, best_locationx, best_locationy;
Int curr_pel, offset_x, offset_y, ref_pel;
Int ref_pel1, ref_pel2, ref_pel3;
Double dm[8], db[8], dA[64], M[8];
Double dE2 = 0;
Int stop = 0, level = 2, ite = 0;
Int x, y, x1, y1;
Double dx1, dy1, dx, dy;
Double dt, du, dv, dtOv, duOv, dlOv, dxOv, dyOv, dk, d1mk, dl, d1ml,
	dI1, de, dI1dx, dI1dy;
Double ddedm[8];
Double I1x1y1[4];
Int error_histgram[256];
Int threshold_T=0, check = 1;

// ref = (SInt *) GetMomImageData(GetMomVopY(ref_vop));
// curr = (SInt *) GetMomImageData(GetMomVopY(curr_vop));
// ref_alpha = (SInt *) GetMomImageData(GetMomVopA(ref_vop));
// curr_alpha = (SInt *) GetMomImageData(GetMomVopA(curr_vop));

//START

 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCcurr = (UChar *) GetMomImageData(GetMomVopY(curr_vop));

 if ( ref_vop->a_chan != NULL )
	 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 if ( curr_vop->a_chan != NULL ) 
	 UCcurr_alpha = (UChar *) GetMomImageData(GetMomVopA(curr_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 curr_width = (GetMomVopWidth(curr_vop));
 curr_height = (GetMomVopHeight(curr_vop));

 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 curr       = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
 curr_alpha = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

 curr_pel = 0;

 for(y=0; y<curr_height; y++)
   for(x=0; x<curr_width; x++, curr_pel++)
     {
       ref       [curr_pel] = (SInt)UCref       [curr_pel];
       curr      [curr_pel] = (SInt)UCcurr      [curr_pel];
     }

 curr_pel = 0;

 if ( ref_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)UCref_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)255;
 }

 curr_pel = 0;

 if ( curr_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)UCcurr_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)255;
 }

	 //END


ref_left = (GetMomVopHorSpatRef(ref_vop)) >> 2;
ref_top = (GetMomVopVerSpatRef(ref_vop)) >> 2;
ref_width = (GetMomVopWidth(ref_vop)) >> 2;
ref_height = (GetMomVopHeight(ref_vop)) >> 2;

curr_left = (GetMomVopHorSpatRef(curr_vop)) >> 2;
curr_top = (GetMomVopVerSpatRef(curr_vop)) >> 2;
curr_width = (GetMomVopWidth(curr_vop)) >> 2;
curr_height = (GetMomVopHeight(curr_vop)) >> 2;

ref_P[0] = ref;
curr_P[0] = curr;
ref_alpha_P[0] = ref_alpha;
curr_alpha_P[0] = curr_alpha;
ref_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
ref_alpha_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
curr_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
curr_alpha_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
ref_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
ref_alpha_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
curr_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
curr_alpha_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

ThreeTapFilter(ref_P[1], ref_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(ref_alpha_P[1], ref_alpha_P[0], ref_width*4, ref_height*4);
ThreeTapFilter(curr_P[1], curr_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(curr_alpha_P[1], curr_alpha_P[0], curr_width*4, curr_height*4);
ThreeTapFilter(ref_P[2], ref_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(ref_alpha_P[2], ref_alpha_P[1], ref_width*2, ref_height*2);
ThreeTapFilter(curr_P[2], curr_P[1], curr_width*2, curr_height*2);
ThreeTapFilter(curr_alpha_P[2], curr_alpha_P[1], curr_width*2, curr_height*2);

for(i = 0; i < 8; i++)
	dm[i] = 0;
for(i = 0; i < 8; i++)
	M[i] = 0;
M[0]=1.0;
M[4]=1.0;

ref_work = ref_P[2];
ref_alpha_work = ref_alpha_P[2];
curr_work = curr_P[2];
curr_alpha_work = curr_alpha_P[2];

best_locationx = 0;
best_locationy = 0;
/* for the absolute coordinate system */
offset_x = curr_left - ref_left;
offset_y = curr_top - ref_top;

threshold_T=ModifiedThreeStepSearch(ref_work, curr_work, 
	ref_alpha_work, curr_alpha_work,
	curr_width, curr_height, ref_width, ref_height,
	offset_x, offset_y,
	&best_locationx, &best_locationy,4); 


M[2] = (Double)best_locationx;
M[5] = (Double)best_locationy;

for(level = 2; level >= 0; level--){

ref_work = ref_P[level];
ref_alpha_work = ref_alpha_P[level];
curr_work = curr_P[level];
curr_alpha_work = curr_alpha_P[level];

for(i=0;i<256;i++)
	error_histgram[i]=0;

for(ite = 0; ite < 32; ite++){

dE2 = 0.;
stop = 0;
curr_pel = 0;

for(j = 0; j < 64; j++)
	dA[j] = 0;
for(i = 0; i < 8; i++)
	db[i] = 0;

for(y=0; y<curr_height; y++) {
dy = (Double)(y + curr_top); 
for(x=0; x<curr_width; x++, curr_pel++) {

if(curr_alpha_work[curr_pel]) {
dx = (Double)(x + curr_left); 

dt = M[0] * dx + M[1] * dy + M[2];
du = M[3] * dx + M[4] * dy + M[5];
dv = M[6] * dx + M[7] * dy + 1;
dtOv = dt / dv;
duOv = du / dv;
dx1 = dtOv-ref_left;
dy1 = duOv-ref_top;

		if ( dx1 >= 0.0 )
		    x1 = (Int) dx1;
		else
		    x1 = (Int) dx1 - 1;

		if ( dy1 >= 0.0 )
		    y1 = (Int) dy1;
		else
		    y1 = (Int) dy1 - 1;

if(x1>=0 && (x1+1)<ref_width && y1>=0 && (y1+1)<ref_height) {
ref_pel = x1 + ref_width * y1;
ref_pel1 = x1+1 + ref_width * y1;
ref_pel2 = x1 + ref_width * (y1+1);
ref_pel3 = x1+1 + ref_width * (y1+1);
if(ref_alpha_work[ref_pel] && ref_alpha_work[ref_pel1] &&
	ref_alpha_work[ref_pel2] && ref_alpha_work[ref_pel3]) {
stop++;
dlOv = 1 / dv;
dxOv = dx / dv;
dyOv = dy / dv;
dk = dx1 - x1;
d1mk = 1. - dk;
dl = dy1 - y1;
d1ml = 1. -dl;
I1x1y1[0] = ref_work[ref_pel];
I1x1y1[1] = ref_work[ref_pel1];
I1x1y1[2] = ref_work[ref_pel2];
I1x1y1[3] = ref_work[ref_pel3];
dI1 = d1mk*d1ml*I1x1y1[0] + dk*d1ml*I1x1y1[1] +
	d1mk*dl*I1x1y1[2] + dk*dl*I1x1y1[3];
de = dI1 - curr_work[curr_pel];
		if(ite==0)
		{
			if ( (Int)(fabs(de)) > 255 )
				error_histgram[255]++;
			else
				error_histgram[(Int)(fabs(de))]++;
		}
if(fabs(de) <= threshold_T) {
dI1dx = (I1x1y1[1]-I1x1y1[0])*d1ml + (I1x1y1[3]-I1x1y1[2])*dl;
dI1dy = (I1x1y1[2]-I1x1y1[0])*d1mk + (I1x1y1[3]-I1x1y1[1])*dk;
ddedm[0] = dxOv * dI1dx;
ddedm[1] = dyOv * dI1dx;
ddedm[2] = dlOv * dI1dx;
ddedm[3] = dxOv * dI1dy;
ddedm[4] = dyOv * dI1dy;
ddedm[5] = dlOv * dI1dy;
ddedm[6] = -dtOv*dxOv*dI1dx-duOv*dxOv*dI1dy;
ddedm[7] = -dtOv*dyOv*dI1dx-duOv*dyOv*dI1dy;
db[0] += -de*ddedm[0];
db[1] += -de*ddedm[1];
db[2] += -de*ddedm[2];
db[3] += -de*ddedm[3];
db[4] += -de*ddedm[4];
db[5] += -de*ddedm[5];
db[6] += -de*ddedm[6];
db[7] += -de*ddedm[7];
for(j=0; j<8; j++)
	for(i=0; i<=j; i++)
		dA[j*8+i] += ddedm[j] * ddedm[i];
dE2 += de*de;
} /* threshold */
} /* limit of ref_luma area */
} /* limit of curr_luma area */
   
} /* limit of curr_alpha value */

} /* x */
} /* y */

if(ite==0){
  stop = stop*80/100;
  j=0;
for(i=0;i<256;i++){
  j+=error_histgram[i];
  if(j > stop){
   threshold_T = i;
   break; 
  }
}
}

for(j=0; j<8; j++)
	for(i=j+1; i<8; i++)
	dA[j*8+i] = dA[i*8+j];

check = DeltaMP(dA, 8, db, dm);

if(check){
for(i=0; i<8; i++)
	M[i] += dm[i];

if(fabs(dm[2]) < 0.001 && fabs(dm[5]) < 0.001 && fabs(dm[0]) < 0.00001 &&
   fabs(dm[1]) < 0.00001 && fabs(dm[3]) < 0.00001 && fabs(dm[4]) < 0.00001 &&
   fabs(dm[6]) < 0.00001 && fabs(dm[7]) < 0.00001) break;
}else {
break;
}

} /* iteration */

curr_width *= 2;
curr_height *= 2;
curr_left *= 2;
curr_top *= 2;
ref_width *= 2;
ref_height *= 2;
ref_left *= 2;
ref_top *= 2;
offset_x *= 2;
offset_y *= 2;

if(level){
M[2] *= 2;
M[5] *= 2;
M[6] /= 2;
M[7] /= 2;
}
} /* level */

  m_parameter -> a1 = M[2];
  m_parameter -> a2 = M[5];
  m_parameter -> a3 = M[0]-1.0;
  m_parameter -> a4 = M[1];
  m_parameter -> a5 = M[3];
  m_parameter -> a6 = M[4]-1.0;
	m_parameter -> a7 = M[6];
	m_parameter -> a8 = M[7];


for(i=1;i<3;i++) {
free(ref_P[i]);
free(curr_P[i]);
free(ref_alpha_P[i]);
free(curr_alpha_P[i]);
}

}

/***********************************************************CommentBegin******
 *
 * --    ParabolicGME
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 * Author : Aljoscha Smolic  : Heinrich-Hertz-Institute
 *
 * Created : 26-05-98
 * Created : 11-11-99
 *
 * Purpose :   generator of global motion parameters for parabolic transform
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/
void
ParabolicGME(MomVop *ref_vop, MomVop *curr_vop,
				Sprite_motion *m_parameter)
{
UChar *UCref, *UCcurr, *UCcurr_alpha, *UCref_alpha;
  SInt *ref, *curr, *curr_alpha, *ref_alpha;
  SInt *ref_P[3], *curr_P[3], *ref_alpha_P[3], *curr_alpha_P[3];
  SInt *ref_work, *curr_work, *curr_alpha_work, *ref_alpha_work;
  Int curr_width, curr_height, curr_left, curr_top;
  Int ref_width, ref_height, ref_left, ref_top;

  Int i, j, best_locationx, best_locationy;
  Int curr_pel, offset_x, offset_y, ref_pel;
  Int ref_pel1, ref_pel2, ref_pel3;
  Double dm[12], db[12], dA[144], M[12];
  Double dE2 = 0;
  Int stop = 0, level = 2, ite = 0;
  Int x, y, x1, y1;
  Double dx1, dy1, dx, dy;
  Double dt, du, dk, d1mk, dl, d1ml,
	dI1, de, dI1dx, dI1dy;
  Double ddedm[12];
  Double I1x1y1[4];
  Int error_histgram[256];
  Int threshold_T=0, check = 1;

//   ref = (SInt *) GetMomImageData(GetMomVopY(ref_vop));
//   curr = (SInt *) GetMomImageData(GetMomVopY(curr_vop));
//   ref_alpha = (SInt *) GetMomImageData(GetMomVopA(ref_vop));
//   curr_alpha = (SInt *) GetMomImageData(GetMomVopA(curr_vop));

//START

 UCref = (UChar *) GetMomImageData(GetMomVopY(ref_vop));
 UCcurr = (UChar *) GetMomImageData(GetMomVopY(curr_vop));

 if ( ref_vop->a_chan != NULL )
	 UCref_alpha = (UChar *) GetMomImageData(GetMomVopA(ref_vop));

 if ( curr_vop->a_chan != NULL ) 
	 UCcurr_alpha = (UChar *) GetMomImageData(GetMomVopA(curr_vop));

 ref_width = (GetMomVopWidth(ref_vop));
 ref_height = (GetMomVopHeight(ref_vop));

 curr_width = (GetMomVopWidth(curr_vop));
 curr_height = (GetMomVopHeight(curr_vop));

 ref        = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 ref_alpha  = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
 curr       = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
 curr_alpha = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

 curr_pel = 0;

 for(y=0; y<curr_height; y++)
   for(x=0; x<curr_width; x++, curr_pel++)
     {
       ref       [curr_pel] = (SInt)UCref       [curr_pel];
       curr      [curr_pel] = (SInt)UCcurr      [curr_pel];
     }

 curr_pel = 0;

 if ( ref_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)UCref_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   ref_alpha [curr_pel] = (SInt)255;
 }

 curr_pel = 0;

 if ( curr_vop->a_chan != NULL )
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)UCcurr_alpha [curr_pel];
 }
 else
 {
   for(y=0; y<curr_height; y++)
	 for(x=0; x<curr_width; x++, curr_pel++)
	   curr_alpha [curr_pel] = (SInt)255;
 }

	 //END


  ref_left = (GetMomVopHorSpatRef(ref_vop)) >> 2;
  ref_top = (GetMomVopVerSpatRef(ref_vop)) >> 2;
  ref_width = (GetMomVopWidth(ref_vop)) >> 2;
  ref_height = (GetMomVopHeight(ref_vop)) >> 2;

  curr_left = (GetMomVopHorSpatRef(curr_vop)) >> 2;
  curr_top = (GetMomVopVerSpatRef(curr_vop)) >> 2;
  curr_width = (GetMomVopWidth(curr_vop)) >> 2;
  curr_height = (GetMomVopHeight(curr_vop)) >> 2;

  ref_P[0] = ref;
  curr_P[0] = curr;
  ref_alpha_P[0] = ref_alpha;
  curr_alpha_P[0] = curr_alpha;
  ref_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
  ref_alpha_P[1] = (SInt *)malloc(ref_width*ref_height*4*sizeof(SInt));
  curr_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
  curr_alpha_P[1] = (SInt *)malloc(curr_width*curr_height*4*sizeof(SInt));
  ref_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
  ref_alpha_P[2] = (SInt *)malloc(ref_width*ref_height*sizeof(SInt));
  curr_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));
  curr_alpha_P[2] = (SInt *)malloc(curr_width*curr_height*sizeof(SInt));

  ThreeTapFilter(ref_P[1], ref_P[0], ref_width*4, ref_height*4);
  ThreeTapFilter(ref_alpha_P[1], ref_alpha_P[0], ref_width*4, ref_height*4);
  ThreeTapFilter(curr_P[1], curr_P[0], curr_width*4, curr_height*4);
  ThreeTapFilter(curr_alpha_P[1], curr_alpha_P[0], curr_width*4, curr_height*4);
  ThreeTapFilter(ref_P[2], ref_P[1], ref_width*2, ref_height*2);
  ThreeTapFilter(ref_alpha_P[2], ref_alpha_P[1], ref_width*2, ref_height*2);
  ThreeTapFilter(curr_P[2], curr_P[1], curr_width*2, curr_height*2);
  ThreeTapFilter(curr_alpha_P[2], curr_alpha_P[1], curr_width*2, curr_height*2);

  for(i = 0; i < 12; i++)
    dm[i] = 0;
  for(i = 0; i < 12; i++)
    M[i] = 0;
  M[0]=1.0;
  M[4]=1.0;

  ref_work = ref_P[2];
  ref_alpha_work = ref_alpha_P[2];
  curr_work = curr_P[2];
  curr_alpha_work = curr_alpha_P[2];

  best_locationx = 0;
  best_locationy = 0;
/* for the absolute coordinate system */
  offset_x = curr_left - ref_left;
  offset_y = curr_top - ref_top;

  threshold_T=ModifiedThreeStepSearch(ref_work, curr_work, 
				      ref_alpha_work, curr_alpha_work,
				      curr_width, curr_height, ref_width, ref_height,
				      offset_x, offset_y,
				      &best_locationx, &best_locationy, 4); 

  M[2] = (Double)best_locationx;
  M[5] = (Double)best_locationy;

  for(level = 2; level >= 0; level--){
    //for(level = -1; level >= 0; level--){

    ref_work = ref_P[level];
    ref_alpha_work = ref_alpha_P[level];
    curr_work = curr_P[level];
    curr_alpha_work = curr_alpha_P[level];

    for(i=0;i<256;i++)
      error_histgram[i]=0;

    for(ite = 0; ite < 32; ite++){

      dE2 = 0.;
      stop = 0;
      curr_pel = 0;

      for(j = 0; j < 144; j++)
	dA[j] = 0;
      for(i = 0; i < 12; i++)
	db[i] = 0;

      for(y=0; y<curr_height; y++) {
	dy = (Double)(y + curr_top); 
	for(x=0; x<curr_width; x++, curr_pel++) {

	  if(curr_alpha_work[curr_pel]) {
	    dx = (Double)(x + curr_left); 

	    dt = M[0] * dx + M[1] * dy + M[2] + M[6] * dx * dx + M[ 7] * dy * dy + M[ 8] * dx * dy;
	    du = M[3] * dx + M[4] * dy + M[5] + M[9] * dx * dx + M[10] * dy * dy + M[11] * dx * dy;
	    dx1 = dt-ref_left;
	    dy1 = du-ref_top;

		if ( dx1 >= 0.0 )
		    x1 = (Int) dx1;
		else
		    x1 = (Int) dx1 - 1;

		if ( dy1 >= 0.0 )
		    y1 = (Int) dy1;
		else
		    y1 = (Int) dy1 - 1;

	    if(x1>=0 && (x1+1)<ref_width && y1>=0 && (y1+1)<ref_height) {
	      ref_pel = x1 + ref_width * y1;
	      ref_pel1 = x1+1 + ref_width * y1;
	      ref_pel2 = x1 + ref_width * (y1+1);
	      ref_pel3 = x1+1 + ref_width * (y1+1);
	      if(ref_alpha_work[ref_pel] && ref_alpha_work[ref_pel1] &&
		 ref_alpha_work[ref_pel2] && ref_alpha_work[ref_pel3]) {
		stop++;
		dk = dx1 - x1;
		d1mk = 1. - dk;
		dl = dy1 - y1;
		d1ml = 1. -dl;
		I1x1y1[0] = ref_work[ref_pel];
		I1x1y1[1] = ref_work[ref_pel1];
		I1x1y1[2] = ref_work[ref_pel2];
		I1x1y1[3] = ref_work[ref_pel3];
		dI1 = d1mk*d1ml*I1x1y1[0] + dk*d1ml*I1x1y1[1] +
		  d1mk*dl*I1x1y1[2] + dk*dl*I1x1y1[3];
		de = dI1 - curr_work[curr_pel];
		if(ite==0)
		{
			if ( (Int)(fabs(de)) > 255 )
				error_histgram[255]++;
			else
				error_histgram[(Int)(fabs(de))]++;
		}
		if(fabs(de) <= threshold_T) {
		  dI1dx = (I1x1y1[1]-I1x1y1[0])*d1ml + (I1x1y1[3]-I1x1y1[2])*dl;
		  dI1dy = (I1x1y1[2]-I1x1y1[0])*d1mk + (I1x1y1[3]-I1x1y1[1])*dk;

		  ddedm[ 0] = dx * dI1dx;
		  ddedm[ 1] = dy * dI1dx;
		  ddedm[ 2] = dI1dx;
		  ddedm[ 3] = dx * dI1dy;
		  ddedm[ 4] = dy * dI1dy;
		  ddedm[ 5] = dI1dy;
		  ddedm[ 6] = dx * dx * dI1dx;
		  ddedm[ 7] = dy * dy * dI1dx;
		  ddedm[ 8] = dx * dy * dI1dx;
		  ddedm[ 9] = dx * dx * dI1dy;
		  ddedm[10] = dy * dy * dI1dy;
		  ddedm[11] = dx * dy * dI1dy;

		  db[ 0] += -de*ddedm[ 0];
		  db[ 1] += -de*ddedm[ 1];
		  db[ 2] += -de*ddedm[ 2];
		  db[ 3] += -de*ddedm[ 3];
		  db[ 4] += -de*ddedm[ 4];
		  db[ 5] += -de*ddedm[ 5];
		  db[ 6] += -de*ddedm[ 6];
		  db[ 7] += -de*ddedm[ 7];
		  db[ 8] += -de*ddedm[ 8];
		  db[ 9] += -de*ddedm[ 9];
		  db[10] += -de*ddedm[10];
		  db[11] += -de*ddedm[11];

		  for(j=0; j<12; j++)
		    for(i=0; i<=j; i++)
		      dA[j*12+i] += ddedm[j] * ddedm[i];
		  dE2 += de*de;

		} /* threshold */
	      } /* limit of ref_luma area */
	    } /* limit of curr_luma area */
	  } /* limit of curr_alpha value */
	} /* x */
      } /* y */

      if(ite==0){
	stop = stop*80/100;
	j=0;
	for(i=0;i<256;i++){
	  j+=error_histgram[i];
	  if(j > stop){
	    threshold_T = i;
	    break; 
	  }
	}
      }

      for(j=0; j<12; j++)
	for(i=j+1; i<12; i++)
	  dA[j*12+i] = dA[i*12+j];

      check = DeltaMP(dA, 12, db, dm);

      if(check){ 
	for(i=0; i<12; i++)
	  M[i] += dm[i];

	if(fabs(dm[ 2]) < 0.001   && fabs(dm[ 5]) < 0.001   && fabs(dm[0]) < 0.00001 &&
	   fabs(dm[ 1]) < 0.00001 && fabs(dm[ 3]) < 0.00001 && 
	   fabs(dm[ 4]) < 0.00001 && fabs(dm[ 6]) < 0.00001 && 
	   fabs(dm[ 7]) < 0.00001 && fabs(dm[ 8]) < 0.00001 && 
	   fabs(dm[ 9]) < 0.00001 && fabs(dm[10]) < 0.00001 && 
	   fabs(dm[11]) < 0.00001 ) break;
      }else {
	break;
      }
    } /* iteration */

    curr_width *= 2;
    curr_height *= 2;
    curr_left *= 2;
    curr_top *= 2;
    ref_width *= 2;
    ref_height *= 2;
    ref_left *= 2;
    ref_top *= 2;
    offset_x *= 2;
    offset_y *= 2;

    if(level){
      M[2] *= 2;
      M[5] *= 2;

      M[ 6] /= 2;
      M[ 7] /= 2;
      M[ 8] /= 2;
      M[ 9] /= 2;
      M[10] /= 2;
      M[11] /= 2;
    }
  } /* level */

  m_parameter -> a1  = M[ 2];
  m_parameter -> a2  = M[ 5];
  m_parameter -> a3  = M[ 0]-1.0;
  m_parameter -> a4  = M[ 1];
  m_parameter -> a5  = M[ 3];
  m_parameter -> a6  = M[ 4]-1.0;
  m_parameter -> a7  = M[ 8];
  m_parameter -> a8  = M[11];
  m_parameter -> a9  = M[ 6];
  m_parameter -> a10 = M[ 7];
  m_parameter -> a11 = M[ 9];
  m_parameter -> a12 = M[10];

  for(i=1;i<3;i++) {
    free(ref_P[i]);
    free(curr_P[i]);
    free(ref_alpha_P[i]);
    free(curr_alpha_P[i]);
  }
}

/***********************************************************CommentBegin******
 *
 * --    ModifiedThreeStepSearch
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose :   coarse estimate for translation component of displacement 
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/
Int
ModifiedThreeStepSearch(SInt *ref_work, SInt *curr_work, 
	SInt *ref_alpha_work, SInt *curr_alpha_work,
	Int curr_width, Int curr_height, Int ref_width, Int ref_height,
	Int offset_x, Int offset_y,
	Int *best_locationx, Int *best_locationy, 
	Int SquareRange) 
{
Int i,j, /*range,*/ locationx, locationy, no_of_pel=0;
Int curr_pel, ref_pel, ref_x, ref_y;
Int range_locationx=0, range_locationy=0;
Int thrs=255,total_no;
Double dE1 = 0, min_error=300;
Int hist[256],round=0, last_thrs;
Int error;

do{

last_thrs=thrs;

range_locationx=*best_locationx;
range_locationy=*best_locationy;
for (locationy = range_locationy-SquareRange; locationy <= range_locationy+SquareRange; 
		locationy ++) {
for (locationx = range_locationx-SquareRange; locationx <= range_locationx+SquareRange; 
		locationx ++) {
dE1 = 0.0;
no_of_pel = 0;
total_no = 0;
curr_pel = 0;
for (j = 0; j< curr_height ; j++) {
for (i = 0; i< curr_width ; i++, curr_pel++) {

if (curr_alpha_work[curr_pel]) {  

ref_x = locationx + i + offset_x;
ref_y = locationy + j + offset_y;
ref_pel = ref_y * ref_width + ref_x;

if ((ref_x >=  0) && (ref_x < ref_width) &&
	(ref_y >=  0) && (ref_y < ref_height) &&
	ref_alpha_work[ref_pel]) {

if((error=GME_ABS(curr_work[curr_pel] - ref_work[ref_pel]))<=last_thrs){
dE1 += error;
no_of_pel ++;
}
total_no++;

} /* limit of ref area */
} /* limit of curr_alpha value */
} /* i */
} /* j */

if (no_of_pel > 0)
	dE1 = dE1 / no_of_pel;
else
	dE1 = min_error+1;
if (((dE1 < min_error) && (no_of_pel*2>total_no)) || 
	((dE1 == min_error) && (no_of_pel*2>total_no) && (GME_ABS(*best_locationx) + 
	GME_ABS(*best_locationy)) > (GME_ABS(locationx) + GME_ABS(locationy)))) {
min_error = dE1;
*best_locationx = locationx;
*best_locationy = locationy;
}

} /* locationx */
} /* locationy */

for (j = 0; j< 256 ; j++)
    hist[j]=0;

no_of_pel=0;
curr_pel = 0;
for (j = 0; j< curr_height ; j++) {
for (i = 0; i< curr_width ; i++, curr_pel++) {

if (curr_alpha_work[curr_pel]) {  

ref_x = *best_locationx + i + offset_x;
ref_y = *best_locationy + j + offset_y;
ref_pel = ref_y * ref_width + ref_x;

if ((ref_x >=  0) && (ref_x < ref_width) &&
	(ref_y >=  0) && (ref_y < ref_height) &&
	ref_alpha_work[ref_pel]) {

hist[GME_ABS(curr_work[curr_pel] - ref_work[ref_pel])]++;
no_of_pel ++;

} /* limit of ref area */
} /* limit of curr_alpha value */
} /* i */
} /* j */

no_of_pel=(Int)(no_of_pel*0.8);
j=0;
for (i=0;i<256;i++){
	j+=hist[i];
	if(j>no_of_pel){thrs=i;break;}
}	

round++;

}while(thrs!=last_thrs && round<5);

return(thrs);

}

/***********************************************************CommentBegin******
 *
 * --    ThreeTapFilter
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 26-05-98
 *
 * Purpose :   3-tap filter with coefficients [1/4 1/2 1/4]
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/
void ThreeTapFilter(SInt *Low, SInt *Hight, Int width, Int height)
{
Int i, j, hwidth = width/2, wwidth = width*2, width2 = width+2;
SInt *lt, *ct, *rt,
     *lc, *cc, *rc,
     *lb, *cb, *rb, *p;

p=Low; lt=Hight; ct=Hight; rt=Hight+1; 
       lc=Hight; cc=Hight; rc=Hight+1; 
       lb=lc+width; cb=cc+width; rb=rc+width;
   *p = (*lt + (*ct<<1) + *rt + 
         (*lc<<1) + (*cc<<2) + (*rc<<1) + 
          *lb + (*cb<<1) + *rb + 8) >> 4;

p=Low+1; lt=Hight+1; ct=Hight+2; rt=Hight+3; 
         lc=Hight+1; cc=Hight+2; rc=Hight+3; 
         lb=lc+width; cb=cc+width; rb=rc+width;
for(i=1; i<width/2; i++, p++, lt+=2, ct+=2, rt+=2,
                              lc+=2, cc+=2, rc+=2,
                              lb+=2, cb+=2, rb+=2)
   *p = (*lt + (*ct<<1) + *rt + 
         (*lc<<1) + (*cc<<2) + (*rc<<1) + 
          *lb + (*cb<<1) + *rb + 8) >> 4;

p=Low+hwidth; lt=Hight+width; ct=Hight+width; rt=Hight+width+1; 
              lc=lt+width; cc=ct+width; rc=rt+width; 
              lb=lc+width; cb=cc+width; rb=rc+width;
for(i=1; i<height/2; i++, p+=hwidth, lt+=wwidth, ct+=wwidth, rt+=wwidth,
                                     lc+=wwidth, cc+=wwidth, rc+=wwidth,
                                     lb+=wwidth, cb+=wwidth, rb+=wwidth)
   *p = (*lt + (*ct<<1) + *rt + 
         (*lc<<1) + (*cc<<2) + (*rc<<1) + 
          *lb + (*cb<<1) + *rb + 8) >> 4;

p=Low+hwidth+1; lt=Hight+1; ct=Hight+2; rt=Hight+3; 
                lc=lt+width; cc=ct+width; rc=rt+width; 
                lb=lc+width; cb=cc+width; rb=rc+width;
for(j=1; j<height/2; j++, p++, lt+=width2, ct+=width2, rt+=width2,
                          lc+=width2, cc+=width2, rc+=width2,
                          lb+=width2, cb+=width2, rb+=width2)
for(i=1; i<hwidth; i++, p++, lt+=2, ct+=2, rt+=2,
                              lc+=2, cc+=2, rc+=2,
                              lb+=2, cb+=2, rb+=2)
   *p = (*lt + (*ct<<1) + *rt + 
         (*lc<<1) + (*cc<<2) + (*rc<<1) + 
          *lb + (*cb<<1) + *rb + 8) >> 4;

}

/***********************************************************CommentBegin******
 *
 * --    DeltaMP
 *
 * Author : Yoshinori Suzuki : Hitachi, Ltd.
 *
 * Created : 07-06-98
 *
 * Purpose :   generating the inverse matrix 
 *
 * Arguments in :
 *
 * Arguments in / out :
 *
 * Return values :
 *  None
 * Side effects :
 *
 *
 * Description :
 *
 * See also :
 *
 * Modified :
 *
 *
 ***********************************************************CommentEnd********/
Int
DeltaMP(Double *dA, Int n, Double *db, Double *dm)
{
int i, j, i2, n2=n*2, k, i_pivot;
Double *dAi = (Double *)malloc(n * n * 2 * sizeof(Double));
Double pivot, tmp;

pivot = *dA;
for(j = 0; j < n; j++)
for(i = 0; i < n; i++)
/*if(fabs(*(dA + j * n2 + i)) > fabs(pivot))*/
if(fabs(*(dA + j * n + i)) > fabs(pivot))
	pivot = *(dA + j * n + i);

if(fabs(pivot) < 0.000000001) {
free(dAi);
return(0);
}
pivot = 1.0 / pivot; 

for(j = 0; j < n; j++)
for(i = 0, i2 = n; i < n; i++, i2++){
	*(dAi + j * n2 + i) = (*(dA + j * n + i))*pivot;
	if(i == j)
		*(dAi + j * n2 + i2) = pivot;
	else
		*(dAi + j * n2 + i2) = 0.0;
}

for(i = 0; i < n; i++) {

i_pivot = i;
pivot = *(dAi + i * n2 + i);
for(j = i; j < n; j++)
if(fabs(*(dAi + j * n2 + i)) > fabs(pivot))
{
	i_pivot = j;
	pivot = *(dAi + j * n2 + i);
}
if(fabs(pivot) < 0.000000001) {
free(dAi);
return(0);
}
if(i_pivot!=i)
for(k = 0; k < n2; k++) {
	tmp = *(dAi + i * n2 + k);
        *(dAi + i * n2 + k) = *(dAi + i_pivot * n2 + k);
        *(dAi + i_pivot * n2 + k) = tmp;
}

for(j = 0; j < n; j++) {
if(j != i)
{
pivot = *(dAi + j * n2 + i) / *(dAi + i * n2 + i);
for(k = i+1; k < n2; k++)
*(dAi + j * n2 + k) -=
	pivot*(*(dAi + i * n2 + k));
}
}

}

for(j = 0; j < n; j++)
for(i = 0, i2 = n; i < n; i++, i2++)
	*(dAi + j * n2 + i2) /= *(dAi + j * n2 + j);

for(i = 0; i < n; i++)
	*(dm + i) = 0.0;

for(j = 0; j < n; j++)
for(i = 0, i2 = n; i < n; i++, i2++)
	*(dm + j) += (*(dAi + j * n2 +i2))*(*(db + i));

free(dAi);
return(1);
}

