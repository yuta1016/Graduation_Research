////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// KDDI R&D Laboratories Inc., Masaru Sugano, Yasuyuki Nakajima
// (contributing organizations names)
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SoundPropertySAppl.cpp
//

#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>

#include "Applications/ServerApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/readparam.h"

/*#ifdef USEXML
#include "dom/DOM.hpp"
#include "util/PlatformUtils.hpp"
#endif
*/ //sth usage not allowed here

using namespace XM;

const UUID SoundPropertyServer::myID = UUID("");
const char *SoundPropertyServer::myName = "SoundPropertyServer";

//=============================================================================
SoundPropertyServer::SoundPropertyServer()
{
}

//----------------------------------------------------------------------------
SoundPropertyServer::~SoundPropertyServer()
{
}

//----------------------------------------------------------------------------
unsigned long SoundPropertyServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SoundPropertyServer::Start(char *ListFile, char *DescriptionFile)
{
       // Check that the file names are ok
       assert(ListFile  != NULL);
       assert(DescriptionFile != NULL);

       // -------------------- Initialisation of data -----------------------
  
	// Media data base
	MediaDB RamDB;			// Audio database
	int iMedia;				// Number of input media files
	MediaDBElement *media;	// Audio media file name
	
	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;
 
	// This sets up the dummytype description interface
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
	GenericDSInterfaceABC* theDescriptionInterface =
	  theDescription.GetInterface();

	// Extraction Tools
	SoundPropertyExtractionTool*  theExtractionTool = NULL;
	SoundPropertyExtractionInterfaceABC* theExtractionInterface = NULL;

	// Generic Coding Scheme
	GenericDSCS *theCS = NULL;
	GenericDSCSInterfaceABC *theCSInterface = NULL;

	// Create media object 
	theMedia = new MultiMedia;

	// obtain media interface
	theMediaInterface=theMedia->GetInterface();

	// Get Extraction Interface
	theExtractionTool = new SoundPropertyExtractionTool(theDescriptionInterface);
	theExtractionInterface = theExtractionTool->GetInterface();

	// Get Coding Scheme Interface
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	// All of the pointers should now be non-null so check them
	assert(theDescriptionInterface         != NULL);
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	assert(theCS                           != NULL);
	assert(theCSInterface                  != NULL);

	// ---------------------- Process the files -----------------------------
  
	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Initialize Database
	RamDB.CreateDatabase(ListFile);

	iMedia = 0;

	// Loop files in ListFile
	while ( iMedia < RamDB.GetNoOfElements() )
	{	
		media = RamDB.GotoEntry(iMedia);

		// Connect media to extraction tool
		theExtractionInterface->SetSourceMedia(media);

		// Initialize values for the extraction
		theExtractionInterface->InitExtracting();

		// Start extraction
		theExtractionInterface->StartExtracting();

		iMedia++;
	}

	// Connect DS to extraction tool
	theExtractionInterface->SetDSInterface(theDescriptionInterface);

	// Connect DS to coding scheme
	theCSInterface->SetDescriptionInterface(theDescriptionInterface);

	// Connect description file to coding scheme
	theCSInterface->SetEncoderFileName(DescriptionFile);
		
	// Build the XML Document
	theExtractionInterface->PostExtracting();

	// Output description file
	theCSInterface->StartEncode();

	RamDB.FreeDatabase();
	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SoundPropertyServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SoundPropertyServer::Close()
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& SoundPropertyServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SoundPropertyServer::GetName(void)
{ return myName; }

