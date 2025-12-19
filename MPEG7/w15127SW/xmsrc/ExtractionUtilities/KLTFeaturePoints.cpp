//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// Yousri Abdeljaoued (EPFL Switzerland)
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
// Applicable File Name:  KLTFeaturePoints.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef _VISUALC_
#include <unistd.h>
#include <netinet/in.h>
#else
#include <io.h>
#include <winsock2.h>
//#define ntohl(n) (n)	//sth should be swapped
//#define htonl(h) (h)	//sth should be swapped
#endif

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
#endif

#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "Utilities/readparam.h"
#include "vopio.h"

#include "ExtractionUtilities/KLTFeaturePoints.h"

#define SW_DENOISE 1
#define SETMARK 127
#define XBORDER 24
#define YBORDER 24
#define MAXFPFILENAMELEN 256

void pix_median (TPixel * res, TPixel * in, TCoor x, TCoor y);
void pix_median_f (TPixel * res, TPixel * in, TCoor x, TCoor y);
void pix_grad_matrix (TPixel * res, TPixel * in, TCoor x, TCoor y);
void pix_grad_matrix_f (TPixel * res, TPixel * in, TCoor x, TCoor y);
void pix_corner_to_list (TPixel * res, TPixel * in, TCoor x, TCoor y);
void pix_corner_to_list_f (TPixel * res, TPixel * in, TCoor x, TCoor y);
#ifdef XYUVDISP
void float_to_int(TPixel *res, TPixel *in, TCoor x, TCoor y);
#endif

//=============================================================================
#define  ydenoise(outvop,invop) \
	intra_proc(outvop,invop,0, \
	           Y_CH,Y_CH/*|U_CH|V_CH*/,CON_8,SCAN_HO, \
	           0,0,0,0,0, \
	           -1,-1,0,0,0,0, \
	           size1to1,(m_FloatArith)?&pix_median_f:&pix_median);

#define  sqrgrad(outvop,invop) \
	intra_proc(outvop,invop,0, \
	           Y_CH|U_CH|V_CH,Y_CH/*|U_CH|V_CH*/,CON_8,SCAN_HO, \
	           0,0,0,0,0, \
	           -1,-1,0,0,0,0, \
	           size1to1, \
		   (m_FloatArith)?&pix_grad_matrix_f:&pix_grad_matrix);

#define  gencornerlist(invop,maxval,minval,listsize,list) \
	_intra_infopt=(void *)list; \
	_intra_regin1=maxval; \
	_intra_regin2=minval; \
	_intra_regin3=*(listsize); \
	_intra_regout1=0; \
	intra_proc(invop,invop,0, \
	           0,Y_CH|U_CH|V_CH,CON_8,SCAN_HO, \
	           0,0,0,0,0, \
	           invop->width-(XBORDER<<1),invop->height-(YBORDER<<1), \
		   XBORDER,YBORDER,XBORDER,YBORDER, \
	           size1to1, \
                   (m_FloatArith)?&pix_corner_to_list_f:&pix_corner_to_list); \
        *(listsize)=_intra_regout1;

#define markwindow(vop,x,y,range,value) \
	_intra_regin1=value; \
        { \
	  int x1=((x-range<0) ? 0 : (x-range)); \
	  int y1=((y-range<0) ? 0 : (y-range)); \
	  int x2=((x+range>(vop->width-1)) ? (vop->width-1) : (x+range)); \
	  int y2=((y+range>(vop->height-1))? (vop->height-1): (y+range)); \
	  intra_proc(vop,0,0, \
		     A_CH,0,CON_24,SCAN_HO, \
		     0,0,0,0,0, \
		     x2-x1+1,y2-y1+1,x1,y1,x1,y1, \
		     size1to1,&pix_set_value); \
	}

#ifdef XYUVDISP
#define convtoint(outvop,invop,channels) \
            intra_proc(outvop,invop,0, \
                       channels, \
                       channels, \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       -1,-1,0,0,0,0,size1to1,&float_to_int)
#endif

static void _kltquicksort (TKLTFeature * pointlist, int n);

//=============================================================================
void pix_median (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  TChan order[9];
  int numbers[9] = { 6, 7, 8, 11, 12, 13, 16, 17, 18 };
  int i, i2, i3, val;
  TChan aktual;

  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->u = in[12].u;
  res->v = in[12].v;
  res->ax = in[12].ax;

  for (i = 0, val = 0; i < 9; i++) {
    aktual = in[numbers[i]].y.i;
    for (i2 = 0; i2 < val; i2++) {
      if (aktual < order[i2]) {
	for (i3 = val; i3 > i2; i3--) {
	  order[i3] = order[i3 - 1];
	}
	break;
      }
    }
    order[i2] = aktual;
    val++;
  }
  res->y.i = order[val / 2];
}

//----------------------------------------------------------------------------
void pix_median_f (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  float order[9];
  int numbers[9] = { 6, 7, 8, 11, 12, 13, 16, 17, 18 };
  int i, i2, i3, val;
  float aktual;

  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->u = in[12].u;
  res->v = in[12].v;
  res->ax = in[12].ax;

  for (i = 0, val = 0; i < 9; i++) {
    aktual = in[numbers[i]].y.f;
    for (i2 = 0; i2 < val; i2++) {
      if (aktual < order[i2]) {
	for (i3 = val; i3 > i2; i3--) {
	  order[i3] = order[i3 - 1];
	}
	break;
      }
    }
    order[i2] = aktual;
    val++;
  }
  res->y.f = order[val / 2];
}

//----------------------------------------------------------------------------
void pix_grad_matrix (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  /* results:
     y = gradx(lum)^2
     u = grady(lum)^2
     v = gradx(lum)*grady(lum)
     result range has to be short type */


  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->ax = in[12].ax;

#define SQR(x) ((x)*(x))
  // sqr of grad in horizontal direction
  res->y.i = SQR (in[8].y.i - in[6].y.i +
		  in[13].y.i - in[11].y.i +
		  in[18].y.i - in[16].y.i) >> 3;
  // sqr of grad in vertical direction
  res->u.i = SQR (in[16].y.i - in[6].y.i +
		  in[17].y.i - in[7].y.i +
		  in[18].y.i - in[8].y.i) >> 3;
#undef SQR
  // horizontal grad * vertical grad
  res->v.i = ((in[8].y.i - in[6].y.i +
	       in[13].y.i - in[11].y.i +
	       in[18].y.i - in[16].y.i) *
	      (in[16].y.i - in[6].y.i +
	       in[17].y.i - in[7].y.i +
	       in[18].y.i - in[8].y.i)) >> 3;
}

//----------------------------------------------------------------------------
void pix_grad_matrix_f (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  /* results:
     y = gradx(lum)^2
     u = grady(lum)^2
     v = gradx(lum)*grady(lum)
     result range has to be short type */


  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->ax = in[12].ax;

#define SQR(x) ((x)*(x))
  // sqr of grad in horizontal direction
  res->y.f = SQR (in[8].y.f - in[6].y.f +
		  in[13].y.f - in[11].y.f +
		  in[18].y.f - in[16].y.f)/8.0;
  // sqr of grad in vertical direction
  res->u.f = SQR (in[16].y.f - in[6].y.f +
		  in[17].y.f - in[7].y.f +
		  in[18].y.f - in[8].y.f)/8.0;
#undef SQR
  // horizontal grad * vertical grad
  res->v.f = ((in[8].y.f - in[6].y.f +
	       in[13].y.f - in[11].y.f +
	       in[18].y.f - in[16].y.f) *
	      (in[16].y.f - in[6].y.f +
	       in[17].y.f - in[7].y.f +
	       in[18].y.f - in[8].y.f))/8.0;
}

//----------------------------------------------------------------------------
void pix_corner_to_list (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  int pos;
  long sumxx, sumyy, sumxy;
  long eigenvalue;


  /* _intra_infopt=(void *)list;
     _intra_regin1=maxval;
     _intra_regin2=minval;
     _intra_regin3=listsize;
     listsize=_intra_regout1; */


  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->y = in[12].y;
  res->u = in[12].u;
  res->v = in[12].v;
  res->ax = in[12].ax;

  sumxx = 0;
  sumyy = 0;
  sumxy = 0;

  for (pos = 0; pos < 25; pos++) {
    sumxx += in[pos].y.i;
    sumyy += in[pos].u.i;
    sumxy += in[pos].v.i;
  }

  /* compute lower eigenvalue of:
     sumxx, sumxy
     sumxy, sumyy */

#define SQR(x) ((x)*(x))
//   eigenvalue=(sumxx+sumyy -
//            (long)(sqrt(((double)(SQR(sumxx-sumyy)+(SQR(sumxy)<<2))))+0.5)+
//            1)>>1;
  eigenvalue = SQR (sumxx - sumyy);
  eigenvalue += (SQR (sumxy) << 2);
  eigenvalue = (-((long) (sqrt ((double) eigenvalue) + 0.5)));
  eigenvalue += sumxx;
  eigenvalue += sumyy;
  eigenvalue++;
  eigenvalue >>= 1;
  /* last shift is only for scaling!! => can be removed */
#undef SQR

  if (eigenvalue < _intra_regin2) {
    return;
  }

  if (eigenvalue > _intra_regin1) {
    eigenvalue = _intra_regin1;
#ifdef DEBUG
    fprintf (stderr, "Warning: overflow of eigenvalue\n");
#endif
  }

  /* write to list */
  (((TKLTFeature *) (_intra_infopt)))->x = (int) x;
  (((TKLTFeature *) (_intra_infopt)))->y = (int) y;
  (((TKLTFeature *) (_intra_infopt)))->val = (int) eigenvalue;
  _intra_infopt = (void *) (((TKLTFeature *) (_intra_infopt)) + 1);
  _intra_regout1++;
}

//----------------------------------------------------------------------------
void pix_corner_to_list_f (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  int pos;
  float sumxx, sumyy, sumxy;
  float eigenvalue;


  /* _intra_infopt=(void *)list;
     _intra_regin1=maxval;
     _intra_regin2=minval;
     _intra_regin3=listsize;
     listsize=_intra_regout1; */


  res->a = in[12].a;	/*position 12 is central position of 5x5 2D input matrix */
  res->y = in[12].y;
  res->u = in[12].u;
  res->v = in[12].v;
  res->ax = in[12].ax;

  sumxx = 0;
  sumyy = 0;
  sumxy = 0;

  for (pos = 0; pos < 25; pos++) {
    sumxx += in[pos].y.f;
    sumyy += in[pos].u.f;
    sumxy += in[pos].v.f;
  }

  /* compute lower eigenvalue of:
     sumxx, sumxy
     sumxy, sumyy */

#define SQR(x) ((x)*(x))
//   eigenvalue=(sumxx+sumyy -
//            (long)(sqrt(((double)(SQR(sumxx-sumyy)+(SQR(sumxy)<<2))))+0.5)+
//            1)>>1;
  eigenvalue = SQR (sumxx - sumyy);
  eigenvalue += (SQR (sumxy) *4.0);
  eigenvalue = (-sqrt(eigenvalue));
  eigenvalue += sumxx;
  eigenvalue += sumyy;
  /* last shift is only for scaling!! => can be removed */
#undef SQR

  if (eigenvalue < TCHAN_TO_FLOAT(_intra_regin2)) {
    return;
  }

  if (eigenvalue > TCHAN_TO_FLOAT(_intra_regin1)) {
    eigenvalue = TCHAN_TO_FLOAT(_intra_regin1);
#ifdef DEBUG
    fprintf (stderr, "Warning: overflow of eigenvalue\n");
#endif
  }

  /* write to list */
  (((TKLTFeature *) (_intra_infopt)))->x = (int) x;
  (((TKLTFeature *) (_intra_infopt)))->y = (int) y;
  (((TKLTFeature *) (_intra_infopt)))->val = (int) eigenvalue;
  _intra_infopt = (void *) (((TKLTFeature *) (_intra_infopt)) + 1);
  _intra_regout1++;
}

//----------------------------------------------------------------------------
void pix_set_value (TPixel * res, TPixel * in, TCoor x, TCoor y)
{
  /* _intra_regin1=value; */


  res->a.i = _intra_regin1;
  res->y.i = _intra_regin1;
  res->u.i = _intra_regin1;
  res->v.i = _intra_regin1;
  res->ax.i = _intra_regin1;
}

#ifdef XYUVDISP
//----------------------------------------------------------------------------
void float_to_int(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
   res->a.i=(int)(in[12].a.f*255.0);
   res->y.i=(int)(in[12].y.f*255.0);
   res->u.i=(int)(in[12].u.f*255.0);
   res->v.i=(int)(in[12].v.f*255.0);
   res->ax.i=(int)(in[12].ax.f*255.0);
}
#endif

//=============================================================================
#ifndef KLTSWAP
#define KLTSWAP(list, i, j)             \
{static TKLTFeature tmp;             \
     tmp=list[i];                    \
     list[i]=list[j];                \
     list[j]=tmp;                    \
}
#endif

//----------------------------------------------------------------------------
static void _kltquicksort (TKLTFeature * pointlist, int n)
{
  unsigned int i, j, ln, rn;

  while (n > 1) {
    KLTSWAP (pointlist, 0, n / 2);
    for (i = 0, j = n;;) {
      do
	--j;
      while (pointlist[j].val < pointlist[0].val);
      do
	++i;
      while (i < j && pointlist[i].val > pointlist[0].val);
      if (i >= j)
	break;
      KLTSWAP (pointlist, i, j);
    }
    KLTSWAP (pointlist, j, 0);
    ln = j;
    rn = n - ++j;
    if (ln < rn) {
      _kltquicksort (pointlist, ln);
      pointlist += j;
      n = rn;
    }
    else {
      _kltquicksort (pointlist + j, rn);
      n = ln;
    }
  }
}

using namespace XM;

//=============================================================================
KLTFeaturePoints::KLTFeaturePoints():
  m_FloatArith(0),
m_IntermediaryResults (-1),
m_intermediaryIO(0),
m_xsize(0),
m_ysize(0),
m_maxlistsize(0),
m_denoised(0),
m_sqrgraded(0),
m_featuremap(0),
m_featurelist(0)
{

  /* read parameters from parameter file*/
  m_denoise = atoi (getpara ("denoise", "0"));
  m_mindist = atoi (getpara ("min_featurepoint_distance", "19"));
/*  m_distthresh = atof (getpara ("distance_threshold", "0.25"));*/

    m_read = 0;
}

//----------------------------------------------------------------------------
KLTFeaturePoints::KLTFeaturePoints(int float_or_int):
m_IntermediaryResults (-1),
m_intermediaryIO(0),
m_xsize(0),
m_ysize(0),
m_maxlistsize(0),
m_denoised(0),
m_sqrgraded(0),
m_featuremap(0),
m_featurelist(0)
{
  m_FloatArith=float_or_int;

  /* read parameters from parameter file*/
  m_denoise = atoi (getpara ("denoise", "0"));
  m_mindist = atoi (getpara ("min_featurepoint_distance", "19"));
/*  m_distthresh = atof (getpara ("distance_threshold", "0.25"));*/

    m_read = 0;
}

//----------------------------------------------------------------------------
KLTFeaturePoints::~KLTFeaturePoints ()
{
  if (m_denoised)
    freevop (m_denoised);
  if (m_sqrgraded)
    freevop (m_sqrgraded);
  if (m_featurelist) delete [] m_featurelist;
  if (m_IntermediaryResults != -1) {
    close(m_IntermediaryResults);
  }
}

//----------------------------------------------------------------------------
int KLTFeaturePoints::SetMediaFilename(char *MediaFileName)
{
  char filename[MAXFPFILENAMELEN];

  m_read=0;

  if (m_IntermediaryResults != -1) {
    close(m_IntermediaryResults);
    m_IntermediaryResults=-1;
  }
  if (!MediaFileName) return -1;
  if (!*MediaFileName) return -1;

  m_intermediaryIO=atoi(getpara("RW_intermediary_results","0"));

  if (!m_intermediaryIO) return -1;

  /* compute directory name from media filename */
  strncpy(filename, MediaFileName, MAXFPFILENAMELEN - 1);
  strncat(filename, ".dir", MAXFPFILENAMELEN - 1 - strlen (filename));
  
  /* appent keyframe filename to path */
  strncat(filename, "/KLTFeatures.bin",
	   MAXFPFILENAMELEN - 1 - strlen (filename));

  m_IntermediaryResults = open (filename, O_RDWR | O_CREAT, 0666);
  if (m_IntermediaryResults != -1)
    m_read = 1;
  return m_IntermediaryResults;
}

//----------------------------------------------------------------------------
void KLTFeaturePoints::SetFloatArith(int float_or_int)
{
  m_FloatArith=float_or_int;
}

//----------------------------------------------------------------------------
TKLTFeature *KLTFeaturePoints::GetKLTFeaturePoints(MomVop *media,
						   int *listsize)
{
  int tmplistsize;
  MomImageData mapval;

#ifdef XYUVDISP
  if (media->u_chan->type != FLOAT_TYPE) {
    xyuv_show_debug(FPWIN,(unsigned short *)media->y_chan->data->s,
		    (unsigned short *)media->u_chan->data->s,
		    (unsigned short *)media->v_chan->data->s,
		    datasize(media->y_chan)&1,
		    media->y_chan->x,media->y_chan->y,
		    media->u_chan->x,media->u_chan->y,
		    "feature points",0,1.0,0,0,0,0,0,0);
  }
  else {
    MomVop *tmpvop=initvop(media->width,
			   media->height,
			   _400_,
			   UCHAR_TYPE,UCHAR_TYPE,
			   UCHAR_TYPE,UCHAR_TYPE);
    convtoint(tmpvop,media,A_CH|Y_CH);
    xyuv_show_debug(FPWIN,(unsigned short *)tmpvop->y_chan->data->s,
		    (unsigned short *)tmpvop->u_chan->data->s,
		    (unsigned short *)tmpvop->v_chan->data->s,
		    datasize(tmpvop->y_chan)&1,
		    tmpvop->y_chan->x,tmpvop->y_chan->y,
		    tmpvop->u_chan->x,tmpvop->u_chan->y,
		    "feature points",0,1.0,0,0,0,0,0,0);
    freevop(tmpvop);
  }
#endif

  /* if intermediaryanary results, read from file */
  if (m_IntermediaryResults != -1) {
    if (m_read) {
      long filepos;
      long nval;

      /* load number of feature points */
      filepos = lseek (m_IntermediaryResults, 0, SEEK_CUR);
      if (read (m_IntermediaryResults, &nval, 4) != 4) {
	m_read = 0;
	lseek (m_IntermediaryResults, filepos, SEEK_SET);
      }
      if (m_read) {

	tmplistsize = ntohl (nval);
	int fi;

	/* load feature points */
	for (fi = 0; fi < tmplistsize; fi++) {
	  if (read (m_IntermediaryResults, &nval, 4) != 4) {
	    m_read = 0;
	    lseek (m_IntermediaryResults, filepos, SEEK_SET);
	    break;
	  }
	  m_finalfeaturelist[fi].x = ntohl (nval);

	  if (read (m_IntermediaryResults, &nval, 4) != 4) {
	    m_read = 0;
	    lseek (m_IntermediaryResults, filepos, SEEK_SET);
	    break;
	  }
	  m_finalfeaturelist[fi].y = ntohl (nval);

	  if (read (m_IntermediaryResults, &nval, 4) != 4) {
	    m_read = 0;
	    lseek (m_IntermediaryResults, filepos, SEEK_SET);
	    break;
	  }
	  m_finalfeaturelist[fi].val = ntohl (nval);

#ifdef XYUVDISP
	  xyuv_setpixel (FPWIN, m_finalfeaturelist[fi].x,
	    m_finalfeaturelist[fi].y, 1, 128, 0, 255);
#endif
	}
      }
    }
  }

  *listsize=tmplistsize;
  if (m_read) return m_finalfeaturelist;

  if (!media->width || !media->height) return 0;

  /* allocate memories*/
  if ((m_xsize != media->width) || (m_ysize != media->height)) {

    m_xsize=media->width;
    m_ysize=media->height;

    if (m_denoised)
      freevop (m_denoised);
    m_denoised = initvop (media->width, media->height, _444_,
			  UCHAR_TYPE, UCHAR_TYPE, UCHAR_TYPE, UCHAR_TYPE);
    if (m_sqrgraded)
      freevop (m_sqrgraded);
    m_sqrgraded = initvop (media->width, media->height, _444_,
			   UCHAR_TYPE, SHORT_TYPE, SHORT_TYPE, SHORT_TYPE);
    m_featuremap = m_sqrgraded->a_chan;
    
    /* alloce lists with feature points */
    if (m_featurelist)
      delete[]m_featurelist;
    m_maxlistsize = media->width * media->height;
    m_featurelist = new TKLTFeature[m_maxlistsize];
    
  }

  /* perform extraction of feature from media
     1. denoise
     2. compute squares of gradient componets
     3. average square sums and extract feature (strength of
     corner = lower eigenvalue) and write to feature list
     4. sort list of features
     5. select strongest corner in local window
  */

  if (!m_read) {
    /* 1. and 2. */
    if (SW_DENOISE) {
#ifdef VERBOSE
      fprintf (stderr, "d");
      fflush (stderr);
#endif
      ydenoise (m_denoised, media);
#ifdef VERBOSE
      fprintf (stderr, "g");
      fflush (stderr);
#endif
      sqrgrad (m_sqrgraded, m_denoised);

    }
    else {
#ifdef VERBOSE
      fprintf (stderr, "g");
      fflush (stderr);
#endif
      sqrgrad (m_sqrgraded, media);
    }

    /* 3. */
    tmplistsize = m_maxlistsize;
#ifdef VERBOSE
    fprintf (stderr, "k");
    fflush (stderr);
#endif

    if (!m_FloatArith) {
      gencornerlist (m_sqrgraded, 0x7fffffff, 100, &tmplistsize,
		     m_featurelist);
    }
    else {
      float min=0.0004,
	max=100.0;
      gencornerlist(m_sqrgraded, FLOAT_TO_TCHAN(max),
		     FLOAT_TO_TCHAN(min), &tmplistsize,
		     m_featurelist);
    }      

    /* 4 */
#ifdef VERBOSE
    fprintf (stderr, "s");
    fflush (stderr);
#endif
    _kltquicksort (m_featurelist, tmplistsize);

    /* 5 */
    /* initalize featuremap */
    memset (m_featuremap->f, 0, m_featuremap->x * m_featuremap->y *
      datasize (m_featuremap));
    /* initalize final feature list */
    memset (m_finalfeaturelist, 0, (KLTMAXINDEX + 1) * sizeof (TKLTFeature));

    /* at the moment reset to zero could be differnt in context of
       tracking */

    if (KLTMAXINDEX) {

#ifdef VERBOSE
      fprintf (stderr, "f");
      fflush (stderr);
#endif

      int ii;			/* index of input list */
      int fi = 0;		/* index of final list */

      /* loop list of important features */
      for (ii = 0; ii < tmplistsize; ii++) {

	/* test if feature was found in neighborhood */
	mapval.u = xydata (m_featuremap, m_featurelist[ii].x,
	  m_featurelist[ii].y);
	/* assumption of fixed data type to UCHAR_TYPE */
	/* else use
	   if (getdata(mapvalu,m_featuremap->type) { */
	if (!(*mapval.u)) {
	  m_finalfeaturelist[fi++] = m_featurelist[ii];

	  /*mark neighborhood of pixels in map */
	  markwindow (m_sqrgraded, m_featurelist[ii].x, m_featurelist[ii].y,
	    m_mindist, SETMARK);

#ifdef XYUVDISP
	  xyuv_setpixel (FPWIN, m_featurelist[ii].x, m_featurelist[ii].y,
	    1, 128, 0, 255);
#endif

	  if (fi > KLTMAXINDEX)
	    break;
	}
      }
      *listsize = tmplistsize = fi;

      /* write intermediary results to file */
      if ((m_IntermediaryResults != -1) && (!m_read)) {
	int fi;			/* index of final list */
	long nval;

	nval = htonl (tmplistsize);
	write (m_IntermediaryResults, &nval, 4);

	for (fi = 0; fi < tmplistsize; fi++) {
	  nval = htonl (m_finalfeaturelist[fi].x);
	  write (m_IntermediaryResults, &nval, 4);
	  nval = htonl (m_finalfeaturelist[fi].y);
	  write (m_IntermediaryResults, &nval, 4);
	  nval = htonl (m_finalfeaturelist[fi].val);
	  write (m_IntermediaryResults, &nval, 4);
	}
      }
    }
//#ifdef XYUVDISP
//    xyuv_show_simple (1, (unsigned short *) m_featuremap->f, 0, 0, 1,
//	m_featuremap->x, m_featuremap->y, 0, 0, "mindist");
//#endif
  }
  return m_finalfeaturelist;
}
