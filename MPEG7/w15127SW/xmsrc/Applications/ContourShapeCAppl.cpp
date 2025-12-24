///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric
// ITE-VIL, W.Price
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeCAppl.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
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

const UUID ContourShapeClient::myID = UUID("");
const char *ContourShapeClient::myName = "Contour-Based Shape Client";

//=============================================================================
ContourShapeClient::ContourShapeClient()
: RamDB()
, NoOfElements(0)
, DBContourShapeDescription(0)
{
}

//----------------------------------------------------------------------------
ContourShapeClient::~ContourShapeClient()
{
}

//----------------------------------------------------------------------------
unsigned long ContourShapeClient::Open(char *ListFile, char *Bitstream)
{
	int CodingMode;

        // Stream data base
        DecoderFileIO theStreamBuffer(0xFFFF);

        // Descriptor interface
        ContourShapeDescriptorInterfaceABC *theContourShapeDescriptorInterface;

        // Coding scheme
        ContourShapeCS *theContourShapeCS;

        // Coding scheme interface
        ContourShapeCSInterfaceABC *theContourShapeCSInterface;

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
	  theContourShapeCS = new ContourShapeCS;

          // Coding scheme interface
          theContourShapeCSInterface = theContourShapeCS->GetInterface();

          // Connect stream to coding scheme
          theContourShapeCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

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

        // Read filename database
        RamDB.CreateDatabase(ListFile);
        NoOfElements = RamDB.GetNoOfElements();

        // Create in memory decompressed database
        DBContourShapeDescription = new ContourShapeDescriptor*[NoOfElements];

        for (int i = 0; i < NoOfElements; i++)
        {
          // Allocate memory for descriptor
          DBContourShapeDescription[i] = new ContourShapeDescriptor();
          DBContourShapeDescription[i]->addref();

          // Get contour shape interface
          theContourShapeDescriptorInterface =
            DBContourShapeDescription[i]->GetInterface();

	  if (CodingMode == 2)
	  {
            // Connect descriptor to coding schemes
            theContourShapeCSInterface->
              SetDescriptorInterface(theContourShapeDescriptorInterface);

            // Decode contour shape
            theContourShapeCS->StartDecode();
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
              if (xsitype == "ContourShapeType")
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
            theContourShapeDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
#endif // USEXML
	  }
	}

	if (CodingMode == 2)
	{
          theContourShapeCSInterface->destroy();
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
unsigned long ContourShapeClient::Start(char *Query, int NoOfMatches)
{
        // image media
        MultiMedia *theMedia;
        MultiMediaInterfaceABC *theMediaInterface;
	// MultiMedia *media;

        // list with best matches
        MatchList theMatchList;

        // Descriptor interfaces
        ContourShapeDescriptorInterfaceABC *theContourShapeDescriptorInterface;
        ContourShapeDescriptorInterfaceABC *DBContourShapeDescriptorInterface;

        // extraction tool
        ContourShapeExtractionTool *theExtractionTool;
        ContourShapeExtractionInterfaceABC *theExtractionInterface;

        // search tool
        ContourShapeSearchTool *theSearchTool;
        ContourShapeSearchInterfaceABC *theSearchInterface;

	// Create media object 
	theMedia = new MultiMedia;

	// obtain media interface
	theMediaInterface=theMedia->GetInterface();

        // Get interfaces
        theExtractionTool = new ContourShapeExtractionTool(0);
        theExtractionInterface = theExtractionTool->GetInterface();

        theContourShapeDescriptorInterface =
          theExtractionInterface->GetDescriptorInterface();

        // Get contour shape search interface
        theSearchTool = new
          ContourShapeSearchTool(theContourShapeDescriptorInterface);
        theSearchInterface = theSearchTool->GetInterface();

        // Connect query descriptor to search tool
        theSearchInterface->
          SetQueryDescriptorInterface(theContourShapeDescriptorInterface);

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

        // prepare match list
        if (NoOfMatches > RamDB.GetNoOfElements())
          NoOfMatches = RamDB.GetNoOfElements();
        theMatchList.SetNumberOfMatches(NoOfMatches);

        // Allocate memory for media and load input data
        if (!RamDB.LoadNamedImage(Query)) return 0;

        // connect media to extraction tool
        theExtractionInterface->SetSourceMedia(theMediaInterface);

        // reset contour shape values
        theExtractionInterface->InitExtracting();

        // extract descriptor from media
        theExtractionInterface->StartExtracting();

        {
	int i;
        for (i = 0; i < NoOfElements; i++)
        {
          // get contour shape interface
          DBContourShapeDescriptorInterface =
            DBContourShapeDescription[i]->GetInterface();

          // connect reference descriptor to search tool
          theSearchInterface->
            SetRefDescriptorInterface(DBContourShapeDescriptorInterface);

          // sort result into list of best matches
          theMatchList.InsertMatch(i, theSearchInterface->GetDistance());
        }
	}

        {
	int j;
	for (j = 0; j < NoOfMatches; j++)
        {
	  char *a,*b;

          if (theMatchList.GetDBEntryNo(j) == (unsigned long)-1) break;
          a=RamDB.GotoEntry(theMatchList.GetDBEntryNo(j))->
	    GetFilename();
	  printf("%s",a);
          printf("\t%g",theMatchList.GetDistanceValue(j));
	  b=RamDB.TestAlphaFile(a,1);
	  if (b) {
	    printf("\t%s",a);
	    printf("\t%s",b);
	  }
          printf("\n");
        }
	}

        theExtractionInterface->destroy();
        theSearchInterface->destroy();

        return 0;
}

//----------------------------------------------------------------------------
unsigned long ContourShapeClient::Stop()
{
        return 0;
}

//----------------------------------------------------------------------------
unsigned long ContourShapeClient::Close()
{
        // free memories
        RamDB.FreeDatabase();
        if (DBContourShapeDescription)
        {
          for (int i = 0; i < NoOfElements; i++)
          {
            if (DBContourShapeDescription[i])
              DBContourShapeDescription[i]->release();
          }
          delete[] DBContourShapeDescription;
        }

        NoOfElements = 0;
        DBContourShapeDescription = 0;

        return 0;
}

        
//----------------------------------------------------------------------------
const UUID& ContourShapeClient::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *ContourShapeClient::GetName(void)
{
  return myName;
}



