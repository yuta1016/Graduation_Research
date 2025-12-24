///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentSAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention
#include "CodingSchemes/CodingSchemes.h"          // Naming Convention
#include "Encoders/Encoders.h"               // Naming Convention
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID SpokenContentServer::myID = UUID("74abc540-db70-11d2-affe-0080c7e1e76d");
const char *SpokenContentServer::myName = "SpokenContent Type Server";

//=============================================================================
SpokenContentServer::SpokenContentServer()
{
}

//----------------------------------------------------------------------------
SpokenContentServer::~SpokenContentServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the DummyXML D/DSType
// created using the data. This is then written to the bitstream. 
//
// In this particular dummy, the files will be xml files. The detail 
// written out will be an integer giving the number of samples.
unsigned long SpokenContentServer::Start(char *ListFile, char *Bitstream)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Set up the GenericDS interface --------------------

  // This will be given a filename and it will read in and create the dom
  // we will then traverse the dom extracting the data we want to populate
  // our c++ code.
  GenericDSCS             *theCS          = new GenericDSCS;
  GenericDSCSInterfaceABC *theCSInterface = theCS->GetInterface();
  
  // -------------------- Initialise the output binary stream ---------------

  // This sets up a binary file io with a buffer size of 512bytes (I think).
  // This will be used to write out the binary form of our data
  EncoderFileIO theStreamBuffer(0x1000);

  // -------------------- Initialisation of data ----------------------------

  // This sets up the dummy type descriptor interface
  SpokenContentDescriptorInterfaceABC* theSpokenContentDescriptorInterface =NULL;

  // This creates the extraction tool and extraction tool interface. Set
  // to null initially for safety.
  SpokenContentExtractionTool*         theExtractionTool               = NULL;
  SpokenContentExtractionInterfaceABC* theExtractionInterface          = NULL;
	
  // This sets up the coding scheme for this binary output
  SpokenContentCS*                     theSpokenContentCS               = NULL;
  SpokenContentCSInterfaceABC*         theSpokenContentCSInterface      = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Pass it null so that it creates a new descriptor
    theExtractionTool = new SpokenContentExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();
 
    // Obtain the descriptor interface
    theSpokenContentDescriptorInterface=
                              theExtractionInterface->GetDescriptorInterface();

    // Now create the coding scheme
    theSpokenContentCS = new SpokenContentCS(theSpokenContentDescriptorInterface);

    // Obtain the interface
    theSpokenContentCSInterface=theSpokenContentCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"SpokenContentServer::Start: Allocation error\n");
    return(1);
  }

  // All of the pointers should now be non-null so check them
  assert(theSpokenContentDescriptorInterface != NULL);
  assert(theExtractionTool                != NULL);
  assert(theExtractionInterface           != NULL);
  assert(theSpokenContentCS                  != NULL);
  assert(theSpokenContentCSInterface         != NULL);

  // Lastly, tell the dummy type about the output stream
  theSpokenContentCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
  theStreamBuffer.StartBitStream(Bitstream);

  // ---------------------- Process the files -----------------------------
  
  // Create a database of all the files to process
  MediaDB RamDB;
  RamDB.CreateDatabase(ListFile);

  // Loop over the files in the database
  int NDBaseEntries=RamDB.GetNoOfElements();
  for(int ientry=0;ientry<NDBaseEntries;ientry++) {

#ifdef DEBUG
    printf("Reading DB file %d of %d\n", ientry+1, NDBaseEntries);
#endif

    // Empty out the D/DSType storage
    theSpokenContentCSInterface->GetDescriptorInterface()->Reset();

    // Get xml filename
    char* fname_ptr=RamDB.GotoEntry(ientry)->GetFilename();

    // Set up the GenericDSCS for this file
    theCSInterface->SetDecoderFileName(fname_ptr);

    // Make up a waste Generic DS to put all the DOM stuff into
    GenericDS theDocument = GenericDS::CreateDescriptionRoot();
    GenericDSInterfaceABC *theDocumentInterfaceABC=theDocument.GetInterface();

    // Tell the coding scheme to use the Generic DS
    theCSInterface->SetDescriptionInterface(theDocumentInterfaceABC);

    // Tell the CS to initilaise the decoding
    theCSInterface->StartDecode();

    // Let's skip past the header node - it should be a document. We want to
    // ensure that this document contains just one SpokenContent
    if(theDocument.GetNumberOfChildren() != 1) {
      fprintf(stderr,"SpokenContentServer::Start: Invalid XML document(1)\n");
      return(1);
    }
    GenericDS dummyxmlnode=theDocument.GetFirstChild();
    if(strcmp(dummyxmlnode.GetDSName().c_str(),"SpokenContent") != 0) {
      fprintf(stderr,"SpokenContentServer::Start: Invalid XML document(2)\n");
      return(1);
    }

    // Tell the extractor the correct node
    theExtractionInterface->SetSourceMedia(&dummyxmlnode);
	  
    // Reinitialise the D/DSType
    theExtractionInterface->InitExtracting();

    // Create the D/DSType from this medium
    theExtractionInterface->StartExtracting();

    // Write the D/DSType to binary
    theSpokenContentCSInterface->StartEncode();
  } // Onto the next source file

  // Tidy up all the memory allocations
  theExtractionInterface->destroy();
  theSpokenContentCSInterface->destroy();

  // Shut down the output stream
  theStreamBuffer.CloseBitStream();

  // All done
  return(0);
}


//----------------------------------------------------------------------------
// This isn't implemented here so it shouldn't be used.
unsigned long SpokenContentServer::Stop()
{
	return(0);
}


//----------------------------------------------------------------------------
// This isn't implemented here so it shouldn't be used.
unsigned long SpokenContentServer::Open()
{
	return(0);
}


//----------------------------------------------------------------------------
// This isn't implemented here so it shouldn't be used.
unsigned long SpokenContentServer::Close()
{
	return(0);
}

	
//----------------------------------------------------------------------------
const UUID& SpokenContentServer::GetObjectID(void)
{ 
        return(myID);
}

//----------------------------------------------------------------------------
const char *SpokenContentServer::GetName(void)
{ 
        return(myName); 
}



