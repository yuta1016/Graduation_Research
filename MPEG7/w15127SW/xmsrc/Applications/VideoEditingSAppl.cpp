///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Rosa Ruiloba 
// (contributing organizations names)
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoEditingSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
#include <direct.h>  // for _mkdir
#endif

#include "momusys.h"
#include "vopio.h"


#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif

#define WRITESCENEFRAMES

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */



using namespace XM;

const UUID VideoEditingServer::myID = UUID("");
const char *VideoEditingServer::myName = "VideoEditing DS Server";

//=============================================================================
VideoEditingServer::VideoEditingServer()
{
}

//----------------------------------------------------------------------------
VideoEditingServer::~VideoEditingServer()
{
}

//----------------------------------------------------------------------------
unsigned long VideoEditingServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoEditingServer::Start(char *ListFile, char *DDLFile)
{
        // Check that the file names are ok
  	assert(ListFile  != NULL);
  	assert(DDLFile != NULL);
	
	long FrameNo;
	
	unsigned long is_scene_frame;


	/*try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        fprintf( stderr, "ERROR(GenericDS): Error during Xerces-c Initialization: ");
		fprintf( stderr, (char *)toCatch.getMessage());
		exit(-1);
    }*/

	// Media data base
  	MediaDB RamDB;

  	// Media
 	 MultiMedia *theMedia;
  	 MultiMediaInterfaceABC *theMediaInterface;
  
  // This sets up the VideoEditing type description interface
  GenericDS theDescription = GenericDS::CreateDescriptionRoot();
  GenericDSInterfaceABC* theDescriptionInterface=
    theDescription.GetInterface();

// Extraction Tools

  VideoEditingExtractionTool  *theExtractionTool               = NULL;
  VideoEditingExtractionInterfaceABC *theExtractionInterface = NULL;
	
  // This sets up the coding scheme for this D/DType and the interface
  GenericDSCS *theCS                           = NULL;
  GenericDSCSInterfaceABC	*theCSInterface= NULL;
// Ok, make new instatiations of these datatypes
	try{
    // Create media object 
	
	theMedia = new MultiMedia;

    // obtain media interface
	theMediaInterface=theMedia->GetInterface();


    // Pass it null so that it creates a new description
	theExtractionTool = new 
	  VideoEditingExtractionTool(theDescriptionInterface);
    // Obtain the extraction interface from this extraction tool

	theExtractionInterface=theExtractionTool->GetInterface();

	/* get coding scheme interface*/
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface=theCS->GetInterface();
}
  catch(...) {
    fprintf(stderr,"VideoEditingServer::Start: Allocation error\n");
    return(1);
  }

	// All of the pointers should now be non-null so check them
  assert(theDescriptionInterface      != NULL);
  assert(theMedia                        != NULL);
  assert(theMediaInterface               != NULL);
  assert(theExtractionTool               != NULL);
  assert(theExtractionInterface          != NULL);
  assert(theCS                           != NULL);
  assert(theCSInterface                  != NULL);


		// Create an empty Document

		/* for writing scene frames*/

	 // ---------------------- Process the files -----------------------------
  
  // Connect media to media data base

	RamDB.SetMedia(theMediaInterface);
	
  // Create a database of all the files to process

	RamDB.CreateDatabase(ListFile);

	
	 // This example is for a video sequence
  // Loop over the files in the database
  int NDBaseEntries=RamDB.GetNoOfElements();
  printf("nbEntries:%d\n",NDBaseEntries);
  for(int ientry=0;ientry<NDBaseEntries;ientry++) {


	   int i;                /* frame of sequence*/
	   char ppmimage[25];    /* filename of single image*/
	  char path[MAXMEDIANAMELEN];

#ifdef WRITESCENEFRAMES
	  int scene=0;
#endif
	  if (!RamDB.DBDecodeMPEGVideo()) break; //cut sequence into parts

	printf("decoded\n");

	  /*********************************/
          /* create directory for sceneframes*/
          /* compute directory name from media filename*/
	  strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));
	  path[MAXMEDIANAMELEN-1]=0;

	  /* create directory*/
#ifndef _VISUALC_
	  mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	  _mkdir(path);
// 	  fprintf(stderr,"Warning : mkdir not supported by VC++, Please "
// 		  "check if directory\n");
#endif

	  	  
    // Tell the dummytype about the output stream
	  strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
	  strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
	  theCSInterface->SetEncoderFileName(path);

	

	
    // Loop over the frames of the sequence
	  i=0;
	  while (1) {

	    sprintf(ppmimage,"m2v/%d.ppm",i);
	    /* if no more images in sequence*/
	    if (!RamDB.LoadNamedImage(ppmimage)) break;

	    /* convert to apropriate color space*/
	    /*convertRGB_To_YUV(media,media);*/

//	    RamDB.StoreNamedImage("X:");
#ifdef XYUVDISP

	    /* display input image*/
	xyuv_show_simple(0,(unsigned short *)
			     theMediaInterface->GetImage()->y_chan->data->s,
			     (unsigned short *)
			     theMediaInterface->GetImage()->v_chan->data->s,
			     (unsigned short *)
			     theMediaInterface->GetImage()->u_chan->data->s,
			     datasize(theMediaInterface->GetImage()->y_chan)&1,
			     theMediaInterface->GetImage()->y_chan->x,
			     theMediaInterface->GetImage()->y_chan->y,
			     theMediaInterface->GetImage()->u_chan->x,
			     theMediaInterface->GetImage()->u_chan->y,
			     "input");
#endif
	// Set Data to SegmentExtraction
		theExtractionInterface->SetSourceMedia(theMediaInterface);
		//theExtractionInterface->SetSourceMedia(media, fp, pcFramesName, pcMasksName);


	    /* initialize values for the extraction before first frame*/
	    if (!i) theExtractionInterface->InitExtracting();
	    /* get frame number from image*/
	    {
	      FrameNo=theMediaInterface->GetImageTime();
	      if (FrameNo == -1) FrameNo=i;
	      theMediaInterface->SetImageTime(FrameNo);

	      /* Write first frame of sequence (only for GUI
		 and processing of sequences)*/
	      if (!FrameNo) {
		strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
		strncat(path,".dir/0.jpg",MAXMEDIANAMELEN-1-strlen(path));
		path[MAXMEDIANAMELEN-1]=0;
		RamDB.StoreNamedImage(path);
	      }

	    }

	    /* extract descriptor form media*/
	    
	    is_scene_frame=theExtractionInterface->StartExtracting();
	    #ifdef VERBOSE
	    printf("scene:%ld\n",is_scene_frame); 	

	    #endif /*VERBOSE*/
	    /****************************/
	    /* write scene frame to disk*/
	    /****************************/
#ifdef WRITESCENEFRAMES
	    if (is_scene_frame) {
	      char scenefilename[16];

	      /* compute scene cut filename*/
	      strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	      strncat(path,".dir/scene",MAXMEDIANAMELEN-1-strlen(path));
	      {
		char framenostr[10];
		sprintf(framenostr,"%05ld",FrameNo);
		strncat(path,framenostr,MAXMEDIANAMELEN-1-strlen(path));
	      }
	      strncat(path,".jpg",MAXMEDIANAMELEN-1-strlen(path));
	      path[MAXMEDIANAMELEN-1]=0;
	      RamDB.StoreNamedImage(path);

	      scene++;
	    }
#endif /*WERITESCENEFRAMES*/

	    i++;
	  }
// Collect partial results of the frame based extraction
    theExtractionInterface->PostExtracting();

    // Write the D/DSType to binary
	  theCSInterface->StartEncode();

	}


	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoEditingServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long VideoEditingServer::Close()
{
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& VideoEditingServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoEditingServer::GetName(void)
{ return myName; }



