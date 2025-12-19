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
// Applicable File Name:  Perceptual3DShapeCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#ifdef _VISUALC_
#include <assert.h> 
#endif

#include "ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"          /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID Perceptual3DShapeClient::myID = UUID("");
const char *Perceptual3DShapeClient::myName = "Perceptual3DShapeClient";

//=============================================================================
Perceptual3DShapeClient::Perceptual3DShapeClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
Perceptual3DShapeClient::~Perceptual3DShapeClient()
{
//[warning]: seems duplicate with Close().
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

#define TRUE 1
#define FALSE 0
#include "Descriptors/Perceptual3DShape_Aux.h"

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeClient::Open(char *ListFile, char *Bitstream)
{
//global member initialization.
	CSplit::MakeSE();

  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  Perceptual3DShapeDescriptorInterfaceABC* theDescriptorInterface = NULL;

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
  Perceptual3DShapeCS*                     theBinCS               = NULL;
  Perceptual3DShapeCSInterfaceABC*         theBinCSInterface      = NULL;
  GenericDSCS*                     theDDLCS               = NULL;
  GenericDSCSInterfaceABC*         theDDLCSInterface      = NULL;

  // For reading the xsi:type attributes
  string xsitype;

  // Ok, make new instatiations of these datatypes
  try{
    //select coding scheme type
    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
      theBinCS = new Perceptual3DShapeCS();

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
      theCollection.GetTextAttribute("xsi:type", xsitype);
      if (xsitype != "DescriptorCollectionType") {
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
    fprintf(stderr,"Perceptual3DShapeClient::Open: Allocation error\n");
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
    DBDescription = new Perceptual3DShapeDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBDescription[ientry] = new Perceptual3DShapeDescriptor();
      DBDescription[ientry]->addref();

      // Set up the interfaces to this D/DSType
      theDescriptorInterface=DBDescription[ientry]->GetInterface();

      if (CodingMode == 2) { // Binary coding
	// Connect Description to CodingScheme
	theBinCSInterface->SetDescriptorInterface(theDescriptorInterface);

	// Now read this in
	theBinCSInterface->StartDecode();
      }
      else { // DDL or BiM coding

	// get next descriptor
	if (ientry==0) {
	  theDDLDescription=theCollectionInterface->GetDescription("Descriptor");
	}
	else {
	  theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
	}

	// search for correct xsi type
	while (!theDDLDescription.isNull()) {
	  theDDLDescription.GetTextAttribute("xsi:type", xsitype);
	  if(xsitype=="Perceptual3DShapeType") break;
	  theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
	}

	// Perceptual3DShape not found
	if (theDDLDescription.isNull()) {fprintf(stderr, "Perceptual 3D Shape not found\n");return (unsigned long)-1;}

	// Get DDLDescription interface
	theDDLDescriptionInterface=theDDLDescription.GetInterface();

	// Import description from DDL Tree
	theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
      }
    }
  }
  catch(...) {
    fprintf(stderr,"Perceptual3DShapeClient::Open: Allocation error2\n");
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
unsigned long Perceptual3DShapeClient::Start(char *Query,int NoOfMatches)
{
  // Check data is sane
  assert(Query       != NULL);
  assert(NoOfMatches != 0);

  // --------------------- Initialise the query data -----------------------
  // Media
  MultiMedia *theMedia = NULL;
  MultiMediaInterfaceABC *theMediaInterface = NULL;
  MediaThreeD *media = NULL;

  // Create a list of all the best matches
  MatchList theMatchList;

  Perceptual3DShapeDescriptorInterfaceABC* theDescriptorInterface = NULL;
  Perceptual3DShapeDescriptorInterfaceABC* DBDescriptorInterface  = NULL;

  // Set up the extraction tool which will be needed for the query
  Perceptual3DShapeExtractionTool*         theExtractionTool      = NULL;
  Perceptual3DShapeExtractionInterfaceABC* theExtractionInterface = NULL;

  // Set up the search tool
  Perceptual3DShapeSearchTool*         theSearchTool              = NULL;
  Perceptual3DShapeSearchInterfaceABC* theSearchInterface         = NULL;

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

    // Create search tool
    theSearchTool = new Perceptual3DShapeSearchTool(theDescriptorInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"Perceptual3DShapeClient::Start: Allocation error1\n");
    return(1);
  }

  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  // Names for elementary parts of the media sequence
//  int i=0;              // part no of sequence
//  char ppmimage[25];    // filename of single image (part of sequence)
  //char path[MAXMEDIANAMELEN]; // use if required for additional input files

  //cut sequence into parts
  media=RamDB.LoadNamed3DMesh(Query);
  if (!media) {
    fprintf(stderr,"Warning: Query not found\n");
    return (unsigned long) -1; // break if end of database
  }

  // Get audio for video sequence
  // if (!RamDB.LoadAudioForVideo()) break;
   
  // Tell the extractor that this is the source medium
  theExtractionInterface->SetSourceMedia(media);
	  
  theExtractionInterface->InitExtracting();
  theExtractionInterface->StartExtracting();

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

//zhanghui new added.
  theMediaInterface->destroy();
  if(media!=NULL)   free_media3D(media);

  return(0);
}

//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeClient::Stop()
{
  return 0;
}
//----------------------------------------------------------------------------
unsigned long Perceptual3DShapeClient::Close()
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
const UUID& Perceptual3DShapeClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Perceptual3DShapeClient::GetName(void)
{ return myName; }



