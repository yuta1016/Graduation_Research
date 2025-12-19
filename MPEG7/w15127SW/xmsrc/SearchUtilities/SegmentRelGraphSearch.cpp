//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SegmentRelGraphSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"


//=============================================================================

using namespace XM;


const UUID SegmentRelGraphSearchInterface::myID = UUID("");
const char *SegmentRelGraphSearchInterface::myName = "SegmentRelGraph Matching Interface";

const UUID SegmentRelGraphSearchTool::myID = UUID("");
const char *SegmentRelGraphSearchTool::myName = "SegmentRelGraph Matching Tool";

const UUID SegmentRelGraphSearchInterfaceABC::myID = UUID();


//=============================================================================
SegmentRelGraphSearchInterfaceABC::SegmentRelGraphSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
SegmentRelGraphSearchInterface::SegmentRelGraphSearchInterface(SegmentRelGraphSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SegmentRelGraphSearchInterface::~SegmentRelGraphSearchInterface()
{
}

//----------------------------------------------------------------------------
void SegmentRelGraphSearchInterface::destroy()
{  delete m_SearchTool; }

//----------------------------------------------------------------------------
int SegmentRelGraphSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *RefGenericDSInterface)
{
  return m_SearchTool->
    SetRefDescriptionInterface(RefGenericDSInterface);
}

//----------------------------------------------------------------------------
int SegmentRelGraphSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)
{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double SegmentRelGraphSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& SegmentRelGraphSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
SegmentRelGraphSearchTool::SegmentRelGraphSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentRelGraphSearchTool::SegmentRelGraphSearchTool(GenericDSInterfaceABC
						     *theParent):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
        SetQueryDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentRelGraphSearchTool::~SegmentRelGraphSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& SegmentRelGraphSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool SegmentRelGraphSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SegmentRelGraphSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SegmentRelGraphSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)
{
  m_RefDescriptionInterface = aRefGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  SegmentRelGraphSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
  m_QueryDescriptionInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double SegmentRelGraphSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	
	double distance = 0.0;

	GenericDS SegRelGraphRefDS, SegRelGraphQueryDS;
	GenericDSInterfaceABC *pSegRelGraphRefDSInt, *pSegRelGraphQueryDSInt;
	string segRelGraphRefName, segRelGraphQueryName;


	// ----------------------------------------------------------
	// Check Description schemes
	// ----------------------------------------------------------

    if (m_RefDescriptionInterface == NULL) {
		fprintf(stderr, "Null Reference DS\n");
		return FLT_MAX;
	}
    if (m_QueryDescriptionInterface == NULL) {
		fprintf(stderr, "Null Query DS\n");
		return FLT_MAX;
	}

	if (strcmp(m_RefDescriptionInterface->GetName(),
		   "GenericDSInterface")) {
	  return FLT_MAX;
	}
	if (strcmp(m_QueryDescriptionInterface->GetName(),
		   "GenericDSInterface")) {
	  return FLT_MAX;
	}

	// Read Segment Graph DS
	SegRelGraphRefDS = m_RefDescriptionInterface->GetFirstChild();
	pSegRelGraphRefDSInt = SegRelGraphRefDS.GetInterface();

	SegRelGraphQueryDS = m_QueryDescriptionInterface->GetFirstChild();
	pSegRelGraphQueryDSInt = SegRelGraphQueryDS.GetInterface();


	/* Check the elements are segment relation graphs */
	if ( m_QueryDescriptionInterface->GetNumberOfChildren()!=1 || m_RefDescriptionInterface->GetNumberOfChildren()!=1) {
		fprintf(stderr, "Lacking Segment Graph DS\n");
		return FLT_MAX;
	} else if (pSegRelGraphRefDSInt->isNull() || pSegRelGraphQueryDSInt->isNull()) {
		fprintf(stderr, "Null Segment Graph DS\n");
		return FLT_MAX;
	} else {
		// Check names of root elements
		segRelGraphRefName = pSegRelGraphRefDSInt->GetDSName();
		segRelGraphQueryName = pSegRelGraphQueryDSInt->GetDSName();
		if ( segRelGraphRefName.compare(segRelGraphQueryName) ||
					segRelGraphRefName.compare("Graph") ) {
			fprintf(stderr, "Wrong DS name\n");
			return FLT_MAX;
		}
	}


	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------
	distance = SegmentRelGraphMatching(pSegRelGraphRefDSInt, pSegRelGraphQueryDSInt);

	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
/*	fprintf( stderr, "Distance between segments of type %s = %f\n", 
			 segRelGraphTypeRef.transcode(), distance);
*/

	return distance;
}

//----------------------------------------------------------------------------
double SegmentRelGraphSearchTool::
SegmentRelGraphMatching(GenericDSInterfaceABC *pRefDSInterface,
			GenericDSInterfaceABC *pQueryDSInterface)
{
	double distance = 0.0;
	char **ppcSegRelListRef = NULL, **ppcSegRelListQuery = NULL;
	double *pdSegRelListRef = NULL, *pdSegRelListQuery = NULL;
	int iNumRelations, iNumRelsRef, iNumRelsQuery;

	int i;


	// ------------------------------------------------------------------------
	// Decode the DSs to obtain the list of segment relations
	// ------------------------------------------------------------------------

	if (SegmentRelGraphDecoding(pRefDSInterface, &ppcSegRelListRef, &iNumRelsRef) < 0) {
		fprintf(stderr, "SegmentRelGraphMatching: Error decoding Ref Segment GraphDS\n");
		distance = FLT_MAX;
	}

	if (SegmentRelGraphDecoding(pQueryDSInterface, &ppcSegRelListQuery, &iNumRelsQuery) < 0) {
		fprintf(stderr, "SegmentRelGraphMatching: Error decoding Query Segment GraphDS\n");
		distance = FLT_MAX;
	}

	iNumRelations = (iNumRelsRef < iNumRelsQuery) ? iNumRelsRef : iNumRelsQuery;


	// ------------------------------------------------------------------------
	// Transform list of string segment relations to integer vector for 
	// matching. The transformations are the following:
	//		After  = +1
	//		Start =  0
	//		Before = -1
	// ------------------------------------------------------------------------

	pdSegRelListRef = new double[iNumRelations];
	pdSegRelListQuery = new double[iNumRelations];

	if (distance < FLT_MAX) {

	for (i=0; i<iNumRelations; i++) {
		if (!strcmp(ppcSegRelListRef[i], "before")) {
			pdSegRelListRef[i] = -1.0;
		} else if (!strcmp(ppcSegRelListRef[i], "starts")) {
			pdSegRelListRef[i] = 0.0;
		} else if (!strcmp(ppcSegRelListRef[i], "after")) {
			pdSegRelListRef[i] = +1.0;
		} else {
			fprintf(stderr, "SegmentRelGraphMatching: Unknown "
				"Ref relation: %s\n", 
				ppcSegRelListRef[i]);
			distance = FLT_MAX;
		}

		if (!strcmp(ppcSegRelListQuery[i], "before")) {
			pdSegRelListQuery[i] = -1;
		} else if (!strcmp(ppcSegRelListQuery[i], "starts")) {
			pdSegRelListQuery[i] = 0;
		} else if (!strcmp(ppcSegRelListQuery[i], "after")) {
			pdSegRelListQuery[i] = +1;
		} else {
			fprintf(stderr,
				"SegmentRelGraphMatching: Unknown Query "
				"relation: %s\n",
				ppcSegRelListQuery[i]);
			distance = FLT_MAX;
		}
	}

	}

	// ------------------------------------------------------------------------
	// Print relation vectors - Testing
	// ------------------------------------------------------------------------
/*
	fprintf(stderr, "Ref relations: ");
	for (i=0; i<iNumRelations; i++) {
		fprintf(stderr, "%f ", pdSegRelListRef[i]);
	}
	fprintf(stderr, "\n");

	fprintf(stderr, "Query relations: ");
	for (i=0; i<iNumRelations; i++) {
		fprintf(stderr, "%f ", pdSegRelListQuery[i]);
	}
	fprintf(stderr, "\n");
*/

	// ------------------------------------------------------------------------
	// Calculate distance
	// ------------------------------------------------------------------------
	
	if (distance < FLT_MAX) {
		distance = L1Dist(pdSegRelListRef, pdSegRelListQuery, iNumRelations);
	}


	// ------------------------------------------------------------------------
	// Release memory
	// ------------------------------------------------------------------------

	if (ppcSegRelListRef!=NULL) {
		for (i=0; i<iNumRelations; i++) {
			if (ppcSegRelListRef[i]!=NULL) {
				delete [] ppcSegRelListRef[i];
			}
		}
		delete [] ppcSegRelListRef;
	}
	if (ppcSegRelListQuery!=NULL) {
		for (i=0; i<iNumRelations; i++) {
			if (ppcSegRelListQuery[i]!=NULL) {
				delete [] ppcSegRelListQuery[i];
			}
		}
		delete [] ppcSegRelListQuery;
	}
	if (pdSegRelListRef!=NULL) {
		delete[] pdSegRelListRef;
	}
	if (pdSegRelListQuery!=NULL) {
		delete[] pdSegRelListQuery;
	}


	return distance;
}

//----------------------------------------------------------------------------
int SegmentRelGraphSearchTool::SegmentRelGraphDecoding(GenericDSInterfaceABC *pDSInterface,
	char ***pppcSegmentRelationsList, int *piNumRelations)
{
	const int STRING_MAXSIZE = 4096;

	GenericDS RelationDS;
	GenericDSInterfaceABC *pRelationDSInt = NULL;
	char **ppcSegmentRelList = NULL;
	int iNumRel = 0;
	int iNumChild = 0;
	int i;
	string str;

	// ------------------------------------------------------------------------
	// Count number of Relation DSs
	// ------------------------------------------------------------------------

	iNumRel = pDSInterface->GetNumberOfChildren();
	for (i=0; i<iNumChild; i++) {
		RelationDS = pDSInterface->GetChild(i);
		pRelationDSInt = RelationDS.GetInterface();
		if ( pRelationDSInt->GetDSName().compare("Relation") ) {
			fprintf(stderr, "SegmentRelGraphDecoding: Unexpected DS in Segment Graph DS\n");
			return -1;	
		}
		RelationDS.GetTextAttribute("xsi:type", str);
		if ( str.compare("BinaryTemporalSegmentRelationType") ) {
			fprintf(stderr, 
				"BinaryTemporalSegmentRelation DS not found as expected in Graph DS\n");
			return -1;
		}

		iNumRel++;
	}


	// ------------------------------------------------------------------------
	// Allocate memory for segment relations
	// ------------------------------------------------------------------------

	ppcSegmentRelList = new char*[iNumRel];
	for (i=0; i<iNumRel; i++) {
		ppcSegmentRelList[i] = new char[STRING_MAXSIZE];
	}


	// ------------------------------------------------------------------------
	// Decode all the Relation DSs
	// Also check for encoding errors
	// ------------------------------------------------------------------------

	RelationDS = pDSInterface->GetFirstChild();
	pRelationDSInt = RelationDS.GetInterface();
	for(i=0; i<iNumRel; i++) {

		/* Read relation name */
		string str;
		pRelationDSInt->GetTextAttribute("name", str);
		strcpy(ppcSegmentRelList[i], str.c_str());

		/* Segment IDs could be checked for source and target */
		string target, source;
		pRelationDSInt->GetTextAttribute("source", source);
		pRelationDSInt->GetTextAttribute("target", target);

		RelationDS = pRelationDSInt->GetNextSibling();
		pRelationDSInt = RelationDS.GetInterface();
	}

	// ------------------------------------------------------------------------
	// Return data
	// ------------------------------------------------------------------------

	(*pppcSegmentRelationsList) = ppcSegmentRelList;
	(*piNumRelations) = iNumRel;


	return 0;
}

//----------------------------------------------------------------------------
double SegmentRelGraphSearchTool::L1Dist(double *pdSegRelListRef, double *pdSegRelListQuery,
										 int dim) 
{
	double dist = 0.0;

	for (int i=0; i<dim; i++) {
		double tmp = pdSegRelListRef[i]-pdSegRelListQuery[i];
		if (tmp < 0) {
			dist -= tmp;
		} else {
			dist += tmp;
		}
	}

	return dist;
}

//----------------------------------------------------------------------------
SegmentRelGraphSearchInterfaceABC *SegmentRelGraphSearchTool::GetInterface(void)
{ return &m_Interface; }

