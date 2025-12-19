///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation Package.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Package tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  PackageCAppl.cpp
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

static const int VERB = 0;//sth define??

//=============================================================================

using namespace XM;
using namespace std;

const UUID PackageClient::myID = UUID("");
const char *PackageClient::myName = "Package Client";

const int	PackageClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
PackageClient::PackageClient() : 
m_Packagefile(0),
m_Descriptionfile(0),
m_thePackageDS(0)
{
}

//----------------------------------------------------------------------------
PackageClient::~PackageClient()
{
	delete [] m_Packagefile;
	delete [] m_Descriptionfile;
	delete m_thePackageDS;
}

//----------------------------------------------------------------------------
unsigned long PackageClient::Open(char *ListFile, char *DescriptionFile)
{

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	char		pcDescriptionFile[STRING_MAXSIZE];	// Description input file to be filtered using package
	char		pcXMLFile[STRING_MAXSIZE];			// Package XML input file
	char		pcBaseDir[STRING_MAXSIZE];			// path to package file and description file

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
		fprintf(stderr,"PackageClient::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);

	// ---------------------------------------------------------------------------------------
	// Start Filtering each description in ListFile
	// ---------------------------------------------------------------------------------------
 
	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);

	int NDBaseEntries=RamDB.GetNoOfElements();
	for(int ientry=0;ientry<NDBaseEntries;ientry++) {
		RamDB.GotoEntry(ientry);
		if (VERB) cerr << RamDB.GetMediaFilename() << endl;
		strcpy(pcDescriptionFile, RamDB.GetMediaFilename());

		ifstream tinf(pcDescriptionFile);

		// VS2013-MOD
		if (!tinf.is_open()) {
		//if (tinf == NULL) {
			fprintf( stderr, "ERROR(PackageServer): Could not find %s file.\n", pcDescriptionFile);
			exit(-1);
		}
		tinf.close();
		if (VERB) cerr <<"pcDescriptionFile=" << pcDescriptionFile << endl;

		strcpy(pcBaseDir, pcDescriptionFile);
		int n = strlen(pcBaseDir)-1;
		while (n>0 && pcBaseDir[n]!='/') n--;
		if (pcBaseDir[n]=='/') pcBaseDir[n+1] = '\0';
		//sth wrong datbase directory structure

		m_Descriptionfile = new char[strlen(pcDescriptionFile)+1];
		strcpy(m_Descriptionfile, pcDescriptionFile);

		sprintf(pcXMLFile, "%s%s", pcBaseDir, DescriptionFile);
		m_Packagefile = new char[strlen(pcXMLFile)+1];
		strcpy(m_Packagefile, pcXMLFile);

		{	/** Get description from DescriptionFile **/

 			GenericDSInterfaceABC *theDescriptionInterface;
			GenericDSCS *theCS;
			GenericDSCSInterfaceABC *theCSInterface;

			/* Connect Interfaces and Get coding scheme interface */
			theCS = new GenericDSCS;//sth outside of loop?
			theCSInterface = theCS->GetInterface();
			m_theDescriptionDS = new GenericDS(GenericDS::CreateDescriptionRoot());

			/* Connect description file to coding scheme */
			theCSInterface->SetDecoderFileName(m_Descriptionfile);
			theDescriptionInterface = m_theDescriptionDS->GetInterface();;

			/* Connect Generic DS to coding schemes and Decode Generic DS */
			theCSInterface->SetDescriptionInterface(theDescriptionInterface);
			theCSInterface->StartDecode();
			theCSInterface->destroy();
		}

		//sth reading the same as before?
		{	/** Get description from DescriptionFile **/

 			GenericDSInterfaceABC *theDescriptionInterface;
			GenericDSCS *theCS;
			GenericDSCSInterfaceABC *theCSInterface;

			/* Connect Interfaces and Get coding scheme interface */
			theCS = new GenericDSCS;
			theCSInterface = theCS->GetInterface();
			m_thePackageDS = new GenericDS(GenericDS::CreateDescriptionRoot());
			
			/* Connect description file to coding scheme */
			theCSInterface->SetDecoderFileName(m_Packagefile);
			theDescriptionInterface = m_thePackageDS->GetInterface();

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

unsigned long PackageClient::Start(char *Query, int NoOfMatches)
{
  //sth what is this kind of key application
	//fprintf( stderr, "PackageClient::Start( %s, %d)\n", Query, NoOfMatches);

	/* Get description interface */
	GenericDSInterfaceABC *theDescriptionInterface;
 	theDescriptionInterface = m_theDescriptionDS->GetInterface();

	/* Get package interface */
	GenericDSInterfaceABC *thePackageInterface;
 	thePackageInterface = m_thePackageDS->GetInterface();

	/* Search Tool */
	PackageSearchTool *theSearch;
	PackageSearchInterfaceABC *theSearchInterface;

	/* Connect DS to SearchTool */
	theSearch = new PackageSearchTool(thePackageInterface, theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	//sth not search interface
	//theSearchInterface->SetDescriptionInterface(theDescriptionInterface);
	theSearchInterface->SetPackageParms(m_Packagefile, m_Descriptionfile);
	theSearchInterface->StartPackaging();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long PackageClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long PackageClient::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& PackageClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PackageClient::GetName(void)
{ return myName; }










				




