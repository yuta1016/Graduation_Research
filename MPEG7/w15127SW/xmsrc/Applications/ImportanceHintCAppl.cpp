///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation ImportanceHint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation ImportanceHint tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  ImportanceHintCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
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
#include <fstream>

static const int VERB = 0;

//=============================================================================

using namespace XM;

const UUID ImportanceHintClient::myID = UUID("");
const char *ImportanceHintClient::myName = "ImportanceHintClient";

const int	ImportanceHintClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
ImportanceHintClient::ImportanceHintClient() : 
m_Mediafile(0), 
m_theGenericDS(0)
{
}

//----------------------------------------------------------------------------
ImportanceHintClient::~ImportanceHintClient()
{
	delete [] m_Mediafile;
	delete [] m_Hintfile;
	delete m_theGenericDS;
}

//----------------------------------------------------------------------------
// ListFile = list of image files
// DescriptionFile = MPEG-7 description with Importance Hints

unsigned long ImportanceHintClient::Open(char *ListFile, char *DescriptionFile)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	char		pcMediaFile[STRING_MAXSIZE];	// Media input file
	char		pcXMLFile[STRING_MAXSIZE];		// XML input file
	char		pcBaseDir[STRING_MAXSIZE];		// path to media file, parameter file, and XML file

	// Media data base
	MediaDB RamDB;

	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// ---------------------------------------------------------------------------------------
	// Instantiate and Connect Media Interfaces 
	// ---------------------------------------------------------------------------------------
	try{
		// Create media object 
		theMedia = new MultiMedia;

		// obtain media interface
		theMediaInterface=theMedia->GetInterface();

	}
	catch(...) {
		fprintf(stderr,"ImportanceHintClient::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);

	// ---------------------------------------------------------------------------------------
	// Start Transcoding for each database in ListFile
	// ---------------------------------------------------------------------------------------
 
	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);

	int NDBaseEntries=RamDB.GetNoOfElements();
	for(int ientry=0;ientry<NDBaseEntries;ientry++) {
		RamDB.GotoEntry(ientry);
		if (VERB) std::cerr << RamDB.GetMediaFilename() << std::endl;
		strcpy(pcMediaFile, RamDB.GetMediaFilename());

		std::ifstream tinf(pcMediaFile);
		// VS2013-MOD
		if (!tinf.is_open()) {
		//if (tinf==NULL) {
			fprintf( stderr, "ERROR(ImportanceHintServer): Could not find %s file.\n", pcMediaFile);
			exit(-1);
		}
		tinf.close();
		if (VERB) std::cerr <<"pcMediaFile=" << pcMediaFile << std::endl;

		//sth not the specified database directory stucture
		strcpy(pcBaseDir, pcMediaFile);
		int n = strlen(pcBaseDir)-1;
		while (n>0 && pcBaseDir[n]!='/') n--;
		if (pcBaseDir[n]=='/') pcBaseDir[n+1] = '\0';

		m_Mediafile = new char[strlen(pcMediaFile)+1];
		strcpy(m_Mediafile, pcMediaFile);

		sprintf(pcXMLFile, "%s%s", pcBaseDir, DescriptionFile);
		m_Hintfile = new char[strlen(pcXMLFile)+1];
		strcpy(m_Hintfile, pcXMLFile);

		/* Description Scheme Interface */
 		GenericDSInterfaceABC *theDescriptionInterface;

		/* Generic Coding Scheme */
		GenericDSCS *theCS;
		GenericDSCSInterfaceABC *theCSInterface;

		/* -----------------------------------------------------------------
			Connect Interfaces 
		-------------------------------------------------------------- */

		/* Get coding scheme interface */
		theCS = new GenericDSCS;
		theCSInterface = theCS->GetInterface();
	
		if (VERB) std::cerr << "Executing ImportanceHintClient using MP7"
			    "description " << m_Hintfile << std::endl;
		m_theGenericDS = new GenericDS(GenericDS::CreateDescriptionRoot());
	
		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(m_Hintfile);

		/* Get DS interface */
		theDescriptionInterface = m_theGenericDS->GetInterface();

		/* Connect Generic DS to coding schemes */
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);

		// This is a transcoding application not retrieval
		/* Decode Generic DS */
		theCSInterface->StartDecode();
		theCSInterface->destroy(); //sth maybe outside loop
	}

	return 0;
}

//----------------------------------------------------------------------------
// Query gives parameters of transcoding goal, i.e., bandwidth, ...
// Ignores NoOfMatches

unsigned long ImportanceHintClient::Start(char *Query, int NoOfMatches)
{
	if (VERB) fprintf( stderr, "ImportanceHintClient::Start( %s, %d)\n", Query, NoOfMatches);

	/* Get description scheme interface */
	GenericDSInterfaceABC *theDescriptionInterface;
 	theDescriptionInterface = m_theGenericDS->GetInterface();

	/* Search Tool */
	ImportanceHintSearchTool *theSearch;
	ImportanceHintSearchInterfaceABC *theSearchInterface;

	/* Connect DS to SearchTool */
	theSearch = new ImportanceHintSearchTool(theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	//sth wrong interface names
	theSearchInterface->SetImportanceHintParms(m_Mediafile, m_Hintfile);
	theSearchInterface->StartTranscoding();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long ImportanceHintClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ImportanceHintClient::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& ImportanceHintClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ImportanceHintClient::GetName(void)
{ return myName; }

				




