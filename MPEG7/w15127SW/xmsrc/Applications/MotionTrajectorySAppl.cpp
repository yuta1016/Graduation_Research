///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sylvie JEANNIN
// Philips Research USA
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
// Applicable File Name:  MotionTrajectorySAppl.cpp
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

/* should be including FileIO.h in Utilities.h */

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;

const UUID MotionTrajectoryServer::myID = UUID("");
const char *MotionTrajectoryServer::myName = "MotionTrajectoryServer";

//=============================================================================
MotionTrajectoryServer::MotionTrajectoryServer()
{
}

//----------------------------------------------------------------------------
MotionTrajectoryServer::~MotionTrajectoryServer()
{
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryServer::Start(char *ListFile, char *Bitstream)
{

	/* Check that the file names are ok */
	assert(ListFile		!= NULL);
	assert(Bitstream	!= NULL);

	/****************/
	/* DECLARATIONS */
	/****************/

	/* stream data base */
	EncoderFileIO theStreamBuffer(0xFFFF);

	/* data base of key points files */
 	MediaDB RamDB;

	/* media and its interface */
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	/* descriptor (and sub-descriptor) */
	MotionTrajectoryDescriptorInterfaceABC *theDescriptorInterface	= NULL;

	// used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface			= NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface			= NULL;

	/* extraction tool and its interface */
	MotionTrajectoryExtractionTool *theExtractionTool		= NULL;
	MotionTrajectoryExtractionInterfaceABC *theExtractionInterface	= NULL;

	/* coding scheme and its interface */
	int CodingMode;	// 0:DDL, 1:BiM, 2:Binary
	MotionTrajectoryCS *theBinCS					= NULL;
	MotionTrajectoryCSInterfaceABC *theBinCSInterface		= NULL;
	GenericDSCS *theDDLCS						= NULL;
	GenericDSCSInterfaceABC *theDDLCSInterface			= NULL;


	/*******************************************/
	/* GET INTERFACES, CONNECT THINGS TOGETHER */
	/*******************************************/

	try
	{
	   /* create media data class and get interface*/
	   theMedia = new MultiMedia;
	   theMediaInterface=theMedia->GetInterface();

	   /* connect descriptor to extraction tool, get extraction interface
	       - create also descriptor */
	   theExtractionTool = new MotionTrajectoryExtractionTool(0);
	   theExtractionInterface=theExtractionTool->GetInterface();

	   /* get motion Trajectory interface */
	   theDescriptorInterface=theExtractionInterface->GetDescriptorInterface();

	   /* select coding scheme type */
#ifdef USEXML
	   CodingMode = atoi(getpara("CodingMode", "2")); // default is binary
#else
	   CodingMode = 2; // binary mode
#endif

	   if(CodingMode == 2) {	// binary coding
	      /* connect descriptor to coding scheme */
	      theBinCS = new MotionTrajectoryCS(theDescriptorInterface);
	      theBinCSInterface=theBinCS->GetInterface();
	   }
	   else {					// DDL or BiM coding
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
	   }
	}

	catch(...)  {
	   fprintf(stderr, "MotionTrajectoryServer::Start: Allocation error!\n");
	   return(1);
	}

	/* all of the pointers should now be non-null : check */
	assert(theDescriptorInterface          != NULL);
	assert(theMedia                        != NULL);
	assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	if (CodingMode == 2)	{	// binary coding
		assert(theBinCS                      != NULL);
		assert(theBinCSInterface             != NULL);
	}
	else	{					// DDL or BiM coding
		assert(theDocumentInterface          != NULL);
		assert(theCollectionInterface        != NULL);
		assert(theDDLCS                      != NULL);
		assert(theDDLCSInterface             != NULL);
	}

	/* connect stream to coding scheme */
	if (CodingMode == 2) {	// Binary coding
		theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
	}
	else {					// DDL or BiM coding
		theDDLCSInterface->SetEncoderFileName(Bitstream);
	}

	/* connect media to extraction tool */
	int check_int;
	check_int = theExtractionInterface->SetSourceMediaInterface(theMediaInterface);


	// create database from file "ListFile"
	RamDB.SetMedia(theMediaInterface);
 	RamDB.CreateDatabase(ListFile);

	/* set predefined descriptor values */
	// TODO or TOBEDONE in other function
	  /* theMotionTrajectoryDescriptorInterface->SetUseDefaultInterpOnly(1); */



	/*******************/
	/* EXTRACTION LOOP */
	/*******************/

	int KeyPointNumber;
	while (1) {
	  /* load input data: initialise m_KeyPointFileName, m_KeyPointNo, m_KeyPoint */
	  KeyPointNumber = RamDB.DBLoadKeyPointListSylvie();	// SJ-0621
	  //KeyPointNumber = RamDB.DBLoadKeyPointList();// SJ-0621
	  if (!KeyPointNumber) break;

	  //sth dont´ write to stdout
	  //cout << "   Extracting ... " << KeyPointNumber << " keypoints;\n";

	  T4DKeyPoint *PointerOnKeyPoints;// SJ-0621
	  //TnDKeyPoint *PointerOnKeyPoints;
	  //PointerOnKeyPoints = theMediaInterface->GetKeyPoint();
	  //for (int bibi=0; bibi<KeyPointNumber; bibi++)
	    // cout << PointerOnKeyPoints[bibi].time << "  "
	      //    << PointerOnKeyPoints[bibi].p[0] << "  "
	        //  << PointerOnKeyPoints[bibi].p[1] << "  "
	          //<< PointerOnKeyPoints[bibi].p[2] << "\n";

	  PointerOnKeyPoints = theMediaInterface->GetKeyPointPointer();
	  //for (int bibi=0; bibi<KeyPointNumber; bibi++)
	  //   cout << PointerOnKeyPoints[bibi].time << "  "
	  //        << PointerOnKeyPoints[bibi].x_position << "  "
	  //        << PointerOnKeyPoints[bibi].y_position << "  "
	  //        << PointerOnKeyPoints[bibi].z_position << "\n";

#ifdef XYUVDISP
	 /*could here display trajectory as an image: have fct to build VOP from file*/
#endif

	  /* extract descriptor from media*/
	  // theExtractionInterface->InitExtracting();      either here or before loop
	  theExtractionInterface->StartExtracting(KeyPointNumber);
	  /* extract the rough D without further processing (simple mapping from data) */

	  /* then eventually use extraction of TemporalInterpolation on top */
	  //if ()
	  //theExtractionInterface->InterpolateKeypoints(KeyPointNumber);

	  /* code descriptor data*/
	  if(CodingMode == 2) { // binary coding
	     theBinCSInterface->StartEncode();
	  }
	  else {				// export description to collection DDL tree
		 theDescriptorInterface->ExportDDL(theCollectionInterface);
	  }
	}

	theMediaInterface->destroy();
	theExtractionInterface->destroy();
	if(CodingMode == 2) {   // binary coding
	   theBinCSInterface->destroy();
   	   theStreamBuffer.CloseBitStream();
	}
	else    {
#ifdef USEXML
	   // write MotionTrajectory to DDL
	   theDDLCSInterface->StartEncode();
	   theDDLCSInterface->destroy();
#endif
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryServer::Close()
{
	return 0;
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryServer::GetName(void)
{ return myName; }



