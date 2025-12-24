///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Applicable File Name:  FaceRecognitionSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <direct.h>
#endif

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

#ifdef USEXML
extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;
#endif // USEXML

using namespace XM;

const UUID FaceRecognitionServer::myID = UUID(" ");
const char *FaceRecognitionServer::myName = "Face Recognition Server";

//=============================================================================
FaceRecognitionServer::FaceRecognitionServer()
{
}

//----------------------------------------------------------------------------
FaceRecognitionServer::~FaceRecognitionServer()
{
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionServer::Start(char *ListFile, char *Bitstream)
{
	int CodingMode;

        /* image media*/
        MomVop *media=0;  //sth use MultiMedia for further development
	MomVop *facemedia=0;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0xFFFF);

	/* Image data base */
	MediaDB RamDB;

	/* descriptor*/
	FaceRecognitionDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* extraction tool*/
	Cface_detect  Faced;
	FaceRecognitionExtractionTool *theExtractionTool;
	FaceRecognitionExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
	FaceRecognitionCS *theCS;
	FaceRecognitionCSInterfaceABC *theCSInterface;


	/*****************/
	/* get interfaces*/
	/* create extraction tool*/
	/* get Face Recognition extraction interface*/
	theExtractionTool = new 
	  FaceRecognitionExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get Face Recognition interface */
	theDescriptorInterface=theExtractionInterface->GetDescriptorInterface();

	
	/* Get the coding mode type */
	#ifdef USEXML
		CodingMode = atoi(getpara("CodingMode", "2"));
	#else
		CodingMode = 2;
	#endif // USEXML

	
#ifdef USEXML
	/* variables for XML I/O */
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	
	GenericDS theMpeg7Root, theCollection;
	GenericDSInterfaceABC *theCollectionInterface;
	if (CodingMode == 2)
	{
		// create coding scheme
		theCS = new FaceRecognitionCS(theDescriptorInterface);
		theCSInterface = theCS->GetInterface();
		// connect stream to coding scheme
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


	RamDB.CreateDatabase(ListFile);

//	theDescriptorInterface->SetValuesFromParameterFile();

	while (1) {

	  int face_cnt;
	  char path[MAXMEDIANAMELEN];

	  /* malloc media and load input data*/
	  media=RamDB.DBLoadImage();
	  if (!media) break;

	  //sth put your detection stuff here after loading the file??


////////////////////////////////////////////////////////////
//start face detection
	  printf("start face detection\n");
	  int face_num;
	  char *SingleFile;
	  //	  Simple_FACE s_face[50]; //sth why here? I thought Faced is an object!
	  //	media=RamDB.LoadNamedImage(ListFile);
	  /*********************************/
	  /* create directory for faces*/
	  /* compute directory name from media filename*/
	  SingleFile=RamDB.GetMediaFilename();

	  {
	    //	    char path[MAXMEDIANAMELEN];

	    strncpy(path,SingleFile,MAXMEDIANAMELEN-1);
	    strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));
	    /* create directory*/
#ifndef _VISUALC_
	    mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	    _mkdir(path);
#endif
	  }

	  //sth redundant: if(!media) return((unsigned long)-1);
	  face_num=Faced.call_FaceDetect(SingleFile,media);
	  fprintf(stderr,"face_num=%d\n",face_num);
	  fprintf(stderr,"finish face detection\n");


//end face detection
////////////////////////////////////////////////////////////

	  for (face_cnt=0; face_cnt <face_num; face_cnt++) { 

	    {
	      char face_path[MAXMEDIANAMELEN];
	      char face_image[12];

	      sprintf(face_image,"/Face%02d.jpg",face_cnt);
	      strncpy(face_path,path,MAXMEDIANAMELEN-1);
	      strncat(face_path,face_image,
		      MAXMEDIANAMELEN-1-strlen(face_path));

	      // segment face from media
	      facemedia=Faced.GetFaceImage(face_cnt);

#ifdef XYUVDISP
	    /* display input image*/
	      xyuv_show_simple(0,(unsigned short *)
			     facemedia->y_chan->data->s,
			     (unsigned short *)
			     facemedia->v_chan->data->s,
			     (unsigned short *)
			     facemedia->u_chan->data->s,
			     datasize(facemedia->y_chan)&1,
			     facemedia->y_chan->x,
			     facemedia->y_chan->y,
			     facemedia->u_chan->x,
			     facemedia->u_chan->y,
			     "input");
#endif

	      unlink(face_path);
	      RamDB.StoreNamedVop(face_path, facemedia);
	    }

	    /* connect media to extraction tool*/
	    theExtractionInterface=theExtractionTool->GetInterface();
	    theExtractionInterface->SetSourceMedia(facemedia);

	    /* reset eigenfeature values */
	    theExtractionInterface->InitExtracting();  
	  
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
#endif 
		  }
	    /*free media */
/*	    freevop(media);*/
	    //	  break;
	  }
	}

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
	  
	theExtractionInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long FaceRecognitionServer::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& FaceRecognitionServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionServer::GetName(void)
{ return myName; }
