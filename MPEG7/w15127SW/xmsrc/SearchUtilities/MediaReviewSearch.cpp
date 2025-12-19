// This software module was originally developed by
//
// Gandhimathi Vaithilingam, Philips Research Laboratories, U.S.A.
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
// Copyright is not released for non MPEG-7 conforming products. Philips
// Research Laboratories retains the full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MediaReviewSearch.cpp
//

#include <stdio.h>
#include <string.h>
#include <float.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	
using namespace XM;


const UUID MediaReviewSearchInterface::myID = UUID("");
const char *MediaReviewSearchInterface::myName = "MediaReview Matching Interface";

const UUID MediaReviewSearchTool::myID = UUID("");
const char *MediaReviewSearchTool::myName = "MediaReview Matching Tool";

const UUID MediaReviewSearchInterfaceABC::myID = UUID();

//=============================================================================
MediaReviewSearchInterfaceABC::MediaReviewSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MediaReviewSearchInterface::MediaReviewSearchInterface(MediaReviewSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MediaReviewSearchInterface::~MediaReviewSearchInterface()
{
}

//----------------------------------------------------------------------------
void MediaReviewSearchInterface::destroy()
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int MediaReviewSearchInterface::
SetRefDSInterface(GenericDSInterfaceABC
		       *RefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDSInterface(RefGenericDSInterface);
}

//----------------------------------------------------------------------------
int MediaReviewSearchInterface::
SetQueryDSInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDSInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double MediaReviewSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& MediaReviewSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaReviewSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
MediaReviewSearchTool::MediaReviewSearchTool():
m_Interface(this),
m_RefDSInterface(0),
m_QueryDSInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//sth second constructor is missing allowing to connect the query description

//----------------------------------------------------------------------------
MediaReviewSearchTool::~MediaReviewSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& MediaReviewSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaReviewSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MediaReviewSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MediaReviewSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MediaReviewSearchTool::
SetRefDSInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  m_RefDSInterface = aRefGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  MediaReviewSearchTool::
SetQueryDSInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  m_QueryDSInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double MediaReviewSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	
	double distance;

	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------

    if (!m_RefDSInterface) return DBL_MAX;
    if (!m_QueryDSInterface) return DBL_MAX;

	if (strcmp(m_RefDSInterface->GetName(),
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
//#ifdef USEXML

	distance = SimilarityMatch();
	// use these two data members 
	// m_RefDSInterface, m_QueryDSInterface
//#else

//	// Read DOM documents for query and database items
//	docRef = m_RefDSInterface->GetDocument();
//	docQuery = m_QueryDSInterface->GetDocument();
//
//	distance = SimilarityMatch(docRef, docQuery);
//
//#endif  /* USEXML */ 


	return distance;
}

//----------------------------------------------------------------------------
// Created by Vincent Hsu
//
double MediaReviewSearchTool::SimilarityMatch()
{
	/*
	The SimilarityMatch returns 1 if the MediaReviewDS of the program in docRef
	satisfies the preference expressed in MediaReviewPreferenceDs in docQuery, 
	else it returns 0.
	Success = 1
	Failure = 0;
	*/

	double distance;
	string refReviewer, queryReviewer;
	int worstRating, bestRating, refRatingVal, queryRatingVal;
		
	// get the reviewer name from the query and the reference documents
	//refReviewer = getReviewerName(docRef);
	//queryReviewer = getReviewerName(docQuery);
      refReviewer = getReviewerName(m_RefDSInterface);   //to-do
      queryReviewer = getReviewerName(m_QueryDSInterface); //to-do


	if (!refReviewer.compare(queryReviewer))
	{
		//reviewers match, proceed to check rating

		// obtain the rating information for the program from the reference document
		//worstRating = getIntegerValue(docRef, "WorstRating");
		//bestRating = getIntegerValue(docRef, "BestRating");
		//refRatingVal = getIntegerValue(docRef, "RatingValue");
        //
        //  m_RefDSInterface->GetIntAttribute("WorstRating", worstRating);
        //  m_RefDSInterface->GetIntAttribute("BestRating", bestRating);
        //  m_RefDSInterface->GetIntAttribute("RatingValue", refRatingVal);

		getRatingSchemeIntegerValue(m_RefDSInterface,
			    worstRating, bestRating);
        refRatingVal = getIntegerValue(m_RefDSInterface, "RatingValue");



		// obtain the rating preference from query
		//queryRatingVal = getIntegerValue(docQuery, "RatingValue");
        //  m_QueryDSInterface->GetIntAttribute("RatingValue", queryRatingVal);
          queryRatingVal = getIntegerValue(m_QueryDSInterface, "RatingValue");



		// It is checked if the program's rating falls favorably on the 
		// rating scale compared to the prefererd rating.
		if ( (queryRatingVal-bestRating)* (queryRatingVal-worstRating) <= 0) 
		{
				if ((bestRating > worstRating) && (refRatingVal >= queryRatingVal))
					distance = 1.0;
				if ((worstRating > bestRating) && (refRatingVal <= queryRatingVal))
					distance = 1.0;
		}
		else 
			distance = 0;
	}
	else
		distance = 0; //no match
	
	return distance;
}

//----------------------------------------------------------------------------
//double MediaReviewSearchTool::SimilarityMatch(DOM_Document docRef, DOM_Document docQuery)
//{
//	/*
//	The SimilarityMatch returns 1 if the MediaReviewDS of the program in docRef
//	satisfies the preference expressed in MediaReviewPreferenceDs in docQuery, 
//	else it returns 0.
//	Success = 1
//	Failure = 0;
//	*/
//
//	double distance;
//	string refReviewer, queryReviewer;
//	int worstRating, bestRating, refRatingVal, queryRatingVal;
//		
//	// get the reviewer name from the query and the reference documents
//	refReviewer = getReviewerName(docRef);
//	queryReviewer = getReviewerName(docQuery);
//
//	if (!refReviewer.compare(queryReviewer))
//	{
//		//reviewers match, proceed to check rating
//
//		// obtain the rating information for the program from the reference document
//		worstRating = getIntegerValue(docRef, "WorstRating");
//		bestRating = getIntegerValue(docRef, "BestRating");
//		refRatingVal = getIntegerValue(docRef, "RatingValue");
//
//		// obtain the rating preference from query
//		queryRatingVal = getIntegerValue(docQuery, "RatingValue");
//
//		// It is checked if the program's rating falls favorably on the 
//		// rating scale compared to the prefererd rating.
//		if ( (queryRatingVal-bestRating)* (queryRatingVal-worstRating) <= 0) 
//		{
//				if ((bestRating > worstRating) && (refRatingVal >= queryRatingVal))
//					distance = 1.0;
//				if ((worstRating > bestRating) && (refRatingVal <= queryRatingVal))
//					distance = 1.0;
//		}
//		else 
//			distance = 0;
//	}
//	else
//		distance = 0; //no match
//	
//	return distance;
//}
//----------------------------------------------------------------------------
// Created by Vincent Hsu
//
string MediaReviewSearchTool::getReviewerName(
	   GenericDSInterfaceABC    *aDescription)
{

  GenericDS l_DDLDescription;
  GenericDSInterfaceABC *l_DDLDescriptionInterface = NULL;
  string reviewer_name;

///////////////////////////////////////////////////////
  if (aDescription->GetDSName() == "MediaReview")  //or GetDescription()?? 
  {
    
      l_DDLDescriptionInterface = aDescription;

  }

  if (!l_DDLDescriptionInterface)
  {
    l_DDLDescription = aDescription->GetDescription("MediaReview");

    while (!l_DDLDescription.isNull())
    {
      l_DDLDescription = l_DDLDescription.GetNextSibling("MediaReview");
    }

    if (l_DDLDescription.isNull())
      return string("");

    l_DDLDescriptionInterface = l_DDLDescription.GetInterface();
  }

///////////////////////////////////////////////

    GenericDS   Reviewer_element = 
		l_DDLDescriptionInterface->GetDescription("Reviewer");
    Reviewer_element.GetTextValue(reviewer_name);



///////////////////////////////////////////////



	return reviewer_name;
}

//----------------------------------------------------------------------------
//string MediaReviewSearchTool::getReviewerName(DOM_Document& docRoot)
//{
//	/*
//	Temporary: No leading or trailing whitespaces are allowed for the FamilyName,
//	GivenName or OrganizationName tags. Ensure that the whitespaces are 
//	trimmed in the XML file. 
//	TODO: Trim leading and trailing whitespaces when extracting the Reviewer Names.
//	*/
//
//	DOM_Node node;
//	DOM_Element el;
//	DOM_NodeList reviewer_list, name_list;
//	string reviewer_name, reviewer_type, tagName, str1;
//
//	reviewer_list = docRoot.getElementsByTagName("Reviewer");
//	if ((reviewer_list.getLength())< 1 )
//	{
//		fprintf(stderr, "Invalid: No reviewers specified\n");
//		return NULL;
//	}
//			
//	node = reviewer_list.item(0).getFirstChild().getNextSibling();
//	el = (DOM_Element & ) node;
//	reviewer_type = el.getTagName().transcode();
//	if (!(reviewer_type.compare("Individual"))) // reviewer is an individual
//	{
//		reviewer_name = getTextValue(el, "GivenName");
//		reviewer_name.append(" ");
//		reviewer_name.append(getTextValue(el, "FamilyName"));
//	}
//	else if (!(reviewer_type.compare("Organization"))) // reviewer is an organization
//		reviewer_name = getTextValue(el, "OrganizationName");
//	
//	return reviewer_name;
//}
//----------------------------------------------------------------------------
// Created by Vincent Hsu
//
string MediaReviewSearchTool::getTextValue(
    GenericDSInterfaceABC *aDescription, char* tagname)
{

	string textValue, trimmedText;

    //to do

	return trimmedText;
}

//----------------------------------------------------------------------------
//string MediaReviewSearchTool::getTextValue(DOM_Element& element, char* tagname)
//{
//	DOM_NodeList list;
//	string textValue, trimmedText;
//
//	list = element.getElementsByTagName(tagname);
//	textValue = list.item(0).getFirstChild().getNodeValue().transcode();
//
//	//Trim leading and trailing white spaces from TextValue
//	// NOTE: It does not trim carriage returns
//	int begin = textValue.find_first_not_of(' ');
//	int end = textValue.find_first_of(' ', begin);
//	trimmedText = textValue.substr(begin, (end-begin));
//
//	return trimmedText;
//}
//
//
//----------------------------------------------------------------------------
// Created by Vincent Hsu
//
int MediaReviewSearchTool::getIntegerValue(
	GenericDSInterfaceABC *aDescription, char* tagName)
{

	int intValue;

    GenericDS   Rating_element = 
		aDescription->GetDescription(tagName);
    Rating_element.GetIntValue(intValue);

    



	return intValue;
}
//----------------------------------------------------------------------------
// Created by Vincent Hsu
//
void MediaReviewSearchTool::getRatingSchemeIntegerValue(
	GenericDSInterfaceABC *aDescription, int &worst, int &best)
{

//	int intValue;
	GenericDS   RatingScheme;
    GenericDSInterfaceABC *RatingSchemeInterface = NULL;

    RatingScheme = 
		aDescription->GetDescription("RatingScheme");
    RatingSchemeInterface = RatingScheme.GetInterface();


    GenericDS   Worst_element = 
		RatingSchemeInterface->GetDescription("WorstRating");
    Worst_element.GetIntValue(worst);

    
    GenericDS   Best_element = 
		RatingSchemeInterface->GetDescription("BestRating");
    Best_element.GetIntValue(best);

}

//----------------------------------------------------------------------------
//int MediaReviewSearchTool::getIntegerValue(DOM_Document& docRoot, char* tagName)
//{
//	DOM_NodeList nodeList;
//	int intValue;
//
//	nodeList = docRoot.getElementsByTagName(tagName);
//	intValue = atoi(nodeList.item(0).getFirstChild().getNodeValue().transcode());
//
//	return intValue;
//}
//
//----------------------------------------------------------------------------
MediaReviewSearchInterfaceABC *MediaReviewSearchTool::GetInterface(void)
{ return &m_Interface; }


