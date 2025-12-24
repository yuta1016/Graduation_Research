//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden
// (contributing organizations names)
//
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (duration)
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
// Applicable File Name:  ParametricObjectMotionExtraction.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif
#include "Utilities/readparam.h"
#include "momusys.h"
#include "intra.h"
#include "inter.h"
#include "coortrans.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"

//#define SCALEERROR
#ifndef min
#define min(a,b) ((a<b) ? a : b)
#endif


//============================================================================
// defines for image processing

#define BORDERSKIP 15

#ifdef INTARITH
#define TRANS_INTERPOLATE TRANS_INT_INTERPOLATE
#else
#define TRANS_INTERPOLATE TRANS_FLOAT_INTERPOLATE
#endif

/* #define makeloatimage (outvop,invop) \ */
/*             intra_proc(0,invop,0,0,Y_CH|U_CH|V_CH|A_CH, \ */
/* 		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \ */
/* 		       -1,-1,0,0,0,0,size1to1,pix_to_float) */


#define scale(outvop,invop) \
            intertrans_proc(outvop,0,invop,Y_CH|A_CH,0,Y_CH|A_CH,SCAN_HO| \
			    TRANS_INTERPOLATE, \
                            -1,-1,0,0,0,0,&affine,&scalepix)

#ifdef DEBUG
#define RES_CH Y_CH|U_CH|V_CH|A_CH
#else
#define RES_CH Y_CH|U_CH|A_CH
#endif
#ifdef INTARITH
#define trans_diff_hist(outvop,in1vop,in2vop,transformation,hist,noofpixels, \
            minerr,errstep) \
            _inter_regout1=0; \
            _inter_infopt=hist; \
            intertrans_proc(outvop,in1vop,in2vop,\
                            RES_CH,Y_CH|A_CH,Y_CH|A_CH,SCAN_HO| \
			    TRANS_INTERPOLATE, \
                            -1,-1,0,0,0,0,transformation,&trans_diff_histpix);\
            *(noofpixels)=_inter_regout1; \
            *(minerr)=0; \
            *(errstep)=1; 

#else
#define trans_diff_hist(outvop,in1vop,in2vop,transformation,hist,noofpixels,\
            minerr,errstep) \
            TCHAN_TO_FLOAT(_inter_regout2)=10.0; \
            TCHAN_TO_FLOAT(_inter_regout3)=-10.0; \
            intertrans_proc(outvop,in1vop,in2vop,\
                            RES_CH,Y_CH|A_CH,Y_CH|A_CH,SCAN_HO| \
			    TRANS_INTERPOLATE, \
                            -1,-1,0,0,0,0,transformation,&trans_diff_histpix);\
            TCHAN_TO_FLOAT(_inter_regout2)=10.0; \
            TCHAN_TO_FLOAT(_inter_regout3)=-10.0; \
            intra_proc(outvop,outvop,0,0,A_CH|U_CH, \
		       CON_0,SCAN_HO,0,0,0,0,0, \
		       outvop->width-(BORDERSKIP<<1), \
		       outvop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,&floathistrangepix); \
            _inter_regout1=0; \
            _inter_infopt=hist; \
            *(minerr)=TCHAN_TO_FLOAT(_inter_regout2); \
            *(errstep)= TCHAN_TO_FLOAT(_inter_regout3)- \
	        TCHAN_TO_FLOAT(_inter_regout2); \
            TCHAN_TO_FLOAT(_inter_regout4)= *(errstep); \
            *(errstep)/=(float)POMHISTBINS; \
            intra_proc(outvop,outvop,0,0,A_CH|U_CH, \
		       CON_0,SCAN_HO,0,0,0,0,0, \
		       outvop->width-(BORDERSKIP<<1), \
		       outvop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,&floathistpix); \
             *(noofpixels)=_inter_regout1;
#endif

#define trans_diff(outvop,in1vop,in2vop,transformation) \
            intertrans_proc(outvop,in1vop,in2vop,\
                            RES_CH,Y_CH|A_CH,Y_CH|A_CH, \
                            SCAN_HO| TRANS_INTERPOLATE, \
                            -1,-1,0,0,0,0,transformation,&trans_diffpix);


#define absimg(outvop,invop) \
            intra_proc(outvop,invop,0,A_CH|Y_CH,A_CH|Y_CH, \
		       CON_0,SCAN_HO,0,0,0,0,0, \
		       -1,-1,0,0,0,0,size1to1,&abs_pix)

#ifndef INTARITH
#define convtofloat(outvop,invop,channels) \
            intra_proc(outvop,invop,0, \
                       channels, \
                       channels, \
		       CON_0,SCAN_HO,0,0,0,0,0, \
		       -1,-1,0,0,0,0,size1to1,&int_to_float)

#ifdef DEBUG
#ifndef NODISP
#define convtoint(outvop,invop,channels) \
            intra_proc(outvop,invop,0, \
                       channels, \
                       channels, \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       -1,-1,0,0,0,0,size1to1,&float_to_int)
#endif
#endif
#endif

#ifndef INTARITH
#define antialiash(outvop,invop,filter) \
            _inter_infopt=filter; \
            intra_proc(outvop,invop,0,A_CH|Y_CH,A_CH|Y_CH, \
		       CON_H8,SCAN_HO,-1,-1,-1,-1,-1, \
		       -1,-1,0,0,0,0,size1to1,&ninetapfilter)

#define antialiasv(outvop,invop,filter) \
            _inter_infopt=filter; \
            intra_proc(outvop,invop,0,A_CH|Y_CH,A_CH|Y_CH, \
		       CON_V8,SCAN_VE,-1,-1,-1,-1,-1, \
		       -1,-1,0,0,0,0,size1to1,&ninetapfilter)
#else  /*INTARITH*/

#define antialias(outvop,invop) \
            intra_proc(outvop,invop,0,A_CH|Y_CH,A_CH|Y_CH, \
		       CON_8,SCAN_HO,-1,-1,-1,-1,-1, \
		       -1,-1,0,0,0,0,size1to1,&antialiaspix)
#endif  /*INTARITH*/

#ifdef INTARITH
#define abssumoverdiff(invop,threshold,abssum,noofpixels,debug) \
            _inter_regin1=(threshold)*(threshold); \
            _inter_regout1=0; \
            _inter_regout2=0; \
            intra_proc(invop,invop,0,(debug)?V_CH:0, \
                       A_CH|U_CH|((debug)?V_CH:0), \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       invop->width-(BORDERSKIP<<1), \
		       invop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,&abssumoverdiffpix); \
            *(abssum)=_inter_regout1; \
            *(noofpixels)=_inter_regout2;
#else
#define abssumoverdiff(invop,threshold,abssum,noofpixels,debug) \
            TCHAN_TO_FLOAT(_inter_regin1)=(threshold)*(threshold); \
            TCHAN_TO_FLOAT(_inter_regout1)=0.0; \
            _inter_regout2=0; \
            intra_proc(invop,invop,0,(debug)?V_CH:0, \
                       A_CH|U_CH|((debug)?V_CH:0), \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       invop->width-(BORDERSKIP<<1), \
		       invop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,&abssumoverdiffpix); \
            *(abssum)=TCHAN_TO_FLOAT(_inter_regout1); \
            *(noofpixels)=_inter_regout2;
#endif

#define gradient(outvop,invop) \
            intra_proc(outvop,invop,0,U_CH|V_CH,Y_CH, \
		       CON_8,SCAN_HO,-1,-1,-1,-1,-1, \
		       outvop->width-(BORDERSKIP<<1), \
		       outvop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,&gradientpix)

#ifdef INTARITH
#define errorgrad(invop,deltaerrparam,threshold,errogradpix) \
            _inter_regin1=threshold; \
            _intra_infopt=deltaerrparam; \
            intra_proc(0,invop,0,0,Y_CH|U_CH|V_CH|A_CH, \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       invop->width-(BORDERSKIP<<1), \
		       invop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,errogradpix)
#else
#define errorgrad(invop,deltaerrparam,threshold,errogradpix) \
            TCHAN_TO_FLOAT(_inter_regin1)=threshold; \
            _intra_infopt=deltaerrparam; \
            intra_proc(0,invop,0,0,Y_CH|U_CH|V_CH|A_CH, \
		       CON_0,SCAN_HO,-1,-1,-1,-1,-1, \
		       invop->width-(BORDERSKIP<<1), \
		       invop->height-(BORDERSKIP<<1), \
		       BORDERSKIP,BORDERSKIP,BORDERSKIP,BORDERSKIP, \
		       size1to1,errogradpix)
#endif

//=============================================================================

using namespace XM;

const UUID ParametricObjectMotionExtractionInterface2::myID = UUID("");
const char *ParametricObjectMotionExtractionInterface2::myName = "ParametricObjectMotionExtractionInterface";

const UUID ParametricObjectMotionExtractionTool2::myID = UUID("");
const char *ParametricObjectMotionExtractionTool2::myName = "ParametricObjectMotionExtractionDescriptorExtractor";

const UUID ParametricObjectMotionExtractionInterfaceABC2::myID = UUID("");

//=============================================================================
ParametricObjectMotionExtractionInterfaceABC2::
ParametricObjectMotionExtractionInterfaceABC2()
{
	SetInterfaceID(myID);
}

//=============================================================================
ParametricObjectMotionExtractionInterface2::
ParametricObjectMotionExtractionInterface2(
                 ParametricObjectMotionExtractionTool2* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionInterface2::
~ParametricObjectMotionExtractionInterface2()
{
}

//----------------------------------------------------------------------------
void ParametricObjectMotionExtractionInterface2::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface2::
SetSourceMedia(MomVop *VOP)
{
  return m_ExtractionTool->SetSourceMedia(VOP);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface2::
SetCurrentSourceMedia(MomVop *VOP)
{
  return m_ExtractionTool->SetCurrentSourceMedia(VOP);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface2::
SetNextSourceMedia(MomVop *VOP)
{
  return m_ExtractionTool->SetNextSourceMedia(VOP);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface2::
SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  return m_ExtractionTool->SetSourceMediaFrameNumber(FrameNo);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC*
ParametricObjectMotionExtractionInterface2::GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface2::
SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *aParametricObjectMotionDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aParametricObjectMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionInterface2::
SetOrigins(float * origins) 

{
  return m_ExtractionTool->SetOrigins(origins);
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionInterface2::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionInterface2::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionExtractionInterface2::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionExtractionInterface2::GetName(void)
{ return myName; }


//=============================================================================
ParametricObjectMotionExtractionTool2::ParametricObjectMotionExtractionTool2():
m_Interface(this),
m_DescriptorInterface(0),
m_CurrTime(0),
m_NextTime(0),
m_CurrScale(0),
m_NextScale(0),
#ifdef INTARITH
m_Filtered(0),
#endif
m_Trans(0),
m_Grad(0),
m_origins(0),
m_Origintype(1),
//m_Resolutions(-1),
m_ResolutionsScale(0.7),
m_alphastartt(0),
m_alphalimitt(0),
m_alphastarta(0),
m_alphalimita(0),
m_param_init_mode(1)

{
  /* number of resolutions*/
  m_Resolutions=atoi(getpara("POMResolutions","4"));
  if (m_Resolutions<1) m_Resolutions=1;

  /* allocate memories for scaled VOPs*/
  CreateLevels(m_Resolutions);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionTool2::
ParametricObjectMotionExtractionTool2(
			     ParametricObjectMotionDescriptorInterfaceABC
			     *aDescriptorInterface):
m_Interface(this),
m_DescriptorInterface(0),
m_CurrTime(0),
m_NextTime(0),
m_Duration(1),			// Aljoscha 12/13/00
m_CurrScale(0),
m_NextScale(0),
#ifdef INTARITH
m_Filtered(0),
#endif
m_Trans(0),
m_Grad(0),
m_origins(0),
m_Origintype(1),
//m_Resolutions(-1),
m_ResolutionsScale(0.7),
m_alphastartt(0),
m_alphalimitt(0),
m_alphastarta(0),
m_alphalimita(0),
m_param_init_mode(1)

{
#ifdef VERBOSE
  fprintf(stderr,"Connect descriptor\n");
#endif

  /* create descriptor if it does not exist*/
  if (!aDescriptorInterface) {
    ParametricObjectMotionDescriptor *descriptor =
      new ParametricObjectMotionDescriptor;
    aDescriptorInterface=descriptor->GetInterface();
  }

  /* connect Descritors with coding schemes*/
  SetDescriptorInterface(aDescriptorInterface);

  /* create sub extraction tools and
     connect them with descriptor if availabe*/
  /* no sub extraction tools (see how ColorHistCS if needed)*/

  /* number of resolutions*/
  m_Resolutions=atoi(getpara("POMResolutions","4"));
  if (m_Resolutions<1) m_Resolutions=1;

  /* allocate memories for scaled VOPs*/
  CreateLevels(m_Resolutions);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionTool2::~ParametricObjectMotionExtractionTool2()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  
  if (m_NextScale) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_NextScale[i]) freevop(m_NextScale[i]);
    }
    delete [] m_NextScale;
  }
  if (m_CurrScale) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_CurrScale[i]) freevop(m_CurrScale[i]);
    }
    delete [] m_CurrScale;
  }
#ifdef INTARITH
  if (m_Filtered) {
    int i;
    for (i=0; i<m_Resolutions-1; i++) {
      if (m_Filtered[i]) freevop(m_Filtered[i]);
    }
    delete [] m_Filtered;
  }
#endif
  if (m_Trans) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_Trans[i]) freevop(m_Trans[i]);
    }
    delete [] m_Trans;
  }
  if (m_Grad) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_Grad[i])  {
	m_Grad[i]->y_chan=0;
	m_Grad[i]->a_chan=0;
	freevop(m_Grad[i]);
      }
    }
    delete [] m_Grad;
  }
  
  if (m_origins) delete [] m_origins;
  if (m_alphastartt) delete [] m_alphastartt;
  if (m_alphalimitt) delete [] m_alphalimitt;
  if (m_alphastarta) delete [] m_alphastarta;
  if (m_alphalimita) delete [] m_alphalimita;
}

//----------------------------------------------------------------------------
bool ParametricObjectMotionExtractionTool2::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ParametricObjectMotionExtractionTool2::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::SetSourceMedia(MomVop *VOP)

{
  if (!VOP) return -1;

  {
    /* shift time values*/
    m_CurrTime=m_NextTime;
    /* m_NextTime time is set in SetFrameNo*/

    /**************/
    /* Create VOPs*/ 
    /**************/

    int i,oldsize;
    
    oldsize=(1 && m_CurrScale[0]);
    if (oldsize) {
      oldsize=(oldsize && (m_CurrScale[0]->width==VOP->width) &&
		  (m_CurrScale[0]->height==VOP->height));
    }

    for (i=0; i<m_Resolutions; i++) {

      oldsize=oldsize && m_CurrScale[i];

      if (oldsize) {

	MomVop *swptmp;

	swptmp=m_CurrScale[i];
	m_CurrScale[i]=m_NextScale[i];
	m_NextScale[i]=swptmp;
      }
      else {

	/* copy current to previous*/
	if (m_CurrScale[i]) freevop(m_CurrScale[i]);
	m_CurrScale[i]= m_NextScale[i];

	/* create scaled next Vop*/
	/* y=Y, a=mask*/
	if (i) {
#ifdef INTARITH
	  m_NextScale[i]=initvop((int)(m_ResolutionsScale *
				       m_NextScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_NextScale[i-1]->height),
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
#else
	  m_NextScale[i]=initvop((int)(m_ResolutionsScale *
				       m_NextScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_NextScale[i-1]->height),
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}
	else {
#ifdef INTARITH
	  m_NextScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
#else
	  m_NextScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}

      }
#ifdef INTARITH
      if (((i+1)<m_Resolutions) &&
	  (!m_Filtered[i] || 
	   ((m_NextScale[0]->width!=m_Filtered[0]->width) ||
	    (m_NextScale[0]->height!=m_Filtered[0]->height)))) {

	if (m_Filtered[i]) freevop(m_Filtered[i]);
	m_Filtered[i]=initvop(m_NextScale[i]->width,m_NextScale[i]->height,
			      _400_,
			      UCHAR_TYPE,UCHAR_TYPE,
			      UCHAR_TYPE,UCHAR_TYPE);
      }
#endif

      /* scale vop*/
      if (i) {
#ifdef INTARITH
	antialias(m_Filtered[i-1],m_NextScale[i-1]);
#endif
	{
	  float m_origins[4]={0.0, 0.0, 0.0, 0.0};
// 	  float scaleparameters[6]={0.0, 0.0, (1.0/m_ResolutionsScale)-1.0,
// 				      0.0, 0.0, (1.0/m_ResolutionsScale)-1.0};
	  float scaleparameters[6]={0.0, 0.0,
				      (((float)m_NextScale[i-1]->width -2)/
				       ((float)m_NextScale[i]->width -1))-1.0,
				      0.0, 0.0,
				      (((float)m_NextScale[i-1]->height -2)/
				       ((float)m_NextScale[i]->height -1))-1.0
				      };
#ifndef INTARITH
	  float gaussfilter[9]= {0.00517275557, 0.0297671501, 0.103897572,
				   0.219951153, 0.28242287, 0.219951153,
				   0.103897572, 0.0297671501, 0.00517275557};
#endif

	  _tr_origins=m_origins;
	  _tr_parameters=scaleparameters;
	  
#ifdef INTARITH
	  scale(m_NextScale[i],m_Filtered[i-1]);
#else
	  scale(m_NextScale[i],m_NextScale[i-1]);
	  antialiash(m_NextScale[i],m_NextScale[i],gaussfilter);
	  antialiasv(m_NextScale[i],m_NextScale[i],gaussfilter);
#endif

	}
      }
      else {
	/* conert to YUV and take only Y and Alpha*/
	intra_proc(m_NextScale[0],VOP,0,
	           A_CH|Y_CH,A_CH|Y_CH|U_CH|V_CH,CON_0,SCAN_HO,
	           0,0,0,0,0,
	           -1,-1,0,0,0,0,
	           size1to1,&RGB_To_YUV);
#ifndef INTARITH
	convtofloat(m_NextScale[0],m_NextScale[0],A_CH|Y_CH);
#endif
      }
      

      /* create scaled intermediate results*/
      if (m_CurrScale[i]) {
	if  ((!m_Trans[i]) ||
	     ((m_CurrScale[i]->width!=m_Trans[i]->width) ||
	      (m_CurrScale[i]->height!=m_Trans[i]->height))) {

	  /* Trans: a= transformed mask, y= transformed, u= prediction error*/
	  if (m_Trans[i]) freevop(m_Trans[i]);
#ifdef INTARITH
	  m_Trans[i]=initvop(m_CurrScale[i]->width,
			     m_CurrScale[i]->height,
			     _444_,
			     UCHAR_TYPE,UCHAR_TYPE,
			     SHORT_TYPE,UCHAR_TYPE);
#else
	  m_Trans[i]=initvop(m_CurrScale[i]->width,
			     m_CurrScale[i]->height,
			     _444_,
			     FLOAT_TYPE,FLOAT_TYPE,
			     FLOAT_TYPE,FLOAT_TYPE);
#endif
#ifndef DEBUG
	  freeimg(m_Trans[i]->v_chan);
	  m_Trans[i]->v_chan=0;
#endif
	}
	
	if  ((!m_Grad[i]) ||
	     ((m_CurrScale[i]->width!=m_Grad[i]->width) ||
	      (m_CurrScale[i]->height!=m_Grad[i]->height))) {

	  /* Grad: a=transformed mask, y=diff, u=gradx, v=grady*/
	  if (m_Grad[i]) {
	    m_Grad[i]->y_chan=0;
	    m_Grad[i]->a_chan=0;
	    freevop(m_Grad[i]);
	  }
#ifdef INTARITH
	  m_Grad[i]=initvop(m_CurrScale[i]->width,
			    m_CurrScale[i]->height,
			    _444_,
			    UCHAR_TYPE,UCHAR_TYPE,
			    SHORT_TYPE,SHORT_TYPE);
#else
	  m_Grad[i]=initvop(m_CurrScale[i]->width,
			    m_CurrScale[i]->height,
			    _444_,
			    FLOAT_TYPE,FLOAT_TYPE,
			    FLOAT_TYPE,FLOAT_TYPE);
#endif
	  freeimg(m_Grad[i]->y_chan);
	  m_Grad[i]->y_chan=m_Trans[i]->u_chan; /* set diff image to y_chan*/
	  freeimg(m_Grad[i]->a_chan);
	  m_Grad[i]->a_chan=m_Trans[i]->a_chan; /* set mask to a_chan*/
	}
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::SetNextSourceMedia(MomVop *VOP)

{
  int i, newsize=0;

  if (!VOP) return -1;

  {

    /* shift time values*/
    m_CurrTime=m_NextTime;
    /* m_NextTime time is set in SetFrameNo*/
  
    /**************/
    /* Create VOPs*/ 
    /**************/

    if (m_NextScale[0]) {
      if ((m_NextScale[0]->width!=VOP->width) || 
	  (m_NextScale[0]->height!=VOP->height)) {
	newsize=1;
      }
    }
    else {
      newsize=1;
    }

    for (i=0; i<m_Resolutions; i++) {
      if (newsize) {
	if (m_NextScale[i]) freevop(m_NextScale[i]);
	if (i) {
#ifdef INTARITH
	  m_NextScale[i]=initvop((int)(m_ResolutionsScale *
				       m_NextScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_NextScale[i-1]->height),
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
#else
	  m_NextScale[i]=initvop((int)(m_ResolutionsScale *
				       m_NextScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_NextScale[i-1]->height),
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}
	else {
#ifdef INTARITH
	  m_NextScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
	  if ((i+1)<m_Resolutions) {
	    if (m_Filtered[i]) freevop(m_Filtered[i]);
	    m_Filtered[i]=initvop(m_NextScale[i]->width,m_NextScale[i]->height,
				  _400_,
				  UCHAR_TYPE,UCHAR_TYPE,
				  UCHAR_TYPE,UCHAR_TYPE);
	  }
#else
	  m_NextScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}
#ifdef INTARITH	
	if ((i+1)<m_Resolutions) {
	  if (m_Filtered[i]) freevop(m_Filtered[i]);
	  m_Filtered[i]=initvop(m_NextScale[i]->width,m_NextScale[i]->height,
				_400_,
				UCHAR_TYPE,UCHAR_TYPE,
				UCHAR_TYPE,UCHAR_TYPE);
	}
#endif
      }


      /* scale vop*/
      if (i) {
#ifdef INTARITH	
	antialias(m_Filtered[i-1],m_NextScale[i-1]);
#endif
	{
	  float m_origins[4]={0.0, 0.0, 0.0, 0.0};
// 	  float scaleparameters[6]={0.0, 0.0, (1.0/m_ResolutionsScale)-1.0,
// 				      0.0, 0.0, (1.0/m_ResolutionsScale)-1.0};
	  float scaleparameters[6]={0.0, 0.0,
				    (((float)m_NextScale[i-1]->width -2)/
				     ((float)m_NextScale[i]->width -1))-1.0,
				    0.0, 0.0,
				    (((float)m_NextScale[i-1]->height -2)/
				     ((float)m_NextScale[i]->height -1))-1.0};
#ifndef INTARITH
	  float gaussfilter[9]= {0.00517275557, 0.0297671501, 0.103897572,
				   0.219951153, 0.28242287, 0.219951153,
				   0.103897572, 0.0297671501, 0.00517275557};
#endif

	  _tr_origins=m_origins;
	  _tr_parameters=scaleparameters;
	  
#ifdef INTARITH
	  scale(m_NextScale[i],m_Filtered[i-1]);
#else
	  scale(m_NextScale[i],m_NextScale[i-1]);
	  antialiash(m_NextScale[i],m_NextScale[i],gaussfilter);
	  antialiasv(m_NextScale[i],m_NextScale[i],gaussfilter);
#endif

	}
      }
      else {
	/* convert to YUV and take only Y and Alpha*/
	intra_proc(m_NextScale[0],VOP,0,
	           A_CH|Y_CH,A_CH|Y_CH|U_CH|V_CH,CON_0,SCAN_HO,
	           0,0,0,0,0,
	           -1,-1,0,0,0,0,
	           size1to1,&RGB_To_YUV);
#ifndef INTARITH
	convtofloat(m_NextScale[0],m_NextScale[0],A_CH|Y_CH);
#endif
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::SetCurrentSourceMedia(MomVop *VOP)

{
  int i, newsize=0;

  if (!VOP) return -1;

  {
    /* shift time values*/
    m_NextTime=m_CurrTime;
    /* m_NextTime time is set in SetFrameNo*/

    /**************/
    /* Create VOPs*/ 
    /**************/

    if (m_CurrScale[0]) {
      if ((m_CurrScale[0]->width!=VOP->width) || 
	  (m_CurrScale[0]->height!=VOP->height)) {
	newsize=1;
      }
    }
    else {
      newsize=1;
    }

    for (i=0; i<m_Resolutions; i++) {
      if (newsize) {
	if (m_CurrScale[i]) freevop(m_CurrScale[i]);
	if (i) {
#ifdef INTARITH
	  m_CurrScale[i]=initvop((int)(m_ResolutionsScale *
				       m_CurrScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_CurrScale[i-1]->height),
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
#else
	  m_CurrScale[i]=initvop((int)(m_ResolutionsScale *
				       m_CurrScale[i-1]->width),
				 (int)(m_ResolutionsScale *
				       m_CurrScale[i-1]->height),
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}
	else {
#ifdef INTARITH
	  m_CurrScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
#else
	  m_CurrScale[i]=initvop(VOP->width,VOP->height,
				 _400_,
				 FLOAT_TYPE,FLOAT_TYPE,
				 FLOAT_TYPE,FLOAT_TYPE);
#endif
	}

	/* Trans: a= transformed mask, y= transformed, u= prediction error*/
	if (m_Trans[i]) freevop(m_Trans[i]);
#ifdef INTARITH
	m_Trans[i]=initvop(m_CurrScale[i]->width,
			   m_CurrScale[i]->height,
			   _444_,
			   UCHAR_TYPE,UCHAR_TYPE,
			   SHORT_TYPE,UCHAR_TYPE);
#else
	m_Trans[i]=initvop(m_CurrScale[i]->width,
			   m_CurrScale[i]->height,
			   _444_,
			   FLOAT_TYPE,FLOAT_TYPE,
			   FLOAT_TYPE,FLOAT_TYPE);
#endif
#ifndef DEBUG
	freeimg(m_Trans[i]->v_chan);
	m_Trans[i]->v_chan=0;
#endif

	/* Grad: a=transformed mask, y=diff, u=gradx, v=grady*/
	if (m_Grad[i]) {
	  m_Grad[i]->y_chan=0;
	  m_Grad[i]->a_chan=0;
	  freevop(m_Grad[i]);
	}
#ifdef INTARITH
	m_Grad[i]=initvop(m_CurrScale[i]->width,
			  m_CurrScale[i]->height,
			  _444_,
			  UCHAR_TYPE,UCHAR_TYPE,
			  SHORT_TYPE,SHORT_TYPE);
#else
	m_Grad[i]=initvop(m_CurrScale[i]->width,
			  m_CurrScale[i]->height,
			  _444_,
			  FLOAT_TYPE,FLOAT_TYPE,
			  FLOAT_TYPE,FLOAT_TYPE);
#endif
	freeimg(m_Grad[i]->y_chan);
	m_Grad[i]->y_chan=m_Trans[i]->u_chan; /* set diff image to y_chan*/
	freeimg(m_Grad[i]->a_chan);
	m_Grad[i]->a_chan=m_Trans[i]->a_chan; /* set mask to a_chan*/


#ifdef INTARITH
	if ((i+1)<m_Resolutions) {
	  if (m_Filtered[i]) freevop(m_Filtered[i]);
	  m_Filtered[i]=initvop(m_CurrScale[i]->width,m_CurrScale[i]->height,
				_400_,
				UCHAR_TYPE,UCHAR_TYPE,
				UCHAR_TYPE,UCHAR_TYPE);
	}
#endif
      }

      /* scale vop*/
      if (i) {
#ifdef INTARITH	
	antialias(m_Filtered[i-1],m_NextScale[i-1]);
#endif
	{
	  float m_origins[4]={0.0, 0.0, 0.0, 0.0};
// 	  float scaleparameters[6]={0.0, 0.0, (1.0/m_ResolutionsScale)-1.0,
// 				      0.0, 0.0, (1.0/m_ResolutionsScale)-1.0};
	  float scaleparameters[6]={0.0, 0.0,
				      (((float)m_CurrScale[i-1]->width -2)/
				       ((float)m_CurrScale[i]->width -1))-1.0,
				      0.0, 0.0,
				      (((float)m_CurrScale[i-1]->height -2)/
				       ((float)m_CurrScale[i]->height -1))-1.0
				      };
#ifndef INTARITH
	  float gaussfilter[9]= {0.00517275557, 0.0297671501, 0.103897572,
				   0.219951153, 0.28242287, 0.219951153,
				   0.103897572, 0.0297671501, 0.00517275557};
#endif

	  _tr_origins=m_origins;
	  _tr_parameters=scaleparameters;
	  
#ifdef INTARITH
	  scale(m_CurrScale[i],m_Filtered[i-1]);
#else
	  scale(m_CurrScale[i],m_CurrScale[i-1]);
	  antialiash(m_CurrScale[i],m_CurrScale[i],gaussfilter);
	  antialiasv(m_CurrScale[i],m_CurrScale[i],gaussfilter);
#endif

	}
      }
      else {
	/* conert to YUV and take only Y and Alpha*/
	intra_proc(m_CurrScale[0],VOP,0,
	           A_CH|Y_CH,A_CH|Y_CH|U_CH|V_CH,CON_0,SCAN_HO,
	           0,0,0,0,0,
	           -1,-1,0,0,0,0,
	           size1to1,&RGB_To_YUV);
#ifndef INTARITH
	convtofloat(m_CurrScale[0],m_CurrScale[0],A_CH|Y_CH);
#endif
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::
SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  m_NextTime=FrameNo;
  return 0;
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC*
ParametricObjectMotionExtractionTool2::GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::
SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *aDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

    /* conect sub descriptors with sub coding schemes*/
    /* not available*/
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub extraction tools schemes*/
    /* not available*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionTool2::
SetOrigins(float * origins) 

{
  int i;     // #ifdef INTARITH oder FRAME_TO_FRAME
  int j;
  /* full resolution */
  m_origins[0] = origins[0];
  m_origins[1] = origins[1];
  m_origins[2] = origins[2];
  m_origins[3] = origins[3]; 	

  for (i=1, j=0 ; i<m_Resolutions; i++) {

    /* reduced resolutions*/
    m_origins[j+4] = m_origins[j]*m_ResolutionsScale;j++;
    m_origins[j+4] = m_origins[j]*m_ResolutionsScale;j++;
    m_origins[j+4] = m_origins[j]*m_ResolutionsScale;j++;
    m_origins[j+4] = m_origins[j]*m_ResolutionsScale;j++;
  }
   
//  /* set origin for transformations*/
//  if (m_Origintype) { /* if set = center*/ 
//    m_origins[0]=((float)(m_NextScale[0]->width-1))/2.0;
//    m_origins[1]=((float)(m_NextScale[0]->height-1))/2.0;
//    m_origins[2]=((float)(m_CurrScale[0]->width-1))/2.0;
//    m_origins[3]=((float)(m_CurrScale[0]->height-1))/2.0;
//  }
//  else {
//    m_origins[0]=0.0;
//    m_origins[1]=0.0;
//    m_origins[2]=0.0;
//    m_origins[3]=0.0;
//  }  
  return 0L;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionTool2::InitExtracting(void)
{
  char parametername[256];
  int motion_model;

  /* init motion parameters*/
  {
    int i;
    for(i=0; i<POM_MAXPARAM; m_Param[i++]=0);
  }
  m_tx=m_ty=0.0;

  m_NextTime=0;

  /* read paramters*/
  /* read model code*/
  motion_model=atoi(getpara("MotionModel","2"));
  /* default = 2 = affine*/
  m_DescriptorInterface->SetModelCode(motion_model);

  /* origin type*/
  m_Origintype=atoi(getpara("POMOrigin","1"));
  /* default = origin in center of image*/
  /* set origin for transformations*/
  if (m_Origintype) { /* if set = center*/
    if (m_NextScale[0]){
        m_origins[0]=((float)(m_NextScale[0]->width-1))/2.0;
        m_origins[1]=((float)(m_NextScale[0]->height-1))/2.0;
    }
    else {
      m_origins[0]=m_origins[1]=0.0;
    }
    if (m_CurrScale[0]){
        m_origins[2]=((float)(m_CurrScale[0]->width-1))/2.0;
        m_origins[3]=((float)(m_CurrScale[0]->height-1))/2.0;
    }
    else {
      m_origins[2]=m_origins[3]=0.0;
    }
  }
  else {
    m_origins[0]=m_origins[1]=m_origins[2]=m_origins[3]=0.0;
  }

  /* maximum number of iterations*/
  m_maxiterations=atoi(getpara("POMMaxIterations","64"));

  m_ResolutionsScale=atof(getpara("POMResolutionScale","0.7"));
  if (m_ResolutionsScale>0.9) m_ResolutionsScale=0.9;

  m_param_init_mode=atoi(getpara("POMInitParameters","1"));

  {
    int i,j;
    char startt[4]="1.0";
    char limitt[5]="0.02";
    char starta[5]="0.02";
    char limita[7]="0.0002";
    char *defaultstartt;
    char *defaultlimitt;
    char *defaultstarta;
    char *defaultlimita;

    defaultstartt=startt;
    defaultlimitt=limitt;
    defaultstarta=starta;
    defaultlimita=limita;

    for (i=0, j=0; i<m_Resolutions; i++) {
      sprintf(parametername,"AlphaStartT%d",i);
      defaultstartt=getpara(parametername,defaultstartt);
      m_alphastartt[i]=atof(defaultstartt);

      sprintf(parametername,"AlphaLimitT%d",i);
      defaultlimitt=getpara(parametername,defaultlimitt);
      m_alphalimitt[i]=atof(defaultlimitt);

      sprintf(parametername,"AlphaStartA%d",i);
      defaultstarta=getpara(parametername,defaultstarta);
      m_alphastarta[i]=atof(defaultstarta);

      sprintf(parametername,"AlphaLimitA%d",i);
      defaultlimita=getpara(parametername,defaultlimita);
      m_alphalimita[i]=atof(defaultlimita);

      if (i) {
      	m_origins[j+4]=m_origins[j]*m_ResolutionsScale;j++;
      	m_origins[j+4]=m_origins[j]*m_ResolutionsScale;j++;
      	m_origins[j+4]=m_origins[j]*m_ResolutionsScale;j++;
      	m_origins[j+4]=m_origins[j]*m_ResolutionsScale;j++;
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::CreateLevels(int newlevels)

{
  if (m_NextScale) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_NextScale[i]) freevop(m_NextScale[i]);
    }
    delete [] m_NextScale;
  }

  if (m_CurrScale) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_CurrScale[i]) freevop(m_CurrScale[i]);
    }
    delete [] m_CurrScale;
  }
#ifdef INTARITH
  if (m_Filtered) {
    int i;
    for (i=0; i<m_Resolutions-1; i++) {
      if (m_Filtered[i]) freevop(m_Filtered[i]);
    }
    delete [] m_Filtered;
  }
#endif
  if (m_Trans) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_Trans[i]) freevop(m_Trans[i]);
    }
    delete [] m_Trans;
  }
  if (m_Grad) {
    int i;
    for (i=0; i<m_Resolutions; i++) {
      if (m_Grad[i])  {
	m_Grad[i]->y_chan=0;
	m_Grad[i]->a_chan=0;
	freevop(m_Grad[i]);
      }
      }
    delete [] m_Grad;
  }

  m_NextScale = new MomVop* [newlevels];
  m_CurrScale  = new MomVop* [newlevels];
#ifdef INTARITH
  m_Filtered = new MomVop* [newlevels-1];
#endif
  m_Trans  = new MomVop* [newlevels];
  m_Grad  = new MomVop* [newlevels];
  {
    int i;
    for (i=0; i<newlevels; i++) {
      m_NextScale[i]=0;
      m_CurrScale[i]=0;
#ifdef INTARITH
      if ((i+1)<newlevels) m_Filtered[i]=0;
#endif
      m_Trans[i]=0;
      m_Grad[i]=0;
    }
  }

  /* create origins*/
  if (m_origins) delete [] m_origins;
  m_origins=new float [newlevels*4];

  if (m_alphastartt) delete [] m_alphastartt;
  m_alphastartt= new float [newlevels];
  if (m_alphalimitt) delete [] m_alphalimitt;
  m_alphalimitt= new float [newlevels];
  if (m_alphastarta) delete [] m_alphastarta;
  m_alphastarta= new float [newlevels];
  if (m_alphalimita) delete [] m_alphalimita;
  m_alphalimita= new float [newlevels];

  return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionTool2::StartExtracting(void)

{
  int no_of_params;
  int scalecnt;

  /*******************************/
  /* 1. for every scale
     1.1. compute translational parameters
     1.2. compute model parameters
  */


  /* check for descriptor*/
  if (!m_DescriptorInterface) return (unsigned long)-1;
 
  if (strcmp(m_DescriptorInterface->GetName(),
	     "Parametric Object Motion Description Interface") != 0) {
    return (unsigned long)-1;
  }
 
  /* check for media*/
  if ((!m_CurrScale)||(!m_NextScale)) {
    return (unsigned long)-1;
  }

  /* read model code*/
  /* default = 2 = affine*/
  switch (m_DescriptorInterface->GetModelCode()) {
  case 0:
    no_of_params=2;
    break;
  case 1:
    no_of_params=4;
    break;
  case 2:
    no_of_params=6;
    break;
  case 3:
    no_of_params=8;
    break;
  case 4:
    no_of_params=12;
    break;
  default:
    fprintf(stderr,"Warning: wrong model code (%d), should be <= 4\n",
	    m_DescriptorInterface->GetModelCode());
    return (unsigned long)-1;
  }

  /* reset motion model*/
  if (m_param_init_mode>0) {
    int i;
    for (i=0; i<no_of_params; m_Param[i++]=0);
  }

  /* scale translational parameters*/
  for(scalecnt=0; scalecnt<m_Resolutions-1; scalecnt++) {
    m_Param[0]*=m_ResolutionsScale;
    m_Param[1]*=m_ResolutionsScale;
  }

  /* add translation from last to this*/
  if (m_param_init_mode<0) {
    float dtx,dty;

    dtx=m_Param[0]-m_tx;
    dty=m_Param[1]-m_ty;
    m_tx=m_Param[0];
    m_ty=m_Param[1];
    m_Param[0]+=dtx;
    m_Param[1]+=dty;
  }

  /* loop for every scale*/
  for(scalecnt=m_Resolutions-1; scalecnt>=0; scalecnt--) {

#ifdef XYUVDISP
#ifdef INTARITH
    xyuv_show_simple(0,
		     (unsigned short *)m_CurrScale[scalecnt]->y_chan->data->s,
		     0,0,
		     datasize(m_CurrScale[scalecnt]->y_chan)&1,
		     m_CurrScale[scalecnt]->y_chan->x,
		     m_CurrScale[scalecnt]->y_chan->y,
		     0,0,
		     "current");
    xyuv_show_simple(1,
		     (unsigned short *)m_NextScale[scalecnt]->y_chan->data->s,
		     0,0,
		     datasize(m_NextScale[scalecnt]->y_chan)&1,
		     m_NextScale[scalecnt]->y_chan->x,
		     m_NextScale[scalecnt]->y_chan->y,
		     m_NextScale[scalecnt]->u_chan->x,
		     m_NextScale[scalecnt]->u_chan->y,
		     "next");
#else
    MomVop *tmpvop=initvop(m_CurrScale[scalecnt]->width,
			   m_CurrScale[scalecnt]->height,
			   _400_,
			   UCHAR_TYPE,UCHAR_TYPE,
			   UCHAR_TYPE,UCHAR_TYPE);
    convtoint(tmpvop,m_CurrScale[scalecnt],A_CH|Y_CH);
    xyuv_show_simple(0,(unsigned short *)tmpvop->y_chan->data->s,
		     0,0,
		     datasize(tmpvop->y_chan)&1,
		     tmpvop->y_chan->x,
		     tmpvop->y_chan->y,
		     0,0,
		     "current");
    freevop(tmpvop);
    tmpvop=initvop(m_NextScale[scalecnt]->width,
		   m_NextScale[scalecnt]->height,
		   _400_,
		   UCHAR_TYPE,UCHAR_TYPE,
		 UCHAR_TYPE,UCHAR_TYPE);
    convtoint(tmpvop,m_NextScale[scalecnt],A_CH|Y_CH);
    xyuv_show_simple(1,(unsigned short *)tmpvop->y_chan->data->s,
		     0,0,
		     datasize(tmpvop->y_chan)&1,
		     tmpvop->y_chan->x,
		     tmpvop->y_chan->y,
		     0,0,
		     "next");
    freevop(tmpvop);
#endif
#endif
    
    fprintf(stderr,"Scale:%d, start\n, %f, %f, %f, %f, %f, %f\n",
	    scalecnt,m_Param[0],m_Param[1],m_Param[2],
	    m_Param[3],m_Param[4],m_Param[5]);

    DiffEstimateParameters(no_of_params,2,
			   0,m_Param,m_Origintype,
			   m_alphastartt[scalecnt],m_alphalimitt[scalecnt],
			   m_CurrScale[scalecnt],m_NextScale[scalecnt],
			   m_Trans[scalecnt],m_Grad[scalecnt],
			   & m_origins[scalecnt*4]);
    fprintf(stderr,"Scale:%d, run 0\n, %f, %f, %f, %f, %f, %f\n",
	    scalecnt,m_Param[0],m_Param[1],m_Param[2],
	    m_Param[3],m_Param[4],m_Param[5]);
    //    if (no_of_params != 2) {
    if (m_DescriptorInterface->GetModelCode()) {
      DiffEstimateParameters(no_of_params,no_of_params,
			     1,m_Param,m_Origintype,
			     m_alphastarta[scalecnt],m_alphalimita[scalecnt],
			     m_CurrScale[scalecnt],m_NextScale[scalecnt],
			     m_Trans[scalecnt],m_Grad[scalecnt],
			     & m_origins[scalecnt*4]);
      fprintf(stderr,"Scale:%d, run 1\n, %f, %f, %f, %f, %f, %f\n",
	      scalecnt,m_Param[0],m_Param[1],m_Param[2],
	      m_Param[3],m_Param[4],m_Param[5]);
    }
    /* rescale parameters for next resolution*/
    if (scalecnt) {
      m_Param[0]/=m_ResolutionsScale;
      m_Param[1]/=m_ResolutionsScale;
    }
  }

  /* write results to descriptor*/
  /*origin*/
  if (m_Origintype) { /* origin is center*/
    m_DescriptorInterface->SetXOrigin(((float)m_NextScale[0]->width-1)/2.0);
    m_DescriptorInterface->SetYOrigin(((float)m_NextScale[0]->height-1)/2.0);
  }
  else {
    m_DescriptorInterface->SetXOrigin(0.0);
    m_DescriptorInterface->SetYOrigin(0.0);
  }

  /* time and duration*/
//  m_DescriptorInterface->SetStartTime(m_CurrTime);// Aljoscha 12/13/00
//  m_DescriptorInterface->SetEndTime(m_NextTime);
  m_DescriptorInterface->SetDuration(m_NextTime-m_CurrTime);
  // Aljoscha 12/13/00

  /* motion paramters*/
  {
    int i;
    for (i=0; i<no_of_params; i++) {
      m_DescriptorInterface->SetMotionParameterValue(i,(double)m_Param[i]);
    }
  }

  return 0;

}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::
DiffEstimateParameters(int no_of_params,int modifyparameters,
		   int usehessian,float *parameters,
		   int origintype,
		   float alphastart,float alphalimit,
		   MomVop *current, MomVop *next,
		   MomVop *transdiff, MomVop *grad,
		   float *origins)

{
  //float m_origins[4];

  float deltaerrparam[POM_MAXPARAM];
  float deltaparam[POM_MAXPARAM];
  float testparam[POM_MAXPARAM];

  float error_hessian[POM_MAXPARAM][POM_MAXPARAM];
  float inverse_hessian[POM_MAXPARAM][POM_MAXPARAM];
  int k;
  unsigned long noofpixels;  /* in erreor histogram*/

  int errorlimit;

#ifdef INTARITH
  int error;
  int testerror;
  TCHAN minerr;
  TCHAN errstep;
#else
  float error;
  float testerror;
  float minerr;
  float errstep;
#endif
  
  void (*transformation)(TCoor width,TCoor height,
			 TCoor inx,TCoor iny,
			 float *outx,float *outy);
  void (*errogradpix)(TPixel *,TPixel *, TCoor, TCoor);

  /* select transformation type*/
  switch (no_of_params) {
  case 2:
    transformation=&tranlational;
    errogradpix=&graderrtrans;
    break;
//   case 4:
//     transformation=&rotational;
//     errogradpix=&graderrrot;
//     break;
  case 6:
    transformation=&affine;
    errogradpix=&graderraffine;
    break;
//   case 8:
//     transformation=&perspective;
//     errogradpix=&graderrpersp;
//     break;
//   case 12:
//     transformation=&parabolic;
//     errogradpix=&graderrparabo;
//     break;
  default:
    fprintf(stderr,"Warning: unsupported motion model, skip extraction\n");
    return -1;
  }

  /* set origin for transformations*/
  switch (m_Origintype) {
  case 0: // at (0,0)
    m_origins[0]=0.0;
    m_origins[1]=0.0;
    m_origins[2]=0.0;
    m_origins[3]=0.0;
    _tr_origins=m_origins;
    break;
  case 1: // center
    m_origins[0]=((float)(m_NextScale[0]->width-1))/2.0;
    m_origins[1]=((float)(m_NextScale[0]->height-1))/2.0;
    m_origins[2]=((float)(m_CurrScale[0]->width-1))/2.0;
    m_origins[3]=((float)(m_CurrScale[0]->height-1))/2.0;
    _tr_origins=m_origins;
    break;
  default:     //extrnally controlled ,e.g., by Mosaic
    _tr_origins=origins;
//    m_origins[0]=origin[0];
//    m_origins[1]=origin[1];
//    m_origins[2]=origin[2];
//    m_origins[3]=origin[3];
     break;
   }


  
  /* 1. transform next image and compute difference and make absdiff histogram
     2. compute error limit 
     3. compute transformation error
     4. loop result parameters until alpha limit
     4.1. compute gradients from transformed
     4.2. compute error gradients
     4.3. compute normalized deltas for parameters and new test parameters
     4.4. loop alpha until test parameters are better
     4.4.1. compute new test parameters
     4.4.2. transform with test parameters and compute difference
     4.4.3. compute test transformation error
     4.4.4. if test transformation is not better reduce alpha
            else make test parameters transform paramters and go to 4.1*/


  /* 1. transform next image and compute difference and
     make absdiff histogram*/
  {
    int i;
    for (i=0; i<POMHISTBINS; m_ErrorHist[i++]=0);
  }
  /* set transformation parameters*/
  _tr_parameters=parameters;
 
  trans_diff_hist(transdiff,current,next,
		  transformation,m_ErrorHist,&noofpixels,&minerr,&errstep);
  /* transdiff: y=transformed y, u= mosaic - transformed,
     v = (mosaic + transformed)/2*/
  //sth limit region

  /* 2 compute error limit*/
  {
    int i;
    unsigned long  limitofpixels;

    limitofpixels=(int)(((float)noofpixels)*0.9+0.5);
    for (i=0, noofpixels=0; (limitofpixels>noofpixels) && (i<POMHISTBINS);) {
      noofpixels+=m_ErrorHist[i++];
    }
    errorlimit=i;
  }

  /* 3. compute transformation error*/
#ifdef DEBUG
  abssumoverdiff(transdiff,minerr+errstep*errorlimit,
		 &error,&noofpixels,1);
#else
  abssumoverdiff(transdiff,minerr+errstep*errorlimit,
		 &error,&noofpixels,0);
#endif
  //sth limit region
 
#ifdef XYUVDISP
#ifdef INTARITH
  xyuv_show_simple(2,(unsigned short *)transdiff->y_chan->data->s,
		   0,0,
		   datasize(transdiff->y_chan)&1,
		   transdiff->y_chan->x,transdiff->y_chan->y,
		   transdiff->u_chan->x,transdiff->u_chan->y,
		   "transformed");
#else
  {
    MomVop *tmpvop=initvop(transdiff->width,
			   transdiff->height,
			   _444_,
			   UCHAR_TYPE,UCHAR_TYPE,
			   SHORT_TYPE,UCHAR_TYPE);
#ifdef DEBUG
    convtoint(tmpvop,transdiff,A_CH|Y_CH|U_CH|V_CH);
#else
    convtoint(tmpvop,transdiff,A_CH|Y_CH|U_CH);
#endif
    xyuv_show_simple(2,(unsigned short *)tmpvop->y_chan->data->s,
		     0,0,
		     datasize(tmpvop->y_chan)&1,
		     tmpvop->y_chan->x,tmpvop->y_chan->y,
		     0,0,
		     "transformed");
    //    absimg(tmpvop,tmpvop);
    xyuv_show_debug(5,(unsigned short *)tmpvop->v_chan->data->s,
 		     0,0,
 		     datasize(tmpvop->v_chan)&1,
 		     tmpvop->v_chan->x,tmpvop->v_chan->y,
 		     0,0,
 		     "error",0,0,0,0,0,0,0,0);
    freevop(tmpvop);
  }
#endif
#endif

#ifdef SCALEERROR
  /* scale error*/
  if (!noofpixels) {
    error=FLT_MAX;
  }
  else {
    error/=(float)noofpixels;
  }
#endif

  /* 4. loop result parameters until alpha limit*/
  k=m_maxiterations;
  while (alphastart>=alphalimit) {

    /* 4.1. compute gradients from transformed*/
    gradient(grad,transdiff);
    /* grad: y==tarnsformed u = mosaic y - transformed y (is not written here),
       u=gradx, v=grady*/
    //sth limit region

    /* 4.2. compute error gradients*/
    {
      int i;
      for (i=0; i<no_of_params; deltaerrparam[i++]=0);
    }
    errorgrad(grad,&deltaerrparam,errorlimit,errogradpix);
    /* deltaerrparam: 0 = error * gradx; 1 = error * grady;
       2 = error * gradx * x; 3 = error * gradx * y
       4 = error * grady * x; 5 = error * grady * y*/
    //sth limit region

    /* 4.3. compute normalized deltas for parameters*/
    if (usehessian) {
      int i,j;

      for (i=0; i<no_of_params; i++) {
	for (j=0; j<no_of_params; j++) {
	  error_hessian[i][j]=deltaerrparam[i]*deltaerrparam[j];
	}
      }

      invert_hessian(inverse_hessian,error_hessian,modifyparameters);

      for (i = 0; i < modifyparameters; i++) {
	deltaparam[i] = 0.0;
 	for (j = 0; j < modifyparameters; j++) {
 	  deltaparam[i] += - inverse_hessian[i][j] * deltaerrparam[j];
 	}
      }
    }
    else { /* no hessian matrix*/
      int i;

      for (i = 0; i < modifyparameters; i++) {
	deltaparam[i] = -deltaerrparam[i];
      }
    }

    /* normailzation*/
    {
      int i;
      float sum=0.0;

      for (i = 0; i < modifyparameters; i++) {
	sum+=deltaparam[i]*deltaparam[i];
      }
      if (sum==0) {
	sum=FLT_MAX;
      }
      else {
	sum = 1.0/sqrt(sum);
      }
      for (i = 0; i < modifyparameters; i++) {
	deltaparam[i]= deltaparam[i]*sum;
      }
    }

    
    /* 4.4. loop alpha until test parameters are better*/
    while (1) {
    
      /* break if max iterations are reached*/
      if (!(--k)) break;

      /* 4.4.1. compute new test parameters*/
      {
	int i;

	for (i = 0; i < modifyparameters; i++) {
	  testparam[i]=parameters[i] + alphastart*deltaparam[i];
	}
	for (;  i < no_of_params; i++) {
	  testparam[i]=parameters[i];
	}
      }

      /* 4.4.2. transform with test parameters and compute difference*/
      _tr_parameters=testparam;
      trans_diff(transdiff,current,next,transformation);
      //sth limit region

      /* 4.4.3. compute test transformation error*/
#ifdef DEBUG
      abssumoverdiff(transdiff,minerr+errstep*errorlimit,
		     &testerror,&noofpixels,1);
#else
      abssumoverdiff(transdiff,minerr+errstep*errorlimit,
		     &testerror,&noofpixels,0);
#endif
      //sth limit region

#ifdef XYUVDISP
#ifdef INTARITH
      xyuv_show_simple(2,(unsigned short *)transdiff->y_chan->data->s,
		       0,0,
		       datasize(transdiff->y_chan)&1,
		       transdiff->y_chan->x,transdiff->y_chan->y,
		       transdiff->u_chan->x,transdiff->u_chan->y,
		       "transformed");
#else
      {
	MomVop *tmpvop=initvop(transdiff->width,
			       transdiff->height,
			       _444_,
			       UCHAR_TYPE,UCHAR_TYPE,
			       SHORT_TYPE,UCHAR_TYPE);
#ifdef DEBUG
	convtoint(tmpvop,transdiff,A_CH|Y_CH|U_CH|V_CH);
#else
	convtoint(tmpvop,transdiff,A_CH|Y_CH|U_CH);
#endif
	xyuv_show_simple(2,(unsigned short *)tmpvop->y_chan->data->s,
			 0,0,
			 datasize(tmpvop->y_chan)&1,
			 tmpvop->y_chan->x,tmpvop->y_chan->y,
			 0,0,
			 "transformed");
	//absimg(tmpvop,tmpvop);
	xyuv_show_debug(5,(unsigned short *)tmpvop->v_chan->data->s,
			0,0,
			datasize(tmpvop->v_chan)&1,
			tmpvop->v_chan->x,tmpvop->v_chan->y,
			0,0,
			"error",0,0,0,0,0,0,0,0);
//	  xyuv_show_debug(6,(unsigned short *)tmpvop->a_chan->data->s,
//			  0,0,
//			  datasize(tmpvop->a_chan)&1,
//			  tmpvop->a_chan->x,tmpvop->a_chan->y,
//			  0,0,
//			  "error-alpha",0,0,0,0,0,0,0,0);
	freevop(tmpvop);
      }
#endif
#endif

#ifdef SCALEERROR
      /* scale error*/
      if (!noofpixels) {
	testerror=FLT_MAX;
      }
      else {
	testerror/=(float)noofpixels;
      }
#endif

      /* 4.4.4. if test transformation is not better reduce alpha*/
      if (testerror>=error) {
	fprintf(stderr,"-");
	alphastart*=0.5;
	if (alphastart<alphalimit) break;
      }
      /* 4.4.4. else make test parameters transform paramters and go to 4.1*/
      else {
	int i;

	fprintf(stderr,"u");

	error=testerror;
	for (i = 0; i < modifyparameters; i++) {
	  parameters[i]=testparam[i];
	}
	break;
      }
      fflush(stderr);
    }

    if (!k) break;
  }
#ifdef XYUVDISP
#ifdef INTARITH
#else
//   {
//     MomVop *tmpvop=initvop(transdiff->width,
// 			   transdiff->height,
// 			   _444_,
// 			   UCHAR_TYPE,UCHAR_TYPE,
// 			   UCHAR_TYPE,UCHAR_TYPE);
//     xyuv_show_simple(6,(unsigned short *)tmpvop->u_chan->data->s,
// 		     0,0,
// 		     datasize(tmpvop->u_chan)&1,
// 		     tmpvop->u_chan->x,tmpvop->u_chan->y,
// 		     0,0,
// 		     "final-error");
//     freevop(tmpvop);
//       }
#endif
#endif

#ifdef DEBUG
  fprintf(stderr,"alphamin %0.4f ",alphastart);
#endif

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::
invert_hessian(float inverse[POM_MAXPARAM][POM_MAXPARAM],
	       float matrix[POM_MAXPARAM][POM_MAXPARAM],
	       int dimension)

{
  int i,j,k;
  int l;
  float delta;
  float epsilon;
  float ans;
  float m_mod[POM_MAXPARAM][POM_MAXPARAM];
  float chol[POM_MAXPARAM][POM_MAXPARAM];
  float inv_chol[POM_MAXPARAM][POM_MAXPARAM];

	
#define	DELTA_LIMIT 0.0001
#define EPSILON_START 0.0
  delta   = DELTA_LIMIT;
  epsilon = EPSILON_START;
#undef DELTA_LIMIT
#undef EPSILON_START


  l = 0;
  while (1 == 1) {
    for (i = 0; i < dimension; i++)
      for (j = 0; j < dimension; j++)
	m_mod[i][j] = matrix[i][j];

    for (i = 0; i < dimension; i++)
      m_mod[i][i] += epsilon;

    ans = try_cholesky(chol,m_mod,delta,dimension);

    if (ans == 0.0)
      break;

    epsilon += ans;
    l++;

    if (l > dimension) { // Safety! Return identity of not exiting.
      for (i = 0; i < dimension; i++)
	for (j = 0; j < dimension; j++)
	  if (i == j)
	    inverse[i][j] = 1.0;
	  else
	    inverse[i][j] = 0.0;

      return 1;
    }
  }

  calc_inverse_udiagonal(inv_chol,chol,dimension);

  for (i = 0; i < dimension; i++)
    for (j = 0; j < dimension; j++) {
      inverse[i][j] = 0.0;

      for (k = 0; k < dimension; k++)
	inverse[i][j] += inv_chol[i][k] * inv_chol[j][k];
      // j k exchanged because of transpose!!
    }
  return 0;
}

//----------------------------------------------------------------------------
float ParametricObjectMotionExtractionTool2::
try_cholesky(float res[POM_MAXPARAM][POM_MAXPARAM],
	     float matrix[POM_MAXPARAM][POM_MAXPARAM],
	     float delta,int dimension)

{
  int i,j,k;
  float scale;


  for (i = 0; i < dimension; i++)
    for (j = 0; j < dimension; j++)
      res[i][j] = matrix[i][j];
	
  for (j = 0; j < dimension; j++) {
    if (res[j][j] < delta)
      return (delta - res[j][j]);
				
    for (i = (j+1); i < dimension; i++) {
      scale = res[i][j] / res[j][j];

      res[i][j] = 0.0;
			
      for (k = (j+1); k < dimension; k++)
	res[i][k] = res[i][k] - scale * res[j][k];

    }	
  }

  for (i = 0; i < dimension; i++) {
    scale = 1.0 / sqrt(res[i][i]);

    for (j = i; j < dimension; j++)
      res[i][j] *= scale;
  }
		
  return 0.0;
}
//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool2::
calc_inverse_udiagonal(float inverse[POM_MAXPARAM][POM_MAXPARAM],
		       float matrix[POM_MAXPARAM][POM_MAXPARAM],int dimension)
{
  int i,j,k;
  float scale;


  for (i = 0; i < dimension; i++)
    for (j = 0; j < dimension; j++)
      if (i == j)
	inverse[i][j] = 1.0;
      else
	inverse[i][j] = 0.0;


  for (i = (dimension-1); i >= 0; i--) {
    scale = 1.0 / matrix[i][i];

    for (j = 0; j < dimension; j++)
      inverse[i][j] *= scale;

    for (k = (i-1); k >= 0; k--) {
      scale = matrix[k][i];

      for (j = 0; j < dimension; j++)
	inverse[k][j] = inverse[k][j] - scale * inverse[i][j];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionExtractionTool2::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionExtractionTool2::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionInterfaceABC2 
   *ParametricObjectMotionExtractionTool2::GetInterface(void)
{ return &m_Interface; }

//===========================================================================
/*pixeloperations for image processing low level tools*/
//inter:
//----------------------------------------------------------------------------
void scalepix(TPixel *res,TPixel *in1, TPixel* in2)

{
#ifdef INTARITH
  if (in2->m == MARKER_LOADED) {
    res->a.i=in2->a.i;
  }
  else {
    res->a.i=0;
  }
  res->y.i=in2->y.i;
  res->u.i=in2->u.i;
  res->v.i=in2->v.i;
#else
  if (in2->m == MARKER_LOADED) {
    res->a.f=in2->a.f;
  }
  else {
    res->a.f=0;
  }
  res->y.f=in2->y.f;
  res->u.f=in2->u.f;
  res->v.f=in2->v.f;
#endif
}

//----------------------------------------------------------------------------
void trans_diff_histpix(TPixel *res,TPixel *in1, TPixel* in2)

{
  /* _inter_regout1 : number of pixels
     _inter_infopt  : pointer to histogram*/

#ifdef INTARITH
  if (in2->m == MARKER_LOADED) {
    res->a.i=min(in1->a.i,in2->a.i);
  }
  else {
    res->a.i=0;
  }
  res->y.i=in2->y.i;

  if (res->a.i > 127) {
    res->u.i=in2->y.i- in1->y.i;
    _inter_regout1++;
    ((int *)_inter_infopt)[abs(res->u.i)]++;
    res->v.i=(in2->y.i- in1->y.i+1)>>1;
  }
  else {
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
//    res->u.i=in2->u.i;
  }
//  res->v.i=in2->v.i;// not to be un-initialized

#else //FLOATARITH
  /* _inter_regout2 : minumum error
     _inter_regout3 : maximum error*/

  if (in2->m == MARKER_LOADED) {
    res->a.f=min(in1->a.f,in2->a.f);
//    res->a.f=in1->a.f*in2->a.f;
  }
  else {
    res->a.f=0;
  }
  res->y.f=in2->y.f;

  if (res->a.f >= 0.5) {
    res->u.f=in2->y.f- in1->y.f;
    if (fabs(res->u.f) < TCHAN_TO_FLOAT(_inter_regout2))
      TCHAN_TO_FLOAT(_inter_regout2)=fabs(res->u.f);
    if (fabs(res->u.f) > TCHAN_TO_FLOAT(_inter_regout3))
      TCHAN_TO_FLOAT(_inter_regout3)=fabs(res->u.f);
    res->v.f=(in2->y.f+ in1->y.f)/2.0;
  }
  else {
    res->y.f=0.0;
    res->u.f=0.0;
    res->v.f=0.0;
  }
//  res->v.f=in2->v.f;
#endif
}

//----------------------------------------------------------------------------
void trans_diffpix(TPixel *res,TPixel *in1, TPixel* in2)

{
#ifdef INTARITH
  if (in2->m == MARKER_LOADED) {
    res->a.i=min(in1->a.i,in2->a.i);
  }
  else {
    res->a.i=0;
  }
  res->y.i=in2->y.i;

  if (res->a.i > 127) {
    res->u.i=in2->y.i- in1->y.i;
    res->v.i=(in2->y.i+ in1->y.i+1)>>1;
  }
  else {
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
  }
  //res->v.i=0;
#else
  if (in2->m == MARKER_LOADED) {
    res->a.f=min(in1->a.f,in2->a.f);
//    res->a.f=in1->a.f*in2->a.f;
  }
  else {
    res->a.f=0;
  }
  res->y.f=in2->y.f;

  if (res->a.f >= 0.5) {
    res->u.f=in2->y.f- in1->y.f;
    res->v.f=(in2->y.f+ in1->y.f)/2.0;
  }
  else {
    res->y.f=0.0;
    res->u.f=0.0;
    res->v.f=0.0;
  }
  //res->v.f=0.0;
#endif
}

//intra:
//----------------------------------------------------------------------------
#ifndef INTARITH
void floathistrangepix(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
  res->a=in[12].a;
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;

  if (res->a.f >= 0.5) {
    if (fabs(in[12].u.f) < TCHAN_TO_FLOAT(_inter_regout2))
      TCHAN_TO_FLOAT(_inter_regout2)=fabs(in[12].u.f);
    if (fabs(in[12].u.f) > TCHAN_TO_FLOAT(_inter_regout3))
      TCHAN_TO_FLOAT(_inter_regout3)=fabs(in[12].u.f);
  }
}

//----------------------------------------------------------------------------
void floathistpix(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
  res->a=in[12].a;
  res->y=in[12].y;
  res->u=in[12].u;
  res->v=in[12].v;
  res->ax=in[12].ax;

  if (in[12].a.f >= 0.5) {
    _inter_regout1++;
    ((int *)_inter_infopt)[(int)
			  ((fabs(in[12].u.f)-TCHAN_TO_FLOAT(_inter_regout2))/
			   TCHAN_TO_FLOAT(_inter_regout4)*(POMHISTBINS-1))]++;
  }
}

//----------------------------------------------------------------------------
void int_to_float(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
   res->a.f=((float)in[12].a.i)/255.0;
   res->y.f=((float)in[12].y.i)/255.0;
   res->u.f=((float)in[12].u.i)/255.0;
   res->v.f=((float)in[12].v.i)/255.0;
   res->ax.f=((float)in[12].ax.i)/255.0;
}

#ifdef XYUVDISP
#ifndef INTARITH
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
#endif

//----------------------------------------------------------------------------
#ifdef XYUVDISP
void abs_pix(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
   res->a.i =abs(in[12].a.i);
   res->y.i =abs(in[12].y.i);
   res->u.i =abs(in[12].u.i);
   res->v.i =abs(in[12].v.i);
   res->ax.i=abs(in[12].ax.i);
}
#endif

//----------------------------------------------------------------------------
void ninetapfilter(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
  int i,j;

  res->a.f=0.0;
  res->y.f=0.0;
  res->u.f=0.0;
  res->v.f=0.0;
  res->ax.f=0.0;

  for (i=0, j=8; i<9; i++, j++) {

    float coeff=((float *)_inter_infopt)[i];

    res->a.f+=coeff*in[j].a.f;
    res->y.f+=coeff*in[j].y.f;
    res->u.f+=coeff*in[j].u.f;
    res->v.f+=coeff*in[j].v.f;
    res->ax.f+=coeff*in[j].ax.f;
  }
}
#endif


//----------------------------------------------------------------------------
void antialiaspix(TPixel *res, TPixel *in, TCoor x, TCoor y)

{
  /* in is 5x5 2D array, center is postion number 12*/
  /* 2D 121 filter*/

  res->a=in[12].a;
  res->y.i=(in[6].y.i +( in[7].y.i <<1) + in[8].y.i +
	  (in[11].y.i<<1)+(in[12].y.i<<2)+(in[13].y.i<<1) +
	  in[16].y.i +( in[17].y.i <<1) + in[18].y.i ) >>4;
  res->u.i=(in[6].u.i +( in[7].u.i <<1) + in[8].u.i +
	  (in[11].u.i<<1)+(in[12].u.i<<2)+(in[13].u.i<<1) +
	  in[16].u.i +( in[17].u.i <<1) + in[18].u.i ) >>4;
  res->v.i=(in[6].v.i +( in[7].v.i <<1) + in[8].v.i +
	  (in[11].v.i<<1)+(in[12].v.i<<2)+(in[13].v.i<<1) +
	  in[16].v.i +( in[17].v.i <<1) + in[18].v.i) >>4;
  res->ax=in[12].ax;
}

//----------------------------------------------------------------------------
void abssumoverdiffpix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = threshold
     _inter_regout1 = errorsum*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->u=in[12].u;
   res->v=in[12].v;
   res->ax=in[12].ax;
 
#ifdef INTARITH
   if ((abs(in[12].u.i) < _inter_regin1) && (in[12].a.i > 127)) {
     _inter_regout1+=abs(in[12].u.i);
     _inter_regout2++;;
   }
   else {
     res->v.i=0;
   }
#else
//    if ((fabs(in[12].u.f) < TCHAN_TO_FLOAT(_inter_regin1)) &&
//        (in[12].a.f >= 0.5)) {
   res->u.f=in[12].u.f*in[12].u.f;
   if ((res->u.f < TCHAN_TO_FLOAT(_inter_regin1)) &&
       (in[12].a.f >= 0.5)) {
     TCHAN_TO_FLOAT(_inter_regout1)+=res->u.f;
     _inter_regout2++;;
   }
   else {
     res->v.f=0.0;
   }
   res->u=in[12].u;
#endif
}

//----------------------------------------------------------------------------
void gradientpix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;

#ifdef INTARITH
   res->u.i=(in[8].y.i - in[6].y.i +
	     ((in[13].y.i - in[11].y.i)<<1) +
	     in[18].y.i - in[16].y.i) >> 2;
   res->v.i=(in[16].y.i - in[6].y.i +
	     ((in[17].y.i - in[7].y.i)<<1) +
	     in[18].y.i - in[8].y.i) >> 2;
#else
   res->u.f=(in[8].y.f - in[6].y.f +
	     ((in[13].y.f - in[11].y.f)*2.0) +
	     in[18].y.f - in[16].y.f)/4.0;
   res->v.f=(in[16].y.f - in[6].y.f +
	     ((in[17].y.f - in[7].y.f)*2.0) +
	     in[18].y.f - in[8].y.f)/4.0;
#endif
}

//error gradient for parameter variation:
//----------------------------------------------------------------------------
void graderrtrans(TPixel *res,TPixel *in, TCoor x, TCoor y)
{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;
   res->u=in[12].u;
   res->v=in[12].v;

#ifdef INTARITH
   if ((abs(in[12].y.i) < _inter_regin1) && (in[12].a.i > 127)) {
     ((float*)_intra_infopt)[0]+=(float)in[12].y.i * (float)in[12].u.i;
     ((float*)_intra_infopt)[1]+=(float)in[12].y.i * (float)in[12].v.i;
   }
#else
   if ((fabs(in[12].y.f) < TCHAN_TO_FLOAT(_inter_regin1)) && 
       (in[12].a.f >= 0.5)) {
     ((float*)_intra_infopt)[0]+=in[12].y.f * (float)in[12].u.f;
     ((float*)_intra_infopt)[1]+=in[12].y.f * (float)in[12].v.f;
   }
#endif
}

//----------------------------------------------------------------------------
void graderrrot(TPixel *res,TPixel *in, TCoor x, TCoor y)
{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;
   res->u=in[12].u;
   res->v=in[12].v;

   if ((abs(in[12].y.i) < _inter_regin1) && (in[12].a.i > 127)) {
   }
}

//----------------------------------------------------------------------------
void graderraffine(TPixel *res,TPixel *in, TCoor x, TCoor y)
{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;
   res->u=in[12].u;
   res->v=in[12].v;

#ifdef INTARITH
   if ((abs(in[12].y.i) < _inter_regin1) && (in[12].a.i > 127)) {
     ((float*)_intra_infopt)[0]+=(float)in[12].y.i * (float)in[12].u.i;
     ((float*)_intra_infopt)[1]+=(float)in[12].y.i * (float)in[12].v.i;
     ((float*)_intra_infopt)[2]+=(float)in[12].y.i * (float)in[12].u.i *
       ((float)x-_tr_origins[2]);
     ((float*)_intra_infopt)[3]+=(float)in[12].y.i * (float)in[12].u.i *
       ((float)y-_tr_origins[3]);
     ((float*)_intra_infopt)[4]+=(float)in[12].y.i * (float)in[12].v.i *
       ((float)x-_tr_origins[2]);
     ((float*)_intra_infopt)[5]+=(float)in[12].y.i * (float)in[12].v.i *
       ((float)y-_tr_origins[3]);
   }
#else
   if ((fabs(in[12].y.f) < TCHAN_TO_FLOAT(_inter_regin1)) && 
       (in[12].a.f >= 0.5)) {
     ((float*)_intra_infopt)[0]+=in[12].y.f * in[12].u.f;
     ((float*)_intra_infopt)[1]+=in[12].y.f * in[12].v.f;
     ((float*)_intra_infopt)[2]+=in[12].y.f * in[12].u.f *
       (x-_tr_origins[2]);
     ((float*)_intra_infopt)[3]+=in[12].y.f * in[12].u.f *
       (y-_tr_origins[3]);
     ((float*)_intra_infopt)[4]+=in[12].y.f * in[12].v.f *
       (x-_tr_origins[2]);
     ((float*)_intra_infopt)[5]+=in[12].y.f * in[12].v.f *
       (y-_tr_origins[3]);
   }
#endif
}

//----------------------------------------------------------------------------
void graderrpersp(TPixel *res,TPixel *in, TCoor x, TCoor y)
{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;
   res->u=in[12].u;
   res->v=in[12].v;

   if ((abs(in[12].u.i) < _inter_regin1) && (in[12].a.i > 127)) {
   }
}

//----------------------------------------------------------------------------
void graderrparabo(TPixel *res,TPixel *in, TCoor x, TCoor y)
{
  /* in is 5x5 2D array, center is postion number 12*/
  /* _inter_regin1 = error threshold*/

   res->a=in[12].a;
   res->y=in[12].y;
   res->ax=in[12].ax;
   res->u=in[12].u;
   res->v=in[12].v;

   if ((abs(in[12].u.i) < _inter_regin1) && (in[12].a.i > 127)) {
   }
}

