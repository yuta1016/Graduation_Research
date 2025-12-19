////////////////////////////////////////////////////////////////////////
//
// ColorStructureSearch.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID ColorStructureSearchInterface::myID = UUID("");
const char *ColorStructureSearchInterface::myName = "ColorStructureMatchingInterface";

const UUID ColorStructureSearchTool::myID = UUID("");
const char *ColorStructureSearchTool::myName = "ColorStructureMatchingTool";

const UUID ColorStructureSearchInterfaceABC::myID = UUID();

//=============================================================================
ColorStructureSearchInterfaceABC::ColorStructureSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ColorStructureSearchInterface::ColorStructureSearchInterface(ColorStructureSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorStructureSearchInterface::~ColorStructureSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ColorStructureSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ColorStructureSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int ColorStructureSearchInterface::
SetRefDescriptorInterface(ColorStructureDescriptorInterfaceABC
		       *RefColorStructureDescriptorInterface)

{
	return m_SearchTool->
		SetRefDescriptorInterface(RefColorStructureDescriptorInterface);
}

//----------------------------------------------------------------------------
ColorStructureDescriptorInterfaceABC* ColorStructureSearchInterface::
GetQueryDescriptorInterface(void)

{
	return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int ColorStructureSearchInterface::
SetQueryDescriptorInterface(ColorStructureDescriptorInterfaceABC
		       *QueryColorStructureDescriptorInterface)

{
	return m_SearchTool->
		SetQueryDescriptorInterface(QueryColorStructureDescriptorInterface);
}

//----------------------------------------------------------------------------
double ColorStructureSearchInterface::GetDistance(void)

{
	return m_SearchTool->GetDistance();
}

//=============================================================================
ColorStructureSearchTool::ColorStructureSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ColorStructureSearchTool::
ColorStructureSearchTool(ColorStructureDescriptorInterfaceABC
		    *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) 
	{
		ColorStructureDescriptor *descriptor =
			new ColorStructureDescriptor();
		aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with Searchs*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorStructureSearchTool::~ColorStructureSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ColorStructureSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ColorStructureSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ColorStructureSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ColorStructureSearchTool::
SetRefDescriptorInterface(ColorStructureDescriptorInterfaceABC
		       *aRefColorStructureDescriptorInterface)

{
	/* check if new value is different from old value*/
	if (m_RefDescriptorInterface == 
		aRefColorStructureDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
		m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
		aRefColorStructureDescriptorInterface;
	if (m_RefDescriptorInterface) {
		m_RefDescriptorInterface->addref();

	}
	else {
		return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
ColorStructureDescriptorInterfaceABC* ColorStructureSearchTool::
GetQueryDescriptorInterface(void)

{
	return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  ColorStructureSearchTool::
SetQueryDescriptorInterface(ColorStructureDescriptorInterfaceABC
		       *aQueryColorStructureDescriptorInterface)

{
	/* check if new value is different from old value*/
	if (m_QueryDescriptorInterface == 
		aQueryColorStructureDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
		m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
		aQueryColorStructureDescriptorInterface;
	if (m_QueryDescriptorInterface) {
		m_QueryDescriptorInterface->addref();

	}
	else {
		return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
double ColorStructureSearchTool::GetDistance(void)
{
	double distance, L1error = 0.0;

	// ------------------------- Check it's all set up ok ---------------------
	if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
	if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);

	if(strcmp(m_RefDescriptorInterface->GetName(),
		"ColorStructureDescriptionInterface") != 0) return(DBL_MAX);
	if(strcmp(m_QueryDescriptorInterface->GetName(),
		"ColorStructureDescriptionInterface") != 0) return(DBL_MAX);

	if(m_RefDescriptorInterface->GetSize() !=
		m_QueryDescriptorInterface->GetSize())
	{
		std::cerr << "GetDistance called with descriptors of different size" << std::endl;
		return(DBL_MAX);
	}

	// ------------------------- Calculate the distance ------------------------

	unsigned int i, size = m_RefDescriptorInterface->GetSize();
	for (i = 0; i < size; i++)
	{
		distance = m_RefDescriptorInterface->GetElement(i) / 255.0 -
				   m_QueryDescriptorInterface->GetElement(i) / 255.0;

		L1error += fabs(distance);
	}

	return L1error;
}

//----------------------------------------------------------------------------
ColorStructureSearchInterfaceABC *ColorStructureSearchTool::GetInterface(void)
{ return &m_Interface; }

