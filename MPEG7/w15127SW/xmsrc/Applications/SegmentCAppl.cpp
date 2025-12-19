///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez		- Columbia University.
// Javier Ruiz Hidalgo	- Universidad Politecnica de Catalunya.
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
// Applicable File Name:  SegmentCAppl.cpp
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

#include <iostream.h>
#include <fstream.h>
//sth not allowed
#include <dom/DOM.hpp>
#include <util/PlatformUtils.hpp>		// Initiliase XML.



using namespace XM;

const UUID SegmentClient::myID = UUID("");
const char *SegmentClient::myName = "Segment DS Client";

const int	SegmentClient::STRING_MAXSIZE= 4096;		// maximum size of the strings


//=============================================================================
SegmentClient::SegmentClient():
RamDB(),
NoOfElements(0),
DBSegmentDS(0)
{
}

//----------------------------------------------------------------------------
SegmentClient::~SegmentClient()
{
}

//----------------------------------------------------------------------------
unsigned long SegmentClient::Open(char *ListFile, char *DescriptionFile)
{

	// NOTES Open() method of the SegmentDS Client
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
	// description file: data1/SegmentDS.xml; being "SegmentDS.xml" the value of DescriptionFile).
	

	// ---------------------------------------------------------------------------------------
    // Initialize the XML4C system
	// ---------------------------------------------------------------------------------------
  //sth not allowed
	try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        fprintf( stderr, "ERROR(GenericDS): Error during Xerces-c Initialization: ");
		fprintf( stderr, (char *)toCatch.getMessage());
		exit(-1);
    }


	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	char        pcBaseDir[STRING_MAXSIZE];		// Base Folder
	char		XMLFile[STRING_MAXSIZE];		// Description XML File
	char		*pcTmp;							// Temp pointer to string


	// Description Scheme Interface
 	GenericDSInterfaceABC *theSegmentDSInterface;

	// Generic Coding Scheme
	GenericDSCS theSegmentDSCS;
	GenericDSCSInterfaceABC *theSegmentDSCSInterface;


	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------

	// Get coding scheme interface
	theSegmentDSCSInterface = theSegmentDSCS.GetInterface();


	// ---------------------------------------------------------------------------------------
	// Read database in ListFile
	// ---------------------------------------------------------------------------------------

	// Read filename data base
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	// Create in memory decompressed database
	// allocate memories
	DBSegmentDS =
	  new GenericDS [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  //sth RamDB contains media file names.
	  //sth DDl filename should be named in a different way
		// Set the Base Directory
		strcpy(pcBaseDir, RamDB.GotoEntry(i)->GetFilename());
		// *nix style dirs
		pcTmp=strrchr( pcBaseDir, '/');
		if (pcTmp!=NULL) {
			pcTmp[1] = '\0';
		} else {
			// Win* style //sth not required
			pcTmp = strrchr( pcBaseDir, '\\');
			if (pcTmp!=NULL) {
				pcTmp[1] = '\0';
			}
		}

		// Generate full description file path
		strcpy(XMLFile, pcBaseDir);
		strcat(XMLFile, DescriptionFile);

		// Connect description file to coding scheme
		theSegmentDSCSInterface->SetXMLFileI(XMLFile);

		// Get SegmentDS interface
		theSegmentDSInterface = DBSegmentDS[i].GetInterface();

		// Connect Generic DS to coding schemes
		theSegmentDSCSInterface->SetDSInterface(theSegmentDSInterface);

		// Decode Generic DS
		theSegmentDSCSInterface->StartDecode();

	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentClient::Start(char *Query,int NoOfMatches)
{

	// NOTES Start() method of the SegmentDS Client
	// 
	// Query is an input image with information of 
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
	// Finally, the original frames and masks are needed as "f_%frameID" and 
	// "m_%maskID_%frameID".

	fprintf( stderr, "SegmentClient::Start( %s, %d)\n", Query, NoOfMatches);

	// ---------------------------------------------------------------------------------------
    // Initialize the XML4C system
	// ---------------------------------------------------------------------------------------
	//sth not allowed
	try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        fprintf( stderr, "ERROR(GenericDS): Error during Xerces-c Initialization: ");
		fprintf( stderr, (char *)toCatch.getMessage());
		exit(-1);
    }

	// ---------------------------------------------------------------------------------------
	// Declarations of variables
	// ---------------------------------------------------------------------------------------

	// Files on disk
	const char	TREEFILE[]	  = "tree.txt";		// name of the tree file on disk.
	const char	FRAMESFILE[]  = "f_%d.ppm";		// Frames name on disk.
	const char  MASKSFILE[]   = "m_%d_%d.ras";  // Masks on disk.

	char		*pcTmp;
	char        pcBaseDir[STRING_MAXSIZE];		// Base Folder
	char		pcTreeName[STRING_MAXSIZE];		// Tree file name on disk
	char		pcFramesName[STRING_MAXSIZE];	// Frames name on disk
	char		pcMasksName[STRING_MAXSIZE];	// Frames name on disk

	FILE		*fp;						// File pointer to read the tree file
	MomVop		*media;						// Image media (FS image)

	// List of best matches
	MatchList theMatchList;

	// Descriptors Schemes for query and element in database
	GenericDS theSegmentDS;
 	GenericDSInterfaceABC *theSegmentDSInterface;
 	GenericDSInterfaceABC *DBSegmentDSInterface;

	// Extraction Tool
	SegmentExtractionTool theSegmentDSExtractionTool;
	SegmentExtractionInterfaceABC *theSegmentDSExtractionToolInterface;

	// Search Tool
	SegmentSearchTool theSegmentDSSearchTool;
	SegmentSearchInterfaceABC *theSegmentDSSearchToolInterface;


	// ---------------------------------------------------------------------------------------
	// Connect Interfaces 
	// ---------------------------------------------------------------------------------------
	
	// Get description scheme interface
 	theSegmentDSInterface = theSegmentDS.GetInterface();

	// Connect DS to ExtractionTool
	theSegmentDSExtractionToolInterface = theSegmentDSExtractionTool.GetInterface();
	theSegmentDSExtractionToolInterface->
	  SetDescriptorInterface(theSegmentDSInterface);

	// Connect DS to SearchTool
	theSegmentDSSearchToolInterface =
	  theSegmentDSSearchTool.GetInterface();
	theSegmentDSSearchToolInterface->
	  SetQueryDSInterface(theSegmentDSInterface);

	// Prepare matchlist
	if (NoOfMatches> (int)(RamDB.GetNoOfElements())) {
	  NoOfMatches = (int)(RamDB.GetNoOfElements());
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	fprintf(stderr, "Number of matches: %d\n", NoOfMatches);

	// ---------------------------------------------------------------------------------------
	// Start Extraction for query
	// ---------------------------------------------------------------------------------------

	// Initialize values for the extraction before first frame
	// Not needed

	// Loop for every Database in ListFile (pointer to FS image)
	// Load input Data (FS image)
	media = RamDB.LoadNamedImage(Query);

	// Stop if null image
	if (!media) return 0;

#ifdef XYUVDISP
	// display input image
	xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s, 
		(unsigned short *)media->u_chan->data->s, 
		(unsigned short *)media->v_chan->data->s,
		datasize(media->y_chan)&1, media->y_chan->x,media->y_chan->y,
		media->u_chan->x,media->u_chan->y, "input");
#endif

	//sth see Open
	// Set the Base Directory
	strcpy( pcBaseDir, Query);
	// *nix style dirs
	pcTmp=strrchr( pcBaseDir, '/');
	if (pcTmp!=NULL) {
		pcTmp[1] = '\0';
	} else {
		// Win* style
		pcTmp = strrchr( pcBaseDir, '\\');
		if (pcTmp!=NULL) {
			pcTmp[1] = '\0';
		}
	}
		
	// Read the "tree.txt" file attached to the FS image
	strcpy( pcTreeName, pcBaseDir);
	strcat( pcTreeName, TREEFILE);
	fp = fopen( pcTreeName, "r");
	if (fp==NULL) {
		fprintf( stderr, "ERROR(SegmentClient): Couldn't not find %s file.\n", pcTreeName);
		exit(-1);
	}

	// Set frame and mask names
	strcpy( pcFramesName, pcBaseDir);
	strcat( pcFramesName, FRAMESFILE);
	strcpy( pcMasksName, pcBaseDir);
	strcat( pcMasksName, MASKSFILE);

	// Create an empty Document
	DOM_Document DOMDoc;
	DOMDoc = DOM_Document::createDocument();

	// Connect DOM Document to DS
	theSegmentDSInterface->SetDOMDocument(DOMDoc);

	// Set Data to SegmentExtraction
	theSegmentDSExtractionToolInterface->SetSourceMedia(media, fp, pcFramesName, pcMasksName);

	// Start Extraction
	theSegmentDSExtractionToolInterface->StartExtracting();

	// Close filehandle (Tree file)
	fclose(fp);

	// Free media (FS image)
	freevop(media);


	// ---------------------------------------------------------------------------------------
	// Match query with each item of database
	// ---------------------------------------------------------------------------------------
	{

	int i;
	for (i=0; i<NoOfElements; i++) {

		// Get Segment DS interface
		DBSegmentDSInterface=
			DBSegmentDS[i].GetInterface();

		// Connect query description scheme to search tool
		theSegmentDSSearchToolInterface->
			SetRefDSInterface(DBSegmentDSInterface);

		// sort result into list of best matches
		theMatchList.InsertMatch(i, theSegmentDSSearchToolInterface->GetDistance());
	}
	}


	// ---------------------------------------------------------------------------------------
	// Print results of the query matching
	// ---------------------------------------------------------------------------------------
	fprintf(stderr, "\n\nList of results: (item, distance)\n");

	{
	int i;
	for (i=0; i<NoOfMatches ; i++) {
		if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
		printf("%s",
			RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
			GetFilename());
		fflush(stdout);
	//#ifdef DEBUG
		fprintf(stderr,"\t%e",theMatchList.GetDistanceValue(i));
	//#endif
		printf("\n");
	}
	}


	// ---------------------------------------------------------------------------------------
	// Ok, we've finished :-)
	// ---------------------------------------------------------------------------------------

	return(0);
	
}

//----------------------------------------------------------------------------
unsigned long SegmentClient::Stop()
{
	fprintf(stderr,"WARNING(SegmentClient): Stop() member function not Implemented.\n");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentClient::Close()
{

	// Free memories
	RamDB.FreeDatabase();
	if (DBSegmentDS) delete [] DBSegmentDS;
	
	NoOfElements = 0;
	DBSegmentDS = 0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SegmentClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentClient::GetName(void)
{ return myName; }










				



