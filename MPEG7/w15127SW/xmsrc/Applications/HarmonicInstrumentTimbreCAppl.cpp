///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// Applicable File Name:  HarmonicInstrumentTimbreCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID HarmonicInstrumentTimbreClient::myID = UUID("");
const char *HarmonicInstrumentTimbreClient::myName = "HarmonicInstrumentTimbreClient";

//=============================================================================
HarmonicInstrumentTimbreClient::HarmonicInstrumentTimbreClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreClient::~HarmonicInstrumentTimbreClient()
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
unsigned long HarmonicInstrumentTimbreClient::Open(char *ListFile, char *Bitstream)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  HarmonicInstrumentTimbreDescriptorInterfaceABC* theDescriptorInterface = NULL;

  // This descriptors are used for DDL/BiM based coding
  GenericDS theDocument;
  GenericDSInterfaceABC *theDocumentInterface             = NULL;
  GenericDS theMpeg7Root;
  GenericDS theCollection;
  GenericDSInterfaceABC *theCollectionInterface           = NULL;
  GenericDS theDDLDescription;
  GenericDSInterfaceABC *theDDLDescriptionInterface       = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
  HarmonicInstrumentTimbreCS*                     theBinCS               = NULL;
  HarmonicInstrumentTimbreCSInterfaceABC*         theBinCSInterface      = NULL;
  GenericDSCS*                     theDDLCS               = NULL;
  GenericDSCSInterfaceABC*         theDDLCSInterface      = NULL;

  // For reading the xsi:type attributes
  string xsitype;

  // Ok, make new instatiations of these datatypes
  try{
    //select coding scheme type
      //CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
      CodingMode=atoi(getpara("CodingMode","0"));//default is Binary

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
      theBinCS = new HarmonicInstrumentTimbreCS();

      // Obtain the interface
      theBinCSInterface=theBinCS->GetInterface();
    }
    else { // DDL or BiM coding
      // Create the XML document and obtain the interface
      theDocument = GenericDS::CreateDescriptionRoot();
      theDocumentInterface=theDocument.GetInterface();

      // Now create the XML parser based coding scheme and obtain the interface
      theDDLCS = new GenericDSCS(theDocumentInterface);
      theDDLCSInterface=theDDLCS->GetInterface();

      // Connect DDL file to coding scheme
      theDDLCSInterface->SetDecoderFileName(Bitstream);

      // Decode DDL file
      theDDLCSInterface->StartDecode();

      // Parse the top level elements  
      // get the Mpeg7Root element
      theMpeg7Root = theDocumentInterface->GetDescription("Mpeg7");

      // Mpeg7Root not found
      if (theMpeg7Root.isNull()) {
	fprintf(stderr, "Top level is not Mpeg7\n");
	return (unsigned long)-1;
      }

      // Get the Collection element for the descriptors
      theCollection = theMpeg7Root.GetDescription("DescriptionUnit");

      // Collection not found
      if (theCollection.isNull()) {
	fprintf(stderr, "DescriptionUnit not found\n");
	return (unsigned long)-1;
      }
      
      // Check type of Collection Unit
      theCollection.GetTextAttribute("xsi:type", xsitype);
      if (xsitype != "DescriptorCollectionType") 
      {
	fprintf(stderr, "DescriptorCollection not found\n");
	return (unsigned long)-1;
      }

      // Get Collection interface
      theCollectionInterface=theCollection.GetInterface();

      // initalize theDDLdescription
      theDDLDescription=theCollection;
      theDDLDescriptionInterface=theCollectionInterface;
    
      // Now create the XML parser based coding scheme
      theDDLCS = new GenericDSCS();

      // Obtain the interface
      theDDLCSInterface=theDDLCS->GetInterface();
    }
  }
  catch(...) {
    fprintf(stderr,"HarmonicInstrumentTimbreClient::Open: Allocation error\n");
    return(1);
  }

  // Connect the io buffer and open the bitstream
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
    theStreamBuffer.OpenBitStream(Bitstream);
  }

  // Create the database of filenames
  RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();

  // Read in each of the Descriptors
  try{
    // Make storage for all the pointers
    DBDescription = new HarmonicInstrumentTimbreDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBDescription[ientry] = new HarmonicInstrumentTimbreDescriptor();
      DBDescription[ientry]->addref();

      // Set up the interfaces to this D/DSType
      theDescriptorInterface=DBDescription[ientry]->GetInterface();

      if (CodingMode == 2) 
      { // Binary coding
	// Connect Description to CodingScheme
	theBinCSInterface->SetDescriptorInterface(theDescriptorInterface);

	// Now read this in
	theBinCSInterface->StartDecode();
      }
      else 
      { // DDL or BiM coding

	// get next descriptor
	if (ientry==0) 
	{
	    theDDLDescription=theCollectionInterface->GetDescription("AudioDS");
	}
	else 
	{
	    theDDLDescription=theDDLDescription.GetNextSibling("AudioDS");
	}

	// search for correct xsi type
	while (!theDDLDescription.isNull()) 
	{
	    theDDLDescription.GetTextAttribute("xsi:type", xsitype);
	    if(xsitype=="HarmonicInstrumentTimbreType") 
		break;
	    theDDLDescription=theDDLDescription.GetNextSibling("AudioDS");
	}

	// HarmonicInstrumentTimbre not found
	if (theDDLDescription.isNull()) 
	{
	    fprintf(stderr, " AudioDS not found !! \n");
	    return (unsigned long)-1;
	}

	// Get DDLDescription interface
	theDDLDescriptionInterface=theDDLDescription.GetInterface();

	// Import description from DDL Tree
	theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
      }
    }
  }
  catch(...) {
    fprintf(stderr,"HarmonicInstrumentTimbreClient::Open: Allocation error2\n");
    return(1);
  }

  // Perform the tidying up
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->destroy();
    theStreamBuffer.CloseBitStream();
  }
  else {
    theDDLCSInterface->destroy();
  }

  return(0);
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreClient::Start(char *Query,int NoOfMatches)
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

  HarmonicInstrumentTimbreDescriptorInterfaceABC* theDescriptorInterface = NULL;
  HarmonicInstrumentTimbreDescriptorInterfaceABC* DBDescriptorInterface  = NULL;

  // Set up the extraction tool which will be needed for the query
  HarmonicInstrumentTimbreExtractionTool*         theExtractionTool      = NULL;
  HarmonicInstrumentTimbreExtractionInterfaceABC* theExtractionInterface = NULL;

  // Set up the search tool
  HarmonicInstrumentTimbreSearchTool*         theSearchTool              = NULL;
  HarmonicInstrumentTimbreSearchInterfaceABC* theSearchInterface         = NULL;

  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new descriptor
    theExtractionTool = new HarmonicInstrumentTimbreExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Obtain the descriptor interface
    theDescriptorInterface=
      theExtractionInterface->GetDescriptorInterface();

    // Create search tool
    theSearchTool = new HarmonicInstrumentTimbreSearchTool(theDescriptorInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"HarmonicInstrumentTimbreClient::Start: Allocation error1\n");
    return(1);
  }

  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

   
  // Tell the extractor that this is the source medium
  theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
  // Reinitialise the D Type; maybe also audio D can be extracted here
  theExtractionInterface->InitExtracting(Query);

  // Loop over the frames of the sequence
  while (1) {

    // Extract D from media per frame of the input sequence
    theExtractionInterface->StartExtracting();

    // Increment frame counter
    break;
  }

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

    // Have we run out of entries?
    if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

    // Display the file and score
    printf("%s\t%f",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
	   GetFilename(),
	   theMatchList.GetDistanceValue(i));

    // print file name to be displayed in GUI (optional)

    // print filename which is display in GUI on right button

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
unsigned long HarmonicInstrumentTimbreClient::Stop()
{
  // fprintf(stderr,"HarmonicInstrumentTimbreClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreClient::Close()
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
const UUID& HarmonicInstrumentTimbreClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreClient::GetName(void)
{ return myName; }



