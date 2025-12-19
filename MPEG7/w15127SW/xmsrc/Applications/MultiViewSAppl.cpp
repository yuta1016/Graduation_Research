///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2000-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewSAppl.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ServerApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention
#include "CodingSchemes/CodingSchemes.h"          // Naming Convention
#include "Encoders/Encoders.h"               // Naming Convention
#include "Encoders/Encoders.h"               // Naming Convention
#include "Utilities/Utilities.h"              // Naming Convention

using namespace XM;
using namespace std;

extern const char *MPEG7_BASE_NS;
extern const char *MPEG7_XML_SCHEMA;
extern const char *MPEG7_BASE_SCHEMA_LOCATION;

const UUID MultiViewServer::myID = UUID("");
//sth please remove space in string (bad for command line)
const char *MultiViewServer::myName = "MultiView Server";

//=============================================================================
MultiViewServer::MultiViewServer()
{
}

//----------------------------------------------------------------------------
MultiViewServer::~MultiViewServer()
{
}

//----------------------------------------------------------------------------
unsigned long MultiViewServer::Open()
{
	// This server doesn't handle pull
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MultiViewServer::Start(char *ListFile, char *Bitstream)
{
	int CodingMode;

        // image media array for all views
        MultiMedia **media;
	MultiMediaInterfaceABC** mediaInterface;

	// stream data base
	EncoderFileIO theStreamBuffer(0x1000);

	// Image data base 
	MediaDB RamDB;

	// descriptor
	MultiViewDescriptorInterfaceABC *theDescriptorInterface;

	// extraction tool
	MultiViewExtractionTool *theExtractionTool;
	MultiViewExtractionInterfaceABC *theExtractionInterface;
	
	// coding schemes
	MultiViewCS *theCS;
	MultiViewCSInterfaceABC *theCSInterface;

	// create extraction tool
	// get 2D extraction interfaces
	theExtractionTool = new MultiViewExtractionTool(0); // create also descriptor
	theExtractionInterface = theExtractionTool->GetInterface();

	// get MultiView interface
	theDescriptorInterface = theExtractionInterface->GetDescriptorInterface();

	// Get the coding scheme type
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "2"));

	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	
	GenericDS theMpeg7Root, theCollection, theDescriptionUnit, theContent, theMultipleView;
	GenericDSInterfaceABC *theMultipleviewInterface;
#else
	CodingMode = 2;
#endif // USEXML
	
	if (CodingMode == 2)
	{
		// create coding scheme
		theCS = new MultiViewCS(theDescriptorInterface);
		theCSInterface = theCS->GetInterface();
		// connect stream to coding scheme
		theCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
		theStreamBuffer.StartBitStream(Bitstream);

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


	RamDB.CreateDatabase(ListFile);

	////////////////////
	// get interfaces
	// create media
	//sth linked list of MomVop structure
	media = new MultiMedia*[TotNumberOfViews];
	mediaInterface = new MultiMediaInterfaceABC*[TotNumberOfViews];
	{
	  int i;
	  for(i = 0; i < TotNumberOfViews; i++ )
	    {
		media[i] = new MultiMedia;
		mediaInterface[i] = media[i]->GetInterface();
	    }


	  // loop all input media files
	  {
	    int imageIndex;
	    for(imageIndex = 0; imageIndex < RamDB.GetNoOfElements();
		imageIndex++ )
	      {
		char QueryName1[256]; 
		strcpy(QueryName1, RamDB.GotoEntry(imageIndex)->GetFilename());
		// malloc media and load input data

		for( i = 0; i < TotNumberOfViews; i++ )
		  { 
			char QueryName[256]; 
			//strcpy(QueryName, QueryName1);
			//strcat(QueryName, "_");
			//char cpViewNumber[4];
			//_itoa(i+1, cpViewNumber, 10 );
			//strcat(QueryName, cpViewNumber);
			//strcat(QueryName, ".gif");

			theDescriptorInterface->SetVisibleViewFlag(i, true);
			theDescriptorInterface->SetFixedViewsFlag(true);

			// Attempt load without visibility flag	
			sprintf(QueryName, "%s_%d.gif", QueryName1, i+1);
			//sth MomVop supports list of images
			// maybe it would be required to extend MediaIO
			// with this functionality
			RamDB.SetMedia(mediaInterface[i]);
			if(!RamDB.LoadNamedImage(QueryName)) break;


			if(!media[i]) // Attempt load with true visibility flag
			{
				sprintf(QueryName, "%s_%d_t.gif", QueryName1, i+1);
				RamDB.SetMedia(mediaInterface[i]);
				RamDB.LoadNamedImage(QueryName);
				theDescriptorInterface->SetVisibleViewFlag(i, true);
				theDescriptorInterface->SetFixedViewsFlag(false); // if view visibility is given unknown views are assumed
			}

			if(!media[i]) // Attempt load with false visibility flag
			{
				sprintf(QueryName, "%s_%d_f.gif", QueryName1, i+1);
				RamDB.SetMedia(mediaInterface[i]);
				RamDB.LoadNamedImage(QueryName);
				theDescriptorInterface->SetVisibleViewFlag(i, false);
				theDescriptorInterface->SetFixedViewsFlag(false); // if view visibility is given unknown views are assumed
			}

			// next view is not available so set the number of views and break
			if(!media[i])
			{
			  //sth never use stdout; use stderr!!
				cout << "Setting number of Views to " << i << endl;
				theDescriptorInterface->SetNoOfViews(i);
				break;
			}
			
			cout << QueryName << endl;
		  }
		
		
		// all TotalNumberOfViews were used
		if (media[i])
		  {
		  //sth
			cout << "Setting number of Views to " << i << endl;
			theDescriptorInterface->SetNoOfViews(i);
		  }

	  // connect media to extraction tool
	  theExtractionInterface->SetSourceMedia(mediaInterface);
	  
	  // extract descriptor form media
	  //sth
		cout << "Extracting file: " << RamDB.GotoEntry(imageIndex)->GetFilename() << flush;
	  theExtractionInterface->StartExtracting();
		cout << endl;

		if (CodingMode == 2)
		{
			// code descriptor data
			theCSInterface->StartEncode();
		}
		else
		{
#ifdef USEXML
			theContent = theDescriptionUnit.CreateChild("Content");
			theContent.SetAttribute("xsi:type", "ImageType");
			theCollection = theContent.CreateChild("Image");
			theMultipleView=theCollection.CreateChild("MultipleView");
			theMultipleviewInterface=theMultipleView.GetInterface();

			theDescriptorInterface->ExportDDL(theMultipleviewInterface);
#endif // USEXML
		}
	      } // end for(int imageIndex ...)
	  }
	}


  if (CodingMode == 2) // Binary coding
	{
		theCSInterface->destroy();
		theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML
		theDDLCSInterface->StartEncode();
		theDDLCSInterface->destroy();
#endif // USEXML
	}

	theExtractionInterface->destroy();

	// free media array
	{
	  int i;
	  for( i = 0; i < TotNumberOfViews; i++ )
	    mediaInterface[i]->destroy();
	}

	if(media) delete [] media;
	if(mediaInterface) delete [] mediaInterface;
	
	return 0;

}

//----------------------------------------------------------------------------
unsigned long MultiViewServer::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long MultiViewServer::Close()
{
	return 0;
}


//----------------------------------------------------------------------------
const UUID& MultiViewServer::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiViewServer::GetName(void)
{ return myName; }



