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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  StructuredCollectionCAppl.cpp
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

#include <iostream> //LINUX_2011
#include <fstream>


//=============================================================================

using namespace XM;


const UUID StructuredCollectionClient::myID = UUID("");
const char *StructuredCollectionClient::myName = "StructuredCollectionClient";

const int	StructuredCollectionClient::STRING_MAXSIZE= 4096;	/* maximum size of the strings */


//=============================================================================
StructuredCollectionClient::StructuredCollectionClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
StructuredCollectionClient::~StructuredCollectionClient()
{
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionClient::Open(char *ListFile, char *DescriptionFile)
{

	// NOTES Open() method of the StructuredCollectionDS Client
	// 
	// ListFile is a list of text files which describe of a collection of images in
	// multiple semantic classes. The path of the images in ListFile is relative
	// the directory where ListFile is kept.
	// 
	// The xml/ddl output descriptions are stored in the base directory of each class
	// text file with name DescriptionFile.	

	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	char        pcBaseDir[STRING_MAXSIZE];		// Base Folder
	char		XMLFile[STRING_MAXSIZE];		// Description XML File


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
	DBDescription =
	  new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

		/* Set the Base Directory */
		strcpy( pcBaseDir, RamDB.GotoEntry(i)->GetFilename());
		strcat( pcBaseDir, ".dir/");

		/* Generate full description file path */
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		/* Initialiaze the Generic DS */
		DBDescription[i] = GenericDS::CreateDescriptionRoot();

		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(XMLFile);

		/* Get StructuredCollectionDS interface */
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
unsigned long StructuredCollectionClient::Start(char *Query,int NoOfMatches)
{

	// NOTES Start() method of the StructuredCollectionDS Client
	// 
	// Query can be of two types: 
	//
	// (1) Query is an text file with a list of classes for the collection.
	//
	// (2) Query is an xml/ddl file with an existing description of a collection. 
	// In this case, the extension of the file will be .xml or .ddl.

	// In both casses above, the color histogram statistics 
	// for query classes are matched to color histogram
	// statistics for reference classes. For each query class, the minimum
	// euclidean distance to reference classes is calculated.
	// The final distance will be the average of the minimum distances for
	// each query class.
	//
	// The input could also be an image whose color histogram could be matched
	// to the one for each collection cluster. The distance of the query to the 
	// a collection will be the mean of distances to each class.


	fprintf( stderr, "StructuredCollectionClient::Start( %s, %d)\n", Query, NoOfMatches);


	/* ---------------------------------------------------------------------------------------
	   Declarations of variables
	   --------------------------------------------------------------------------------------- */

	/* Files on disk */
	char pcBaseDir[STRING_MAXSIZE];		/* Base Folder */
	char pcImageName[STRING_MAXSIZE];	/* Image file name */

	const char IMAGEFILE[] = "%s";	/* Name for image file name */


	/* List of best matches */
	MatchList theMatchList;

	/* Descriptors Schemes for query and element in database */
	GenericDS theDescription = GenericDS::CreateDescriptionRoot();
 	GenericDSInterfaceABC *theDescriptionInterface;
 	GenericDSInterfaceABC *DBDescriptionInterface;

	/* Extraction Tool */
	StructuredCollectionExtractionTool *theExtraction; 
	StructuredCollectionExtractionInterfaceABC *theExtractionInterface;

	/* Generic Coding Scheme */
	GenericDSCS *theCS; 
	GenericDSCSInterfaceABC *theCSInterface;

	/* Search Tool */
	StructuredCollectionSearchTool *theSearch; 
	StructuredCollectionSearchInterfaceABC *theSearchInterface;


	/* -----------------------------------------------------------------
	   Connect Interfaces 
	   -----------------------------------------------------------------*/
	
	/* Get description scheme interface */
 	theDescriptionInterface = theDescription.GetInterface(); 

	/* Connect DS to ExtractionTool */
	theExtraction = new
	  StructuredCollectionExtractionTool(theDescriptionInterface); 
	theExtractionInterface = theExtraction->GetInterface(); 

	/* Connect DS to GenericDSCS */
	theCS = new GenericDSCS(theDescriptionInterface); 
	theCSInterface = theCS->GetInterface();

	/* Connect DS to SearchTool */
	theSearch = 
		new StructuredCollectionSearchTool(theDescriptionInterface);
	theSearchInterface = theSearch->GetInterface();

	/* Prepare matchlist */
	if (NoOfMatches> (int)(RamDB.GetNoOfElements())) {
	  NoOfMatches = (int)(RamDB.GetNoOfElements());
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	fprintf(stderr, "Number of matches: %d\n", NoOfMatches);


	/* ---------------------------------------------------------------------------------------
	   Check for types of query: (1) Collection data (classes.txt) or (2) XML/DDL description (.xml)
	   --------------------------------------------------------------------------------------- */

	/* If extension of Query is "xml" them query by XML description (2) */
	if ( strncmp(Query+strlen(Query)-3, "xml", 3) && 
		strncmp(Query+strlen(Query)-3, "ddl", 3)) {

		/* ---------------------------------------------------------------------------------------
		   Query by collection data: extract Collection Structure DS from Query
		   --------------------------------------------------------------------------------------- */

		/* Set the Base Directory */
		strcpy( pcBaseDir, Query);
		//fprintf(stderr, "\n\nClass File: %s\n", query);
		strcat( pcBaseDir, ".dir/");

		/* Set image names */
		strcpy( pcImageName, pcBaseDir);
		strcat( pcImageName, IMAGEFILE);

		/* Set Data to StructuredCollectionExtraction */
		theExtractionInterface->SetSourceMedia(Query, pcImageName);

		/* Start Extraction */
		theExtractionInterface->StartExtracting();


	} else {

		/* ---------------------------------------------------------------------------------------
		   Query by XML description: decode SegmentRelationshipGraph DS from Query
		   --------------------------------------------------------------------------------------- */

		/* Connect description file to coding scheme */
		theCSInterface->SetDecoderFileName(Query);

		/* Decode Generic DS */
		theCSInterface->StartDecode();

	}


	/* ---------------------------------------------------------------------------------------
	   Match query with each item of database
	   --------------------------------------------------------------------------------------- */
	{

	int i;
	for (i=0; i<NoOfElements; i++) {

		/* Get StructuredCollection DS interface */
		DBDescriptionInterface=
			DBDescription[i].GetInterface();

		/* Connect query description scheme to search tool */
		theSearchInterface->
			SetRefDescriptionInterface(DBDescriptionInterface);

		/* sort result into list of best matches */
		theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
	}
	}


	/* ---------------------------------------------------------------------------------------
	   Print results of the query matching
	   --------------------------------------------------------------------------------------- */

	{
	int i;
	for (i=0; i<NoOfMatches ; i++) {
		if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
		printf("%s",
			RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
			GetFilename());
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
unsigned long StructuredCollectionClient::Stop()
{
	fprintf(stderr,"WARNING(StructuredCollectionClient): Stop() member function not Implemented.\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionClient::Close()
{

	// Free memories
	RamDB.FreeDatabase();
	if (DBDescription) delete [] DBDescription;
	
	NoOfElements = 0;
	DBDescription = 0;

	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& StructuredCollectionClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionClient::GetName(void)
{ return myName; }

