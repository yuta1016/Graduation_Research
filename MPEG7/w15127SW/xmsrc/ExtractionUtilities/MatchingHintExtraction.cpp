/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee : LG-Elite
//
// (contributing organizations names)

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
// Applicable File Name:  MatchingHintExtraction.cpp
//
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Descriptors/VisualDescriptorID.h"

#include <stdio.h>
#include <float.h>

#define MAX_NUM_DESCRIPTORS 26 
//#define COLOR_STRUCTURE		0
//#define DOMINANT_COLOR		1
//#define SCALABLE_COLOR		2
//#define EDGE_HISTOGRAM		3
//#define HOMOGENEOUS_TEXTURE	4
//#define REGION_SHAPE		5
//#define CONTURE_SHAPE		6

//=============================================================================

using namespace XM;


const UUID MatchingHintExtractionInterface::myID = UUID("");
const char *MatchingHintExtractionInterface::myName = "MatchingHintExtractionInterface";

const UUID MatchingHintExtractionTool::myID = UUID("");
const char *MatchingHintExtractionTool::myName = "MatchingHintExtraction";

const UUID MatchingHintExtractionInterfaceABC::myID = UUID();

//=============================================================================
MatchingHintExtractionInterfaceABC::MatchingHintExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MatchingHintExtractionInterface::MatchingHintExtractionInterface(MatchingHintExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MatchingHintExtractionInterface::~MatchingHintExtractionInterface()
{

}

//----------------------------------------------------------------------------
void MatchingHintExtractionInterface::destroy()
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int MatchingHintExtractionInterface::
SetFeedbackDSInterface(GenericDSInterfaceABC
		       *FeedbackGenericDSInterface)

{
  return m_ExtractionTool->
    SetFeedbackDSInterface(FeedbackGenericDSInterface);
}

//----------------------------------------------------------------------------
int MatchingHintExtractionInterface::
SetQueryDSInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)

{
  return m_ExtractionTool->
    SetQueryDSInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double MatchingHintExtractionInterface::StartExtract(void)

{
  return m_ExtractionTool->StartExtract();
}

//----------------------------------------------------------------------------
const UUID& MatchingHintExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
MatchingHintExtractionTool::MatchingHintExtractionTool():
m_Interface(this),
m_FeedbackDSInterface(0),
m_QueryDSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MatchingHintExtractionTool::
MatchingHintExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_FeedbackDSInterface(0),
m_QueryDSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MatchingHintExtractionTool::~MatchingHintExtractionTool()
{
}

//----------------------------------------------------------------------------
bool MatchingHintExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MatchingHintExtractionTool::SupportsPull(void)
{ return false; }


//----------------------------------------------------------------------------
const UUID& MatchingHintExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintExtractionTool::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
int MatchingHintExtractionTool::
SetFeedbackDSInterface(GenericDSInterfaceABC
		       *aFeedbackGenericDSInterface)

{
  m_FeedbackDSInterface = aFeedbackGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  MatchingHintExtractionTool::
SetQueryDSInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  m_QueryDSInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double MatchingHintExtractionTool::StartExtract(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	double distance;

	GenericDS docRef, docQuery;

	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------

    if (!m_FeedbackDSInterface) return DBL_MAX;
    if (!m_QueryDSInterface) return DBL_MAX;

	if (strcmp(m_FeedbackDSInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return DBL_MAX;
	}
	if (strcmp(m_QueryDSInterface->GetName(),
		   "GenericDSInterface") != 0) {
	  return DBL_MAX;
	}

	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------

	// Read DOM documents for query and database items
	GenericDS Querychild = m_QueryDSInterface->GetChild((int)0).
			GetChild((int)0).GetChild((int)0).GetChild((int)0);
	GenericDS Feedbackchild = m_FeedbackDSInterface->GetChild((int)0).
			GetChild((int)0).GetChild((int)0).GetChild((int)0);

	distance = Updating(Querychild, Feedbackchild);

	return distance;
}

//----------------------------------------------------------------------------
double MatchingHintExtractionTool::Updating(GenericDS segDSQuery,
											GenericDS segDSRef)
{
	// ------------------------------------------------------------------------
	// Variable declaration
	// ------------------------------------------------------------------------

	double distance;

	string segTypeRef, segTypeQuery;

	// ------------------------------------------------------------------------
	// Read info from DOM tree for each Still Region
	// ------------------------------------------------------------------------
	GenericDSInterfaceABC *pRefSegInterface = segDSRef.GetInterface();
	GenericDSInterfaceABC *pQuerySegInterface = segDSQuery.GetInterface();
	segTypeRef = pRefSegInterface->GetDSName();
	segTypeQuery = pQuerySegInterface->GetDSName();

	// ------------------------------------------------------------------------
	// Validate elements are segments and of the same type
	// ------------------------------------------------------------------------
	distance = StillRegionsUpdating(segDSQuery, segDSRef);
	
	return distance;
}

//----------------------------------------------------------------------------
double MatchingHintExtractionTool::StillRegionsUpdating(GenericDS DSQuery,
											   GenericDS DSRef)
{
	double distance= 0.0;
	double distanceSet[MAX_NUM_DESCRIPTORS];
	double maxDistanceSet[MAX_NUM_DESCRIPTORS];


	GenericDS MatchingHintDS;

	// (MatchingHint)
	MatchingHintDS = DSQuery.GetDescription("MatchingHint");
	GetParameters(distanceSet, maxDistanceSet);
	
	distance =  UpdatingMatchingHintUsingOneFeedback(distanceSet, maxDistanceSet, MatchingHintDS);

	return distance;
}


//----------------------------------------------------------------------------
void MatchingHintExtractionTool::GetParameters(double distanceSet[], double maxDistanceSet[])
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
double MatchingHintExtractionTool::UpdatingMatchingHintUsingOneFeedback(double distanceSet[],
																		double maxDistanceSet[],
																		GenericDS MatchingHint)
{
	double reliability;
	double distance = 0.0;
	MatchingHint.GetRealAttribute("reliability", reliability);	
	double MatchingHintvalue[MAX_NUM_DESCRIPTORS];
	double tmpMatchingHintvalue[MAX_NUM_DESCRIPTORS];
	string DescriptorName;
	int i;
	
	//--------------------------------------------------------------
	// Initialize MatchingHintvalues
	//--------------------------------------------------------------
	for(i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		MatchingHintvalue[i] = -1.0;
		tmpMatchingHintvalue[i] = -1.0;
		distanceSet[i] = -1.0;
	}
	
	GenericDS HintNode = MatchingHint.GetDescription("Hint");
	//printf("\nOld MatchingHint Values:\n");
	//printf("==================================================\n");

	//Read MatchingHint values
	while(1)
	{
		if ( HintNode.isNull() ) {
				fprintf(stderr, "\nNull Hint value\n");
		} 
		else
		{
			HintNode.GetTextAttribute("xpath", DescriptorName);

			if(!strcmp(DescriptorName.data(), "../../../ColorStructure"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_ColorStructure]);	 
				//printf("MatchingHint for ColorStructure: %f\n", MatchingHintvalue[COLOR_STRUCTURE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../DominantColor"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_DominantColor]);	 
				//printf("MatchingHint for DominantColor: %f\n", MatchingHintvalue[DOMINANT_COLOR]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../ScalableColor"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_ScalableColor]);	 
				//printf("MatchingHint for ScalableColor: %f\n", MatchingHintvalue[SCALABLE_COLOR]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../EdgeHistogram"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_EdgeHistogram]);	 
				//printf("MatchingHint for EdgeHistogram: %f\n", MatchingHintvalue[EDGE_HISTOGRAM]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../Homogeneous_Texture"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_HomogeneousTexture]); 
				//printf("MatchingHint for HomogeneousTexture: %f\n", MatchingHintvalue[HOMOGENEOUS_TEXTURE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../RegionShape"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_RegionShape]);	 
				//printf("MatchingHint for RegionShape: %f\n", MatchingHintvalue[REGION_SHAPE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../ContureShape"))
			{
				HintNode.GetRealAttribute("value", MatchingHintvalue[ID_ContourShape]);	 
				//printf("MatchingHint for ContureShape: %f\n", MatchingHintvalue[CONTURE_SHAPE]);
			}

		}

		GenericDS nextHintNode = HintNode.GetNextSibling();
		if(!nextHintNode.isNull())
			HintNode = nextHintNode;
		else
			break;
	}

	for(i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		if(distanceSet[i] > -1)
			tmpMatchingHintvalue[i] = maxDistanceSet[i] - distanceSet[i];
	}

	double Performance = 1.0;
	double Authority = 1.0;
	double sumHintvalue = 0.0;

	for(i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		if(MatchingHintvalue[i] > -1)
		{
			MatchingHintvalue[i] = 	(reliability*MatchingHintvalue[i] 
               		+ tmpMatchingHintvalue[i]/(tmpMatchingHintvalue[i]+tmpMatchingHintvalue[i])
					*(1.0-reliability)) / (reliability+1.0);

			sumHintvalue += MatchingHintvalue[i];
		}
	}

	//------------------------------------------------------------------------
	// Normalizing MatchingHint values
	//-------------------------------------------------------------------------

	for(i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		if(MatchingHintvalue[i] > -1)
		{
			MatchingHintvalue[i] = MatchingHintvalue[i] / sumHintvalue;
		}
	}
	
	// Update or extract Reliability
	reliability += Authority * Performance;
	reliability = 1.0 /(1+pow(4.0,-1.0*reliability));

	//Set DomTree with new MatchingHint
	HintNode = MatchingHint.GetDescription("Hint");

	while(1)
	{
		if ( HintNode.isNull() ) {
				fprintf(stderr, "\nNull Hint value\n");
		} 
		else
		{
			HintNode.GetTextAttribute("xpath",DescriptorName);

			if(!strcmp(DescriptorName.data(), "../../../ColorStructure"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_ColorStructure]);	 
				//printf("\nMatchingHint for ColorStructure:%f\n", MatchingHintvalue[COLOR_STRUCTURE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../DominantColor"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_DominantColor]);	 
				//printf("\nMatchingHint for DominantColor:%f\n", MatchingHintvalue[DOMINANT_COLOR]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../ScalableColor"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_ScalableColor]);	 
				//printf("\nMatchingHint for ScalableColor:%f\n", MatchingHintvalue[SCALABLE_COLOR]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../EdgeHistogram"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_EdgeHistogram]);	 
				//printf("\nMatchingHint for EdgeHistogram:%f\n", MatchingHintvalue[EDGE_HISTOGRAM]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../Homogeneous_Texture"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_HomogeneousTexture]);	 
				//printf("\nMatchingHint for HomogeneousTexture:%f\n", MatchingHintvalue[HOMOGENEOUS_TEXTURE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../RegionShape"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_RegionShape]);	 
				//printf("\nMatchingHint for RegionShape:%f\n", MatchingHintvalue[REGION_SHAPE]);
			}
			else if(!strcmp(DescriptorName.data(), "../../../ContureShape"))
			{
				HintNode.SetAttribute("value", MatchingHintvalue[ID_ContourShape]);	 
				//printf("\nMatchingHint for ContureShape:%f\n", MatchingHintvalue[CONTURE_SHAPE]);
			}
		}

		GenericDS nextHintNode = HintNode.GetNextSibling();
		if(!nextHintNode.isNull())
			HintNode = nextHintNode;
		else
			break;
	}

	// Set values of Matching Hint to the DOM
	MatchingHint.SetAttribute("reliability", reliability);	

	//printf("\nNew Reliability:%f\n\n",reliability);
	// return distance

	{
	int i;
	for(i = 0;i < MAX_NUM_DESCRIPTORS;i++)
	{
		if(distanceSet[i] > -1)
			distance += distanceSet[i]/maxDistanceSet[i] * MatchingHintvalue[i];
	}
	}

	return distance;
}

//----------------------------------------------------------------------------
MatchingHintExtractionInterfaceABC *MatchingHintExtractionTool::GetInterface(void)
{ return &m_Interface; }
