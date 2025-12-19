//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// Applicable File Name:  MosaicExtraction.cpp
//

#include <stdio.h>
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
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"

//#define FIXEDMOSAICSIZE
#define m_CreateMosaic 1

static void (*m_Transformation)(TCoor width,TCoor height,
				TCoor inx,TCoor iny,
				float *outx,float *outy);

using namespace XM;

const UUID MosaicExtractionInterface::myID = UUID("");
const char *MosaicExtractionInterface::myName = "MosaicExtractionInterface";

const UUID MosaicExtractionTool::myID = UUID("");
const char *MosaicExtractionTool::myName = "MosaicExtraction";

const UUID MosaicExtractionInterfaceABC::myID = UUID("");

//=============================================================================
MosaicExtractionInterfaceABC::MosaicExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MosaicExtractionInterface::MosaicExtractionInterface(
                 MosaicExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MosaicExtractionInterface::~MosaicExtractionInterface()
{
}

//----------------------------------------------------------------------------
void MosaicExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int MosaicExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC *Media)
{
  return m_ExtractionTool->SetSourceMedia(Media);
}

//----------------------------------------------------------------------------
int MosaicExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long MosaicExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long MosaicExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long MosaicExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
MomVop* MosaicExtractionInterface::GetMosaic(void)

{
  return m_ExtractionTool->GetMosaic();
}

//----------------------------------------------------------------------------
const UUID& MosaicExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
MosaicExtractionTool::MosaicExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
//m_CreateMosaic(0),
m_MosaicX(512),
m_MosaicY(512),
m_SetBackground(0),
m_nnfraction(30),
m_nn(1),
m_SkipBoundary(0),
m_MosaicInterface(0),
m_CurrTime(-1),
m_NextTime(0),
m_Mosaic(0),
m_Next(0),
m_NextXSize(0),
m_NextYSize(0),
m_Sets(0),
m_OffsetX(0),
m_OffsetY(0)

{
  ParametricObjectMotionExtractionTool2 *pomextraction;

  pomextraction = new ParametricObjectMotionExtractionTool2(0);
  m_MotionExtraction=pomextraction->GetInterface();

  m_MotionD = m_MotionExtraction->GetDescriptorInterface();

  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
MosaicExtractionTool::MosaicExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptorInterface(0),
//m_CreateMosaic(0),
m_MosaicX(512),
m_MosaicY(512),
m_SetBackground(0),
m_nnfraction(30),
m_nn(1),
m_SkipBoundary(0),
m_MosaicInterface(0),
m_CurrTime(-1),
m_NextTime(0),
m_Mosaic(0),
m_Next(0),
m_NextXSize(0),
m_NextYSize(0),
m_Sets(0),
m_OffsetX(0),
m_OffsetY(0)

{
  ParametricObjectMotionExtractionTool2 *pomextraction;

  pomextraction = new ParametricObjectMotionExtractionTool2(0);
  m_MotionExtraction=pomextraction->GetInterface();

  m_MotionD = m_MotionExtraction->GetDescriptorInterface();

  SetDescriptorInterface(theParent);

  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
MosaicExtractionTool::~MosaicExtractionTool()
{
  /* release descriptor*/
  if (m_MotionExtraction) {
    m_MotionExtraction->destroy();
    m_MotionExtraction=0;
  }

  if (m_Mosaic) freevop(m_Mosaic);
  if (m_Next) freevop(m_Next);
}

//----------------------------------------------------------------------------
bool MosaicExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool MosaicExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MosaicExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *Media)

{
  MomVop *VOP;

  if (!Media) return -1;
  VOP=Media->GetImage();
  if (!VOP) return -1;

  {
    /* shift time values*/
    m_CurrTime=m_NextTime;
    m_NextTime=Media->GetImageTime();

    /* make a copy from alpha channel and reduce object mask*/
    if ((VOP->width != m_NextXSize) ||
	(VOP->height != m_NextYSize)) {

      m_NextXSize=VOP->width;
      m_NextYSize=VOP->height;

      /* create new alpha image if required*/
      if (m_Next) freevop(m_Next);
      m_Next=initvop(VOP->width,VOP->height,_444_,
			 UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
    }
    /* set media*/
    erosion_skip_boundary(m_Next,VOP,m_SkipBoundary);

    /* set inital values for origin and offset*/
//     if (m_Origiontype) {
//       m_Origins[2]=(m_Next->width-1)/2.0; /*input image*/
//       m_Origins[3]=(m_Next->height-1)/2.0;
//     }
//     else {
//       m_Origins[2]=0.0;
//       m_Origins[3]=0.0;
//     }

//     m_SetBackground=0;

    /* set media to parametric object motion*/
    m_MotionExtraction->SetNextSourceMedia(VOP);
  }

  return 0;
}

//----------------------------------------------------------------------------
// GenericDSInterfaceABC*
// MosaicExtractionTool::
// GetDescriptorInterface(void)

// {
//   return m_DescriptorInterface;
// }

//----------------------------------------------------------------------------
int MosaicExtractionTool::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicExtractionTool::InitExtracting(void)
{
  //  char parametername[256];
  //  int newlevels;
  int modelcode;

  if (!m_DescriptorInterface)  return (unsigned long)-1;

  /* read paramters*/
  /* origion type*/
  m_Origiontype=atoi(getpara("POMOrigin","1"));
  /* default = origin in center of image*/
  if (m_Origiontype) {
      m_Origins[0]=m_Origins[2]=(m_Next->width-1)/2.0; /*mosaic*/
      m_Origins[1]=m_Origins[3]=(m_Next->height-1)/2.0;
    }
    else {
      m_Origins[0]=m_Origins[2]=0.0;
      m_Origins[1]=m_Origins[3]=0.0;
    }

  /* create mosaic flag*/
//  m_CreateMosaic=atoi(getpara("CreateMosaic","0")); //now a define

  /* read model code*/
  /* default = 2 = affine*/
  modelcode=atoi(getpara("MotionModel","2"));
  switch (modelcode) {
  case 0:
    m_NoOfParams=2;
    m_Transformation=&tranlational;
    break;
//   case 1:
//     m_NoOfParams=4;
//     m_Transformation=&rotational;
//     break;
  case 2:
    m_NoOfParams=6;
    m_Transformation=&affine;
    break;
//   case 3:
//     m_NoOfParams=8;
//     m_Transformation=&perspective;
//     break;
//   case 4:
//     m_NoOfParams=12;
//     m_Ttransformation=&parabolic;
//     break;
  default:
    fprintf(stderr,"Warning: unsupported motion model, skip mosaic creation\n");
//    m_CreateMosaic=0;
    return (unsigned long)-1;
  }

  m_nnfraction=atoi(getpara("nnfraction","30"));
  m_nn=atoi(getpara("nn","1"));

  m_SkipBoundary=atoi(getpara("SkipBoundaryWidth","0"));

  if (m_CreateMosaic) {
    m_SetBackground=1;

    /* init motion parameters*/
    {
      int i;
      for(i=0; i<POM_MAXPARAM; m_Param[i++]=0.0);
    }

    /* mosaic creation type*/
    m_MosaicType=atoi(getpara("MosaicType","0"));
    /* default = 0 = past*/
    /*           1 = extend*/
    /*           2 = merge*/

    /* size of mosaic (default =1024x1024)*/
    m_MosaicX=atoi(getpara("MosaicX","1024"));
    m_MosaicY=atoi(getpara("MosaicY","1024"));
  }

  /* set values for motion parameter extraction*/ 
  m_MotionD->SetModelCode(modelcode);
  m_MotionExtraction->InitExtracting();

  /* create MosiacDS Node and set values*/
  if (!m_DescriptorInterface)  return (unsigned long)-1;

  /* create mosaic description*/
  m_MosaicD=m_DescriptorInterface->
    CreateChild("MosaicWarpingParameters");
  m_MosaicInterface=m_MosaicD.GetInterface();

  m_MosaicInterface->SetAttribute("ModelType",m_NoOfParams);

  m_Sets=0;
  m_OffsetX=0;
  m_OffsetY=0;

  return 0;
}



//----------------------------------------------------------------------------
unsigned long MosaicExtractionTool::StartExtracting(void)

{
  int iminx,iminy,imaxx,imaxy;
  double currparam[POM_MAXPARAM]={0.0,0.0,0.0,0.0,0.0,0.0,
				    0.0,0.0,0.0,0.0,0.0,0.0};
  
  /* check for descriptor*/
  if (!m_MosaicInterface) return (unsigned long)-1;

#ifdef USEXML
  if (m_MosaicInterface->GetDSName()!= "MosaicWarpingParameters") {
    return (unsigned long)-1;
  }
#endif

  if (m_Mosaic) {
    m_MotionExtraction->SetCurrentSourceMedia(m_Mosaic);
    m_MotionExtraction->SetOrigins(m_Origins);
  }

  /*******************************/
  /* 1. compute motion model parameters with POM Extraction2
     2. compute accumulated parameters
     3. invert them accumulated parameters
     4. write accumulated and parameters to description
     5. compute new image size
     6. transform image and add it to mosaic
  */

  /* check if first frame*/
  if (m_SetBackground) {
    int i;
    for(i=0; i<m_NoOfParams; i++) {
      currparam[i]=0;
    }
  }
  else {
    /* 1. compute motion model paramters with POM Extraction2*/
    m_MotionExtraction->StartExtracting();
    {
      int i;
      for(i=0; i<m_NoOfParams; i++) {
	m_MotionD->GetMotionParameterValue(i,currparam[i]);
      }
    }
  }

  /* 2. compute accumulated parameters and invert them*/
  /* if no mosic is created return here*/
  //  if (!m_CreateMosaic) return 0;

#ifdef FRAME_TO_FRAME
  switch (m_NoOfParams) {
  case 2:
  case 6:
    /* get total motion parameters*/
      m_Param[0] = m_Param[0] + m_Param[2]*currparam[0] +
	m_Param[3]*currparam[1] + currparam[0];
      m_Param[1] = m_Param[1] + m_Param[4]*currparam[0] +
	m_Param[5]*currparam[1] + currparam[1];
      m_Param[2] = m_Param[2] + m_Param[2]*currparam[2] +
	m_Param[3]*currparam[4] + currparam[2];
      m_Param[3] = m_Param[3] + m_Param[2]*currparam[3] +
	m_Param[3]*currparam[5] + currparam[3];
      m_Param[4] = m_Param[4] + m_Param[4]*currparam[2] +
	m_Param[5]*currparam[4] + currparam[4];
      m_Param[5] = m_Param[5] + m_Param[4]*currparam[3] +
	m_Param[5]*currparam[5] + currparam[5];	
      break;
  default:
    fprintf(stderr,"Waring unsupported Motion Model in Mosic\n");
    return (unsigned long) -1;
  }
#else
  m_Param[0] = currparam[0];
  m_Param[1] = currparam[1];
  m_Param[2] = currparam[2];
  m_Param[3] = currparam[3];
  m_Param[4] = currparam[4];
  m_Param[5] = currparam[5];	
#endif

//#ifndef FIXEDMOSAICSIZE

  /* 3. invert the accumulated parameters*/
  /* get new origin and framesize*/
  {
    float iparam[POM_MAXPARAM];
    float iorigin[4];
    float minx,miny,maxx,maxy;
    float x,y;

    if (!invertaffine(m_Param, m_Origins, iparam, iorigin)) {
   
      /* 4. write accumulated and parameters to description*/
      /* add time reference*/
      {
	GenericDS l_Time=m_MosaicInterface->CreateChild("MediaTime");
	GenericDS l_RelIncTime=l_Time.CreateChild("RelIncTime");

	l_RelIncTime.SetAttribute("nnFraction",m_nnfraction);
	l_RelIncTime.SetAttribute("nn",m_nn);
	l_RelIncTime.SetValue((int)m_NextTime);
      }

      {
	int i;
	for(i=0; i<m_NoOfParams; i++) {
      
	  /* add parmeters to description*/
	  {
	    GenericDS l_Param=
	      m_MosaicInterface->CreateChild("MotionParmeters");
	    l_Param.SetValue(iparam[i]);

#ifdef VERBOSE
	    fprintf(stderr,"Warning: Watch out for different parmeter "
		    "definition of POM and Mosaic\n");
#endif
	  }
	}
      }


      /* increment counter for parameter sets*/
      m_Sets++;

      /* 5. compute new image size*/
      if (m_SetBackground) {
	/* set size of image*/
	/* create mosaic with new size*/
	if (m_Mosaic) freevop(m_Mosaic);
	m_Mosaic=initvop(m_Next->width,m_Next->height,_444_,
			  UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
	memset(m_Mosaic->a_chan->data->u,0,m_Next->width*m_Next->height);
	memset(m_Mosaic->y_chan->data->u,0,m_Next->width*m_Next->height);
	memset(m_Mosaic->u_chan->data->u,0,m_Next->width*m_Next->height);
	memset(m_Mosaic->v_chan->data->u,0,m_Next->width*m_Next->height);
      }
      else {

	/* comput new image size*/
	/* make inverse transformation parmeter public for transformation*/
	_tr_parameters=iparam;
	_tr_origins=iorigin;

	/* test first corner*/
	m_Transformation(0,0,0,0,&x,&y);
	maxx=minx=x;
	maxy=miny=y;

	/* test second corner*/
	m_Transformation(0,0,m_Next->width,0,&x,&y);
	if (x>maxx) maxx=x;
	if (x<minx) minx=x;
	if (y>maxy) maxy=y;
	if (y<miny) miny=y;
	/* test third corner*/
	m_Transformation(0,0,0,m_Next->height,&x,&y);
	if (x>maxx) maxx=x;
	if (x<minx) minx=x;
	if (y>maxy) maxy=y;
	if (y<miny) miny=y;
	/* test fourth corner*/
	m_Transformation(0,0,m_Next->width,m_Next->height,&x,&y);
	if (x>maxx) maxx=x;
	if (x<minx) minx=x;
	if (y>maxy) maxy=y;
	if (y<miny) miny=y;

	imaxx=(int)(maxx+0.5);
	iminx=(int)(minx-0.5);
	imaxy=(int)(maxy+0.5);
	iminy=(int)(miny-0.5);

	/* check if new mosaic is bigger than previous*/
	if ((iminx<0) || (imaxx > m_Mosaic->width) ||
	    (iminy<0) || (imaxy > m_Mosaic->height)) {

	  MomVop* newmosaic;
	  int pminx,pminy/*,pmaxx,pmaxy*/;

	  pminx=pminy=0;

	  /* if image is extented into positive*/
	  if (imaxx > m_Mosaic->width) {
	    if (imaxx > m_MosaicX) {
	      imaxx = m_MosaicX;
	    }
	    else {
	      imaxx=(imaxx+1) &~1;
	    }
	  }
	  else {
	    imaxx=m_Mosaic->width;
	  }
	  
	  if (imaxy > m_Mosaic->height) {
	    if (imaxy >  m_MosaicY) {
	      imaxy = m_MosaicY;
	    }
	    else {
	      imaxy=(imaxy+1) &~1;
	    }
	  }
	  else {
	    imaxy=m_Mosaic->height;
	  }

	  /* if image is extented into negative*/
	  if (iminx<0) {
	    pminx=(-iminx+1)&~1;
	    if ((pminx+imaxx) > m_MosaicX) {
	      pminx=m_MosaicX-imaxx;
#ifdef DEBUG	    
	      fprintf(stderr,"Warning: Mosaic is not resized (Limit X=%d)\n",
		      m_MosaicX);
#endif
	    }
	    m_OffsetX+=pminx;
	    m_Origins[0]+=pminx;
	    imaxx+=pminx;
	    iminx+=pminx;
	  }

	  if (iminy<0) {
	    pminy=(-iminy+1)&~1;
	    if ((pminy+imaxy) > m_MosaicY) {
	      pminy=m_MosaicY-imaxy;
#ifdef DEBUG	    
	      fprintf(stderr,"Warning: Mosaic is not resized (Limit Y=%d)\n",
		      m_MosaicY);
#endif
	    }
	    m_OffsetY+=pminy;
	    m_Origins[1]+=pminy;
	    imaxy+=pminy;
	    iminy+=pminy;
	  }

#ifdef DEBUG
	  fprintf(stderr,"Mosaic: Size (%d,%d) Origin (%f,%f)\n",
		  imaxx,imaxy,m_Origins[0],m_Origins[1]);
#endif
	  /* create mosaic with new size*/
	  newmosaic=initvop(imaxx,imaxy,_444_,
			    UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
	  memset(newmosaic->a_chan->data->u,0,imaxx*imaxy);
	  memset(newmosaic->y_chan->data->u,0,imaxx*imaxy);
	  memset(newmosaic->u_chan->data->u,0,imaxx*imaxy);
	  memset(newmosaic->v_chan->data->u,0,imaxx*imaxy);

	  /*copy old values to new mosaic*/
	  intra_proc(newmosaic,m_Mosaic,0,
		     Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH,
		     CON_0,SCAN_HO,
		     -1,-1,-1,-1,-1,
		     m_Mosaic->width,m_Mosaic->height,pminx,pminy,0,0,
		     size1to1,cp_apix);

#ifdef XYUVDISP
	  MomVop *tmpvop=initvop(m_Mosaic->width,
				 m_Mosaic->height,
				 _444_,
				 UCHAR_TYPE,UCHAR_TYPE,
				 UCHAR_TYPE,UCHAR_TYPE);
	  convertRGB_To_YUV(tmpvop,m_Mosaic);
	  xyuv_show_debug(4,(unsigned short *)tmpvop->y_chan->data->s,
			  (unsigned short *)tmpvop->u_chan->data->s,
			  (unsigned short *)tmpvop->v_chan->data->s,
			  datasize(tmpvop->y_chan)&1,
			  tmpvop->y_chan->x,tmpvop->y_chan->y,
			  tmpvop->y_chan->x,tmpvop->y_chan->y,
			  "mosaic",0,0,0,0,0,0,0,0);
	  freevop(tmpvop);
#endif

	  /* delete old mosaic and make new mosaic to old mosaic*/
	  freevop(m_Mosaic);
	  m_Mosaic=newmosaic;
	}

	else {
	  iminx=iminy=0;
	  imaxx=m_Mosaic->width;
	  imaxy=m_Mosaic->height;
	}
      }
    }
  }
//#endif // not  FIXEDMOSAICSIZE



  /* make transformation parmeter public for transformation*/
  _tr_parameters=m_Param;
  _tr_origins=m_Origins;
  
  /* 6. transform image and add it to mosaic*/
  switch (m_MosaicType) {
  case 1:
    trans_extend(m_Mosaic,m_Next,m_Transformation);
    break;
  case 2:
    trans_merge(m_Mosaic,m_Next,m_Transformation);
    break;
  default:
    trans_past(m_Mosaic,m_Next,m_Transformation);
    break;
  }

  /* add foreground*/
  /* select forground after thresholding with 128*/
//   theMediaInterface->SelectVopAlphaLayer(ForgroundFromAlpha);
//  trans_past(m_Mosaic,m_Next,m_Transformation);



#ifdef XYUVDISP
  MomVop *tmpvop=initvop(m_Mosaic->width,
			 m_Mosaic->height,
			 _444_,
			 UCHAR_TYPE,UCHAR_TYPE,
			 UCHAR_TYPE,UCHAR_TYPE);
  convertRGB_To_YUV(tmpvop,m_Mosaic);
  xyuv_show_debug(4,(unsigned short *)tmpvop->y_chan->data->s,
		   (unsigned short *)tmpvop->u_chan->data->s,
		   (unsigned short *)tmpvop->v_chan->data->s,
		   datasize(tmpvop->y_chan)&1,
		   tmpvop->y_chan->x,tmpvop->y_chan->y,
		   tmpvop->y_chan->x,tmpvop->y_chan->y,
		   "mosaic",0,0,0,0,0,0,0,0);
  freevop(tmpvop);
#endif

  m_SetBackground=0;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicExtractionTool::PostExtracting(void)

{
  if (!m_MosaicInterface) return (unsigned long) -1;
  if (!m_Next) return (unsigned long) -1;

  m_MosaicInterface->SetAttribute("SourceSequenceWidth",m_Next->width);
  m_MosaicInterface->SetAttribute("SourceSequenceHeight",m_Next->height);
  m_MosaicInterface->SetAttribute("OriginX",m_Origins[0]);
  m_MosaicInterface->SetAttribute("OriginY",m_Origins[1]);
  m_MosaicInterface->SetAttribute("OffsetX",m_OffsetX);
  m_MosaicInterface->SetAttribute("OffsetY",m_OffsetY);
  m_MosaicInterface->SetAttribute("NoOfMotionParameterSets",m_Sets);

  return 0;
}

//----------------------------------------------------------------------------
MomVop* MosaicExtractionTool::GetMosaic(void)

{
  return m_Mosaic;
}

//----------------------------------------------------------------------------
const UUID& MosaicExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
MosaicExtractionInterfaceABC 
   *MosaicExtractionTool::GetInterface(void)
{ return &m_Interface; }

//===========================================================================
/*pixeloperations for image processing low level tools*/
//inter:
void trans_pastpix(TPixel *res,TPixel *in1, TPixel* in2)

{
  /* in1 pixel from previous frame
     in2 pixel from transformed frame*/
  
  res->ax.i=0;
  if ((in2->m == MARKER_LOADED) && (in2->a.i>127)) {
    res->a.i=in2->a.i;
    res->y.i=in2->y.i;
    res->u.i=in2->u.i;
    res->v.i=in2->v.i;
  }
  else if (in1->a.i>127) {
    res->a.i=in1->a.i;
    res->y.i=in1->y.i;
    res->u.i=in1->u.i;
    res->v.i=in1->v.i;
  }
  else {
    res->a.i=0;
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
  }
}

//----------------------------------------------------------------------------
void trans_extendpix(TPixel *res,TPixel *in1, TPixel* in2)

{
  /* in1 pixel from previous frame
     in2 pixel from transformed frame*/
  
  res->ax.i=0;
  if (in1->a.i==255) {
    res->a.i=in1->a.i;
    res->y.i=in1->y.i;
    res->u.i=in1->u.i;
    res->v.i=in1->v.i;
  }
  else if ((in2->m == MARKER_LOADED)/* && (in2->a.i==255)*/) {
    res->a.i=in2->a.i;
    res->y.i=in2->y.i;
    res->u.i=in2->u.i;
    res->v.i=in2->v.i;
  }
  else {
    res->a.i=0;
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
  }
}

//----------------------------------------------------------------------------
void trans_mergepix(TPixel *res,TPixel *in1, TPixel* in2)

{
  float a=0,y=0,u=0,v=0;
  int n=0;

  /* in1 pixel from previous frame
     in2 pixel from transformed frame*/


  res->ax.i=0;
  if ((in2->m == MARKER_LOADED) && (in2->a.i)) {
    a=in2->a.i;
    y=in2->y.i*in2->a.i;
    u=in2->u.i*in2->a.i;
    v=in2->v.i*in2->a.i;
    n++;
  }
  if (in1->a.i) {
    a+=in1->a.i;
    y+=in1->y.i*in1->a.i;
    u+=in1->u.i*in1->a.i;
    v+=in1->v.i*in1->a.i;
    n++;
  }
  if (n) {
    res->y.i=(int) ((y+a/2)/a); 
    res->u.i=(int) ((u+a/2)/a); 
    res->v.i=(int) ((v+a/2)/a); 
    res->a.i=(int) ((a+(n>>1)/n));
  }
  else {
    res->y.i=0; 
    res->u.i=0; 
    res->v.i=0; 
    res->a.i=0;
  }
}

//intra:
//----------------------------------------------------------------------------
void cp_apix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{

  res->a=in[12].a;
  res->ax=in[12].ax;
  if (in[12].a.i) {
    res->y.i=in[12].y.i;
    res->u.i=in[12].u.i;
    res->v.i=in[12].v.i;
  }
  else {
    res->y.i=0;
    res->u.i=0;
    res->v.i=0;
  }
}

//----------------------------------------------------------------------------
void aclean_pix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{
  /* _intra_regin1= min value for x and y
     _intra_regin2= max value for x
     _intra_regin3= max value for y */
  res->ax=in[12].ax;
  res->y.i=in[12].y.i;
  res->u.i=in[12].u.i;
  res->v.i=in[12].v.i;

  if ((x>=_intra_regin1) && (x<_intra_regin2) &&
      (y>=_intra_regin1) && (y<_intra_regin3) &&
      (in[6].a.i > 127) && (in[7].a.i > 127) &&
      (in[8].a.i > 127) && (in[11].a.i > 127) &&
      (in[13].a.i > 127) && (in[16].a.i > 127) &&
      (in[17].a.i > 127) && (in[18].a.i > 127)) {
    res->a=in[12].a;
  }
  else {
    res->a.i=0;
  }    

}

//----------------------------------------------------------------------------
void bb_apix(TPixel *res,TPixel *in, TCoor x, TCoor y)

{

  res->a=in[12].a;
  res->ax=in[12].ax;
  res->y.i=in[12].y.i;
  res->u.i=in[12].u.i;
  res->v.i=in[12].v.i;
  if (in[12].a.i) {
    if (_intra_regout1 < x) _intra_regout1=x;
    if (_intra_regout2 > x) _intra_regout2=x;
    if (_intra_regout3 < y) _intra_regout3=y;
    if (_intra_regout4 > y) _intra_regout4=y;
  }
}

