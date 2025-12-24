//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu,Shawn Newsam, Baris Newsam, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// Electrical and Computer Engineering Deptt., University of California Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// Applicable File Name:  TextureBrowsingSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <cstring>

#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/texture_browsing_extraction.h"
#define Td2 255
//=============================================================================
	

using namespace XM;


const UUID TextureBrowsingSearchInterface::myID = UUID("");
const char *TextureBrowsingSearchInterface::myName = "TextureBrowsingMatchingInterface";

const UUID TextureBrowsingSearchTool::myID = UUID("");
const char *TextureBrowsingSearchTool::myName = "TextureBrowsingMatchingTool";

const UUID TextureBrowsingSearchInterfaceABC::myID = UUID();

//=============================================================================
TextureBrowsingSearchInterfaceABC::TextureBrowsingSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
TextureBrowsingSearchInterface::TextureBrowsingSearchInterface(TextureBrowsingSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TextureBrowsingSearchInterface::~TextureBrowsingSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void TextureBrowsingSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int TextureBrowsingSearchInterface::
SetRefDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *RefTextureBrowsingDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefTextureBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC* TextureBrowsingSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int TextureBrowsingSearchInterface::
SetQueryDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *QueryTextureBrowsingDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryTextureBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
double TextureBrowsingSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
TextureBrowsingSearchTool::TextureBrowsingSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
TextureBrowsingSearchTool::
TextureBrowsingSearchTool(TextureBrowsingDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    TextureBrowsingDescriptor *descriptor =
      new TextureBrowsingDescriptor;
    aQueryDescriptorInterface=descriptor->GetInterface();
  }

  /* connect Descritors with Searchs*/
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TextureBrowsingSearchTool::~TextureBrowsingSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool TextureBrowsingSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool TextureBrowsingSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int TextureBrowsingSearchTool::
SetRefDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aRefTextureBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface == 
      aRefTextureBrowsingDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefTextureBrowsingDescriptorInterface;
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC* TextureBrowsingSearchTool::
GetQueryDescriptorInterface(void)

{
  return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  TextureBrowsingSearchTool::
SetQueryDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aQueryTextureBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
      aQueryTextureBrowsingDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryTextureBrowsingDescriptorInterface;
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
double TextureBrowsingSearchTool::GetDistance(void)
{
  // ------------------------- Check it's all set up ok ---------------------
  if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptorInterface->GetName(),
	    "TextureBrowsingDescriptionInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptorInterface->GetName(),
	    "TextureBrowsingDescriptionInterface") != 0) return(DBL_MAX);

  double dist;
  int flag = m_RefDescriptorInterface->GetComponentNumberFlag() * m_QueryDescriptorInterface->GetComponentNumberFlag();
  dist=distance_PBC(flag, m_RefDescriptorInterface->GetBrowsing_Component(),m_QueryDescriptorInterface->GetBrowsing_Component());
 
  return(dist);
}

//----------------------------------------------------------------------------
TextureBrowsingSearchInterfaceABC *TextureBrowsingSearchTool::GetInterface(void)
{ return &m_Interface; }
