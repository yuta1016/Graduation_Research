//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath-UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivitySAppl.cpp
//

#include <stdio.h>
#include <string.h>

#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
#endif

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

const UUID MotionActivityServer::myID = UUID("74abc540-db70-11d2-affe-0080c7e1e76d");
const char *MotionActivityServer::myName = "Motion Activity Server";

//=============================================================================
MotionActivityServer::MotionActivityServer()
{
}

//----------------------------------------------------------------------------
MotionActivityServer::~MotionActivityServer()
{
}

//----------------------------------------------------------------------------
unsigned long MotionActivityServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityServer::Start(char *ListFile, char *Bitstream)
{
	int CodingMode;
  	/* Shot data base */
	MediaDB RamDB;
        /* image media*/
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0x1000);

        /* descriptor and sub-descriptor*/
	MotionActivityDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* extraction tool*/
	MotionActivityExtractionTool *theExtraction;
	MotionActivityExtractionInterfaceABC
	  *theExtractionInterface;

	// This descriptors are used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface = NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC * theCollectionInterface = NULL;
	GenericDSCS* theDDLCS = NULL;
	GenericDSCSInterfaceABC* theDDLCSInterface = NULL;
	/* coding schemes*/
	MotionActivityCS *theCS;
	MotionActivityCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/
	/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	/* create extraction tool*/
	/* get  extraction interface*/
	theExtraction = new 
	  MotionActivityExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtraction->GetInterface();

	/* get  interface*/
	theDescriptorInterface=
	  theExtractionInterface->
	  GetDescriptorInterface();

	//select coding scheme type
#ifdef USEXML
	//CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
	CodingMode=atoi(getpara("CodingMode","0"));//for XML
	//CodingMode=atoi(getpara("CodingMode","1"));//for BiM
#else
	CodingMode=2; //Binary mode
#endif

	if (CodingMode==2)
	{
	/* create coding scheme*/
	theCS= new 
	  MotionActivityCS(theDescriptorInterface);
	theCSInterface=theCS->GetInterface();
	}
else { // DDL or BiM coding
      // Create the XML document
      theDocument = GenericDS::CreateDescriptionRoot();
      theDocumentInterface=theDocument.GetInterface();

      // Create the Mpeg7Root element
      theMpeg7Root=theDocumentInterface->CreateChild("Mpeg7");
	  theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
	  theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
	  theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

      // Create the Collection element for the descriptors
      theCollection=theMpeg7Root.CreateChild("DescriptionUnit");
      theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
      theCollectionInterface=theCollection.GetInterface();

      // Now create the XML parser based coding scheme
      theDDLCS = new GenericDSCS(theDocumentInterface);

      // Obtain the interface
      theDDLCSInterface=theDDLCS->GetInterface();

      // Connect DDL file to coding scheme
      theDDLCSInterface->SetDecoderFileName(Bitstream);
    }

	/* connect media to extraction tool*/
	theExtractionInterface->SetSourceMedia(theMediaInterface);
if (CodingMode==2)
{
	/* connect stream to coding scheme*/
	theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	theStreamBuffer.StartBitStream(Bitstream);

}
else { // DDL or BiM coding
    theDDLCSInterface->SetEncoderFileName(Bitstream);
  }

	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);

	/* set predefined descriptor values*/
	// blank

	while (1) {

	  int i;                /* frame of sequence*/
	  char vectorimage[25]; /*filename of image containing motion vector*/

	  if (!RamDB.DBExtractMPEGMotionVectors()) break;

	  /* initialize descriptor for extarction of first frame*/
	  theExtractionInterface=theExtraction->GetInterface();

	  /* initialize values for the extarction before first frame*/
	  /* theExtractionInterface->InitExtraction();*/

	  /* loop frame of vector file*/
	  i=0;
	  while (1) {

	    /* malloc media and load input data*/
	    sprintf(vectorimage,"vectorfile.ppm[%d]",i);
	    /* if no more images in sequence*/
		MomVop *vop = RamDB.LoadNamedImage(vectorimage);
	    if (!vop || vop->scene != i) break;

	    /* connect media to extarction tool*/
	    theExtractionInterface->SetSourceMedia(theMediaInterface);

	    /* get frame number from image*/
	    /* theExtractionInterface->
	       SetSourceMediaFrameNo(GetFrameNoFromComment());*/

	    /* extract descriptor form media*/
	    theExtractionInterface->StartExtracting();

	    i++;
	  }

	  theExtractionInterface->PostExtracting(); // for SLMA
		if (CodingMode==2)
		{
			/* code descriptor data*/
			theCSInterface->StartEncode();
		}
		else
		{
			// Export description to Collection DDL Tree
			theDescriptorInterface->ExportDDL(theCollectionInterface);
		}

	}
	theMediaInterface->destroy();
	theExtractionInterface->destroy();
	if(CodingMode == 2)
	{
		theCSInterface->destroy();

		theStreamBuffer.CloseBitStream();
	}
	else
	{
		// Write the D/DSType to DDL
		theDDLCSInterface->StartEncode();

		theDDLCSInterface->destroy();
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityServer::Stop()
{
	//fprintf(stderr,"Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityServer::Close()
{
	//fprintf(stderr,"Not Implemented");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MotionActivityServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityServer::GetName(void)
{ return myName; }



