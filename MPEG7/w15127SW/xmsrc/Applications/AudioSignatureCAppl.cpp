///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Thorsten Heinz
// Fraunhofer IIS
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
// Applicable File Name:  AudioSignatureCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"     /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID AudioSignatureClient::myID = UUID("");
const char *AudioSignatureClient::myName = "AudioSignatureClient";

//=============================================================================
AudioSignatureClient::AudioSignatureClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
AudioSignatureClient::~AudioSignatureClient()
{
  // Free memories
  RamDB.FreeDatabase();
  delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long AudioSignatureClient::Open(char *ListFile, char *DDLFile)
{
  DDLFilename = DDLFile;

  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(DDLFile != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up the dummy type descriptor interface
  GenericDSInterfaceABC* theDescriptionInterface = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  GenericDSCS*                     theCS                  = NULL;
  GenericDSCSInterfaceABC*         theCSInterface         = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Allocate memory for the coding scheme
    theCS=new GenericDSCS();

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"AudioSignatureClient::Open: Allocation error\n");
    return(1);
  }

  // Create the database of filenames
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  // Read in each of the Descriptions
  try{
    // Make storage for all the pointers
    DBDescription = new GenericDS [NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {
      // Initialiaze the Generic DS
      DBDescription[ientry] = GenericDS::CreateDescriptionRoot();

      // Get descriptor interface
      theDescriptionInterface=DBDescription[ientry].GetInterface();

      // Connect DDL file to coding scheme
      theCSInterface->SetDecoderFileName(RamDB.GotoEntry(ientry)->GetFilename());

      // Connect descriptors to coding schemes
      theCSInterface->SetDescriptionInterface(theDescriptionInterface);

      // Now read this in
      theCS->StartDecode();
    }
  }
  catch(...) {
    fprintf(stderr,"AudioSignatureClient::Open: Allocation error2\n");
    return(1);
  }

  // Perform the tidying up
  theCSInterface->destroy();

  return(0);
}



//----------------------------------------------------------------------------
unsigned long AudioSignatureClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------
    
  // Media
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;

  // Create media object 
  theMedia = new MultiMedia;

  // obtain media interface
  theMediaInterface=theMedia->GetInterface();


  // Create a list of all the best matches
  MatchList theMatchList;



  // -------------------- Read in the query ---------------------------------

  // Set up a generic DSCS to read in the file
  GenericDSCS             *theCS          = new GenericDSCS;
  GenericDSCSInterfaceABC *theCSInterface = theCS->GetInterface();
  theCSInterface->SetDecoderFileName(Query);

  // Make up a waste Generic DS to put all the DOM stuff into
  GenericDS theDocument = GenericDS::CreateDescriptionRoot();
  GenericDSInterfaceABC* theDocumentInterfaceABC = theDocument.GetInterface();
  GenericDSInterfaceABC* DBDescriptionInterface  = NULL;

  // Tell the coding scheme to use the Generic DS
  theCSInterface->SetDescriptionInterface(theDocumentInterfaceABC);

  // Tell the CS to initilaise the decoding
  theCSInterface->StartDecode();



  //--- Set up the search tool
  AudioSignatureSearchTool*         theSearchTool              = NULL;
  AudioSignatureSearchInterfaceABC* theSearchInterface         = NULL;


  try{
    // Create search tool
    theSearchTool = new AudioSignatureSearchTool(theDocumentInterfaceABC );

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();

    //--- Tell the search interface about the query
    theSearchInterface->SetQueryDescriptionInterface(theDocumentInterfaceABC);
  }
  catch(...) {
    fprintf(stderr,"AudioSignatureClient::Start: Allocation error1\n");
    return(1);
  }
  


  
  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // If more matches are requested than the dbase has, limit it
  //--- NoOfMatches can also be set to number of elements in RamDB
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);


  /// ------------------- Now do the matching -------------------------------
  for(int ientry=0;ientry<NoOfElements;ientry++) {
    // Get the interface of data base entry
    DBDescriptionInterface=DBDescription[ientry].GetInterface();

    // Tell the search interface which we're matching to
    theSearchInterface->SetRefDescriptionInterface(DBDescriptionInterface);

    //--- Sort the result according to distance score
    theMatchList.InsertMatch(ientry,theSearchInterface->GetDistance());
  }


  
  // -------------------- Display the best matches --------------------------
  std::cerr << "Best Matches\n\n:";

  for(int i=0;i<NoOfMatches;i++) {
    // Have we run out of entries?
    if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

    // Display the file and score
    printf("%s\t%f",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
	   GetFilename(),
	   theMatchList.GetDistanceValue(i));

    // finish printed line
    printf("\n");
    
  }

  // Finally tidy up
  theSearchInterface->destroy();
  
  return(0);
}

//----------------------------------------------------------------------------
unsigned long AudioSignatureClient::Stop()
{
  // fprintf(stderr,"AudioSignatureClient::Stop: Not Implemented");
  return 0;
}


//----------------------------------------------------------------------------
unsigned long AudioSignatureClient::Close()
{
  // Free memories
  RamDB.FreeDatabase();

  delete [] DBDescription;

  NoOfElements =0;
  DBDescription =0;

  return 0;
}


	
//----------------------------------------------------------------------------
const UUID& AudioSignatureClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AudioSignatureClient::GetName(void)
{ return myName; }



