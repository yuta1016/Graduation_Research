///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// Applicable File Name:  ShapeVariationSAppl.cpp
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Media/Media.h"
#include "Utilities/Utilities.h"
#include "AddressLib/momusys.h"
#include "AddressLib/vopio.h"

extern const char *MPEG7_AMD1_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_AMD1_SCHEMA_LOCATION;

using namespace XM;

const UUID ShapeVariationServer::myID = UUID("");
const char *ShapeVariationServer::myName = "ShapeVariationServer";

//=============================================================================
ShapeVariationServer::ShapeVariationServer()
{
}

//----------------------------------------------------------------------------
ShapeVariationServer::~ShapeVariationServer()
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


unsigned long ShapeVariationServer::Start(char *ListFile, char *Bitstream)
{
	// Check that the file names are ok
	assert(ListFile  != NULL);
	assert(Bitstream != NULL);
	
	// -------------------- Initialisation of data ----------------------------
	
	// Media data base
	MediaDB RamDB;
	
	// Media
	//MultiMedia *theMedia;
	//MultiMediaInterfaceABC *theMediaInterface;
	MultiMedia **media;
	MultiMediaInterfaceABC** mediaInterface;
	
	// This sets up a binary file io with a buffer size of 512bytes (I think)
	EncoderFileIO theStreamBuffer(0x1000);
	
	// This sets up the dummy type descriptor interface
	ShapeVariationDescriptorInterfaceABC* theDescriptorInterface          = NULL;
	
	// This descriptors are used for DDL/BiM based coding
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface                      = NULL;
	GenericDS theMpeg7Root;
	GenericDS theCollection;
	GenericDSInterfaceABC *theCollectionInterface                    = NULL;
	
	// This creates the extraction tool and extraction tool interface. Set
	// to null initially for safety.
	ShapeVariationExtractionTool*         theExtractionTool               = NULL;
	ShapeVariationExtractionInterfaceABC* theExtractionInterface          = NULL;
	
	// This sets up the coding scheme for this D/DType and the interface
	int CodingMode;  // 0:DDL, 1: BiM, 2:Binary
	ShapeVariationCS*                     theBinCS                        = NULL;
	ShapeVariationCSInterfaceABC*         theBinCSInterface               = NULL;
	GenericDSCS*                     theDDLCS                        = NULL;
	GenericDSCSInterfaceABC*         theDDLCSInterface               = NULL;
	
	// Ok, make new instatiations of these datatypes
	try{
		// Create media object 
		//theMedia = new MultiMedia;
		
		// obtain media interface
		//theMediaInterface=theMedia->GetInterface();
		
		// Pass it null so that it creates a new descriptor
		theExtractionTool = new ShapeVariationExtractionTool(NULL);
		
		// Obtain the extraction interface from this extraction tool
		theExtractionInterface=theExtractionTool->GetInterface();
		
		// Obtain the descriptor interface
		theDescriptorInterface=
			theExtractionInterface->GetDescriptorInterface();
		
		//select coding scheme type
#ifdef USEXML
		CodingMode=atoi(getpara("CodingMode","2"));//default is Binary
#else
		CodingMode=2; //Binary mode
#endif
		
		if (CodingMode == 2) { // Binary coding
			// Now create the binary coding scheme
			theBinCS = new ShapeVariationCS(theDescriptorInterface);
			
			// Obtain the interface
			theBinCSInterface=theBinCS->GetInterface();
		}
		else { // DDL or BiM coding
			// Create the XML document
			theDocument = GenericDS::CreateDescriptionRoot();
			theDocumentInterface=theDocument.GetInterface();
			
			// Create the Mpeg7Root element
			theMpeg7Root=theDocumentInterface->CreateChild("Mpeg7");
			theMpeg7Root.SetAttribute("xmlns", MPEG7_AMD1_NS);
			theMpeg7Root.SetAttribute("xmlns:xsi", MPEG7_XML_SCHEMA);
			theMpeg7Root.SetAttribute("xsi:schemaLocation", MPEG7_AMD1_SCHEMA_LOCATION);
			
			// Create the Collection element for the descriptors
			theCollection=theMpeg7Root.CreateChild("DescriptionUnit");
			theCollection.SetAttribute("xsi:type", "DescriptorCollectionType");
			theCollectionInterface=theCollection.GetInterface();
			
			// Now create the XML parser based coding scheme
			theDDLCS = new GenericDSCS(theDocumentInterface);
			
			// Obtain the interface
			theDDLCSInterface=theDDLCS->GetInterface();
			
			// Connect DDL file to coding scheme
			theDDLCSInterface->SetDecoderFileName(Bitstream);
		}
	}
	catch(...) {
		fprintf(stderr,"ShapeVariationServer::Start: Allocation error\n");
		return(1);
	}
	
	// All of the pointers should now be non-null so check them
	assert(theDescriptorInterface          != NULL);
	//assert(theMedia                        != NULL);
	//assert(theMediaInterface               != NULL);
	assert(theExtractionTool               != NULL);
	assert(theExtractionInterface          != NULL);
	if (CodingMode == 2) { // Binary coding
		assert(theBinCS                      != NULL);
		assert(theBinCSInterface             != NULL);
	}
	else { // // DDL or BiM coding
		assert(theDocumentInterface          != NULL);
		assert(theCollectionInterface        != NULL);
		assert(theDDLCS                      != NULL);
		assert(theDDLCSInterface             != NULL);
	}
	
	// Lastly, tell the dummy type about the output stream
	if (CodingMode == 2) { // Binary coding
		theBinCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);
	}
	else { // DDL or BiM coding
		theDDLCSInterface->SetEncoderFileName(Bitstream);
	}
	
	// ---------------------- Process the files -----------------------------
	
	// Connect media to media data base
	//RamDB.SetMedia(theMediaInterface);
	
	// Create a database of all the files to process
	RamDB.CreateDatabase(ListFile);


	int NoOfSequence = RamDB.GetNoOfElements();

	int i,j;
	char SequenceName[256];
	char* tmp;
	char str[20];
	char group[20];	
	int set;
	//int frame;
	int total;
	//char filename[256];

	media = new MultiMedia*[200];
	mediaInterface = new MultiMediaInterfaceABC*[200];

	for( i = 0 ; i < 200 ; i++ )
	{
		media[i] = new MultiMedia;
		mediaInterface[i] = media[i]->GetInterface();
	}

	for( i = 0 ; i < NoOfSequence ; i++ )
	{
		
		strcpy(SequenceName, RamDB.GotoEntry(i)->GetFilename());
		tmp = strstr( SequenceName, ".gif" );
		tmp -= 15;

		memset(group, 0, 20);
		strncpy(group, tmp, 6);
		memset(str, 0, 20);
		strncpy(str, tmp+7, 2);
		set = atoi(str);
		memset(str, 0, 20);
		strncpy(str, tmp+13, 2);
		total = atoi(str);
		
		//tmp = strstr(SequenceName, "-")+5;
		tmp = strstr( SequenceName, ".gif" );
		tmp -= 8;

		//media = new MultiMedia*[total];
		//mediaInterface = new MultiMediaInterfaceABC*[total];
	
		for( j = 0 ; j < total ; j++ )
		{
			sprintf(tmp, "%02d-%02d-%02d.gif", set, j+1, total);
			
			//media[j] = new MultiMedia;
			//mediaInterface[j] = media[j]->GetInterface();

			RamDB.SetMedia(mediaInterface[j]);
			if(!RamDB.LoadNamedImage(SequenceName)) 
			{
				fprintf(stderr,"%s file read error\n",SequenceName);
				break;
			}
		}
		theExtractionInterface->SetSourceMedia(mediaInterface, j);
		
		theExtractionInterface->StartExtracting();

		for( j = 0 ; j < total ; j++ )
		{
			media[j]->FreeImage();
		}
		//delete [] media;
		//delete [] mediaInterface;

		if (CodingMode == 2) { // Binary coding
			// Write the D/DSType to binary
			theBinCSInterface->StartEncode();
		}
		else { // DDL or BiM coding
			
			// Export description to Collection DDL Tree
			theDescriptorInterface->ExportDDL(theCollectionInterface);
		}
		
	} // Onto the next source file
	
	delete [] media;
	delete [] mediaInterface;

	// Tidy up all the memory allocations
	theExtractionInterface->destroy();
	
	if (CodingMode == 2) { // Binary coding
		theBinCSInterface->destroy();		
		// Shut down the output stream
		theStreamBuffer.CloseBitStream();
	}
	else {
		// Write the D/DSType to DDL
		theDDLCSInterface->StartEncode();
		
		theDDLCSInterface->destroy();
	}
	
	// All done
	return(0);
}


//----------------------------------------------------------------------------
unsigned long ShapeVariationServer::Stop()
{
	fprintf(stderr,"ShapeVariationServer Stopped\n");
	return(0);
}


//----------------------------------------------------------------------------
unsigned long ShapeVariationServer::Open()
{
	return(0);
}


//----------------------------------------------------------------------------
unsigned long ShapeVariationServer::Close()
{
	fprintf(stderr, "ShapeVariationServer Closed\n");
	return(0);
}


//----------------------------------------------------------------------------
const UUID& ShapeVariationServer::GetObjectID(void)
{ 
	return(myID);
}

//----------------------------------------------------------------------------
const char *ShapeVariationServer::GetName(void)
{ 
	return(myName); 
}



