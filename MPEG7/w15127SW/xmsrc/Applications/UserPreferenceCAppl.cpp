////////////////////////////////////////////////////////////////////////
//
// UserPreferenceCAppl.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2003
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


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
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"

#include <iostream>
#include <fstream>


using namespace XM;
using namespace std;

const UUID UserPreferenceClient::myID = UUID("");
const char *UserPreferenceClient::myName = "User Preference DS Client";

//=============================================================================
UserPreferenceClient::UserPreferenceClient():
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
UserPreferenceClient::~UserPreferenceClient()
{
}

//----------------------------------------------------------------------------
unsigned long UserPreferenceClient::Open(char *ListFile, char *DescriptionFile)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------
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

	// The DS interface (theInterface) is set within the loop


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
			cerr << "Could not decode " << XMLFile << endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long UserPreferenceClient::Start(char *Query,int NoOfMatches)
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
	UserPreferenceSearchTool *theSearchTool;
	UserPreferenceSearchInterfaceABC *theSearchInterface;


	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	theCSInterface->SetDecoderFileName(Query);
	theCSInterface->StartDecode();

	// Connect DS to SearchTool
	theSearchTool = new UserPreferenceSearchTool(theDescriptionInterface);
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
////TODO Must build query by SetDecoderFileName and StartDecode, BEFORE constructing SearchTool, because construction also sets the DSinterface into the searchTool (which expects a valid DOM loaded in the DS)	theCSInterface->SetDecoderFileName(Query);

	// Connect Generic DS to coding schemes
//	theCSInterface->SetDescriptorInterface(theDescriptionInterface);

	// Decode Generic DS
////TODO	theCSInterface->StartDecode();

	// Set this query into the search tool
//	theSearchInterface->SetQueryDSInterface(theDescriptionInterface);

	// ---------------------------------------------------------------------------------------
	// Match query with each item of database
	// ---------------------------------------------------------------------------------------
	{

	int i;
	double d;
	for (i=0; i<NoOfElements; i++)
	{
#ifdef VERBOSE
		cout << "Getting distance for : " << m_RamDB.GotoEntry(i)->GetFilename() << endl;
#endif

		// Get Program DS interface
		DBDescriptionInterface=
			m_DBDescription[i].GetInterface();

		// Connect reference description scheme to search tool
		theSearchInterface->
			SetRefDescriptionInterface(DBDescriptionInterface);

		d = theSearchInterface->GetDistance();

		// sort result into list of best matches
		theMatchList.InsertMatch(i, d);

#ifdef VERBOSE
		cout << "Distance (" << m_RamDB.GotoEntry(i)->GetFilename() << ") :" << d << endl;
#endif
	}
	}


	// ---------------------------------------------------------------------------------------
	// Print results of the query matching
	// ---------------------------------------------------------------------------------------
	fprintf(stdout, "\nList of results: (distance, item)\n");

	{
		int i;
		for (i=0; i<NoOfMatches ; i++)
		{
			if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
			fprintf(stdout,"%.1f\t%s\n",theMatchList.GetDistanceValue(i),
				m_RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
		}
		fprintf(stdout,"-------------------------------------------------------------------------\n");
		fflush(stdout);
	}

	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long UserPreferenceClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long UserPreferenceClient::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& UserPreferenceClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *UserPreferenceClient::GetName(void)
{ return myName; }










				



