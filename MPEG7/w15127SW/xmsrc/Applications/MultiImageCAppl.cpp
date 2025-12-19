///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of Munich
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
// Applicable File Name:  MultiImageCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID MultiImageClient::myID = UUID("");
const char *MultiImageClient::myName = "MultiImageClient";

//=============================================================================
MultiImageClient::MultiImageClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
MultiImageClient::~MultiImageClient()
{
  // Free memories
  RamDB.FreeDatabase();
  {
    int i;
    for (i=0; i<NoOfElements; i++) {
      if (DBDescription[i])
	DBDescription[i]->release();
    }
  }
  delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long MultiImageClient::Open(char *ListFile, char *Bitstream)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  MultiImageDescriptorInterfaceABC* theDescriptorInterface = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  MultiImageCS*                     theCS                  = NULL;
  MultiImageCSInterfaceABC*         theCSInterface         = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Allocate memory for the coding scheme
    theCS=new MultiImageCS();

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"MultiImageClient::Open: Allocation error\n");
    return(1);
  }

  // Connect the io buffer and open the bitstream
  theCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
  theStreamBuffer.OpenBitStream(Bitstream);

  // Create the database of filenames
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  // Read in each of the Descriptors
  try{
    // Make storage for all the pointers
    DBDescription = new MultiImageDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBDescription[ientry] = new MultiImageDescriptor(0,0);
      DBDescription[ientry]->addref();

      // Set up the interfaces to this D/DSType
      theDescriptorInterface=DBDescription[ientry]->GetInterface();
      theCSInterface->SetDescriptorInterface(theDescriptorInterface);

      // Now read this in
      theCS->StartDecode();
    }

  }
  catch(...) {
    fprintf(stderr,"MultiImageClient::Open: Allocation error2\n");
    return(1);
  }

  // Perform the tidying up
  theCSInterface->destroy();
  theStreamBuffer.CloseBitStream();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long MultiImageClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------
  // Media
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;

  // Create a list of all the best matches
  MatchList theMatchList;

  MultiImageDescriptorInterfaceABC* theDescriptorInterface = NULL;
  MultiImageDescriptorInterfaceABC* DBDescriptorInterface  = NULL;

  // Set up the extraction tool which will be needed for the query
  MultiImageExtractionTool*         theExtractionTool      = NULL;
  MultiImageExtractionInterfaceABC* theExtractionInterface = NULL;

  // Set up the search tool
  MultiImageSearchTool*         theSearchTool              = NULL;
  MultiImageSearchInterfaceABC* theSearchInterface         = NULL;

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

    // Create search tool
    theSearchTool = new MultiImageSearchTool(theDescriptorInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"MultiImageClient::Start: Allocation error1\n");
    return(1);
  }

  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  if (!RamDB.LoadNamedImage(Query)) {
    fprintf(stderr,"Warning: Query not found\n");
    return (unsigned long) -1; // break if end of database
  }

  // Tell the extractor that this is the source medium
  theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
  // Reinitialise the D Type; maybe also audio D can be extracted here
  theExtractionInterface->InitExtracting();

  // e.g., select foreground if region mask is available
  theMediaInterface->SelectVopAlphaLayer(ForegroundFromAlpha);

  // Extract D from media per frame of the input sequence
  theExtractionInterface->StartExtracting();

  // Collect partial results of the frame based extraction
  theExtractionInterface->PostExtracting();

  /// ------------------- Now do the matching -------------------------------


  for(int ientry=0;ientry<NoOfElements;ientry++) {

    // Get the interface of data base entry
    DBDescriptorInterface=DBDescription[ientry]->GetInterface();

    // Tell the search interface which we're matching to
    theSearchInterface->SetRefDescriptorInterface(DBDescriptorInterface);

    // Sort the result according to distance score
    theMatchList.InsertMatch(ientry,theSearchInterface->GetDistance());
  }

  // -------------------- Display the best matches --------------------------
  for(int i=0;i<NoOfMatches;i++) {

    char *a,*b;
    
    // Have we run out of entries?
    if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

    // Display the file and score
    printf("%s\t%f",a=RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
	   GetFilename(),
	   theMatchList.GetDistanceValue(i));

    // print file name to be displayed in GUI (optional)
    b=RamDB.TestAlphaFile(a,1);
    if (b) {
      printf("\t%s",a);
      printf("\t%s",b);
    }

    // print other informtion

    // finish printed line
    printf("\n");
    
  }

  // Finally tidy up
  theExtractionInterface->destroy();
  theSearchInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long MultiImageClient::Stop()
{
  // fprintf(stderr,"MultiImageClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long MultiImageClient::Close()
{
  // Free memories
  RamDB.FreeDatabase();
  {
    int i;
    for (i=0; i<NoOfElements; i++) {
      if (DBDescription[i])
	DBDescription[i]->release();
    }
  }
  delete [] DBDescription;

  NoOfElements =0;
  DBDescription =0;

  return 0;
}

	
//----------------------------------------------------------------------------
const UUID& MultiImageClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageClient::GetName(void)
{ return myName; }



