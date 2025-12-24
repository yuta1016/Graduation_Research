//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich Univeristy of Technology
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
// Applicable File Name:  MultiImageSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/readparam.h"
//=============================================================================
	

using namespace XM;


const UUID MultiImageSearchInterface::myID = UUID("");
const char *MultiImageSearchInterface::myName = "MultiImageMatchingInterface";

const UUID MultiImageSearchTool::myID = UUID("");
const char *MultiImageSearchTool::myName = "MultiImageMatchingTool";

const UUID MultiImageSearchInterfaceABC::myID = UUID();

//=============================================================================
MultiImageSearchInterfaceABC::MultiImageSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MultiImageSearchInterface::MultiImageSearchInterface(MultiImageSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiImageSearchInterface::~MultiImageSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MultiImageSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void MultiImageSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int MultiImageSearchInterface::
SetRefDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *RefMultiImageDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefMultiImageDescriptorInterface);
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC* MultiImageSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int MultiImageSearchInterface::
SetQueryDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *QueryMultiImageDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryMultiImageDescriptorInterface);
}

//----------------------------------------------------------------------------
double MultiImageSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
MultiImageSearchTool::MultiImageSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_ScalableColorScale(0.0),
m_ScalableColorWeight(0.0),
m_ContourShapeScale(0.0),
m_ContourShapeWeight(0.0)

{
  /* create sub Searchs and
     connect them with descriptor if availabe*/
  ScalableColorSearchTool *aScalableColorSearch=
    new ScalableColorSearchTool();
  ContourShapeSearchTool *aContourShapeSearch=
    new ContourShapeSearchTool();

  /* connect sub Search with MultiImageSearch */
  if (aScalableColorSearch)
    m_ScalableColorSearch=aScalableColorSearch->GetInterface();
  if (aContourShapeSearch)
    m_ContourShapeSearch=aContourShapeSearch->GetInterface();

  m_ScalableColorScale=atof(getpara("ScalableColorScale","1000.0"));
  if (m_ScalableColorScale == 0.0) m_ScalableColorScale=1.0;
  m_ScalableColorWeight=atof(getpara("ScalableColorWeight","1.0"));
  m_ContourShapeScale=atof(getpara("ContourShapeScale","1.0"));
  if (m_ContourShapeScale == 0.0) m_ContourShapeScale=1.0;
  m_ContourShapeWeight=atof(getpara("ContourShapeWeight","1.0"));

#ifdef DEBUG
  fprintf(stderr,"ScalableColorScale: %f\n",m_ScalableColorScale);
  fprintf(stderr,"ScalableColorWeight: %f\n",m_ScalableColorWeight);
  fprintf(stderr,"ContourShapeScale: %f\n",m_ContourShapeScale);
  fprintf(stderr,"ContourShapeWeight: %f\n",m_ContourShapeWeight);
#endif

  SetExposedInterface(&m_Interface);
}

//=============================================================================
MultiImageSearchTool::
MultiImageSearchTool(MultiImageDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_ScalableColorScale(0.0),
m_ScalableColorWeight(0.0),
m_ContourShapeScale(0.0),
m_ContourShapeWeight(0.0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    MultiImageDescriptor *descriptor =
      new MultiImageDescriptor(0,0); /* one argument for each sub-component*/
    aQueryDescriptorInterface=descriptor->GetInterface();
  }

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub Search and
     connect them with descriptor if availabe*/
  {
    ScalableColorSearchTool *aScalableColorSearch;

    if (aQueryDescriptorInterface->GetScalableColorDescriptorInterface()) {
      aScalableColorSearch= new
	ScalableColorSearchTool(aQueryDescriptorInterface->
				 GetScalableColorDescriptorInterface());
    }
    else { 
      aScalableColorSearch= new ScalableColorSearchTool();
    }
    if (aScalableColorSearch) {
      m_ScalableColorSearch=aScalableColorSearch->GetInterface();
    }
  }

  {
    ContourShapeSearchTool *aContourShapeSearch;

    if (aQueryDescriptorInterface->GetContourShapeDescriptorInterface()) {
      aContourShapeSearch= new
	ContourShapeSearchTool(aQueryDescriptorInterface->
			       GetContourShapeDescriptorInterface());
    }
    else { 
      aContourShapeSearch= new ContourShapeSearchTool();
    }
    if (aContourShapeSearch) {
      m_ContourShapeSearch=aContourShapeSearch->GetInterface();
    }
  }

  /* connect descritors with CS*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  m_ScalableColorScale=atof(getpara("ScalableColorScale","1000.0"));
  if (m_ScalableColorScale == 0.0) m_ScalableColorScale=1.0;
  m_ScalableColorWeight=atof(getpara("ScalableColorWeight","1.0"));
  m_ContourShapeScale=atof(getpara("ContourShapeScale","1.0"));
  if (m_ContourShapeScale == 0.0) m_ContourShapeScale=1.0;
  m_ContourShapeWeight=atof(getpara("ContourShapeWeight","1.0"));

#ifdef DEBUG
  fprintf(stderr,"ScalableColorScale: %f\n",m_ScalableColorScale);
  fprintf(stderr,"ScalableColorWeight: %f\n",m_ScalableColorWeight);
  fprintf(stderr,"ContourShapeScale: %f\n",m_ContourShapeScale);
  fprintf(stderr,"ContourShapeWeight: %f\n",m_ContourShapeWeight);
#endif

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiImageSearchTool::~MultiImageSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
  if (m_ScalableColorSearch) m_ScalableColorSearch->destroy();
  if (m_ContourShapeSearch) m_ContourShapeSearch->destroy();
}

//----------------------------------------------------------------------------
const UUID& MultiImageSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MultiImageSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MultiImageSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MultiImageSearchTool::
SetRefDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aRefMultiImageDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefMultiImageDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefMultiImageDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();

    /* conect sub descriptors with sub Searchs*/
    if ((m_RefDescriptorInterface->
	 GetScalableColorDescriptorInterface()) &&
	m_ScalableColorSearch) {
      m_ScalableColorSearch->
	SetRefDescriptorInterface(m_RefDescriptorInterface->
			       GetScalableColorDescriptorInterface());
    }
    if ((m_RefDescriptorInterface->
	 GetContourShapeDescriptorInterface()) &&
	m_ContourShapeSearch) {
      m_ContourShapeSearch->
	SetRefDescriptorInterface(m_RefDescriptorInterface->
			       GetContourShapeDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_ScalableColorSearch->SetRefDescriptorInterface(NULL);
    m_ContourShapeSearch->SetRefDescriptorInterface(NULL);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC* MultiImageSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  MultiImageSearchTool::
SetQueryDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aQueryMultiImageDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryMultiImageDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryMultiImageDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();

    /* conect sub descriptors with sub Searchs*/
    if ((m_QueryDescriptorInterface->
	 GetScalableColorDescriptorInterface()) &&
	m_ScalableColorSearch) {
      m_ScalableColorSearch->
	SetQueryDescriptorInterface(m_QueryDescriptorInterface->
				    GetScalableColorDescriptorInterface());
    }
    if ((m_QueryDescriptorInterface->
	 GetContourShapeDescriptorInterface()) &&
	m_ContourShapeSearch) {
      m_ContourShapeSearch->
	SetQueryDescriptorInterface(m_QueryDescriptorInterface->
				    GetContourShapeDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_ScalableColorSearch->SetQueryDescriptorInterface(0);
    m_ContourShapeSearch->SetQueryDescriptorInterface(0);
    return -1;
  }

  return 0;
}

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
//----------------------------------------------------------------------------
ScalableColorSearchInterfaceABC *MultiImageSearch::
GetScalableColorSearchInterface(void)
{
  return m_ScalableColorSearch;
}

//----------------------------------------------------------------------------
int MultiImageSearch::
SetScalableColorSearchInterface(ScalableColorSearchInterfaceABC
			    *aScalableColorSearchInterface)
{
  if (!aScalableColorSearchInterface) return -1;

  m_ScalableColorSearch=aScalableColorSearchInterface;

  return 0;
}

//----------------------------------------------------------------------------
ContourShapeSearchInterfaceABC *MultiImageSearch::
GetContourShapeSearchInterface(void)
{
  return m_ContourShapeSearch;
}

//----------------------------------------------------------------------------
int MultiImageSearch::
SetContourShapeSearchInterface(ContourShapeSearchInterfaceABC
			    *aContourShapeSearchInterface)
{
  if (!aContourShapeSearchInterface) return -1;

  m_ContourShapeSearch=aContourShapeSearchInterface;

  return 0;
}
#endif /* __HasSubTypes*/

//----------------------------------------------------------------------------
double MultiImageSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "MultiImageDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "MultiImageDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
  // For this dummy type all I want is the abs difference between the values
  double dist;
  double chdist=m_ScalableColorSearch->GetDistance();
  double scdist=m_ContourShapeSearch->GetDistance();

  dist=chdist/m_ScalableColorScale*m_ScalableColorWeight +
    scdist/m_ContourShapeScale*m_ContourShapeWeight;

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
MultiImageSearchInterfaceABC *MultiImageSearchTool::GetInterface(void)
{ return &m_Interface; }

