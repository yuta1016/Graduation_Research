/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee : LG-Elite
//
// (contributing organizations names)
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
// Copyright (c) 2000
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchingHintSearch.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Descriptors/VisualDescriptorID.h"
#include "Utilities/readparam.h"

#define MAX_NUM_DESCRIPTORS     26
//#define COLOR_STRUCTURE		0
//#define DOMINANT_COLOR		1
//#define SCALABLE_COLOR		2
//#define EDGE_HISTOGRAM		3
//#define HOMOGENEOUS_TEXTURE	4
//#define REGION_SHAPE		5
//#define CONTURE_SHAPE		6

//=============================================================================
	

using namespace XM;


const UUID MatchingHintSearchInterface::myID = UUID("");
const char *MatchingHintSearchInterface::myName = "MatchingHintSearchInterface";

const UUID MatchingHintSearchTool::myID = UUID("");
const char *MatchingHintSearchTool::myName = "MatchingHintSearchTool";

const UUID MatchingHintSearchInterfaceABC::myID = UUID();

//=============================================================================
MatchingHintSearchInterfaceABC::MatchingHintSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MatchingHintSearchInterface::MatchingHintSearchInterface(MatchingHintSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MatchingHintSearchInterface::~MatchingHintSearchInterface()
{
}

//----------------------------------------------------------------------------
void MatchingHintSearchInterface::destroy()
{  delete m_SearchTool; }

//----------------------------------------------------------------------------
int MatchingHintSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *RefGenericDSInterface)
{
  return m_SearchTool->
    SetRefDescriptionInterface(RefGenericDSInterface);
}

//----------------------------------------------------------------------------
int MatchingHintSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)
{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double MatchingHintSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& MatchingHintSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
MatchingHintSearchTool::MatchingHintSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MatchingHintSearchTool::MatchingHintSearchTool(GenericDSInterfaceABC
						     *theParent):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
        SetQueryDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MatchingHintSearchTool::~MatchingHintSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& MatchingHintSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MatchingHintSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MatchingHintSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MatchingHintSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  m_RefDescriptionInterface = aRefGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  MatchingHintSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
  m_QueryDescriptionInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double MatchingHintSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	double distance;

	GenericDS docRef, docQuery;

	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------
    if (!m_RefDescriptionInterface) return DBL_MAX;
    if (!m_QueryDescriptionInterface) return DBL_MAX;

	if (strcmp(m_RefDescriptionInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return DBL_MAX;
	}
	if (strcmp(m_QueryDescriptionInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return DBL_MAX;
	}

	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------


	// Read DOM documents for query Image Node
	GenericDS QueryImageNode = m_QueryDescriptionInterface->GetChild((int)0).
			GetChild((int)0).GetChild((int)0).GetChild((int)0);	
	//GenericDS QueryDOMRoot = Querychild.GetParent();

	GenericDS RefImageNode = m_RefDescriptionInterface->GetChild((int)0).
			GetChild((int)0).GetChild((int)0).GetChild((int)0);
	//GenericDS RefDOMRoot = Refchild.GetParent();

	distance = Matching(QueryImageNode, RefImageNode);

	return distance;
}

//----------------------------------------------------------------------------
double MatchingHintSearchTool::
Matching(GenericDS segDSQuery, GenericDS segDSRef)
{
	// ------------------------------------------------------------------------
	// Variable declaration
	// ------------------------------------------------------------------------
	double distance;
	string segTypeRef, segTypeQuery;
	GenericDS RefDSRoot, QueryDSRoot;
	GenericDSInterfaceABC *pRefSegInterface = segDSRef.GetInterface();
	GenericDSInterfaceABC *pQuerySegInterface = segDSQuery.GetInterface();

	// ------------------------------------------------------------------------
	// Read info from DOM tree for each segment
	// ------------------------------------------------------------------------

	segTypeRef = pRefSegInterface->GetDSName();
	segTypeQuery = pQuerySegInterface->GetDSName();
	
	// ------------------------------------------------------------------------
	// Validate elements are segments and of the same type
	// ------------------------------------------------------------------------
	distance = StillRegionsMatching(segDSQuery,segDSRef);

	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
	

	return distance;

}

//----------------------------------------------------------------------------
MatchingHintSearchInterfaceABC *MatchingHintSearchTool::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
double MatchingHintSearchTool::StillRegionsMatching(GenericDS DSQuery, GenericDS DSRef)
{
	double distance= 0.0;
	double distanceSet[MAX_NUM_DESCRIPTORS];
	double maxDistanceSet[MAX_NUM_DESCRIPTORS];

	//initialize vectors
	{
	int i;
	for(i=0; i < MAX_NUM_DESCRIPTORS-1;i++)
	{
		distanceSet[i] = -1.0;
		maxDistanceSet[i] = -1.0;
	}
	}

	GenericDS MatchingHint;

	// (MatchingHint)
	MatchingHint = DSQuery.GetDescription("MatchingHint");
	GetParameters(distanceSet, maxDistanceSet);
	
	distance =  GetDistanceUsingMatchingHint(distanceSet, maxDistanceSet, MatchingHint);
	return distance;
}

//----------------------------------------------------------------------------
void MatchingHintSearchTool::GetParameters(double distanceSet[], double maxDistanceSet[])
{
	distanceSet[ID_ColorStructure] = (double)(atoi(getpara("ColorStructure","-1")));
	distanceSet[ID_DominantColor] = (double)(atoi(getpara("DominantColor","-1")));
	distanceSet[ID_ScalableColor] = (double)(atoi(getpara("ScalableColor","-1")));
	distanceSet[ID_EdgeHistogram] = (double)(atoi(getpara("EdgeHistogram","-1")));
	distanceSet[ID_HomogeneousTexture] = (double)(atoi(getpara("HomogeneousTexture","-1")));
	distanceSet[ID_RegionShape] = (double)(atoi(getpara("RegionShape","-1")));
	distanceSet[ID_ContourShape] = (double)(atoi(getpara("ContureShape","-1")));	

	maxDistanceSet[ID_ColorStructure] = (double)(atoi(getpara("maxColorStructure","-1")));
	maxDistanceSet[ID_DominantColor] = (double)(atoi(getpara("maxDominantColor","-1")));
	maxDistanceSet[ID_ScalableColor] = (double)(atoi(getpara("maxScalableColor","-1")));
	maxDistanceSet[ID_EdgeHistogram] = (double)(atoi(getpara("maxEdgeHistogram","-1")));
	maxDistanceSet[ID_HomogeneousTexture] = (double)(atoi(getpara("maxHomogeneousTexture","-1")));
	maxDistanceSet[ID_RegionShape] = (double)(atoi(getpara("maxRegionShape","-1")));
	maxDistanceSet[ID_ContourShape] = (double)(atoi(getpara("maxContureShape","-1")));	

}

//----------------------------------------------------------------------------
double MatchingHintSearchTool::GetDistanceUsingMatchingHint(double distanceSet[],
															double maxDistanceSet[],
															GenericDS MatchingHint)
{
	double reliability;
	double distance = 0.0;
	MatchingHint.GetRealAttribute("reliability", reliability);	
	double MatchingHintValue[MAX_NUM_DESCRIPTORS];
	string DescriptorName;
	
	GenericDS HintNode = MatchingHint.GetDescription("Hint");

	while(1)
	{
		if ( HintNode.isNull() ) {
				fprintf(stderr, "\nNull Hint Value\n");
		} 
		else
		{
			HintNode.GetTextAttribute("xpath",DescriptorName);

			if(!strcmp(DescriptorName.data(), "../../../ColorStructure"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_ColorStructure]);	 
				//fprintf(stdout, "\nColorStructure:%f\n", MatchingHintValue[ID_ColorStructure]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../DominantColor"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_DominantColor]);	
				//fprintf(stdout, "\nDominantColor:%f\n", MatchingHintValue[ID_DominantColor]);

			}
			else if(!strcmp(DescriptorName.data(), "../../../ScalableColor"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_ScalableColor]);	 
				//fprintf(stdout, "\nScalableColor:%f\n", MatchingHintValue[ID_ScalableColor]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../EdgeHistogram"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_EdgeHistogram]);	 
				//fprintf(stdout, "\nEdgeHistogram%f\n", MatchingHintValue[ID_EdgeHistogram]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../HomogeneousTexture"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_HomogeneousTexture]); 
				//fprintf(stdout, "\nHomogenousTexture%f\n", MatchingHintValue[ID_HomogeneousTexture]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../RegionShape"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_RegionShape]);	 
				//fprintf(stdout, "\nRegionShape%f\n", MatchingHintValue[ID_RegionShape]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../ContureShape"))
			{
				HintNode.GetRealAttribute("value", MatchingHintValue[ID_ContourShape]);	 
				//fprintf(stdout, "\nContureShape%f\n", MatchingHintValue[ID_ContourShape]);
			}

		}

		GenericDS nextHintNode = HintNode.GetNextSibling();
		if(!nextHintNode.isNull())
			HintNode = nextHintNode;
		else
			break;

	}

	for(int i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		if(distanceSet[i] > -1)
			distance += distanceSet[i]/maxDistanceSet[i] * MatchingHintValue[i];
	}
	//fprintf(stdout, "\n====================================================");
	//fprintf(stdout, "\nNew Distance:%f\n", distance);	
	return distance;	
}

