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
// Applicable File Name:  MatchingHintSAppl.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
#endif

#include "momusys.h"
#include "vopio.h"
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
//#include "MatchingHintSAppl.h"	
//#include "MatchingHintExtraction.h"	


#include <iostream>
#include <fstream>


using namespace XM;

const UUID MatchingHintServer::myID = UUID("");
const char *MatchingHintServer::myName = "MatchingHintSAppl";

//=============================================================================
MatchingHintServer::MatchingHintServer():
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
MatchingHintServer::~MatchingHintServer()
{
}

//----------------------------------------------------------------------------
unsigned long MatchingHintServer::Open()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MatchingHintServer::Start(char *ListFile, char *Query)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------
	// Generic Description Scheme Interface
	GenericDS theDescription;
	GenericDSInterfaceABC *DBDescriptionInterface;
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

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	// List of best matches
	//MatchList theMatchList;

	// Descriptors Schemes for query and elements in database
	//+-GenericDS theDescription;
 	//+-GenericDSInterfaceABC *theDescriptionInterface;
 	//+-GenericDSInterfaceABC *DBDescriptionInterface;

	// Generic Coding Scheme for query only
	//+-GenericDSCS *theCS;
	//+-GenericDSCSInterfaceABC *theCSInterface;

	// Extraction Tool
	MatchingHintExtractionTool *theExtractionTool;
	MatchingHintExtractionInterfaceABC *theExtractionInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theDescriptionInterface = theDescription.GetInterface();

	// Get coding scheme interface
	//+-theCS = new GenericDSCS(theDescriptionInterface);
	//+-theCSInterface = theCS->GetInterface();
	theCSInterface->SetDescriptionInterface(theDescriptionInterface);

	// Connect DS to ExtractionTool
	theExtractionTool = new MatchingHintExtractionTool();
	theExtractionInterface = theExtractionTool->GetInterface();

	// Connect description file to coding scheme
	char Out[MAXMEDIANAMELEN];

	strcpy(Out, Query);
	strcat(Query, ".dir/");
	strcat(Query, "MatchingHintIn.xml");
	strcat(Out, ".dir/");
	strcat(Out, "MatchingHintOut.xml");

	theCSInterface->SetDecoderFileName(Query);

	// Decode Generic DS
	theCSInterface->StartDecode();
	theExtractionInterface->SetQueryDSInterface(theDescriptionInterface);
	// ---------------------------------------------------------------------------------------
	// Match query with each item of database
	// ---------------------------------------------------------------------------------------
	{

	int i;
	for (i=0; i<NoOfElements; i++)
	{

		// Get Program DS interface
		DBDescriptionInterface=
			m_DBDescription[i].GetInterface();

		// Connect reference description scheme to Extraction tool
		theExtractionInterface->
			SetFeedbackDSInterface(DBDescriptionInterface);

		// Extract Matching Hint of Query
		theExtractionInterface->StartExtract();
	}
	}

	// Encode the query description using the updated MatchingHint
	theCSInterface->SetEncoderFileName(Out);
	theCSInterface->StartEncode();
	theCSInterface->destroy();
	theExtractionInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long MatchingHintServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MatchingHintServer::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MatchingHintServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MatchingHintServer::GetName(void)
{ return myName; }










				





