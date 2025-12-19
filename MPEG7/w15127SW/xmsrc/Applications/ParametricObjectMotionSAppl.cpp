///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (coordinates, duration)
// - usage of parameter file to set model code
//
// Updated 02/21/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update, usage of MultiMedia class
//
// Updated 06/14/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - XML-IO
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
// Applicable File Name:  ParametricObjectMotionSAppl.cpp
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
#include "SearchUtilities/ParametricObjectMotionUtilities.h"
#include "Utilities/readparam.h"

#ifdef USEXML
extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;
#endif // USEXML

using namespace XM;

const UUID ParametricObjectMotionServer::myID = UUID("74abc540-db70-11d2-affe-0080c7e1e76d");
const char *ParametricObjectMotionServer::myName = "Parametric Object Motion Server";

//=============================================================================
ParametricObjectMotionServer::ParametricObjectMotionServer()
{
}

//----------------------------------------------------------------------------
ParametricObjectMotionServer::~ParametricObjectMotionServer()
{
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionServer::Open()
{

	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionServer::Start(char *ListFile, char *Bitstream)
{
  // ListFile should contains the two images for each "object"
  // should add here option file which contain the model type

	int CodingMode;
	
        /* image media*/
	MultiMedia *theMedia1;
	MultiMedia *theMedia2;

	MultiMediaInterfaceABC *theMediaInterface1;
	MultiMediaInterfaceABC *theMediaInterface2;

	MomVop *media1;
	MomVop *media2;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0xFFFF);

	/* Image data base */
	MediaDB RamDB;

        /* descriptor and sub-descriptor*/
	ParametricObjectMotionDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* extraction tool*/
	ParametricObjectMotionExtractionTool
	  *theExtractionTool;
	ParametricObjectMotionExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
	ParametricObjectMotionCS *theCS;
	ParametricObjectMotionCSInterfaceABC *theCSInterface;

	theExtractionTool = new 
	  ParametricObjectMotionExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get descriptor interface*/
	theDescriptorInterface=
	  theExtractionInterface->
	  GetDescriptorInterface();

	/* create coding scheme*/
	theCS = new 
	  ParametricObjectMotionCS(theDescriptorInterface);
	theCSInterface=theCS->GetInterface();

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

  if (CodingMode == 2) // Binary coding
	{
		/* connect stream to coding scheme*/
		theCSInterface->
		SetEncoderStreamBuffer(&theStreamBuffer);

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
		
	// create media objects
	theMedia1 = new MultiMedia;
	theMedia2 = new MultiMedia;
	// obtain media interfaces
	theMediaInterface1 = theMedia1->GetInterface();
	theMediaInterface2 = theMedia2->GetInterface();

	RamDB.CreateDatabase(ListFile);

	/* read predefined descriptor values from paramter file*/
	theDescriptorInterface->							// Aljoscha 12/13/00
	  SetValuesFromParameterFile();

	while (1) {

  RamDB.SetMedia(theMediaInterface1);
	
	  /* malloc media and load input data*/
		if (!RamDB.DBLoadImage()) break;

//		swap = theMediaInterface1;
//		theMediaInterface1 = theMediaInterface2;
//		theMediaInterface2 = swap;

  RamDB.SetMedia(theMediaInterface2);
	
		if (!RamDB.DBLoadImage()) break;

		
		media2=theMediaInterface2->GetImage();
		media1=theMediaInterface1->GetImage();

		// here add alpha if not available in Vop structure

		if( !GetMomVopA( media1 ) )
			{
				int ii;
				int width = (GetMomVopWidth(media1));
				int height = (GetMomVopHeight(media1));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media1));
				unsigned char *pixelval;

				media1->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)media1->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

	  if( !GetMomVopA( media2 ) )
			{
				int ii;
				int width = (GetMomVopWidth(media2));
				int height = (GetMomVopHeight(media2));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media2));
				unsigned char *pixelval;

				media2->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)media2->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

/*
	  UChar *UCrefTest = (UChar *) GetMomImageData(GetMomVopA(media1));
	  int width = (GetMomVopWidth(media1));
	  int height = (GetMomVopHeight(media1));
	  int ii,isAlpha=0;
	  for (ii=0;ii<height*width;ii++)
	    if (UCrefTest[ii]) {
	      isAlpha=1;
	      break;
	    }
	  UChar *UCcurrTest = (UChar *) GetMomImageData(GetMomVopA(media2));
	  width = (GetMomVopWidth(media2));
	  height = (GetMomVopHeight(media2));
	  for (ii=0;ii<height*width;ii++)
	    if (UCcurrTest[ii]) {
	      isAlpha=1;
	      break;
	    }
	  if (!isAlpha){
	    UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media1));
	    width = (GetMomVopWidth(media1));
	    height = (GetMomVopHeight(media1));
	    unsigned char *pixelval;
	    for (ii=0,pixelval=(unsigned char *)media1->a_chan->f;ii<height*width;
		 *(pixelval++)=(unsigned char)UCref[ii],ii++);

	    UChar *UCcurr = (UChar *) GetMomImageData(GetMomVopY(media2));
	    width = (GetMomVopWidth(media2));
	    height = (GetMomVopHeight(media2));
	    for (ii=0,pixelval=(unsigned char *)media2->a_chan->f;ii<height*width;
		 *(pixelval++)=(unsigned char)UCcurr[ii],ii++);
	  }
*/
// end add alpha if not available in Vop structure

	  /* connect media to extraction tool*/
	  theExtractionInterface=theExtractionTool->GetInterface();
	  theExtractionInterface->SetSourceMedia(media2,media1);
	  
// default configuration Aljoscha 12/13/00
		theDescriptorInterface->SetCoordFlag(0);	// using embedded coordinates
		theDescriptorInterface->SetXOrigin(0.0);	// embedded coordinates
		theDescriptorInterface->SetYOrigin(0.0);	// embedded coordinates
		theDescriptorInterface->SetDuration(1);	  // time information

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

		if (CodingMode == 2)
		{
			// code descriptor data
			theCSInterface->StartEncode();
		}
		else
		{
#ifdef USEXML
			theDescriptorInterface->ExportDDL(theCollectionInterface);
#endif // USEXML
		}
	}

	theMediaInterface1->destroy();
	theMediaInterface2->destroy();
	theExtractionInterface->destroy();

	if (CodingMode == 2)
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
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionServer::Stop()
{
//	fprintf(stderr,"\nStop method in ParametricObjectMotion Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionServer::Close()
{


//	fprintf(stderr,"\nClose method in ParametricObjectMotion Not Implemented");
	return 0;

}

	
//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionServer::GetName(void)
{ return myName; }



