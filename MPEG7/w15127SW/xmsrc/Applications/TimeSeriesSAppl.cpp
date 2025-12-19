///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Takehiro FUJITA
// Hitachi, Ltd.
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
// Applicable File Name:  TimeSeriesSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//T.Fujita@Hitachi add for "_mkdir()" -->
#ifdef _VISUALC_
#include <direct.h>
#endif
// <-- T.Fujita@Hitachi


#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "momusys.h"
#include "vopio.h"

using namespace XM;

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

const UUID TimeSeriesServer::myID = UUID("");
const char *TimeSeriesServer::myName = "TimeSeriesServer";

//=============================================================================
TimeSeriesServer::TimeSeriesServer()
{
	// Initialize the ID of an accomodated descriptor
	DescriptorId = 0;
}


//----------------------------------------------------------------------------
TimeSeriesServer::~TimeSeriesServer()
{
}


//----------------------------------------------------------------------------
unsigned long TimeSeriesServer::Open()
{
	// This server doesn't handle pull
	return(0);
}


//----------------------------------------------------------------------------
// This takes four input parameters:
//   DescId - Visual Descriptor ID
//   ListFile - This gives a text string giving the filename of the file
//              containing a sequence of filenames which in turn contain 
//              the media to encode.
//   Bitstream - This gives the file name of the binary output file.
//   
//
// The contents of each file in ListFile is read in and the Dummy D/DSType
// created using the data. This is then written to the bitstream/DDL file. 


//unsigned long TimeSeriesServer::Start(short DescId, char *ListFile, char *Bitstream, char *AddtParamListFile)  //sth interface violation
unsigned long TimeSeriesServer::Start(char *ListFile, char *Bitstream)  //sth interface violation
{
	int CodingMode;

	short DescId = atoi(getpara("DescriptorID", "2"));
	char *AddtParamListFile = getpara("AddtParamListFile", "");

	// Check that the file names are ok
	assert(DescId  != 0);
	assert(ListFile  != NULL);
	assert(Bitstream != NULL);
	assert(AddtParamListFile  != NULL);

	// -------------------- Initialisation of data ----------------------------

	// the current input stream
	char	CurrentInputFile[256];
	// the current output stream
	char	CurrentBitStream[256];
	// the extension of the output stream name
	char	ext_string[8];

	// the number of descriptors to be extracted
	int	nEntries;

	// Media data base
	MediaDB RamDB;		// MPEG streams data base
	MediaDB SubRamDB;		// frame images data base

	// Media (MPEG streams)
	MultiMedia *theMedia;
	MultiMediaInterfaceABC *theMediaInterface;

	// This sets up a binary file io with a buffer size of 512bytes (I think)
	EncoderFileIO theStreamBuffer(0xFFFF);

	// This sets up the dummy type descriptor interface
	TimeSeriesDescriptorInterfaceABC* theDescriptorInterface = NULL;

	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	TimeSeriesExtractionTool*         theExtractionTool               = NULL;
	TimeSeriesExtractionInterfaceABC* theExtractionInterface          = NULL;

	// This sets up the coding scheme for this D/DType and the interface
	TimeSeriesCS*                     theCS                           = NULL;
	TimeSeriesCSInterfaceABC*         theCSInterface                  = NULL;

	// Get the coding scheme type
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "0"));

	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;

	GenericDS theMpeg7Root, theCollection, theDescriptionUnit, theContent, theVisualTimeSeries;
	GenericDSInterfaceABC *theVisualTimeSeriesInterface;

#else
	CodingMode = 2;
#endif // USEXML

	// Ok, make new instatiations of these datatypes
	try{
		// Create media object 
		theMedia = new MultiMedia;

		// obtain media interface
		/* connect descriptor interface */
		theMediaInterface=theMedia->GetInterface();

		// Pass it null so that it creates a new descriptor
		/* extraction tool*/
		//    theExtractionTool = new TimeSeriesExtractionTool(NULL);
		theExtractionTool = new TimeSeriesExtractionTool(NULL);

		// Obtain the extraction interface from this extraction tool
		/* connect descriptor to extraction tool*/
		theExtractionInterface=theExtractionTool->GetInterface();

		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();

		// Now create the coding scheme
		theCS = new TimeSeriesCS(theDescriptorInterface);

		// Obtain the interface
		/* connect descriptor to coding scheme*/
		theCSInterface=theCS->GetInterface();
	}
	catch(...) {
		fprintf(stderr,"TimeSeriesServer::Start: Allocation error\n");
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

	// Lastly, tell the TimeSeries type about the output stream
	theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	//  theStreamBuffer.StartBitStream(Bitstream);

	// ---------------------- Assign the content descriptor ID --------------
	theDescriptorInterface->SetContentDescriptorId(DescId);


	if (CodingMode == 2)
	{
#if 1
		fprintf(stderr,"Binary coding of TimeSeries is not supported. Set CodingMode to 0 for DDL representation.\n");
		return (1);
#else
		// create coding scheme
		theCS = new TimeSeriesCS(theDescriptorInterface);
		theCSInterface = theCS->GetInterface();
		// connect stream to coding scheme
		theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
#endif
	}
#ifdef USEXML
	else
	{
		// Create the XML document
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();

		// Create the MPEG7 root element
		theMpeg7Root = theDocumentInterface->CreateChild("Mpeg7");
		theMpeg7Root.SetAttribute("xmlns", MPEG7_BASE_NS);
		theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
		theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_BASE_SCHEMA_LOCATION);
		theMpeg7Root.SetAttribute("xmlns:mpeg7", MPEG7_BASE_NS);

		theDescriptionUnit = theMpeg7Root.CreateChild("DescriptionUnit");
		theDescriptionUnit.SetAttribute("xsi:type", "ContentCollectionType");

		// Now create the XML parser based coding scheme
		theDDLCS = new GenericDSCS(theDocumentInterface);

		// Obtain the interface
		theDDLCSInterface = theDDLCS->GetInterface();

		// Connect the DDL file to the coding scheme
		theDDLCSInterface->SetEncoderFileName(Bitstream);
	}
#endif // USEXML

	// ---------------------- Process the files -----------------------------

	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// Create a database of all the MPEG files to process
	RamDB.CreateDatabase(ListFile);
	int NDBaseEntries=RamDB.GetNoOfElements();

	// -------------------- Check additional parameters -----------------------
	AddtParams *a_params = new AddtParams[NDBaseEntries];

	/* read additional parameter file*/
	this->ReadAdditionalParameters(AddtParamListFile, a_params, NDBaseEntries);


	//------------- process each file in the DB ----------------
	AddtParams *pa = a_params;
	// Loop over the files in the database
	for(int ientry=0;ientry<NDBaseEntries;ientry++, pa++) {

		// Names for elementary parts of the media sequence
		//	int i=0;              // part no of sequence
		//	char ppmimage[25];    // filename of single image (part of sequence)
		//	char path[MAXMEDIANAMELEN];

		// set additional parameters
		theDescriptorInterface->SetSeriesType(pa->sTimeSeriesType);
		theDescriptorInterface->SetTimeIncr(pa->nTimeIncr);
		theDescriptorInterface->SetOffset(pa->nOffset);
		theDescriptorInterface->SetFrameRate(pa->nFrameRateCode, 1);	// Only integer/per second frames rates are supported, fix for other frame rates

#ifdef _VISUALC_
		// Make a temporary drectory to store frame images
		// used by RamDB.DBDecodeMPEGVideo()
		_mkdir("m2v");//sth both (Win + Unix) or nothing
#endif

		// extract all frames
		if (!RamDB.DBDecodeMPEGVideo()) break; //cut sequence into parts

		// Prepare the current output stream (name)
		short currentType = theDescriptorInterface->GetSeriesType();
		memset(ext_string, 0, 8);		// initialize
		if (currentType == TYPE_REGULAR_SERIES) {
			strcpy(ext_string, EXT_REGULAR_SERIES);
		} else if (currentType == TYPE_IRREGULAR_SERIES) {
			strcpy(ext_string, EXT_IRREGULAR_SERIES);
		}
		memset(CurrentInputFile, 0, 256);		// initialize
		strcpy(CurrentInputFile, RamDB.GetMediaFilename());		// get the name
		int nLen = strlen(CurrentInputFile);
		memset(CurrentBitStream, 0, 256);	    // initialize
		strncpy(CurrentBitStream, CurrentInputFile, nLen-3);
		strcat(CurrentBitStream, ext_string);	    // change an extention

		// create a list of input frames
		//  (a list of frames to be contained in a Regular TimeSeries)
		this->CreateInputList(theDescriptorInterface, pa, "m2v", &nEntries);
		//sth check this

		//----- extract content descriptors according to the list --------
		// Create a database of frame images to be processed
		SubRamDB.CreateDatabase(INPUT_LIST_FILE);

		// Prepare extraction
		theDescriptorInterface->SetNumOfContents(nEntries);
		theDescriptorInterface->AllocContentDescriptors();

		// Reinitialise the D Type; maybe also audio D can be extracted here
		theExtractionInterface->InitExtracting();
		// Extract D from media per frame of the input sequence
		theExtractionInterface->StartExtracting(&SubRamDB);

		// Collect partial results of the frame based extraction
		theExtractionInterface->PostExtracting();


		if (CodingMode == 2)
		{
			// Write the D/DSType to binary
			theCSInterface->StartEncode();
		}
#ifdef USEXML
		else
		{
			theContent = theDescriptionUnit.CreateChild("Content");
			theContent.SetAttribute("xsi:type", "VideoType");
			theCollection = theContent.CreateChild("Video");
			theVisualTimeSeries=theCollection.CreateChild("VisualTimeSeriesDescriptor");
			theVisualTimeSeriesInterface=theVisualTimeSeries.GetInterface();

			theDescriptorInterface->ExportDDL(theVisualTimeSeriesInterface);
		}
#endif // USEXML

		// Free Descrptors in the TimeSeries
		//	theDescriptorInterface->FreeContentDescriptors();

	} // Onto the next source file

	if(CodingMode == 2)
	{
		// Close the current output stream (Shut down the output stream)
		theStreamBuffer.CloseBitStream();
		theCSInterface->destroy();
	}else
	{
#ifdef USEXML
		theDDLCSInterface->StartEncode();
		theDDLCSInterface->destroy();
#endif // USEXML
	}

	// Tidy up all the memory allocations
	theExtractionInterface->destroy();

	//  // Shut down the output stream
	//  theStreamBuffer.CloseBitStream();

	// delete additional parametes
	delete []a_params;

	// delete temporally list
	remove(INPUT_LIST_FILE);

	// All done
	return(0);
}


//----------------------------------------------------------------------------
unsigned long TimeSeriesServer::Stop()
{
  return(0);
}


//----------------------------------------------------------------------------
unsigned long TimeSeriesServer::Close()
{
  return(0);
}

	
//----------------------------------------------------------------------------
const UUID& TimeSeriesServer::GetObjectID(void)
{ 
  return(myID);
}

//----------------------------------------------------------------------------
const char *TimeSeriesServer::GetName(void)
{ 
  return(myName); 
}


//====================================================================

bool TimeSeriesServer::ReadAdditionalParameters(char *parameterfile, AddtParams *a_params, int nEntries)
{
  //sth proprietary parameter file format?? why not usage of getpara()?

	FILE *parafile;
	AddtParams	*pa;
	char		parastring[128];
	int i;
  
	parafile=fopen(parameterfile,"rt");
	if (!parafile) {
		perror("ERROR : Could not open parameterfile!");
		exit(2);
	}

	pa = a_params;
	for (i = 0; i < nEntries; i++) {
		while (1) {
			fscanf(parafile, "%s", parastring);
			if (parastring[0] == '/') {
				if (parastring[1] == '/' || parastring[1] == '*') {
					continue;
				} else {
					fscanf(parafile, "%d", &pa->nFrameRateCode);
				}
			} else {
				sscanf(parastring, "%d", &pa->nFrameRateCode);
				break;
			}
		}
		fscanf(parafile, "%s", pa->sTimeSeriesType);
		fscanf(parafile, "%d", &pa->nTimeIncr);
		fscanf(parafile, "%d", &pa->nOffset);
		fscanf(parafile, "%d", &pa->nDuration);
		pa++;
	}

	fclose(parafile);

	return (true);
}

//----------------------------------------------------------------------------
// CAUTION: this routine handles only the case of 2 frame-per-second.
//          because only ColorLayout descriptor is containd currently
//          and because 2 fps is the best parameter for the ColorLayout D.
//          this is needed to modify in order to handle other cases
//          which is specified by "TimeIncr" frame-per-second.
bool TimeSeriesServer::CreateInputList(TimeSeriesDescriptorInterfaceABC* theDescriptorInterface, AddtParams *a_param, const char *in_path, int *nEntries)
{
	FILE	*listfile;
	int		Offset;
//	MediaDuration	TimeIncr;
	int		md;
	bool	IsOffset;
	int		startFrame;
	int		endFrame;
	int		i;
	int		tmp;
	int		num;

	num = 0;

	// create a temporal list
	listfile = fopen(INPUT_LIST_FILE, "wt");
	if (listfile == NULL) {
		return (false);
	}

	// get the number of start frame and the number of end frame
	theDescriptorInterface->GetOffset(Offset, IsOffset);
	md = Offset;
	startFrame = 0;
	if (IsOffset) {
		startFrame += Offset;
		//startFrame += md.D;
		//startFrame = startFrame * 24 + md.h;
		//startFrame = startFrame * 60 + md.m;
		//startFrame = startFrame * 60 + md.s;
		//startFrame = startFrame * md.nnFraction + md.nn;

		endFrame = startFrame + a_param->nDuration;
	} else {
		endFrame = a_param->nDuration;
	}

	for (i = startFrame; i < endFrame; i+=a_param->nTimeIncr) {
		// 1st frame
#ifndef _VISUALC_
		fprintf(listfile,"%s/%d.ppm\n", in_path, i);
#else
		fprintf(listfile,"%s\\%d.ppm\n", in_path, i);
#endif
		num++;
	}

	fclose(listfile);

	// return the number of contents
	*nEntries = num;

	return (true);
}

//------------
//------------
unsigned long TimeSeriesServer::CLStart(int nEntries, char *Bitstream, MediaDB *ramDB)
{
	/* image media*/
	MomVop *media;
	/* stream data base*/
	EncoderFileIO theStreamBuffer(0xFFFF);
	/* descriptor */
	ColorLayoutDescriptorInterfaceABC	*theDescriptorInterface;
	/* extraction tool*/
	ColorLayoutExtractionTool **theExtractionTool;
	ColorLayoutExtractionTool *pExtTool;
	ColorLayoutExtractionInterfaceABC	*theExtractionInterface;

	/* descriptor */
//	ColorLayoutDescriptor *theDescriptor;
	/* coding schemes*/
	ColorLayoutCS **theCS;
	ColorLayoutCS *pCS;
	ColorLayoutCSInterfaceABC *theCSInterface;

	int		cnt;

    // Create a database of frame images to be processed
	theStreamBuffer.StartBitStream(Bitstream);

	theExtractionTool = new ColorLayoutExtractionTool *[nEntries];
	for (cnt = 0; cnt < nEntries; cnt++) {
		/* get color layout extraction interface*/
		pExtTool = new ColorLayoutExtractionTool(0); /* create also descriptor*/
		theExtractionTool[cnt] = pExtTool;

		theExtractionInterface=pExtTool->GetInterface();
		/* get color layout interface*/
		theDescriptorInterface= theExtractionInterface->GetDescriptorInterface();

		/* malloc media and load input data*/
		media=ramDB->DBLoadImage();
		if (!media) break;
		//sth moved to MultiMedia!!		theExtractionInterface->SetSourceMedia(media);
		/* reset layout values*/
		theExtractionInterface->InitExtracting();
		/* extract descriptor form media*/
		theExtractionInterface->StartExtracting();
			/* if you would like to use other combination of NumberOfCoeff,
			execute StartExtracting(NumberOfYCoeff, NumberOfCCoeff); */

		/* free media*/
		freevop(media);
		theExtractionInterface->destroy();
	}

	theCS = new ColorLayoutCS *[nEntries];
	for (cnt = 0; cnt < nEntries; cnt++) {
		pExtTool = theExtractionTool[cnt];
		theExtractionInterface=pExtTool->GetInterface();
		/* get color layout interface*/
		theDescriptorInterface= theExtractionInterface->GetDescriptorInterface();
		/* connect descriptor to coding scheme*/
		pCS = new ColorLayoutCS(theDescriptorInterface);
		theCS[cnt] = pCS;
		theCSInterface=pCS->GetInterface();

		/* connect stream to coding scheme*/
		theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
//		theDescriptorInterface->SetValuesFromParameterFile();


		/* code descriptor data*/
		theCSInterface->StartEncode();
		theCSInterface->destroy();
	}


	theStreamBuffer.CloseBitStream();
	
	delete []theExtractionTool;
	delete []theCS;
	return 0;
}
