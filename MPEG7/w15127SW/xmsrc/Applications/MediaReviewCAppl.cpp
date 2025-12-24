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
// Applicable File Name:  MediaReviewCAppl.cpp
//

/**********************************************************************
Files Modified
==============
XmManager/XMMain.cpp
Applications/ClientApplications.h
SearchUtilities/SearchUtilities.h

Files Created
=============
Applications/MediaReviewCAppl.cpp
Applications/MediaReviewCAppl.h
SearchUtilities/MediaReviewSearch.h
SearchUtilities/MediaReviewSearch.cpp

Sample XML files
================
testset/listfile - 
	lists the program description files, one per line
testset/program1.xml, program2.xml, progra,3.xml - 
	program description files including the MediaReview DSs
testset/MR-preference-1.xml, MR-preference-2.xml
	user specified MediaReviewPreferences expressed as MediaReviewPreferenceDSs

Similarity Matching
===================
Success = 1
No match  = 0
A successful match is when the user specified mediaReview Rating Value is satisfied by the
program's Rating Value (program's Rating Value is better than the user's Rating Value on the
specified Rating Scale defined by BestRating and WorstRating). 

Output
======
A listing of the programs along with the results of the similarity matching indicating
if they satisfied the user's desired rating for the MediaReview DS.

Notes
======
1. The query is assumed to have one reviewer only.
2. Each program is assumed to have only 1 MediaReview.

**********************************************************************/

#include <stdio.h>
#include <momusys.h>

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>

//#include <dom/DOM.hpp>
//#include <util/PlatformUtils.hpp>		// Initiliase XML


using namespace XM;

const UUID MediaReviewClient::myID = UUID("");
const char *MediaReviewClient::myName = "MediaReview DS Client";

MediaReviewClient::MediaReviewClient():
m_RamDB(),
m_NoOfElements(0),
m_DBProgramDS(0)
{
}

//----------------------------------------------------------------------------
MediaReviewClient::~MediaReviewClient()
{
}

//----------------------------------------------------------------------------
unsigned long MediaReviewClient::Open(char *ListFile, char *DescriptionFile)
{

	// Generic Description Scheme Interface
 	GenericDSInterfaceABC *theDescriptionInterface;

	// Generic Coding Scheme
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC  *theCSInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------

	// Get coding scheme interface
	theCS = new GenericDSCS;
	theCSInterface = theCS->GetInterface();

	// ---------------------------------------------------------------------------------------
	// Read database in ListFile
	// ---------------------------------------------------------------------------------------

	// Read filename data base
	m_RamDB.CreateDatabase(ListFile);
	m_NoOfElements=m_RamDB.GetNoOfElements();

	m_DBProgramDS =  new GenericDS [m_NoOfElements];

	for (int i=0; i<m_NoOfElements; i++)
	{

		// Set the description file
		char XMLFile[MAXMEDIANAMELEN];		// Description XML File
		strcpy(XMLFile, m_RamDB.GotoEntry(i)->GetFilename());

		// Connect description file to coding scheme
		theCSInterface->SetDecoderFileName(XMLFile);

		// Get ProgramDS interface
		theDescriptionInterface = m_DBProgramDS[i].GetInterface();

		// Connect Generic DS to coding schemes
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);

		// Decode Generic DS
		theCSInterface->StartDecode();
		if ( theDescriptionInterface->isNull() )
			std::cerr << "Could not decode " << XMLFile << std::endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MediaReviewClient::Start(char *Query,int NoOfMatches)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	// List of best matches
	MatchList theMatchList;

	// Descriptors Schemes for query and elements in database
	GenericDS theDescription;
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	// Generic Coding Scheme for query only
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	// Search Tool
	MediaReviewSearchTool *theSearchTool;
	MediaReviewSearchInterfaceABC *theSearchInterface;


	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	// Connect DS to SearchTool
	theSearchTool = new MediaReviewSearchTool();
	theSearchTool->SetQueryDSInterface(theDescriptionInterface);
	theSearchInterface = theSearchTool->GetInterface();

	// Prepare matchlist
	if (NoOfMatches> (int)(m_RamDB.GetNoOfElements())) {
	  NoOfMatches = (int)(m_RamDB.GetNoOfElements());
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	// ---------------------------------------------------------------------------------------
	// Build query
	// ---------------------------------------------------------------------------------------

	// Connect description file to coding scheme
	theCSInterface->SetDecoderFileName(Query);

	// Decode Generic DS
	theCSInterface->StartDecode();

	// ---------------------------------------------------------------------------------------
	// Match query with each item of database
	// ---------------------------------------------------------------------------------------
	{

	int i;
	for (i=0; i<m_NoOfElements; i++)
	{
		// Get Program DS interface
		DBDescriptionInterface=m_DBProgramDS[i].GetInterface();

		// Connect reference description scheme to search tool
		theSearchInterface->SetRefDSInterface(DBDescriptionInterface);

		// sort result into list of best matches
		theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
	}
	}

	// ---------------------------------------------------------------------------------------
	// Print results of the query matching
	// ---------------------------------------------------------------------------------------
	fprintf(stdout, "\nList of results: (distance, item)\n");

	{
	  int i;
	  for (i=0; i<NoOfMatches ; i++) {
	    if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	    //			fprintf(stdout,"%.0f\t%s\n",theMatchList.GetDistanceValue(i),
	    //				m_RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
	    fprintf(stdout,"%s\t%.0f\n",
		    m_RamDB.GotoEntry(theMatchList.
				      GetDBEntryNo(i))->GetFilename(),
		    theMatchList.GetDistanceValue(i));
	  }
	  //	  fprintf(stdout,"-------------------------------------------------------------------------\n");
		fflush(stdout);
	}

	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
}

//----------------------------------------------------------------------------
unsigned long MediaReviewClient::Stop()
{
	fprintf(stderr,"WARNING(MediaReviewClient): Stop() member function not Implemented.\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MediaReviewClient::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBProgramDS) delete [] m_DBProgramDS;
	
	m_NoOfElements = 0;
	m_DBProgramDS = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MediaReviewClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaReviewClient::GetName(void)
{ return myName; }










				




