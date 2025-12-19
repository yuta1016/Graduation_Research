//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
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
// Applicable File Name:  RegionShapeSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID RegionShapeSearchInterface::myID = UUID("");
const char *RegionShapeSearchInterface::myName = "Region Shape Matching Interface";

const UUID RegionShapeSearchTool::myID = UUID("");
const char *RegionShapeSearchTool::myName = "Region Shape Matching Tool";

const UUID RegionShapeSearchInterfaceABC::myID = UUID();

//=============================================================================
RegionShapeSearchInterfaceABC::RegionShapeSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
RegionShapeSearchInterface::RegionShapeSearchInterface(RegionShapeSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RegionShapeSearchInterface::~RegionShapeSearchInterface()
{
}

//----------------------------------------------------------------------------
void RegionShapeSearchInterface::destroy(void)
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int RegionShapeSearchInterface::
SetRefDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *RefRegionShapeDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefRegionShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
int RegionShapeSearchInterface::
SetQueryDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *QueryRegionShapeDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryRegionShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
double RegionShapeSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& RegionShapeSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
RegionShapeSearchTool::RegionShapeSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeSearchTool::
RegionShapeSearchTool(RegionShapeDescriptorInterfaceABC
                       *aQueryDescriptorInterface) :
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif

	if (!aQueryDescriptorInterface) {
	  RegionShapeDescriptor *descriptor = new RegionShapeDescriptor();
	  aQueryDescriptorInterface = descriptor->GetInterface();
	}

	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeSearchTool::~RegionShapeSearchTool()
{
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& RegionShapeSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool RegionShapeSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool RegionShapeSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int RegionShapeSearchTool::
SetRefDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *aRefRegionShapeDescriptorInterface)

{
  if (m_RefDescriptorInterface == aRefRegionShapeDescriptorInterface) 
    return 0;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();

  m_RefDescriptorInterface = aRefRegionShapeDescriptorInterface;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
int  RegionShapeSearchTool::
SetQueryDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *aQueryRegionShapeDescriptorInterface)

{
  if (m_QueryDescriptorInterface == aQueryRegionShapeDescriptorInterface) 
    return 0;

  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

  m_QueryDescriptorInterface = aQueryRegionShapeDescriptorInterface;

  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
double RegionShapeSearchTool::GetDistance(void)
{
    // check for descriptors
    if (!m_RefDescriptorInterface) return DBL_MAX;
    if (!m_QueryDescriptorInterface) return DBL_MAX;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "Region Shape Descriptor Interface") != 0) {
	  return DBL_MAX;
	}
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		   "Region Shape Descriptor Interface") != 0) {
	  return DBL_MAX;
	}

	// perform matching
	double distance = 0;

	for(int i=0 ; i<ART_ANGULAR ; i++)
	for(int j=0 ; j<ART_RADIAL ; j++)
	if(i!=0 || j!= 0)
	{
	  distance += fabs(m_RefDescriptorInterface->GetRealValue(i, j) -
			   m_QueryDescriptorInterface->GetRealValue(i, j));
	}

	return distance;
}

//----------------------------------------------------------------------------
RegionShapeSearchInterfaceABC *RegionShapeSearchTool::GetInterface(void)
{ return &m_Interface; }
