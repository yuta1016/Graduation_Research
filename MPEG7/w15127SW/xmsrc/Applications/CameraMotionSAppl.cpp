//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips
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
// Applicable File Name:  CameraMotionSAppl.cpp
//

#include <stdio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif



#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
#include "Applications/ServerApplications.h"

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;


const UUID CameraMotionServer::myID = UUID("9d5ffae0-ce8d-11d3-82cc-0050041cc4a9");
const char *CameraMotionServer::myName = "Camera Motion Server";

//=============================================================================
CameraMotionServer::CameraMotionServer()
{
}

//----------------------------------------------------------------------------
CameraMotionServer::~CameraMotionServer()
{
}

//----------------------------------------------------------------------------
unsigned long CameraMotionServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionServer::Start(char *ListFile, char *Bitstream)
{
	/* Check that the file names are ok */	//0719-Sylvie
	assert(ListFile		!= NULL);			//0719-Sylvie
	assert(Bitstream	!= NULL);			//0719-Sylvie

	/* shot media*/
	MultiMedia *media;
	MultiMediaInterfaceABC *mediaInterface;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0x1000);

	/* Shot data base */
	MediaDB RamDB;

	/* descriptor and sub-descriptor*/
	CameraMotionDescriptorInterfaceABC *theCameraMotionDescriptorInterface;

	// used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface     = NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface	= NULL;

	/* extraction tool*/
	CameraMotionExtractionTool *theExtractionTool;
	CameraMotionExtractionInterfaceABC *theExtractionInterface;
	
	/* coding schemes*/
	int CodingMode;	// 0:DDL, 1:BiM, 2:Binary
	//CameraMotionCS *theCameraMotionCS;
	//CameraMotionCSInterfaceABC *theCameraMotionCSInterface;
	CameraMotionCS *theBinCS			= NULL;
	CameraMotionCSInterfaceABC *theBinCSInterface	= NULL;
	GenericDSCS *theDDLCS				= NULL;
	GenericDSCSInterfaceABC *theDDLCSInterface	= NULL;

	/* create extraction and descriptor */
	theExtractionTool = new CameraMotionExtractionTool(0) ;
	theExtractionInterface = theExtractionTool->GetInterface() ;

	/* get descriptor interface */
	theCameraMotionDescriptorInterface =   theExtractionInterface->GetDescriptorInterface();

	   /* select coding scheme type */										//0719-Sylvie
#ifdef USEXML																//0719-Sylvie
	   CodingMode = atoi(getpara("CodingMode", "2")); // default is binary	//0719-Sylvie
#else																		//0719-Sylvie
	   CodingMode = 2; // binary mode										//0719-Sylvie
#endif																		//0719-Sylvie

	if(CodingMode == 2) {	// binary coding								//0719-Sylvie
	   /* create coding scheme and connect it to stream*/
	   //theCameraMotionCS = new CameraMotionCS(theCameraMotionDescriptorInterface) ;	//0719-Sylvie
	   theBinCS = new CameraMotionCS(theCameraMotionDescriptorInterface);				//0719-Sylvie
	   //theCameraMotionCSInterface = theCameraMotionCS->GetInterface() ;				//0719-Sylvie
	   theBinCSInterface=theBinCS->GetInterface();										//0719-Sylvie
	}
	else {					// DDL or BiM coding			// all the else statement: //0719-Sylvie
		/* create the XML document */
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();

		/* create the Mpeg7Root element */
		theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
		theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
		theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
		theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

		/* create the collection element for the descriptors */
		theCollection = theMpeg7Root.CreateChild("DescriptionUnit");
		theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
		theCollectionInterface = theCollection.GetInterface();

		/* now create the XML parser based coding scheme */
		theDDLCS = new GenericDSCS(theDocumentInterface);

		/* obtain the interface */
		theDDLCSInterface = theDDLCS->GetInterface();

		/* connect DDL file to coding scheme */
		theDDLCSInterface->SetDecoderFileName(Bitstream);
	} // end of the else statement added //0719-Sylvie

	/* connect stream to coding scheme */										//0719-Sylvie
	if (CodingMode == 2) {	// Binary coding									//0719-Sylvie
		//theCameraMotionCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);	//0719-Sylvie
		theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);			//0719-Sylvie
		theStreamBuffer.StartBitStream(Bitstream);
	}																			//0719-Sylvie
	else {					// DDL or BiM coding								//0719-Sylvie
		theDDLCSInterface->SetEncoderFileName(Bitstream);						//0719-Sylvie
	}																			//0719-Sylvie

	fprintf(stderr," List file : %s \n",ListFile) ;
	fprintf(stderr," BitStream : %s \n",Bitstream) ;

	RamDB.CreateDatabase(ListFile);
	//2013/03/01 fix - create media object.
	media = new MultiMedia();
	mediaInterface = media->GetInterface();

	for (int i=0; i<RamDB.GetNoOfElements(); i++)
	{
	  fprintf(stderr," %s\n",RamDB.GotoEntry(i)->GetFilename()) ;
	  //media = ComputeFromText(RamDB.GotoEntry(i)->GetFilename()) ;
	  media->SetVideoFileName( RamDB.GotoEntry(i)->GetFilename() );

	  /* malloc media and load input data*/
	  if (!media) break;

	  /* connect media to extraction tool*/
	  theExtractionInterface->SetSourceMedia(mediaInterface);

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

	  /* code descriptor data*/
	  if(CodingMode == 2) { // binary coding								//0719-Sylvie
	     theBinCSInterface->StartEncode();									//0719-Sylvie
	  //theCameraMotionCSInterface->StartEncode();							//0719-Sylvie
	  }																		//0719-Sylvie
	  else {				// export description to collection DDL tree	//0719-Sylvie
		 theCameraMotionDescriptorInterface->ExportDDL(theCollectionInterface);			//0719-Sylvie
	  }																		//0719-Sylvie
	}

	//theStreamBuffer.CloseBitStream();

	theExtractionInterface->destroy() ;
	
	if(CodingMode == 2) {   // binary coding	//0719-Sylvie
	   theBinCSInterface->destroy();			//0719-Sylvie
	//theCameraMotionCSInterface->destroy() ;	//0719-Sylvie
   	   theStreamBuffer.CloseBitStream();		//0719-Sylvie
	}											//0719-Sylvie
	else    {									//0719-Sylvie
#ifdef USEXML									//0719-Sylvie
	   // write CameraMotion to DDL				//0719-Sylvie
	   theDDLCSInterface->StartEncode();		//0719-Sylvie
	   theDDLCSInterface->destroy();			//0719-Sylvie
#endif											//0719-Sylvie
	}											//0719-Sylvie

	return 0;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionServer::Stop()
{
	fprintf(stderr,"CameraMotionServer Stopped\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long CameraMotionServer::Close()
{
	fprintf(stderr, "CameraMotionServer Closed\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& CameraMotionServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionServer::GetName(void)
{ return myName; }



