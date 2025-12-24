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
// Applicable File Name:  ModelSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
//#include <vopio.h>//sth shouldn't be required
#include <sys/stat.h>

#include "Applications/ServerApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"

static const int VERB = 0;//sth define?
static const char PARMFILE[] = "parms.txt";//sth given in XM call and unfortunate name

using namespace XM;
using namespace std;

const UUID  ModelServer::myID = UUID("");
const char *ModelServer::myName = "ModelSAppl";
const int	ModelServer::STRING_MAXSIZE= 4096;		// maximum size of the strings

//=============================================================================
ModelServer::ModelServer()
{
}

//----------------------------------------------------------------------------
ModelServer::~ModelServer()
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


unsigned long ModelServer::Start(char *ListFile, char *DDLFile)
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
	ModelExtractionTool*         theExtractionTool               = NULL;
	ModelExtractionInterfaceABC* theExtractionInterface          = NULL;
	
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
		fprintf(stderr,"ModelServer::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);

	// ---------------------------------------------------------------------------------------
	// Start Extraction for each database in ListFile
	// ---------------------------------------------------------------------------------------
 
	// Connect media to media data base
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
			fprintf( stderr, "ERROR(ModelServer): Could not find %s file.\n", pcMediaFile);
			exit(-1);
		}
		tinf.close();
		if (VERB) cerr <<"pcMediaFile=" << pcMediaFile << endl;

		//sth wrong style for file naming in database
		//sth see DSDummyType
		strcpy(pcBaseDir, pcMediaFile);
		int n = strlen(pcBaseDir)-1;
		while (n>0 && pcBaseDir[n]!='/') n--;
		if (pcBaseDir[n]=='/') pcBaseDir[n+1] = '\0';

		sprintf(pcParmFile, "%s%s", pcBaseDir, PARMFILE);
		// Read extraction parameter file associated with input media
		ifstream inf(pcParmFile); //sth there is already a library to deal with paramter files

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
		  theExtractionTool = new ModelExtractionTool(theDescriptionInterface);//sth usually outside of loop

			// Obtain the extraction interface from this extraction tool
			theExtractionInterface=theExtractionTool->GetInterface();

			// Now create the coding scheme
			theCS = new GenericDSCS(theDescriptionInterface);//sth usually outside of loop

			// Obtain the interface
			theCSInterface=theCS->GetInterface();
		}
		catch(...) {
			fprintf(stderr,"ModelServer::Start: Allocation error\n");
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
		if (VERB) cerr <<"InitExtracting" << endl;

		// Set source file for extraction tool
		theExtractionInterface->SetSourceMedia(inf);
		if (VERB) cerr <<"SetSourceMedia" << endl;
		
		// Start Extraction
		theExtractionInterface->StartExtracting();
		if (VERB) cerr <<"StartExtracting" << endl;

		// Post extraction  
		theExtractionInterface->PostExtraction();
		if (VERB) cerr <<"PostExtraction" << endl;

		// Name of output XML file
		sprintf(pcXMLFile, "%s%s", pcBaseDir, DDLFile);
		cerr << "Creating: " << pcXMLFile << endl;

		// connect stream to coding scheme
		theCSInterface->SetEncoderFileName(pcXMLFile);
		if (VERB) cerr <<"SetEncoderFileName to " << pcXMLFile << endl;

		// Output description
		theCSInterface->StartEncode();
		if (VERB) cerr <<"StartEncode" << endl;

		// Close filehandle (parameter file)
		inf.close();

		theExtractionInterface->destroy(); 
		theCSInterface->destroy();//sth usually outside of loop

	} 

	return(0);
	
}



//----------------------------------------------------------------------------
unsigned long ModelServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long ModelServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long ModelServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& ModelServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *ModelServer::GetName(void)
{ 
  return(myName); 
}



/*
//----------------------------------------------------------------------------
unsigned long ModelServer::Start(char *ListFile, char *DescriptionFile)
{

	// NOTES Start() method of the ModelDS Server
	// 
	// ListFile lists the models
	
	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	// Model files
	const char	INFILE[]	  = "model.txt";		// name of the model file on disk.

	char        pcBaseDir[STRING_MAXSIZE];		// Base directory
	char		pcVarName[STRING_MAXSIZE];		// Model file
	char		XMLFile[STRING_MAXSIZE];		// Output description file

	// sth not permitted to be used here
	// ---------------------------------------------------------------------------------------
    // Initialize the XML4C system
	// ---------------------------------------------------------------------------------------
	try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        fprintf( stderr, "ERROR(GenericDS): Error during Xerces-c Initialization: ");
		fprintf( stderr, (char *)toCatch.getMessage());
		exit(-1);
    }

	// Descriptors Schemes
	GenericDS theDescription = GenericDS::CreateDescriptionRoot(); // move inside loop because we cannot claer the XML document
 	GenericDSInterfaceABC *theDescriptionInterface;

	// Extraction Tools
	ModelExtractionTool *theExtraction;  // sth
	ModelExtractionInterfaceABC *theExtractionInterface;
	
	// Generic Coding Scheme
	GenericDSCS *theCS;  // sth
	GenericDSCSInterfaceABC *theCSInterface;

	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get DS interface
 	theDescriptionInterface = theDescription.GetInterface(); // sth inside loop

	// Connect DS to ExtractionTool
	theExtraction = new ModelExtractionTool;
	theExtractionInterface = theExtraction->GetInterface();
	theExtractionInterface->SetDSInterface(theDescriptionInterface); // sth inside loop

	// Connect DS to coding scheme
	theCS = new GenericDSCS;
	theCSInterface = theCS.GetInterface();
	theCSInterface->SetDescriptorInterface(theDescriptionInterface); // sth inside loop

	// ---------------------------------------------------------------------------------------
	// Start Extraction for each database in ListFile
	// ---------------------------------------------------------------------------------------

	if (1) {
	  // sth listfile is handled by MediaDB
		ifstream linf(ListFile);
		if (!linf.getline(pcBaseDir, STRING_MAXSIZE)) {
			fprintf( stderr, "ERROR(ModelServer): Couldn't not open %s file.\n", ListFile);
			exit(-1);
		}
		
	  // sth use MultiMedia class and use specified directory structure
		// sth mediafilename.dir/model.txt
		// Read the "model.txt" file 
		strcpy( pcVarName, pcBaseDir);
		strcat( pcVarName, INFILE);

		ifstream inf(pcVarName);
		if (inf==NULL) {
			fprintf( stderr, "ERROR(ModelServer): Couldn't not find %s file.\n", pcVarName);
			exit(-1);
		}

		// Open the Output File
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		// Initialize extraction tool
		theExtractionInterface->InitExtracting();

		// Set source file for extraction tool
		theExtractionInterface->SetSourceMedia(inf);
		
		// Start Extraction
		theExtractionInterface->StartExtracting();

		// Post extraction
		theExtractionInterface->PostExtraction();

		// connect stream to coding scheme
		theCSInterface->SetEncoderFileName(XMLFile);

		// Output description
		theCSInterface->StartEncode();

		// Close filehandle (Tree file)
		inf.close();

	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return(0);
	
}
*/



				




