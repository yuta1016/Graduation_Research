///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich LIS, S. Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// Applicable File Name:  ScalableColorSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"

#ifdef USEXML
extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;
#endif // USEXML

using namespace XM;


const UUID ScalableColorServer::myID = UUID("74abc540-db70-11d2-affe-0080c7e1e76d");
const char *ScalableColorServer::myName = "ScalableColorServer";

//=============================================================================
ScalableColorServer::ScalableColorServer()
{
}

//----------------------------------------------------------------------------
ScalableColorServer::~ScalableColorServer()
{
}

//----------------------------------------------------------------------------
unsigned long ScalableColorServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorServer::Start(char *ListFile, char *Bitstream)
{
	int CodingMode;
        //int bildnummer=0;
	/* image data base */
	MediaDB RamDB;

        /* image media*/
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;
	
	/* stream data base*/
	EncoderFileIO theStreamBuffer(0x1000);

        /* descriptor and sub-descriptor*/
	ScalableColorDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* extraction tool*/
	ScalableColorExtractionTool *theExtractionTool;
	ScalableColorExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
	ScalableColorCS *theCS;
	ScalableColorCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/

	/* create extraction tool*/
	/* get color histogram extraction interface*/
	theExtractionTool = new 
	  ScalableColorExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get color histogram interface*/
	theDescriptorInterface=
	  theExtractionInterface->
	  GetDescriptorInterface();

	// Get the coding scheme type
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "2"));
#else
	CodingMode = 2;
#endif // USEXML

#ifdef USEXML
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	
	GenericDS theMpeg7Root, theCollection;
	GenericDSInterfaceABC *theCollectionInterface;
	if (CodingMode == 2)
	{
	  /* create coding scheme*/
	  theCS = new 
  	  ScalableColorCS(theDescriptorInterface);
  	theCSInterface=theCS->GetInterface();

	  /* connect stream to coding scheme*/
	  theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	  theStreamBuffer.StartBitStream(Bitstream);
	}
	else
	{
		// Create the XML document
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();
		
		// Create the MPEG7 root element
		theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
		theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
		theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
		theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

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

		/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

  /* connect media to extraction tool*/
  theExtractionInterface->SetSourceMedia(theMediaInterface);


	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);
	//	printf("vor SetValuesFromParameterFile\n");


	/* read predefined descriptor values from paramter file*/
	//theDescriptorInterface->
	  //SetValuesFromParameterFile();

	/* loop all input media files*/
	while (1) {


	  /* load input data*/
	  //	 printf("******************bildnummer:%5d\n",bildnummer);
	  if (!RamDB.DBLoadImage()) break;

	  /* select VOP layer*/
	  if (theMediaInterface->GetImage()->a_chan) {
	    theMediaInterface->SelectVopAlphaLayer(ForegroundFromAlpha);
	  }
	  else {
	    theMediaInterface->SelectVopAlphaLayer(BackgroundFromAlpha);
	  }

	  //bildnummer=bildnummer+1;

	  /* reset histogram values*/
	  theExtractionInterface->InitExtracting();

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

		if (CodingMode == 2)
		{
	  /* code descriptor data*/
			theCSInterface->StartEncode();
		}
		else
		{
#ifdef USEXML
			theDescriptorInterface->ExportDDL(theCollectionInterface);
#endif // USEXML
		}

	}

	// Cleaning
  if (CodingMode == 2) // Binary coding
	{
		theCSInterface->destroy();
		theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML
		theDDLCSInterface->StartEncode();
		theDDLCSInterface->destroy();
#endif // USEXML
	}

	theMediaInterface->destroy();
	theExtractionInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long ScalableColorServer::Close()
{
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& ScalableColorServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorServer::GetName(void)
{ return myName; }



