///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Dulce Ponceleon (Almaden Research Center, IBM, USA)
// Jan H. Pieper   (Almaden Research Center, IBM, USA)
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
// Applicable File Name:  SeqSummaryKeywordsSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _VISUALC_
#include <unistd.h>
#else
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
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */


//#define WRITEKEYFRAMES

using namespace XM;

const UUID SeqSummaryKeywordsServer::myID = UUID("");
const char *SeqSummaryKeywordsServer::myName = "SeqSummaryKeywordsSAppl";

//=============================================================================
SeqSummaryKeywordsServer::SeqSummaryKeywordsServer()
{
}

//----------------------------------------------------------------------------
SeqSummaryKeywordsServer::~SeqSummaryKeywordsServer()
{
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsServer::Start(char *ListFile, char *unused)
{

	/* Image data base */
	MediaDB RamDB;	//we abuse RamDB to get the filenames of the speech transcript files

	/* extraction tool*/
	SeqSummaryKeywordsExtractionTool *theExtractionTool;
	SeqSummaryKeywordsExtractionInterfaceABC *theExtractionInterface;
	
	/* coding schemes*/
	GenericDSCS *theCS;
 	GenericDSCSInterfaceABC *theCSInterface;

	/*****************/
	/* get interfaces*/
	/* create extraction tool*/
	/* get extraction interface*/
	theExtractionTool = new SeqSummaryKeywordsExtractionTool;
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get coding scheme interface*/
	theCS = new GenericDSCS;
	theCSInterface=theCS->GetInterface();

	RamDB.CreateDatabase(ListFile);

	/* set predifined descriptor values*/

	for(int i=0; i<RamDB.GetNoOfElements(); i++)
	{
	  char path[MAXMEDIANAMELEN];

	  /*********************************/
	  /* create directory for keyframes*/
	  /* compute directory name from media filename*/
	  strncpy(path,RamDB.GotoEntry(i)->GetFilename(),MAXMEDIANAMELEN-1);
	  strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));

		printf(path);
		printf("\n");

	  /* create directory*/
#ifndef _VISUALC_
	  mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	  _mkdir(path);
#endif

	  //sth see what is done now in DSDummyType
	  /* reset description data*/
	  /* create XML Document for Description*/
	  GenericDS theDocument = GenericDS::CreateDescriptionRoot();

		/* get descriptor interface*/
	  GenericDSInterfaceABC *theDocumentInterfaceABC=theDocument.GetInterface();

	  /* connect descriptor to extraction tool*/
	  theExtractionInterface->SetDescriptorInterface(theDocumentInterfaceABC);

	  /* connect descriptor to coding scheme*/
	  theCSInterface->SetDescriptionInterface(theDocumentInterfaceABC);

	  /* connect output file to coding scheme*/
	  strncat(path,"/SeqSummaryKeywordsDS.ddl",MAXMEDIANAMELEN-1-strlen(path));
	  theCSInterface->SetEncoderFileName(path);

	  /* allow file IO for prelimenary results*/
	  theExtractionInterface->SetSourceMediaFilename(RamDB.GotoEntry(i)->GetFilename());

	  /* reset descrition data*/
// 	    theDescriptionInterfaceABC->InitDescriptor();
	  
	  /* loop frame of vector file*/

	  /* connect media to extraction tool*/
	  //theExtractionInterface->SetSourceMedia(media);

	  /* initialize values for the extraction before first frame*/
	  theExtractionInterface->InitExtracting();

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

	  /* theExtractionInterface->PostExtraction();*/

	  /* code descriptor data*/
	  theCSInterface->StartEncode();

	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsServer::Stop()
{
	fprintf(stderr,"Not Implemented: SeqSummaryKeywordsServer::Stop()\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryKeywordsServer::Close()
{
	fprintf(stderr,"Not Implemented: SeqSummaryKeywordsServer::Close()\n");
	return 0;
}

//----------------------------------------------------------------------------
const UUID& SeqSummaryKeywordsServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SeqSummaryKeywordsServer::GetName(void)
{ return myName; }



