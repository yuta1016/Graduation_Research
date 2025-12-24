//////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Katsunao Takahashi
// Toshihiko Munetsugu 
//        (Matsushita Research Institute Tokyo, Inc./
//         Matsushita Electric Industrial Co., Ltd.)
//
// Ver4.0 for FDIS, 20010928
// Ver3.0 for FCD,  20010824
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard (ISO/IEC 15938) free licence
// to this software module or modifications thereof for use in hardware or
// software products claiming confermance to the MPEG-7 standard (ISO/IEC 15938).
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents.
// THE ORIGINAL DEVELOPER OF THIS SOFTWARE MODULE AND HIS/HER COMPANY,
// THE SUBSEQUENT EDITORS AND THEIR COMPANIES, AND ISO/IEC HAVE NO 
// LIABILITY FOR USE OF THIS SOFTWARE MODULE OR MODIFICATIONS THEROF.
// No license to this software module is granted for non MPEG-7 standard
// (ISO/IEC 15938) comforming products.
// Matsushita Electric Industrial Co., Ltd. and Matsushita Research Institute
// Tokyo, Inc. retain full right to use the software module for their own
// purpose, assign or donate the software module to a third party and 
// to inhibit third parties from using the code for non MPEG-7 standard
// (ISO/IEC 15938) conforming products.
//
// Copyright (c) 2000-2001.
//
// This copyright notice must be included in all copies or derivative works
// of the software module.
//
//
//////////////////////////////////////////////////////////////////////////////////
/*********************************
 * PointOfViewSearch.cpp         *
 *********************************/

#include <stdio.h>
#include <assert.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "Applications/ClientApplications.h"

#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"		    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <cstring>
#include <iostream>
#include <fstream>


using namespace XM ;
using namespace std;

const UUID PointOfViewSearchInterface::myID    = UUID("") ;
const char *PointOfViewSearchInterface::myName = "PointOfViewSearchInterface" ;

const UUID PointOfViewSearchTool::myID         = UUID("") ;
const char *PointOfViewSearchTool::myName      = "PointOfViewSearchTool" ;


const UUID PointOfViewSearchInterfaceABC::myID = UUID() ;

string	str_type;
string	str_xmlns;
string	str_xmlnsmpeg7;
string	str_targetNS;
string	str_elementFD;
string	str_attributeFD;
int	flag_type;
int	flag_xmlns;
int	flag_xmlnsmpeg7;
int	flag_targetNS;
int	flag_elementFD;
int	flag_attributeFD;

//=============================================================================
PointOfViewSearchInterfaceABC::PointOfViewSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
PointOfViewSearchInterface::
PointOfViewSearchInterface(PointOfViewSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID) ;
}

//----------------------------------------------------------------------------
PointOfViewSearchInterface::~PointOfViewSearchInterface()
{
}

//----------------------------------------------------------------------------
void PointOfViewSearchInterface::destroy(void)
{
  delete m_SearchTool ;
}

//----------------------------------------------------------------------------
int PointOfViewSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC  *aRefGenericDSInterface)
{
  return(m_SearchTool->SetRefDescriptionInterface(aRefGenericDSInterface)) ;
}

//----------------------------------------------------------------------------
int PointOfViewSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface)
{
	return(m_SearchTool->SetQueryDescriptionInterface(aQueryInterface)) ;
}

//----------------------------------------------------------------------------
int PointOfViewSearchInterface::SetQuery(void)
{
  return(m_SearchTool->SetQuery()) ;
}

//----------------------------------------------------------------------------
const UUID & 
PointOfViewSearchInterface::GetInterfaceID(void)
{ 
	return(myID) ;
}

//----------------------------------------------------------------------------
const char * 
PointOfViewSearchInterface::GetName(void)
{
	return(myName) ;
}

//----------------------------------------------------------------------------
double
PointOfViewSearchInterface::GetDistance(void)
{
	return(m_SearchTool->GetDistance()) ;
}

//----------------------------------------------------------------------------
GenericDS *
PointOfViewSearchInterface::GetFilteredSegment(void)
{
	return(m_SearchTool->GetFilteredSegment()) ;
}

//=============================================================================
PointOfViewSearchTool::PointOfViewSearchTool():
m_Interface(this),
m_RefDSInterface(0),
m_QueryDSInterface(0),
m_Query(0)
{
	SetExposedInterface(&m_Interface) ;
}

//----------------------------------------------------------------------------
PointOfViewSearchTool::
PointOfViewSearchTool(GenericDSInterfaceABC *aQueryInterface):
m_Interface(this),
m_RefDSInterface(0),
m_Query(0)
{
	SetExposedInterface(&m_Interface) ;
	SetQueryDescriptionInterface(aQueryInterface) ;
}

//----------------------------------------------------------------------------
PointOfViewSearchTool::~PointOfViewSearchTool()
{
	if(m_Query) delete m_Query ;
}

//----------------------------------------------------------------------------
const UUID& PointOfViewSearchTool::GetObjectID(void)
{
	return(myID) ;
}

//----------------------------------------------------------------------------
const char *PointOfViewSearchTool::GetName(void)
{
	return(myName) ;
}

//----------------------------------------------------------------------------
bool PointOfViewSearchTool::SupportsPush(void)
{
	return(true) ;
}

//----------------------------------------------------------------------------
bool PointOfViewSearchTool::SupportsPull(void)
{
	return(true) ;
}

//----------------------------------------------------------------------------
PointOfViewSearchInterfaceABC *
PointOfViewSearchTool::GetInterface(void)
{
	return(&m_Interface) ;
}

//----------------------------------------------------------------------------
int PointOfViewSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC *aRefGenericDSInterface)
{
	assert(aRefGenericDSInterface) ;
	m_RefDSInterface = (GenericDSInterface *)aRefGenericDSInterface ;
	return(0) ;
}

//----------------------------------------------------------------------------
int PointOfViewSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface)
{
	assert(aQueryInterface) ;
	m_QueryDSInterface = (GenericDSInterface *)aQueryInterface ;
	return(0) ;
}

//----------------------------------------------------------------------------
int
PointOfViewSearchTool::SetQuery(void)
{
	if(m_Query) delete m_Query ;
	m_Query = new POV_BrowsingQuery ;
	return(m_Query->ExtractQueryFromDS(m_QueryDSInterface)) ;
}


//----------------------------------------------------------------------------
double
PointOfViewSearchTool::GetDistance(void)
{
	if(!SegmentFiltering())
	{
		m_distanceValue = 1.0 ;
	}
	else
	{
		m_distanceValue = 0.1 ;
	}
	return(m_distanceValue) ;
}

//----------------------------------------------------------------------------
bool	PointOfViewSearchTool::SegmentFiltering(void) {
	assert(m_RefDSInterface);

	if (!m_Query) {// Query is not built yet.
		if ((!m_QueryDSInterface) || (SetQuery() != 0)) { // QueryDSInterface is not set yet or failed to build Query
			return(false);
		}
	}

	// Check PreferredSummaryTheme array
	if(m_Query->GetPreferredSummaryThemeLength() <= 0)
	{
		cerr << "PointOfView List is null\n";
		return(false);
	}

	// Check if really Segment DS
	// Check Root Node
	// Check Root Node -> "Mpeg7"
	GenericDS theNowNode = m_RefDSInterface->GetFirstChild();
	string    nodeName   = theNowNode.GetDSName();
	if (nodeName != MPEG7MAIN_ELEMENT) {
		return(false);
	}

	if (theNowNode.GetTextAttribute(MAIN_TYPE_ATTR, str_type) != 0) {
		flag_type     = 0;
	}
	else {
		flag_type     = 1;
	}
	if (theNowNode.GetTextAttribute(MAIN_XMLNS_ATTR, str_xmlns) != 0) {
		flag_xmlns    = 0;
	}
	else {
		flag_xmlns    = 1;
	}
	if (theNowNode.GetTextAttribute(MAIN_XMLNSMPEG7_ATTR, str_xmlnsmpeg7) != 0) {
		flag_xmlnsmpeg7 = 0;
	}
	else {
		flag_xmlnsmpeg7 = 1;
	}
	if (theNowNode.GetTextAttribute(MAIN_TARGETNS_ATTR, str_targetNS) != 0) {
		flag_targetNS   = 0;
	}
	else {
		flag_targetNS   = 1;
	}
	if (theNowNode.GetTextAttribute(MAIN_ELEMENTFD_ATTR, str_elementFD) != 0) {
		flag_elementFD  = 0;
	}
	else {
		flag_elementFD  = 1;
	}
	if (theNowNode.GetTextAttribute(MAIN_ATTRIBUTEFD_ATTR, str_attributeFD) != 0) {
		flag_attributeFD = 0;
	}
	else {
		flag_attributeFD = 1;
	}

	// Check ContentDescription Node
	theNowNode = theNowNode.GetFirstChild();
	nodeName   = theNowNode.GetDSName();
	if (nodeName != CONTENTDESCRIPTION) {
		return(false);
	}

	// Check if type attribute value is ContentEntityDescriptionType
	string attValue;
	if (theNowNode.GetTextAttribute(CDESC_TYPE_ATTR, attValue) != 0) {
		return(false);
	}
	if (attValue != CDESC_CENTITY_TYPE) {
		return(false);
	}

	// Check MultimediaContent
	theNowNode = theNowNode.GetFirstChild();
	nodeName   = theNowNode.GetDSName();
	if(nodeName != AUDIOVISUAL_CONTENT) {
		return(false);
	}

	// Check AudioVisual Node
	theNowNode = theNowNode.GetDescription(AUDIOVISUAL_ELEMENT);
	if(theNowNode.isNull()) {
		return(false);
	}

	// Set Result Segment DS
	m_FilteredSegment = GenericDS::CreateDescriptionRoot() ;
	GenericDSInterface * theFilteredSegmentInterface ;
	
	// Get Reference SegmentDS and Copy all node to FilteredSegment
	theFilteredSegmentInterface = (GenericDSInterface *)m_FilteredSegment.GetInterface() ;
	CopyDescriptionTool theCopyTool(theFilteredSegmentInterface, m_RefDSInterface) ;
	theCopyTool.ExecFullCopy() ;

	// Set Filtering Tool
	if(m_Query->GetSummaryDuration() > 0)
	{
		m_FilteringTool = new POVSegmentFilterWithDurationTool(m_Query, m_FilteredSegment) ;
	}
	else
	{
		m_FilteringTool = new POVSegmentFilterWithoutDurationTool(m_Query, m_FilteredSegment) ;
	}

	// Filtering of Segment
	assert(m_FilteringTool) ;
	m_FilteringTool->PrepareForFiltering() ;
	bool retVal = m_FilteringTool->ExecuteFiltering() ;

	delete m_FilteringTool ;

	return(retVal) ;
}

//----------------------------------------------------------------------------
GenericDS *
PointOfViewSearchTool::GetFilteredSegment(void)
{
	return(&m_FilteredSegment) ;
}

//=============================================================================
POVSegmentFilterToolABC::
POVSegmentFilterToolABC(POV_BrowsingQuery *aQuery,
			GenericDS & aFilteredSegment):
m_Query(aQuery),
m_FilteredSegment(aFilteredSegment)
{
}

//----------------------------------------------------------------------------
POVSegmentFilterToolABC::~POVSegmentFilterToolABC()
{
}

//----------------------------------------------------------------------------
int
POVSegmentFilterToolABC::SetObjectDS(GenericDS & aFilteredSegment)
{
	m_FilteredSegment = aFilteredSegment;
	return(0);
}

//----------------------------------------------------------------------------
bool
POVSegmentFilterToolABC::ExecuteFiltering()
{
	// Get AudioVisual Node
//	bool      found	     = false;
	GenericDS theNowNode = FindTopSegmentNode(m_FilteredSegment);

	if(theNowNode.isNull())
	{
		return(false);
	}

	// Filtering Segment
	while (!(theNowNode.isNull())) {
		GenericDS theNextNode = theNowNode.GetNextSibling();
		int       status      = NodeFiltering(theNowNode);
		if (status == -1) {
			return(false);
		}
		else if (status == 1) {
			m_FilteredSegment.RemoveChild(theNowNode);
		}
		theNowNode = theNextNode;
	}
	return(true);
}

//----------------------------------------------------------------------------
GenericDS
POVSegmentFilterToolABC::FindTopSegmentNode(GenericDS & theNowNode)
{
	string nodeName = theNowNode.GetDSName() ;
	if(nodeName == AUDIOVISUAL_ELEMENT)
	{
		return(theNowNode) ;
	}
	else{
		GenericDS childNode = theNowNode.GetFirstChild() ;
		GenericDS theNode ;
		while(!(childNode.isNull()))
		{
			theNode = FindTopSegmentNode(childNode) ;
			if(!(theNode.isNull()))
			{
				break ;
			}
			childNode = childNode.GetNextSibling();
		}
		return(theNode) ;
	}
}

//----------------------------------------------------------------------------
int
POVSegmentFilterToolABC::NodeFiltering(GenericDS & theNowNode)
{
	int    fstatus;
	string nodeName = theNowNode.GetDSName();
	if ((nodeName == AUDIOVISUAL_SEGMENT) || (nodeName == AUDIOVISUAL_ELEMENT)) {	// Filtering Segment Node
		// For SegmentDecomposition node
		std::vector<GenericDS> segDecomps = theNowNode.GetAllDescriptions(AV_SEGMENTDECOMP);

		bool remainSD = false;

		for (int i = 0; i < (int) segDecomps.size(); i++) {
			// for the children AudioVideoSegments of SegmentDecomposition
			GenericDS theNowSegmentDecomp = segDecomps[i];
			std::vector<GenericDS> videoSegs = theNowSegmentDecomp.GetAllDescriptions(AUDIOVISUAL_SEGMENT);
			bool remainVS = false;
			for (int j = 0; j < (int) videoSegs.size(); j ++) {
				GenericDS theNowSegment = videoSegs[j];
				fstatus = NodeFiltering(theNowSegment);
				if (fstatus == -1) {
					return(-1);
				}
				else if (fstatus == 1) {
					theNowSegmentDecomp.RemoveChild(theNowSegment);
					theNowSegmentDecomp.SetAttribute(AUDIOVISUAL_GAP_ATTR, "true");
				}
				else {
					remainVS = true;
					stripPointOfView(theNowSegment);
				}
			}
			if (!remainVS) {
				// This SegmentDecompotion element has no children.
				theNowNode.RemoveChild(theNowSegmentDecomp);
			}
			else {
				// This SegmentDecompotion element has some children.
				remainSD = true;
			}
		}
		if (remainSD) {
			// Because this VideoSegment has child SegmentDecomposition,
			// it is not filtered out.
			stripPointOfView(theNowNode);

			return(0);
		}

		// Check this VideoSegment PointOfView
		if(IsThisSegmentFilteredOut(theNowNode)) {
			// Filter out this segments
			return(1);
		}
		else {
			stripPointOfView(theNowNode);

			return(0);
		}
	}
	else {
		// for the node except VideoSegment
		GenericDS theNowChildNode = theNowNode.GetFirstChild();
		while (!(theNowChildNode.isNull())) {
			GenericDS theNextChildNode = theNowChildNode.GetNextSibling();
			fstatus = NodeFiltering(theNowChildNode);
			if (fstatus == -1) {
				return(-1);
			}
			else if (fstatus == 1) {
				theNowNode.RemoveChild(theNowChildNode);
			}
			theNowChildNode = theNextChildNode;
		}

		return(0);
	}
}

//----------------------------------------------------------------------------
void
POVSegmentFilterToolABC::stripPointOfView(GenericDS & theNowNode)
{
	std::vector<GenericDS> vsVector = theNowNode.GetAllDescriptions(POINTOFVIEW_ELEMENT);
	std::vector<POV_PreferredSummaryTheme> povList;
	povList = m_Query -> GetPreferredSummaryTheme();
	string povKeyword;

	for(int i = 0; i < (int) vsVector.size(); i++)
	{
		GenericDS thePOVNode = vsVector[i];
		if (thePOVNode.GetTextAttribute(POV_VIEWPOINT_ATTR, povKeyword) != 0)
		{
			continue ;
		}
		
		bool removeThisPOV = true;
		for(int j = 0; j < (int) povList.size(); j++)
		{
			POV_PreferredSummaryTheme psTheme = povList[j];
			if (psTheme.GetPreferrenceValue() < 0)
			{
				continue ;
			}
			string themeValue = psTheme.GetThemeValue() ;
			
			if(povKeyword == themeValue)
			{
				removeThisPOV = false ;
				break ;
			}
		}
		if(removeThisPOV)
		{
			theNowNode.RemoveChild(thePOVNode) ;
		}
	}
	return ;
}

//----------------------------------------------------------------------------
double
POVSegmentFilterToolABC::calcSegmentScore(GenericDS & theNowNode)
{
	std::vector<POV_PreferredSummaryTheme>	povList;
	std::vector<GenericDS>			vsVector;
	int					i;

	povList  = m_Query -> GetPreferredSummaryTheme();
	vsVector = theNowNode.GetAllDescriptions(POINTOFVIEW_ELEMENT);

	double	SegValue = 0.0;
	for (i = 0; i < (int) vsVector.size(); i ++) {
		GenericDS	thePOVNode = vsVector[i];
		string		viewPoint;
		if (thePOVNode.GetTextAttribute(POV_VIEWPOINT_ATTR, viewPoint) != 0) {
			continue;
		}

		GenericDS	theValueNode0 = thePOVNode.GetDescription(POV_IMPORTANCE_ELEMENT);
		GenericDS	theValueNode  = theValueNode0.GetDescription(POV_VALUE_ELEMENT);
		double		povValue;
		if (theValueNode.GetRealValue(povValue) != 0) {
			continue;
		}

		for (int j = 0; j < (int) povList.size(); j ++) {
			POV_PreferredSummaryTheme	psTheme = povList[j];
			string			themeValue = psTheme.GetThemeValue();
			double			prefValue  = (double) psTheme.GetPreferrenceValue();
			double			nowSegValue;
			if (viewPoint == themeValue) {
				nowSegValue = prefValue * povValue;
				SegValue    = (abs((int) SegValue) > abs((int) nowSegValue)) ? SegValue : nowSegValue;
			}
		}
	}

	return(SegValue);
}

//=============================================================================
POVSegmentFilterWithDurationTool::
POVSegmentFilterWithDurationTool(POV_BrowsingQuery *aQuery, GenericDS &aFilteredSegment):
POVSegmentFilterToolABC(aQuery, aFilteredSegment)
{
	m_selectedIdx = -1 ;
}

//----------------------------------------------------------------------------
POVSegmentFilterWithDurationTool::~POVSegmentFilterWithDurationTool()
{
}

//----------------------------------------------------------------------------
void
POVSegmentFilterWithDurationTool::PrepareForFiltering()
{
	sortObjectSegment() ;
	m_selectedIdx = selectSegments() ;
}

//----------------------------------------------------------------------------
void POVSegmentFilterWithDurationTool::sortObjectSegment()
{
	GenericDS theNowNode = m_FilteredSegment.GetFirstChild() ;
	while(!(theNowNode.isNull()))
	{
		checkNode(theNowNode) ;
		theNowNode = theNowNode.GetNextSibling() ;
	}
	return ;
}

//----------------------------------------------------------------------------
void POVSegmentFilterWithDurationTool::checkNode(GenericDS & theNowNode)
{
	string nodeName = theNowNode.GetDSName();
	if (nodeName == AUDIOVISUAL_SEGMENT) {
		checkObjectSegment(theNowNode);
	}
	else if (nodeName == AV_MEDIATIME) {
		GenericDS nextNode = theNowNode.GetNextSibling();
		checkNode(nextNode);
	}
	else
	{
		GenericDS childNode = theNowNode.GetFirstChild() ;
		while(!(childNode.isNull()))
		{
			checkNode(childNode) ;
			childNode = childNode.GetNextSibling() ;
		}
	}
	return ;
}

//----------------------------------------------------------------------------
void POVSegmentFilterWithDurationTool::checkObjectSegment(GenericDS & theNowNode)
{
	std::vector<GenericDS> segDecomps = theNowNode.GetAllDescriptions(AV_SEGMENTDECOMP) ;
	if (segDecomps.size() > 0) {
		for (int i = 0; i < (int) segDecomps.size(); i ++) {
			GenericDS segDecomp = segDecomps[i];
			std::vector<GenericDS> vSegs = segDecomp.GetAllDescriptions(AUDIOVISUAL_SEGMENT);
			for (int j = 0; j < (int) vSegs.size(); j ++) {
				GenericDS vSeg = vSegs[j];
				checkObjectSegment(vSeg);
			}
		}
	}
	else {
		string segID  ;
		theNowNode.GetTextAttribute(AUDIOVISUAL_ID_ATTR, segID);
		double    segValue     = calcSegmentScore(theNowNode);
		GenericDS theMediaTime = theNowNode.GetDescription(AV_MEDIATIME);
		GenericDS theDuration  = theMediaTime.GetDescription(AV_MEDIADURATION);
		CalcDurationValue theCalcDuration(theDuration);
		double segmentDuration = theCalcDuration.GetDurationValue();

		ObjectSegmentInfo newSegment;
		newSegment.SetID(segID);
		newSegment.SetValue(segValue);
		newSegment.SetDuration(segmentDuration);
		appendSegmentVector(newSegment);
	}
	return ;
}

//----------------------------------------------------------------------------
void
POVSegmentFilterWithDurationTool::appendSegmentVector(ObjectSegmentInfo newSegment)
{
	double SegValue = newSegment.GetValue() ;
	std::vector<ObjectSegmentInfo>::iterator p = m_vSegs.begin() ;
	while(p != m_vSegs.end())
	{
		if(p->GetValue() < SegValue)
		{
			break ;
		}
		p++ ;
	}
	m_vSegs.insert(p, newSegment) ;
	return ;
}

//----------------------------------------------------------------------------
int
POVSegmentFilterWithDurationTool::selectSegments()
{
	int	i;
	double maxDuration = (double) (m_Query -> GetSummaryDuration());
	double sumDuration = 0;
	for (i = 0; i < (int) m_vSegs.size(); i++) {
		ObjectSegmentInfo nowSegment = m_vSegs[i];
		if (nowSegment.GetValue() < 0.0) {
			break;
		}
		sumDuration += nowSegment.GetDuration();
		if (sumDuration > maxDuration) {
			break;
		}
	}
	return(i - 1);
}

//----------------------------------------------------------------------------
bool POVSegmentFilterWithDurationTool::
IsThisSegmentFilteredOut(GenericDS & theNowNode)
{
	// Return : false :: This segment is selected
	//          true  :: This segment is filtered out.
	string nowSegmentId ;

	theNowNode.GetTextAttribute(AUDIOVISUAL_ID_ATTR, nowSegmentId);
	bool filteredOut = true;
	for (int i = 0; i <= (int) m_selectedIdx; i++) {
		ObjectSegmentInfo nowSegment = m_vSegs[i];
		string            nowSegsId  = nowSegment.GetID();
		if(nowSegsId == nowSegmentId) {
			filteredOut = false;
			break;
		}
	}
	return(filteredOut);
}

//=============================================================================
POVSegmentFilterWithoutDurationTool::
POVSegmentFilterWithoutDurationTool(POV_BrowsingQuery *aQuery, GenericDS &aFilteredSegment):
POVSegmentFilterToolABC(aQuery, aFilteredSegment)
{
}

//----------------------------------------------------------------------------
POVSegmentFilterWithoutDurationTool::
~POVSegmentFilterWithoutDurationTool()
{
}

//----------------------------------------------------------------------------
void
POVSegmentFilterWithoutDurationTool::PrepareForFiltering()
{
}

//----------------------------------------------------------------------------
bool POVSegmentFilterWithoutDurationTool::
IsThisSegmentFilteredOut(GenericDS & theNowNode)
{
	// Return : false :: This segment is selected
	//          true  :: This segment is filtered out.
	if(calcSegmentScore(theNowNode) > THRESHOLD_SEGMENT_VALUE)
	{
		return(false) ;
	}
	else
	{
		return(true) ;
	}
}

//=============================================================================
ObjectSegmentInfo::ObjectSegmentInfo()
{
}

//----------------------------------------------------------------------------
ObjectSegmentInfo::~ObjectSegmentInfo()
{
}

//----------------------------------------------------------------------------
void
ObjectSegmentInfo::SetID(string aID)
{
	m_segmentID = string(aID) ;
}

//----------------------------------------------------------------------------
string
ObjectSegmentInfo::GetID()
{
	return(string(m_segmentID)) ;
}

//----------------------------------------------------------------------------
void
ObjectSegmentInfo::SetValue(double aValue)
{
	m_value = aValue ;
}

//----------------------------------------------------------------------------
double
ObjectSegmentInfo::GetValue()
{
	return(m_value) ;
}

//----------------------------------------------------------------------------
void
ObjectSegmentInfo::SetDuration(double aDuration)
{
	m_segmentDuration = aDuration ;
}

//----------------------------------------------------------------------------
double
ObjectSegmentInfo::GetDuration()
{
	return(m_segmentDuration) ;
}

//=============================================================================
FilteredSegmentToSummary::FilteredSegmentToSummary()
{
}

//----------------------------------------------------------------------------
FilteredSegmentToSummary::~FilteredSegmentToSummary()
{
}

//----------------------------------------------------------------------------
void
FilteredSegmentToSummary::SetFilteredSegment(GenericDS *afilteredSegment)
{
	m_filteredSegment = afilteredSegment ;
}

//----------------------------------------------------------------------------
bool
FilteredSegmentToSummary::ConvertToSummary()
{
	// Create Summarization DS root node
	m_Summarization = GenericDS::CreateDescriptionRoot();

	GenericDS theNewNode = m_Summarization.CreateChild("Mpeg7");

	GenericDS theNowNode = m_filteredSegment -> GetFirstChild();

	// Set TYPE     Attribute
	if (flag_type) {
		theNewNode.SetAttribute(MAIN_TYPE_ATTR,     str_type);
	}
	// Set XMLNS    Attribute
	if (flag_xmlns) {
		theNewNode.SetAttribute(MAIN_XMLNS_ATTR,    str_xmlns);
	}
	// Set XMLNSMPEG7 Attribute
	if (flag_xmlnsmpeg7) {
		theNewNode.SetAttribute(MAIN_XMLNSMPEG7_ATTR, str_xmlnsmpeg7);
	}
	// Set TARGETNAMESPACE Attribute
	if (flag_targetNS) {
		theNewNode.SetAttribute(MAIN_TARGETNS_ATTR,    str_targetNS);
	}
	// Set ELEMENTFORMDEFALULT Attribute
	if (flag_elementFD) {
		theNewNode.SetAttribute(MAIN_ELEMENTFD_ATTR,    str_elementFD);
	}
	// Set ATTRIBUTEFORMDEFALULT Attribute
	if (flag_attributeFD) {
		theNewNode.SetAttribute(MAIN_ATTRIBUTEFD_ATTR,    str_attributeFD);
	}

	// Create ContentDescription Node
	theNowNode = theNewNode ;
	theNewNode = theNowNode.CreateChild(CONTENTDESCRIPTION) ;
	// Set type Attribute
	theNewNode.SetAttribute(CDESC_TYPE_ATTR, CDESC_SUMMARYTYPE_VAL) ;
	
	// Create Summarization Node
	theNowNode = theNewNode ;
	GenericDS theTopNode = theNowNode.CreateChild(SUMMARIZATION_ELEMENT) ;

	// Create Summary node and set attributes
	GenericDS theHierarchicalSummary = theTopNode.CreateChild(SUMMARY_ELEMENT) ;
	// type
	theHierarchicalSummary.SetAttribute(SUMMARY_TYPE_ATTR, HIERAR_SUMM_TYPE) ;
	// components
	theHierarchicalSummary.SetAttribute(HS_COMPLIST_ATTR, HLS_KEYVIDEOCLIP_VAL);
	// hierarchy
	theHierarchicalSummary.SetAttribute(HS_HIERARCHY_ATTR, "independent") ;

	// Create SummaryThemeList node
	GenericDS theSummaryThemeList = theHierarchicalSummary.CreateChild(HS_SUMMARYTHEMELIST) ;

	// Create HighlightSummary node and set attributes
	GenericDS theHighlightSummary = theHierarchicalSummary.CreateChild(HIGHLIGHTSUMMARY_ELEM) ;
	theHighlightSummary.SetAttribute(HLL_LEVEL_ATTR, 1) ;

	// Convert SegmentDS to Summrization
	m_summaryDuration = 0 ;
	theNowNode = FindTopSegmentNode(*m_filteredSegment) ;
	while(!(theNowNode.isNull()))
	{
		convertNode(theNowNode, theHighlightSummary, theSummaryThemeList) ;
		theNowNode = theNowNode.GetNextSibling() ;
	}

	// Set SummaryDuration to HighlightLevel attribute "duration"
	if(m_summaryDuration > 0){
		string durString = createDurationString(m_summaryDuration) ;
		theHighlightSummary.SetAttribute(HLL_DURATION_ATTR, durString) ;
	}

	// if SummaryThemeList has no child, remove SummaryThemeListNode
	if(theSummaryThemeList.GetNumberOfChildren() == 0)
	{
		theHierarchicalSummary.RemoveChild(theSummaryThemeList) ;
	}

	return(true) ;
}

//----------------------------------------------------------------------------
GenericDS *
FilteredSegmentToSummary::GetSummarization()
{
	return(&m_Summarization) ;
}

//----------------------------------------------------------------------------
void FilteredSegmentToSummary::
convertNode(GenericDS & theNowNode, GenericDS & theHighlightSummary, GenericDS & theSummaryThemeList)
{
	if(theNowNode.GetDSName() == AUDIOVISUAL_SEGMENT)
	{
		convertObjectSegment(theNowNode, theHighlightSummary, theSummaryThemeList) ;		
	}
	else
	{
		GenericDS theChildNode = theNowNode.GetFirstChild() ;
		while(!(theChildNode.isNull()))
		{
			convertNode(theChildNode, theHighlightSummary, theSummaryThemeList) ;
			theChildNode = theChildNode.GetNextSibling() ;
		}
	}
	return ;
}

//----------------------------------------------------------------------------
void FilteredSegmentToSummary::
convertObjectSegment(GenericDS & theNowNode, GenericDS & theHighlightSummary, GenericDS & theSummaryThemeList)
{
	std::vector<GenericDS> segDecomps = theNowNode.GetAllDescriptions(AV_SEGMENTDECOMP);
	if (segDecomps.size() > 0) {
		for(int i = 0; i < (int) segDecomps.size(); i++) {
			GenericDS segDecomp = segDecomps[i];
			std::vector<GenericDS> vSegs = segDecomp.GetAllDescriptions(AUDIOVISUAL_SEGMENT);
			for(int j = 0; j < (int) vSegs.size(); j++)
			{
				GenericDS vSeg = vSegs[j];
				convertObjectSegment(vSeg, theHighlightSummary, theSummaryThemeList);
			}
		}
	}
	else {
		GenericDS theHighlightSegment = theHighlightSummary.CreateChild(HIGHLIGHTSEGMENT_ELEM);
		string vsegID;
		if (theNowNode.GetTextAttribute(AUDIOVISUAL_ID_ATTR, vsegID) == 0) {
			GenericDS theNameNode = theHighlightSegment.CreateChild(HLS_NAME_ELEM);
			theNameNode.SetValue(vsegID);
		}
		string themeIDs;
		std::vector<GenericDS> povList = theNowNode.GetAllDescriptions(POINTOFVIEW_ELEMENT);
		int i;
		for(i = 0; i < (int) povList.size(); i++) {
			string viewPoint;
			GenericDS thePOVNode = povList[i];
			if (thePOVNode.GetTextAttribute(POV_VIEWPOINT_ATTR, viewPoint) != 0) {
				continue;
			}
			string thisID;
			std::map<string, string>::iterator p;
			p = m_summaryTheme.find(viewPoint);
			if (p == m_summaryTheme.end()) {
				// New SummaryTheme
				char tmpC[50] ;
				sprintf(tmpC, "theme%d", m_summaryTheme.size()) ;
				thisID = string(tmpC) ;
				m_summaryTheme.insert(std::pair<string, string>(viewPoint, thisID)) ;
				// Create New SummaryTheme node
				GenericDS theNewSummaryTheme = theSummaryThemeList.CreateChild(HS_SUMMARYTHEME) ;
				theNewSummaryTheme.SetAttribute(HS_ST_ID_ATTR, thisID) ;
				theNewSummaryTheme.SetValue(viewPoint) ;
			}
			else
			{
				thisID = p->second ;
			}
			if(themeIDs != "")
			{
				themeIDs += " " ;
			}
			themeIDs += thisID ;
		}
		if(themeIDs != "")
		{
			theHighlightSegment.SetAttribute(HLS_THEMEIDS_ATTR, themeIDs) ;
		}

		GenericDS theVideoClip = theHighlightSegment.CreateChild(HLS_KEYVIDEOCLIP_ELEM) ;
		// Copy MediaTime 
		GenericDS theNewMediaTime = theVideoClip.CreateChild(AV_MEDIATIME) ;
		GenericDS theMediaTime = theNowNode.GetDescription(AV_MEDIATIME) ;
		GenericDSInterface *destInterface = (GenericDSInterface *)theNewMediaTime.GetInterface() ;
		GenericDSInterface *srcInterface  = (GenericDSInterface *)theMediaTime.GetInterface() ;
		CopyDescriptionTool theCopyTool(destInterface, srcInterface) ;
		theCopyTool.ExecFullCopy() ;

		GenericDS theDuration = theMediaTime.GetDescription(AV_MEDIADURATION) ;
		CalcDurationValue theCalcDuration(theDuration) ;
		double thisDuration = theCalcDuration.GetDurationValue() ;
		if(thisDuration > 0)
		{
			m_summaryDuration += thisDuration ;
		}
	}

	return ;
}

//----------------------------------------------------------------------------
string
FilteredSegmentToSummary::createDurationString(double durValue, long base)
{
	long hour, minute, second, frac;
	long durIntValue = (long)durValue;
	double durFracValue = durValue - (double)durIntValue;
	if (base <= 0) {
		base = 10;
	}

	char tmpC[10];
	frac = (long)(durFracValue * (double)base + 0.5);
	if (frac == base) {
		frac = 0;
		durIntValue++;
	}
	second = durIntValue % 60;
	minute = ((durIntValue - second)/60) % 60;
	hour = (durIntValue - (minute * 60) - second) / 3600;
	if ((hour == 0) && (minute == 0) && (second == 0) && (frac == 0)) {
		return(string(""));
	}
	string durString = "PT";
	if (hour > 0) {
		sprintf(tmpC, "%dH", (int) hour);
		durString += string(tmpC);
	}
	if (minute > 0) {
		sprintf(tmpC, "%dM", (int) minute);
		durString += string(tmpC);
	}
	if (second > 0) {
		sprintf(tmpC, "%dS", (int) second);
		durString += string(tmpC);
	}
	if (frac > 0) {
		sprintf(tmpC, "%dN%dF", (int) frac, (int) base);
		durString += string(tmpC);
	}
	
	return(durString) ;
}

//----------------------------------------------------------------------------
GenericDS
FilteredSegmentToSummary::FindTopSegmentNode(GenericDS & theNowNode)
{
	string nodeName = theNowNode.GetDSName() ;
	if(nodeName == AUDIOVISUAL_ELEMENT)
	{
		return(theNowNode) ;
	}
	else{
		GenericDS childNode = theNowNode.GetFirstChild() ;
		GenericDS theNode ;
		while(!(childNode.isNull()))
		{
			theNode = FindTopSegmentNode(childNode) ;
			if(!(theNode.isNull()))
			{
				break ;
			}
			childNode = childNode.GetNextSibling();
		}
		return(theNode) ;
	}
}

//=============================================================================
CalcDurationValue::CalcDurationValue(GenericDS & aMediaDuration)
{
	SetMediaDuration(aMediaDuration) ;
}

//----------------------------------------------------------------------------
CalcDurationValue::~CalcDurationValue()
{
}

//----------------------------------------------------------------------------
void
CalcDurationValue::SetMediaDuration(GenericDS & aMediaDuration)
{
	theMediaDuration = aMediaDuration ;
}

//----------------------------------------------------------------------------
double
CalcDurationValue::GetDurationValue()
{
	string durString;
	if (theMediaDuration.GetTextValue(durString) != 0) {
		cerr << "Faild to get the MediaDuration value.\n";

		return(-1);
	}

	int i ;
	for(i = 0 ; durString[i] != '\0' ; i++)
	{
		if(durString[i] == 'T')
		{
			break ;
		}
	}
	if (i >= (int) durString.size()) {
		cerr << "This MediaDuration is wrong format.\n";

		return(-1);
	}
	
	double    hour, minute, second, msec, denom;
	hour = minute = second = msec = denom = 0.0;
	double          msecValue;
	char            c;
	char            valStr[10];
	int             vid = 0;
	string          whiteSpace = " \t\n\r";
	while ((c = durString[++i]) != '\0') {
		if (isdigit(c)){
			valStr[vid++] = c;
		}
		else if (c == 'H') {
			if (vid > 0) {
				valStr[vid] = '\0';
				hour = atof(valStr);
				vid = 0;
			}
		}
		else if (c == 'M') {
			if (vid > 0) {
				valStr[vid] = '\0';
				minute = atof(valStr);
				vid = 0;
			}
		}
		else if (c == 'S') {
			if (vid > 0) {
				valStr[vid] = '\0';
				second = atof(valStr);
				vid = 0;
			}
		}
		else if (c == 'N') {
			if (vid > 0) {
				valStr[vid] = '\0';
				msec = atof(valStr);
				vid = 0;
			}
		}
		else if (c == 'F') {
			if (vid > 0) {
				valStr[vid] = '\0';
				denom = atof(valStr);
				vid = 0;
			}
		}
		else if (whiteSpace.find(c) >= 0) {
			break;
		}
		else
		{
			return(-1) ;
		}
	}
	if((msec > 0) && (denom > 0))
	{
		msecValue = msec / denom ;
	}
	else
	{
		msecValue = 0.0 ;
	}

	return(((hour * 60.0) + minute) * 60.0 + second + msecValue) ;
}

//=============================================================================
CopyDescriptionTool::CopyDescriptionTool():
m_DestInterface(0),
m_SrcInterface(0)
{
}

//----------------------------------------------------------------------------
CopyDescriptionTool::CopyDescriptionTool(GenericDSInterface *aDestInterface, GenericDSInterface *aSrcInterface)
{
	SetDestination(aDestInterface) ;
	SetSource(aSrcInterface) ;
}

//----------------------------------------------------------------------------
CopyDescriptionTool::~CopyDescriptionTool()
{
}

//----------------------------------------------------------------------------
void
CopyDescriptionTool::SetDestination(GenericDSInterface *aDestInterface)
{
	assert(aDestInterface) ;
	m_DestInterface = aDestInterface ;
}

//----------------------------------------------------------------------------
void
CopyDescriptionTool::SetSource(GenericDSInterface *aSrcInterface)
{
	assert(aSrcInterface) ;
	m_SrcInterface = aSrcInterface ;
}

//----------------------------------------------------------------------------
void
CopyDescriptionTool::ExecCopy()
{
	assert(m_DestInterface) ;
	assert(m_SrcInterface) ;

	// Copy Attribute
	copyAttributes(m_DestInterface, m_SrcInterface) ;

	// Copy Content
	copyContents(m_DestInterface, m_SrcInterface) ;

	return ;
}

//----------------------------------------------------------------------------
void
CopyDescriptionTool::ExecFullCopy()
{
	assert(m_DestInterface) ;
	assert(m_SrcInterface) ;

	copyNodeWithChildren(m_DestInterface, m_SrcInterface) ;
	return ;
}

//----------------------------------------------------------------------------
void	CopyDescriptionTool::copyAttributes(
		GenericDSInterface *destInterface,
		GenericDSInterface *srcInterface
	)
{
	// Copy Attribute
	std::vector<string> attrNames;

	attrNames = srcInterface -> GetAttributeNames();
	string  attrName;
	string  attrValue;

	for (int i = 0; i < (int) attrNames.size(); i ++) {
		attrName = attrNames[i];
		if (srcInterface -> GetTextAttribute(attrName, attrValue) != 0) {
			cerr << "Unable to extract attribute value of " << attrName.c_str() << "\n";
/*			continue;*/
		}
		destInterface -> SetAttribute(attrName, attrValue);
	}

	return;
}

//----------------------------------------------------------------------------
void CopyDescriptionTool::
copyContents(GenericDSInterface *destInterface, GenericDSInterface *srcInterface)
{
	// Copy Content
	string contentValue;
	if ((srcInterface -> GetTextValue(contentValue) == 0) && (contentValue != "")) {
		destInterface -> SetValue(contentValue);
	}

	return ;
}

//----------------------------------------------------------------------------
void CopyDescriptionTool::
copyNodeWithChildren(GenericDSInterface *destInterface, GenericDSInterface *srcInterface)
{
	// Copy Attribute
	copyAttributes(destInterface, srcInterface) ;

	// Copy Content
	copyContents(destInterface, srcInterface) ;

	// Copy Child Node
	int childLength = srcInterface -> GetNumberOfChildren();
	for(int i = 0; i < (int) childLength; i++) {
		GenericDS srcChildNode = srcInterface -> GetChild(i);
		GenericDSInterface *srcChildInterface = (GenericDSInterface *)srcChildNode.GetInterface();
		string nodeName = srcChildInterface -> GetDSName();
		GenericDS destChildNode = destInterface -> CreateChild(nodeName);
		GenericDSInterface *destChildInterface = (GenericDSInterface *)destChildNode.GetInterface();
		copyNodeWithChildren(destChildInterface, srcChildInterface);
	}
	
	return ;
}


//=============================================================================
POV_BrowsingQuery::POV_BrowsingQuery()
{
	m_summaryDuration = -1 ;
}

//----------------------------------------------------------------------------
POV_BrowsingQuery::~POV_BrowsingQuery()
{
}

//----------------------------------------------------------------------------
int
POV_BrowsingQuery::ExtractQueryFromDS(GenericDSInterface *QueryDSInterface)
{
	if (!QueryDSInterface) {
		return(-9);
	}

	GenericDSInterface	*theSummaryPreferencesDSInterface = QueryDSInterface;

	GenericDS		 theSummaryPreferencesDS          = theSummaryPreferencesDSInterface -> GetFirstChild();

	string			 nodeName;
	// Search SummaryPreferences Node
	// Check Root Node
	nodeName = theSummaryPreferencesDS.GetDSName();
	if (nodeName != MPEG7MAIN_ELEMENT) {
		return(-1);
	}
	// Check ContentManagement Node
	GenericDS theNowNode = theSummaryPreferencesDS.GetFirstChild();
	nodeName = theNowNode.GetDSName();
	if (nodeName != CONTENTMANAGEMENT) {
		return(-1);
	}
	// Check if type attribute value is UserDescriptionType
	string attValue;
	if (theNowNode.GetTextAttribute(CMNG_TYPE_ATTR, attValue) != 0) {
		return(NOT_USERPREFERENCES);
	}
	if (attValue != POV_USERDESCRIPTION) {
		return(NOT_USERPREFERENCES);
	}

	// Check  UserPreferences Node
	theNowNode = theNowNode.GetFirstChild();
	nodeName = theNowNode.GetDSName();
	if (nodeName != USERPREFERENCE_ELEMENT) {
		return(-1);
	}

	// search BrowsingPreferences
	nodeName = POV_BROWSINGPREF;
	GenericDS childNode;
	childNode = theNowNode.GetDescription(nodeName);
	if (childNode.isNull()) {
		return(-3);
	}

	// search SummaryPreferences
	nodeName = POV_SUMMARYPREF;
	childNode = childNode.GetDescription(nodeName);
	if (childNode.isNull()) {
		return(-4);
	}

	// Extract Value
	childNode = childNode.GetFirstChild();
	while (!(childNode.isNull())) {
		nodeName = childNode.GetDSName();
		if (nodeName == POV_PREFERREDTHEME) {
			// set PreferredSummaryTheme to array
			string aPoVKey;
			int prefValue;
			string langValue;
			if (childNode.GetTextValue(aPoVKey) != 0) {
				return(-5);
			}
			if (childNode.GetIntAttribute(POV_PREFERENCEVALUE, prefValue) != 0) {
				prefValue = DEFAULT_SUMMARYTHEME_PREFERENCEVALUE;
			}
			if (childNode.GetTextAttribute("xml:lang", langValue) != 0) {
				langValue = "";
			}
			POV_PreferredSummaryTheme summaryTheme;
			summaryTheme.SetThemeValue(string(aPoVKey));
			summaryTheme.SetPreferrenceValue(prefValue);
			summaryTheme.SetXMLLanguage(string(langValue));
			m_povList.push_back(summaryTheme);
		}
		else if (nodeName == POV_SUMMARYDURATION) {
			// Set SummaryDuration
			string durationValue;
			if (childNode.GetTextValue(durationValue) != 0) {
				return(-6);
			}
			SetSummaryDuration(GetDurationValue(durationValue.c_str()));
		}
		childNode = childNode.GetNextSibling() ;
	}

	return(0);
}

//----------------------------------------------------------------------------
long
POV_BrowsingQuery::GetDurationValue(const char * durString)
{
	int i ;
	for(i = 0 ; durString[i] != '\0' ; i++)
	{
		if(durString[i] == 'T')
		{
			break ;
		}
	}
	if(i >= strlen(durString))
	{
		return(-1) ;
	}
	
	unsigned long    hour, minute, second ;
	hour = minute = second = 0 ;
	char            c ;
	char            valStr[10] ;
	int             vid = 0 ;
	while((c = durString[++i]) != '\0')
	{
		if(isdigit(c)){
			valStr[vid++] = c ;
		}
		else if(c == 'H')
		{
			if(vid > 0){
				valStr[vid] = '\0' ;
				hour = atol(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'M')
		{
			if(vid > 0){
				valStr[vid] = '\0' ;
				minute = atol(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'S')
		{
			if(vid > 0){
				valStr[vid] = '\0' ;
				second = atol(valStr) ;
				vid = 0 ;
			}
		}
		else
		{
			return(-1) ;
		}
	}
	return(((hour * 60) + minute) * 60 + second) ;
}

//----------------------------------------------------------------------------
int 
POV_BrowsingQuery::GetPreferredSummaryThemeLength(void)
{
	return(m_povList.size()) ;
}

//----------------------------------------------------------------------------
std::vector<POV_PreferredSummaryTheme>
POV_BrowsingQuery::GetPreferredSummaryTheme()
{
	return(m_povList) ;
}

//----------------------------------------------------------------------------
void
POV_BrowsingQuery::SetSummaryDuration(long newValue)
{
	m_summaryDuration = newValue ;
}

//----------------------------------------------------------------------------
long
POV_BrowsingQuery::GetSummaryDuration()
{
	return(m_summaryDuration) ;
}

//=============================================================================
POV_PreferredSummaryTheme::POV_PreferredSummaryTheme()
{
	m_prefValue = DEFAULT_SUMMARYTHEME_PREFERENCEVALUE ;
}

//----------------------------------------------------------------------------
POV_PreferredSummaryTheme::~POV_PreferredSummaryTheme()
{
}

//----------------------------------------------------------------------------
void
POV_PreferredSummaryTheme::SetThemeValue(string newThemeValue)
{
	m_themeValue = newThemeValue ;
}

//----------------------------------------------------------------------------
string
POV_PreferredSummaryTheme::GetThemeValue()
{
	return(string(m_themeValue)) ;
}

//----------------------------------------------------------------------------
void
POV_PreferredSummaryTheme::SetPreferrenceValue(int newPrefValue)
{
	if(newPrefValue < LOWERBOUND_OF_PREFERENCEVALUE)
	{
		m_prefValue = LOWERBOUND_OF_PREFERENCEVALUE ;
	}
	else if(newPrefValue > UPPERBOUND_OF_PREFERENCEVALUE)
	{
		m_prefValue = UPPERBOUND_OF_PREFERENCEVALUE ;
	}
	else
	{
		m_prefValue = newPrefValue ;
	}
}

//----------------------------------------------------------------------------
int
POV_PreferredSummaryTheme::GetPreferrenceValue()
{
	return(m_prefValue) ;
}

//----------------------------------------------------------------------------
void
POV_PreferredSummaryTheme::SetXMLLanguage(string newLangValue)
{
	m_langValue = newLangValue ;
}

//----------------------------------------------------------------------------
string
POV_PreferredSummaryTheme::GetXMLLanguage()
{
	return(string(m_langValue)) ;
}
