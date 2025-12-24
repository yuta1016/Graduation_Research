///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// Applicable File Name:  MosaicCAppl.cpp
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
#include <momusys.h>
#include <math.h>
#include <float.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "vopio.h"
#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Media/Media.h"
#include "Utilities/readparam.h"

using namespace XM;

const UUID MosaicClient::myID = UUID("");
const char *MosaicClient::myName = "MosaicClient";

//=============================================================================
MosaicClient::MosaicClient():
RamDB(),
NoOfElements(0),
DBDescription(0)

{
}

//----------------------------------------------------------------------------
MosaicClient::~MosaicClient()
{
	if (DBDescription) delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long MosaicClient::Open(char *ListFile, char *DDLFile)
{
        /* descriptor and sub-descriptor*/
	GenericDSInterfaceABC *theDescriptionInterface;

	/* coding schemes*/
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	/* allocate memory for coding scheme*/
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	/* read filename data base*/
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* create in table with references to descriptors in data base*/
	DBDescription = new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  char path[MAXMEDIANAMELEN];

	  /* Initialiaze the Generic DS */
	  DBDescription[i] = GenericDS::CreateDescriptionRoot();

	  /* get descriptor interface*/
	  theDescriptionInterface=
	    DBDescription[i].GetInterface();

	  /***********************************/
          /* get DDL file name from data media
	     filename and DDL file name*/
	  strncpy(path,RamDB.GotoEntry(i)->
		  GetFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
	  strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
	  path[MAXMEDIANAMELEN-1]=0;

#ifdef DEBUG
	  /* print filename for debuging*/
	  fprintf(stderr,"Decoding %s\n",path);
#endif

	  /* connect DDL file to coding scheme*/
	  theCSInterface->SetDecoderFileName(path);

	  /* get description interface */
	  theDescriptionInterface = DBDescription[i].GetInterface();

	  /* connect descriptors to coding schemes*/
	  theCSInterface->SetDescriptionInterface(theDescriptionInterface);

	  /*decode descriptor*/
	  theCSInterface->StartDecode();
	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicClient::Start(char *Query,int NoOfMatches)

{
        /* media data*/
        MultiMedia *theDBMedia;
        MultiMediaInterfaceABC *theDBMediaInterface;

        /* descriptor*/
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	/* coding schemes for DDL Query*/
	GenericDSCS *theCS=0;
	GenericDSCSInterfaceABC *theCSInterface=0;
	
	/* search tool*/
	MosaicSearchTool *theSearch;
	MosaicSearchInterfaceABC *theSearchInterface;

	/*****************************/
	/* get and connect interfaces*/
	/* create media data class and get interface*/
	theDBMedia = new MultiMedia;
	theDBMediaInterface=theDBMedia->GetInterface();

	if (Query) {

	  /* get description interface */
	  theDescriptionInterface = theDescription.GetInterface();

	  /* connect DS to GenericDSCS */
	  theCS = new GenericDSCS(theDescriptionInterface);
	  theCSInterface = theCS->GetInterface();

	  /* read query*/
	  /* connect DDL file to coding scheme*/
	  theCSInterface->SetDecoderFileName(Query);

	  /*decode descriptor*/
	  theCSInterface->StartDecode();
	}
	else {
	  theDescriptionInterface=0;
	}

	/* connect DS to SearchTool */
	theSearch = new MosaicSearchTool(theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	/* connect media to extarction tool*/
	theSearchInterface->SetMedia(theDBMediaInterface);

	/* for each data base entry*/
	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {

	    int j=0;                 /* number of frame to be processed*/
	    int n=0;                 /* number of output frame*/
	    char outputseq[MAXMEDIANAMELEN]; /* name of result sequence*/
	    int outseqexists;        /* flag if output sequence already
					exists*/
	    char mosaic[MAXMEDIANAMELEN];
	                             /* name of result mosaic image*/
	    MultiMediaInterfaceABC *OutMedia;
                                     /* output media*/
	    MomVop *outvop;          /* output image from output media*/

	    /* get data base description interface*/
	    DBDescriptionInterface=
	      DBDescription[i].GetInterface();
	    
	    /* connect reference descriptor to search tool*/
	    theSearchInterface->
	      SetRefDescriptionInterface(DBDescriptionInterface);

	    /* initialize search process*/
	    theSearchInterface->InitSearch();

	    /* compute name of output sequence*/
	    strncpy(outputseq,RamDB.GotoEntry(i)->GetFilename(),
		    MAXMEDIANAMELEN-1);
	    strncat(outputseq,".dir/Mosaic",
		    MAXMEDIANAMELEN-1-strlen(outputseq));
	    strncat(outputseq,".mpg",
		    MAXMEDIANAMELEN-1-strlen(outputseq));
	    outputseq[MAXMEDIANAMELEN-1]=0;

	    /* test if output sequence already exists*/
	    outseqexists=open(outputseq,O_RDONLY);
	    if (outseqexists!=-1) {
	      close(outseqexists);
	    }

	    /* if output sequence does not exists then for each input image*/
	    else {

	      /* decode input sequence from data base entry*/
	      RamDB.SetMedia(theDBMediaInterface);
	      sprintf(mosaic,"%s.dir/mosaic.jpg",
		      RamDB.GotoEntry(i)->GetFilename());

	      /* load mosaic image*/
	      if (!RamDB.LoadNamedImage(mosaic)) continue;
	      theDBMediaInterface->SetImageTime(j);
	      theDBMediaInterface->SelectVopAlphaLayer(BackgroundFromAlpha);
	      //sth background selection must be changed when alpha is
	      // written with mosaic

	      while (1) {

		float writeframe;

		/* search parmeters of current Image and create output image*/
		writeframe=theSearchInterface->StartSearch();

		/* check if no more images in description*/
		if (writeframe < -1.5) break;

		/* increment image counter*/
		j++;
		theDBMediaInterface->SetImageTime(j);

		if (writeframe < -0.5) { /* something went wrong*/
		  continue;
		}

		/* get output media from search*/
		OutMedia=theSearchInterface->GetOutMedia();
		if (!OutMedia) {
		  continue;
		}

		outvop=OutMedia->GetImage();
		if (!outvop) {
		  continue;
		}

#ifdef XYUVDISP
		if (outvop) {
		  /* display input image*/
		  xyuv_show_simple(0,
				   (unsigned short *)outvop->y_chan->data->s,
				   (unsigned short *)outvop->u_chan->data->s,
				   (unsigned short *)outvop->v_chan->data->s,
				   datasize(outvop->y_chan)&1,
				   outvop->y_chan->x,outvop->y_chan->y,
				   outvop->u_chan->x,outvop->u_chan->y,
				   "Output");
		}
#endif


		{

		  char outimagename[25];    /* filename of output image*/
		
		  sprintf(outimagename,"m2v/o%05d.ppm",n);
		  RamDB.StoreNamedVop(outimagename,outvop);
		  n++;
		}

	      }

	      /* post processing search process*/
	      /*activitytreshold=theSearchInterface->PostSearch();*/
	      /*activity=theSearchInterface->PostSearch();*/

	      /* Encode outout images*/
	      if (n) {
		char *parfilename;

		parfilename=RamDB.GenEncoderParFile(0,"m2v/o%05d",n,
						    outvop->width,
						    outvop->height);
		if (parfilename) {
		  RamDB.EncodeNamedMPEGVideo(outputseq,parfilename,0);

		  unlink(parfilename);
		}
	      }
	    }

	    /* write result from search*/
	    printf("%s",RamDB.GetMediaFilename());
	    printf("\t%f",0.0);
	    printf("\t%s.dir/mosaic.jpg",
		   RamDB.GetMediaFilename());
	    printf("\t%s",outputseq);
	    printf("\n");
	    fflush(stdout);
	  }
	}

	theDBMediaInterface->destroy();
	if (theCSInterface) theCSInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MosaicClient::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long MosaicClient::Close()
{
	/* free memories*/
        RamDB.FreeDatabase();
	if (DBDescription) delete [] DBDescription;

	NoOfElements =0;
	DBDescription =0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MosaicClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MosaicClient::GetName(void)
{ return myName; }



