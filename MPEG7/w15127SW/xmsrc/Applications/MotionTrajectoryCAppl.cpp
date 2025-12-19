///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sylvie JEANNIN
// Philips Research USA
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
// Applicable File Name:  MotionTrajectoryCAppl.cpp
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
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"              /* Naming Convention */

using namespace XM;
using namespace std;

const UUID MotionTrajectoryClient::myID = UUID("");
const char *MotionTrajectoryClient::myName = "MotionTrajectoryClient";

//=============================================================================
MotionTrajectoryClient::MotionTrajectoryClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
MotionTrajectoryClient::~MotionTrajectoryClient()
{
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryClient::Open(char *ListFile, char *Bitstream)
{

  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  /* sets up an io binary file (buffer size: 512bytes) */
  DecoderFileIO theStreamBuffer(0xFFFF);

  /* descriptor interface */
  MotionTrajectoryDescriptorInterfaceABC *theDescriptorInterface	= NULL;

  /* Descriptors used for DDL/BiM based coding */
  GenericDS theDocument;
  GenericDSInterfaceABC *theDocumentInterface						= NULL;
  GenericDS theMpeg7Root;
  GenericDS theCollection;
  GenericDSInterfaceABC *theCollectionInterface						= NULL;
  GenericDS theDDLDescription;
  GenericDSInterfaceABC *theDDLDescriptionInterface					= NULL;

  /* coding scheme and its interface */
  int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
  MotionTrajectoryCS *theBinCS										= NULL;
  MotionTrajectoryCSInterfaceABC *theBinCSInterface					= NULL;
  GenericDSCS *theDDLCS												= NULL;
  GenericDSCSInterfaceABC *theDDLCSInterface						= NULL;

  /* For reading the xsi:type attributes */
  string xsitype;

  /* make new instatiations of these datatypes */
  try{
	//select coding scheme type
	CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

	if(CodingMode == 2) {	// Binary coding
	   /* allocate memory for coding scheme, get its interface */
       theBinCS = new MotionTrajectoryCS;
       theBinCSInterface=theBinCS->GetInterface();
	}
	else  {					// DDL or BiM coding
		/* Create the XML document, get its interface */
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface=theDocument.GetInterface();

		/* Create the XML parser based coding scheme, get its interface */
		theDDLCS = new GenericDSCS(theDocumentInterface);
		theDDLCSInterface=theDDLCS->GetInterface();

		/* Connect DDL file to coding scheme */
		theDDLCSInterface->SetDecoderFileName(Bitstream);

		/* Decode DDL file */
		theDDLCSInterface->StartDecode();

		/* Parse the top level elements */ 
		/* get the Mpeg7Root element */
		theMpeg7Root = theDocumentInterface->GetDescription("Mpeg7");
		if (theMpeg7Root.isNull()) {	// Mpeg7Root not found
			fprintf(stderr, "Top level is not Mpeg7\n");
			return (unsigned long) -1;
		}

		/* Get the Collection element for the descriptors */
		theCollection = theMpeg7Root.GetDescription("DescriptionUnit");
		if(theCollection.isNull()) {	// Collection not found
			fprintf(stderr, "DescriptionUnit not found\n");
			return (unsigned long) -1;
		}
		theCollection.GetTextAttribute("xsi:type", xsitype);
		if(xsitype != "DescriptorCollectionType") {
			fprintf(stderr, "DescriptorCollection not found\n");
			return (unsigned long)-1;
		}

		/* Get Collection interface */
		theCollectionInterface=theCollection.GetInterface();

		/* initalize theDDLdescription */
		theDDLDescription=theCollection;
		theDDLDescriptionInterface=theCollectionInterface;
    
		/* Create the XML parser based coding scheme, get its interface */
		theDDLCS = new GenericDSCS();
		theDDLCSInterface=theDDLCS->GetInterface();
	}
  }
  catch(...) {
	fprintf(stderr,"MotionTrajectoryClient::Open: Allocation error\n");
	return(1);
  }

  /* connect stream to coding scheme and open bitstream */
  if(CodingMode == 2) { // Binary coding
	theBinCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
    theStreamBuffer.OpenBitStream(Bitstream);
  }

  /* read filename data base */
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  /* create table with references to descriptors in data base*/
  DBDescription = new MotionTrajectoryDescriptor* [NoOfElements];

  for (int i=0; i<NoOfElements; i++) {

     /* allocate memory for descriptors */
     //COORD	  DBDescription[i] = new MotionTrajectoryDescriptor(0,0);
     DBDescription[i] = new MotionTrajectoryDescriptor(0);
     DBDescription[i]->addref();

     /* get motion trajectory interface*/
     theDescriptorInterface= DBDescription[i]->GetInterface();

	 if(CodingMode == 2) { // Binary coding
		/* connect descriptors to coding schemes*/
		theBinCSInterface->SetDescriptorInterface(theDescriptorInterface);

		/* decode motion trajectory*/
		theBinCSInterface->StartDecode();
	 }
	 else {				// DDL or BiM coding
		/* get next descriptor */
		if (i==0) {
			theDDLDescription=theCollectionInterface->GetDescription("Descriptor");
		}
		else {
			theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
		}

		/* search for correct xsi type */
		while(!theDDLDescription.isNull()) {
			theDDLDescription.GetTextAttribute("xsi:type", xsitype);
			if(xsitype=="MotionTrajectoryType") break;
			theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
		}

		if(theDDLDescription.isNull()) // MotionTrajectoryType not found
			return (unsigned long)-1;

		// Get DDLDescription interface
		theDDLDescriptionInterface=theDDLDescription.GetInterface();

		// Import description from DDL Tree
		theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
	 }
     //int bibi;
     //cerr << "Print tests after decoding\n";
     //cerr << "Element" << i << "\n";
     // for (bibi=0; bibi<theDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyPointNum(); bibi++)
     //   cerr << theDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetMediaTimePoint(bibi)
     //   << "  " << theDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(0,bibi)
     //   << "  " << theDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(1,bibi)
     //   << "  " << theDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(2,bibi) << "\n";
  }

  if(CodingMode == 2) { // Binary coding
	theBinCSInterface->destroy();
	theStreamBuffer.CloseBitStream();
  }
  else {
	theDDLCSInterface->destroy();
  }

  return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryClient::Start(char *Query,int NoOfMatches)
{

  /* Check data is sane */
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  /* get the query type from the parameter file:      
  (currently 15 query types implemented)
  0: on all trajectory data for same time positions
  1: on positions only for same time positions
  2: on speeds only for same time positions
  3: on accelerations only for same time positions
  4: on all trajectory data, start instants aligned
  5: on positions only, start instants aligned
  6: on speeds only, start instants aligned
  7: on accelerations only, start instants aligned
  8: on positions only, times normalized to align durations
  11: find objects moving to the right
  12: find objects moving to the left
  13: find objects moving up
  14: find objects moving down */
  //int queryType=8;
  int queryType = atoi(getpara("QueryType", "8"));

  /****************/
  /* DECLARATIONS */
  /****************/

  /* media and its interface */
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;

  /* list with best matches*/
  MatchList theMatchList;

  /* descriptors interfaces: Query and Database */
  MotionTrajectoryDescriptorInterfaceABC *theDescriptorInterface	= NULL;
  MotionTrajectoryDescriptorInterfaceABC *DBDescriptorInterface		= NULL;

  /* extraction tool */
  MotionTrajectoryExtractionTool *theExtractionTool			= NULL;
  MotionTrajectoryExtractionInterfaceABC *theExtractionInterface	= NULL;

  /* search tool*/
  MotionTrajectorySearchTool *theSearchTool				= NULL;
  MotionTrajectorySearchInterfaceABC *theSearchInterface		= NULL;


  /*******************************************/
  /* GET INTERFACES, CONNECT THINGS TOGETHER */
  /*******************************************/

  try {
     /* create media data class and get interface*/
     theMedia = new MultiMedia;
     theMediaInterface=theMedia->GetInterface();

     /* get motion trajectory extraction interface - create also descriptor */
     theExtractionTool = new MotionTrajectoryExtractionTool(0);
     theExtractionInterface = theExtractionTool->GetInterface();

     /* get descriptor interface*/
     theDescriptorInterface = theExtractionInterface->GetDescriptorInterface();

     /* get motion trajectory search interface*/
     theSearchTool = new MotionTrajectorySearchTool(theDescriptorInterface);
     theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
	 fprintf(stderr,"MotionTrajectoryClient::Start: Allocation error\n");
	 return(1);
  }

  /* Connexion media-extraction */
  int check_int;
  check_int = theExtractionInterface->SetSourceMediaInterface(theMediaInterface);

  /* connect query descriptor to search tool*/
  theSearchInterface->SetQueryDescriptorInterface(theDescriptorInterface);

  /* read here any predefined values: coordinates + others? */
  /* TBD */

  /* specify type of query */
  theSearchInterface->SetQueryType(queryType);
  switch (queryType) {
    //sth don´t write to stdout
	case 0:   /* query on whole trajectory elements, equally */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - comparing only data at same time instants        */\n";
	   cerr << "/*     - based on positions, speeds and accelerations     */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 1:   /* query by position only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - comparing only data at same time instants        */\n";
	   cerr << "/*     - based on positions only                          */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 2:   /* query by speed only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - comparing only data at same time instants        */\n";
	   cerr << "/*     - based on speeds only                             */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 3:   /* query by acceleration only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - comparing only data at same time instants        */\n";
	   cerr << "/*     - based on accelerations only                      */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 4:   /* query on whole trajectory elements, equally */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - aligning start instants                          */\n";
	   cerr << "/*     - based on positions, speeds and accelerations     */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 5:   /* query by position only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - aligning start instants                          */\n";
	   cerr << "/*     - based on positions only                          */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 6:   /* query by speed only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - aligning start instants                          */\n";
	   cerr << "/*     - based on speeds only                             */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 7:   /* query by acceleration only */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - aligning start instants                          */\n";
	   cerr << "/*     - based on accelerations only                      */\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 8:   /* query independant of time: by position only (find replays...) */
	   cerr << "/**********************************************************/\n";
	   cerr << "/*             QUERY BY TRAJECTORY SIMILARITY:            */\n";
	   cerr << "/*     - normalizing time: based on positions & relative s*/\n";
	   cerr << "/**********************************************************/\n";
	   break;
	case 11:   /* higher level query : find objects moving to the right */
	   cerr << "/************************************************/\n";
	   cerr << "/*              HIGH LEVEL QUERY :              */\n";
	   cerr << "/*     - find objects moving to the right       */\n";
	   cerr << "/************************************************/\n";
	   break;
	case 12:   /* higher level query : find objects moving to the left */
	   cerr << "/************************************************/\n";
	   cerr << "/*              HIGH LEVEL QUERY :              */\n";
	   cerr << "/*     - find objects moving to the left        */\n";
	   cerr << "/************************************************/\n";
	   break;
	case 13:   /* higher level query : find objects moving up */
	   cerr << "/**************************************/\n";
	   cerr << "/*         HIGH LEVEL QUERY :         */\n";
	   cerr << "/*     - find objects moving up       */\n";
	   cerr << "/**************************************/\n";
	   break;
	case 14:   /* higher level query : find objects moving down */
	   cerr << "/****************************************/\n";
	   cerr << "/*          HIGH LEVEL QUERY :          */\n";
	   cerr << "/*     - find objects moving down       */\n";
	   cerr << "/****************************************/\n";
	   break;
	default:
	   cerr << " invalid query type: " << queryType << "\n";
	   break;
  }

  /* prepare matchlist*/
  if (NoOfMatches>RamDB.GetNoOfElements()) {
     NoOfMatches=RamDB.GetNoOfElements();
  }
  theMatchList.SetNumberOfMatches(NoOfMatches);

  /* malloc media and load input data*/
  RamDB.SetMedia(theMediaInterface);
  if (!RamDB.LoadNamed4DKeyPointList(Query))	//SJ-0621
  //if (!RamDB.LoadNamedKeyPointList(Query))	//SJ-0621
     return 0;

#ifdef XYUVDISP
  /*could here display trajectory as an image: have fct to build VOP from file*/
#endif

  /* extract descriptor from media */
  int KeyPointsNumber = theMediaInterface->GetKeyPointNumber();
  theExtractionInterface->InitExtracting(); /* should values: do nothing yet */
  theExtractionInterface->StartExtracting(KeyPointsNumber); //has to follow same strategy as Server: Post extraction?

  /* free media*/
  theMediaInterface->FreeKeyPointLists();	// CHECK - SJ AUG 23

  int i;
  double matchingDistance;
  for (i=0; i<NoOfElements; i++) {

     /* get motion trajectory interface */
     DBDescriptorInterface = DBDescription[i]->GetInterface();

     /* connect query descriptor to search tool*/
     theSearchInterface->SetRefDescriptorInterface(DBDescriptorInterface);

     /* sort result into list of best matches*/
     if (queryType < 10)      // query by similarity
       matchingDistance = theSearchInterface->GetDistance();
     else                     // higher level query
       matchingDistance = theSearchInterface->GetHighLevelFeatureMatch();
     theMatchList.InsertMatch(i,matchingDistance);
     cerr << "Matching Result " << i << " = " << matchingDistance << "\n";
  }

  for (i=0; i<NoOfMatches ; i++) {
     if (theMatchList.GetDBEntryNo(i) == (unsigned long) -1)
        break;
     printf("%s", RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
     printf("\t%e",theMatchList.GetDistanceValue(i));
     printf("\n");
  }

  theMediaInterface->destroy();
  theExtractionInterface->destroy();
  theSearchInterface->destroy();

  return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryClient::Stop()
{
  return 0;
}
//----------------------------------------------------------------------------
unsigned long MotionTrajectoryClient::Close()
{
  /* free memories*/
  RamDB.FreeDatabase();     /*check SJ*/

  int i;
  for (i=0; i<NoOfElements; i++) {
     if (DBDescription[i])
	DBDescription[i]->release();
  }

  NoOfElements =0;
  DBDescription =0;

  return 0;
}


//----------------------------------------------------------------------------
const UUID& MotionTrajectoryClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryClient::GetName(void)
{ return myName; }



