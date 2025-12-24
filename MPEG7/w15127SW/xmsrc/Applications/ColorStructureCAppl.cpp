////////////////////////////////////////////////////////////////////////
//
// ColorStructureCAppl.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// Dean Messing, Sharp Laboratories of America, Camas, WA
// 
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Description of associated preprocessor definitions
// 
// Project XMWinExe
//		USECT				includes the ColorStructure Server and Client in main()
//		READ_WRITE_MOM		internal (Sharp) patch to MediaIO to read/write MomVop 
//								structures into single file, bypassing imagemagick
// Project XMLib
//		READ_WRITE_MOM		see above project (required in both projects)
//		FLOAT_DISTANCE		causes the distance value to be a scaled quantity rather than
//								the cardinal ranking which is required for CEMeasure for ANMRR
//
// The standard configuration is for all the above definitions (except USECT) to be undefined
// The internal (Sharp) standard configuration is to define: USECT, READ_WRITE_MOM in XMWinExe 
// and READ_WRITE_MOM in XMLib.
// Also, all unused USExx definitions are undefined and all unused folders in XMLib 
// are set to "exclude from build" to speed up compilation
// 
////////////////////////////////////////////////////////////////////////

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

const UUID ColorStructureClient::myID = UUID("");
const char *ColorStructureClient::myName = "ColorStructureClient";

#define FLOAT_DISTANCE

//=============================================================================
ColorStructureClient::ColorStructureClient():
RamDB(),
NoOfElements(0),
DBDescription(0)
{
	m_ResultFile = stdout;
}

//----------------------------------------------------------------------------
ColorStructureClient::~ColorStructureClient()
{
	// Free memories
	RamDB.FreeDatabase();
	{
		int i;
		for (i=0; i<NoOfElements; i++)
		{
			if (DBDescription[i])
				DBDescription[i]->release();
		}
	}
	delete [] DBDescription;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureClient::Open(char *ListFile, char *Bitstream)
{
	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(Bitstream != NULL);

	// -------------------- Initialisation of data ----------------------------

	// Build mapping tables (only useful if more than 2^24/(32*240)=218 queries)
	//ColorStructureExtractionTool::BuildColorQuantTable();	// Base == 256

	// This sets up a binary file io with a buffer size of 512bytes
	DecoderFileIO theStreamBuffer(0x1000);

	// This sets up the dummy type descriptor interface
	ColorStructureDescriptorInterfaceABC* theDescriptorInterface = NULL;

	// This descriptors are used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface				= NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface			= NULL;
	GenericDS theDDLDescription;
	GenericDSInterfaceABC *theDDLDescriptionInterface		= NULL;

	// This sets up the coding scheme for this D/DType and the interface
	int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
	ColorStructureCS*                     theBinCS               = NULL;
	ColorStructureCSInterfaceABC*         theBinCSInterface      = NULL;
	GenericDSCS*                          theDDLCS               = NULL;
	GenericDSCSInterfaceABC*              theDDLCSInterface      = NULL;

	// For reading the xsi:type attributes
	string xsitype;

	// Ok, make new instatiations of these datatypes
	try{
		//select coding scheme type
		CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

		if(CodingMode == 2) { // Binary coding
			// Allocate memory for the coding scheme
			theBinCS=new ColorStructureCS();

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
			//theCollection = theMpeg7Root.GetDescription("ContentDescription"); //RO'C 
			theCollection= theMpeg7Root.GetDescription("DescriptionUnit"); // m15122

			// Collection not found
			if(theCollection.isNull()) {
				fprintf(stderr, "DescriptionUnit not found\n");
				return (unsigned long)-1;
			}
			theCollection.GetTextAttribute("xsi:type", xsitype);
			if(xsitype != "DescriptorCollectionType") {
				fprintf(stderr, "DescriptorCollectionType not found\n");
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
		fprintf(stderr,"ColorStructureClient::Open: Allocation error\n");
		return(1);
	}

	// Connect the io buffer and open the bitstream
	if(CodingMode == 2) { // Binary coding
		theBinCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.OpenBitStream(Bitstream);
	}

	// Create the database of filenames
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	// Read in each of the Descriptors
	try{
		// Make storage for all the pointers
		DBDescription = new ColorStructureDescriptor*[NoOfElements];

		// Loop over the entries
		for(int ientry=0;ientry<NoOfElements;ientry++) {

			// Read the binary in
			DBDescription[ientry] = new ColorStructureDescriptor();
			DBDescription[ientry]->addref();

			// Set up the interfaces to this D/DSType
			theDescriptorInterface=DBDescription[ientry]->GetInterface();
			if(CodingMode == 2) { // Binary coding
				// Connect Description to CodingScheme
				theBinCSInterface->SetDescriptorInterface(theDescriptorInterface);

				// Now read this in
				theBinCS->StartDecode();
			}
			else { // DDL or BiM coding

				// get next descriptor
				if (ientry==0) {
					//theDDLDescription=theCollectionInterface->GetDescription("MultimediaContent");
					// m15122
					theDDLDescription=theCollectionInterface->GetDescription("Descriptor");
				}
				else {
					//theDDLDescription=theDDLDescription.GetNextSibling("MultimediaContent");
					// m15122
					theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
				}

				// search for correct xsi type
				while(!theDDLDescription.isNull()) {
					theDDLDescription.GetTextAttribute("xsi:type", xsitype);
					//if(xsitype=="ImageType") break;
					//theDDLDescription=theDDLDescription.GetNextSibling("MultimediaContent");
					// m15122
					if(xsitype=="ColorStructureType") break;
					theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
				}

				// DominantColorType not found
				if(theDDLDescription.isNull()) return (unsigned long)-1;

				// Get DDLDescription interface
				theDDLDescriptionInterface=theDDLDescription.GetInterface();

				// Import description from DDL Tree
				theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
			}
		}

	}
	catch(...) {
		fprintf(stderr,"ColorStructureClient::Open: Allocation error2\n");
		return(1);
	}

	// Perform the tidying up
	if(CodingMode == 2) { // Binary coding
		theBinCSInterface->destroy();
		theStreamBuffer.CloseBitStream();
	}
	else {
		theDDLCSInterface->destroy();
	}

	return(0);
}

//----------------------------------------------------------------------------
unsigned long ColorStructureClient::Start(char *Query,int NoOfMatches)
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

	ColorStructureDescriptorInterfaceABC* theDescriptorInterface = NULL;
	ColorStructureDescriptorInterfaceABC* DBDescriptorInterface  = NULL;

	// Set up the extraction tool which will be needed for the query
	ColorStructureExtractionTool*         theExtractionTool      = NULL;
	ColorStructureExtractionInterfaceABC* theExtractionInterface = NULL;

	// Set up the search tool
	ColorStructureSearchTool*         theSearchTool              = NULL;
	ColorStructureSearchInterfaceABC* theSearchInterface         = NULL;

	try{
		// Create media object 
		theMedia = new MultiMedia;


		// obtain media interface
		theMediaInterface=theMedia->GetInterface();

		// Pass it null so that it creates a new descriptor
		theExtractionTool = new ColorStructureExtractionTool(NULL);

		// Obtain the extraction interface from this extraction tool
		theExtractionInterface=theExtractionTool->GetInterface();

		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();

		// Create search tool
		theSearchTool = new ColorStructureSearchTool(theDescriptorInterface);

		// Obtain the search tool interface


		theSearchInterface=theSearchTool->GetInterface();

	}
	catch(...) {
		fprintf(stderr,"ColorStructureClient::Start: Allocation error1\n");

		return(1);
	}





	// Connect media to media data base
	RamDB.SetMedia(theMediaInterface);

	// If more matches are requested than the dbase has, limit it
	if(NoOfMatches>RamDB.GetNoOfElements()) NoOfMatches=RamDB.GetNoOfElements();
	theMatchList.SetNumberOfMatches(NoOfMatches);



	// -------------------- Read in the query ---------------------------------

	if (!RamDB.LoadNamedImage(Query))
	{
		std::cerr << "Unable to load query image " << Query << std::endl;
		exit(1);
	}

	// Tell the extractor that this is the source medium
	theExtractionInterface->SetSourceMedia(theMediaInterface);
	
	// Reinitialise the D Type; maybe also audio D can be extracted here
	theExtractionInterface->InitExtracting();

	// Extract D from media per frame of the input sequence

	theExtractionInterface->StartExtracting();


	/// ------------------- Now do the matching -------------------------------


	// Should only downquantize the query or DB once
	int nOrigRefSize   = DBDescription[0]->GetInterface()->GetSize();
	int nOrigQuerySize = theDescriptorInterface->GetSize();

	for(int ientry=0;ientry<NoOfElements;ientry++) {

		// Get the interface of data base entry
		DBDescriptorInterface=DBDescription[ientry]->GetInterface();

		// Tell the search interface which we're matching to
		theSearchInterface->SetRefDescriptorInterface(DBDescriptorInterface);

		// Test size
		int nQuerySize = theDescriptorInterface->GetSize();
		int nRefSize = DBDescriptorInterface->GetSize();
		// Test that all DB descriptors are the same size
		if (nRefSize != nOrigRefSize)
		{
			std::cerr << "DB Descriptors not of same size!  RefSize[0]: " << 
				nOrigRefSize << " RefSize[" << ientry << "]: " << nRefSize <<
				" OrigQuerySize: " << nOrigQuerySize << std::endl;
		}


		// Down quantize if necessary
		if (nRefSize != nQuerySize)
		{
			if (nQuerySize < nRefSize)
			{
				theExtractionInterface->SetDescriptorInterface(
											DBDescriptorInterface);
				theExtractionInterface->DownQuantHMMD(nQuerySize);
			}
			else
			{
				theExtractionInterface->SetDescriptorInterface(
											theDescriptorInterface);
				theExtractionInterface->DownQuantHMMD(nRefSize);
			}
		}

		// Sort the result according to distance score
		theMatchList.InsertMatch(ientry,theSearchInterface->GetDistance());
	}

	// -------------------- Display the best matches --------------------------
	int rank = 0;
	double prevValue = 0.0;
	for(int i=0;i<NoOfMatches;i++)

	{
		if (theMatchList.GetDBEntryNo(i)== (unsigned long) -1) break;
		fprintf(m_ResultFile, "%s",
			RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->
			GetFilename());

#ifndef FLOAT_DISTANCE
		if(!i) prevValue = theMatchList.GetDistanceValue(i);
		if (theMatchList.GetDistanceValue(i) != prevValue)
		{
			prevValue = theMatchList.GetDistanceValue(i);
			rank++;
		}
		fprintf(m_ResultFile, "\t%d",rank);
#else
		fprintf(m_ResultFile, "\t%e",theMatchList.GetDistanceValue(i));
#endif

		fprintf(m_ResultFile, "\n");
	}


	// Finally tidy up

	theExtractionInterface->destroy();
	theSearchInterface->destroy();

	return(0);
}


//----------------------------------------------------------------------------
unsigned long ColorStructureClient::Stop()
{
	// fprintf(stderr,"ColorStructureClient::Stop: Not Implemented");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ColorStructureClient::Close()
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
const UUID& ColorStructureClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureClient::GetName(void)
{ return myName; }



