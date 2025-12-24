///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Applicable File Name:  SpatioTemporalLocatorSAppl.cpp
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
#include "Utilities/Utilities.h"              /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;

const UUID SpatioTemporalLocatorServer::myID = UUID("");
const char *SpatioTemporalLocatorServer::myName = "Spatio Temporal Locator Server";

//=============================================================================
SpatioTemporalLocatorServer::SpatioTemporalLocatorServer()
{
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorServer::~SpatioTemporalLocatorServer()
{
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorServer::Start(char *ListFile, char *Bitstream)
{
	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(Bitstream != NULL);

        MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;
 	MediaDB RamDB;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0x2FFFF);

	// descriptor and sub-descriptor
	SpatioTemporalLocatorDescriptorInterfaceABC
	  *theSpatioTemporalLocatorDescriptorInterface;


	// extraction tool
	SpatioTemporalLocatorExtractionTool *theExtarctionTool;
	SpatioTemporalLocatorExtractionInterfaceABC
	  *theExtarctionInterface;
	
	// coding schemes and sub CS
	SpatioTemporalLocatorCS *theSpatioTemporalLocatorCS;
	SpatioTemporalLocatorCSInterfaceABC
	  *theSpatioTemporalLocatorCSInterface;

	GenericDSCS*                     theDDLCS                        = NULL;
	GenericDSCSInterfaceABC*         theDDLCSInterface               = NULL;

	// create media data class and get interface
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	// connect descriptor to extraction tool
	theExtarctionTool = new SpatioTemporalLocatorExtractionTool(0);
	theExtarctionInterface=theExtarctionTool->GetInterface();
        theSpatioTemporalLocatorDescriptorInterface=
	  theExtarctionInterface->GetDescriptorInterface();

    //select coding scheme type
    int CodingMode=atoi(getpara("CodingMode","2"));//default is Binary (2)

	if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
		theSpatioTemporalLocatorCS = new
		  SpatioTemporalLocatorCS(theSpatioTemporalLocatorDescriptorInterface);
		theSpatioTemporalLocatorCSInterface=
		  theSpatioTemporalLocatorCS->GetInterface();

	} else {	 // DDL or BiM coding
      // Now create the XML parser based coding scheme
      theDDLCS = new GenericDSCS();

      // Obtain the interface
      theDDLCSInterface=theDDLCS->GetInterface();
    }

	// connect media to extraction tool
	theExtarctionInterface->SetSourceMedia(theMediaInterface);

	// connect stream to coding scheme
	if (CodingMode == 2) { // Binary coding
		theSpatioTemporalLocatorCSInterface->
		  SetEncoderStreamBuffer(&theStreamBuffer);

		theStreamBuffer.StartBitStream(Bitstream);
	}

	// ---------------------- Process the files -----------------------------
	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);

    int counter = 0;
    while (1) {

	    //* malloc media and load input data
	    if (!RamDB.DBLoadImage()) break;

	    //* extract descriptor form media*
	    theExtarctionInterface->StartExtracting();
            counter++;
	  }
          if (counter == 0) { return (unsigned long)-1; }

	  //* code descriptor data*
	theExtarctionInterface->PostExtraction();

	if (CodingMode == 2) { // Binary coding
		// Write the D/DSType to binary
 		theSpatioTemporalLocatorCSInterface->StartEncode();

	}  else { // DDL or BiM coding
		GenericDS theMpeg7Root, theCollection;
		GenericDSInterfaceABC *theCollectionInterface;

	    GenericDS theDDLDescription = GenericDS::CreateDescriptionRoot();
		GenericDSInterfaceABC* theDDLDescriptionInterface =
				theDDLDescription.GetInterface();

		assert(theDDLDescriptionInterface     != NULL);

		theMpeg7Root = theDDLDescriptionInterface->CreateChild("Mpeg7");
		theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
		theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
		theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

		// Create the collection element for the descriptors
		theCollection = theMpeg7Root.CreateChild("DescriptionUnit");
		theCollection.SetAttribute("xsi:type", "MovingRegionType");
		theCollectionInterface = theCollection.GetInterface();
		
	    // Get DDL file name from data media filename and DDL file name
		//char path[256];
		//strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
		//strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
		//strncat(path,"DDLFile.xml",MAXMEDIANAMELEN-1-strlen(path));//sth see cleient
		//path[MAXMEDIANAMELEN-1]=0;

		//strcpy(path, Bitstream);
//		strcat(path, ".xml");	// deleted by Kaneko
		//if( strncmp(path+strlen(path)-4,".xml",4) != 0 ) strcat(path, ".xml");	// inserted by Kaneko
		//sth see client

		// Connect DDL file to coding scheme
		//theDDLCSInterface->SetEncoderFileName(path);
		theDDLCSInterface->SetEncoderFileName(Bitstream);

		// Connect descriptors to coding schemes
		theDDLCSInterface->SetDescriptionInterface(theDDLDescriptionInterface);

		// Export description to DDL Tree
		theSpatioTemporalLocatorDescriptorInterface->ExportDDL(theCollectionInterface);

		// Write the D/DSType to binary
		theDDLCSInterface->StartEncode();
    }

	// Tidy up all the memory allocations
	theMediaInterface->destroy();
	theExtarctionInterface->destroy();

	if (CodingMode == 2) { // Binary coding
		theSpatioTemporalLocatorCSInterface->destroy();

		theStreamBuffer.CloseBitStream();
	}  else {
		theDDLCSInterface->destroy();
	}


	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorServer::Stop()
{
//	fprintf(stderr,"Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorServer::Close()
{
//	fprintf(stderr,"Not Implemented");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorServer::GetName(void)
{ return myName; }



