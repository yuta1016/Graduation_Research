///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez		- Columbia University.
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
// Applicable File Name:  SegmentRelGraphSAppl.cpp
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


//=============================================================================

using namespace XM;


const UUID SegmentRelGraphServer::myID = UUID("");
const char *SegmentRelGraphServer::myName = "SegmentRelGraph DS Server";

const int	SegmentRelGraphServer::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
SegmentRelGraphServer::SegmentRelGraphServer()
{
}

//----------------------------------------------------------------------------
SegmentRelGraphServer::~SegmentRelGraphServer()
{
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphServer::Open()
{
	/* This server doesn't handle pull */
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphServer::Start(char *ListFile, char *DescriptionFile)
{

//     NOTES Start() method of the SegmentRelGraphDS Server
	 
//     ListFile lists a set of input images with information of each 
//     "segment". A segment can be a VideoSegment, a StillRegion or a
//     MovingRegion and can have associated any number of frames
//     (VideoSegmentRelGraph), frames+masks (MovingRegion)
//     or frame+mask (StillRegion). This image contains that info and the
//     X-coor represents frame number and Y-coor segment_id. The gray
//     scale information points to the maskID stored on disk.
	
//     Together with this image a text file with the hierarchical
//     relationship between these segments is needed "tree.txt".Its format is:
//     segmentID type N_children <childrenID's> N_annotations <annotation's>
	
//     Finally, the original frames and masks are needed as "f_%frameID" and 
//     "m_%maskID_%frameID".

	
	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	/* Files on disk */
	const char   TREEFILE[]	  = "tree.txt";		/* name of the tree file on disk */
	const char	FRAMESFILE[]  = "f_%d.ppm";		/* Frames name on disk */
	const char  MASKSFILE[]   = "m_%d_%d.ras";  /* Masks on disk */

	char        pcBaseDir[STRING_MAXSIZE];		/* Base Folder */
	char		pcTreeName[STRING_MAXSIZE];		/* Tree file name on disk */
	char		pcFramesName[STRING_MAXSIZE];	/* Frames name on disk */
	char		pcMasksName[STRING_MAXSIZE];	/* Frames name on disk */

	char		XMLFile[STRING_MAXSIZE];

	FILE		*fp;						/* File pointer to read the tree file */
	MomVop		*media;						/* Image media (FS image) */
	MediaDB		RamDB;						/* Image DataBase */


	/* Extraction Tools */
	SegmentRelGraphExtractionTool *theExtraction;
	SegmentRelGraphExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;


	/* ---------------------------------------------------------------------------------------
	   Connect Interfaces
	   --------------------------------------------------------------------------------------- */

	/* Connect DS to ExtractionTool */
	theExtraction =
	  new SegmentRelGraphExtractionTool();
	theExtractionInterface = theExtraction->GetInterface();

	/* Connect descriptor to coding scheme */
	theCS = new GenericDSCS();
	theCSInterface = theCS->GetInterface();


	/* ---------------------------------------------------------------------------------------
	   Start Extraction for each database in ListFile
	   --------------------------------------------------------------------------------------- */

	/* Initialise Database */
	RamDB.CreateDatabase(ListFile);

	/* Initialize values for the extraction before first frame */
	/* Not needed */

	/* Loop for every Database in ListFile (pointer to FS image) */
	unsigned int iDB = 0;

	while (1) {

		/* Load input Data (FS image) */
		media = RamDB.DBLoadImage();

		/* Stop if no more images in ListFile */
		if (!media) break;

#ifdef XYUVDISP
		/* display input image */
		xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s, 
				 (unsigned short *)media->u_chan->data->s, 
				 (unsigned short *)media->v_chan->data->s,
				 datasize(media->y_chan)&1,
				 media->y_chan->x,media->y_chan->y,
				 media->u_chan->x,media->u_chan->y, "input");
#endif

		/* Set the Base Directory */
		strcpy( pcBaseDir, RamDB.GetMediaFilename());
		fprintf(stderr, "\nInput data: %s\n", RamDB.GetMediaFilename());
		strcat( pcBaseDir, ".dir/");

		/* Read the "tree.txt" file attached to the FS image */
		strcpy( pcTreeName, pcBaseDir);
		strcat( pcTreeName, TREEFILE);
		fp = fopen( pcTreeName, "r");
		if (fp==NULL) {
			fprintf( stderr, "ERROR(SegmentRelGraphServer): Couldn't not find %s file.\n", pcTreeName);
			exit(-1);
		}

		/* Set frame and mask names */
		strcpy( pcFramesName, pcBaseDir);
		strcat( pcFramesName, FRAMESFILE);
		strcpy( pcMasksName, pcBaseDir);
		strcat( pcMasksName, MASKSFILE);

		/* Output file name */
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		/* Descriptors Schemes */
		GenericDS theDescription = GenericDS::CreateDescriptionRoot();
	 	GenericDSInterfaceABC *theDescriptionInterface;
	
		/* Get DS Interface */
	 	theDescriptionInterface = theDescription.GetInterface();

		/* Connect descriptor to ExtractionTool */
		theExtractionInterface->SetDescriptionInterface(theDescriptionInterface);

		/* Connect descriptor to coding scheme */
		theCSInterface->SetDescriptionInterface(theDescriptionInterface);

		/* Connect output file to coding scheme */
		theCSInterface->SetEncoderFileName(XMLFile);

		/* Set Data to SegmentRelGraphExtraction */
		theExtractionInterface->
		  SetSourceMedia(media, fp, pcFramesName, pcMasksName);

		/* Start Extraction */
		theExtractionInterface->StartExtracting();

		/* Output each segment tree description in a separate file */
		theCSInterface->StartEncode();

		/* Close filehandle (Tree file) */
		fclose(fp);

		/* Free media (FS image) */
		freevop(media);

		/* Let's go to the next database */
		iDB++;

	} /* for each database (FS image + Tree file) */

	theExtractionInterface->destroy();
	theCSInterface->destroy();

	RamDB.FreeDatabase();

	return(0);

}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphServer::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphServer::Close()
{
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& SegmentRelGraphServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphServer::GetName(void)
{ return myName; }










				



