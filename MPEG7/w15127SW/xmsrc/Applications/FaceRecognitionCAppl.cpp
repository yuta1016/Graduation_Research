///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  FaceRecognitionCAppl.cpp
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
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Utilities/Utilities.h"
#include "Media/Media.h"

using namespace XM;

const UUID FaceRecognitionClient::myID = UUID("");
const char *FaceRecognitionClient::myName = "FaceRecognition Client";

//=============================================================================
FaceRecognitionClient::FaceRecognitionClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
FaceRecognitionClient::~FaceRecognitionClient()
{
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionClient::Open(char *ListFile, char *Bitstream)
{
	/* stream data base*/
	DecoderFileIO theStreamBuffer(0xFFFF);

	/* descriptor */
	FaceRecognitionDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* coding schemes*/
	FaceRecognitionCS *theCS;
	FaceRecognitionCSInterfaceABC *theCSInterface;

	// Select coding mode type
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
			theCS = new FaceRecognitionCS;
			/* get coding scheme interfaces*/
			theCSInterface = theCS->GetInterface();
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
			// Get the collection interface
			theCollectionInterface = theCollection.GetInterface();
			#endif // USEXML
			}

		/* read filename data base*/
		RamDB.CreateDatabase(ListFile);
		NoOfElements=RamDB.GetNoOfElements();
		/* create in table with references to descriptors in data base */
		DBDescription = new FaceRecognitionDescriptor* [NoOfElements];

		for (int i=0; i<NoOfElements; i++) {
			/* allocate memory for descriptor*/
			DBDescription[i] = new FaceRecognitionDescriptor();
			DBDescription[i]->addref();
			/* get FaceRecognition  interface*/
			theDescriptorInterface= DBDescription[i]->GetInterface();

			if (CodingMode == 2)
			{
				// connect descriptors to coding schemes
				theCSInterface->SetDescriptorInterface(theDescriptorInterface);
				//decode descriptor
				theCSInterface->StartDecode();
			} else {
				#ifdef USEXML
				// Get the next descriptor
				if (i == 0)
					theDDLDescription = theCollectionInterface->GetDescription("Descriptor");
				else
					theDDLDescription = theDDLDescription.GetNextSibling("Descriptor");
				// Search for the correct xsi type
				while (!theDDLDescription.isNull())
					{
					string xsitype;
					theDDLDescription.GetTextAttribute("xsi:type", xsitype);
					if (xsitype == "FaceRecognitionType")	break;
					theDDLDescription.GetNextSibling("Descriptor");
					}
				// FaceRecognitionType not found
				if(theDDLDescription.isNull()) return (unsigned long)-1;
				// Get the description interface
				GenericDSInterfaceABC *theDDLDescriptionInterface =	theDDLDescription.GetInterface();
				// Import the description from the DDL tree
				theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
				#endif // USEXML
			}
		}

		if (CodingMode == 2)
			{
			theCSInterface->destroy();
			theStreamBuffer.CloseBitStream();
		}	else	{
			#ifdef USEXML
			theDDLCSInterface->destroy();
			#endif // USEXML
		}

		return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionClient::Start(char *Query,int NoOfMatches)
{
        /* image media*/
        MomVop *media;

	/* list with best matches*/
	MatchList theMatchList;

	/* descriptor */
	FaceRecognitionDescriptorInterfaceABC
	  *theDescriptorInterface;

	FaceRecognitionDescriptorInterfaceABC
	  *DBDescriptorInterface;

	/* extraction tool*/
	FaceRecognitionExtractionTool *theExtractionTool;
	FaceRecognitionExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* search tool*/
	FaceRecognitionSearchTool *theSearchTool;
	FaceRecognitionSearchInterfaceABC *theSearchInterface;

	/*****************/
	/* get interfaces*/
	/* get FaceRecognition  extraction interface*/

	theExtractionTool = new 
	  FaceRecognitionExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();


	/* get desctiptor interface*/
	theDescriptorInterface=
	  theExtractionInterface->GetDescriptorInterface();


	/* get search interface*/
	theSearchTool = new 
	  FaceRecognitionSearchTool(theDescriptorInterface);
	theSearchInterface=theSearchTool->GetInterface();

	/* connect query descriptor to search tool*/
        theSearchInterface->
	  SetQueryDescriptorInterface(theDescriptorInterface);

	/* read predefined descriptor values from paramter file*/
	theDescriptorInterface->
	  SetValuesFromParameterFile();

	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	/* malloc media and load input data*/
	media=RamDB.LoadNamedImage(Query);
	if (!media) return 0;

#ifdef XYUVDISP
	/* display input image*/
	xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s,
			 (unsigned short *)media->u_chan->data->s,
			 (unsigned short *)media->v_chan->data->s,
			 datasize(media->y_chan)&1,
			 media->y_chan->x,media->y_chan->y,
			 media->u_chan->x,media->u_chan->y,
			 "Query");
#endif

	/* connect media to extarction tool*/
	theExtractionInterface->SetSourceMedia(media);

	/* reset extraction tool*/
	theExtractionInterface->InitExtracting();

	/* extract descriptor form media*/
	theExtractionInterface->StartExtracting();

	/* free media*/
	freevop(media);

	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {
	    DBDescriptorInterface=
	      DBDescription[i]->GetInterface();

	    /* connect reference descriptor to search tool*/
	    theSearchInterface->
	      SetRefDescriptorInterface(DBDescriptorInterface);

	    /* sort result into list of best matches*/
	    theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	  } /* for(i) */
	} /* free */

	{
	  int i;
	  for ( i=0; i<NoOfMatches ; i++) {
	    if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	    printf("%s",
		   RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		   GetFilename());
	    printf("\t%e",theMatchList.GetDistanceValue(i));
	    printf("\n");
	  }
	}

	theExtractionInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long FaceRecognitionClient::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long FaceRecognitionClient::Close()
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
const UUID& FaceRecognitionClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionClient::GetName(void)
{ return myName; }
