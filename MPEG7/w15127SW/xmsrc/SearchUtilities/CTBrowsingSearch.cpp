//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
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
// Applicable File Name:  CTBrowsingSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
//#include "SearchUtilities/calctemperature.h"

//=============================================================================
	
extern double distance_PCTBC(int *Ref, int* Query);

using namespace XM;


const UUID CTBrowsingSearchInterface::myID = UUID("");
const char *CTBrowsingSearchInterface::myName = "CTBrowsingMatchingInterface";

const UUID CTBrowsingSearchTool::myID = UUID("");
const char *CTBrowsingSearchTool::myName = "CTBrowsingMatchingTool";

const UUID CTBrowsingSearchInterfaceABC::myID = UUID();

//=============================================================================
CTBrowsingSearchInterfaceABC::CTBrowsingSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
CTBrowsingSearchInterface::CTBrowsingSearchInterface(CTBrowsingSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CTBrowsingSearchInterface::~CTBrowsingSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void CTBrowsingSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int CTBrowsingSearchInterface::
SetRefDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *RefCTBrowsingDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefCTBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC* CTBrowsingSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int CTBrowsingSearchInterface::
SetQueryDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *QueryCTBrowsingDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryCTBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
double CTBrowsingSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
CTBrowsingSearchTool::CTBrowsingSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
CTBrowsingSearchTool::
CTBrowsingSearchTool(CTBrowsingDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    CTBrowsingDescriptor *descriptor =
      new CTBrowsingDescriptor;
    aQueryDescriptorInterface=descriptor->GetInterface();
  }


  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CTBrowsingSearchTool::~CTBrowsingSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool CTBrowsingSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool CTBrowsingSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int CTBrowsingSearchTool::
SetRefDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aRefCTBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefCTBrowsingDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefCTBrowsingDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC* CTBrowsingSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  CTBrowsingSearchTool::
SetQueryDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aQueryCTBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryCTBrowsingDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryCTBrowsingDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double CTBrowsingSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "CTBrowsingDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "CTBrowsingDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
  // For this dummy type all I want is the abs difference between the values
  double dist;
  dist = distance_PCTBC(m_RefDescriptorInterface->GetCTBrowsing_Component(),
	  m_QueryDescriptorInterface->GetCTBrowsing_Component());

  // All done
  return(dist);
}

//----------------------------------------------------------------------------
CTBrowsingSearchInterfaceABC *CTBrowsingSearchTool::GetInterface(void)
{ return &m_Interface; }

