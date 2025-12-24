///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang	Siemens AG 	12/05/2000
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
// Applicable File Name:  TimeCAppl.cpp
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
#include "Descriptors/Descriptors.h"
#include "SearchUtilities/SearchUtilities.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/Utilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"

#include <iostream>
#include <fstream>


//=============================================================================

using namespace XM;


const UUID TimeClient::myID = UUID("");
const char *TimeClient::myName = "SegmentRelGraph DS Client";

const int	TimeClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
TimeClient::TimeClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
TimeClient::~TimeClient()
{
}

//----------------------------------------------------------------------------
unsigned long TimeClient::Open(char *ListFile, char *DescriptionFile)
{

	// NOTES Open() method of the TimeDS Client
	// 
	// ListFile lists a set of input images with information of each 
	// "segment". A segment can be a VideoSegment, a StillRegion or a MovingRegion and
	// can have associated any number of frames (VideoSegment), frames+masks (MovingRegion)
	// or frame+mask (StillRegion). This image contains that info and the X-coor
	// represents frame number and Y-coor segment_id. The gray scale information points 
	// to the maskID stored on disk.
	//
	// The segment description file for each input data can be obtained by changing the name of
	// the input image in ListFile for the Bitstream value (input image: data1/fs1.ras;
	// description file: data1/TimeDS.xml; being "SegmentRelGraphDS.xml" the value of DescriptionFile).
	


	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	char        pcBaseDir[STRING_MAXSIZE];		// Base Folder
	char		XMLFile[STRING_MAXSIZE];		// Description XML File
	char		*pcTmp;							// Temp pointer to string


	/* Description Scheme Interface */
 	GenericDSInterfaceABC *theDescriptionInterface;

	/* Generic Coding Scheme */
	GenericDSCS *theCS;
	GenericDSCSInterfaceABC *theCSInterface;

	/* ---------------------------------------------------------------------------------------
	   Connect Interfaces 
	   --------------------------------------------------------------------------------------- */

	/* Get coding scheme interface */
	theCS = new GenericDSCS;
	theCSInterface = theCS->GetInterface();


	/* ---------------------------------------------------------------------------------------
	   Read database in ListFile
	   --------------------------------------------------------------------------------------- */

	/* Read filename data base */
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* Create in memory decompressed database */
	/* allocate memories */
	DBDescription = new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

		/* Set the Base Directory */
		strcpy(pcBaseDir, RamDB.GotoEntry(i)->GetFilename());

		// sth please use specified directory structure
		// mediafilename/DDLfile.ddl

		/* *nix style dirs */
		pcTmp=strrchr( pcBaseDir, '/');
		if (pcTmp!=NULL) {
			pcTmp[1] = '\0';
		} else {
			/* Win* style */
			pcTmp = strrchr( pcBaseDir, '\\');
			if (pcTmp!=NULL) {
				pcTmp[1] = '\0';
			}
			/* Unix style */
			pcTmp = pcBaseDir;
			pcTmp[0] = '\0';
		}

		/* Generate full description file path */
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(XMLFile);

		/* Get TimeDS interface */
		theDescriptionInterface = DBDescription[i].GetInterface();

		/* Connect Generic DS to coding schemes */
		theCSInterface->
		  SetDescriptionInterface(theDescriptionInterface);

		/* Decode Generic DS */
		theCSInterface->StartDecode();
		
		if(theDescriptionInterface->isNull())
		  fprintf(stderr, "Error:GenericDS is null\n");

	}

	
	theCSInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeClient::Start(char *pcQuery,int NoOfMatches)
{

	// NOTES Start() method of the TimeDS Client
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
	FILE		*fpQuery;  // sth has to become part of MultiMedia

	/* List of best matches */
	MatchList theMatchList;

	/* Descriptors Schemes for query and element in database */
 	GenericDSInterfaceABC *DBDescriptionInterface;

	/* Search Tool */
	TimeSearchTool *theSearch;
	TimeSearchInterfaceABC *theSearchInterface;

	/* ---------------------------------------------------------------------------------------
	   Connect Interfaces 
	   --------------------------------------------------------------------------------------- */
	theSearch = new TimeSearchTool;
	/* Connect DS to SearchTool */
	theSearchInterface = theSearch->GetInterface();


	/* Prepare matchlist */
	if (NoOfMatches> (int)(RamDB.GetNoOfElements())) {
	  NoOfMatches = (int)(RamDB.GetNoOfElements());
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	fprintf(stderr, "Number of matches: %d\n", NoOfMatches);

	/* Read the query file */
	fpQuery = fopen(pcQuery, "r");
	if (fpQuery==NULL) {
		fprintf( stderr, "ERROR(Query File): Couldn't not find %s file.\n", pcQuery);
		exit(-1);
	}
	if(theSearchInterface->SetSourceQuery(fpQuery)<0) 
		exit(-1);
	
	/* ---------------------------------------------------------------------------------------
	   Match query with each item of database
	   --------------------------------------------------------------------------------------- */

	ppcIDList = new char*[NoOfElements];
	int i;
	for (i=0; i<NoOfElements; i++) {

		/* Get SegmentRelGraph DS interface */
		DBDescriptionInterface=DBDescription[i].GetInterface();

		/* Connect query description scheme to search tool */
		theSearchInterface->
		  SetDescriptionInterface(DBDescriptionInterface);

		/* sort result into list of best matches */
		theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
		
		ppcIDList[i] = new char[STRING_MAXSIZE];
		theSearchInterface->GetSegmentID(ppcIDList[i]);
		
	}


	/* ---------------------------------------------------------------------------------------
	   Print results of the query matching
	   --------------------------------------------------------------------------------------- */
	fprintf(stderr, "\nList of results: (item, distance, segment info)\n");

	{
	int i;
	for (i=0; i<NoOfMatches ; i++) {
		if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
		printf("%s", RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
		fflush(stdout);
		fprintf(stderr,"\t%e", theMatchList.GetDistanceValue(i));
		fprintf(stderr,"\t%s",ppcIDList[i]);
		printf("\n");
	}
	}

	/* Close filehandle (Query file) */
	fclose(fpQuery);
	for (i=0;i<NoOfElements;i++)
	  delete [] ppcIDList[i];
	delete [] ppcIDList;

	theSearchInterface->destroy();

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long TimeClient::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long TimeClient::Close()
{

	// Free memories
	RamDB.FreeDatabase();
	if (DBDescription) delete [] DBDescription;
	
	NoOfElements = 0;
	DBDescription = 0;

	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& TimeClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeClient::GetName(void)
{ return myName; }










				



