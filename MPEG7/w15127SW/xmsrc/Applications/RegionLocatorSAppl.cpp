///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, F. Sanahuja
// (contributing organizations names)
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RegionLocatorSAppl.cpp
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


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;
using namespace std;

const UUID RegionLocatorServer::myID = UUID("");
const char *RegionLocatorServer::myName = "Region Locator Server";

//=============================================================================
RegionLocatorServer::RegionLocatorServer()
{
    cerr << myName << " created" << endl;
}

//----------------------------------------------------------------------------
RegionLocatorServer::~RegionLocatorServer()
{
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorServer::Start(char *ListFile, char *Outfile)
{

        GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	GenericDS theMpeg7Root, theDescriptionUnit, theContent, theCollection, Descr;
	GenericDSInterfaceABC *theCollectionInterface; //=theCollection.GetInterface();
	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(Outfile != NULL);

	// -------------------- Initialisation of data ----------------------------

	// Determine binary or XML format
	bool bBinary = true;
	if (strlen(Outfile) >= 4 && !strcmp(&Outfile[strlen(Outfile)-4], ".xml"))
		bBinary = false;

	// Media data base
	MediaDB RamDB;

	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// This sets up a binary file io with a buffer size of 512bytes (I think)
	EncoderFileIO *theStreamBuffer;
	if (bBinary){
		theStreamBuffer = new EncoderFileIO(0x1000);
	}
	else {

		// Create the XML document
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();

		// Create the MPEG7 root element
		theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
		theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
		theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
		theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);
		theMpeg7Root.SetAttribute("xmlns:mpeg7", MPEG7_BASE_NS);

		theDescriptionUnit = theMpeg7Root.CreateChild("DescriptionUnit");
		theDescriptionUnit.SetAttribute("xsi:type", "ContentCollectionType");

		// Now create the XML parser based coding scheme
		theDDLCS = new GenericDSCS(theDocumentInterface);

		// Obtain the interface
		theDDLCSInterface = theDDLCS->GetInterface();

		// Connect the DDL file to the coding scheme
		theDDLCSInterface->SetEncoderFileName(Outfile);
	}

	// This sets up the RegionLocator type descriptor interface
	RegionLocatorDescriptorInterfaceABC* theDescriptorInterface = NULL;

	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	RegionLocatorExtractionTool*         theExtractionTool               = NULL;
	RegionLocatorExtractionInterfaceABC* theExtractionInterface          = NULL;

	// This sets up the coding scheme for this D/DType and the interface
	RegionLocatorCS*                     theCS                           = NULL;
	RegionLocatorCSInterfaceABC*         theCSInterface                  = NULL;

	// Ok, make new instatiations of these datatypes
	try{
		// Create media object
		theMedia = new MultiMedia;

		// obtain media interface
		theMediaInterface=theMedia->GetInterface();

		// Pass it null so that it creates a new descriptor
		theExtractionTool = new RegionLocatorExtractionTool(NULL);

		// Obtain the extraction interface from this extraction tool
		theExtractionInterface=theExtractionTool->GetInterface();

		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();

		// Now create the coding scheme
		theCS = new RegionLocatorCS(theDescriptorInterface);

		// Obtain the interface
		theCSInterface=theCS->GetInterface();
	}
	catch(...) {
		fprintf(stderr,"RegionLocatorServer::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theDescriptorInterface          != NULL);
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	assert(theCS                           != NULL);
	assert(theCSInterface                  != NULL);

	// Tell the RegionLocator type about the output stream or XML file
	if (bBinary)
	{
		theCSInterface->SetEncoderStreamBuffer(theStreamBuffer);
		theStreamBuffer->StartBitStream(Outfile);
	}
	else
		theCSInterface->SetEncoderFileName(Outfile);


	// ---------------------- Process the files -----------------------------

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	//RamDB.CreateDatabase(ListFile, true);
	RamDB.CreateDatabase(ListFile);

#ifdef SHOW_STATUS
	time_t tStart, tPrev, tNow;
	time(&tStart);
	tPrev = tStart;
#endif

	// Loop over the files in the database
	int NDBaseEntries=RamDB.GetNoOfElements();
	for(int ientry=0;ientry<NDBaseEntries;ientry++)
	{
		// Get the image

		if (!RamDB.DBLoadImage()) break;

		/* connect media to Extraction tool*/
		theExtractionInterface->SetSourceMedia(theMediaInterface->GetImage());

		/* reset histogram values*/
		theExtractionInterface->InitExtracting();

		/* extract descriptor form media*/
		theExtractionInterface->StartExtracting();

		if(bBinary)
		  {
		    /* code descriptor data*/
		    theCSInterface->StartEncode();
		  }
		else {
		  /* export into DOM*/
			theContent = theDescriptionUnit.CreateChild("Content");
			theContent.SetAttribute("xsi:type", "ImageType");
			theCollection = theContent.CreateChild("Image");
			theCollectionInterface=theCollection.GetInterface();
		  Descr=theCollectionInterface->CreateChild("SpatialLocator");
		  if (Descr.isNull())
		    return 1;
		  theDescriptorInterface->ExportDDL(Descr.GetInterface());
		}
		// free ImageMedia (should be done through RamDB?)
		theMediaInterface->FreeImage();

#ifdef SHOW_STATUS
		fprintf(stderr,"%6ld of %ld", ientry, RamDB.GetNoOfElements());
		time(&tNow);
		long r, d = (long)difftime(tNow,tStart);
		r = ientry ? d * (RamDB.GetNoOfElements() - ientry) / ientry : 0;
		if (tNow > tPrev)
		{
			fprintf(stderr," %ld:%02ld:%02ld %ld:%02ld:%02ld    ", d/3600, d%3600/60, d%60, r/3600, r%3600/60, r%60);
			tPrev = tNow;
		}
		fprintf(stderr,"\r");
#endif
	}

#ifdef SHOW_STATUS
	fprintf(stderr,"\n");
#endif

	// Write out the XML file
	if (!bBinary){
		  theDDLCSInterface->StartEncode();
		  theDDLCSInterface->destroy();
	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	if (bBinary)
	{
		theStreamBuffer->CloseBitStream();
		delete theStreamBuffer;
	}
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorServer::Stop()
{
  // fprintf(stderr,"RegionLocatorServer Stopped\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long RegionLocatorServer::Close()
{
  // fprintf(stderr, "RegionLocatorServer Closed\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& RegionLocatorServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorServer::GetName(void)
{ return myName; }



