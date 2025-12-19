// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// Applicable File Name:  HomoTextureSAppl.cpp
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

using namespace XM;

const UUID HomogeneousTextureServer::myID = UUID("1cafefb0-6741-11d3-9d88-00609791d5bb");
const char *HomogeneousTextureServer::myName = "HomogeneousTextureServer";

//=============================================================================
HomogeneousTextureServer::HomogeneousTextureServer()
{
}

//----------------------------------------------------------------------------
HomogeneousTextureServer::~HomogeneousTextureServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the homogeneous texture D/DSType
// created using the data. This is then written to the bitstream/DDL file. 


unsigned long HomogeneousTextureServer::Start(char *ListFile, char *Bitstream)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------
  
  // Media data base
  MediaDB RamDB;

  // Media
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;
  
  // This sets up a binary file io with a buffer size of 512bytes (I think)
  //yjyu - 010213  EncoderFileIO theStreamBuffer(0x1000);
	EncoderFileIO theStreamBuffer(0xFFFF);	//yjyu - 010223

  // This sets up the homogeneous texture descriptor interface
  HomogeneousTextureDescriptorInterfaceABC* theDescriptorInterface = NULL;

  // This descriptors are used for DDL/BiM based coding
  GenericDS theDocument;
  GenericDSInterfaceABC *theDocumentInterface                      = NULL;
  GenericDS theMpeg7Root;
  GenericDS theCollection;
  GenericDSInterfaceABC *theCollectionInterface                    = NULL;
  
  // extraction tool and extraction tool interface. Set to null initially for safety.
  HomogeneousTextureExtractionTool*         theExtractionTool               = NULL;
  HomogeneousTextureExtractionInterfaceABC* theExtractionInterface          = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
  HomogeneousTextureCS*                     theBinCS                        = NULL;
  HomogeneousTextureCSInterfaceABC*         theBinCSInterface               = NULL;
  GenericDSCS*                     theDDLCS                        = NULL;
  GenericDSCSInterfaceABC*         theDDLCSInterface               = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new descriptor
    theExtractionTool = new HomogeneousTextureExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Obtain the descriptor interface
    theDescriptorInterface=
      theExtractionInterface->GetDescriptorInterface();

#ifdef USEXML
    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
	printf("CodingMode %d\n", CodingMode);
#else
    CodingMode=2; //Binary mode
#endif

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
      theBinCS = new HomogeneousTextureCS(theDescriptorInterface);

      // Obtain the interface
      theBinCSInterface=theBinCS->GetInterface();
    }
    else { // DDL or BiM coding
      // Create the XML document
      theDocument = GenericDS::CreateDescriptionRoot();
      theDocumentInterface=theDocument.GetInterface();

      // Create the Mpeg7Root element
      theMpeg7Root=theDocumentInterface->CreateChild("Mpeg7");
	  theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
	  theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
	  theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);

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
    fprintf(stderr,"HomogeneousTextureTypeServer::Start: Allocation error\n");
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

  // Lastly, tell the homogeneous texture type about the output stream
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
    theStreamBuffer.StartBitStream(Bitstream);
  }
  else { // DDL or BiM coding
    theDDLCSInterface->SetEncoderFileName(Bitstream);
  }
    
  // ---------------------------------------------------------------------------	
	/* connect media to extraction tool*/
	theExtractionInterface->SetSourceMedia(theMediaInterface);

	/* connect descriptor to extraction tool */
	theExtractionInterface->SetDescriptorInterface(theDescriptorInterface);	//yjyu - 010223

	RamDB.SetMedia(theMediaInterface);
	RamDB.CreateDatabase(ListFile);

	/* read predefined descriptor values from paramter file*/
//	theDescriptorInterface->
//	  SetValuesFromParameterFile();

	/* loop all input media files*/
	while (1) {

	  /* load input data*/
	  if (!RamDB.DBLoadImage()) break;

#ifdef XYUVDISP
	  /* display input image*/
	  xyuv_show_simple(0,(unsigned short *)
			   theMediaInterface->GetImage()->y_chan->data->s,
			   (unsigned short *)
			   theMediaInterface->GetImage()->u_chan->data->s,
			   (unsigned short *)
			   theMediaInterface->GetImage()->v_chan->data->s,
			   datasize(theMediaInterface->GetImage()->y_chan)&1,
			   theMediaInterface->GetImage()->y_chan->x,
			   theMediaInterface->GetImage()->y_chan->y,
			   theMediaInterface->GetImage()->u_chan->x,
			   theMediaInterface->GetImage()->u_chan->y,
			   "input");
#endif

	  /* reset histogram values*/
	  theExtractionInterface->InitExtracting();

	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();
  
	  theExtractionInterface->PostExtracting();

	  if (CodingMode == 2) { // Binary coding
	    // Write the D/DSType to binary
	    theBinCSInterface->StartEncode();
	  }
	  else { // DDL or BiM coding

	    // Export description to Collection DDL Tree
	    theDescriptorInterface->ExportDDL(theCollectionInterface);
	  }

	}

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

//  theMediaInterface->destroy();//sth why commented? memory leakage!!
  return(0);

}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long HomogeneousTextureServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long HomogeneousTextureServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& HomogeneousTextureServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *HomogeneousTextureServer::GetName(void)
{ 
  return(myName); 
}




