///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShapeSAppl.cpp
//

#include <stdio.h>
#include <stdlib.h> 

#ifdef _VISUALC_
#include <assert.h> 
#include <direct.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"          /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

extern const char *MPEG7_AMD2_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_AMD2_SCHEMA_LOCATION;

using namespace XM;

const UUID Perceptual3DShapeServer::myID = UUID("");
const char *Perceptual3DShapeServer::myName = "Perceptual3DShapeServer";

//=============================================================================
Perceptual3DShapeServer::Perceptual3DShapeServer()
{
}

//----------------------------------------------------------------------------
Perceptual3DShapeServer::~Perceptual3DShapeServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the Dummy D/DSType
// created using the data. This is then written to the bitstream/DDL file. 

#define TRUE 1
#define FALSE 0
#include "Descriptors/Perceptual3DShape_Aux.h"

unsigned long Perceptual3DShapeServer::Start(char *ListFile, char *Bitstream)
{
//global member initialization.
	CSplit::MakeSE();

  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------
  
  // Media data base
  MediaDB RamDB;
  MediaThreeD *media1=NULL;

  // Media
  MultiMedia *theMedia=NULL;
  MultiMediaInterfaceABC *theMediaInterface = NULL;
  
  // This sets up a binary file io with a buffer size of 512bytes (I think)
  EncoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  Perceptual3DShapeDescriptorInterfaceABC* theDescriptorInterface          = NULL;

  // This descriptors are used for DDL/BiM based coding
  GenericDS theDocument;
  GenericDSInterfaceABC *theDocumentInterface                      = NULL;
  GenericDS theMpeg7Root;
  GenericDS theCollection;
  GenericDSInterfaceABC *theCollectionInterface                    = NULL;

  // This creates the extraction tool and extraction tool interface. Set
  // to null initially for safety.
  Perceptual3DShapeExtractionTool*         theExtractionTool               = NULL;
  Perceptual3DShapeExtractionInterfaceABC* theExtractionInterface          = NULL;
	
  // This sets up the coding scheme for this D/DType and the interface
  int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
  Perceptual3DShapeCS*                     theBinCS                        = NULL;
  Perceptual3DShapeCSInterfaceABC*         theBinCSInterface               = NULL;
  GenericDSCS*                     theDDLCS                        = NULL;
  GenericDSCSInterfaceABC*         theDDLCSInterface               = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new descriptor
    theExtractionTool = new Perceptual3DShapeExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Obtain the descriptor interface
    theDescriptorInterface=
      theExtractionInterface->GetDescriptorInterface();

    //select coding scheme type
#ifdef USEXML
    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
#else
    CodingMode=2; //Binary mode
#endif

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
      theBinCS = new Perceptual3DShapeCS(theDescriptorInterface);

      // Obtain the interface
      theBinCSInterface=theBinCS->GetInterface();
    }
    else { // DDL or BiM coding
      // Create the XML document
      theDocument = GenericDS::CreateDescriptionRoot();
      theDocumentInterface=theDocument.GetInterface();

      // Create the Mpeg7Root element
      theMpeg7Root=theDocumentInterface->CreateChild("Mpeg7");
	  theMpeg7Root.SetAttribute("xmlns", MPEG7_AMD2_NS);
	  theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
	  theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_AMD2_SCHEMA_LOCATION);

      // Create the Collection element for the descriptors
      theCollection=theMpeg7Root.CreateChild("DescriptionUnit");
      theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
      theCollectionInterface=theCollection.GetInterface();

      // Now create the XML parser based coding scheme
      theDDLCS = new GenericDSCS(theDocumentInterface);

      // Obtain the interface
      theDDLCSInterface=theDDLCS->GetInterface();

      // Connect DDL file to coding scheme
      theDDLCSInterface->SetDecoderFileName(Bitstream);
    }
  }
  catch(...) {
    fprintf(stderr,"Perceptual3DShapeServer::Start: Allocation error\n");
    return(1);
  }

  // All of the pointers should now be non-null so check them
  assert(theDescriptorInterface          != NULL);
  assert(theMedia                        != NULL);
  assert(theMediaInterface               != NULL);
  assert(theExtractionTool               != NULL);
  assert(theExtractionInterface          != NULL);
  if (CodingMode == 2) { // Binary coding
    assert(theBinCS                      != NULL);
    assert(theBinCSInterface             != NULL);
  }
  else { // // DDL or BiM coding
    assert(theDocumentInterface          != NULL);
    assert(theCollectionInterface        != NULL);
    assert(theDDLCS                      != NULL);
    assert(theDDLCSInterface             != NULL);
  }

  // Lastly, tell the dummy type about the output stream
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
    theStreamBuffer.StartBitStream(Bitstream);
  }
  else { // DDL or BiM coding
    theDDLCSInterface->SetEncoderFileName(Bitstream);
  }

  // ---------------------- Process the files -----------------------------
  
  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // Create a database of all the files to process
  RamDB.CreateDatabase(ListFile);

  // This example is for a video sequence
  // Loop over the files in the database
  int NDBaseEntries=RamDB.GetNoOfElements();
  for(int ientry=0;ientry<NDBaseEntries;ientry++) {
    printf("%d\n", ientry);

    // Names for elementary parts of the media sequence
    int i=0;              // part no of sequence
//    char ppmimage[25];    // filename of single image (part of sequence)
//    char path[MAXMEDIANAMELEN];

    media1 = RamDB.DBLoad3DMesh(); 
    if (!media1) break; 
   
    // Tell the extractor that this is the source medium
    theExtractionInterface->SetSourceMedia(media1);
	  
    theExtractionInterface->InitExtracting();
    theExtractionInterface->StartExtracting(); 

    if (CodingMode == 2) { // Binary coding
      // Write the D/DSType to binary
      theBinCSInterface->StartEncode();
    }
    else { // DDL or BiM coding

      // Export description to Collection DDL Tree
      theDescriptorInterface->ExportDDL(theCollectionInterface);
    }
    free_media3D(media1);//zhanghui new added:
  } // Onto the next source file

  // Tidy up all the memory allocations
  theExtractionInterface->destroy();
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->destroy();

    // Shut down the output stream
    theStreamBuffer.CloseBitStream();
  }
  else {
    // Write the D/DSType to DDL
    theDDLCSInterface->StartEncode();

    theDDLCSInterface->destroy();
  }

//zhanghui added:
  theMediaInterface->destroy();

  // All done
  return(0);
}


//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& Perceptual3DShapeServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *Perceptual3DShapeServer::GetName(void)
{ 
  return(myName); 
}



