////////////////////////////////////////////////////////////////////////
//
// ColorStructureSAppl.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Description of related preprocessor definitions
// 
// Project XMWinExe
//		USECT				includes the ColorStructure Server and Client in main()
//		READ_WRITE_MOM		internal (Sharp) patch to MediaIO to read/write MomVop 
//								structures into single file, bypassing imagemagick
// Project XMLib
//		READ_WRITE_MOM		see above project (required in both projects)
//		CLIENT_QUANT		moves (nonlinear) quantization to the server side
//		WRITE_HISTOGRAMS	enables writing out histogram files to mirror the input images
//
// The standard configuration is for all the above definitions (except USECT) to be undefined
// The internal (Sharp) standard configuration is to define: USECT, READ_WRITE_MOM in XMWinExe and
// READ_WRITE_MOM, CLIENT_QUANT in XMLib.
// Also, all unused USExx definitions are undefined and all unused folders in XMLib 
// are set to "exclude from build" to speed up compilation
// 
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;

const UUID ColorStructureServer::myID = UUID("");
const char *ColorStructureServer::myName = "ColorStructureServer";

//=============================================================================
ColorStructureServer::ColorStructureServer()
{
}

//----------------------------------------------------------------------------
ColorStructureServer::~ColorStructureServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the ColorStructure D/DSType
// created using the data. This is then written to the bitstream/DDL file. 
//----------------------------------------------------------------------------
unsigned long ColorStructureServer::Start(char *ListFile, char *Bitstream)
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

	// This sets up the ColorStructure type descriptor interface
	ColorStructureDescriptorInterfaceABC* theDescriptorInterface = NULL;

	// used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface						= NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface					= NULL;

	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	ColorStructureExtractionTool*         theExtractionTool               = NULL;
	ColorStructureExtractionInterfaceABC* theExtractionInterface          = NULL;

	// This sets up the coding scheme for this D/DType and the interface
	int CodingMode;	// 0:DDL, 1:BiM, 2:Binary
	ColorStructureCS*                     theBinCS                        = NULL;
	ColorStructureCSInterfaceABC*         theBinCSInterface               = NULL;
	GenericDSCS*                          theDDLCS                        = NULL;
	GenericDSCSInterfaceABC*              theDDLCSInterface               = NULL;

	// Ok, make new instatiations of these datatypes
	try{
		// Create media object 
		theMedia = new MultiMedia;

		// obtain media interface
		theMediaInterface=theMedia->GetInterface();

		// Pass it null so that it creates a new descriptor
		theExtractionTool = new ColorStructureExtractionTool(NULL);

		// Obtain the extraction interface from this extraction tool
		theExtractionInterface=theExtractionTool->GetInterface();

		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();

		// select coding scheme type
#ifdef USEXML
		CodingMode = atoi(getpara("CodingMode", "2")); // default is binary
#else
		CodingMode = 2; // binary mode
#endif

		if(CodingMode == 2) // binary coding
		{
			// Now create the binary coding scheme
			theBinCS = new ColorStructureCS(theDescriptorInterface);

			// Obtain the interface
			theBinCSInterface=theBinCS->GetInterface();
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
			//theCollection = theMpeg7Root.CreateChild("Description");
			//theCollection.SetAttribute("xsi:type","ContentEntityType");
			// m15122
			theCollection = theMpeg7Root.CreateChild("DescriptionUnit");
			theCollection.SetAttribute("xsi:type","DescriptorCollectionType");
			theCollectionInterface = theCollection.GetInterface();

			// now create the XML parser based coding scheme
			theDDLCS = new GenericDSCS(theDocumentInterface);

			// obtain the interface
			theDDLCSInterface = theDDLCS->GetInterface();

			// connect DDL file to coding scheme
			theDDLCSInterface->SetDecoderFileName(Bitstream);
		}
	}
	catch(...) {
		fprintf(stderr,"ColorStructureServer::Start: Allocation error\n");
		return(1);
	}

	// All of the pointers should now be non-null so check them
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
		theExtractionInterface->SetSourceMedia(theMediaInterface);

		/* reset histogram values*/
		theExtractionInterface->InitExtracting();

		/* extract descriptor form media*/
		theExtractionInterface->StartExtracting();

#ifdef WRITE_HISTOGRAMS
		string histFilename = RamDB.GetMediaFilename();
		histFilename.insert(0,"histograms/");
		histFilename.append(".tagXXX");
		ofstream histFile(&histFilename.at(0), ios::binary);
		// TODO insert normalization factor if desired
		for (int i=0; i<theDescriptorInterface->GetSize(); i++)
		{
			int v = theDescriptorInterface->GetElement(i);
			histFile.write((char*)&v,sizeof(int));
		}
		histFile.close();
#endif

		/* code descriptor data*/
		if(CodingMode == 2) // binary coding
		{
			// write to binary
			theBinCSInterface->StartEncode();
		}
		else
		{
			// export description to collection DDL tree
			theDescriptorInterface->ExportDDL(theCollectionInterface);
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

	// tidy up all the memory allocations
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
		// write to DDL
		theDDLCSInterface->StartEncode();

		theDDLCSInterface->destroy();
#endif
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureServer::Stop()
{
	return(0);
}


//----------------------------------------------------------------------------
unsigned long ColorStructureServer::Open()
{
	// If there are more total pixels to be converted (RGB to HMMD) than the
	// the total possible RGB permutations, then create a lookup table to 
	//  speed up. (The threshold is just a couple hundred images, so always 
	//  create table for server applications.)
	ColorStructureExtractionTool::BuildColorQuantTable();	// Base == 256

//	ColorStructureExtractionTool::BuildColorQuantTable(2);	// 128
//	ColorStructureExtractionTool::BuildColorQuantTable(1);	// 64
//	ColorStructureExtractionTool::BuildColorQuantTable(0);	// 32
	
	return(0);
}


//----------------------------------------------------------------------------
unsigned long ColorStructureServer::Close()
{
	return(0);
}

	
//----------------------------------------------------------------------------
const UUID& ColorStructureServer::GetObjectID(void)
{ 
	return(myID);
}

//----------------------------------------------------------------------------
const char *ColorStructureServer::GetName(void)
{ 
	return(myName); 
}



