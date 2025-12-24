///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
// (contributing organizations names)
//
// Modified by
// Min-Seok Choi - Hanyang University, Korea
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RegionShapeCAppl.cpp
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

const UUID RegionShapeClient::myID = UUID("");
const char *RegionShapeClient::myName = "Region Shape Client";

//=============================================================================
RegionShapeClient::RegionShapeClient():
RamDB(),
NoOfElements(0),
DBRegionShapeDescription(0)
{
}

//----------------------------------------------------------------------------
RegionShapeClient::~RegionShapeClient()
{
}

//----------------------------------------------------------------------------
unsigned long RegionShapeClient::Open(char *ListFile, char *Bitstream)
{

	int CodingMode;


	/* stream data base*/
	DecoderFileIO theStreamBuffer(0xFFFF);

	/* Descriptor interface */
	RegionShapeDescriptorInterfaceABC *theRegionShapeDescriptorInterface;
	
	  /* Coding scheme interface */
	RegionShapeCSInterfaceABC *theRegionShapeCSInterface;

	// Select coding scheme type
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

	/* Coding scheme */
        {
	  RegionShapeCS *theRegionShapeCS = new RegionShapeCS;
	
	  theRegionShapeCSInterface = theRegionShapeCS->GetInterface();
	}

	if( CodingMode == 2 )
	{
		/* connect stream to coding scheme*/
		theRegionShapeCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
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

	/* create in memory decompressed database*/
	/* allocate memeories*/
	DBRegionShapeDescription =
	  new RegionShapeDescriptor *[NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  /* Allocate memory for descriptor*/
	  DBRegionShapeDescription[i] = new RegionShapeDescriptor();
	  DBRegionShapeDescription[i]->addref();

	  /* get descriptor interface*/
	  theRegionShapeDescriptorInterface=
	    DBRegionShapeDescription[i]->GetInterface();

	  if( CodingMode == 2 )
	  {
		  /* connect descriptor to coding schemes*/
		  theRegionShapeCSInterface->
			SetDescriptorInterface(theRegionShapeDescriptorInterface);


		  /*decode descriptor*/
		  theRegionShapeCSInterface->StartDecode();
	  }
	  else
	  {
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
              if (xsitype == "RegionShapeType")
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
		  theRegionShapeDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
#endif // USEXML
	  }

	}

	if( CodingMode == 2 )
	{
		theRegionShapeCSInterface->destroy();

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
unsigned long RegionShapeClient::Start(char *Query,int NoOfMatches)
{
        /* image media*/
        MultiMediaInterfaceABC *theMultiMediaInterface;

	/* list with best matches*/
	MatchList theMatchList;

	/* stream data base*/
	EncoderFileIO theStreamBuffer(0xFFFF);

        /* descriptor and sub-descriptor*/
	RegionShapeDescriptorInterfaceABC
	  *theRegionShapeDescriptorInterface;

	RegionShapeDescriptorInterfaceABC
	  *DBRegionShapeDescriptorInterface;

	/* extraction tool*/
	RegionShapeExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* search tool*/
	RegionShapeSearchTool *theSearchTool;
	RegionShapeSearchInterfaceABC *theSearchInterface;


	/* create extraction and descriptor*/
	{
	  RegionShapeExtractionTool *theExtractionTool;
	  theExtractionTool = new RegionShapeExtractionTool(0);
	  theExtractionInterface = theExtractionTool->GetInterface();
	}

	/* get descriptor interface*/
	theRegionShapeDescriptorInterface = 
		theExtractionInterface->GetDescriptorInterface();

	/* get search interface*/
	theSearchTool =
	  new RegionShapeSearchTool(theRegionShapeDescriptorInterface);
	theSearchInterface = theSearchTool->GetInterface();

	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	/* create MultiMedia object*/
	{
	  MultiMedia *theMultiMedia;
	  theMultiMedia=new MultiMedia;
	  theMultiMediaInterface=theMultiMedia->GetInterface();
	}
	RamDB.SetMedia(theMultiMediaInterface);

	/* malloc media and load input data*/
	if (!RamDB.LoadNamedImage(Query)) return 0;

#ifdef XYUVDISP
	{
	  MomVop *media;
	  media=theMultiMediaInterface->GetImage();
	  /* display input image*/
	  xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s,
			   (unsigned short *)media->u_chan->data->s,
			   (unsigned short *)media->v_chan->data->s,
			   datasize(media->y_chan)&1,
			   media->y_chan->x,media->y_chan->y,
			   media->u_chan->x,media->u_chan->y,
			   "Query");
	}
#endif

	/* connect media to extraction tool*/
	theExtractionInterface->SetSourceMedia(theMultiMediaInterface);

	/* extract descriptor from media*/
	theExtractionInterface->StartExtracting();

	{
	int i;
	for (i=0; i<NoOfElements; i++) {

	  /* get descriptor interface*/
	  DBRegionShapeDescriptorInterface=
		DBRegionShapeDescription[i]->GetInterface();

	  /* connect reference descriptor to search tool*/
	  theSearchInterface->
	    SetRefDescriptorInterface(DBRegionShapeDescriptorInterface);

	  /* sort result into list of best matches*/
	  theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	}
	}

	{
	int i;
	for (i=0; i<NoOfMatches ; i++) {
	  if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	  printf("%s",
		 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename());
	  printf("\t%e",theMatchList.GetDistanceValue(i));
	  printf("\n");
	}
	}

	theMultiMediaInterface->destroy();
	theExtractionInterface->destroy();
	theSearchInterface->destroy();
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionShapeClient::Stop()
{
	fprintf(stderr,"RegionShapeClient Stopped \n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long RegionShapeClient::Close()
{
	/* free memories*/
        RamDB.FreeDatabase();
	if (DBRegionShapeDescription) {
	  for (int i = 0; i < NoOfElements; i++)
	    {
	      if (DBRegionShapeDescription[i])
		DBRegionShapeDescription[i]->release();
	    }
	  delete[] DBRegionShapeDescription;
	}
	
	NoOfElements =0;
	DBRegionShapeDescription =0;

	fprintf(stderr, "RegionShapeClient Closed\n");
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& RegionShapeClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeClient::GetName(void)
{ return myName; }



