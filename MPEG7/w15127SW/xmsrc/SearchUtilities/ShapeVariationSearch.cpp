//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// Applicable File Name:  ShapeVariationSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#ifndef WIN32
#include <cstring> //LINUX_2011
#endif
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================

using namespace XM;

const UUID ShapeVariationSearchInterface::myID = UUID("");
const char *ShapeVariationSearchInterface::myName = "ShapeVariationMatchingInterface";

const UUID ShapeVariationSearchTool::myID = UUID("");
const char *ShapeVariationSearchTool::myName = "ShapeVariationMatchingTool";

const UUID ShapeVariationSearchInterfaceABC::myID = UUID();

//=============================================================================
ShapeVariationSearchInterfaceABC::ShapeVariationSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ShapeVariationSearchInterface::ShapeVariationSearchInterface(ShapeVariationSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeVariationSearchInterface::~ShapeVariationSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void ShapeVariationSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int ShapeVariationSearchInterface::
SetRefDescriptorInterface(ShapeVariationDescriptorInterfaceABC
						  *RefShapeVariationDescriptorInterface)
						  
{
	return m_SearchTool->
		SetRefDescriptorInterface(RefShapeVariationDescriptorInterface);
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC* ShapeVariationSearchInterface::
GetQueryDescriptorInterface(void)

{
	return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeVariationSearchInterface::
SetQueryDescriptorInterface(ShapeVariationDescriptorInterfaceABC
							*QueryShapeVariationDescriptorInterface)
							
{
	return m_SearchTool->
		SetQueryDescriptorInterface(QueryShapeVariationDescriptorInterface);
}

//----------------------------------------------------------------------------
double ShapeVariationSearchInterface::GetDistance(void)

{
	return m_SearchTool->GetDistance();
}

//=============================================================================
ShapeVariationSearchTool::ShapeVariationSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	
	SetExposedInterface(&m_Interface);
}

//=============================================================================
ShapeVariationSearchTool::
ShapeVariationSearchTool(ShapeVariationDescriptorInterfaceABC
						*aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
		ShapeVariationDescriptor *descriptor =  new ShapeVariationDescriptor;
		aQueryDescriptorInterface=descriptor->GetInterface();
	}
	
	/* connect Descritors with Searchs*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeVariationSearchTool::~ShapeVariationSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
		m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
		m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ShapeVariationSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeVariationSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ShapeVariationSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ShapeVariationSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ShapeVariationSearchTool::
SetRefDescriptorInterface(ShapeVariationDescriptorInterfaceABC
						  *aRefShapeVariationDescriptorInterface)
						  
{
	/* check if new value is different from old value*/
	if (m_RefDescriptorInterface == 
		aRefShapeVariationDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
		m_RefDescriptorInterface->release();
	
	/* add new interface*/
	m_RefDescriptorInterface = 
		aRefShapeVariationDescriptorInterface;
	if (m_RefDescriptorInterface) {
		m_RefDescriptorInterface->addref();
	}
	else {
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
ShapeVariationDescriptorInterfaceABC* ShapeVariationSearchTool::
GetQueryDescriptorInterface(void)

{
	return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  ShapeVariationSearchTool::
SetQueryDescriptorInterface(ShapeVariationDescriptorInterfaceABC
							*aQueryShapeVariationDescriptorInterface)
							
{
	/* check if new value is different from old value*/
	if (m_QueryDescriptorInterface == 
		aQueryShapeVariationDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
		m_QueryDescriptorInterface->release();
	
	/* add new interface*/
	m_QueryDescriptorInterface = 
		aQueryShapeVariationDescriptorInterface;
	if (m_QueryDescriptorInterface) {
		m_QueryDescriptorInterface->addref();
	}
	else {
		return -1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
double ShapeVariationSearchTool::GetDistance(void)
{
	// ------------------------- Check it's all set up ok ---------------------
	if(m_RefDescriptorInterface==NULL)   return(DBL_MAX);
	if(m_QueryDescriptorInterface==NULL) return(DBL_MAX);
	
	if(strcmp(m_RefDescriptorInterface->GetName(),
		"ShapeVariationDescriptorInterface") != 0) return(DBL_MAX);
	if(strcmp(m_QueryDescriptorInterface->GetName(),
		"ShapeVariationDescriptorInterface") != 0) return(DBL_MAX);
	
	// ------------------------- Calculate the distance ------------------------
	// For this dummy type all I want is the abs difference between the values
	//const long ref_val=m_RefDescriptorInterface->GetDummyContents();
	//const long qur_val=m_QueryDescriptorInterface->GetDummyContents();
	
	// perform matching
	double distance = 0;

	for( int p = 0 ; p < ANGULAR ; p++ )
	{
		for( int r = 0 ; r < RADIAL ; r++ )
		{
			if( p != 0 || r != 0)
			{
				// distance of StaticShapeVariation
				distance += fabs(m_RefDescriptorInterface->GetLVMValue(p,r) -
					m_QueryDescriptorInterface->GetLVMValue(p,r));
				// distance of DynamicShapeVariation
				distance += fabs(m_RefDescriptorInterface->GetHVMValue(p,r) -
					m_QueryDescriptorInterface->GetHVMValue(p,r));
				// distance of StatisticalVariation
				distance += fabs(m_RefDescriptorInterface->GetSTDValue(p,r) -
					m_QueryDescriptorInterface->GetSTDValue(p,r));				
			}
		}
	}
	
	// All done
	return(distance);
}

//----------------------------------------------------------------------------
ShapeVariationSearchInterfaceABC *ShapeVariationSearchTool::GetInterface(void)
{ return &m_Interface; }

