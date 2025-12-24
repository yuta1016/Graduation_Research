///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation SpaceFrequencyView.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation SpaceFrequencyView tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpaceFrequencyViewExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

//=============================================================================

//#include <strstrea.h>
#include <strstream>

#ifdef _VISUALC_
#define STREAMNS(stream) stream
#else
#define STREAMNS(stream) std::stream
#endif

using namespace XM;
using namespace std;

//=============================================================================
// Static vars and local classes for handling SpaceFrequencyView DSs
//=============================================================================

static const char* SFVIEWCODE[] = {
	"SFV_V",
	"SFV_SV",
	"SFV_FV",
	"SFV_SFV",
	"SFV_RV",
	"SFV_SRV",
	"SFV_VS",
	"SFV_ST_SV",
	"SFV_FT_FV",
	"SFV_SFG_SFV",
	"SFV_VVG_FV",
	"SFV_MRP_RV"
};

//=============================================================================


const UUID SpaceFrequencyViewExtractionInterface::myID = UUID("");
const char *SpaceFrequencyViewExtractionInterface::myName = "SpaceFrequencyViewDSExtractionInterface";

const UUID SpaceFrequencyViewExtractionTool::myID = UUID("");
const char *SpaceFrequencyViewExtractionTool::myName = "SpaceFrequencyViewExtraction";

const UUID SpaceFrequencyViewExtractionInterfaceABC::myID = UUID();


//=============================================================================
SpaceFrequencyViewExtractionInterfaceABC::SpaceFrequencyViewExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SpaceFrequencyViewExtractionInterface::SpaceFrequencyViewExtractionInterface(SpaceFrequencyViewExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpaceFrequencyViewExtractionInterface::~SpaceFrequencyViewExtractionInterface()
{
}

//----------------------------------------------------------------------------
void SpaceFrequencyViewExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//----------------------------------------------------------------------------
int SpaceFrequencyViewExtractionInterface::
SetViewParms(const char* imname, const char* ext, int depth, SFVIEWDECOMPTYPES viewtype)
{
  return m_ExtractionTool->SetViewParms(imname, ext, depth, viewtype);
}


//----------------------------------------------------------------------------
int SpaceFrequencyViewExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& SpaceFrequencyViewExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpaceFrequencyViewExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
SpaceFrequencyViewExtractionTool::SpaceFrequencyViewExtractionTool():
m_Interface(this),
m_DescriptionInterface(0),
m_Depth(0),
m_ViewType(SFV_VIEW),
m_ImName(0),
m_Ext(0),
m_ViewCount(0)
{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
SpaceFrequencyViewExtractionTool::SpaceFrequencyViewExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(theParent),
m_Depth(0),
m_ViewType(SFV_VIEW),
m_ImName(0),
m_Ext(0),
m_ViewCount(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpaceFrequencyViewExtractionTool::~SpaceFrequencyViewExtractionTool()
{

	delete [] m_ImName;
	delete [] m_Ext;
}

//----------------------------------------------------------------------------
bool SpaceFrequencyViewExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SpaceFrequencyViewExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------

int SpaceFrequencyViewExtractionTool::SetViewParms(const char* imname, const char* ext, 
												   int depth, SFVIEWDECOMPTYPES viewtype) 
{
	m_Depth = depth;
	m_ViewType = viewtype;
	
	delete [] m_ImName;
	m_ImName = 0;
	if (imname) {
		m_ImName = new char[strlen(imname)+1];
		strcpy(m_ImName, imname);
	}

	delete [] m_Ext;
	m_Ext = 0;
	if (ext) {
		m_Ext = new char[strlen(ext)+1];
		strcpy(m_Ext, ext);
	}

	return(0);
}

//----------------------------------------------------------------------------
int SpaceFrequencyViewExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;

  return 0;
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionTool::InitExtracting(void)
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionTool::StartExtracting(void)
{

	ExtractSpaceFrequencyViews( 1, m_DescriptionInterface);

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewExtractionTool::PostExtraction(void)
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& SpaceFrequencyViewExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpaceFrequencyViewExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpaceFrequencyViewExtractionInterfaceABC *SpaceFrequencyViewExtractionTool::GetInterface(void)
{ return &m_Interface; }

/*
		SFV_VIEW,			// <--- Abstract
		SFV_SPACEVIEW,
		SFV_FREQUENCYVIEW,
		SFV_SPACEFREQUENCYVIEW,
		SFV_RESOLUTIONVIEW,
		SFV_SPACERESOLUTIONVIEW,
		SFV_VIEWSET,
		SFV_SPACETREE,
		SFV_FREQUENCYTREE,
		SFV_SPACEFREQUENCYGRAPH,
		SFV_VIDEOVIEWGRAPH,
		SFV_MULTIRESOLUTIONPYRAMID
*/


// ------------------------------------------------------------------------
// Extract SpaceFrequencyViews 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceFrequencyViews(int iSpaceFrequencyViewID, 
										GenericDSInterfaceABC *parentInterface) {

	switch (m_ViewType) {

	case SFV_VIEW: 
		cerr << "Cannot create abstract View" << endl;
		return parentInterface;
		break;

	case SFV_SPACETREE:
		return ExtractSpaceTree(iSpaceFrequencyViewID, parentInterface, 0, 0, m_Depth, 0, 0, 1, 1);
		break;

	case SFV_FREQUENCYTREE:
		return ExtractFrequencyTree(iSpaceFrequencyViewID, parentInterface, 0, 0, m_Depth, 0, 0, 1, 1);
		break;

	case SFV_SPACEFREQUENCYGRAPH:
		return ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, parentInterface, 0, 0, m_Depth, 
			0, 0, 1, 1, 0, 0, 1, 1);
		break;

	case SFV_VIDEOVIEWGRAPH:
		return ExtractVideoViewGraph(iSpaceFrequencyViewID, parentInterface, 
			0, 0, m_Depth, 0, 0, 0, 1, 1, 1);
		break;

	case SFV_MULTIRESOLUTIONPYRAMID:
		return ExtractMultiResolutionPyramid(iSpaceFrequencyViewID, parentInterface, 
			0, 0, m_Depth, 1);
		break;
	}

	return parentInterface;

}


// ------------------------------------------------------------------------
// Extract Partition 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractPartition(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h) {

	// ------------------------------------------------------------------------
	// Create Partition 
	// ------------------------------------------------------------------------

	/*
			<Partition>
				<Origin xOrigin="left" yOrigin="bottom" />
				<Start xsi:type="SignalPlaneFractionType" x="0" y="0" />
				<End xsi:type="SignalPlaneFractionType" x="0.5" y="1" />
			</Partition>
	*/
	
	GenericDS Partition;
	if (name && strlen(name)>1) Partition = parentInterface->CreateChild(name);
	else Partition = parentInterface->CreateChild("Partition");
	GenericDSInterfaceABC *PartitionInterface = Partition.GetInterface();

	GenericDS Origin = PartitionInterface->CreateChild("Origin");
	GenericDSInterfaceABC *OriginInterface = Origin.GetInterface();
	OriginInterface->SetAttribute("xOrigin", "left");
	OriginInterface->SetAttribute("yOrigin", "top");
	
	GenericDS Start = PartitionInterface->CreateChild("Start");
	GenericDSInterfaceABC *StartInterface = Start.GetInterface();
	StartInterface->SetAttribute("xsi:type", "SignalPlaneFractionType");  // <-- JRS: should be xsi:type
	StartInterface->SetAttribute("x", x);
	StartInterface->SetAttribute("y", y);

	GenericDS End = PartitionInterface->CreateChild("End");
	GenericDSInterfaceABC *EndInterface = End.GetInterface();
	EndInterface->SetAttribute("xsi:type", "SignalPlaneFractionType"); // <-- JRS: should be xsi:type
	EndInterface->SetAttribute("x", w);
	EndInterface->SetAttribute("y", h);

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


// ------------------------------------------------------------------------
// Extract Partition 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractPartition(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float t,
									float w, float h, float d) {

	// ------------------------------------------------------------------------
	// Create Partition 
	// ------------------------------------------------------------------------
	
	GenericDS Partition;
	if (name && strlen(name)>1) Partition = parentInterface->CreateChild(name);
	else Partition = parentInterface->CreateChild("Partition");
	GenericDSInterfaceABC *PartitionInterface = Partition.GetInterface();

	GenericDS Origin = PartitionInterface->CreateChild("Origin");
	GenericDSInterfaceABC *OriginInterface = Origin.GetInterface();
	OriginInterface->SetAttribute("xOrigin", "left");
	OriginInterface->SetAttribute("yOrigin", "top");
	OriginInterface->SetAttribute("tOrigin", "past");

	GenericDS Start = PartitionInterface->CreateChild("Start");
	GenericDSInterfaceABC *StartInterface = Start.GetInterface();
	StartInterface->SetAttribute("xsi:type", "SignalPlaneFractionType");  // <-- JRS: should be xsi:type
	StartInterface->SetAttribute("x", x);
	StartInterface->SetAttribute("y", y);
	StartInterface->SetAttribute("t", t);

	GenericDS End = PartitionInterface->CreateChild("End");
	GenericDSInterfaceABC *EndInterface = End.GetInterface();
	EndInterface->SetAttribute("xsi:type", "SignalPlaneFractionType"); // <-- JRS: should be xsi:type
	EndInterface->SetAttribute("x", w);
	EndInterface->SetAttribute("y", h);
	EndInterface->SetAttribute("t", d);

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


// ------------------------------------------------------------------------
// Extract View 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractView(int iSpaceFrequencyViewID, 
											  GenericDSInterfaceABC *parentInterface) {

	// ------------------------------------------------------------------------
	// Create View 
	// ------------------------------------------------------------------------
	

	char buf[256];
	sprintf(buf, "%s-%s-%d.%s", m_ImName, SFVIEWCODE[m_ViewType], m_ViewCount++, m_Ext);

	// ------------------------------------------------------------------------
	// Create View Target
	// ------------------------------------------------------------------------
	
	GenericDS Target = parentInterface->CreateChild("Target");
	GenericDSInterfaceABC *TargetInterface = Target.GetInterface();

	GenericDS TargetMedia;
	if (m_ViewType==10) TargetMedia = TargetInterface->CreateChild("Video");
	else TargetMedia = TargetInterface->CreateChild("Image");
	GenericDSInterfaceABC *TargetMediaInterface = TargetMedia.GetInterface();

	GenericDS TargetMediaLocator = TargetMediaInterface->CreateChild("MediaLocator");
	GenericDSInterfaceABC *TargetMediaLocatorInterface = TargetMediaLocator.GetInterface();
	
	GenericDS TargetUri = TargetMediaLocatorInterface->CreateChild("MediaUri");
	GenericDSInterfaceABC *TargetUriInterface = TargetUri.GetInterface();
	TargetUriInterface->SetValue(buf);

	// ------------------------------------------------------------------------
	// Create View Source
	// ------------------------------------------------------------------------
	
	GenericDS Source = parentInterface->CreateChild("Source");
	GenericDSInterfaceABC *SourceInterface = Source.GetInterface();

	GenericDS SourceMedia;
	if (m_ViewType==10) SourceMedia = SourceInterface->CreateChild("Video");
	else SourceMedia = SourceInterface->CreateChild("Image");
	GenericDSInterfaceABC *SourceMediaInterface = SourceMedia.GetInterface();

	GenericDS SourceMediaLocator = SourceMediaInterface->CreateChild("MediaLocator");
	GenericDSInterfaceABC *SourceMediaLocatorInterface = SourceMediaLocator.GetInterface();

	GenericDS SourceUri = SourceMediaLocatorInterface->CreateChild("MediaUri");
	GenericDSInterfaceABC *SourceUriInterface = SourceUri.GetInterface();
	sprintf(buf, "%s.%s", m_ImName, m_Ext);
	SourceUriInterface->SetValue(buf);

	return parentInterface;
} 


// ------------------------------------------------------------------------
// Extract SpaceView 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h) {

	// ------------------------------------------------------------------------
	// Create SpaceView 
	// ------------------------------------------------------------------------
	
	GenericDS SpaceView;
	if (name && strlen(name)>1) SpaceView = parentInterface->CreateChild(name);
	else SpaceView = parentInterface->CreateChild("SpaceView");
	GenericDSInterfaceABC *SpaceViewInterface = SpaceView.GetInterface();
	
	ExtractView(iSpaceFrequencyViewID, SpaceViewInterface);

	return ExtractPartition(iSpaceFrequencyViewID, SpaceViewInterface, "SpacePartition", x, y, w, h);

} 


// ------------------------------------------------------------------------
// Extract FrequencyView 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h) {

	// ------------------------------------------------------------------------
	// Create FrequencyView 
	// ------------------------------------------------------------------------
	
	GenericDS FrequencyView;
	if (name && strlen(name)>1) FrequencyView = parentInterface->CreateChild(name);
	else FrequencyView = parentInterface->CreateChild("FrequencyView");
	GenericDSInterfaceABC *FrequencyViewInterface = FrequencyView.GetInterface();

	ExtractView(iSpaceFrequencyViewID, FrequencyViewInterface);

	return ExtractPartition(iSpaceFrequencyViewID, FrequencyViewInterface, "FrequencyPartition", x, y, w, h);

} 


// ------------------------------------------------------------------------
// Extract FrequencyView
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float t,
									float w, float h, float d) {

	// ------------------------------------------------------------------------
	// Create FrequencyView 
	// ------------------------------------------------------------------------
	
	GenericDS FrequencyView;
	if (name && strlen(name)>1) FrequencyView = parentInterface->CreateChild(name);
	else FrequencyView = parentInterface->CreateChild("FrequencyView");
	GenericDSInterfaceABC *FrequencyViewInterface = FrequencyView.GetInterface();

	ExtractView(iSpaceFrequencyViewID, FrequencyViewInterface);

	return ExtractPartition(iSpaceFrequencyViewID, FrequencyViewInterface, 
		"FrequencyPartition", x, y, t, w, h, d);

} 


// ------------------------------------------------------------------------
// Extract ResolutionView
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractResolutionView(int iSpaceFrequencyViewID, 
														GenericDSInterfaceABC *parentInterface,
														const char* name, float r) {

	// ------------------------------------------------------------------------
	// Create ResolutionView 
	// ------------------------------------------------------------------------
	
	GenericDS ResolutionView;
	if (name && strlen(name)>1) ResolutionView = parentInterface->CreateChild(name);
	else ResolutionView = parentInterface->CreateChild("ResolutionView");
	GenericDSInterfaceABC *ResolutionViewInterface = ResolutionView.GetInterface();

	ExtractView(iSpaceFrequencyViewID, ResolutionViewInterface);

	GenericDS Resolution = ResolutionViewInterface->CreateChild("Resolution");
	GenericDSInterfaceABC *ResolutionInterface = Resolution.GetInterface();

	STREAMNS(strstream) rstr;
	rstr << r << " " << r << '\0';
	ResolutionInterface->SetValue(rstr.str());
	delete [] rstr.str();
	ResolutionInterface->SetAttribute("size", 2);

	return ExtractPartition(iSpaceFrequencyViewID, ResolutionViewInterface, "FrequencyPartition", 0, 0, r, r);

}

// ------------------------------------------------------------------------
// Extract SpaceResolutionView
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceResolutionView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h, float r) {

	
	// ------------------------------------------------------------------------
	// Create SpaceResolutionView 
	// ------------------------------------------------------------------------
	
	GenericDS SpaceResolutionView;
	if (name && strlen(name)>1) SpaceResolutionView = parentInterface->CreateChild(name);
	else SpaceResolutionView = parentInterface->CreateChild("SpaceResolutionView");
	GenericDSInterfaceABC *SpaceResolutionViewInterface = SpaceResolutionView.GetInterface();

	ExtractView(iSpaceFrequencyViewID, SpaceResolutionViewInterface);

	ExtractPartition(iSpaceFrequencyViewID, SpaceResolutionViewInterface, "SpacePartition", x, y, w, h);

	GenericDS Resolution = SpaceResolutionViewInterface->CreateChild("Resolution");
	GenericDSInterfaceABC *ResolutionInterface = Resolution.GetInterface();

	STREAMNS(strstream) rstr;
	rstr << r << " " << r << '\0';
	ResolutionInterface->SetValue(rstr.str());
	delete [] rstr.str();
	ResolutionInterface->SetAttribute("size", 2);

	return ExtractPartition(iSpaceFrequencyViewID, SpaceResolutionViewInterface, "FrequencyPartition", 0, 0, r, r);

}


// ------------------------------------------------------------------------
// Extract SpaceFrequencyView
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name,
									float sx, float sy, float sw, float sh,
									float fx, float fy, float fw, float fh) {

	// ------------------------------------------------------------------------
	// Create SpaceFrequencyView 
	// ------------------------------------------------------------------------
	
	GenericDS SpaceFrequencyView;
	if (name && strlen(name)>1) SpaceFrequencyView = parentInterface->CreateChild(name);
	else SpaceFrequencyView = parentInterface->CreateChild("SpaceFrequencyView");
	GenericDSInterfaceABC *SpaceFrequencyViewInterface = SpaceFrequencyView.GetInterface();

	ExtractView(iSpaceFrequencyViewID, SpaceFrequencyViewInterface);

	ExtractPartition(iSpaceFrequencyViewID, SpaceFrequencyViewInterface, "SpacePartition", sx, sy, sw, sh);
	ExtractPartition(iSpaceFrequencyViewID, SpaceFrequencyViewInterface, "FrequencyPartition", fx, fy, fw, fh);

	return parentInterface;
} 


// ------------------------------------------------------------------------
// Extract SpaceTree
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceTree(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float x, float y, float w, float h) {

	// ------------------------------------------------------------------------
	// Create SpaceTree 
	// ------------------------------------------------------------------------
	
	GenericDS SpaceTree;
	if (name && strlen(name)>1) SpaceTree = parentInterface->CreateChild(name);
	else SpaceTree = parentInterface->CreateChild("SpaceTree");
	GenericDSInterfaceABC *SpaceTreeInterface = SpaceTree.GetInterface();

	ExtractSpaceView(iSpaceFrequencyViewID, SpaceTreeInterface, "View", x, y, w, h);

	if (atLevel<maxDepth) {
		ExtractSpaceTree(iSpaceFrequencyViewID, SpaceTreeInterface, "Child", 
			atLevel+1, maxDepth, x, y, w/2, h/2);
		ExtractSpaceTree(iSpaceFrequencyViewID, SpaceTreeInterface, "Child", 
			atLevel+1, maxDepth, x+w/2, y, w/2, h/2);
		ExtractSpaceTree(iSpaceFrequencyViewID, SpaceTreeInterface, "Child", 
			atLevel+1, maxDepth, x, y+h/2, w/2, h/2);
		ExtractSpaceTree(iSpaceFrequencyViewID, SpaceTreeInterface, "Child", 
			atLevel+1, maxDepth, x+w/2, y+h/2, w/2, h/2);

		SpaceTreeInterface->SetAttribute("complete", "true");
		SpaceTreeInterface->SetAttribute("branching", 4);
		SpaceTreeInterface->SetAttribute("nonRedundant", "true");
	}
		
	else {
		SpaceTreeInterface->SetAttribute("complete", "false");
		SpaceTreeInterface->SetAttribute("branching", 0);
		SpaceTreeInterface->SetAttribute("nonRedundant", "true");
	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


// ------------------------------------------------------------------------
// Extract FrequencyTree
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractFrequencyTree(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float x, float y, float w, float h) {

	// ------------------------------------------------------------------------
	// Create FrequencyTree 
	// ------------------------------------------------------------------------
	
	GenericDS FrequencyTree;
	if (name && strlen(name)>1) FrequencyTree = parentInterface->CreateChild(name);
	else FrequencyTree = parentInterface->CreateChild("FrequencyTree");
	GenericDSInterfaceABC *FrequencyTreeInterface = FrequencyTree.GetInterface();

	ExtractFrequencyView(iSpaceFrequencyViewID, FrequencyTreeInterface, "View", x, y, w, h);

	if (atLevel<maxDepth) {
		ExtractFrequencyTree(iSpaceFrequencyViewID, FrequencyTreeInterface, "Child", 
			atLevel+1, maxDepth, x, y, w/2, h/2);
		ExtractFrequencyTree(iSpaceFrequencyViewID, FrequencyTreeInterface, "Child", 
			atLevel+1, maxDepth, x+w/2, y, w/2, h/2);
		ExtractFrequencyTree(iSpaceFrequencyViewID, FrequencyTreeInterface, "Child", 
			atLevel+1, maxDepth, x, y+h/2, w/2, h/2);
		ExtractFrequencyTree(iSpaceFrequencyViewID, FrequencyTreeInterface, "Child", 
			atLevel+1, maxDepth, x+w/2, y+h/2, w/2, h/2);

		FrequencyTreeInterface->SetAttribute("complete", "true");
		FrequencyTreeInterface->SetAttribute("branching", 4);
		FrequencyTreeInterface->SetAttribute("nonRedundant", "true");
	}
		
	else {
		FrequencyTreeInterface->SetAttribute("complete", "false");
		FrequencyTreeInterface->SetAttribute("branching", 0);
		FrequencyTreeInterface->SetAttribute("nonRedundant", "true");
	}


	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}

/*
	<!-- #####################################################  -->
		<!-- ###         SpaceFrequencyGraph Description       ###  -->
		<!-- #####################################################  -->
		<SpaceFrequencyGraphType complete="false" branching="2" nonRedundant="true">
			<Source>
				<Image>
					<MediaLocator>
						<MediaUri>soccer.jpg</MediaUri>
					</MediaLocator>
				</Image>
			</Source>
			<SpaceChild branching="0">
				<View>
					<Target>
						<Image>
							<MediaLocator>
								<MediaUri>soccer-0.jpg</MediaUri>
							</MediaLocator>
						</Image>
					</Target>
					<SpacePartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="0.5" y="1" xsi:type="SignalPlaneFractionType"/>
					</SpacePartition>
					<FrequencyPartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="1" y="1" xsi:type="SignalPlaneFractionType"/>
					</FrequencyPartition>
				</View>
			</SpaceChild>
			<SpaceChild branching="0">
				<View>
					<Target>
						<Image>
							<MediaLocator>
								<MediaUri>soccer-1.jpg</MediaUri>
							</MediaLocator>
						</Image>
					</Target>
					<SpacePartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="1" y="0.5" xsi:type="SignalPlaneFractionType"/>
					</SpacePartition>
					<FrequencyPartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="1" y="1" xsi:type="SignalPlaneFractionType"/>
					</FrequencyPartition>
				</View>
			</SpaceChild>
			<FrequencyChild branching="0">
				<View>
					<Target>
						<Image>
							<MediaLocator>
								<MediaUri>soccer-ll.jpg</MediaUri>
							</MediaLocator>
						</Image>
					</Target>
					<SpacePartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="1" y="1" xsi:type="SignalPlaneFractionType"/>
					</SpacePartition>
					<FrequencyPartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="0.5" y="0.5" xsi:type="SignalPlaneFractionType"/>
					</FrequencyPartition>
				</View>
			</FrequencyChild>
			<FrequencyChild branching="0">
				<View>
					<Target>
						<Image>
							<MediaLocator>
								<MediaUri>soccer-lh.jpg</MediaUri>
							</MediaLocator>
						</Image>
					</Target>
					<SpacePartition>
						<Start x="0" y="0" xsi:type="SignalPlaneFractionType"/>
						<End x="1" y="1" xsi:type="SignalPlaneFractionType"/>
					</SpacePartition>
					<FrequencyPartition>
						<Start x="0" y="0.5" xsi:type="SignalPlaneFractionType"/>
						<End x="0.5" y="1" xsi:type="SignalPlaneFractionType"/>
					</FrequencyPartition>
				</View>
			</FrequencyChild>
		</SpaceFrequencyGraphType>

*/
// ------------------------------------------------------------------------
// Extract SpaceFrequencyGraph
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractSpaceFrequencyGraph(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float sx, float sy, float sw, float sh,
						  float fx, float fy, float fw, float fh) {

	// ------------------------------------------------------------------------
	// Create SpaceFrequencyGraph 
	// ------------------------------------------------------------------------
	
	GenericDS SpaceFrequencyGraph;
	if (name && strlen(name)>1) SpaceFrequencyGraph = parentInterface->CreateChild(name);
	else SpaceFrequencyGraph = parentInterface->CreateChild("SpaceFrequencyGraph");
	GenericDSInterfaceABC *SpaceFrequencyGraphInterface = SpaceFrequencyGraph.GetInterface();

	ExtractSpaceFrequencyView(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, 
		"View", sx, sy, sw, sh, fx, fy, fw, fh);

	if (atLevel<maxDepth) {
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, sx, sy, sw/2, sh/2, fx, fy, fw, fh);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, sx+sw/2, sy, sw/2, sh/2, fx, fy, fw, fh);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, sx, sy+sh/2, sw/2, sh/2, fx, fy, fw, fh);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, sx+sw/2, sy+sh/2, sw/2, sh/2, fx, fy, fw, fh);

		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "FrequencyChild", 
			atLevel+1, maxDepth, sx, sy, sw, sh, fx, fy, fw/2, fh/2);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "FrequencyChild", 
			atLevel+1, maxDepth, sx, sy, sw, sh, fx+fw/2, fy, fw/2, fh/2);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "FrequencyChild", 
			atLevel+1, maxDepth, sx, sy, sw, sh, fx, fy+fh/2, fw/2, fh/2);
		ExtractSpaceFrequencyGraph(iSpaceFrequencyViewID, SpaceFrequencyGraphInterface, "FrequencyChild", 
			atLevel+1, maxDepth, sx, sy, sw, sh, fx+fw/2, fy+fh/2, fw/2, fh/2);

		SpaceFrequencyGraphInterface->SetAttribute("complete", "true");
		SpaceFrequencyGraphInterface->SetAttribute("branching", 4);
		SpaceFrequencyGraphInterface->SetAttribute("nonRedundant", "true");
	}

	else {
		SpaceFrequencyGraphInterface->SetAttribute("complete", "false");
		SpaceFrequencyGraphInterface->SetAttribute("branching", 0);
		SpaceFrequencyGraphInterface->SetAttribute("nonRedundant", "true");
	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}


// ------------------------------------------------------------------------
// Extract VideoViewGraph 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractVideoViewGraph(int iSpaceFrequencyViewID, 
														GenericDSInterfaceABC *parentInterface,
														const char* name, 
														int atLevel, int maxDepth,
														float fx, float fy, float ft,
														float fw, float fh, float fd) {

	// ------------------------------------------------------------------------
	// Create VideoViewGraph 
	// ------------------------------------------------------------------------
	
	GenericDS VideoViewGraph;
	if (name && strlen(name)>1) VideoViewGraph = parentInterface->CreateChild(name);
	else VideoViewGraph = parentInterface->CreateChild("VideoViewGraph");
	GenericDSInterfaceABC *VideoViewGraphInterface = VideoViewGraph.GetInterface();

	ExtractFrequencyView(iSpaceFrequencyViewID, VideoViewGraphInterface, 
		"View", fx, fy, ft, fw, fh, fd);

	if (atLevel<maxDepth) {
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, fx, fy, ft, fw/2, fh/2, fd);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, fx+fw/2, fy, ft, fw/2, fh/2, fd);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, fx, fy+fh/2, ft, fw/2, fh/2, fd);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "SpaceChild", 
			atLevel+1, maxDepth, fx+fw/2, fy+fh/2, ft, fw/2, fh/2, fd);

		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "TimeChild", 
			atLevel+1, maxDepth, fx, fy, ft, fw, fh, fd/4);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "TimeChild", 
			atLevel+1, maxDepth, fx, fy, ft+fd/4, fw, fh, fd/4);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "TimeChild", 
			atLevel+1, maxDepth, fx, fy, ft+2*fd/4, fw, fh, fd/4);
		ExtractVideoViewGraph(iSpaceFrequencyViewID, VideoViewGraphInterface, "TimeChild", 
			atLevel+1, maxDepth, fx, fy, ft+3*fd/4, fw, fh, fd/4);

		VideoViewGraphInterface->SetAttribute("complete", "true");
		VideoViewGraphInterface->SetAttribute("branching", 4);
		VideoViewGraphInterface->SetAttribute("nonRedundant", "true");
	}
		
	else {
		VideoViewGraphInterface->SetAttribute("complete", "false");
		VideoViewGraphInterface->SetAttribute("branching", 0);
		VideoViewGraphInterface->SetAttribute("nonRedundant", "true");
	}


	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}



// ------------------------------------------------------------------------
// Extract MultiResolutionPyramid 
// ------------------------------------------------------------------------

GenericDSInterfaceABC*
SpaceFrequencyViewExtractionTool::ExtractMultiResolutionPyramid(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth, float r) {
	
	// ------------------------------------------------------------------------
	// Create MultiResolutionPyramid 
	// ------------------------------------------------------------------------
	
	GenericDS MultiResolutionPyramid;
	if (name && strlen(name)>1) MultiResolutionPyramid = parentInterface->CreateChild(name);
	else MultiResolutionPyramid = parentInterface->CreateChild("MultiResolutionPyramid");
	GenericDSInterfaceABC *MultiResolutionPyramidInterface = MultiResolutionPyramid.GetInterface();

	ExtractResolutionView(iSpaceFrequencyViewID, MultiResolutionPyramidInterface, "View", r);

	if (atLevel<maxDepth) {
		ExtractMultiResolutionPyramid(iSpaceFrequencyViewID, MultiResolutionPyramidInterface, "Child", 
			atLevel+1, maxDepth, r/2);

		MultiResolutionPyramidInterface->SetAttribute("complete", "false");
		MultiResolutionPyramidInterface->SetAttribute("branching", 1);
		MultiResolutionPyramidInterface->SetAttribute("nonRedundant", "true");
	}
		
	else {
		MultiResolutionPyramidInterface->SetAttribute("complete", "false");
		MultiResolutionPyramidInterface->SetAttribute("branching", 0);
		MultiResolutionPyramidInterface->SetAttribute("nonRedundant", "true");
	}

	// ------------------------------------------------------------------------
	// DONE
	// ------------------------------------------------------------------------

	return parentInterface;

	}



