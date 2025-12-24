/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee - LG Electronics Institute of Technology
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
// Copyright (c) 2000 - .
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchingHintCAppl.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <momusys.h>
//#include <vopio.h>
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
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
//#include "MatchingHintCAppl.h"		
//#include "MatchingHintSearch.h"		

#include <iostream>
#include <fstream>


using namespace XM;

const UUID MatchingHintClient::myID = UUID("");
const char *MatchingHintClient::myName = "MatchingHintCAppl";

//=============================================================================
MatchingHintClient::MatchingHintClient():
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
MatchingHintClient::~MatchingHintClient()
{
}

//----------------------------------------------------------------------------
unsigned long MatchingHintClient::Open(char *ListFile, char *DescriptionFile)
{

	// Generic Description Scheme Interface
 	GenericDSInterfaceABC *theDescriptionInterface;

	// Generic Coding Scheme
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

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
	NoOfElements=m_RamDB.GetNoOfElements();

	// Create in memory decompressed database
	// allocate memories
	m_DBDescription =
	  new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++)
	{
		// Set the description file
		char XMLFile[MAXMEDIANAMELEN];		// Description XML File
		strcpy(XMLFile, m_RamDB.GotoEntry(i)->GetFilename());
		strcat(XMLFile, ".dir/");
		strcat(XMLFile, "MatchingHintIn.xml");

		// Connect description file to coding scheme
		theCSInterface->SetDecoderFileName(XMLFile);

		// Get ProgramDS interface
		theDescriptionInterface = m_DBDescription[i].GetInterface();

		// Connect Generic DS to coding schemes
		theCSInterface->
		  SetDescriptionInterface(theDescriptionInterface);

		// Decode Generic DS
		theCSInterface->StartDecode();
		if ( theDescriptionInterface->isNull() )
			std::cerr << "Could not decode " << XMLFile << std::endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MatchingHintClient::Start(char *Query,int NoOfMatches)
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
	MatchingHintSearchTool *theSearchTool;
	MatchingHintSearchInterfaceABC *theSearchInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	// Connect DS to SearchTool
	theSearchTool = new MatchingHintSearchTool(theDescriptionInterface);
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
	strcat(Query, ".dir/");
	strcat(Query, "MatchingHintIn.xml");
	theCSInterface->SetDecoderFileName(Query);

	// Decode Generic DS
	theCSInterface->StartDecode();

	// Set this query into the search tool
	theSearchInterface->
	  SetQueryDescriptionInterface(theDescriptionInterface);

	// ---------------------------------------------------------------------------------------
	// Match query with each item of database
	// ---------------------------------------------------------------------------------------
	{

	int i;
	for (i=0; i<NoOfElements; i++)
	{
#ifdef VERBOSE
		cout << "Getting distance for : " << m_RamDB.GotoEntry(i)->GetFilename() << endl;
#endif

		// Get DS interface
		DBDescriptionInterface=
			m_DBDescription[i].GetInterface();

		// Connect reference description scheme to search tool
		theSearchInterface->
		  SetRefDescriptionInterface(DBDescriptionInterface);
	
		// sort result into list of best matches
		theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
	}
	}


	// ---------------------------------------------------------------------------------------
	// Print results of the query matching
	// ---------------------------------------------------------------------------------------
	{
		int i;
		for (i=0; i<NoOfMatches ; i++)
		{
			if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
			printf("%s\t",m_RamDB.
			       GotoEntry(theMatchList.GetDBEntryNo(i))->
			       GetFilename());
			printf("%f\t%s\n",theMatchList.GetDistanceValue(i),
				m_RamDB.
			       GotoEntry(theMatchList.GetDBEntryNo(i))->
			       GetFilename());
		}
		fflush(stdout);
	}

	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long MatchingHintClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MatchingHintClient::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MatchingHintClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintClient::GetName(void)
{ return myName; }










				



