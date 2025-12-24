/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada and Eiji Kasutani
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
// Applicable File Name:  ColorLayoutSearch.cpp
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <malloc.h>
#include <time.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#define Td2 255
//=============================================================================
	

using namespace XM;


const UUID ColorLayoutSearchInterface::myID = UUID("");
const char *ColorLayoutSearchInterface::myName = "Color Layout Matching Interface";

const UUID ColorLayoutSearchTool::myID = UUID("");
const char *ColorLayoutSearchTool::myName = "Color Layout Matching Tool";

const UUID ColorLayoutSearchInterfaceABC::myID = UUID();

//=============================================================================
ColorLayoutSearchInterfaceABC::ColorLayoutSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ColorLayoutSearchInterface::ColorLayoutSearchInterface(ColorLayoutSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorLayoutSearchInterface::~ColorLayoutSearchInterface()
{
}

// start modification for version 3.0
void ColorLayoutSearchInterface::destroy(void)
{ delete m_SearchTool; }
// end modification for version 3.0
//----------------------------------------------------------------------------
int ColorLayoutSearchInterface::
SetRefDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *RefColorLayoutDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefColorLayoutDescriptorInterface);
}

//----------------------------------------------------------------------------
// start modification for version 3.0
ColorLayoutDescriptorInterfaceABC* ColorLayoutSearchInterface::
GetQueryDescriptorInterface(void)
{
  return m_SearchTool->GetQueryDescriptorInterface();
}
// end modification for version 3.0

//----------------------------------------------------------------------------
int ColorLayoutSearchInterface::
SetQueryDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *QueryColorLayoutDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryColorLayoutDescriptorInterface);
}

//----------------------------------------------------------------------------
float ColorLayoutSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& ColorLayoutSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutSearchInterface::GetName(void)
{ return myName; }

void ColorLayoutSearchInterface::SetWeightingValues(int weight[3][64])
{
  m_SearchTool->SetWeightingValues(weight);
}



//=============================================================================
ColorLayoutSearchTool::ColorLayoutSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
    SetExposedInterface(&m_Interface);
    // change for WD 2.0
//    m_weight[0][0]=3; m_weight[0][1]=m_weight[0][2]=3;
//    m_weight[1][0]=2; m_weight[1][1]=m_weight[1][2]=2;
//    m_weight[2][0]=4; m_weight[2][1]=m_weight[2][2]=2;
    m_weight[0][0]=2; m_weight[0][1]=m_weight[0][2]=2;
    m_weight[1][0]=2; m_weight[1][1]=m_weight[1][2]=1;
    m_weight[2][0]=4; m_weight[2][1]=m_weight[2][2]=2;
    for(int i=0; i<3; i++){
	for(int j=3; j<64; j++) m_weight[i][j]=1;
    }
}

// start modification for version 3.0
//----------------------------------------------------------------------------
ColorLayoutSearchTool::ColorLayoutSearchTool(ColorLayoutDescriptorInterfaceABC *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
	  ColorLayoutDescriptor *descriptor =
	    new ColorLayoutDescriptor();
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	/* create sub coding schemes and
	   connect them with descriptor if availabe*/
	/* no sub matching tools (see how ColorHistCS if needed)*/

    SetExposedInterface(&m_Interface);
    m_weight[0][0]=3; m_weight[0][1]=m_weight[0][2]=3;
    m_weight[1][0]=2; m_weight[1][1]=m_weight[1][2]=2;
    m_weight[2][0]=4; m_weight[2][1]=m_weight[2][2]=2;
    for(int i=0; i<3; i++){
	for(int j=3; j<64; j++) m_weight[i][j]=1;
    }
}

//----------------------------------------------------------------------------
ColorLayoutSearchTool::~ColorLayoutSearchTool()
{
	/* release descriptor */
	if (m_RefDescriptorInterface) m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface) m_QueryDescriptorInterface->release();
}
// end modification for version 3.0

//----------------------------------------------------------------------------
const UUID& ColorLayoutSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorLayoutSearchTool::GetName(void)
{ return myName; }

// start modification for version 3.0
//----------------------------------------------------------------------------
bool ColorLayoutSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool ColorLayoutSearchTool::SupportsPull(void)
{ return false; }
// end modification for version 3.0

//----------------------------------------------------------------------------
int ColorLayoutSearchTool::
SetRefDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *aRefColorLayoutDescriptorInterface)

{
	// start modification for version 3.0
        /* check if new value is different from old value*/
        if (m_RefDescriptorInterface == 
	    aRefColorLayoutDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_RefDescriptorInterface) 
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface = 
	  aRefColorLayoutDescriptorInterface;
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
//  m_RefDescriptorInterface = aRefColorLayoutDescriptorInterface;
  return 0;
}

// start modification for version 3.0
//----------------------------------------------------------------------------
ColorLayoutDescriptorInterfaceABC* ColorLayoutSearchTool::
GetQueryDescriptorInterface(void)
{
        return m_QueryDescriptorInterface;
}
// end modification for version 3.0

//----------------------------------------------------------------------------
int  ColorLayoutSearchTool::
SetQueryDescriptorInterface(ColorLayoutDescriptorInterfaceABC
		       *aQueryColorLayoutDescriptorInterface)

{
	// start modification for version 3.0
        /* check if new value is different from old value*/
        if (m_QueryDescriptorInterface == 
	    aQueryColorLayoutDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_QueryDescriptorInterface) 
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface = 
	  aQueryColorLayoutDescriptorInterface;
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
	// end modification for version 3.0
//  m_QueryDescriptorInterface=aQueryColorLayoutDescriptorInterface;
  return 0;
}

//----------------------------------------------------------------------------
float ColorLayoutSearchTool::GetDistance()
{
    if (!m_RefDescriptorInterface) return (float)-1;
    if (!m_QueryDescriptorInterface) return (float)-1;
    if (strcmp(m_RefDescriptorInterface->GetName(),
	       "Color Layout Description Interface") != 0) {
	return DBL_MAX;
    }
    if (strcmp(m_QueryDescriptorInterface->GetName(),
	       "Color Layout Description Interface") != 0) {
	return DBL_MAX;
    }
    int NY1, NY2, NC1, NC2, NY, NC;
    NY1=m_RefDescriptorInterface->GetNumberOfYCoeff();
    NY2=m_QueryDescriptorInterface->GetNumberOfYCoeff();
    NC1=m_RefDescriptorInterface->GetNumberOfCCoeff();
    NC2=m_QueryDescriptorInterface->GetNumberOfCCoeff();

    NY=(NY1<NY2)? NY1:NY2;
    NC=(NC1<NC2)? NC1:NC2;

    int *Y1, *Y2, *Cb1, *Cb2, *Cr1, *Cr2;
    Y1=m_RefDescriptorInterface->GetYCoeff();
    Cb1=m_RefDescriptorInterface->GetCbCoeff();
    Cr1=m_RefDescriptorInterface->GetCrCoeff();
    Y2=m_QueryDescriptorInterface->GetYCoeff();
    Cb2=m_QueryDescriptorInterface->GetCbCoeff();
    Cr2=m_QueryDescriptorInterface->GetCrCoeff();

    int j;
    int sum[3], diff;
    sum[0]=0;
    for(j=0; j<NY; j++){
	diff=(Y1[j]-Y2[j]);
	sum[0]+= (m_weight[0][j]*diff*diff);
    }
    sum[1]=0;
    for(j=0; j<NC; j++){
	diff=(Cb1[j]-Cb2[j]);
	sum[1]+= (m_weight[1][j]*diff*diff);
    }
    sum[2]=0;
    for(j=0; j<NC; j++){
	diff=(Cr1[j]-Cr2[j]);
	sum[2]+= (m_weight[2][j]*diff*diff);
    }

    return sqrt(sum[0]*1.0)+sqrt(sum[1]*1.0)+sqrt(sum[2]*1.0);
}

//----------------------------------------------------------------------------
void
ColorLayoutSearchTool::SetWeightingValues(int weight[3][64])
{
    int i, j;
    for(i=0; i<3; i++){
	for(j=0; j<64; j++) m_weight[i][j]=weight[i][j];
    }
}

//----------------------------------------------------------------------------
ColorLayoutSearchInterfaceABC* ColorLayoutSearchTool::GetInterface(void)
{ return &m_Interface; }

