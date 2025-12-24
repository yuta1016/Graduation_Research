//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID GoFGoPColorSearchInterface::myID = UUID("");
const char *GoFGoPColorSearchInterface::myName = "GoFGoPColorMatchingInterface";

const UUID GoFGoPColorSearchTool::myID = UUID("");
const char *GoFGoPColorSearchTool::myName = "GoFGoPColorMatchingTool";

const UUID GoFGoPColorSearchInterfaceABC::myID = UUID();

//=============================================================================
GoFGoPColorSearchInterfaceABC::GoFGoPColorSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
GoFGoPColorSearchInterface::GoFGoPColorSearchInterface(GoFGoPColorSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GoFGoPColorSearchInterface::~GoFGoPColorSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void GoFGoPColorSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int GoFGoPColorSearchInterface::
SetRefDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *RefGoFGoPColorDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefGoFGoPColorDescriptorInterface);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC* GoFGoPColorSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int GoFGoPColorSearchInterface::
SetQueryDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *QueryGoFGoPColorDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryGoFGoPColorDescriptorInterface);
}

//----------------------------------------------------------------------------
double GoFGoPColorSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
GoFGoPColorSearchTool::GoFGoPColorSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create sub Searchs and
     connect them with descriptor if availabe*/
  ScalableColorSearchTool *aScalableColorSearch=
    new ScalableColorSearchTool();

  /* connect sub Search with GoFGoPColorSearch */
  if (aScalableColorSearch)
    m_ScalableColorSearch=aScalableColorSearch->GetInterface();

  SetExposedInterface(&m_Interface);
}

//=============================================================================
GoFGoPColorSearchTool::
GoFGoPColorSearchTool(GoFGoPColorDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    GoFGoPColorDescriptor *descriptor =
      new GoFGoPColorDescriptor(0); /* one argument for each sub-component*/
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
      //      SetScalableColorSearchInterface(m_ScalableColorSearch);
    }
  }

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GoFGoPColorSearchTool::~GoFGoPColorSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
  if (m_ScalableColorSearch) m_ScalableColorSearch->destroy();
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool GoFGoPColorSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool GoFGoPColorSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int GoFGoPColorSearchTool::
SetRefDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aRefGoFGoPColorDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefGoFGoPColorDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefGoFGoPColorDescriptorInterface;
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
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_ScalableColorSearch->SetRefDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC* GoFGoPColorSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  GoFGoPColorSearchTool::
SetQueryDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aQueryGoFGoPColorDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryGoFGoPColorDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryGoFGoPColorDescriptorInterface;
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
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Searchs*/
    m_ScalableColorSearch->SetQueryDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double GoFGoPColorSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "GoFGoPColorDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "GoFGoPColorDescriptionInterface") != 0) return(DBL_MAX);

  double dist=m_ScalableColorSearch->GetDistance();

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
GoFGoPColorSearchInterfaceABC *GoFGoPColorSearchTool::GetInterface(void)
{ return &m_Interface; }

