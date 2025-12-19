///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
//
// Modified by
// Min-Seok Choi - Hanyang University, Korea
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
// Applicable File Name:  RegionShapeSAppl.cpp
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

#ifdef USEXML
extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;
#endif // USEXML

using namespace XM;

const UUID RegionShapeServer::myID = UUID("");
const char *RegionShapeServer::myName = "Region Shape Server";

//=============================================================================
RegionShapeServer::RegionShapeServer()
{
}

//----------------------------------------------------------------------------
RegionShapeServer::~RegionShapeServer()
{
}

//----------------------------------------------------------------------------
unsigned long RegionShapeServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionShapeServer::Start(char *ListFile, char *Bitstream)
{
        /* image media*/
        MultiMediaInterfaceABC *theMultiMediaInterface;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0x1000);

	int CodingMode;

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
#endif

	/* Image data base */
	MediaDB RamDB;

        /* descriptor and sub-descriptor*/
	RegionShapeDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* extraction tool*/
	RegionShapeExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
	RegionShapeCSInterfaceABC *theCSInterface;

	/* create extraction and descriptor*/
	{
	  RegionShapeExtractionTool *theExtractionTool;
	  theExtractionTool = new
	    RegionShapeExtractionTool(0);
	  theExtractionInterface = theExtractionTool->GetInterface();
	}

	/* get descriptor interface*/
	theDescriptorInterface =
		theExtractionInterface->GetDescriptorInterface();

	if( CodingMode == 2 )
	{
	        RegionShapeCS *theCS;
		/* create coding scheme and connect descriptor*/
		theCS = new
		  RegionShapeCS(theDescriptorInterface);
		theCSInterface = theCS->GetInterface();

		/* connect stream to coding scheme*/
		theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
	}		
	else		
	{
#ifdef USEXML
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
#endif // USEXML
	}	

	/* create MultiMedia object*/
	{
	  MultiMedia *theMultiMedia;
	  theMultiMedia=new MultiMedia;
	  theMultiMediaInterface=theMultiMedia->GetInterface();
	}

	RamDB.CreateDatabase(ListFile);
	RamDB.SetMedia(theMultiMediaInterface);

	/* set predifined descriptor values*/

	while (1) {

	  /* malloc media and load input data*/
	  if (!RamDB.DBLoadImage()) break;
		
		/* connect media to extraction tool*/
		theExtractionInterface->SetSourceMedia(theMultiMediaInterface);
		
		/* extract descriptor form media*/
		theExtractionInterface->StartExtracting();
		
		
		if( CodingMode == 2 )
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
		
		/* code descriptor data*/
		//theCSInterface->StartEncode();
	}

	theExtractionInterface->destroy();
	theMultiMediaInterface->destroy();

	if( CodingMode == 2 )
	{
		theStreamBuffer.CloseBitStream();	
		theCSInterface->destroy();
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
unsigned long RegionShapeServer::Stop()
{
	fprintf(stderr,"RegionShapeServer Stopped\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long RegionShapeServer::Close()
{
	fprintf(stderr, "RegionShapeServer Closed\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& RegionShapeServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeServer::GetName(void)
{ return myName; }



