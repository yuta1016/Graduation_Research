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
// Applicable File Name:  SegmentRelGraphCAppl.cpp
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


#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "Utilities/Utilities.h"				/* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>


//=============================================================================

using namespace XM;


const UUID SegmentRelGraphClient::myID = UUID("");
const char *SegmentRelGraphClient::myName = "SegmentRelGraph DS Client";

const int	SegmentRelGraphClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
SegmentRelGraphClient::SegmentRelGraphClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
SegmentRelGraphClient::~SegmentRelGraphClient()
{
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphClient::Open(char *ListFile, char *DescriptionFile)
{

	// NOTES Open() method of the SegmentRelGraphDS Client
	// 
	// ListFile lists a set of input images with information of each 
	// "segment". A segment can be a VideoSegmentRelGraph, a StillRegion or a MovingRegion and
	// can have associated any number of frames (VideoSegmentRelGraph), frames+masks (MovingRegion)
	// or frame+mask (StillRegion). This image contains that info and the X-coor
	// represents frame number and Y-coor segment_id. The gray scale information points 
	// to the maskID stored on disk.
	//
	// The segment description file for each input data can be obtained by changing the name of
	// the input image in ListFile for the Bitstream value (input image: data1/fs1.ras;
	// description file: data1/SegmentRelGraphDS.ddl; being "SegmentRelGraphDS.ddl" the value of 
	// DescriptionFile).
	

	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	char pcBaseDir[STRING_MAXSIZE];		// Base Folder
	char XMLFile[STRING_MAXSIZE];		// Description XML File

	/* Description Scheme Interface */
 	GenericDSInterfaceABC *theDescriptionInterface;

	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;


	/* -----------------------------------------------------------------
	   Connect Interfaces 
	   -------------------------------------------------------------- */

	/* Get coding scheme interface */
	theCS = new GenericDSCS;
	theCSInterface = theCS->GetInterface();


	/* -----------------------------------------------------------------
	   Read database in ListFile
	   -------------------------------------------------------------- */

	/* Read filename data base */
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* Create in memory decompressed database */
	/* allocate memories */
	DBDescription = new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

		/* Set the Base Directory */
		strcpy(pcBaseDir, RamDB.GotoEntry(i)->GetFilename());
		strcat(pcBaseDir, ".dir/");

		/* Generate full description file path */
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		/* Initialiaze the Generic DS */
		DBDescription[i] = GenericDS::CreateDescriptionRoot();

		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(XMLFile);

		/* Get DS interface */
		theDescriptionInterface = DBDescription[i].GetInterface();

		/* Connect Generic DS to coding schemes */
		theCSInterface->
		  SetDescriptionInterface(theDescriptionInterface);

		/* Decode Generic DS */
		theCSInterface->StartDecode();

	}

	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphClient::Start(char *Query,int NoOfMatches)
{

	// NOTES Start() method of the SegmentRelGraphDS Client
	// 
	// Query can be of two types: 
	//
	// (1) Query is an input image with information of one 
	// "segment". A segment can be a VideoSegment, a StillRegion or a MovingRegion and
	// can have associated any number of frames (VideoSegmentRelGraph), frames+masks (MovingRegion)
	// or frame+mask (StillRegion). This image contains that info and the X-coor
	// represents frame number and Y-coor segment_id. The gray scale information points 
	// to the maskID stored on disk.
	//
	// Together with this image a text file with the hierarchical relationship between 
	// these segments is needed "tree.txt".Its format is:
	//         segmentID type N_children <childrenID's> N_annotations <annotation's>
	//
	// Finally, the original frames and masks are needed as "f_%frameID" and 
	// "m_%maskID_%frameID".
	//
	// Only VideoSegment and MovingRegion input data are valid for the SegmentRelGraphClient,
	// at the moment.
	//
	// (2) Query is an xml/ddl file with an existing description of a segment relationship
	// graph. In this case, the extension of the file will be .xml or .ddl.
	//
	// In both cases above, the graphs are match as follows. Each relationship is assigned
	// a value (-1, 0, +1 for Before, Starts, and After). The resulting vectors are compared
	// using L1 distance limited to the distance of the shorter vector.


	fprintf( stderr, "SegmentRelGraphClient::Start( %s, %d)\n", Query, NoOfMatches);


	/* -----------------------------------------------------------------
	   Declarations of variables
	   ---------------------------------------------------------------*/

	/* Files on disk */
	const char TREEFILE[] = "tree.txt"; /* name of the tree file on disk */
	const char FRAMESFILE[] = "f_%d.ppm";	/* Frames name on disk */
	const char MASKSFILE[] = "m_%d_%d.ras";  /* Masks on disk */

	char pcBaseDir[STRING_MAXSIZE];		/* Base Folder */
	char pcTreeName[STRING_MAXSIZE];	/* Tree file name on disk */
	char pcFramesName[STRING_MAXSIZE];	/* Frames name on disk */
	char pcMasksName[STRING_MAXSIZE];	/* Frames name on disk */

	FILE *fp;		/* File pointer to read the tree file */
	MomVop *media;		/* Image media (FS image) */

	/* List of best matches */
	MatchList theMatchList;

	/* Descriptors Schemes for query and element in database */
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	/* Extraction Tool */
	SegmentRelGraphExtractionTool *theExtraction;
	SegmentRelGraphExtractionInterfaceABC *theExtractionInterface;

	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	/* Search Tool */
	SegmentRelGraphSearchTool *theSearch;
	SegmentRelGraphSearchInterfaceABC *theSearchInterface;


	/* ----------------------------------------------------------------
	   Connect Interfaces 
	   --------------------------------------------------------------*/
	
	/* Get description scheme interface */
 	theDescriptionInterface = theDescription.GetInterface();

	/* Connect DS to ExtractionTool */
	theExtraction =
	  new SegmentRelGraphExtractionTool(theDescriptionInterface);
	theExtractionInterface = theExtraction->GetInterface();

	/* Connect DS to GenericDSCS */
	theCS = new GenericDSCS(theDescriptionInterface);
	theCSInterface = theCS->GetInterface();

	/* Connect DS to SearchTool */
	theSearch =
	  new SegmentRelGraphSearchTool(theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	/* Prepare matchlist */
	if (NoOfMatches> (int)(RamDB.GetNoOfElements())) {
	  NoOfMatches = (int)(RamDB.GetNoOfElements());
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	fprintf(stderr, "Number of matches: %d\n", NoOfMatches);


	/* ----------------------------------------------------------
	   Check for types of query: (1) Segment data (.ras) or
	   -                         (2) XML/DDL description
	   --------------------------------------------------------*/
	
	/* If extension of Query is "xml" or "ddl" them query by XML/DDL description (2) */
	if ( strncmp(Query+strlen(Query)-4, ".xml", 4) && 
		strncmp(Query+strlen(Query)-4, ".ddl", 4) ) {

	  /* --------------------------------------------------
	     Query by segment data: extract 
	     SegmentRelationshipGraph DS from Query
	     ------------------------------------------------*/

	  /* Load input Data (FS image) */
	  media = RamDB.LoadNamedImage(Query);

	  /* Stop if null image */
	  if (!media) return 0;

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
	  strcpy( pcBaseDir, Query);
	  strcat( pcBaseDir, ".dir/");

	  /* Read the "tree.txt" file attached to the FS image */
	  strcpy( pcTreeName, pcBaseDir);
	  strcat( pcTreeName, TREEFILE);
	  fp = fopen( pcTreeName, "r");
	  if (fp==NULL) {
	    fprintf(stderr,
		    "ERROR(SegmentRelGraphClient): "
		    "Couldn't not find %s file.\n", pcTreeName);
	    return (unsigned long)-1;
	  }

	  /* Set frame and mask names */
	  strcpy( pcFramesName, pcBaseDir);
	  strcat( pcFramesName, FRAMESFILE);
	  strcpy( pcMasksName, pcBaseDir);
	  strcat( pcMasksName, MASKSFILE);

	  /* Set Data to SegmentRelGraphExtraction */
	  theExtractionInterface->
	    SetSourceMedia(media, fp, pcFramesName, pcMasksName);

	  /* Start Extraction */
	  theExtractionInterface->StartExtracting();

	  /* Close filehandle (Tree file) */
	  fclose(fp);

	  /* Free media (FS image) */
	  freevop(media);

	}
	else {

	  /* ----------------------------------------------------
	     Query by XML decription: decode
	     SegmentRelationshipGraph DS from Query
	     --------------------------------------------------*/

	  /* Connect description file to coding scheme */
	  theCSInterface->SetDecoderFileName(Query);

	  /* Decode Generic DS */
	  theCSInterface->StartDecode();
	}


	/* ----------------------------------------------------------
	   Match query with each item of database
	   --------------------------------------------------------*/
	{

	int i;
	for (i=0; i<NoOfElements; i++) {

	  /* Get  DS interface */
	  DBDescriptionInterface=
	    DBDescription[i].GetInterface();

	  /* Connect query description scheme to search tool */
	  theSearchInterface->
	    SetRefDescriptionInterface(DBDescriptionInterface);

	  /* sort result into list of best matches */
	  theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
	}
	}


	/* --------------------------------------------------------
	   Print results of the query matching
	   ------------------------------------------------------*/

	fprintf(stderr, "\n\nList of results: (item, distance)\n");

	{
	int i;
	for (i=0; i<NoOfMatches ; i++) {
	  if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	  printf("%s",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());
	  fflush(stdout);
	  fprintf(stderr,"\t%e",theMatchList.GetDistanceValue(i));
	  printf("\n");
	}
	}

	theExtractionInterface->destroy();
	theCSInterface->destroy();
	theSearchInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentRelGraphClient::Close()
{

	// Free memories
	RamDB.FreeDatabase();
	if (DBDescription) delete [] DBDescription;
	
	NoOfElements = 0;
	DBDescription = 0;

	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& SegmentRelGraphClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentRelGraphClient::GetName(void)
{ return myName; }










				



