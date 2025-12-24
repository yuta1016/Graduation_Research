///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Dongguk University, Chee Sun Won, Dong Kwon Park
// ETRI, Soo-Jun Park
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  EdgeHistCAppl.cpp
//

#ifdef WIN32
#pragma warning(disable : 4786)
#endif // WIN32

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "ClientApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Decoders/Decoders.h"
#include "SearchUtilities/SearchUtilities.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"

using namespace XM;

const UUID EdgeHistogramClient::myID = UUID("");
const char *EdgeHistogramClient::myName = "EdgeHistogramClient";

//=============================================================================
EdgeHistogramClient::EdgeHistogramClient(MediaDB &originalDB):
RamDB(originalDB),
NoOfElements(0),
DBDescription(0)
{
}

//----------------------------------------------------------------------------
EdgeHistogramClient::~EdgeHistogramClient()
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
}

//----------------------------------------------------------------------------
unsigned long EdgeHistogramClient::Open(char *Bitstream)
{
  // Check that the file names are ok
  assert(Bitstream != NULL);

  // -------------------- Initialisation of data ----------------------------

  // This sets up a binary file io with a buffer size of 512bytes
  DecoderFileIO theStreamBuffer(0x1000);

  // This sets up the edge histogram descriptor interface
  EdgeHistogramDescriptorInterfaceABC *theDescriptorInterface = NULL;

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
  EdgeHistogramCS*                     theBinCS               = NULL;
  EdgeHistogramCSInterfaceABC*         theBinCSInterface      = NULL;
  GenericDSCS*                         theDDLCS               = NULL;
  GenericDSCSInterfaceABC*             theDDLCSInterface      = NULL;

  // For reading the xsi:type attributes
  string xsitype;

  // Ok, make new instatiations of these datatypes
  try{
    //select coding scheme type
    CodingMode=atoi(getpara("CodingMode","2"));//default is Binary

    if (CodingMode == 2) { // Binary coding
      // Now create the binary coding scheme
      theBinCS = new EdgeHistogramCS();

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
    fprintf(stderr,"EdgeHistogramClient::Open: Allocation error\n");
    return(1);
  }

  // Connect the io buffer and open the bitstream
  if (CodingMode == 2) { // Binary coding
    theBinCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);
    theStreamBuffer.OpenBitStream(Bitstream);
  }

  // Create the database of filenames
  //RamDB.CreateDatabase(ListFile);
  NoOfElements=RamDB.GetNoOfElements();
  // Read in each of the Descriptors
  try{
    // Make storage for all the pointers
    DBDescription = new EdgeHistogramDescriptor*[NoOfElements];

    // Loop over the entries
    for(int ientry=0;ientry<NoOfElements;ientry++) {

      // Read the binary in
      DBDescription[ientry] = new EdgeHistogramDescriptor();
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
	  if(xsitype=="EdgeHistogramType") break;
	  theDDLDescription=theDDLDescription.GetNextSibling("Descriptor");
	}

	// DummyType not found
	if (theDDLDescription.isNull()) return (unsigned long)-1;

	// Get DDLDescription interface
	theDDLDescriptionInterface=theDDLDescription.GetInterface();

	// Import description from DDL Tree
	theDescriptorInterface->ImportDDL(theDDLDescriptionInterface);
      }
    }
  }
  catch(...) {
    fprintf(stderr,"EdgeHistogramClient::Open: Allocation error2\n");
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
unsigned long EdgeHistogramClient::Start(std::vector<DATEORD> &dateOrdVec, double*** ptrToDists)
{
 

  EdgeHistogramDescriptorInterfaceABC* thisQDescriptorInterface = NULL;
  EdgeHistogramDescriptorInterfaceABC* DBDescriptorInterface  = NULL;


  // Set up the search tool
  EdgeHistogramSearchTool*         theSearchTool              = NULL;
  EdgeHistogramSearchInterfaceABC* theSearchInterface         = NULL;

  try{
 

    // Create search tool
    theSearchTool = new EdgeHistogramSearchTool();

    // Obtain the search tool interface
    theSearchInterface=theSearchTool->GetInterface();
  }
  catch(...) {
    fprintf(stderr,"EdgeHistogramClient::Start: Allocation error1\n");
    return(1);
  }

  //RJO'C ..... here's the business end:
  //Match each record against the next, next+1 and next+2 (1st, 2nd, 3rd order distance)

  FILE *distFile[3]; //debug parts, make sure the distances pop out ok)
  distFile[0] = fopen("EH_1.txt","w");
  distFile[1] = fopen("EH_2.txt","w");
  distFile[2] = fopen("EH_3.txt","w");

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
unsigned long EdgeHistogramClient::Stop()
{
  // fprintf(stderr,"EdgeHistogramClient::Stop: Not Implemented");
  return 0;
}
//----------------------------------------------------------------------------
unsigned long EdgeHistogramClient::Close()
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
const UUID& EdgeHistogramClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramClient::GetName(void)
{ return myName; }
