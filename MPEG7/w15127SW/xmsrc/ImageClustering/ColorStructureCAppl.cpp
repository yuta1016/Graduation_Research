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
// Copyright (c) 2000
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

#ifdef WIN32
#pragma warning(disable : 4786)
#endif // WIN32

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "ClientApplications.h"
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

//=============================================================================
ColorStructureClient::ColorStructureClient(MediaDB &originalDB):
RamDB(originalDB),
NoOfElements(0),
DBDescription(0)
{
	m_ResultFile = stdout;
}

//----------------------------------------------------------------------------
ColorStructureClient::~ColorStructureClient()
{
	// Free memories
	//RamDB.FreeDatabase();
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
unsigned long ColorStructureClient::Open(char *Bitstream)
{
	// Check that the file names are ok

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
			//theCollection = theMpeg7Root.GetDescription("ContentDescription"); 
			theCollection = theMpeg7Root.GetDescription("Description"); //RJOC

			// Collection not found
			if(theCollection.isNull()) {
				fprintf(stderr, "Description not found\n");
				return (unsigned long)-1;
			}
			theCollection.GetTextAttribute("xsi:type", xsitype);
			if(xsitype != "ContentEntityType") {
				fprintf(stderr, "ContentEntityType not found\n");
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
	//RamDB.CreateDatabase(ListFile);
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

				// get next descriptor (the highest level of the descriptor is a "MultimediaContent" element
				if (ientry==0) {
					theDDLDescription=theCollectionInterface->GetDescription("MultimediaContent");
				}
				else {
					theDDLDescription=theDDLDescription.GetNextSibling("MultimediaContent");
				}

				// search for correct xsi type
				while(!theDDLDescription.isNull()) {
					theDDLDescription.GetTextAttribute("xsi:type", xsitype);
					if(xsitype=="ImageType") break;
					theDDLDescription=theDDLDescription.GetNextSibling("MultimediaContent");
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
unsigned long ColorStructureClient::Start(std::vector<DATEORD> &dateOrdVec, double*** ptrToDists)
{

	//Initialise everything
	ColorStructureDescriptorInterfaceABC* thisQDescriptorInterface = NULL;
	ColorStructureDescriptorInterfaceABC* DBDescriptorInterface  = NULL;


	// Set up the search tool
	ColorStructureSearchTool*         theSearchTool              = NULL;
	ColorStructureSearchInterfaceABC* theSearchInterface         = NULL;

	try{

		theSearchTool = new ColorStructureSearchTool();

		// Obtain the search tool interface
		theSearchInterface=theSearchTool->GetInterface();

	}
	catch(...) {
		fprintf(stderr,"ColorStructureClient::Start: Allocation error1\n");

		exit(1);
	}


  //RJO'C ..... here's the business end:
  //Match each record against the next, next+1 and next+2 (1st, 2nd, 3rd order distance)

  FILE *distFile[3]; //debug parts, make sure the distances pop out ok)
  distFile[0] = fopen("CS_1.txt","w");
  distFile[1] = fopen("CS_2.txt","w");
  distFile[2] = fopen("CS_3.txt","w");

  int QEntry, REntry, i;

  for(i=0;i<3;i++)
	  if (!distFile[i]) cout<<"Trouble with distfile "<<i<<endl;
  
  double **dists;
  dists = new double*[3];
  *ptrToDists = dists;
  dists[0] = new double[NoOfElements*3];
  for(i=1;i<3; i++)
	  dists[i] = (*dists)+NoOfElements*i;
  
  for(QEntry=0; QEntry<NoOfElements; QEntry++)
  {
	  thisQDescriptorInterface=DBDescription[dateOrdVec[QEntry].index]->GetInterface();
	  theSearchInterface->SetQueryDescriptorInterface(thisQDescriptorInterface);
	  for(i=0; i<3; i++)
	  {
		  REntry=QEntry+i+1;
		  if(REntry>=NoOfElements) break;
		  DBDescriptorInterface=DBDescription[dateOrdVec[REntry].index]->GetInterface();
		  theSearchInterface->SetRefDescriptorInterface(DBDescriptorInterface);
		  dists[i][QEntry]=theSearchInterface->GetDistance();
		  fprintf(distFile[i],"%f\n",dists[i][QEntry]);
	  }
  }

  fclose(distFile[0]);
  fclose(distFile[1]);
  fclose(distFile[2]);


	theSearchInterface->destroy();

	return(NoOfElements);
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
	//RamDB.FreeDatabase();
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



