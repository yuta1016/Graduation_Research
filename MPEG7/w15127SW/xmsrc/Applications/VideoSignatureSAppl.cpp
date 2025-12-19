///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric MERCE
// P. Brasnett
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
// Applicable File Name:  VideoSignatureSAppl.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

#ifdef USEXML
extern const char *MPEG7_AMD4_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_AMD4_SCHEMA_LOCATION;
#endif // USEXML

using namespace XM;

const UUID VideoSignatureServer::myID = UUID("");
const char *VideoSignatureServer::myName = "Video Signature Server";

//=============================================================================
VideoSignatureServer::VideoSignatureServer()
{
}

//----------------------------------------------------------------------------
VideoSignatureServer::~VideoSignatureServer()
{
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureServer::Start(char *ListFile, char *Bitstream)
{
	int CodingMode;
	int CompressionFlag = 1;

	// Media data base
	MediaDB RamDB;

        // Media
        MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// stream data base
	EncoderFileIO theStreamBuffer(0x1000);

	// Get the coding scheme type
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "2"));
#else
	CodingMode = 2;
#endif // USEXML

#ifdef USEXML
	if(atoi(getpara("Compression", "1")) == 0)
	{
		CompressionFlag = 0;
	}

	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;

	GenericDS theMpeg7Root, theCollection;
        GenericDSInterfaceABC *theCollectionInterface;
	if (CodingMode != 2)
	{
          // Create the XML document
	  theDocument = GenericDS::CreateDescriptionRoot();
	  theDocumentInterface = theDocument.GetInterface();

          // Create the MPEG7 root element
	  theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
	  theMpeg7Root.SetAttribute("xmlns", MPEG7_AMD4_NS);
	  theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
	  theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_AMD4_SCHEMA_LOCATION);

          // Create the collection element for the descriptors
	  theCollection = theMpeg7Root.CreateChild("DescriptionUnit");
	  theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
          theCollectionInterface = theCollection.GetInterface();

          // Now create the XML parser based coding scheme
	  theDDLCS = new GenericDSCS(theDocumentInterface);

          // Obtain the interface
	  theDDLCSInterface = theDDLCS->GetInterface();

          // Connect the DDL file to the coding scheme
	  theDDLCSInterface->SetEncoderFileName(Bitstream);
	}
#endif // USEXML

	// descriptor
	VideoSignatureDescriptorInterfaceABC *theVideoSignatureDescriptorInterface;

	// extraction tool
	VideoSignatureExtractionTool *theExtractionTool;
	VideoSignatureExtractionInterfaceABC *theExtractionInterface;
	
	// coding schemes
	VideoSignatureCS *theVideoSignatureCS;
	VideoSignatureCSInterfaceABC *theVideoSignatureCSInterface;

	// get interfaces
	// Create media object 
	theMedia = new MultiMedia;

	// obtain media interface
	theMediaInterface=theMedia->GetInterface();

	// get extraction interface
	theExtractionTool = new VideoSignatureExtractionTool(0);
	theExtractionInterface = theExtractionTool->GetInterface();

	// get video signature interface
	theVideoSignatureDescriptorInterface =
	  theExtractionInterface->GetDescriptorInterface();

	// create coding scheme
	theVideoSignatureCS = new
	  VideoSignatureCS(theVideoSignatureDescriptorInterface);
	theVideoSignatureCSInterface = theVideoSignatureCS->GetInterface();

	if (CodingMode == 2)
	{
		// set compression flag for binary coding
		theVideoSignatureDescriptorInterface->SetCompressionFlag(CompressionFlag);
	}

	// link stream to coding scheme
	theVideoSignatureCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	theStreamBuffer.StartBitStream(Bitstream);

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	RamDB.CreateDatabase(ListFile);
	while (1) {

	  if (RamDB.DBOpenMPEGFile()==0) break;

	  // link media to extraction tool
	  theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
	  theExtractionInterface->InitExtracting();

	  // extract descriptor form media
	  if(theExtractionInterface->StartExtracting()!=0)
	  {
		  printf("Extraction error: %s\n", theMediaInterface->GetVideoFileName());
		  break;
	  }

	  if (CodingMode == 2)
	  {
	    // code descriptor data
	    theVideoSignatureCSInterface->StartEncode();
	  }
	  else
	  {
#ifdef USEXML
	    theVideoSignatureDescriptorInterface->ExportDDL(theCollectionInterface);
#endif // USEXML
	  }
	}

	// Tidy up memory allocations
	theExtractionInterface->destroy();

	if (CodingMode == 2)
	{
	  theVideoSignatureCSInterface->destroy();
	  theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML
	  theDDLCSInterface->StartEncode();
	  theDDLCSInterface->destroy();
#endif // USEXML
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureServer::Stop()
{
//	fprintf(stderr,"Not Implemented\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long VideoSignatureServer::Close()
{
//	fprintf(stderr,"Not Implemented\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& VideoSignatureServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoSignatureServer::GetName(void)
{ return myName; }



