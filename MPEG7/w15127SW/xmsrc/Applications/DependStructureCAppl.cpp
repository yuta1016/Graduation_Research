////////////////////////////////////////////////////////////////////////
//
// DependStructureCAppl.cpp
//
// This software module was originally developed by
//
// Masahiro Shibata, NHK(Japan Broadcasting Corporation)
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
// NHK(Japan Broadcasting Corp.) retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
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
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace XM;

const UUID DependStructureClient::myID = UUID("");
const char *DependStructureClient::myName = "DependStructureClientClient";

//=============================================================================
DependStructureClient::DependStructureClient():
m_RamDB(),
NoOfElements(0),
m_DBDescription(0)
{
}

//----------------------------------------------------------------------------
DependStructureClient::~DependStructureClient()
{
}

//----------------------------------------------------------------------------
unsigned long DependStructureClient::Open(char *ListFile, char *DescriptionFile)
{

	// Get coding scheme interface
	GenericDSCS	*theCS = new GenericDSCS;
	GenericDSCSInterfaceABC *theCSInterface = theCS->GetInterface();

	// Read filename data base
	m_RamDB.CreateDatabase(ListFile);
	NoOfElements=m_RamDB.GetNoOfElements();

	// Create in memory decompressed database
	// allocate memories
	m_DBDescription =
	  new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++)
	{

		// Connect description file to coding scheme
		theCSInterface->SetDecoderFileName(m_RamDB.GotoEntry(i)->GetFilename());

		// Connect Generic DS to coding schemes
		theCSInterface->
		  SetDescriptionInterface(m_DBDescription[i].GetInterface());

		// Decode Generic DS
		theCSInterface->StartDecode();
		if ( m_DBDescription[i].GetInterface()->isNull() )
			std::cerr << "Could not decode " << m_RamDB.GotoEntry(i)->GetFilename() << std::endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long DependStructureClient::Start(char *Query,int NoOfMatches)
{

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------

	// Get description scheme interface
	GenericDS theDescription;

	// Get coding scheme interface & Connect Generic DS to coding schemes
	GenericDSCS *theCS = new GenericDSCS(theDescription.GetInterface());
	GenericDSCSInterfaceABC *theCSInterface = theCS->GetInterface();

	// Build query
	theCSInterface->SetDecoderFileName(Query);
	theCSInterface->StartDecode();

	// Connect DS to SearchTool & Set this query into the search tool
	DependStructureSearchTool *theSearchTool =
								new DependStructureSearchTool(theDescription.GetInterface());
	DependStructureSearchInterfaceABC *theSearchInterface = theSearchTool->GetInterface();

	std::vector< DependStructureSearchResult > results;
	string	queryText;
	int	i;
	for(i=0; i<NoOfElements; i++)
	{
	  theSearchInterface->
	    SetRefDescriptionInterface(m_DBDescription[i].GetInterface());
	  theSearchInterface->GetDistance(results, queryText);//sth interface violation
	}

	std::cerr << "Query(" << queryText.c_str() << ")" << std::endl;

	std::sort(results.begin(), results.end());
	if(NoOfMatches < 0 || results.size() < NoOfMatches){
	    NoOfMatches = results.size();
	}
	for(i=0; i<NoOfMatches; i++){
	  std::cerr << "Penalty=" << results[i].GetPenalty() << "/ " <<
	    results[i].GetIndex().c_str(); //sth don´t write to stdout
	  if(!results[i].GetLack().empty()){
	    std::cerr << "/";
	    std::cerr << " " << results[i].GetLack().c_str();
	  }
	  std::cerr << std::endl;
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long DependStructureClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long DependStructureClient::Close()
{

	// Free memories
	m_RamDB.FreeDatabase();
	if (m_DBDescription) delete [] m_DBDescription;
	
	NoOfElements = 0;
	m_DBDescription = 0;

	return 0;
}

//----------------------------------------------------------------------------
const UUID& DependStructureClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DependStructureClient::GetName(void)
{ return myName; }

