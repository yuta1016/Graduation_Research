///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of Technology
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiImageSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

using namespace XM;

const UUID MultiImageServer::myID = UUID("");
const char *MultiImageServer::myName = "MultiImageServer";

//=============================================================================
MultiImageServer::MultiImageServer()
{
}

//----------------------------------------------------------------------------
MultiImageServer::~MultiImageServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.



unsigned long MultiImageServer::Start(char *ListFile, char *Bitstream)
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
  EncoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  MultiImageDescriptorInterfaceABC* theDescriptorInterface = NULL;

  // This creates the extraction tool and extraction tool interface. Set
  // to null initially for safety.
  MultiImageExtractionTool*         theExtractionTool               = NULL;
  MultiImageExtractionInterfaceABC* theExtractionInterface          = NULL;
	
  // This sets up the coding scheme for this D/DType and the interface
  MultiImageCS*                     theCS                           = NULL;
  MultiImageCSInterfaceABC*         theCSInterface                  = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new descriptor
    theExtractionTool = new MultiImageExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Obtain the descriptor interface
    theDescriptorInterface=
      theExtractionInterface->GetDescriptorInterface();

    // Now create the coding scheme
    theCS = new MultiImageCS(theDescriptorInterface);

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"MultiImageServer::Start: Allocation error\n");
    return(1);
  }

  // All of the pointers should now be non-null so check them
  assert(theDescriptorInterface          != NULL);
  assert(theMedia                        != NULL);
  assert(theMediaInterface               != NULL);
  assert(theExtractionTool               != NULL);
  assert(theExtractionInterface          != NULL);
  assert(theCS                           != NULL);
  assert(theCSInterface                  != NULL);

  // Lastly, tell the dummy type about the output stream
  theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
  theStreamBuffer.StartBitStream(Bitstream);

  // ---------------------- Process the files -----------------------------
  
  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // Create a database of all the files to process
  RamDB.CreateDatabase(ListFile);

  // This example is for a video sequence
  // Loop over the files in the database
  int NDBaseEntries=RamDB.GetNoOfElements();
  for(int ientry=0;ientry<NDBaseEntries;ientry++) {

    if (!RamDB.DBLoadImage()) break; // break if end of sequence

    // Tell the extractor that this is the source medium
    theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
    // Reinitialise the D Type; maybe also audio D can be extracted here
    theExtractionInterface->InitExtracting();

    // e.g., select foregraound if region mask is available
    theMediaInterface->SelectVopAlphaLayer(ForegroundFromAlpha);

    // Extract D from media per frame of the input sequence
    theExtractionInterface->StartExtracting();

      // Collect partial results of the frame based extraction
    theExtractionInterface->PostExtracting();

    // Write the D/DSType to binary
    theCSInterface->StartEncode();
       
  } // Onto the next source file

  // Tidy up all the memory allocations
  theExtractionInterface->destroy();
  theCSInterface->destroy();

  // Shut down the output stream
  theStreamBuffer.CloseBitStream();
       
  // All done
  return(0);
}


//----------------------------------------------------------------------------
unsigned long MultiImageServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long MultiImageServer::Open()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long MultiImageServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& MultiImageServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *MultiImageServer::GetName(void)
{ 
  return(myName); 
}



