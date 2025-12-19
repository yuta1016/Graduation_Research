///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Hawley K. Rising III
// Company: Sony Corp.
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
// Copyright (c) 2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GraphicalCSCAppl.cpp
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

const UUID GraphicalCSClient::myID = UUID("");
const char *GraphicalCSClient::myName = "GraphicalCSClient";

//=============================================================================
GraphicalCSClient::GraphicalCSClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
GraphicalCSClient::~GraphicalCSClient()
{
  // Free memories
  RamDB.FreeDatabase();
  if (DBDescription) delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long GraphicalCSClient::Open(char *ListFile, char *DDLFile)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(DDLFile != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up the GraphicalCS descriptor interface
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
    fprintf(stderr,"GraphicalCSClient::Open: Allocation error\n");
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

      char path[MAXMEDIANAMELEN]; /* file name of DDL description*/

     // Initialiaze the Generic DS
      DBDescription[ientry] = GenericDS::CreateDescriptionRoot();

      // Get descriptor interface
      theDescriptionInterface=DBDescription[ientry].GetInterface();

      // Get DDL file name from data media filename and DDL file name
      strncpy(path,RamDB.GotoEntry(ientry)->GetFilename(),MAXMEDIANAMELEN-1);
      strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
      strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
      path[MAXMEDIANAMELEN-1]=0;

	  // TESTING
	  fprintf(stderr, "DB Entry %d: %s\n", ientry, path);

      // Connect DDL file to coding scheme
      theCSInterface->SetDecoderFileName(path);

      // Connect descriptors to coding schemes
      theCSInterface->SetDescriptionInterface(theDescriptionInterface);

      // Now read this in
      theCSInterface->StartDecode();

    }

  }
  catch(...) {
    fprintf(stderr,"GraphicalCSClient::Open: Allocation error2\n");
    return(1);
  }

  // TESTING
  //theCSInterface->SetEncoderFileName("test.xml");
  //theCSInterface->StartEncode();

  // Perform the tidying up
  theCSInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long GraphicalCSClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------

  // Create a list of all the best matches
  MatchList theMatchList;

  // Set up query and referece (DB) descriptions
  GenericDS theDescription = GenericDS::CreateDescriptionRoot();
  GenericDSInterfaceABC*             theDescriptionInterface = NULL;
  GenericDSInterfaceABC*             DBDescriptionInterface  = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  GenericDSCS*                     theCS                  = NULL;
  GenericDSCSInterfaceABC*         theCSInterface         = NULL;

  // Set up the search tool
  GraphicalCSSearchTool*         theSearchTool              = NULL;
  GraphicalCSSearchInterfaceABC* theSearchInterface         = NULL;
  
  try {  
    // Allocate memory for the coding scheme
    theCS=new GenericDSCS();
    // Obtain the interface
    theCSInterface=theCS->GetInterface();

    // Obtain the descriptor interface
    theDescriptionInterface = theDescription.GetInterface();

    // Create search tool
    theSearchTool = new GraphicalCSSearchTool(theDescriptionInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();

  }
  catch(...) {
    fprintf(stderr,"GraphicalCSClient::Start: Allocation error1\n");
    return(1);
  }

  // If more matches are requested than the dbase has, limit it
  /* NoOfMatches can also be set to number of elements in RamDB*/
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  // Connect DDL file to coding scheme
  theCSInterface->SetDecoderFileName(Query);
  // Connect descriptors to coding schemes
  theCSInterface->SetDescriptionInterface(theDescriptionInterface);
  // Now read query in
  theCSInterface->StartDecode();

  /// ------------------- Now do the matching -------------------------------

  for(int ientry=0;ientry<NoOfElements;ientry++) {

    // Get the interface of data base entry
    DBDescriptionInterface=DBDescription[ientry].GetInterface();

    // Tell the search interface which we're matching to
    theSearchInterface->SetRefDescriptionInterface(DBDescriptionInterface);

	fprintf(stderr, "\nMATCHING DB Entry %d\n", ientry);

    // Sort the result according to distance score
    theMatchList.InsertMatch(ientry,theSearchInterface->GetDistance());
  }

  // -------------------- Display the best matches --------------------------

  for(int i=0;i<NoOfMatches;i++) {

    // Have we run out of entries?
    if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

    // Display the file and score
    printf("%s\t%f\n",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
	   GetFilename(),
	   theMatchList.GetDistanceValue(i));

    // finish printed line
    printf("\n");
    
  }

  // Finally tidy up
  theCSInterface->destroy();
  theSearchInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long GraphicalCSClient::Stop()
{
  // fprintf(stderr,"GraphicalCSClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long GraphicalCSClient::Close()
{
  // Free memories
  RamDB.FreeDatabase();

  if (DBDescription) delete [] DBDescription;

  NoOfElements =0;
  DBDescription =0;

  return 0;
}

	
//----------------------------------------------------------------------------
const UUID& GraphicalCSClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GraphicalCSClient::GetName(void)
{ return myName; }


