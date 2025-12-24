///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// 
// Modifications to this base code have been done to enable TimeStamp generation by
// Dulce Ponceleon and Jan H. Pieper (Almaden Research Center, IBM, USA)
// 
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
// Applicable File Name:  SeqSummaryMovingStoryBoardSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
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


#define WRITEKEYFRAMES

using namespace XM;

const UUID SeqSummaryMovingStoryBoardServer::myID = UUID("");
const char *SeqSummaryMovingStoryBoardServer::myName = "SeqSummaryMovingStoryBoardSAppl";

//=============================================================================
SeqSummaryMovingStoryBoardServer::SeqSummaryMovingStoryBoardServer()
{
}

//----------------------------------------------------------------------------
SeqSummaryMovingStoryBoardServer::~SeqSummaryMovingStoryBoardServer()
{
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryMovingStoryBoardServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryMovingStoryBoardServer::Start(char *ListFile, char *unused)
{
	/* image media*/
	MomVop *media;
	long FrameNo;

	/* for writing key frames*/
	unsigned long is_key_frame;

	/* Image data base */
	MediaDB RamDB;

	/* extraction tool*/
	SeqSummaryMovingStoryBoardExtractionTool *theExtractionTool;
	SeqSummaryMovingStoryBoardExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
  	GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/
	/* create extraction tool*/
	/* get extraction interface*/
	theExtractionTool = new SeqSummaryMovingStoryBoardExtractionTool;
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get coding scheme interface*/
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	RamDB.CreateDatabase(ListFile);

	/* set predifined descriptor values*/

	while (1) {

	  int i;                /* frame of sequence*/
	  char ppmimage[25];    /* filename of single image*/
	  char path[MAXMEDIANAMELEN];

	  if (!RamDB.DBDecodeMPEGVideo()) break;


	  /*********************************/
          /* create directory for keyframes*/
          /* compute directory name from media filename*/
	  strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

	  /* create directory*/
#ifndef _VISUALC_
	  mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	  fprintf(stderr,"Wraning : mkdir not supported by VC++, Please "
		  "check if directory\n");
#endif

	  {
	    /* reset description data*/
	    /* create XML Document for Description*/
	    GenericDS theDocument = GenericDS::CreateDescriptionRoot();
	    /* get descriptor interface*/
	    GenericDSInterfaceABC *theDocumentInterfaceABC=
	      theDocument.GetInterface();

	    /* connect descriptor to extraction tool*/
	    theExtractionInterface->
	      SetDescriptorInterface(theDocumentInterfaceABC);

	    /* connect descriptor to coding scheme*/
	    theCSInterface->
	      SetDescriptionInterface(theDocumentInterfaceABC);

	    /* connect output file to coding scheme*/
	    strncat(path,"/SeqSummaryMovingStoryBoardDS.ddl",MAXMEDIANAMELEN-1-strlen(path));
	    theCSInterface->SetEncoderFileName(path);

	    /* allow file IO for prelimenary results*/
	    theExtractionInterface->
	      SetSourceMediaFilename(RamDB.GetMediaFilename());
// 	    theExtractionInterface->SetSourceMediaFilename("test.mpg");

	    /* reset descrition data*/
// 	    theDescriptionInterfaceABC->InitDescriptor();
	    
	    /* loop frame of vector file*/
	    i=0;
	    while (1) {

	      /* malloc media and load input data*/
	      sprintf(ppmimage,"m2v/%d.ppm",i);
	      media=RamDB.LoadNamedImage(ppmimage);

	      if (!media) break; /* if no more images in sequence*/

	      /* convert to apropriate color space*/
	      convertRGB_To_YUV(media,media);

#ifdef XYUVDISP
	      /* display input image*/
	      xyuv_show_debug(0,(unsigned short *)media->y_chan->data->s,
			      (unsigned short *)media->u_chan->data->s,
			      (unsigned short *)media->v_chan->data->s,
			      datasize(media->y_chan)&1,
			      media->y_chan->x,media->y_chan->y,
			      media->u_chan->x,media->u_chan->y,
			      "input",0,0,0,0,0,0,0,0);
#endif

	      /* connect media to extraction tool*/
	      theExtractionInterface->SetSourceMedia(media);

	      /* initialize values for the extraction before first frame*/
	      if (!i) theExtractionInterface->InitExtracting();

	      /* get frame number from image*/
	      {
	      
		FrameNo=RamDB.GetFrameNoFromComment();
		if (FrameNo == -1) FrameNo=i;
		theExtractionInterface->SetSourceMediaFrameNumber(FrameNo);
	      }

	      /* extract descriptor form media*/
	      is_key_frame=theExtractionInterface->StartExtracting();

	      /* free media*/
	      freevop(media);

	      /**************************/
	      /* write key frame to disk*/
	      /**************************/
#ifdef WRITEKEYFRAMES
	  //if ((is_key_frame < 3) && (is_key_frame > 0)) 
		if (is_key_frame == 2)  //only write keyframes that are part of the DS
		{
			char keyfilename[16];

			/* compute directory name from media filename*/
			strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
			strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

			/* appent keyframe filename to path*/
			strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
			sprintf(keyfilename,"key%05ld.jpg",FrameNo);
			strncat(path,keyfilename,MAXMEDIANAMELEN-1-strlen(path));

			RamDB.StoreNamedImage(path);
	  }
#endif /*WERITEKEYFRAMES*/

	      /* delete image*/
	      unlink(ppmimage);

	      i++;
	    }

	    theExtractionInterface->PostExtraction();

	    /* code descriptor data*/
	    theCSInterface->StartEncode();

	  }
	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryMovingStoryBoardServer::Stop()
{
	fprintf(stderr,"Not Implemented: SeqSummaryMovingStoryBoardServer::Stop()\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SeqSummaryMovingStoryBoardServer::Close()
{
	fprintf(stderr,"Not Implemented: SeqSummaryMovingStoryBoardServer::Close()\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SeqSummaryMovingStoryBoardServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SeqSummaryMovingStoryBoardServer::GetName(void)
{ return myName; }



