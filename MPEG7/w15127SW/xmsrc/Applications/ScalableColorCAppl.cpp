///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich LIS, S. Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
// Applicable File Name:  ScalableColorCAppl.cpp
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
#include "Utilities/Utilities.h"
#include "SearchUtilities/MatchList.h"
FILE *fin1;

using namespace XM;

const UUID ScalableColorClient::myID = UUID("");
const char *ScalableColorClient::myName = "ScalableColorClient";

//=============================================================================
ScalableColorClient::ScalableColorClient():
RamDB(),
NoOfElements(0),
DBDescription(0)/*,
DBColorSpaceDescription(0),
DBColorQuantizerDescription(0),
DBHistogramDescription(0)*/

{
}

//----------------------------------------------------------------------------
ScalableColorClient::~ScalableColorClient()
{
}

//----------------------------------------------------------------------------
unsigned long ScalableColorClient::Open(char *ListFile, char *Bitstream)
{
	/* stream data base*/
	DecoderFileIO theStreamBuffer(0x1000);

        /* descriptor interface*/
	ScalableColorDescriptorInterfaceABC *theDescriptorInterface;

	/* coding schemes*/
	ScalableColorCS *theCS;
	ScalableColorCSInterfaceABC *theCSInterface;

	// Select coding scheme type
	int CodingMode;
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "2"));
#else
	CodingMode = 2;
#endif // USEXML
	
#ifdef USEXML
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	GenericDS theMpeg7Root, theCollection;
	GenericDSInterfaceABC *theCollectionInterface;
	GenericDS theDDLDescription;
#endif // USEXML

	if (CodingMode == 2)
	{
	  /* allocate memory for coding scheme*/
	  theCS = new ScalableColorCS;

	  /* get coding scheme interfaces*/
	  theCSInterface=theCS->GetInterface();

	  /* connect stream to coding scheme*/
	  theCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

	  /* open bit stream*/
	  theStreamBuffer.OpenBitStream(Bitstream);
	}
	else
	{
#ifdef USEXML
		// Create the XML document and obtain the interface
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();		
		// Create the XML parser based coding scheme and obtain the interface
		theDDLCS = new GenericDSCS(theDocumentInterface);
		theDDLCSInterface = theDDLCS->GetInterface();		
		// Connect the DDL to the coding scheme
		theDDLCSInterface->SetDecoderFileName(Bitstream);		
		// Decode the DDL file
		theDDLCSInterface->StartDecode();		
		// Parse the top level elements
		theMpeg7Root = theDocumentInterface->GetDescription("Mpeg7");		
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
		if (xsitype != "DescriptorCollectionType")
		{
			fprintf(stderr, "DescriptorCollection not found\n");
			return (unsigned long)-1;
		}		
		// Get the collection in terface
		theCollectionInterface = theCollection.GetInterface();
#endif // USEXML
	}

	/* read filename data base*/
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* create in table with references to descriptors in data base*/
	DBDescription =
	  new ScalableColorDescriptor* [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  
	  /* allocate memory for descriptor*/
	  DBDescription[i] =
	    new ScalableColorDescriptor();
	  DBDescription[i]->addref();

	  /* get color histogram interface*/
	  theDescriptorInterface=
	    DBDescription[i]->GetInterface();

		if (CodingMode == 2)
		{
  	  /* connect descriptors to coding schemes*/
	    theCSInterface->SetDescriptorInterface(theDescriptorInterface);

	    /*decode descriptor*/
	    theCSInterface->StartDecode();
		}
				else
		{
#ifdef USEXML
			// Get the next descriptor
			if (i == 0)
				theDDLDescription = 
				  theCollectionInterface->GetDescription("Descriptor");
			else
				theDDLDescription = theDDLDescription.GetNextSibling("Descriptor");			
			// Search for the correct xsi type
			while (!theDDLDescription.isNull())
			{
				string xsitype;
				theDDLDescription.GetTextAttribute("xsi:type", xsitype);
				if (xsitype == "ScalableColorType")
					break;
				theDDLDescription.GetNextSibling("Descriptor");
			}			
			// ContourShapeType not found
			if (theDDLDescription.isNull())
				return (unsigned long)-1;			
			// Get the description interface
			GenericDSInterfaceABC *theDDLDescriptionInterface =	
				theDDLDescription.GetInterface();			
			// Import the description from the DDL tree
			theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
#endif // USEXML
		}
	}

	if (CodingMode == 2)
	{
		theCSInterface->destroy();

		theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML
		theDDLCSInterface->destroy();
#endif // USEXML
	}

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorClient::Start(char *Query,int NoOfMatches)
{
  // printf("in Start\n");
	/* image media*/
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;

	/* list with best matches*/
	MatchList theMatchList;

        /* descriptor*/
	ScalableColorDescriptorInterfaceABC
	  *theDescriptorInterface;

	ScalableColorDescriptorInterfaceABC
	  *DBDescriptorInterface;

	/* extraction tool */
	ScalableColorExtractionTool *theExtractionTool;
	ScalableColorExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* search tool*/
	ScalableColorSearchTool *theSearchTool;
	ScalableColorSearchInterfaceABC *theSearchInterface;


	/*****************/
	/* get interfaces*/
	/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	/* create  extraction tool*/
	/* get color histogram extraction interface*/
	theExtractionTool = new 
	  ScalableColorExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get desctiptor interface*/
	theDescriptorInterface=
	  theExtractionInterface->
	  GetDescriptorInterface();

	/* get search interface*/
	theSearchTool = new 
	  ScalableColorSearchTool(theDescriptorInterface);
	theSearchInterface=theSearchTool->GetInterface();

	/* connect media to extarction tool*/
	theExtractionInterface->SetSourceMedia(theMediaInterface);

	/* connect query descriptor to search tool*/
        theSearchInterface->
	  SetQueryDescriptorInterface(theDescriptorInterface);

	/* read predefined descriptor values from paramter file*/

	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	/* load input data*/
	RamDB.SetMedia(theMediaInterface);
	if (!RamDB.LoadNamedImage(Query)) return 0;

	/* select VOP layer*/
	if (theMediaInterface->GetImage()->a_chan) {
	  theMediaInterface->SelectVopAlphaLayer(ForegroundFromAlpha);
	}
	else {
	  theMediaInterface->SelectVopAlphaLayer(BackgroundFromAlpha);
	}

#ifdef XYUVDISP
	{
	  /* display input image*/
	  MomVop *media=theMediaInterface->GetImage();
	  xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s,
			   (unsigned short *)media->u_chan->data->s,
			   (unsigned short *)media->v_chan->data->s,
			   datasize(media->y_chan)&1,
			   media->y_chan->x,media->y_chan->y,
			   media->u_chan->x,media->u_chan->y,
			   "Query");
	}
#endif

	/* reset histogram values*/
	theExtractionInterface->InitExtracting();

	/* extract descriptor form media*/
	theExtractionInterface->StartExtracting();

	/* free media*/
	theMediaInterface->FreeImage();

	{
	int i;
	//NoOfElements=10;
	//NoOfMatches=10;
	for (i=0; i<NoOfElements; i++) {


	  DBDescriptorInterface=
	    DBDescription[i]->GetInterface();

	  /* connect reference descriptor to search tool*/
	  theSearchInterface->
	    SetRefDescriptorInterface(DBDescriptorInterface);

	  /* sort result into list of best matches*/
	  theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	}
	}


	{
	int i;
	fin1=fopen("sort_list_of_best_matches.txt","w");  //unsere aend/makai
	//sth nice private extension, but usually shells allow output redirection!!
	//if(fin1==NULL) printf("sort_list_of_best_matches.txt kann nicht er. werden\n");//sth violation of primary output interface
	//printf("NoOfMatches:%5d\n",NoOfMatches);//sth
	fprintf(fin1,"%d  200  0\n",NoOfElements);


	for (i=0; i<NoOfMatches ; i++) {
		 /*printf("%s",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());
	  printf("\t%5.1f\n",theMatchList.GetDistanceValue(i));*/

	  if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	  //sth keep the required output interface//sth added again
	  printf("%s",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());
	  printf("\t%f",theMatchList.GetDistanceValue(i));
	  printf("\n");
	  fflush(stdout);

/********** unsere aenderungen/makai                                  */
          {

        int iwert=0;
         double wert;

          wert=theMatchList.GetDistanceValue(i);
          iwert=(int)((double)1000*wert);

         /*printf("%d    0",iwert);
	  printf("    %s\n",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());*/

          fprintf(fin1,"%d 0",iwert);

	  fprintf(fin1," %s\n",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());
          }

/****** ende unserer  aenderung/makai ************************/

	}
	}
	
	theMediaInterface->destroy();
	theExtractionInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ScalableColorClient::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long ScalableColorClient::Close()
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
const UUID& ScalableColorClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorClient::GetName(void)
{ return myName; }



