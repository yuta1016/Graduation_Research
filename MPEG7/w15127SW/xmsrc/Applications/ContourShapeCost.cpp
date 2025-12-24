///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric ITE-VIL
// W.Price
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeCost.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention
#include "SearchUtilities/SearchUtilities.h"        // Naming Convention
#include "CodingSchemes/CodingSchemes.h"          // Naming Convention
#include "Encoders/Encoders.h"               // Naming Convention
#include "Media/Media.h"                  // Naming Convention
#include "Utilities/Utilities.h"              // Naming Convention

using namespace XM;

const UUID ContourShapeCost::myID = UUID("");
const char *ContourShapeCost::myName = "Contour-Based Shape Cost";

//=============================================================================
ContourShapeCost::ContourShapeCost()
: RamDB()
, NoOfElements(0)
, DBContourShapeDescription(0)
{
}

//----------------------------------------------------------------------------
ContourShapeCost::~ContourShapeCost()
{
}

//----------------------------------------------------------------------------
unsigned long ContourShapeCost::Open(char *ListFile, char *Bitstream)
{
	int CodingMode;

        // Stream data base
        DecoderFileIO theStreamBuffer(0xFFFF);

        // Descriptor and sub-descriptors
        ContourShapeDescriptorInterfaceABC *theContourShapeDescriptorInterface;

        // Coding scheme
        ContourShapeCS *theContourShapeCS;

        // Coding scheme interface
        ContourShapeCSInterfaceABC *theContourShapeCSInterface;

	// Select coding mode
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
          theContourShapeCS = new ContourShapeCS();

          // Coding scheme interface
          theContourShapeCSInterface = theContourShapeCS->GetInterface();

          // Connect to buffer
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
          // Allocate memory
          DBContourShapeDescription[i] = new ContourShapeDescriptor();
          DBContourShapeDescription[i]->addref();

          // Get contour shape interface
          theContourShapeDescriptorInterface =
            DBContourShapeDescription[i]->GetInterface();

	  if (CodingMode == 2)
	  {
	    // Connect descriptors to coding schemes
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
unsigned long ContourShapeCost::Start(int NoOfMatches)
{
        // list with best matches
        MatchList theMatchList;

        // stream data base
        EncoderFileIO theStreamBuffer(0xFFFF);

        // descriptor and sub-descriptor
        ContourShapeDescriptorInterfaceABC *queryContourShapeDescriptorInterface;
        ContourShapeDescriptorInterfaceABC *DBContourShapeDescriptorInterface;

        // search tool
        ContourShapeSearchTool *theSearchTool = new ContourShapeSearchTool();

        ContourShapeSearchInterfaceABC *theSearchInterface =
          theSearchTool->GetInterface();

        // prepare match list
        if (NoOfMatches > RamDB.GetNoOfElements())
          NoOfMatches = RamDB.GetNoOfElements();
        theMatchList.SetNumberOfMatches(NoOfMatches);

        for (int i = 0; i < NoOfElements; i++)
        {
          theMatchList.ResetMatchList();

          // get contour shape interface
          queryContourShapeDescriptorInterface =
              DBContourShapeDescription[i]->GetInterface();

          // connect query descriptor to search tool
          theSearchInterface->
            SetQueryDescriptorInterface(queryContourShapeDescriptorInterface);

          char *cName = new char[strlen(RamDB.GotoEntry(i)->GetFilename())+1];
          strcpy(cName, RamDB.GotoEntry(i)->GetFilename());
          char *pDash = strrchr(cName, '-');
          if (pDash) *(pDash+1) = '\0';

          int nClass = 0;
          for (int j = 0; j < NoOfElements; j++)
          {
            // get contour shape interface
            DBContourShapeDescriptorInterface =
              DBContourShapeDescription[j]->GetInterface();

            // connect query descriptor to search tool
            theSearchInterface->
              SetRefDescriptorInterface(DBContourShapeDescriptorInterface);

            // sort result into list of best matches
            theMatchList.InsertMatch(j, theSearchInterface->GetDistance());

            if (strncmp(cName, RamDB.GotoEntry(j)->GetFilename(), strlen(cName)) == 0)
              nClass++;
          }

          int nCost = Count(theMatchList, cName);
          printf("%s\t%d/%d\n", RamDB.GotoEntry(i)->GetFilename(), nCost, nClass);

          fflush(stdout);
          delete[] cName;
        }

        theSearchInterface->destroy();

        return 0;
}

//----------------------------------------------------------------------------
unsigned long ContourShapeCost::Stop()
{
        return 0;
}
//----------------------------------------------------------------------------
unsigned long ContourShapeCost::Close()
{
        // free memory
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
//sth unkown interface
int ContourShapeCost::Count(MatchList &matchList, const char *className)
{
  int NoOfMatches = matchList.GetNumberOfMatches();
  int count = 0;

  for (int i = 0; i < NoOfMatches; i++)
  {
    if (matchList.GetDBEntryNo(i) == (unsigned long)-1) break;
    if (strncmp(className, RamDB.GotoEntry(matchList.GetDBEntryNo(i))->GetFilename(), strlen(className)) == 0)
      count++;
  }

  return count;
}

//----------------------------------------------------------------------------
const UUID& ContourShapeCost::GetObjectID(void)
{
  return myID;
}

//----------------------------------------------------------------------------
const char *ContourShapeCost::GetName(void)
{
  return myName;
}



