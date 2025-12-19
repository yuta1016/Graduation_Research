////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by 
// Karol Wnukowicz (Visual Atoms) 
// Stavros Paschalakis (Visual Atoms) 
// in the course of development of the MPEG-7 Standard (ISO/IEC 15938). 
// This software module is an implementation of a part 
// of one or more MPEG-7 Standard tools as specified by the MPEG-7 Standard. 
// ISO/IEC gives users of the MPEG-7 Standard free license to this software
// module or modifications thereof for use in hardware or software products
// claiming conformance to the MPEG-7 Standard. 
// Those intending to use this software module in hardware or software products 
// are advised that its use may infringe existing patents. The original 
// developer of this software module and his/her company, the subsequent 
// editors and their companies, and ISO/IEC have no liability for use of this 
// software module or modifications thereof in an implementation.
// Copyright is not released for non MPEG-7 Standard conforming products. 
// <CN1> retains full right to use the code for his/her own purpose, 
// assign or donate the code to a third party and to inhibit third parties 
// from using the code for non MPEG-7 Standard conforming products. 
// This copyright notice must be included in all copies or derivative works. 
// Copyright (c)2003-2015
//
////////////////////////////////////////////////////////////////////////////////
//
// Applicable File Name:  GridLayoutSAppl.cpp

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

using namespace XM;

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

const UUID GridLayoutServer::myID = UUID("");
const char *GridLayoutServer::myName = "GridLayoutServer";

//=============================================================================
GridLayoutServer::GridLayoutServer()
{
}

//----------------------------------------------------------------------------
GridLayoutServer::~GridLayoutServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the Dummy D/DSType
// created using the data. This is then written to the bitstream/DDL file. 


unsigned long GridLayoutServer::Start(char *ListFile, char *Bitstream)
{
	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(Bitstream != NULL);

	// -------------------- Initialisation of data ----------------------------

	// Media data base
	MediaDB RamDB;

	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// This sets up a binary file io with a buffer size of 512bytes (I think)
	EncoderFileIO theStreamBuffer(0x1000);

	// This sets up the dummy type descriptor interface
	GridLayoutDescriptorInterfaceABC* theDescriptorInterface          = NULL;

	// This descriptors are used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface                      = NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection, theDescriptionUnit;
	GenericDSInterfaceABC *theCollectionInterface                    = NULL;
	GenericDS theContent, theGridLayout;
	GenericDSInterfaceABC *theGridLayoutInterface;

	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	GridLayoutExtractionTool*         theExtractionTool               = NULL;
	GridLayoutExtractionInterfaceABC* theExtractionInterface          = NULL;

	// This sets up the coding scheme for this D/DType and the interface
	int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
	GridLayoutCS*                     theBinCS                        = NULL;
	GridLayoutCSInterfaceABC*         theBinCSInterface               = NULL;
	GenericDSCS*                     theDDLCS                        = NULL;
	GenericDSCSInterfaceABC*         theDDLCSInterface               = NULL;

	// Ok, make new instatiations of these datatypes
	try{
		// Create media object 
		theMedia = new MultiMedia;

		// obtain media interface
		theMediaInterface=theMedia->GetInterface();

		// Pass it null so that it creates a new descriptor
		theExtractionTool = new GridLayoutExtractionTool(NULL);

		// Obtain the extraction interface from this extraction tool
		theExtractionInterface=theExtractionTool->GetInterface();

		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();

		//select coding scheme type
#ifdef USEXML
		CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
#else
		CodingMode=2; //Binary mode
#endif

		if (CodingMode == 2) { // Binary coding
			// Now create the binary coding scheme
			theBinCS = new GridLayoutCS(theDescriptorInterface);

			// Obtain the interface
			theBinCSInterface=theBinCS->GetInterface();
		}
		else { // DDL or BiM coding
			// Create the XML document
			theDocument = GenericDS::CreateDescriptionRoot();
			theDocumentInterface=theDocument.GetInterface();

			// Create the Mpeg7Root element
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
			theDDLCSInterface=theDDLCS->GetInterface();

			// Connect DDL file to coding scheme
			theDDLCSInterface->SetDecoderFileName(Bitstream);
		}
	}
	catch(...) {
		fprintf(stderr,"GridLayoutServer::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
	assert(theDescriptorInterface          != NULL);
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	if (CodingMode == 2) { // Binary coding
		assert(theBinCS                      != NULL);
		assert(theBinCSInterface             != NULL);
	}
	else { // // DDL or BiM coding
		assert(theDocumentInterface          != NULL);
		//assert(theCollectionInterface        != NULL);
		assert(theDDLCS                      != NULL);
		assert(theDDLCSInterface             != NULL);
	}

	// Lastly, tell the dummy type about the output stream
	if (CodingMode == 2) { // Binary coding
		theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
	}
	else { // DDL or BiM coding
		theDDLCSInterface->SetEncoderFileName(Bitstream);
	}

	// ---------------------- Process the files -----------------------------

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);

	theExtractionInterface->InitExtracting();

	// loop over all input media files
	int NDBaseEntries = RamDB.GetNoOfElements();
	int ientry;
	for(ientry=0; ientry < NDBaseEntries; ientry++) {
		if(!RamDB.DBLoadImage()) break;

		// tell the extractor that this is the source medium
		theExtractionInterface->SetSourceMedia(theMediaInterface);

		// extract descriptor from media
		theExtractionInterface->StartExtracting();

		if(CodingMode == 2) // binary coding
		{
			// write GridLayout to binary
			theBinCSInterface->StartEncode();
		}
		else
		{
			theContent = theDescriptionUnit.CreateChild("Content");
			theContent.SetAttribute("xsi:type", "ImageType");
			theCollection = theContent.CreateChild("Image");
			theGridLayout=theCollection.CreateChild("GridLayoutDescriptors");
			theGridLayoutInterface=theGridLayout.GetInterface();

			theDescriptorInterface->ExportDDL(theGridLayoutInterface);

			// export description to collection DDL tree
			theDescriptorInterface->ExportDDL(theCollectionInterface);
		}
	}

	// tidy up all the memory allocations
	theExtractionInterface->PostExtracting(); // probably not needed here
	// as we delete D below
	theExtractionInterface->destroy();
	if(CodingMode == 2) // binary coding
	{
		theBinCSInterface->destroy();

		// shut down the output stream;
		theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML

		// write DominantColor to DDL
		theDDLCSInterface->StartEncode();

		theDDLCSInterface->destroy();
#endif
	}

	// All done
	return(0);
}


//----------------------------------------------------------------------------
unsigned long GridLayoutServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long GridLayoutServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long GridLayoutServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& GridLayoutServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *GridLayoutServer::GetName(void)
{ 
  return(myName); 
}



