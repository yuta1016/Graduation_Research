///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// Yousri Abdeljaoued (EPFL Switzerland)
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SummarizationSAppl.cpp
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


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */


#define WRITESCENEFRAMES

using namespace XM;

const UUID SummarizationServer::myID = UUID("");
const char *SummarizationServer::myName = "SummarizationSAppl";

//=============================================================================
SummarizationServer::SummarizationServer()
{
}

//----------------------------------------------------------------------------
SummarizationServer::~SummarizationServer()
{
}

//----------------------------------------------------------------------------
unsigned long SummarizationServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationServer::Start(char *ListFile, char *DDLFile)
{
        /* media data*/
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;
	long FrameNo;

	/* for writing scene frames*/
	unsigned long is_scene_frame;

	/* Image data base */
	MediaDB RamDB;

	/* description*/
	GenericDS theDocument = GenericDS::CreateDescriptionRoot();
	/* get descriptor interface*/
	GenericDSInterfaceABC *theDocumentInterfaceABC=
	  theDocument.GetInterface();

	/* extraction tool*/
	SummarizationExtractionTool *theExtractionTool;
	SummarizationExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
  	GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/
	/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	/* create extraction tool*/
	/* get extraction interface*/
	theExtractionTool = new 
	  SummarizationExtractionTool(theDocumentInterfaceABC);
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get coding scheme interface*/
	theCS = new GenericDSCS(theDocumentInterfaceABC);
	theCSInterface=theCS->GetInterface();

	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);

	/* set predifined descriptor values*/

	while (1) {

	  int i;                /* frame of sequence*/
	  char ppmimage[25];    /* filename of single image*/
	  char path[MAXMEDIANAMELEN];

#ifdef WRITESCENEFRAMES
	  int scene=0;
#endif
	  if (!RamDB.DBDecodeMPEGVideo()) break;


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

	  /* connect output file to coding scheme*/
	  strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
	  strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
	  theCSInterface->SetEncoderFileName(path);

	  /* reset descrition data*/
// 	  theDescriptionInterfaceABC->InitDescriptor();
	    
	  /* loop frame of vector file*/
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

	    /* connect media to extraction tool*/
	    theExtractionInterface->SetSourceMedia(theMediaInterface);

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

	  /* theExtractionInterface->PostExtraction();*/

	  /* code descriptor data*/
	  theCSInterface->StartEncode();

	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SummarizationServer::Close()
{
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SummarizationServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationServer::GetName(void)
{ return myName; }



