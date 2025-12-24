///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeSeriesCAppl.cpp
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
#include "SearchUtilities/MatchList.h"

using namespace XM;

const UUID TimeSeriesClient::myID = UUID("");
const char *TimeSeriesClient::myName = "TimeSeriesClient";

//=============================================================================
TimeSeriesClient::TimeSeriesClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
TimeSeriesClient::~TimeSeriesClient()
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
unsigned long TimeSeriesClient::Open(short DescId, char *ListFile, char *Bitstream, char *AddtParamListFile)
{
  // Check that the file names are ok
  assert(DescId  != 0);
  assert(ListFile  != NULL);
  assert(Bitstream != NULL);
  assert(AddtParamListFile  != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the dummy type descriptor interface
  TimeSeriesDescriptorInterfaceABC* theDescriptorInterface = NULL;

  // This sets up the coding scheme for this D/DType and the interface
  TimeSeriesCS*                     theCS                  = NULL;
  TimeSeriesCSInterfaceABC*         theCSInterface         = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Allocate memory for the coding scheme
    theCS=new TimeSeriesCS();

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"TimeSeriesClient::Open: Allocation error\n");
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
    DBDescription = new TimeSeriesDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBDescription[ientry] = new TimeSeriesDescriptor();
      DBDescription[ientry]->addref();

      // Set up the interfaces to this D/DSType
      theDescriptorInterface=DBDescription[ientry]->GetInterface();
      theCSInterface->SetDescriptorInterface(theDescriptorInterface);

      // Now read this in
      theCS->StartDecode();
    }

  }
  catch(...) {
    fprintf(stderr,"TimeSeriesClient::Open: Allocation error2\n");
    return(1);
  }

  // Perform the tidying up
  theCSInterface->destroy();
  theStreamBuffer.CloseBitStream();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesClient::Start(char *Query,int NoOfMatches)
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

  TimeSeriesDescriptorInterfaceABC* theDescriptorInterface = NULL;
  TimeSeriesDescriptorInterfaceABC* DBDescriptorInterface  = NULL;

  // Set up the extraction tool which will be needed for the query
  TimeSeriesExtractionTool*         theExtractionTool      = NULL;
  TimeSeriesExtractionInterfaceABC* theExtractionInterface = NULL;

  // Set up the search tool
  TimeSeriesSearchTool*         theSearchTool              = NULL;
  TimeSeriesSearchInterfaceABC* theSearchInterface         = NULL;

  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new descriptor
    theExtractionTool = new TimeSeriesExtractionTool(NULL);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Obtain the descriptor interface
    theDescriptorInterface=
      theExtractionInterface->GetDescriptorInterface();

    // Create search tool
    theSearchTool = new TimeSeriesSearchTool(theDescriptorInterface);

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"TimeSeriesClient::Start: Allocation error1\n");
    return(1);
  }

  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // If more matches are requested than the dbase has, limit it
  if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
  theMatchList.SetNumberOfMatches(NoOfMatches);

  // -------------------- Read in the query ---------------------------------

  // Names for elementary parts of the media sequence
  int i=0;              // part no of sequence
  char ppmimage[25];    // filename of single image (part of sequence)
  //char path[MAXMEDIANAMELEN]; // use if required for additional input files

  //cut sequence into parts
  if (!RamDB.DecodeNamedMPEGVideo(Query)) return (unsigned long) -1;

  // Get audio for video sequence
  // if (!RamDB.LoadAudioForVideo()) break;
   
  // Tell the extractor that this is the source medium
  theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
  // Reinitialise the D Type; maybe also audio D can be extracted here
  theExtractionInterface->InitExtracting();

  // Loop over the frames of the sequence
  while (1) {

    // Load frame of video sequnece
// T.Fujita@Hitachi added #ifndef .. #else .. #endif -->
//      sprintf(ppmimage,"m2v/%d.ppm",i);
#ifndef _VISUALC_
      sprintf(ppmimage,"m2v/%d.ppm",i);
#else
      sprintf(ppmimage,"m2v\\%d.ppm",i);
#endif
// <-- T.Fujita@Hitachi
    if (!RamDB.LoadNamedImage(ppmimage)) break; // break if end of sequence

    // check frame number of image
    {
      long FrameNo;

      FrameNo=theMediaInterface->GetImageTime();
      if (FrameNo == -1) FrameNo=i-1;
      theMediaInterface->SetImageTime(FrameNo);
    }

    // e.g., select background if region mask is available
    theMediaInterface->SelectVopAlphaLayer(-2);

    // Extract D from media per frame of the input sequence
    theExtractionInterface->StartExtracting(&RamDB);

    // Increment frame counter
    i++;
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
  {
	  int	i;
	  for(i=0;i<NoOfMatches;i++) {

		// Have we run out of entries?
		if(theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;

		// Display the file and score
		printf("%s\t%f",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename(), theMatchList.GetDistanceValue(i));

		// print file name to be displayed in GUI (optional)
		printf("\t%s.dir/0.jpg",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());

		// print other informtion

		// finish printed line
		printf("\n");

	  }
  }
  // Finally tidy up
  theExtractionInterface->destroy();
  theSearchInterface->destroy();

  return(0);
}

//----------------------------------------------------------------------------
unsigned long TimeSeriesClient::Stop()
{
  // fprintf(stderr,"TimeSeriesClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long TimeSeriesClient::Close()
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
const UUID& TimeSeriesClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeSeriesClient::GetName(void)
{ return myName; }



