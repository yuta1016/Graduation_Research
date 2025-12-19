///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski
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
// Applicable File Name:  DominantColorAppl.cpp
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
#include "Utilities/Utilities.h"

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

namespace XM {
//#define DEBUG

const UUID DominantColorServer::myID = UUID(" ");
const char *DominantColorServer::myName = "Dominant Color Server";

//=============================================================================
DominantColorServer::DominantColorServer()
{
}

//----------------------------------------------------------------------------
DominantColorServer::~DominantColorServer()
{
}

//----------------------------------------------------------------------------
unsigned long DominantColorServer::Open()
{
  // This server doesn't handle pull
  return 0;
}

//----------------------------------------------------------------------------
unsigned long DominantColorServer::Start(char *ListFile, char *Bitstream)
{
	// Check that the file names are ok
	assert(ListFile		!= NULL);
	assert(Bitstream	!= NULL);

	// Initialization of data
	// ---------------------- Initialization of data -----------------------------
	
	// Media data base
	MediaDB RamDB;

	// Media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// stream data base
	EncoderFileIO theStreamBuffer(0xFFFF);

	// set up descriptor interface
	DominantColorDescriptorInterfaceABC *theDescriptorInterface		= NULL;

	// used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface						= NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface					= NULL;

	// set up extraction tool and the interface
	DominantColorExtractionTool *theExtractionTool					= NULL;
	DominantColorExtractionInterfaceABC *theExtractionInterface		= NULL;

	// set up coding scheme for descriptor and the interface
	int CodingMode;	// 0:DDL, 1:BiM, 2:Binary
	DominantColorCS *theBinCS										= NULL;
	DominantColorCSInterfaceABC *theBinCSInterface					= NULL;
	GenericDSCS *theDDLCS											= NULL;
	GenericDSCSInterfaceABC *theDDLCSInterface						= NULL;

	// OK, make new instantiations of these datatypes
	try
	{
		// create media object
		theMedia = new MultiMedia;

		// obtain media interface
		theMediaInterface = theMedia->GetInterface();

		// pass it null so that it creates a new descirptor
		theExtractionTool = new DominantColorExtractionTool(0);

		// obtain the extraction interface from this extraction tool
		theExtractionInterface = theExtractionTool->GetInterface();

		// obtain the descriptor interface
		theDescriptorInterface = theExtractionInterface->GetDescriptorInterface();

		// select coding scheme type
#ifdef USEXML
		CodingMode = atoi(getpara("CodingMode", "2")); // default is binary
#else
		CodingMode = 2; // binary mode
#endif

		if(CodingMode == 2) // binary coding
		{
			// now create the binary coding scheme
			theBinCS = new DominantColorCS(theDescriptorInterface);

			// obtain the interface
			theBinCSInterface = theBinCS->GetInterface();
		}
		else // DDL or BiM coding
		{
			// create the XML document
			theDocument = GenericDS::CreateDescriptionRoot();
			theDocumentInterface = theDocument.GetInterface();

			// create the Mpeg7Root element
			theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
			theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
			theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
			theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

			// create the collection element for the descriptors
			theCollection = theMpeg7Root.CreateChild("DescriptionUnit");
			theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
			theCollectionInterface = theCollection.GetInterface();

			// now create the XML parser based coding scheme
			theDDLCS = new GenericDSCS(theDocumentInterface);

			// obtain the interface
			theDDLCSInterface = theDDLCS->GetInterface();

			// connect DDL file to coding scheme
			theDDLCSInterface->SetDecoderFileName(Bitstream);
		}
	}

	catch(...)
	{
		fprintf(stderr, "DominantColorServer::Start: Allocation error!\n");
		return(1);
	}

	// all of the pointers should now be non-null so check them
	assert(theDescriptorInterface          != NULL);
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	if (CodingMode == 2) // binary coding
	{
		assert(theBinCS                      != NULL);
		assert(theBinCSInterface             != NULL);
	}
	else // DDL or BiM coding
	{
		assert(theDocumentInterface          != NULL);
		assert(theCollectionInterface        != NULL);
		assert(theDDLCS                      != NULL);
		assert(theDDLCSInterface             != NULL);
	}

	// lastly, tell the dummy type about the output stream
	if (CodingMode == 2) // Binary coding
	{
		theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
	}
	else // DDL or BiM coding
	{
		theDDLCSInterface->SetEncoderFileName(Bitstream);
	}

	// ---------------------- Process the files -----------------------------
  
	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);

        // Initialise extractor
        theExtractionInterface->InitExtracting();

	// loop over all input media files
	int NDBaseEntries = RamDB.GetNoOfElements();
        int ientry;
	for(ientry=0; ientry < NDBaseEntries; ientry++) {

#ifdef DEBUG
          std::cerr << "Info(DCS::Start): entry (" << ientry << ")" << std::endl;
#else
          std::cerr << "processing image " << ientry << ": "
               << RamDB.GetMediaFilename() << std::endl;
#endif
		// get the image
		if(!RamDB.DBLoadImage()) break;
#ifdef DEBUG
          std::cerr << "Info(DCS::Start): loaded (" << ientry << ")" << std::endl;
#endif

		// tell the extractor that this is the source medium
		theExtractionInterface->SetSourceMedia(theMediaInterface);

		// extract descriptor from media
		theExtractionInterface->StartExtracting();

#ifdef DEBUG
          std::cerr << "Info(DCS::Start): extracted (" << ientry << ")"
               << std::endl;
#endif

		if(CodingMode == 2) // binary coding
		{
			// write DominantColor to binary
			theBinCSInterface->StartEncode();
		}
		else
		{
			// export description to collection DDL tree
			theDescriptorInterface->ExportDDL(theCollectionInterface);
		}
#ifdef DEBUG
                std::cerr << "Info(DCS::StartExtract): exported (" << ientry << ")"
                     << std::endl;
#endif
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

	return 0;
}

//----------------------------------------------------------------------------
unsigned long DominantColorServer::Stop()
{
  return 0;
}

//----------------------------------------------------------------------------
unsigned long DominantColorServer::Close()
{
  return 0;
}

	
//----------------------------------------------------------------------------
const UUID& DominantColorServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DominantColorServer::GetName(void)
{ return myName; }

//#undef  DEBUG
};
