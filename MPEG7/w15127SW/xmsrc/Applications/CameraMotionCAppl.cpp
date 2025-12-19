//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips
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
// Applicable File Name:  CameraMotionCAppl.cpp
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
#include "Utilities/Utilities.h"              /* Naming Convention */

using namespace XM;

const UUID CameraMotionClient::myID = UUID("");
const char *CameraMotionClient::myName = "Camera Motion Client";

//=============================================================================
CameraMotionClient::CameraMotionClient():
/*RamDB()*/
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
CameraMotionClient::~CameraMotionClient()
{
}

//----------------------------------------------------------------------------
unsigned long CameraMotionClient::Open(char *ListFile, char *Bitstream)
{
	int i ;

	// Check that the file names are ok		//0719-Sylvie
	assert(ListFile  != NULL);				//0719-Sylvie
	assert(Bitstream != NULL);				//0719-Sylvie

	fprintf(stderr,"Open camera motion client \n") ;
	/* Stream data base */
	DecoderFileIO theStreamBuffer(0x1000);
     
	/* Descriptor interface */
	CameraMotionDescriptorInterfaceABC  *theDescriptorInterface;
	
	/* Descriptors used for DDL/BiM based coding */							//0719-Sylvie
	GenericDS theDocument;													//0719-Sylvie
	GenericDSInterfaceABC *theDocumentInterface						= NULL;	//0719-Sylvie
	GenericDS theMpeg7Root;													//0719-Sylvie
	GenericDS theCollection;												//0719-Sylvie
	GenericDSInterfaceABC *theCollectionInterface					= NULL;	//0719-Sylvie
	GenericDS theDDLDescription;											//0719-Sylvie
	GenericDSInterfaceABC *theDDLDescriptionInterface				= NULL;	//0719-Sylvie

	/* Coding scheme*/
	int CodingMode;  // 0:DDL, 1: BiM, 2:Binary								//0719-Sylvie
	//CameraMotionCS * theCS = new CameraMotionCS ;							//0719-Sylvie
	CameraMotionCS *theBinCS										= NULL;	//0719-Sylvie
	/* Coding scheme interface */
	//CameraMotionCSInterfaceABC *theCSInterface = theCS->GetInterface();	//0719-Sylvie
	CameraMotionCSInterfaceABC *theBinCSInterface					= NULL;	//0719-Sylvie
	GenericDSCS *theDDLCS											= NULL;	//0719-Sylvie
	GenericDSCSInterfaceABC *theDDLCSInterface						= NULL;	//0719-Sylvie

	/* For reading the xsi:type attributes */								//0719-Sylvie
	string xsitype;															//0719-Sylvie

	/* make new instatiations of these datatypes */							//0719-Sylvie
	try{				// added until the following catch statement by:	//0719-Sylvie
	//select coding scheme type
	CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

	if(CodingMode == 2) {	// Binary coding
	   /* allocate memory for coding scheme, get its interface */
       theBinCS = new CameraMotionCS;
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
		fprintf(stderr,"CameraMotionClient::Open: Allocation error\n");
		return(1);
	}	// end of catch corresponding to text added by:	//0719-Sylvie

	/* connect stream to coding scheme*/
	if(CodingMode == 2) { // Binary coding	//0719-Sylvie
		//theCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
		theBinCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

		/* open bit stream*/
		theStreamBuffer.OpenBitStream(Bitstream);
	}					//0719-Sylvie

	/* read filename data base*/
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* create in memory decompressed database*/
	/* allocate memories*/
	DBDescription =	  new CameraMotionDescriptor *[NoOfElements];

	for (i=0; i<NoOfElements; i++) {

	  /* Allocate memory for descriptor*/
	  DBDescription[i] = new CameraMotionDescriptor();
	  DBDescription[i]->addref();

	  /* get motion activity interface*/
	  theDescriptorInterface=  DBDescription[i]->GetInterface();

	  if(CodingMode == 2) { // Binary coding								//0719-Sylvie
		/* connect descriptors to coding schemes*/
		//theCSInterface->SetDescriptorInterface(theDescriptorInterface);	//0719-Sylvie
		theBinCSInterface->SetDescriptorInterface(theDescriptorInterface);	//0719-Sylvie

		/*decode motion activity*/
		//theCS->StartDecode();												//0719-Sylvie
		theBinCSInterface->StartDecode();									//0719-Sylvie
	  }																		//0719-Sylvie
	 else {				// DDL or BiM coding	// else statement added by	//0719-Sylvie
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
			if(xsitype=="CameraMotionType") break;
			theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
		}

		if(theDDLDescription.isNull()) // CameraMotionType not found
			return (unsigned long)-1;

		// Get DDLDescription interface
		theDDLDescriptionInterface=theDDLDescription.GetInterface();

		// Import description from DDL Tree
		theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
	 }	// end of else statement added by	//0719-Sylvie
	}

	// sth here are also some destroys required

	if(CodingMode == 2) { // Binary coding	//0719-Sylvie
		theBinCSInterface->destroy();		//0719-Sylvie
		theStreamBuffer.CloseBitStream();
	}										//0719-Sylvie
	else {									//0719-Sylvie
		theDDLCSInterface->destroy();		//0719-Sylvie
	}										//0719-Sylvie

	return 0;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionClient::Start(char *Query,int NoOfMatches)
{
 	fprintf(stderr,"Start camera motion client \n") ;

	MultiMedia* media ;
	MultiMediaInterfaceABC *mediaInterface;
	// sth unknown media, are mpg sequences feasible? MediaForCameraMotion *media=0;

	/* list with best matches*/
	MatchList theMatchList;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0xFFFF);

    /* descriptor */
	CameraMotionDescriptorInterfaceABC * theDescriptorInterface;
	CameraMotionDescriptorInterfaceABC *DBDescriptorInterface;


	/* extraction tool*/
	CameraMotionExtractionTool * theExtractionTool;
	CameraMotionExtractionInterfaceABC  *theExtractionInterface;
	
	/* search tool*/
	CameraMotionSearchTool* theSearchTool;
	CameraMotionSearchInterfaceABC *theSearchInterface;

	/* Create descriptor and extraction method */
	theExtractionTool = new CameraMotionExtractionTool(0) ;
	theExtractionInterface = theExtractionTool->GetInterface() ;

	/* get  interface*/
	theDescriptorInterface=  theExtractionInterface->GetDescriptorInterface();

	/* get search interface*/
	theSearchTool = new CameraMotionSearchTool(theDescriptorInterface) ;
	theSearchInterface=theSearchTool->GetInterface();
	
	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);


	/* malloc media and load input data*/
	//media=ComputeFromText(Query) ;
	// 2013/07/23 KW fix
	media = new MultiMedia();
	mediaInterface = media->GetInterface();

	media->SetVideoFileName(Query) ;
	if (!media) return 0;

	/* connect media to extarction tool*/
	theExtractionInterface->SetSourceMedia(mediaInterface);

	/* extract descriptor form media*/
	theExtractionInterface->StartExtracting();

	int i;
	for (i=0; i<NoOfElements; i++) {

	  /* get camera motion interface*/
	  DBDescriptorInterface =  DBDescription[i]->GetInterface();

	  /* connect query descriptor to search tool*/
	  theSearchInterface->SetRefDescriptorInterface(DBDescriptorInterface);

	  /* sort result into list of best matches*/
	  theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	}

	fprintf(stderr,"  -----------------------------------------------------------------------\n") ;
	fprintf(stderr,"    Matching Result for %s\n",Query);
	fprintf(stderr,"  -----------------------------------------------------------------------\n\n") ;

	/*ShotDBElement *element;*/
	for (i=0; i<NoOfMatches ; i++) {

	  /*element = RamDB.GotoEntry(theMatchList.GetDBEntryNo(i));*/
	  if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	  printf("%s", RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
	  printf("\t%f",theMatchList.GetDistanceValue(i));
	  printf("\n");
	}
	
	theExtractionInterface->destroy() ;
	theSearchInterface->destroy() ;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionClient::Stop()
{
	fprintf(stderr,"CameraMotionClient stopped\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long CameraMotionClient::Close()
{
	/* free memories*/
    RamDB.FreeDatabase();

	if (DBDescription)
	{
		for (int i = 0; i < NoOfElements; i++)
	    {
	      if (DBDescription[i])
		  DBDescription[i]->release();
	    }
	  delete[] DBDescription;
	}
	
	NoOfElements =0;
	DBDescription =0;

	fprintf(stderr,"CameraMotionClient closed\n");
	return 0;
}
	
//----------------------------------------------------------------------------
const UUID& CameraMotionClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionClient::GetName(void)
{ return myName; }



