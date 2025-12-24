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
// Applicable File Name:  SpokenContentCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention
#include "CodingSchemes/CodingSchemes.h"          // Naming Convention
#include "Decoders/Decoders.h"               // Naming Convention
#include "SearchUtilities/SearchUtilities.h"        // Naming Convention
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID SpokenContentClient::myID = UUID("");
const char *SpokenContentClient::myName = "SpokenContent Type Client";

//=============================================================================
SpokenContentClient::SpokenContentClient():
RamDB(),
NoOfElements(0),
DBSpokenContentDescription(0)
{
}

//----------------------------------------------------------------------------
SpokenContentClient::~SpokenContentClient()
{
}

//----------------------------------------------------------------------------
unsigned long SpokenContentClient::Open(char *ListFile, char *Bitstream)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes (I think)
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy xmltype descriptor interface
  SpokenContentDescriptorInterfaceABC* theSpokenContentDescriptorInterface= NULL;

  // This sets up the coding scheme for this D/DType and the interface
  SpokenContentCS*                     theSpokenContentCS                 = NULL;
  SpokenContentCSInterfaceABC*         theSpokenContentCSInterface        = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Allocate memory for the coding scheme
    theSpokenContentCS=new SpokenContentCS();

    // Obtain the interface
    theSpokenContentCSInterface=theSpokenContentCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"SpokenContentClient::Open: Allocation error\n");
    return(1);
  }

  // Connect the io buffer and open the bitstream
  theSpokenContentCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
  theStreamBuffer.OpenBitStream(Bitstream);

  // Create the database of filenames
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  // Read in each of the Descriptors
  try{
    // Make storage for all the pointers
    DBSpokenContentDescription = new SpokenContentDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBSpokenContentDescription[ientry] = new SpokenContentDescriptor();
      DBSpokenContentDescription[ientry]->addref();

      // Set up the interfaces to this D/DSType
      theSpokenContentDescriptorInterface=
                             DBSpokenContentDescription[ientry]->GetInterface();
      theSpokenContentCSInterface->
	            SetDescriptorInterface(theSpokenContentDescriptorInterface);

      // Now read this in
      theSpokenContentCS->StartDecode();
    }

  }
  catch(...) {
    fprintf(stderr,"SpokenContentClient::Open: Allocation error2\n");
    return(1);
  }

  // Perform the tidying up
  theSpokenContentCSInterface->destroy();
  theStreamBuffer.CloseBitStream();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------

  // Create a list of all the best matches
  MatchList theMatchList;

  // Set up the extraction tool which will be needed for the query
  SpokenContentExtractionTool*         theExtractionTool      = NULL;
  SpokenContentExtractionInterfaceABC* theExtractionInterface = NULL;
  try{
    theExtractionTool=new SpokenContentExtractionTool(0);
    theExtractionInterface=theExtractionTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"SpokenContentClient::Start: Allocation error1\n");
    return(1);
  }

  // Set up the descriptor pointers
  SpokenContentDescriptorInterfaceABC* theSpokenContentDescriptorInterface = 
                              theExtractionInterface->GetDescriptorInterface();
  SpokenContentDescriptorInterfaceABC* DBSpokenContentDescriptorInterface = NULL;

  // Set up the search tool
  SpokenContentSearchTool*         theSearchTool      = NULL;
  SpokenContentSearchInterfaceABC* theSearchInterface = NULL;
  try{
    theSearchTool = new SpokenContentSearchTool(
                                           theSpokenContentDescriptorInterface);
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"SpokenContentClient::Start: Allocation error2\n");
    return(1);
  }

  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  // Set up a generic DSCS to read in the file
  GenericDSCS             *theCS          = new GenericDSCS;
  GenericDSCSInterfaceABC *theCSInterface = theCS->GetInterface();
  theCSInterface->SetDecoderFileName(Query);
  //sth to stdout not permitted printf("Source \"%s\"\n", Query);

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
    fprintf(stderr,"SpokenContentClient::Start: Invalid XML document(1)\n");
    return(1);
  }
  GenericDS dummyxmlnode=theDocument.GetFirstChild();
  if(strcmp(dummyxmlnode.GetDSName().c_str(),"SpokenContent") != 0) {
    fprintf(stderr,"SpokenContentClient::Start: Invalid XML document(2)\n");
    return(1);
  }

  // Tell the extractor the correct node
  theExtractionInterface->SetSourceMedia(&dummyxmlnode);
	  
  // Reinitialise the D/DSType
  theExtractionInterface->InitExtracting();

  // Create the D/DSType from this medium
  theExtractionInterface->StartExtracting();

  /// ------------------- Now do the matching -------------------------------
  for(int ientry=0;ientry<NoOfElements;ientry++) {

    // Get the interface
    DBSpokenContentDescriptorInterface=
	                    DBSpokenContentDescription[ientry]->GetInterface();

    // Tell the search interface which we're matching to
    theSearchInterface->
	          SetRefDescriptorInterface(DBSpokenContentDescriptorInterface);

    // Sort the result according to distance score
    theMatchList.InsertMatch(ientry,theSearchInterface->GetDistance());
  }

  // -------------------- Display the best matches --------------------------
  for(int i=0;i<NoOfMatches;i++) {

    // Have we run out of entries?
    if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

    // Display the file and score
    printf("%s %f\n",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
		 GetFilename(),theMatchList.GetDistanceValue(i));
  }

  // Finally tidy up
  theExtractionInterface->destroy();
  theSearchInterface->destroy();

 
  return(0);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentClient::Stop()
{
  //	fprintf(stderr,"SpokenContentClient::Stop: Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long SpokenContentClient::Close()
{
  // free memories
        RamDB.FreeDatabase();
	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {
	    if (DBSpokenContentDescription[i])
	      DBSpokenContentDescription[i]->release();
	  }
	}
	
	NoOfElements =0;
	DBSpokenContentDescription =0;
       
	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& SpokenContentClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentClient::GetName(void)
{ return myName; }



