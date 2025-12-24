///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
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
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpaceFrequencyViewSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#include <sys/stat.h>

#include "Applications/ServerApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"

static const int VERB = 0; //sth define??
static const char PARMFILE[] = "parms.txt";//sth paramterfile is loaded in main

using namespace XM;
using namespace std;

const UUID  SpaceFrequencyViewServer::myID = UUID("");
const char *SpaceFrequencyViewServer::myName = "SpaceFrequencyViewSAppl";
const int	SpaceFrequencyViewServer::STRING_MAXSIZE= 4096;		// maximum size of the strings

//=============================================================================
SpaceFrequencyViewServer::SpaceFrequencyViewServer()
{
}

//----------------------------------------------------------------------------
SpaceFrequencyViewServer::~SpaceFrequencyViewServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   DDLFile - This gives the file name of the DDL output file.
//
// The contents of each file in ListFile is read in and the Dummy D/DSType
// created using the data. This is then written to the bitstream/DDL file. 

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewServer::Start(char *ListFile, char *DDLFile)
{

	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(DDLFile != NULL);
	
	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	char		pcParmFile[STRING_MAXSIZE];		// Parameter input file
	char		pcMediaFile[STRING_MAXSIZE];	// Media input file
	char		pcXMLFile[STRING_MAXSIZE];		// XML output file
	char		pcBaseDir[STRING_MAXSIZE];		// path to media file, parameter file, and XML file

	// Media data base
	MediaDB RamDB;

	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;
  
	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	SpaceFrequencyViewExtractionTool*         theExtractionTool               = NULL;
	SpaceFrequencyViewExtractionInterfaceABC* theExtractionInterface          = NULL;
	
	// This sets up the coding scheme for this D/DType and the interface
	GenericDSCS*                     theCS                           = NULL;
	GenericDSCSInterfaceABC*         theCSInterface                  = NULL;

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
		fprintf(stderr,"SpaceFrequencyViewServer::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);

	// ---------------------------------------------------------------------------------------
	// Start Extraction for each database in ListFile
	// ---------------------------------------------------------------------------------------
 
	// Connect media to media database
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);

	int NDBaseEntries=RamDB.GetNoOfElements();
	for(int ientry=0;ientry<NDBaseEntries;ientry++) {

		RamDB.GotoEntry(ientry);
		if (VERB) cerr << RamDB.GetMediaFilename() << endl;
		strcpy(pcMediaFile, RamDB.GetMediaFilename());

		ifstream tinf(pcMediaFile);
		// VS2013-MOD
		if (!tinf.is_open()) {
		//if (tinf == NULL) {
			fprintf( stderr, "ERROR(SpaceFrequencyViewServer): Could not find %s file.\n", pcMediaFile);
			exit(-1);
		}
		tinf.close();
		if (VERB) cerr <<"pcMediaFile=" << pcMediaFile << endl;

		strcpy(pcBaseDir, pcMediaFile);
		int n = strlen(pcBaseDir)-1;
		while (n>0 && pcBaseDir[n]!='/') n--;
		if (pcBaseDir[n]=='/') pcBaseDir[n+1] = '\0';
		//sth wrong database directory structure

		int depth, viewtype;
		char viewname[STRING_MAXSIZE], imname[STRING_MAXSIZE], ext[STRING_MAXSIZE];

		// Read extraction parameter file associated with input media
		sprintf(pcParmFile, "%s%s", pcBaseDir, PARMFILE);
		ifstream inf(pcParmFile);//sth per media paramter sets? or database?
		while (inf >> imname >> ws >> ext >> depth >> viewtype >> ws >> viewname >> ws) {

			// ---------------------------------------------------------------------------------------
			// Setup interfaces
			// ---------------------------------------------------------------------------------------

			// This sets up the DS interface
			GenericDS theDescription = GenericDS::CreateDescriptionRoot();
			GenericDSInterfaceABC* theDescriptionInterface = theDescription.GetInterface();

			// ---------------------------------------------------------------------------------------
			// Instantiate and Connect Interfaces 
			// ---------------------------------------------------------------------------------------
			try{

				// Pass it null so that it creates a new description
				theExtractionTool = new SpaceFrequencyViewExtractionTool(theDescriptionInterface);

				// Obtain the extraction interface from this extraction tool
				theExtractionInterface=theExtractionTool->GetInterface();

				// Now create the coding scheme
				theCS = new GenericDSCS(theDescriptionInterface);

				// Obtain the interface
				theCSInterface=theCS->GetInterface();
			}	
			catch(...) {
				fprintf(stderr,"SpaceFrequencyViewServer::Start: Allocation error\n");
				return(1);
			}

			// All of the pointers should now be non-null so check them
			assert(theDescriptionInterface         != NULL);
			assert(theExtractionTool               != NULL);
			assert(theExtractionInterface          != NULL);
			assert(theCS                           != NULL);
			assert(theCSInterface                  != NULL);	

			// Initialize extraction tool
			theExtractionInterface->InitExtracting();

			// Set parameters for extraction
			theExtractionInterface->SetViewParms(imname, ext, depth, (SFVIEWDECOMPTYPES) viewtype);
			
			// Start Extraction
			theExtractionInterface->StartExtracting();

			// Post extraction
			theExtractionInterface->PostExtraction();

			// Name of output XML file
			sprintf(pcXMLFile, "%s%s_%d_%s", pcBaseDir, viewname, depth, DDLFile);
			cerr << "Creating: " << pcXMLFile << endl;

			// connect stream to coding scheme
			theCSInterface->SetEncoderFileName(pcXMLFile);

			// Output description
			theCSInterface->StartEncode();

			theExtractionInterface->destroy(); //sth outside of loop
			theCSInterface->destroy();//sth
		}

		// Close extraction parameter file
		inf.close();
	}

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewServer::Stop()
{
	//fprintf(stderr,"WARNING(SpaceFrequencyViewServer): Stop() member function not Implemented.\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewServer::Open()
{
  return(0);
}

//----------------------------------------------------------------------------
unsigned long SpaceFrequencyViewServer::Close()
{
	//fprintf(stderr,"WARNING(SpaceFrequencyViewServer): Close() member function not Implemented.\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SpaceFrequencyViewServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpaceFrequencyViewServer::GetName(void)
{ return myName; }










				



