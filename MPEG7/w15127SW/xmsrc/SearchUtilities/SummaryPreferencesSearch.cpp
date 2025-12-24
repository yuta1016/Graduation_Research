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
 * SummaryPreferencesSearch.cpp  *
 *********************************/

#include <stdio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ClientApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
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

const UUID SummaryPreferencesSearchInterface::myID = UUID("") ;
const char *SummaryPreferencesSearchInterface::myName = "SummaryPreferencesSearchInterface" ;

const UUID SummaryPreferencesSearchTool::myID = UUID("") ;
const char *SummaryPreferencesSearchTool::myName = "SummaryPreferencesSearchTool" ;

const UUID SummaryPreferencesSearchInterfaceABC::myID = UUID() ;


//=============================================================================
SummaryPreferencesSearchInterfaceABC::SummaryPreferencesSearchInterfaceABC()
{
	SetInterfaceID(myID) ;
}

//=============================================================================
SummaryPreferencesSearchInterface::
SummaryPreferencesSearchInterface(SummaryPreferencesSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID) ;
}

//----------------------------------------------------------------------------
SummaryPreferencesSearchInterface::~SummaryPreferencesSearchInterface()
{
}

//----------------------------------------------------------------------------
void SummaryPreferencesSearchInterface::destroy(void)
{
  delete m_SearchTool ;
}

//----------------------------------------------------------------------------
int SummaryPreferencesSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC  *aRefGenericDSInterface)
{
  return(m_SearchTool->SetRefDescriptionInterface(aRefGenericDSInterface)) ;
}

//----------------------------------------------------------------------------
int SummaryPreferencesSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface)
{
	return(m_SearchTool->SetQueryDescriptionInterface(aQueryInterface)) ;
}

//----------------------------------------------------------------------------
int SummaryPreferencesSearchInterface::SetQuery(void)
{
  return(m_SearchTool->SetQuery()) ;
}

//----------------------------------------------------------------------------
const UUID & SummaryPreferencesSearchInterface::GetInterfaceID(void)
{ 
	return(myID) ;
}

//----------------------------------------------------------------------------
const char * SummaryPreferencesSearchInterface::GetName(void)
{
	return(myName) ;
}

//----------------------------------------------------------------------------
double
SummaryPreferencesSearchInterface::GetDistance(void)
{
	return(m_SearchTool->GetDistance()) ;
}

//----------------------------------------------------------------------------
GenericDS *
SummaryPreferencesSearchInterface::GetFilteredSegment(void)
{
	return(m_SearchTool->GetFilteredSegment()) ;
}

//=============================================================================
SummaryPreferencesSearchTool::SummaryPreferencesSearchTool():
m_Interface(this),
m_RefDSInterface(0),
m_QueryDSInterface(0),
m_Query(0)
{
	SetExposedInterface(&m_Interface) ;
}

//----------------------------------------------------------------------------
SummaryPreferencesSearchTool::
SummaryPreferencesSearchTool(GenericDSInterfaceABC *aQueryInterface):
m_Interface(this),
m_RefDSInterface(0),
m_Query(0)
{
	SetExposedInterface(&m_Interface) ;
	SetQueryDescriptionInterface(aQueryInterface) ;
}

//----------------------------------------------------------------------------
SummaryPreferencesSearchTool::~SummaryPreferencesSearchTool()
{
	if(m_Query) delete m_Query ;
}

//----------------------------------------------------------------------------
const UUID& SummaryPreferencesSearchTool::GetObjectID(void)
{
	return(myID) ;
}

//----------------------------------------------------------------------------
const char *SummaryPreferencesSearchTool::GetName(void)
{
	return(myName) ;
}

//----------------------------------------------------------------------------
bool SummaryPreferencesSearchTool::SupportsPush(void)
{
	return(true) ;
}

//----------------------------------------------------------------------------
bool SummaryPreferencesSearchTool::SupportsPull(void)
{
	return(true) ;
}

//----------------------------------------------------------------------------
SummaryPreferencesSearchInterfaceABC *
SummaryPreferencesSearchTool::GetInterface(void)
{
	return(&m_Interface) ;
}

//----------------------------------------------------------------------------
int SummaryPreferencesSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC *aRefGenericDSInterface)
{
	assert(aRefGenericDSInterface) ;
	m_RefDSInterface = (GenericDSInterface *)aRefGenericDSInterface ;
	return(0) ;
}

//----------------------------------------------------------------------------
int SummaryPreferencesSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface)
{
	assert(aQueryInterface) ;
	m_QueryDSInterface = (GenericDSInterface *)aQueryInterface ;
	return(0) ;
}

//----------------------------------------------------------------------------
int
SummaryPreferencesSearchTool::SetQuery(void)
{
	if (m_Query) delete m_Query;
	m_Query = new SPRBrowsingQuery;

	return(m_Query -> ExtractQueryFromDS(m_QueryDSInterface));
}

//----------------------------------------------------------------------------
double
SummaryPreferencesSearchTool::GetDistance(void)
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
bool
SummaryPreferencesSearchTool::SegmentFiltering(void)
{
	assert(m_RefDSInterface) ;

	if(!m_Query)
	{// Query is not built yet.
		if((!m_QueryDSInterface) || (SetQuery() != 0))
		{ // QueryDSInterface is not set yet or failed to build Query
				return(false) ;
		}
	}

	// Set Result Segment DS
	m_FilteredSegment = GenericDS::CreateDescriptionRoot() ;

	// Variable, interface declare
	GenericDS            theNowNode ;
	string               nodeName ;
	GenericDSInterface * theFilteredSegmentInterface ;
	
	// Check PreferredSummaryTheme array
	if(m_Query->GetPreferredSummaryThemeLength() <= 0)
	{
		cerr << "PointOfView List is null\n" ;
		return(false) ;
	}

	// Get Reference SegmentDS and Copy all node to FilteredSegment
	theFilteredSegmentInterface = (GenericDSInterface *)m_FilteredSegment.GetInterface() ;
	CopyNode(theFilteredSegmentInterface, m_RefDSInterface) ;

	// Check if this is really AudioVisualSegmentDS ?
	// Check Mpeg7Main Node
	theNowNode   = m_FilteredSegment.GetFirstChild();
	nodeName     = theNowNode.GetDSName();
	if(nodeName != MPEG7MAIN_ELEMENT) {
		return(false);
	}
	// Check ContentDescription Node
	theNowNode   = theNowNode.GetFirstChild();
	nodeName     = theNowNode.GetDSName();
	if(nodeName != CONTENTDESCRIPTION) {
		return(false);
	}
	// Check if type attribute value is ContentEntityDescriptionType
	string attValue;
	if (theNowNode.GetTextAttribute(CDESC_TYPE_ATTR, attValue) != 0) {
		return(false);
	}
	if(attValue != CDESC_CENTITY_TYPE) {
		return(false);
	}

	// Check AudioVisualContent
	theNowNode   = theNowNode.GetFirstChild();
	nodeName     = theNowNode.GetDSName();
	if(nodeName != AUDIOVISUAL_CONTENT)
	{
		return(false);
	}

	// Filtering of AudioVisualSegment
	GenericDS theParentNode = theNowNode; // AudioVisualContent Node
	theNowNode = theParentNode.GetFirstChild();
	while (!(theNowNode.isNull())) {
		GenericDS theNextNode = theNowNode.GetNextSibling();
		int status = ExecuteFiltering(theNowNode);
		if (status == -1) {
			return(false);
		}
		else if (status == 1) {
			theParentNode.RemoveChild(theNowNode);
		}
		theNowNode = theNextNode;
	}

	// Change attribute valuses of Mpeg7Main element
	// Get Root Element
	theNowNode = m_FilteredSegment.GetFirstChild() ;

	return(true) ;
}

//----------------------------------------------------------------------------
int
SummaryPreferencesSearchTool::ExecuteFiltering(GenericDS & theNowNode)
{
	int    fstatus ;
	string nodeName = theNowNode.GetDSName() ;
	if((nodeName == AUDIOVISUAL_SEGMENT) || (nodeName == AUDIOVISUAL_ELEMENT))
	{	// Filtering AudioVisualSegment Node
		// For SegmentDecomposition node
		std::vector<GenericDS> segDecomps = theNowNode.GetAllDescriptions(AV_SEGMENTDECOMP);

		bool remainSD = false;
		for (int i = 0; i < (int) segDecomps.size(); i++) {
			// for the children AudioVisualSegments of SegmentDecomposition
			GenericDS theNowSegmentDecomp = segDecomps[i];
			std::vector<GenericDS> objectSegs = theNowSegmentDecomp.GetAllDescriptions(AUDIOVISUAL_SEGMENT);
			bool remainVS = false;
			for (int j = 0; j < (int) objectSegs.size(); j++) {
			GenericDS theNowObjectSegment = objectSegs[j];
				fstatus = ExecuteFiltering(theNowObjectSegment);
				if (fstatus == -1) {
					return(-1);
				}
				else if(fstatus == 1) {
					theNowSegmentDecomp.RemoveChild(theNowObjectSegment) ;
					theNowSegmentDecomp.SetAttribute(AUDIOVISUAL_GAP_ATTR, "true") ;
				}
				else {
					remainVS = true;
				}
			}
			if(!remainVS)
			{
				// This SegmentDecompotion element has no child.
				theNowNode.RemoveChild(theNowSegmentDecomp) ;
			}
			else
			{
				// This SegmentDecompotion element has some children.
				remainSD = true ;
			}
		}
		if(remainSD)
		{
			// Because this AudioVisualSegment has child SegmentDecomposition, it is not filtered out.
			return(0);
		}
		// Check this AudioVisualSegment PointOfView
		if(IsThisSegmentFilteredOut(theNowNode))
		{
			// Filter out this segments
			return(1);
		}
		else {
			return(0);
		}
	}
	else
	{
		// for the node except AudioVisualSegment
		GenericDS theNowChildNode = theNowNode.GetFirstChild();
		while (!(theNowChildNode.isNull())) {
			GenericDS theNextChildNode = theNowChildNode.GetNextSibling();
			fstatus = ExecuteFiltering(theNowChildNode);
			if (fstatus == -1) {
				return(-1);
			}
			else if (fstatus == 1) {
				theNowNode.RemoveChild(theNowChildNode);
			}
			theNowChildNode = theNextChildNode ;
		}

		return(0);
	}
}

//----------------------------------------------------------------------------
bool SummaryPreferencesSearchTool::
IsThisSegmentFilteredOut(GenericDS & theNowSegment)
{
	// Return : false :: This segment is selected
	//          true  :: This segment is filtered out.
	std::vector<SPRPreferredSummaryTheme>	povList;
	std::vector<GenericDS>			vsVector;
	bool					filteredOut  = true;
	GenericDS				thePOVNode;
	string					povKeyword;
	int					nowPrefValue = 0;

	povList  = m_Query -> GetPreferredSummaryTheme();
	vsVector = theNowSegment.GetAllDescriptions(POINTOFVIEW_ELEMENT);
	for (int i = 0; i < (int) vsVector.size(); i++) {
		thePOVNode = vsVector[i];
		
		if (thePOVNode.GetTextAttribute(POV_VIEWPOINT_ATTR, povKeyword) != 0) {
			continue;
		}
		for (int j = 0; j < (int) povList.size(); j++) {
			SPRPreferredSummaryTheme	psTheme      = povList[j];
			string			themeValue   = psTheme.GetThemeValue();	
			
			if(povKeyword == themeValue)
			{
				int prefValue = psTheme.GetPreferrenceValue() ;
				if(prefValue < 0)
				{
					if(nowPrefValue < 0)
					{
						filteredOut = true ;
						nowPrefValue =
							(abs(nowPrefValue) > abs(prefValue)) ? nowPrefValue : prefValue ;
					}
					else if(nowPrefValue >= abs(prefValue))
					{
						filteredOut = false ;
					}
					else
					{
						filteredOut = true ;
						nowPrefValue = prefValue ;
					}
				}
				else // prefValue >= 0
				{
					if(nowPrefValue >= 0)
					{
						filteredOut = false ;
						nowPrefValue = (nowPrefValue > prefValue) ? nowPrefValue : prefValue ;
					}
					else if(abs(nowPrefValue) > prefValue)
					{
						filteredOut = true ;
					}
					else
					{
						filteredOut = false ;
						nowPrefValue = prefValue ;
					}
				}
			}
		}
	}
	return(filteredOut) ;
}

//----------------------------------------------------------------------------
GenericDS *
SummaryPreferencesSearchTool::GetFilteredSegment(void)
{
	return(&m_FilteredSegment) ;
}

//----------------------------------------------------------------------------
void	SummaryPreferencesSearchTool::CopyNode(GenericDSInterface * destInterface,
		GenericDSInterface * srcInterface)
{
	int i;

	// Copy Attribute
	std::vector<string> attrNames;

	attrNames = srcInterface -> GetAttributeNames();
	string  attrName;
	string  attrValue;

	for (i = 0; i < (int) attrNames.size(); i ++) {
		attrName = attrNames[i];
		if (srcInterface -> GetTextAttribute(attrName, attrValue) != 0) {
			cerr << "Unable to extract attribute value of " << attrName.c_str() << "\n";
			continue;
		}
		destInterface -> SetAttribute(attrName, attrValue);
	}

	// Copy Content
	string contentValue;
	if (srcInterface -> GetTextValue(contentValue) == 0) {
		destInterface -> SetValue(contentValue);
	}

	// Copy Child Node
	GenericDS            srcChildNode;
	GenericDS            destChildNode;
	GenericDSInterface * srcChildInterface;
	GenericDSInterface * destChildInterface;
	int childLength = srcInterface -> GetNumberOfChildren();
	for (i = 0; i < childLength; i++) {
		srcChildNode = srcInterface -> GetChild(i);
		srcChildInterface = (GenericDSInterface *)srcChildNode.GetInterface();
		string nodeName = srcChildInterface -> GetDSName();
		destChildNode = destInterface -> CreateChild(nodeName);
		destChildInterface = (GenericDSInterface *)destChildNode.GetInterface();
		CopyNode(destChildInterface, srcChildInterface);
	}
}


//=============================================================================
SPRBrowsingQuery::SPRBrowsingQuery()
{
	m_summaryDuration = -1 ;
}

//----------------------------------------------------------------------------
SPRBrowsingQuery::~SPRBrowsingQuery()
{
}

//----------------------------------------------------------------------------
int
SPRBrowsingQuery::ExtractQueryFromDS(GenericDSInterface *QueryDSInterface)
{
	if (!QueryDSInterface) {
		return(-9);
	}

	GenericDSInterface	* theSummaryPreferencesDSInterface = QueryDSInterface;

	GenericDS theSummaryPreferencesDS = theSummaryPreferencesDSInterface -> GetFirstChild();

	string nodeName;
	// Traverse SummaryPreferencesDS and search SummaryPreferences
	// Is this really SummaryPreferencesDS ?
	// Search SummaryPrefereces Node
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
		return(NOT_SUMMARYPREFERENCES);
	}
	if (attValue != SPR_USERDESCRIPTION) {
		return(NOT_SUMMARYPREFERENCES);
	}

	// Check SummaryPreferences Node
	theNowNode = theNowNode.GetFirstChild();
	nodeName = theNowNode.GetDSName();
	if (nodeName != SUMMARYPREFERENCE_ELEMENT) {
		return(-1);
	}

	// search BrowsingPreferences
	nodeName = SPR_BROWSINGPREF;
	GenericDS childNode;
	childNode = theNowNode.GetDescription(nodeName);
	if (childNode.isNull()) {
		return(-3);
	}

	// search SummaryPreferences
	nodeName = SPR_SUMMARYPREF;
	childNode = childNode.GetDescription(nodeName);
	if (childNode.isNull()) {
		return(-4);
	}

	// Extract Value
	childNode = childNode.GetFirstChild();
	while (!(childNode.isNull())) {
		nodeName = childNode.GetDSName();
		if (nodeName == SPR_PREFERREDTHEME) {
			// set PreferredSummaryTheme to array
			string aPoVKey;
			int prefValue;
			string langValue;
			if (childNode.GetTextValue(aPoVKey) != 0) {
				return(-5);
			}
			if (childNode.GetIntAttribute(SPR_PREFERENCEVALUE, prefValue) != 0) {
				prefValue = DEFAULT_SUMMARYTHEME_PREFERENCEVALUE;
			}
			if (childNode.GetTextAttribute("xml:lang", langValue) != 0) {
				langValue = "";
			}
			SPRPreferredSummaryTheme summaryTheme;
			summaryTheme.SetThemeValue(string(aPoVKey));
			summaryTheme.SetPreferrenceValue(prefValue);
			summaryTheme.SetXMLLanguage(string(langValue));
			m_povList.push_back(summaryTheme);
		}
		else if (nodeName == SPR_SUMMARYDURATION) {
			// Set SummaryDuration
			string durationValue;
			if (childNode.GetTextValue(durationValue) != 0) {
				return(-6);
			}
			SetSummaryDuration(GetDurationValue(durationValue.c_str()));
		}
		childNode = childNode.GetNextSibling();
	}
	return(0);
}

//----------------------------------------------------------------------------
long
SPRBrowsingQuery::GetDurationValue(const char * durString)
{
	int i;

	for (i = 0; durString[i] != '\0'; i++) {
		if (durString[i] == 'T') {
			break;
		}
	}
	if (i >= (int) strlen(durString)) {
		return(-1);
	}
	
	unsigned long    hour, minute, second;
	hour = minute = second = 0;
	char            c;
	char            valStr[10];
	int             vid = 0;
	while ((c = durString[++i]) != '\0') {
		if (isdigit(c)) {
			valStr[vid++] = c;
		}
		else if (c == 'H') {
			if (vid > 0) {
				valStr[vid] = '\0';
				hour = atol(valStr);
				vid = 0;
			}
		}
		else if (c == 'M') {
			if (vid > 0) {
				valStr[vid] = '\0';
				minute = atol(valStr);
				vid = 0;
			}
		}
		else if (c == 'S') {
			if (vid > 0) {
				valStr[vid] = '\0';
				second = atol(valStr);
				vid = 0;
			}
		}
		else {
			return(-1);
		}
	}

	return(((hour * 60) + minute) * 60 + second);
}

//----------------------------------------------------------------------------
int 
SPRBrowsingQuery::GetPreferredSummaryThemeLength(void)
{
	return(m_povList.size()) ;
}

//----------------------------------------------------------------------------
std::vector<SPRPreferredSummaryTheme>
SPRBrowsingQuery::GetPreferredSummaryTheme()
{
	return(m_povList) ;
}

//----------------------------------------------------------------------------
void
SPRBrowsingQuery::SetSummaryDuration(long newValue)
{
	m_summaryDuration = newValue ;
}

//----------------------------------------------------------------------------
long
SPRBrowsingQuery::GetSummaryDuration()
{
	return(m_summaryDuration) ;
}

//=============================================================================
SPRPreferredSummaryTheme::SPRPreferredSummaryTheme()
{
	m_prefValue = DEFAULT_SUMMARYTHEME_PREFERENCEVALUE ;
}

//----------------------------------------------------------------------------
SPRPreferredSummaryTheme::~SPRPreferredSummaryTheme()
{
}

//----------------------------------------------------------------------------
void
SPRPreferredSummaryTheme::SetThemeValue(string newThemeValue)
{
	m_themeValue = newThemeValue ;
}

//----------------------------------------------------------------------------
string
SPRPreferredSummaryTheme::GetThemeValue()
{
	return(string(m_themeValue)) ;
}

//----------------------------------------------------------------------------
void
SPRPreferredSummaryTheme::SetPreferrenceValue(int newPrefValue)
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
SPRPreferredSummaryTheme::GetPreferrenceValue()
{
	return(m_prefValue) ;
}

//----------------------------------------------------------------------------
void
SPRPreferredSummaryTheme::SetXMLLanguage(string newLangValue)
{
	m_langValue = newLangValue ;
}

//----------------------------------------------------------------------------
string
SPRPreferredSummaryTheme::GetXMLLanguage()
{
	return(string(m_langValue)) ;
}
