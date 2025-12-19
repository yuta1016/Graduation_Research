///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
// (contributing organizations names)
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
// Applicable File Name:  SpatioTemporalLocatorSAppl.cpp
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
#ifdef _VISUALC_
#include "io.h"
#endif

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "Media/Media.h"
#include "SearchUtilities/MatchList.h"

#include <fstream>

using namespace XM;

const UUID SpatioTemporalLocatorClient::myID = UUID("");
const char *SpatioTemporalLocatorClient::myName = "Spatio Temporal Locator Client";

//=============================================================================
SpatioTemporalLocatorClient::SpatioTemporalLocatorClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
SpatioTemporalLocatorClient::~SpatioTemporalLocatorClient()
{
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorClient::Open(char *ListFile, char *Bitstream)
{
    // Check that the file names are ok
    assert(ListFile  != NULL);
    assert(Bitstream != NULL);

    // -------------------- Initialisation of data ----------------------------
	//* stream data base*
	DecoderFileIO theStreamBuffer(0x2FFFF);

	// descriptor and sub-descriptor
	SpatioTemporalLocatorDescriptorInterfaceABC
	  *theSpatioTemporalLocatorDescriptorInterface;

	// coding schemes and sub CS
	SpatioTemporalLocatorCS *theSpatioTemporalLocatorCS;
	SpatioTemporalLocatorCSInterfaceABC *theSpatioTemporalLocatorCSInterface;

	GenericDSCS*                     theDDLCS               = NULL;
	GenericDSCSInterfaceABC*         theDDLCSInterface      = NULL;

	int    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
		theSpatioTemporalLocatorCS = new SpatioTemporalLocatorCS(0);
		theSpatioTemporalLocatorCSInterface=
		  theSpatioTemporalLocatorCS->GetInterface();

	} else {				// DDL or BiM coding
		// Now create the XML parser based coding scheme
		theDDLCS = new GenericDSCS();

		// Obtain the interface
		theDDLCSInterface=theDDLCS->GetInterface();
	}


	if (CodingMode == 2) { // Binary coding
		//* connect stream to coding scheme*
		theSpatioTemporalLocatorCSInterface->
		  SetDecoderStreamBuffer(&theStreamBuffer);

		//* open bit stream*
		theStreamBuffer.OpenBitStream(Bitstream);
	}

	//* read filename data base*
	RamDB.CreateDatabase(ListFile);
//	NoOfElements=RamDB.GetNoOfElements();
	NoOfElements = 1;    // because one STL-DS make from a lot of pictures.
	//sth no more than one stl-DS possible

	// create in memory decompressed database*
	//* allocate memeories*
	DBDescription =
	  new SpatioTemporalLocatorDescriptor* [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  /* allocate memory for descriptor*/
		DBDescription[i] =
		   new SpatioTemporalLocatorDescriptor(0,0);
		DBDescription[i]->addref();
	
		//* get motion activity interface*
		theSpatioTemporalLocatorDescriptorInterface=
			DBDescription[i]->GetInterface();

        if (CodingMode == 2) { // Binary coding
			//* connect descriptors to coding schemes*
			theSpatioTemporalLocatorCSInterface->
				SetDescriptorInterface(theSpatioTemporalLocatorDescriptorInterface);

			//*decode motion activity*
			theSpatioTemporalLocatorCS->StartDecode();

		} else {
			GenericDS theMpeg7Root, theCollection;
			GenericDSInterfaceABC* theCollectionInterface;
		    GenericDS theDDLDescription = GenericDS::CreateDescriptionRoot();
		    GenericDSInterfaceABC* theDDLDescriptionInterface =
				theDDLDescription.GetInterface();

			assert(theSpatioTemporalLocatorDescriptorInterface     != NULL);

			// Get DDL file name from data media filename and DDL file name
			//char path[256];
			//strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
			//strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
			//strncat(path,"DDLFile",MAXMEDIANAMELEN-1-strlen(path));
//sth what is this for when we have a bitstream file name??
			//path[MAXMEDIANAMELEN-1]=0;

			//strcpy(path, Bitstream);
//			strcat(path, ".xml");	// deleted by Kaneko
			//if( strncmp(path+strlen(path)-4,".xml",4) != 0 ) strcat(path, ".xml");	// inserted by Kaneko
			//sth I prefered the previous version. The name can
			//Sth be set in the parameter file or command line!


			// Connect DDL file to coding scheme
			//theDDLCSInterface->SetDecoderFileName(path);
			theDDLCSInterface->SetDecoderFileName(Bitstream);

			// Connect descriptors to coding schemes
			theDDLCSInterface->SetDescriptionInterface(theDDLDescriptionInterface);

			// Write the D/DSType to binary
			theDDLCSInterface->StartDecode();

			// Parse the top level elements
			theMpeg7Root = theDDLDescriptionInterface->GetDescription("Mpeg7");		
			// Mpeg7 root not found
			if (theMpeg7Root.isNull())
			{
				fprintf(stderr, "Top level is not Mpeg7\n");
				return (unsigned long)-1;
			}		
			// Get the collection element for the descriptors
			theCollection = theMpeg7Root.GetDescription("DescriptionUnit");
			// Collection not found
			if (theCollection.isNull())
			{
				fprintf(stderr, "DescriptionUnit not found\n");
				return (unsigned long)-1;

			}
		
			string xsitype;
			theCollection.GetTextAttribute("xsi:type", xsitype);
			if (xsitype != "MovingRegionType")
			{
				fprintf(stderr, "MovingRegionType not found\n");
				return (unsigned long)-1;
			}		
			
			// Get the collection in terface
			theCollectionInterface = theCollection.GetInterface();


			// Import description from DDL Tree
			GenericDS DS_Ele0=theCollectionInterface->GetFirstChild();
			GenericDSInterfaceABC *DS_Ele=DS_Ele0.GetInterface();

			theSpatioTemporalLocatorDescriptorInterface->ImportDDL(DS_Ele);
		}

	}

	// Perform the tidying up
	if (CodingMode == 2) { // Binary coding
		theSpatioTemporalLocatorCSInterface->destroy();

		theStreamBuffer.CloseBitStream();
	} else {
		theDDLCSInterface->destroy();
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorClient::Start(char *Query, int NoOfMatches)
{
#define MaxMatch 30
        int i;

    // Check data is sane
    assert(Query       != NULL);
    assert(NoOfMatches != 0);

    // --------------------- Initialise the query data -----------------------
    /* image media*/
    MultiMedia *theMedia;
    MultiMediaInterfaceABC *theMediaInterface;

	//* list with best matches*
	MatchList theMatchList;

	SpatioTemporalLocatorDescriptorInterfaceABC
	  *DBSpatioTemporalLocatorDescriptorInterface = NULL;

	//* search tool*
	SpatioTemporalLocatorSearchTool *theSearchTool = NULL;
	SpatioTemporalLocatorSearchInterfaceABC *theSearchInterface = NULL;

	
	/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	//* get motion activity search interface*
	theSearchTool = new SpatioTemporalLocatorSearchTool(0);
	theSearchInterface=theSearchTool->GetInterface();


	/* load input data*/
	RamDB.SetMedia(theMediaInterface);
	if (!RamDB.LoadNamedKeyPointList(Query)) return 0;


	//* prepare matchlist*
//	int a = RamDB.GetNoOfElements();
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	for (i=0; i<NoOfElements; i++) {
		theMediaInterface->GotoKeyPointList(i);
		theSearchInterface->SetQueryPoint(theMediaInterface); 

		DBSpatioTemporalLocatorDescriptorInterface=
			DBDescription[i]->GetInterface();

	  //* connect query descriptor to search tool*
		theSearchInterface->SetRefDescriptorInterface(
			DBSpatioTemporalLocatorDescriptorInterface);

	  //* sort result into list of best matches*
		theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	}

  // -------------------- Display the best matches --------------------------
	fprintf(stderr,"#### Matching Result ####\n");
	for (i=0; i<NoOfMatches ; i++) {
	    if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
		theMediaInterface->GotoKeyPointList(theMatchList.GetDBEntryNo(i));
		theMediaInterface->GotoKeyPoint(0);
		printf("(t,x,y)=(%d, %.2f, %.2f)", theMediaInterface->GetKeyPoint()->time,
			 theMediaInterface->GetKeyPoint()->p[0], theMediaInterface->GetKeyPoint()->p[1]);
        printf("\t%f" , theMatchList.GetDistanceValue(i));
	    printf("\n");

	  fflush(stdout);
	}

	theMediaInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorClient::Stop()
{
//	fprintf(stderr,"Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SpatioTemporalLocatorClient::Close()
{
	/* free memories*/
    RamDB.FreeDatabase();
	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {
	    if (DBDescription[i])
	      DBDescription[i]->release();
	  }
	}
	
	NoOfElements =0;
	DBDescription =0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SpatioTemporalLocatorClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpatioTemporalLocatorClient::GetName(void)
{ return myName; }

