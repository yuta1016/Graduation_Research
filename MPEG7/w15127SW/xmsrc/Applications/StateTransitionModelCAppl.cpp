///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation StateTransitionModel.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation StateTransitionModel tools as specified by the MPEG-7 Requirements.
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  StateTransitionModelCAppl.cpp
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
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"

#include <iostream> //LINUX_2011
#include <fstream>

static const int VERB = 1;

//=============================================================================

using namespace XM;
using namespace std; //LINUC_2011


const UUID StateTransitionModelClient::myID = UUID("");
const char *StateTransitionModelClient::myName = "StateTransitionModelClient";

const int	StateTransitionModelClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
StateTransitionModelClient::StateTransitionModelClient() : 
m_QueryFile(0), 
m_TargetFile(0),
m_theQueryDS(0), 
m_theTargetDS(0)
{
}

//----------------------------------------------------------------------------
StateTransitionModelClient::~StateTransitionModelClient()
{
	delete [] m_QueryFile;
	delete [] m_TargetFile;
	delete m_theQueryDS;
	delete m_theTargetDS;
}

//----------------------------------------------------------------------------
// ListFile = list of target MPEG-7 StateTransitionModel descriptions
// DescriptionFile = MPEG-7 query StateTransitionModel description

unsigned long StateTransitionModelClient::Open(char *ListFile, char *DescriptionFile)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	char		pcTargetDescriptionFile[STRING_MAXSIZE];	// Media input file
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
		fprintf(stderr,"StateTransitionModelClient::Start: Allocation error\n");
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
	//for(int ientry=0;ientry<NDBaseEntries;ientry++) {
	for(int ientry=0;ientry<1;ientry++) {
		RamDB.GotoEntry(ientry);
		if (VERB) cerr << RamDB.GetMediaFilename() << endl;
		strcpy(pcTargetDescriptionFile, RamDB.GetMediaFilename());

		ifstream tinf(pcTargetDescriptionFile);
		if (tinf==NULL) {
			fprintf( stderr, "ERROR(StateTransitionModelServer): Could not find %s file.\n", pcTargetDescriptionFile);
			exit(-1);
		}
		tinf.close();
		if (VERB) cerr <<"pcTargetDescriptionFile=" << pcTargetDescriptionFile << endl;

		strcpy(pcBaseDir, pcTargetDescriptionFile);
		int n = strlen(pcBaseDir)-1;
		while (n>0 && pcBaseDir[n]!='/') n--;
		if (pcBaseDir[n]=='/') pcBaseDir[n+1] = '\0';

		m_TargetFile = new char[strlen(pcTargetDescriptionFile)+1];
		strcpy(m_TargetFile, pcTargetDescriptionFile);

		sprintf(pcXMLFile, "%s%s", pcBaseDir, DescriptionFile);
		m_QueryFile = new char[strlen(pcXMLFile)+1];
		strcpy(m_QueryFile, pcXMLFile);

		/* -----------------------------------------------------------------
			Connect Interfaces 
		-------------------------------------------------------------- */

		{	/** Get description from DescriptionFile **/

 			GenericDSInterfaceABC *theDescriptionInterface;
			GenericDSCS *theCS;
			GenericDSCSInterfaceABC *theCSInterface;

			/* Connect Interfaces and Get coding scheme interface */
			theCS = new GenericDSCS;
			theCSInterface = theCS->GetInterface();
			m_theQueryDS = new GenericDS(GenericDS::CreateDescriptionRoot());

			/* Connect description file to coding scheme */
			theCSInterface->SetDecoderFileName(m_TargetFile);
			theDescriptionInterface = m_theQueryDS->GetInterface();;

			/* Connect Generic DS to coding schemes and Decode Generic DS */
			theCSInterface->SetDescriptionInterface(theDescriptionInterface);
			theCSInterface->StartDecode();
			theCSInterface->destroy();
		}

		{	/** Get description from DescriptionFile **/

 			GenericDSInterfaceABC *theDescriptionInterface;
			GenericDSCS *theCS;
			GenericDSCSInterfaceABC *theCSInterface;

			/* Connect Interfaces and Get coding scheme interface */
			theCS = new GenericDSCS;
			theCSInterface = theCS->GetInterface();
			m_theTargetDS = new GenericDS(GenericDS::CreateDescriptionRoot());
			
			/* Connect description file to coding scheme */
			theCSInterface->SetDecoderFileName(m_TargetFile);
			theDescriptionInterface = m_theTargetDS->GetInterface();

			/* Connect Generic DS to coding schemes and Decode Generic DS */
			theCSInterface->SetDescriptionInterface(theDescriptionInterface);
			theCSInterface->StartDecode();
			theCSInterface->destroy();
		}
	}

	return 0;
}

//----------------------------------------------------------------------------
// Query gives parameters of transcoding goal, i.e., bandwidth, ...
// Ignores NoOfMatches

unsigned long StateTransitionModelClient::Start(char *Query, int NoOfMatches)
{
	if (VERB) fprintf( stderr, "StateTransitionModelClient::Start( %s, %d)\n", Query, NoOfMatches);

	/* Get target description interface */
	GenericDSInterfaceABC *theTargetInterface;
 	theTargetInterface = m_theTargetDS->GetInterface();

	/* Get query description interface */
	GenericDSInterfaceABC *theQueryInterface;
 	theQueryInterface = m_theQueryDS->GetInterface();

	/* Search Tool */
	StateTransitionModelSearchTool *theSearch;
	StateTransitionModelSearchInterfaceABC *theSearchInterface;

	/* Connect DS to SearchTool */
	theSearch = new StateTransitionModelSearchTool(theQueryInterface, theTargetInterface);
	theSearchInterface = theSearch->GetInterface();

	theSearchInterface->SetStateTransitionModelParms(m_QueryFile, m_TargetFile);
	theSearchInterface->StartMatching();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long StateTransitionModelClient::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& StateTransitionModelClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StateTransitionModelClient::GetName(void)
{ return myName; }

				



