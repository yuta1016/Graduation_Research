///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - usage of parameter file to set search parameters
//   using theSearchInterface->SetValuesFromParameterFile() including default values
// - therefore removal of hard coded search parameter settings
// - bugfixes: DBLoadImage instead of LoadNamedImage
// - update according to LaBaule CD-text (coordinates, duration)
//
// Updated 02/21/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update, usage of MultiMedia class
//
// Updated 06/14/01:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - XML-IO
//
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
// Applicable File Name:  ParametricObjectMotionCAppl.cpp
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
#include "SearchUtilities/ParametricObjectMotionUtilities.h"

using namespace XM;

const UUID ParametricObjectMotionClient::myID = UUID("74abc540-db70-11d2-affe-0080c7e1e76d");
const char *ParametricObjectMotionClient::myName = "Parametric Object Motion Client";

//=============================================================================
ParametricObjectMotionClient::ParametricObjectMotionClient():
RamDB(),
NoOfElements(0),
DBDescription(0)

{
}

//----------------------------------------------------------------------------
ParametricObjectMotionClient::~ParametricObjectMotionClient()
{
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionClient::Open(char *ListFile, char *Bitstream)
{
	int CodingMode;

	/* stream data base*/
	DecoderFileIO theStreamBuffer(0xFFFF);

        /* descriptor and sub-descriptor*/

	ParametricObjectMotionDescriptorInterfaceABC
	  *theDescriptorInterface;

	/* coding schemes*/
	ParametricObjectMotionCS *theCS;
	ParametricObjectMotionCSInterfaceABC *theCSInterface;


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
	
	if (CodingMode == 2)
	{
	/* allocate memory for coding scheme*/
	theCS = new ParametricObjectMotionCS;

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
	NoOfElements=(int)(RamDB.GetNoOfElements()/2);

	/* create in table with references to descriptors in data base*/
	DBDescription =
	  new ParametricObjectMotionDescriptor* [NoOfElements];

	for (int i=0; i<NoOfElements; i++) {

	  /* allocate memory for descriptor*/
	  DBDescription[i] =
	    new ParametricObjectMotionDescriptor();
	  DBDescription[i]->addref();

	  theDescriptorInterface=
	    DBDescription[i]->GetInterface();

		if (CodingMode == 2)
		{
	  /* connect descriptors to coding schemes*/
	  theCSInterface->
	    SetDescriptorInterface(theDescriptorInterface);

	  /*decode descriptor*/
	  theCSInterface->StartDecode();
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
				if (xsitype == "ParametricMotionType")
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
unsigned long ParametricObjectMotionClient::Start(char *ListQuery,
						  int NoOfMatches)
{
/* image media*/							// Aljoscha 12/13/00, removed hard coded query options
											// now set in theSearchInterface->SetValuesFromParameterFile();
 											// including defaults
	MultiMedia *theMedia1;
	MultiMedia *theMedia2;
	MultiMedia *theMediaGeneric1;
	MultiMedia *theMediaGeneric2;

	MultiMediaInterfaceABC *theMediaInterface1;
	MultiMediaInterfaceABC *theMediaInterface2;
	MultiMediaInterfaceABC *theMediaGenericInterface1;
	MultiMediaInterfaceABC *theMediaGenericInterface2;

	MomVop *media1;
	MomVop *media2;
	MomVop *mediaGeneric1;
	MomVop *mediaGeneric2;

	MediaDB RamQuery;

	RamQuery.CreateDatabase(ListQuery);

	/* list with best matches*/
	MatchList theMatchList;

	/* stream data base*/
	//	EncoderFileIO theStreamBuffer(0xFFFF);

        /* descriptor and sub-descriptor*/
	ParametricObjectMotionDescriptorInterfaceABC
	  *theDescriptorInterface;

	ParametricObjectMotionDescriptorInterfaceABC
	  *DBDescriptorInterface;

	/* extraction tool*/
	ParametricObjectMotionExtractionTool *theExtractionTool;
	ParametricObjectMotionExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* search tool*/
	ParametricObjectMotionSearchTool *theSearchTool;
	ParametricObjectMotionSearchInterfaceABC *theSearchInterface;


	/*****************/
	/* get interfaces*/
	/* get color histogram extraction interface*/
	/* get color histogram extraction interface*/
	theExtractionTool = new 
	  ParametricObjectMotionExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtractionTool->GetInterface();

	/* get desctiptor interface*/
	theDescriptorInterface=
	  theExtractionInterface->
	  GetDescriptorInterface();

	/* get search interface*/
	theSearchTool = new 
	  ParametricObjectMotionSearchTool(theDescriptorInterface);
	theSearchInterface=theSearchTool->GetInterface();

	/* connect query descriptor to search tool*/
        theSearchInterface->
	  SetQueryDescriptorInterface(theDescriptorInterface);

	/* read predefined descriptor values from paramter file*/
	theDescriptorInterface->SetValuesFromParameterFile();

	/* read search options from paramter file*/
	theSearchInterface->SetValuesFromParameterFile();

	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}
	theMatchList.SetNumberOfMatches(NoOfMatches);

	// create media objects
	theMedia1 = new MultiMedia;
	theMedia2 = new MultiMedia;
	// obtain media interfaces
	theMediaInterface1 = theMedia1->GetInterface();
	theMediaInterface2 = theMedia2->GetInterface();

	RamQuery.SetMedia(theMediaInterface1);
	
	/* malloc media and load input data*/
	if (!RamQuery.DBLoadImage())
	{
		fprintf(stderr,"Error reading query data\n");
		exit(-1);
	}

	RamQuery.SetMedia(theMediaInterface2);
	
	if (!RamQuery.DBLoadImage())
	{
		fprintf(stderr,"Error reading query data\n");
		exit(-1);
	}


	media2=theMediaInterface2->GetImage();
	media1=theMediaInterface1->GetImage();

	// here add alpha if not available in Vop  structure//

		if( !GetMomVopA( media1 ) )
			{
				int ii;
				int width = (GetMomVopWidth(media1));
				int height = (GetMomVopHeight(media1));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media1));
				unsigned char *pixelval;

				media1->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)media1->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

	  if( !GetMomVopA( media2 ) )
			{
				int ii;
				int width = (GetMomVopWidth(media2));
				int height = (GetMomVopHeight(media2));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media2));
				unsigned char *pixelval;

				media2->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)media2->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

/*
	UChar *UCrefTest = (UChar *) GetMomImageData(GetMomVopA(media1));
	int width = (GetMomVopWidth(media1));
	int height = (GetMomVopHeight(media1));
	int ii,isAlpha=0;
	for (ii=0;ii<height*width;ii++)
	if (UCrefTest[ii]) {
	isAlpha=1;
	break;
	}
	UChar *UCcurrTest = (UChar *) GetMomImageData(GetMomVopA(media2));
	width = (GetMomVopWidth(media2));
	height = (GetMomVopHeight(media2));
	for (ii=0;ii<height*width;ii++)
	if (UCcurrTest[ii]) {
	isAlpha=1;
	break;
	}
	unsigned char *pixelval;
	
	if (!isAlpha){
	UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media1));
	width = (GetMomVopWidth(media1));
	height = (GetMomVopHeight(media1));
	for (ii=0,pixelval=(unsigned char *)media1->a_chan->f;ii<height*width;
	*(pixelval++)=(unsigned char)UCref[ii],ii++);

	UChar *UCcurr = (UChar *) GetMomImageData(GetMomVopY(media2));
	width = (GetMomVopWidth(media2));
	height = (GetMomVopHeight(media2));
	for (ii=0,pixelval=(unsigned char *)media2->a_chan->f;ii<height*width;
	*(pixelval++)=(unsigned char)UCcurr[ii],ii++);
	}
*/
	// end add alpha if not available in Vop structure//

#ifdef XYUVDISP
	/* display input image*/
	xyuv_show_simple(0,(unsigned short *)media1->y_chan->data->s,
			 (unsigned short *)media1->u_chan->data->s,
			 (unsigned short *)media1->v_chan->data->s,
			 datasize(media1->y_chan)&1,
			 media1->y_chan->x,media1->y_chan->y,
			 media1->u_chan->x,media1->u_chan->y,
			 "Query1");
	xyuv_show_simple(0,(unsigned short *)media2->y_chan->data->s,
			 (unsigned short *)media2->u_chan->data->s,
			 (unsigned short *)media2->v_chan->data->s,
			 datasize(media2->y_chan)&1,
			 media2->y_chan->x,media2->y_chan->y,
			 media2->u_chan->x,media2->u_chan->y,
			 "Query2");
#endif

	theExtractionInterface->SetSourceMedia(media2,media1);

// Aljoscha 12/13/00, default configuration
	theDescriptorInterface->SetCoordFlag(0);	// using embedded coordinates
	theDescriptorInterface->SetXOrigin(0.0);	// embedded coordinates
	theDescriptorInterface->SetYOrigin(0.0);	// embedded coordinates
	theDescriptorInterface->SetDuration(1);	  // time information

	theExtractionInterface->StartExtracting();

	double distance;

	int querymet=theSearchInterface->GetQueryMethod();
	int querynorm=theSearchInterface->GetQueryNorm();
	int squaredim=theSearchInterface->GetSquareDim();
	 
	{
	int i;
	for (i=0; i<NoOfElements; i++) {

		// create media objects
		theMediaGeneric1 = new MultiMedia;
		theMediaGeneric2 = new MultiMedia;
		// obtain media interfaces
		theMediaGenericInterface1 = theMediaGeneric1->GetInterface();
		theMediaGenericInterface2 = theMediaGeneric2->GetInterface();

		RamDB.SetMedia(theMediaGenericInterface1);
	
		/* malloc media and load input data*/
		if (!RamDB.DBLoadImage()) break;

		RamDB.SetMedia(theMediaGenericInterface2);
	
		if (!RamDB.DBLoadImage()) break;

		mediaGeneric2=theMediaGenericInterface2->GetImage();
		mediaGeneric1=theMediaGenericInterface1->GetImage();

		if( !GetMomVopA( mediaGeneric1 ) )
			{
				int ii;
				int width = (GetMomVopWidth(mediaGeneric1));
				int height = (GetMomVopHeight(mediaGeneric1));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(mediaGeneric1));
				unsigned char *pixelval;

				mediaGeneric1->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)mediaGeneric1->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

	  if( !GetMomVopA( mediaGeneric2 ) )
			{
				int ii;
				int width = (GetMomVopWidth(mediaGeneric2));
				int height = (GetMomVopHeight(mediaGeneric2));
				UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(mediaGeneric2));
				unsigned char *pixelval;

				mediaGeneric2->a_chan = initimg(width,height,UCHAR_TYPE);

	      for (ii=0,pixelval=(unsigned char *)mediaGeneric2->a_chan->f;ii<height*width;
		      *(pixelval++)=(unsigned char)UCref[ii],ii++);
			}

// add here alpha for mediaGeneric1 if not available in Vop structure
/*
	UChar *UCrefTestGen = (UChar *) GetMomImageData(GetMomVopA(mediaGeneric1));
	width = (GetMomVopWidth(mediaGeneric1));
	height = (GetMomVopHeight(mediaGeneric1));
	isAlpha=0;
	for (ii=0;ii<height*width;ii++)
	if (UCrefTestGen[ii]) {
	isAlpha=1;
	break;
	}
	if (!isAlpha){
	UChar *UCrefGen = (UChar *) GetMomImageData(GetMomVopY(mediaGeneric1));
	for (ii=0,pixelval=(unsigned char *)mediaGeneric1->a_chan->f;ii<height*width;
	*(pixelval++)=(unsigned char)UCrefGen[ii],ii++);
	}
*/
// end add here alpha for mediaGeneric1 if not available in Vop structure


	DBDescriptorInterface=DBDescription[i]->GetInterface();

	/* connect query descriptor to search tool*/
	theSearchInterface->
	  SetRefDescriptorInterface(DBDescriptorInterface);

	
	  /* sort result into list of best matches*/
	distance=(double)theSearchInterface->
	  GetDistance(mediaGeneric1,media1,querymet,querynorm,squaredim);
	theMatchList.InsertMatch(i,distance);

	theMediaGenericInterface1->destroy();
	theMediaGenericInterface2->destroy();
	}
	}

	/* free query media*/
	theMediaInterface1->destroy();
	theMediaInterface2->destroy();
	RamQuery.FreeDatabase();	
	{
	  int i;
	  for (i=0; i<NoOfMatches ; i++) {
	    if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	    printf("%s",
		   RamDB.GotoEntry(2*theMatchList.GetDBEntryNo(i))->
		   GetFilename());
	    printf("\t%g",theMatchList.GetDistanceValue(i));
	    printf("\n");
	  }
	}

	theExtractionInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionClient::Stop()
{
//	fprintf(stderr,"Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionClient::Close()
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
const UUID& ParametricObjectMotionClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionClient::GetName(void)
{ return myName; }



