//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivityCAppl.cpp
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
#include "Encoders/Encoders.h"

#include "ExtractionUtilities/motion_activity_categorization.h"
// SLMACategorization(MotionActivityDescriptorInformation *pInfo)

using namespace XM;

const UUID MotionActivityClient::myID = UUID("");
const char *MotionActivityClient::myName = "Motion Activity Client";

//=============================================================================
MotionActivityClient::MotionActivityClient():
RamDB(),
NoOfElements(0),
DBDescription(0)

{
}

//----------------------------------------------------------------------------
MotionActivityClient::~MotionActivityClient()
{
}

//----------------------------------------------------------------------------
unsigned long MotionActivityClient::Open(char *ListFile, char *Bitstream)
{
        /* stream data base*/
        DecoderFileIO theStreamBuffer(0x1000);

        /* descriptor and sub-descriptor*/
	MotionActivityDescriptorInterfaceABC
	  *theDescriptorInterface;

	  // This descriptors are used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface             = NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface           = NULL;
	GenericDS theDDLDescription;
	GenericDSInterfaceABC *theDDLDescriptionInterface       = NULL;

	int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
	GenericDSCS* theDDLCS  = NULL;
	GenericDSCSInterfaceABC* theDDLCSInterface = NULL;


	/* coding schemes*/
	MotionActivityCS *theCS;
	MotionActivityCSInterfaceABC *theCSInterface;
	 // For reading the xsi:type attributes
	string xsitype;

   //select coding scheme type
    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

	if(CodingMode == 2) 
	{
		/* allocate memory for coding scheme*/
		theCS = new MotionActivityCS;

		/* get coding scheme interfaces*/
		theCSInterface=theCS->GetInterface();
	}
	else
	{
			  // Create the XML document and obtain the interface
		  theDocument = GenericDS::CreateDescriptionRoot();
		  theDocumentInterface=theDocument.GetInterface();

		  // Now create the XML parser based coding scheme and obtain the interface
		  theDDLCS = new GenericDSCS(theDocumentInterface);
		  theDDLCSInterface=theDDLCS->GetInterface();

		  // Connect DDL file to coding scheme
		  theDDLCSInterface->SetDecoderFileName(Bitstream);

		  // Decode DDL file
		  theDDLCSInterface->StartDecode();

		  // Parse the top level elements  
		  // get the Mpeg7Root element
		  theMpeg7Root = theDocumentInterface->GetDescription("Mpeg7");

		  // Mpeg7Root not found
		  if (theMpeg7Root.isNull()) {
		fprintf(stderr, "Top level is not Mpeg7\n");
		return (unsigned long)-1;
		  }

		  // Get the Collection element for the descriptors
		  theCollection = theMpeg7Root.GetDescription("DescriptionUnit");

		  // Collection not found
		  if (theCollection.isNull()) {
		fprintf(stderr, "DescriptionUnit not found\n");
		return (unsigned long)-1;
		  }
		  theCollection.GetTextAttribute("xsi:type", xsitype);
		  if (xsitype != "DescriptorCollectionType") {
		fprintf(stderr, "DescriptorCollection not found\n");
		return (unsigned long)-1;
		  }

		  // Get Collection interface
		  theCollectionInterface=theCollection.GetInterface();

		  // initalize theDDLdescription
		  theDDLDescription=theCollection;
		  theDDLDescriptionInterface=theCollectionInterface;
    
		  // Now create the XML parser based coding scheme
		  theDDLCS = new GenericDSCS();

		  // Obtain the interface
		  theDDLCSInterface=theDDLCS->GetInterface();
	}

	if(CodingMode == 2)
	{ 
		/* connect stream to coding scheme*/
		theCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

		/* open bit stream*/
		theStreamBuffer.OpenBitStream(Bitstream);
	}

	/* read filename data base*/
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	/* create in memory decompressed database*/
	/* allocate memeories*/
	DBDescription =
	  new MotionActivityDescriptor* [NoOfElements];



	for (int i=0; i<NoOfElements; i++) {
	  /* allocate memory for descriptor*/
	  DBDescription[i] =
	    new MotionActivityDescriptor();
	  DBDescription[i]->addref();



	  /* get motion activity interface*/
	  theDescriptorInterface=
	    DBDescription[i]->GetInterface();

	  if(CodingMode == 2)
	  {
		  /* connect descriptors to coding schemes*/
		  theCSInterface->
			  SetDescriptorInterface(theDescriptorInterface);

		  /*decode motion activity*/
		  theCSInterface->StartDecode();
	  }
	  else
	  { // DDL or BiM coding

		  // get next descriptor
		  if (i==0)
		  {
			  theDDLDescription=theCollectionInterface->GetDescription("Descriptor");
		  }
		  else
		  {
			  theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
		  }

		  // search for correct xsi type
		  while (!theDDLDescription.isNull()) {
			  theDDLDescription.GetTextAttribute("xsi:type", xsitype);
			  // 2013/05/03 - fix - replace DummyType with "MotionActivity"
			  //if(xsitype=="DummyType") break;
			  if(xsitype=="MotionActivityType") break;
			  theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
		  }

		  // DummyType not found
		  if (theDDLDescription.isNull()) return (unsigned long)-1;

		  // Get DDLDescription interface
		  theDDLDescriptionInterface=theDDLDescription.GetInterface();

		  // Import description from DDL Tree
		  theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
      }

	  
  	  /** categorization procedure */
	  {
		  int a;
		  int division_block;
		  MotionActivityDescriptorInformation info;

		  /*
		  a=DBMotionActivityDescription[i].GetSpaLocNumber();
		  */
#ifndef SPALOCNUMBER
#define SPALOCNUMBER 1
#endif
		  a=SPALOCNUMBER;
		  info.spa_loc_number=a;
		  division_block=(int)(pow(4.0, (double)(a+1.0)));
		  for(int k=0;k<division_block;k++){
			  a=DBDescription[i]->GetSpatialLocalizedParameter(k);
			  info.spatial_localized_parameter[k]=a;
		  }
		  SLMACategorization(&info);
	  }
	  /* categorization procedure **/
	}
	if (CodingMode == 2) 
	{ // Binary coding
		theCSInterface->destroy();
		theStreamBuffer.CloseBitStream();
	}
	else
	{
		 theDDLCSInterface->destroy();
	}

	return 0;
}

//----------------------------------------------------------------------------
/*unsigned long MotionActivityClient::Start(char *Query,int startframe,
int endframe, int NoOfMatches)*/
unsigned long MotionActivityClient::Start(char *Query,int  NoOfMatches)
{
        int i;
	MatchList theMatchList;
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;

	/* descriptor and sub-descriptor*/
	MotionActivityDescriptorInterfaceABC
	  *theDescriptorInterface;

	MotionActivityDescriptorInterfaceABC
	  *DBDescriptorInterface;

        /* extraction tool*/
        MotionActivityExtractionTool *theExtraction;
        MotionActivityExtractionInterfaceABC 
	  *theExtractionInterface;

        /* search tool*/
        MotionActivitySearchTool *theSearch;
        MotionActivitySearchInterfaceABC *theSearchInterface;

	/*****************/
	/* get interfaces*/
	/* create media data class and get interface*/
	theMedia = new MultiMedia;
	theMediaInterface=theMedia->GetInterface();

	/* create  extraction tool*/
	/* get color histogram extraction interface*/
	theExtraction = new 
	  MotionActivityExtractionTool(0); /* create also descriptor*/
	theExtractionInterface=theExtraction->GetInterface();


	/* get motion activity interface*/
	theDescriptorInterface=
	  theExtractionInterface->GetDescriptorInterface();


	/* get motion activity search interface*/
	theSearch = new 
	  MotionActivitySearchTool(theDescriptorInterface);
	theSearchInterface=theSearch->GetInterface();

	/* connect media to extraction tool*/
	theExtractionInterface->SetSourceMedia(theMediaInterface);


	/* connect query descriptor to search tool*/
        theSearchInterface->
          SetQueryDescriptorInterface(theDescriptorInterface);

        /* set predifined descriptor values*/


	/* prepare matchlist*/
	if (NoOfMatches>RamDB.GetNoOfElements()) {
	  NoOfMatches=RamDB.GetNoOfElements();
	}

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);
	
	theMatchList.SetNumberOfMatches(NoOfMatches);

	// 2013/05/03 fixed - set source media after image is loaded, moved after RamDB.LoadNamedImage() below
	/* connect media to extarction tool*/
	//theExtractionInterface->SetSourceMedia(theMediaInterface);

	if(!RamDB.ExtractNamedMPEGMotionVectors(Query))
	  return 0;
//        if(!theExtractionInterface->PrioExtracting())
//	  return 0;
	i=0;
	while (1) {
	  char vectorimage[25]; /*filename of image containing motion vector*/

	  /* load input data*/
	  sprintf(vectorimage,"vectorfile.ppm[%d]",i); 
	  MomVop *vop = RamDB.LoadNamedImage(vectorimage);
	  if (!vop || vop->scene != i) break;
	                             /* break if no more images in sequence*/
	  
	  /* connect media to extarction tool*/
	  theExtractionInterface->SetSourceMedia(theMediaInterface); // fixed 2013/03/05

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

	  // Increment frame counter
	  i++;
	}       
//        theExtractionInterface->PostExtracting();

	for (i=0; i<NoOfElements; i++) {

	  /* get color space interface*/
	  DBDescriptorInterface=
	    DBDescription[i]->GetInterface();

	  /* connect query descriptor to search tool*/
	  theSearchInterface->
	    SetRefDescriptorInterface(DBDescriptorInterface);

	  /* sort result into list of best matches*/
	  theMatchList.InsertMatch(i,theSearchInterface->GetDistance());
	}


	fprintf(stderr,"#### Matching Result\n");

/*	ShotDBElement *element;
	for (i=0; i<NoOfMatches ; i++) {

	  element = RamDB.GotoEntry(theMatchList.GetDBEntryNo(i));
	  if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
	  printf("%s (%d %d) ", element->filename, element->startframeindex, element->endframeindex );

	  free( element );
	  fflush(stdout);
	  fprintf(stderr,"\t%e",theMatchList.GetDistanceValue(i));
	  printf("\n");
	}
*/
        for (i=0; i<NoOfMatches ; i++) {
          if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
          printf("%s",
                 RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
                 GetFilename());
          printf("\t%f",theMatchList.GetDistanceValue(i));
          printf("\n");

        }

	theMediaInterface->destroy();
	theExtractionInterface->destroy();
	theSearchInterface->destroy();

	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityClient::Stop()
{
        //fprintf(stderr,"Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long MotionActivityClient::Close()
{
	/* free memories*/
        RamDB.FreeDatabase();

		// 2013/03/05 fix - move the deletion to valid place
	//if (DBDescription) delete[] DBDescription;

	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {
	    if (DBDescription[i])
	      DBDescription[i]->release();
	  }
	}
	if (DBDescription) delete[] DBDescription; // fix 2013/03/05
	

	NoOfElements =0;
	DBDescription =0;


	return 0;
}

//----------------------------------------------------------------------------
const UUID& MotionActivityClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityClient::GetName(void)
{ return myName; }



