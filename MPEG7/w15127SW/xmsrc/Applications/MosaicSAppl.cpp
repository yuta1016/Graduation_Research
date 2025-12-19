//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// Applicable File Name:  MosaicSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
#include <io.h>
#include <direct.h>
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
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/readparam.h"


using namespace XM;

const UUID MosaicServer::myID = UUID("");
const char *MosaicServer::myName = "MosaicSAppl";

//=============================================================================
MosaicServer::MosaicServer()
{
}

//----------------------------------------------------------------------------
MosaicServer::~MosaicServer()
{
}

//----------------------------------------------------------------------------
unsigned long MosaicServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicServer::Start(char *ListFile, char *ddlfile)
{
  int dbentry=0;        /* postion in media database*/ 
  int dbentries=0;      /* total number of elements in the database*/
  int frameoffset=1;    /* steps between frames in the sequence*/

  /* image media*/
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;

  /* Image data base */
  MediaDB RamDB;

  /* extraction tool*/
  MosaicExtractionTool *theExtractionTool;
  MosaicExtractionInterfaceABC
    *theExtractionInterface;
	
  /* coding schemes*/
  GenericDSCS *theCS;
  GenericDSCSInterfaceABC *theCSInterface;

  /* parameters*/
  int skip_extracted=0;

  /*****************/
  /* get interfaces*/
  /* create media data class and get interface*/
  theMedia = new MultiMedia;
  theMediaInterface=theMedia->GetInterface();

  /* create extraction tool*/
  /* get extraction interface*/
  theExtractionTool = new 
    MosaicExtractionTool;
  theExtractionInterface=theExtractionTool->GetInterface();

  /* get coding scheme interface*/
  theCS = new GenericDSCS;
  theCSInterface=theCS->GetInterface();

  RamDB.SetMedia(theMediaInterface);
  RamDB.CreateDatabase(ListFile);
  dbentries=RamDB.GetNoOfElements();

  /* set predifined descriptor values*/

  /* read parameters*/
  skip_extracted=atoi(getpara("SkipExtracted","0"));
  frameoffset=atoi(getpara("FrameDistance","1"));

  while (1) {

    int i=0;              /* frame of sequence*/
    char ppmimage[25];    /* filename of single image*/
    char path[MAXMEDIANAMELEN];

    RamDB.GotoEntry(dbentry++);
    if (dbentry > dbentries) break;

    /*********************************/
    /* create directory for keyframes*/
    /* compute directory name from media filename*/
    strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
    strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

    /* create directory*/
#ifndef _VISUALC_
    mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
    _mkdir(path);
#endif

    if (skip_extracted) {
      int mosaicfile;
      strncat(path,"/mosaic.jpg",MAXMEDIANAMELEN-1-strlen(path));

      mosaicfile=open(path,O_RDONLY);
      if (mosaicfile!=-1) {
	close(mosaicfile);
	continue;
      }
    }

    if (!RamDB.DBDecodeMPEGVideo()) break;

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
      strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
      strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
      strncat(path,ddlfile,MAXMEDIANAMELEN-1-strlen(path));
      theCSInterface->SetEncoderFileName(path);

      /* loop frames of sequence*/
      while (1) {

	/* get first frame of sequence*/
	/* malloc media and load input data*/
	sprintf(ppmimage,"m2v/%d.ppm",i);
	/* if images in sequence*/
	if (!RamDB.LoadNamedImage(ppmimage)) break;

	/* convert to appropriate color space*/
//        convertRGB_To_YUV(media,media);

	/* select background after thresholding with 128*/
	theMediaInterface->SelectVopAlphaLayer(BackgroundFromAlpha);

	/* connect media to extraction tool*/
	theExtractionInterface->SetSourceMedia(theMediaInterface);

	/* Check frame number of image*/
	{
	  long FrameNo;

	  FrameNo=theMediaInterface->GetImageTime();
	  if (FrameNo == -1) FrameNo=i;
	  theMediaInterface->SetImageTime(FrameNo);

	  if (!FrameNo) {

	    /* initialize descriptor for extraction of first frame*/
	    theExtractionInterface->InitExtracting();


	  /* Write first frame of sequence (only for GUI
	     and processing of sequences)*/
	    strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	    strncat(path,".dir/0.jpg",MAXMEDIANAMELEN-1-strlen(path));
	    path[MAXMEDIANAMELEN-1]=0;
	    RamDB.StoreNamedImage(path);
	  }
	}

	/* extract descriptor form media*/
	theExtractionInterface->StartExtracting();

	i+=frameoffset;
      }

      theExtractionInterface->PostExtracting();

      /* code descriptor data*/
      theCSInterface->StartEncode();

      {
	MomVop *mosaicvop;
	mosaicvop=theExtractionInterface->GetMosaic();

	if (mosaicvop) {

	  /* compute directory name from media filename*/
	  strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

	  /* appent keyframe filename to path*/
	  strncat(path,"/mosaic.jpg",MAXMEDIANAMELEN-1-strlen(path));

	  RamDB.StoreNamedVop(path,mosaicvop);
	}
      }
    }
  }

  theMediaInterface->destroy();
  theExtractionInterface->destroy();
  theCSInterface->destroy();

  return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicServer::Stop()
{
  return 0;
}
//----------------------------------------------------------------------------
unsigned long MosaicServer::Close()
{
  return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MosaicServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicServer::GetName(void)
{ return myName; }



