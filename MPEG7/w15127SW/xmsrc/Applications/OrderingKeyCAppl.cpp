///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
// Author: Alessandro Bugatti
// Company: University of Brescia
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
// Applicable File Name:  OrderingKeyCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID OrderingKeyClient::myID = UUID("");
const char *OrderingKeyClient::myName = "OrderingKeyClient";

//=============================================================================
OrderingKeyClient::OrderingKeyClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
OrderingKeyClient::~OrderingKeyClient()
{
  // Free memories
  RamDB.FreeDatabase();
//  {
//    int i;
//    for (i=0; i<NoOfElements; i++) {
//      if (DBDescription[i])
//	DBDescription[i]->release();
//    }
//  }
  delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long OrderingKeyClient::Open(char *ListFile, char *DDLFile)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(DDLFile != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up the OrderingKey descriptor interface
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
    fprintf(stderr,"OrderingKeyClient::Open: Allocation error\n");
    return(1);
  }


  // Create the database of filenames
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  // Read in each of the Descriptors
  try{
    // Make storage for all the pointers
    DBDescription = new GenericDS [NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Inizialize the generic DS
      DBDescription[ientry] = GenericDS::CreateDescriptionRoot();
     
      // Get descriptor interface
      theDescriptionInterface=DBDescription[ientry].GetInterface();
      
 	// Get DDL file name from data media filename and DDL file name
      char path[MAXMEDIANAMELEN]; /* file name of DDL description*/
	strncpy(path,RamDB.GotoEntry(ientry)->GetFilename(),MAXMEDIANAMELEN-1);
      strncat(path,".dir/",MAXMEDIANAMELEN-1-strlen(path));
      strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
      path[MAXMEDIANAMELEN-1]=0;

	 // Connect DDL file to coding scheme
      theCSInterface->SetDecoderFileName(path);

      // Connect descriptors to coding schemes
      theCSInterface->SetDescriptionInterface(theDescriptionInterface);

      // Now read this in
      theCS->StartDecode();
    }

  }
  catch(...) {
    fprintf(stderr,"OrderingKeyClient::Open: Allocation error\n");
    return(1);
  }

  // Perform the tidying up
  theCSInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long OrderingKeyClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------
  
  // Create a list of all the best matches. In case of OrderingKey
  // this list is not a list of the best matches but a list of the
  // interesting items ordered by the selected OrderingKey

  MatchList theMatchList;

  //Set up query and reference (DB) descriptions
  GenericDS theDescription = GenericDS::CreateDescriptionRoot();

  GenericDSInterfaceABC* theDescriptionInterface = NULL;
  GenericDSInterfaceABC* DBDescriptionInterface  = NULL;

  // This set up the coding scheme for this D/DType and the interface
  GenericDSCS* 				theCS 			= NULL;
  GenericDSCSInterfaceABC*		theCSInterface		= NULL;

  // Set up the search tool
  OrderingKeySearchTool*         theSearchTool              = NULL;
  OrderingKeySearchInterfaceABC* theSearchInterface         = NULL;

  try{

    //Allocate memory for the coding scheme
    theCS = new GenericDSCS();

    //Obtain the interface
    theCSInterface=theCS->GetInterface();

    // Obtain the descriptor interface
    theDescriptionInterface = theDescription.GetInterface();

    // Create search tool
    theSearchTool = new OrderingKeySearchTool(theDescriptionInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"OrderingKeyClient::Start: Allocation error\n");
    return(1);
  }


  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  // Connect DDL file to coding scheme
  theCSInterface->SetDecoderFileName(Query);
  // Connect descriptors to coding schemes
  theCSInterface->SetDescriptionInterface(theDescriptionInterface);
  //Now read query in 
  theCSInterface->StartDecode();


  /// ------------------- Now do the matching -------------------------------
  //This function GetOrderedList() finds the entities that have to be 
  //ordered and print a list of them on stdout

  theSearchInterface->GetOrderedList();

  // Finally tidy up
  //  theExtractionInterface->destroy();
  theSearchInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long OrderingKeyClient::Stop()
{
  // fprintf(stderr,"OrderingKeyClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long OrderingKeyClient::Close()
{
  // Free memories
  RamDB.FreeDatabase();
//  {
//    int i;
//    for (i=0; i<NoOfElements; i++) {
//      if (DBDescription[i])
//	DBDescription[i]->release();
//    }
//  }
  delete [] DBDescription;

  NoOfElements =0;
  DBDescription =0;

  return 0;
}

	
//----------------------------------------------------------------------------
const UUID& OrderingKeyClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *OrderingKeyClient::GetName(void)
{ return myName; }



