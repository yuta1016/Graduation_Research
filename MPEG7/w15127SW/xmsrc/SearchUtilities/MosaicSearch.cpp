//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MosaicSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"
#include "momusys.h"
#include "intra.h"
#include "inter.h"
#include "ExtractionUtilities/coortrans.h"

static void (*m_Transformation)(TCoor width,TCoor height,
				TCoor inx,TCoor iny,
				float *outx,float *outy);

//=============================================================================
	

using namespace XM;


const UUID MosaicSearchInterface::myID = UUID("");
const char *MosaicSearchInterface::myName = "MosaicMatchingInterface";

const UUID MosaicSearchTool::myID = UUID("");
const char *MosaicSearchTool::myName = "MosaicMatchingTool";

const UUID MosaicSearchInterfaceABC::myID = UUID();

//=============================================================================
MosaicSearchInterfaceABC::MosaicSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MosaicSearchInterface::MosaicSearchInterface(MosaicSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MosaicSearchInterface::~MosaicSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MosaicSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void MosaicSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int MosaicSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefMosaicDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefMosaicDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* MosaicSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int MosaicSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QueryMosaicDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryMosaicDescriptionInterface);
}

//----------------------------------------------------------------------------
int MosaicSearchInterface::SetMedia(MultiMediaInterfaceABC* media)

{
  return m_SearchTool->SetMedia(media);
}

//----------------------------------------------------------------------------
double MosaicSearchInterface::InitSearch(void)
{
  return m_SearchTool->InitSearch();
}

//----------------------------------------------------------------------------
double MosaicSearchInterface::StartSearch(void)
{
  return m_SearchTool->StartSearch();
}

//----------------------------------------------------------------------------
double MosaicSearchInterface::PostSearch(void)
{
  return m_SearchTool->PostSearch();
}

//----------------------------------------------------------------------------
MultiMediaInterfaceABC *MosaicSearchInterface::GetOutMedia(void)
{
  return m_SearchTool->GetOutMedia();
}

//=============================================================================
MosaicSearchTool::MosaicSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  MultiMedia *OutMedia;

  /* create output media object*/
  OutMedia = new MultiMedia;
  if (OutMedia) m_OutMedia=OutMedia->GetInterface();
  else {
    m_OutMedia=0;
    fprintf(stderr,"Warning: Out of Memory\n");
  }
  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
MosaicSearchTool::
MosaicSearchTool(GenericDSInterfaceABC
		      *aQueryDescriptionInterface):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  MultiMedia *OutMedia;

  /* connect Descritors with Searchs*/
  SetQueryDescriptionInterface(aQueryDescriptionInterface);

  /* create output media object*/
  OutMedia = new MultiMedia;
  if (OutMedia) m_OutMedia=OutMedia->GetInterface();
  else {
    m_OutMedia=0;
    fprintf(stderr,"Warning: Out of Memory\n");
  }
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MosaicSearchTool::~MosaicSearchTool()
{
//   MomVop *outvop;

  if (!m_OutMedia) return;

//   outvop=m_OutMedia->GetImage();

//   if (outvop) freevop(outvop);
//   m_OutMedia->SetImage(0);

  m_OutMedia->destroy();;
}

//----------------------------------------------------------------------------
const UUID& MosaicSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MosaicSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MosaicSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MosaicSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefMosaicDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefMosaicDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* MosaicSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  MosaicSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryMosaicDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQueryMosaicDescriptionInterface;
  
  /* could also look totally different if
     aQueryMosaicDescriptionInterface is not of Mosaic node*/

  return 0;
}

//----------------------------------------------------------------------------
int MosaicSearchTool::SetMedia(MultiMediaInterfaceABC* media)

{
  if(media==NULL) return -1;
  m_Media=media;

  return 0;
}

//----------------------------------------------------------------------------
double MosaicSearchTool::InitSearch(void)
{
  int modelcode;
  double value=1.0;

  /* init motion parameters*/
  {
    int i;
    for(i=0; i<POM_MAXPARAM; m_Param[i++]=0.0);
  }

  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL)   return(DBL_MAX);
//   if(m_QueryDescriptionInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);
//   if(strcmp(m_QueryDescriptionInterface->GetName(),
// 	    "GenericDSInterface") != 0) return(DBL_MAX);

  /* if ref is Mosaic*/
  if (m_RefDescriptionInterface->GetDSName()=="MosaicWarpingParameters") {
    m_MosaicInterface=m_RefDescriptionInterface;
  }
  else {
    /* else get first Mosaic*/
    m_Mosaic=m_RefDescriptionInterface->
      GetDescription("MosaicWarpingParameters",0);
    string a = m_Mosaic.GetDSName();

    /* Mosaic not found*/
    if (m_Mosaic.isNull()) return -1.0;
    /* Mosaic found*/
    m_MosaicInterface=m_Mosaic.GetInterface();
  }

  /* initialize time node*/
  m_MediaTime=m_MosaicInterface->GetDescription("MediaTime",0);
  string a = m_MediaTime.GetDSName();
  if (m_MediaTime.isNull()) {
    fprintf(stderr,"No MediaTime in Mosaic\n");
    return -1.0;
  }

  /* read motion model type and compute no of parameters*/
  m_MosaicInterface->GetIntAttribute("ModelType",modelcode);
  switch (modelcode) {
  case 2:
    m_NoOfParams=2;
    m_Transformation=&tranlational;
    break;
//   case 4:
//     m_NoOfParams=4;
//     m_Transformation=&rotational;
//     break;
  case 6:
    m_NoOfParams=6;
    m_Transformation=&affine;
    break;
//   case 8:
//     m_NoOfParams=8;
//     m_Transformation=&perspective;
//     break;
//   case 12:
//     m_NoOfParams=12;
//     m_Ttransformation=&parabolic;
//     break;
  default:
    fprintf(stderr,"Warning: unsupported motion model, skip inverse mosaic\n");
    return -1.0;
  }

  /* read framesize of original image*/
  {
    int tmpint;
    m_MosaicInterface->GetIntAttribute("SourceSequenceWidth",tmpint);
    xsize=tmpint;
    m_MosaicInterface->GetIntAttribute("SourceSequenceHeight",tmpint);
    ysize=tmpint;
  }
  
  /* create output image*/
  {
    MomVop *outvop=m_OutMedia->GetImage();

    if (!outvop || ((outvop->width!=xsize) || (outvop->height!=ysize))) {
      freevop(outvop);
      outvop=initvop(xsize,ysize,_444_,
		     UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
      if (!outvop) return -1.0;
      m_OutMedia->SetImage(outvop);
    }
  }


  /* read origin for transformation*/
  {
    double tmpdouble;
    m_MosaicInterface->GetRealAttribute("OriginX",tmpdouble);
    m_Origins[2]=tmpdouble;
    m_MosaicInterface->GetRealAttribute("OriginY",tmpdouble);
    m_Origins[3]=tmpdouble;

    /*comput output origin from size input origin and offset*/
    m_MosaicInterface->GetRealAttribute("OffsetX",tmpdouble);
    m_Origins[0]=m_Origins[2]-tmpdouble;
    m_MosaicInterface->GetRealAttribute("OffsetY",tmpdouble);
    m_Origins[1]=m_Origins[3]-tmpdouble;
  }

  // All done
  return(value);
}

//----------------------------------------------------------------------------
double MosaicSearchTool::StartSearch(void)
{
  GenericDS l_RelIncTime,
    l_Param;
  int FrameTime;
  double value=1.0;
  float currparam[POM_MAXPARAM]={0.0,0.0,0.0,0.0,0.0,0.0,
				    0.0,0.0,0.0,0.0,0.0,0.0};
  MomVop *outvop;

  while (!m_MediaTime.isNull()) {

    l_RelIncTime=m_MediaTime.GetDescription("RelIncTime",0);
    if (l_RelIncTime.isNull()) {
      fprintf(stderr,"MediaTime without RelIncTime in Mosaic\n");
      return -1.0;
    }

    l_RelIncTime.GetIntValue(FrameTime);

    if (FrameTime>m_Media->GetImageTime()) {
      return -1.0;
    }
    if (FrameTime==m_Media->GetImageTime()) break;

    m_MediaTime=m_MediaTime.GetNextSibling("MediaTime");
  }

  /* if no more time entries*/
  if (m_MediaTime.isNull()) return -2.0;


  /* read motion parameters*/
  {
    double tmpdouble;

    l_Param=m_MediaTime.GetNextSibling("MotionParmeters");
    if (l_Param.isNull()) return -1.0;
    l_Param.GetRealValue(tmpdouble);
    currparam[0]=tmpdouble;
    {
      for(int i=1; i<m_NoOfParams; i++) {
	l_Param=l_Param.GetNextSibling("MotionParmeters");
	if (l_Param.isNull()) return -1.0;
	l_Param.GetRealValue(tmpdouble);
	currparam[i]=tmpdouble;
      }
    }
  }

  if (!m_OutMedia) return -1.0;
  outvop=m_OutMedia->GetImage();
  if (!outvop) return -1.0;

  _tr_parameters=currparam;
  _tr_origins=m_Origins;

  /* perform transformation*/
  trans_past(outvop,m_Media->GetImage(),m_Transformation);

  // All done
  return(value);
}

//----------------------------------------------------------------------------
double MosaicSearchTool::PostSearch(void)
{
  return 0.0;
}

//----------------------------------------------------------------------------
MultiMediaInterfaceABC *MosaicSearchTool::GetOutMedia(void)
{
  return m_OutMedia;
}


//----------------------------------------------------------------------------
MosaicSearchInterfaceABC *MosaicSearchTool::GetInterface(void)
{ return &m_Interface; }

