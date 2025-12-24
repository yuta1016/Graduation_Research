//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivitySearch.cpp
//

#include <stdio.h>
#include <math.h> // for SLMA
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID MotionActivitySearchInterface::myID = UUID("");
const char *MotionActivitySearchInterface::myName = "Motion Activity Matching Interface";

const UUID MotionActivitySearchTool::myID = UUID("");
const char *MotionActivitySearchTool::myName = "Motion Activity Matching Tool";

const UUID MotionActivitySearchInterfaceABC::myID = UUID();

//=============================================================================
MotionActivitySearchInterfaceABC::MotionActivitySearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MotionActivitySearchInterface::MotionActivitySearchInterface(MotionActivitySearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionActivitySearchTool::
MotionActivitySearchTool(MotionActivityDescriptorInterfaceABC
			 *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  MotionActivityDescriptor *descriptor =
	    new MotionActivityDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	/* no sub matching tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivitySearchInterface::~MotionActivitySearchInterface()
{
}

//----------------------------------------------------------------------------
int MotionActivitySearchInterface::
SetRefDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *RefMotionActivityDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefMotionActivityDescriptorInterface);
}

//----------------------------------------------------------------------------
int MotionActivitySearchInterface::
SetQueryDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *QueryMotionActivityDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryMotionActivityDescriptorInterface);
}

//----------------------------------------------------------------------------
double MotionActivitySearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& MotionActivitySearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivitySearchInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionActivitySearchInterface::destroy(void)
{ delete m_SearchTool; }


//=============================================================================
MotionActivitySearchTool::MotionActivitySearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivitySearchTool::~MotionActivitySearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& MotionActivitySearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivitySearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MotionActivitySearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MotionActivitySearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MotionActivitySearchTool::
SetRefDescriptorInterface(MotionActivityDescriptorInterfaceABC
			  *aRefDescriptorInterface)

{
  
        if (m_RefDescriptorInterface == 
	    aRefDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
	  aRefDescriptorInterface;
	if (m_RefDescriptorInterface) {
	  m_RefDescriptorInterface->addref();

	  /* connect sub descriptors with sub search tools*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub search tools*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
int  MotionActivitySearchTool::
SetQueryDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *aQueryDescriptorInterface)

{
         /* check if new value is different from old value*/
        if (m_QueryDescriptorInterface == 
	    aQueryDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
	  aQueryDescriptorInterface;
	if (m_QueryDescriptorInterface) {
	  m_QueryDescriptorInterface->addref();

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
double MotionActivitySearchTool::GetDistance(void)

{
	// 2013/07/25 KW fixed
	double distance,weight;
	int i;
	int division_block; // for SLMA

        /* check for descriptors*/
        if (!m_RefDescriptorInterface) return 10000000000.0;
        if (!m_QueryDescriptorInterface) return 10000000000.0;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "Motion Activity Descriptor Interface") != 0) {
	  return 10000000000.0;
	}
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		   "Motion Activity Descriptor Interface") != 0) {
	  return 10000000000.0;
	}

	/* perform matching*/
	weight = m_RefDescriptorInterface->GetIntensity();
	if( m_RefDescriptorInterface->GetSpatialDistributionFlag() != 0 )
	{
		for( i=0; i<NUM_Nsr_Nmr_Nlr; i++)
			weight += m_RefDescriptorInterface->
					  GetSpatialParameters(i);
	}
	distance = 0;
	if ( m_RefDescriptorInterface->GetIntensity() != 0 )
	{
		distance = ( weight/m_RefDescriptorInterface->GetIntensity() )
				   * abs(  m_RefDescriptorInterface->GetIntensity() -
						   m_QueryDescriptorInterface->GetIntensity() );
	}
	if( m_RefDescriptorInterface->GetSpatialDistributionFlag() != 0 )
	{
		for( i=0; i<NUM_Nsr_Nmr_Nlr; i++)
		{
			if( m_RefDescriptorInterface->GetSpatialParameters(i) != 0 )
			{
				distance += ( weight/m_RefDescriptorInterface->GetSpatialParameters(i) )
						    * abs( m_RefDescriptorInterface->GetSpatialParameters(i) -
						           m_QueryDescriptorInterface->GetSpatialParameters(i) );
			}
		}
	}
	// Note: if SpatialLocalizedDistribution is present, the previously computed distance is overwritten (two distance methods).
	if(m_RefDescriptorInterface->GetSpatialLocalizedDistributionFlag()!=0) // for SLMA
	{
		distance=0;
#ifndef SPALOCNUMBER
#define SPALOCNUMBER 1
#endif
		division_block=(int)(pow(4.0, (double)(SPALOCNUMBER+1.0)));
		for(i=0;i<division_block;i++)
		{
			distance+=((m_RefDescriptorInterface->GetSpatialLocalizedParameter(i)-m_QueryDescriptorInterface->GetSpatialLocalizedParameter(i))
				      *(m_RefDescriptorInterface->GetSpatialLocalizedParameter(i)-m_QueryDescriptorInterface->GetSpatialLocalizedParameter(i)));
		}
		distance=sqrt(distance);
	}

	return distance;
}

//----------------------------------------------------------------------------
MotionActivitySearchInterfaceABC *MotionActivitySearchTool::GetInterface(void)
{ return &m_Interface; }

