///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TOSHIBA Corp., Osamu Hori, Toshimitsu Kaneko, Koji Yamamoto
// R&D Center, TOSHIBA Corp.
// (contributing organizations names)
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
// Applicable File Name:  SequentialSummarySAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#include <sys/stat.h>
#ifdef _VISUALC_
#include <direct.h>
#endif

#include "Applications/ServerApplications.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

using namespace XM;

const UUID SequentialSummaryServer::myID = UUID("");
const char *SequentialSummaryServer::myName = "SequentialSummaryServer";

//=============================================================================
SequentialSummaryServer::SequentialSummaryServer()
{
}

//----------------------------------------------------------------------------
SequentialSummaryServer::~SequentialSummaryServer()
{
}

//----------------------------------------------------------------------------
// This takes two input parameters:
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//
// The contents of each file in ListFile is read in and the Dummy D/DSType
// created using the data. This is then written to the bitstream/DDL file. 


unsigned long SequentialSummaryServer::Start(char *ListFile, char *DDLFile)
{
  // Check that the file names are ok
  assert(ListFile  != NULL);
  assert(DDLFile != NULL);

  // -------------------- Initialisation of data ----------------------------

  // Media data base
  MediaDB RamDB;

  // Media
  MultiMedia *theMedia;
  MultiMediaInterfaceABC *theMediaInterface;
  
  // This sets up the dummytype description interface
  GenericDS theDescription = GenericDS::CreateDescriptionRoot();
  GenericDSInterfaceABC* theDescriptionInterface =
    theDescription.GetInterface();

  // This creates the extraction tool and extraction tool interface. Set
  // to null initially for safety.
  SequentialSummaryExtractionTool*         theExtractionTool               = NULL;
  SequentialSummaryExtractionInterfaceABC* theExtractionInterface          = NULL;
	
  // This sets up the coding scheme for this D/DType and the interface
  GenericDSCS*                     theCS                           = NULL;
  GenericDSCSInterfaceABC*         theCSInterface                  = NULL;

  // Ok, make new instatiations of these datatypes
  try{
    // Create media object 
    theMedia = new MultiMedia;

    // obtain media interface
    theMediaInterface=theMedia->GetInterface();

    // Pass it null so that it creates a new description
    theExtractionTool = new SequentialSummaryExtractionTool(theDescriptionInterface);

    // Obtain the extraction interface from this extraction tool
    theExtractionInterface=theExtractionTool->GetInterface();

    // Now create the coding scheme
    theCS = new GenericDSCS(theDescriptionInterface);

    // Obtain the interface
    theCSInterface=theCS->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"SequentialSummaryServer::Start: Allocation error\n");
    return(1);
  }

  // All of the pointers should now be non-null so check them
  assert(theDescriptionInterface         != NULL);
  assert(theMedia                        != NULL);
  assert(theMediaInterface               != NULL);
  assert(theExtractionTool               != NULL);
  assert(theExtractionInterface          != NULL);
  assert(theCS                           != NULL);
  assert(theCSInterface                  != NULL);

  // ---------------------- Process the files -----------------------------
  
  // Connect media to media data base
  RamDB.SetMedia(theMediaInterface);

  // Create a database of all the files to process
  RamDB.CreateDatabase(ListFile);

  // This example is for a video sequence
  // Loop over the files in the database
  int NDBaseEntries=RamDB.GetNoOfElements();
  for(int ientry=0;ientry<NDBaseEntries;ientry++) {

    // Names for elementary parts of the media sequence
    int i=0;              // part no of sequence
    char ppmimage[25];    // filename of single image (part of sequence)
    char path[MAXMEDIANAMELEN];

    if (!RamDB.DBDecodeMPEGVideo()) break; //cut sequence into parts

    // Create directory for DDL files or for additional media results
    // Compute directory name from media filename
    // use this only if required
    strncpy(path,RamDB.GetMediaFilename(),MAXMEDIANAMELEN-1);
    strncat(path,".dir",MAXMEDIANAMELEN-1-strlen(path));
    path[MAXMEDIANAMELEN-1]=0;
#ifndef _VISUALC_
    mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
    _mkdir(path);
#endif

    // Tell the dummytype about the output stream
    strncat(path,"/",MAXMEDIANAMELEN-1-strlen(path));
    strncat(path,DDLFile,MAXMEDIANAMELEN-1-strlen(path));
    theCSInterface->SetEncoderFileName(path);

    // Tell the extractor that this is the source medium
    theExtractionInterface->SetSourceMedia(theMediaInterface);
	  
    // Reinitialise the D Type; maybe also audio D can be extracted here
    theExtractionInterface->InitExtracting();

    // Loop over the frames of the sequence
    while (1) {

      // Load frame of video sequnece
      sprintf(ppmimage,"m2v/%d.ppm",i);
      if (!RamDB.LoadNamedImage(ppmimage)) break; // break if end of sequence

      // Extract D from media per frame of the input sequence
      theExtractionInterface->StartExtracting();

      // Increment frame counter
      i++;
    }

    // Collect partial results of the frame based extraction
    theExtractionInterface->PostExtracting();

    // Write the D/DSType to binary
    theCSInterface->StartEncode();
       
  } // Onto the next source file

  // Tidy up all the memory allocations
  theExtractionInterface->destroy();
  theCSInterface->destroy();

  // All done
  return(0);
}

//----------------------------------------------------------------------------
unsigned long SequentialSummaryServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long SequentialSummaryServer::Open()
{
  return(0);
}

//----------------------------------------------------------------------------
unsigned long SequentialSummaryServer::Close()
{
  return(0);
}

//----------------------------------------------------------------------------
const UUID& SequentialSummaryServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *SequentialSummaryServer::GetName(void)
{ 
  return(myName); 
}


