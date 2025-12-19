///////////////////////////////////////////////////////////////////////////////
//
// TMosaghis software module was originally developed by
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SummarizationCAppl.cpp
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
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
#include "Utilities/readparam.h"

using namespace XM;

const UUID SummarizationClient::myID = UUID("");
const char *SummarizationClient::myName = "SummarizationClient";

//=============================================================================
SummarizationClient::SummarizationClient():
RamDB(),
NoOfElements(0),
DBDescription(0)

{
}

//----------------------------------------------------------------------------
SummarizationClient::~SummarizationClient()
{
	if (DBDescription) delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long SummarizationClient::Open(char *ListFile, char *DDLFile)
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
unsigned long SummarizationClient::Start(char *Query,int NoOfMatches)

{
        /* media data*/
        MultiMedia *theDBMedia;
        MultiMediaInterfaceABC *theDBMediaInterface;

        /* descriptor*/
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	/* coding schemes for DDL Query*/
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;
	
	/* search tool*/
	SummarizationSearchTool *theSearch;
	SummarizationSearchInterfaceABC *theSearchInterface;

	/*****************************/
	/* get and connect interfaces*/
	/* create media data class and get interface*/
	theDBMedia = new MultiMedia;
	theDBMediaInterface=theDBMedia->GetInterface();

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

	/* connect DS to SearchTool */
	theSearch = new SummarizationSearchTool(theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	/* connect media to extarction tool*/
	theSearchInterface->SetMedia(theDBMediaInterface);

	/* for each data base entry*/
	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {

	    int j=0;                 /* frame of sequence*/
	    int k=0;                 /* input frame to be copied*/
	    int n=0;                 /* number of output frame*/
	    char outputseq[MAXMEDIANAMELEN]; /* name of result sequence*/
	    int outseqexists;        /* flag if output sequence already
					exists*/
	    double activitytreshold; /* number of frames in result sequence*/

	    /* get data base description interface*/
	    DBDescriptionInterface=
	      DBDescription[i].GetInterface();
	    
	    /* connect reference descriptor to search tool*/
	    theSearchInterface->
	      SetRefDescriptionInterface(DBDescriptionInterface);

	    /* initialize search process*/
	    theSearchInterface->InitSearch();

	    /* compute name of output sequence*/
	    activitytreshold=theSearchInterface->ActivityTreshold();
	    strncpy(outputseq,RamDB.GotoEntry(i)->GetFilename(),
		    MAXMEDIANAMELEN-1);
	    strncat(outputseq,".dir/Summary",
		    MAXMEDIANAMELEN-1-strlen(outputseq));
	    if (atoi(getpara("WriteAllFramesToDDL","0"))) {
	      strncat(outputseq,"A",
		      MAXMEDIANAMELEN-1-strlen(outputseq));
	    }
	    {
	      char activitystring[32];
	      sprintf(activitystring,"_%f",activitytreshold);
	      strncat(outputseq,activitystring,
		      MAXMEDIANAMELEN-1-strlen(outputseq));
	    }
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
	      if (!RamDB.DBDecodeMPEGVideo()) break;
	      
	      while (1) {

		int writeframe;       /* if frame k should be copied*/
		char ppmimage[25];    /* filename of single image*/

		/* load input image*/
		sprintf(ppmimage,"m2v/%d.ppm",j);
	      
		/* load input data*/
		if (!RamDB.LoadNamedImage(ppmimage)) break;

#ifdef XYUVDISP
		{
		  MomVop *media=theDBMediaInterface->GetImage();

		  /* display input image*/
		  xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s,
				   (unsigned short *)media->u_chan->data->s,
				   (unsigned short *)media->v_chan->data->s,
				   datasize(media->y_chan)&1,
				   media->y_chan->x,media->y_chan->y,
				   media->u_chan->x,media->u_chan->y,
				   "Query");
		}
#endif

		writeframe=theSearchInterface->StartSearch();
		if (writeframe==-1) writeframe=0;
		if (writeframe & 2) {
		  k=j;
		}
		if (writeframe) {

		  char outimage[25];    /* filename of output image*/
		
		  sprintf(ppmimage,"m2v/%d.ppm",k);
		  sprintf(outimage,"m2v/o%05d.ppm",n);
#ifndef _VISUALC_
		  link(ppmimage,outimage);
#else
		  {
			char command[1024];
			command[1023]=0;
			sprintf(command,"copy %s %s",ppmimage,outimage);
			system(command);
		  }
#endif
#ifdef DEBUG
		  fprintf(stderr,"Copying : %s -> %s\n",ppmimage,outimage);
#endif
		  n++;
		}

		j++;
	      }

	      /* post processing search process*/
	      /*activitytreshold=theSearchInterface->PostSearch();*/
	      /*activity=theSearchInterface->PostSearch();*/

	      /* Encode outout images*/
	      {
		char *parfilename;
		MomVop *an_image;

		//		LoadNamedImage("m2v/%d.ppm");
		
		an_image=theDBMediaInterface->GetImage();

		parfilename=RamDB.GenEncoderParFile(0,"m2v/o%05d",n,
					      an_image->width,
					      an_image->height);
		if (parfilename) {
		  RamDB.EncodeNamedMPEGVideo(outputseq,parfilename,0);

		  unlink(parfilename);
		}
	      }
	    }

	    /* write result from search*/
	    printf("%s",RamDB.GetMediaFilename());
	    printf("\t%f",activitytreshold);
	    printf("\t%s.dir/0.jpg",
		   RamDB.GetMediaFilename());
	    printf("\t%s",outputseq);
	    printf("\n");
	    fflush(stdout);
	  }
	}

	theDBMediaInterface->destroy();
	theCSInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationClient::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SummarizationClient::Close()
{
	/* free memories*/
        RamDB.FreeDatabase();
	if (DBDescription) delete [] DBDescription;

	NoOfElements =0;
	DBDescription =0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SummarizationClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationClient::GetName(void)
{ return myName; }



