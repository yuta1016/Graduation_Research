///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang	Siemens AG	12/05/2000
// Joerg Heuer          Siemens AG      30/06/2000
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
// Applicable File Name:  TimeSAppl.cpp
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


#include <iostream>
#include <fstream>
//#include "dom/DOM.hpp"
//#include "util/PlatformUtils.hpp"		// Initiliase XML.


//=============================================================================

using namespace XM;


const UUID TimeServer::myID = UUID("");
const char *TimeServer::myName = "TIME DS Server";

const int	TimeServer::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
TimeServer::TimeServer()
{
}

//----------------------------------------------------------------------------
TimeServer::~TimeServer()
{
}

//----------------------------------------------------------------------------
unsigned long TimeServer::Open()
{
	/* This server doesn't handle pull */
	return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeServer::Start(char *ListFile, char *DescriptionFile)
{

	// NOTES Start() method of the TimeDS Server
	// 
	// ListFile lists a set of input images with information of each 
	// "segment". A segment can be a VideoSegment, a StillRegion or a MovingRegion and
	// can have associated any number of frames (VideoSegment), frames+masks (MovingRegion)
	// or frame+mask (StillRegion). This image contains that info and the X-coor
	// represents frame number and Y-coor segment_id. The gray scale information points 
	// to the maskID stored on disk.
	//
	// Together with this image a text file with the hierarchical relationship between 
	// these segments is needed "tree.txt".Its format is:
	//         segmentID type N_children <childrenID's> N_annotations <annotation's>
	//
	
	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	/* Files on disk */
	const char	TREEFILE[]	  = "tree.txt";		/* name of the tree file on disk */
	const char	TIMEFILE[]	  = "time.txt";
	const char 	MEDIAFILE[]	  = "mpg";

	char		*pcTmp;
	char	        pcBaseDir[STRING_MAXSIZE];		/* Base Folder */
	char		pcTreeName[STRING_MAXSIZE];		/* Tree file name on disk */
	char		pcTimeName[STRING_MAXSIZE];
	char		pcMediaName[STRING_MAXSIZE];

	char		XMLFile[STRING_MAXSIZE];

	FILE		*fpTree;					/* File pointer to read the tree file */
	FILE		*fpTime;
	MomVop		*media;						/* Image media (FS image) */
	MediaDB		RamDB;						/* Image DataBase */


	/* Extraction Tools */
	TimeExtractionTool *theExtraction;
	TimeExtractionInterfaceABC *theExtractionInterface;
	
	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	/* ---------------------------------------------------------------------------------------
	   Connect Interfaces 
	   --------------------------------------------------------------------------------------- */
	
	/* Connect DS to ExtractionTool */
	theExtraction = new TimeExtractionTool;
	theExtractionInterface = theExtraction->GetInterface();

	/* Connect descriptor to coding scheme */
	theCS = new GenericDSCS;
	theCSInterface = theCS->GetInterface();


	/* -----------------------------------------------------------------
	   Start Extraction for each database in ListFile
	   --------------------------------------------------------------- */

	/* Initialise Database */
//	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);

	/* Initialize values for the extraction before first frame */
	/* Not needed */

	/* Loop for every Media in ListFile Database (pointer to FS image) */
	unsigned int iDB = 0;

	while (1) {


		/* Load input Data (FS image) */
		media = RamDB.DBLoadImage();

		/* Stop if no more images in ListFile */
		if (!media) break;


		/* Set the Base Directory */
		strcpy( pcBaseDir, RamDB.GetMediaFilename());
		fprintf(stderr, "\nInput data: %s\n", RamDB.GetMediaFilename());

		/* *nix style dirs */
		strcpy(pcMediaName, pcBaseDir);
		pcMediaName[strlen(pcBaseDir)-3] = '\0';
		strcat(pcMediaName, MEDIAFILE);
		
		pcTmp=strrchr( pcBaseDir, '/');
		if (pcTmp!=NULL) {
			pcTmp[1] = '\0';
		} else {
			/* Win* style */
			//pcTmp = strrchr( pcBaseDir, '\\');
			//if (pcTmp!=NULL) {
				//pcTmp[1] = '\0';
			//}
			/* Unix style */
			pcTmp = pcBaseDir;
			pcTmp[0] = '\0';
		}
		
		/* Read the "tree.txt" file attached to the FS image */
		strcpy( pcTreeName, pcBaseDir);
		strcat( pcTreeName, TREEFILE);
		fpTree = fopen( pcTreeName, "r");
		if (fpTree==NULL) {
			fprintf( stderr, "ERROR(TimeServer): Couldn't not find %s file.\n", pcTreeName);
			exit(-1);
		}

		
		/* Read the "time.txt" file attached to the FS image */
		strcpy( pcTimeName, pcBaseDir);
		strcat( pcTimeName, TIMEFILE);
		fpTime = fopen( pcTimeName, "r");
		if (fpTime==NULL) {
			fprintf( stderr, "ERROR(TimeServer): Couldn't not find %s file.\n", pcTimeName);
			exit(-1);
		}

#ifdef XYUVDISP
		/* display input image */
		xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s, 
				 (unsigned short *)media->u_chan->data->s, 
				 (unsigned short *)media->v_chan->data->s,
				 datasize(media->y_chan)&1,
				 media->y_chan->x,media->y_chan->y,
				 media->u_chan->x,media->u_chan->y, "input");
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
		    SetDescriptionInterface(theDocumentInterfaceABC);

		  /* connect descriptor to coding scheme*/
		  theCSInterface->
		    SetDescriptionInterface(theDocumentInterfaceABC);

		  /* Open the Output File */
		  strcpy(XMLFile, pcBaseDir);
		  strcat(XMLFile, DescriptionFile);

		  /* connect stream to coding scheme */
		  theCSInterface->SetEncoderFileName(XMLFile);

		  /* Set Data to TimeExtraction */
		  theExtractionInterface->
		    SetSourceMedia(media, fpTree, fpTime, pcMediaName);
		  // sth stay with specified interface

		  /* Perform Extraction at beginning of Sequence*/
		  theExtractionInterface->InitExtracting();

		  /* Output each DOM tree in separate files */
		  theCSInterface->StartEncode();

		  /* Close filehandle (Tree file) */
		  fclose(fpTree);

		  /* Close filehandle (Time file) */
		  fclose(fpTime);

		  /* Free media (FS image) */
		  freevop(media);

		}

		/* Let's go to the next database */
		iDB++;

	} /* for each database (FS image + Tree file) */

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long TimeServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeServer::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& TimeServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeServer::GetName(void)
{ return myName; }

