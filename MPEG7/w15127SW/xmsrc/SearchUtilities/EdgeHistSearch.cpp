//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul.
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
// Applicable File Name:  EdgeHistSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <memory.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID EdgeHistogramSearchInterface::myID = UUID("");
const char *EdgeHistogramSearchInterface::myName = "EdgeHistogramMatchingInterface";

const UUID EdgeHistogramSearchTool::myID = UUID("");
const char *EdgeHistogramSearchTool::myName = "EdgeHistogramMatchingTool";

const UUID EdgeHistogramSearchInterfaceABC::myID = UUID();

//=============================================================================
EdgeHistogramSearchInterfaceABC::EdgeHistogramSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
EdgeHistogramSearchInterface::EdgeHistogramSearchInterface(EdgeHistogramSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
EdgeHistogramSearchInterface::~EdgeHistogramSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& EdgeHistogramSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void EdgeHistogramSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int EdgeHistogramSearchInterface::
SetRefDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
		       *RefEdgeHistogramDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefEdgeHistogramDescriptorInterface);
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC* EdgeHistogramSearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int EdgeHistogramSearchInterface::
SetQueryDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
		       *QueryEdgeHistogramDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryEdgeHistogramDescriptorInterface);
}

//----------------------------------------------------------------------------
double EdgeHistogramSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
EdgeHistogramSearchTool::EdgeHistogramSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
EdgeHistogramSearchTool::
EdgeHistogramSearchTool(EdgeHistogramDescriptorInterfaceABC 
				 *aQueryDescriptorInterface):
  m_Interface(this),
  m_RefDescriptorInterface(0),
  m_QueryDescriptorInterface(0)
{
  /* create descriptor if it does not exist*/
  if (!aQueryDescriptorInterface) {
    EdgeHistogramDescriptor *descriptor =
      new EdgeHistogramDescriptor();
    aQueryDescriptorInterface=descriptor->GetInterface();
  }

  /* connect descritor with search tool */
  SetQueryDescriptorInterface(aQueryDescriptorInterface);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
EdgeHistogramSearchTool::~EdgeHistogramSearchTool()
{
  /* release descriptor*/
  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();
  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& EdgeHistogramSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool EdgeHistogramSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool EdgeHistogramSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int EdgeHistogramSearchTool::
SetRefDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
		       *aRefEdgeHistogramDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_RefDescriptorInterface ==
    aRefEdgeHistogramDescriptorInterface) return 0;

  /* release old interface*/
  if (m_RefDescriptorInterface) 
    m_RefDescriptorInterface->release();

  /* add new interface*/
  m_RefDescriptorInterface = 
    aRefEdgeHistogramDescriptorInterface;
  if (m_RefDescriptorInterface) {
    m_RefDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC* EdgeHistogramSearchTool::
GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  EdgeHistogramSearchTool::
SetQueryDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
		       *aQueryEdgeHistogramDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_QueryDescriptorInterface == 
    aQueryEdgeHistogramDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_QueryDescriptorInterface) 
    m_QueryDescriptorInterface->release();

  /* add new interface*/
  m_QueryDescriptorInterface = 
    aQueryEdgeHistogramDescriptorInterface;
  if (m_QueryDescriptorInterface) {
    m_QueryDescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
double EdgeHistogramSearchTool::GetDistance()
{
  // ------------------------- Check it's all set up ok ---------------------
  if (m_RefDescriptorInterface==NULL) return(DBL_MAX);
  if (m_QueryDescriptorInterface==NULL) return(DBL_MAX);

  if (strcmp(m_RefDescriptorInterface->GetName(),
             "EdgeHistogramDescriptionInterface") != 0)	return(DBL_MAX);
  if (strcmp(m_QueryDescriptorInterface->GetName(),
             "EdgeHistogramDescriptionInterface") != 0) return(DBL_MAX);

  // ------------------------- Calculate the distance ------------------------
 
	EHD	*pLocal_EdgeHist_Ref;
	EHD	*pLocal_EdgeHist_Query;
	double	Total_EdgeHist_Ref[150];
	// Local(80)+ Global(5)+Semi_Global(65) 
	double	Total_EdgeHist_Query[150];

	int	i;
  	double	dist;

	pLocal_EdgeHist_Ref=m_RefDescriptorInterface->GetEdgeHistogram();
	pLocal_EdgeHist_Query=m_QueryDescriptorInterface->GetEdgeHistogram();
	
	Make_Global_SemiGlobal(pLocal_EdgeHist_Ref->Local_Edge,
			       Total_EdgeHist_Ref);
	Make_Global_SemiGlobal(pLocal_EdgeHist_Query->Local_Edge,
			       Total_EdgeHist_Query);
	dist = 0.0;
	for( i=0; i < 80+70; i++){
	  // Global(5)+Semi_Global(65) 
	  dist += (fabs((Total_EdgeHist_Ref[i] - Total_EdgeHist_Query[i])));
	}
	return 	dist;
}

//----------------------------------------------------------------------------
void EdgeHistogramSearchTool::Make_Global_SemiGlobal(double *LocalHistogramOnly, double *TotalHistogram)
{
	int i, j;	

	memcpy(TotalHistogram+5, LocalHistogramOnly, 80*sizeof(double) );
	
// Make Global Histogram Start
	for(i=0; i<5; i++)
	  TotalHistogram[i]=0.0;
	for( j=0; j < 80; j+=5) {
	  for( i=0; i < 5; i++) {
	    TotalHistogram[i] += TotalHistogram[5+i+j]; 
	  }
	}  // for( j ) 
	for(i=0; i<5; i++)
// Global *5.
		TotalHistogram[i] = TotalHistogram[i]*5/16.0;

// Make Global Histogram end


// Make Semi-Global Histogram start
	for(i=85; i <105; i++) {
		j = i-85;
		TotalHistogram[i] =
			(TotalHistogram[5+j]
			+TotalHistogram[5+20+j]
			+TotalHistogram[5+40+j]
			+TotalHistogram[5+60+j])/4.0;
	}
	for(i=105; i < 125; i++) {
		j = i-105;
		TotalHistogram[i] =
			(TotalHistogram[5+20*(j/5)+j%5]
			+TotalHistogram[5+20*(j/5)+j%5+5]
			+TotalHistogram[5+20*(j/5)+j%5+10]
			+TotalHistogram[5+20*(j/5)+j%5+15])/4.0;
	}
///////////////////////////////////////////////////////
//				4 area Semi-Global
///////////////////////////////////////////////////////
//  Upper area 2.
	for(i=125; i < 135; i++) {
		j = i-125;    // j = 0 ~ 9
		TotalHistogram[i] =
			(TotalHistogram[5+10*(j/5)+0+j%5]
				   +TotalHistogram[5+10*(j/5)+5+j%5]
			       +TotalHistogram[5+10*(j/5)+20+j%5]
			       +TotalHistogram[5+10*(j/5)+25+j%5])/4.0;
	}
//  Down area 2.
	for(i=135; i < 145; i++) {
		j = i-135;    // j = 0 ~ 9
		TotalHistogram[i] =
			(TotalHistogram[5+10*(j/5)+40+j%5]
			       +TotalHistogram[5+10*(j/5)+45+j%5]
			       +TotalHistogram[5+10*(j/5)+60+j%5]
			       +TotalHistogram[5+10*(j/5)+65+j%5])/4.0;
	}
// Center Area 1 
	for(i=145; i < 150; i++) {
		j = i-145;    // j = 0 ~ 9
		TotalHistogram[i] =
			(TotalHistogram[5+25+j%5]
			       +TotalHistogram[5+30+j%5]
			       +TotalHistogram[5+45+j%5]
			       +TotalHistogram[5+50+j%5])/4.0;
	}
// Make Semi-Global Histogram end
	
}

//----------------------------------------------------------------------------
EdgeHistogramSearchInterfaceABC *EdgeHistogramSearchTool::GetInterface(void)
{ return &m_Interface; }

