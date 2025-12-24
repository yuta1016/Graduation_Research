///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric ITE-VIL
// P. Brasnett,
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignatureCAppl.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "SearchUtilities/SearchUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
//#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID VideoSignatureClient::myID = UUID("");
const char *VideoSignatureClient::myName = "Video Signature Client";

//=============================================================================

VideoSignatureMatchList::VideoSignatureMatchList(void):
MatchInfo(0)
{
}

VideoSignatureMatchList::VideoSignatureMatchList(int NoOfMatches ) : MatchList( NoOfMatches)
{

}

void VideoSignatureMatchList::SetNumberOfMatches(int theNoOfMatches)
{
  if (theNoOfMatches<=0) return;
  if (NoOfMatches) {
    if (DBEntryNumbers) delete [] DBEntryNumbers;
    if (SubNumbers) delete [] SubNumbers;
    if (DistanceValues) delete [] DistanceValues;
	if (MatchInfo) delete [] MatchInfo;
  }

  NoOfMatches=theNoOfMatches;
  DBEntryNumbers = new int[theNoOfMatches];
  SubNumbers = new int[theNoOfMatches];
  DistanceValues =  new double[theNoOfMatches];
  MatchInfo = new VIDEO_MATCH_INFO[theNoOfMatches];

  ResetMatchList(); 
}

void VideoSignatureMatchList::ResetMatchList()
{
  if (NoOfMatches<=0) return;	
  VIDEO_MATCH_INFO VidInfo={MATCH,0.0,0.0,0.0,0.0};
  for (int i=0; i<NoOfMatches; i++) {
    DBEntryNumbers[i]=-1;
    SubNumbers[i]=-1;
    DistanceValues[i]=DBL_MAX;
	MatchInfo[i]=VidInfo;
  }
}

VideoSignatureMatchList::~VideoSignatureMatchList()

{
  if (!NoOfMatches) return;

  if (MatchInfo) delete [] MatchInfo;
}

void VideoSignatureMatchList::InsertMatch(int aDBEntryNo, double theDistanceValue, VIDEO_MATCH_INFO VidInfo )
{
  int i,j;
  if (NoOfMatches<=0) return;

  for (i=0; i<NoOfMatches; i++) {
    if (DistanceValues[i]>theDistanceValue) break;
  }
  if (i>=NoOfMatches) return;
  j=NoOfMatches-1;
  while (j>i) {
    DBEntryNumbers[j]=DBEntryNumbers[j-1];
    DistanceValues[j]=DistanceValues[j-1];
    j--;
  }
  DBEntryNumbers[i]=aDBEntryNo;
  SubNumbers[i]=-1;
  DistanceValues[i]=theDistanceValue;
  MatchInfo[i]=VidInfo;
}

VIDEO_MATCH_INFO VideoSignatureMatchList::GetMatchInfo( int index )
{
	return MatchInfo[index];
}
//=============================================================================
VideoSignatureClient::VideoSignatureClient()
: RamDB()
, NoOfElements(0)
, DBVideoSignatureDescription(0)
{
}

//----------------------------------------------------------------------------
VideoSignatureClient::~VideoSignatureClient()
{
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureClient::Open(char *ListFile, char *Bitstream)
{
	int CodingMode;
	int KeyFrameMode;

        // Stream data base
        DecoderFileIO theStreamBuffer(0xFFFF);

        // Descriptor interface
        VideoSignatureDescriptorInterfaceABC *theVideoSignatureDescriptorInterface;

        // Coding scheme
        VideoSignatureCS *theVideoSignatureCS;

        // Coding scheme interface
        VideoSignatureCSInterfaceABC *theVideoSignatureCSInterface;

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
          // Coding scheme
	  theVideoSignatureCS = new VideoSignatureCS;

          // Coding scheme interface
          theVideoSignatureCSInterface = theVideoSignatureCS->GetInterface();

          // Connect stream to coding scheme
          theVideoSignatureCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

          // Open bit stream
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

	KeyFrameMode = atoi(getpara("KeyFrameInfo", "0"));

        // Read filename database
        RamDB.CreateDatabase(ListFile);
        NoOfElements = RamDB.GetNoOfElements();

        // Create in memory decompressed database
        DBVideoSignatureDescription = new VideoSignatureDescriptor*[NoOfElements];

        for (int i = 0; i < NoOfElements; i++)
        {
          // Allocate memory for descriptor
          DBVideoSignatureDescription[i] = new VideoSignatureDescriptor();
          DBVideoSignatureDescription[i]->addref();

          // Get video signature interface
          theVideoSignatureDescriptorInterface =
            DBVideoSignatureDescription[i]->GetInterface();

	  if (CodingMode == 2)
	  {
            // Connect descriptor to coding schemes
            theVideoSignatureCSInterface->
              SetDescriptorInterface(theVideoSignatureDescriptorInterface);


			theVideoSignatureCS->SetKeyFrameInfoFlag(KeyFrameMode);
            // Decode video signature
            theVideoSignatureCS->StartDecode();
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
              if (xsitype == "VideoSignatureType")
                break;
              theDDLDescription.GetNextSibling("Descriptor");
            }

            // VideoSignatureType not found
            if (theDDLDescription.isNull())
              return (unsigned long)-1;

            // Get the description interface
            GenericDSInterfaceABC *theDDLDescriptionInterface =
              theDDLDescription.GetInterface();

            // Import the description from the DDL tree
            theVideoSignatureDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
#endif // USEXML
	  }
	}

	if (CodingMode == 2)
	{
          theVideoSignatureCSInterface->destroy();
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
unsigned long VideoSignatureClient::Start(char *Query, int NoOfMatches)
{
	// image media
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;
	// MultiMedia *media;
	
	// list with best matches
	VideoSignatureMatchList theMatchList;
	
	// Descriptor interfaces
	VideoSignatureDescriptorInterfaceABC *theVideoSignatureDescriptorInterface;
	VideoSignatureDescriptorInterfaceABC *DBVideoSignatureDescriptorInterface;
	
	// extraction tool
	VideoSignatureExtractionTool *theExtractionTool;
	VideoSignatureExtractionInterfaceABC *theExtractionInterface;
	
	// search tool
	VideoSignatureSearchTool *theSearchTool;
	VideoSignatureSearchInterfaceABC *theSearchInterface;
	
	// Create media object 
	theMedia = new MultiMedia;
	
	// obtain media interface
	theMediaInterface=theMedia->GetInterface();
	
	// Get interfaces
	theExtractionTool = new VideoSignatureExtractionTool(0);
	theExtractionInterface = theExtractionTool->GetInterface();
	
	theVideoSignatureDescriptorInterface =
		theExtractionInterface->GetDescriptorInterface();
	
	// Get contour shape search interface
	theSearchTool = new
		VideoSignatureSearchTool(theVideoSignatureDescriptorInterface);
	theSearchInterface = theSearchTool->GetInterface();
	
	// Connect query descriptor to search tool
	theSearchInterface->
		SetQueryDescriptorInterface(theVideoSignatureDescriptorInterface);
	
	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);
	
	// prepare match list
	if (NoOfMatches > RamDB.GetNoOfElements())
		NoOfMatches = RamDB.GetNoOfElements();
	theMatchList.SetNumberOfMatches(NoOfMatches);
	
	
	if (RamDB.OpenNamedMPEGFile(Query)==0) return (unsigned long)-1;
	
	// link media to extraction tool
	theExtractionInterface->SetSourceMedia(theMediaInterface);
	
	theExtractionInterface->InitExtracting();
	
	int KeyFrameMode;
	KeyFrameMode = atoi(getpara("KeyFrameInfo", "0"));
	theVideoSignatureDescriptorInterface->SetKeyFrameInfoFlag(KeyFrameMode);
	
	// extract descriptor form media
	if(theExtractionInterface->StartExtracting()!=0)
	{
		printf("Extraction error: %s\n", theMediaInterface->GetVideoFileName());
		return (unsigned long)-1;;
	}
	
	{
		int i;

		if(strcmp(getpara("MODE", "0"),"PARTIAL")==0)
			VideoMatching::mode = PARTIAL;
		else
			VideoMatching::mode = DIRECT;

		i = atoi(getpara("DURATION", "0"));
		if(i> 0) 
			VideoMatching::duration = i;

		i = atoi(getpara("TH_JACCARD_SCORE", "0"));
		if(i> 0)
			VideoMatching::TH_JACCARD_SCORE = i * SEGMENT_WORD_COUNT;

		i = atoi(getpara("TH_L1_DIST", "0"));
		if(i> 0)
			VideoMatching::TH_L1_DIST = i;

		i = atoi(getpara("TH_L1_DIST_HOUGH", "0"));
		if(i> 0)
			VideoMatching::TH_L1_DIST_HOUGH = i;

		i = atoi(getpara("NUMBER_OF_USED_WORDS", "0"));
		if(i> 0)
			VideoMatching::NUMBER_OF_USED_WORDS = i;

		i = atoi(getpara("NUMBER_OF_MATCHED_FRAME", "0"));
		if(i> 0)
			VideoMatching::NUMBER_OF_MATCHED_FRAME = i;

		i = atoi(getpara("TH_FRAME_DISTANCE", "0"));
		if(i> 0)
			VideoMatching::TH_FRAME_DISTANCE = i;

		VideoMatching::setLUT_L1_DIST();


        for (i = 0; i < NoOfElements; i++)
        {
			// get contour shape interface
			DBVideoSignatureDescriptorInterface =
				DBVideoSignatureDescription[i]->GetInterface();
			
			// connect reference descriptor to search tool
			theSearchInterface->
				SetRefDescriptorInterface(DBVideoSignatureDescriptorInterface);
			
			// sort result into list of best matches
			VIDEO_MATCH_INFO videoMatchInfo;
			
			double distance = theSearchInterface->GetDistance(videoMatchInfo);
			
			theMatchList.InsertMatch(i, distance, videoMatchInfo );
        }

		VideoMatching::freeLUT_L1_DIST();
	}
	
	{
		int j;
		for (j = 0; j < NoOfMatches; j++)
        {
			char *a,*b;
			
			if (theMatchList.GetDBEntryNo(j) == (unsigned long)-1) break;
			a=theMediaInterface->GetVideoFileName();
			b=RamDB.GotoEntry(theMatchList.GetDBEntryNo(j))->
				GetFilename();
			printf("%s,%s",a,b);
			double distance=theMatchList.GetDistanceValue(j);
			if(distance==0.0f)
			{
				printf(",1");			
				VIDEO_MATCH_INFO videoMatchInfo=theMatchList.GetMatchInfo(j);
				printf(",%0.2f",videoMatchInfo.oStartTime);
				if(VideoMatching::mode==PARTIAL)
					printf(",%0.2f,%0.2f,%0.2f",videoMatchInfo.oEndTime,videoMatchInfo.qStartTime,videoMatchInfo.qEndTime);
			}
			else 
				printf(",0");
			printf("\n");

        }
	}
	
	theExtractionInterface->destroy();
	theSearchInterface->destroy();
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureClient::Stop()
{
        return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoSignatureClient::Close()
{
        // free memories
        RamDB.FreeDatabase();
        if (DBVideoSignatureDescription)
        {
          for (int i = 0; i < NoOfElements; i++)
          {
            if (DBVideoSignatureDescription[i])
              DBVideoSignatureDescription[i]->release();
          }
          delete[] DBVideoSignatureDescription;
        }

        NoOfElements = 0;
        DBVideoSignatureDescription = 0;

        return 0;
}

        
//----------------------------------------------------------------------------
const UUID& VideoSignatureClient::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *VideoSignatureClient::GetName(void)
{
  return myName;
}



