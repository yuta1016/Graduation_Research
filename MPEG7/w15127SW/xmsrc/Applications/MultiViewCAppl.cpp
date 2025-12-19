///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI 
// K.Mueller
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
// Applicable File Name:  MultiViewCAppl.cpp
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fstream>
#include <math.h>
#include <momusys.h>
#include <vopio.h>

#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            // Naming Convention
#include "ExtractionUtilities/ExtractionUtilities.h"    // Naming Convention
#include "SearchUtilities/SearchUtilities.h"        // Naming Convention
#include "CodingSchemes/CodingSchemes.h"
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"
#include "Utilities/Utilities.h"              // Naming Convention

using namespace XM;
using namespace std;

//sth remove spaces in string
const UUID MultiViewClient::myID = UUID("");
const char *MultiViewClient::myName = "MultiView Client";

//=============================================================================
MultiViewClient::MultiViewClient()
: RamDB()
, NoOfElements(0)
, DBMultiViewDescriptor(0)
{
}

//----------------------------------------------------------------------------
MultiViewClient::~MultiViewClient()
{
}

//----------------------------------------------------------------------------
unsigned long MultiViewClient::Open(char *ListFile, char *Bitstream)
{
	// Stream data base
	DecoderFileIO theStreamBuffer(0xFFFF);
	
	// Descriptor interface
	MultiViewDescriptorInterfaceABC *theMultiViewDescriptorInterface;//sth rename to theDescriptorInterface
	
	// Coding scheme
	MultiViewCS *theMultiViewCS;//sth rename to theCS
	MultiViewCSInterfaceABC *theMultiViewCSInterface;//sth rename to theCSInterface

	// Select coding scheme type
	int CodingMode;
#ifdef USEXML
	CodingMode = atoi(getpara("CodingMode", "2"));
#else
	CodingMode = 2;
#endif // USEXML
	
#ifdef USEXML
	GenericDS theDocument;
	GenericDSInterfaceABC *theDocumentInterface;
	GenericDSCS *theDDLCS;
	GenericDSCSInterfaceABC *theDDLCSInterface;
	GenericDS theMpeg7Root, theCollection;
	GenericDSInterfaceABC *theCollectionInterface;
	GenericDS theDDLDescription;
#endif // USEXML

	if (CodingMode == 2)
	{
		/* allocate memory for coding scheme*/
		theMultiViewCS = new MultiViewCS;

		/* get coding scheme interfaces*/
		theMultiViewCSInterface = theMultiViewCS->GetInterface();

		/* connect stream to coding scheme*/
		theMultiViewCSInterface->SetDecoderStreamBuffer(&theStreamBuffer);

		/* open bit stream*/
		theStreamBuffer.OpenBitStream(Bitstream);
	}
	else
	{
#ifdef USEXML
		// Create the XML document and obtain the interface
		theDocument = GenericDS::CreateDescriptionRoot();
		theDocumentInterface = theDocument.GetInterface();
		
		// Create the XML parser based coding scheme and obtain the interface
		theDDLCS = new GenericDSCS(theDocumentInterface);
		theDDLCSInterface = theDDLCS->GetInterface();
		
		// Connect the DDL to the coding scheme
		theDDLCSInterface->SetDecoderFileName(Bitstream);
		
		// Decode the DDL file
		theDDLCSInterface->StartDecode();
		
		// Parse the top level elements
		theMpeg7Root = theDocumentInterface->GetDescription("Mpeg7");
		
		// Mpeg7 root not found
		if (theMpeg7Root.isNull())
		{
			fprintf(stderr, "Top level is not Mpeg7\n");
			return (unsigned long)-1;
		}
		
		// Get the collection element for the descriptors
		theCollection = theMpeg7Root.GetDescription("DescriptionUnit");
		
		// Collection not found
		if (theCollection.isNull())
		{
			fprintf(stderr, "DescriptionUnit not found\n");
			return (unsigned long)-1;
		}
		
		string xsitype;
		theCollection.GetTextAttribute("xsi:type", xsitype);
		if (xsitype != "ContentCollectionType")
		{
			fprintf(stderr, "ContentCollectionType not found\n");
			return (unsigned long)-1;
		}
		
		// Get the collection in terface
		theCollectionInterface = theCollection.GetInterface();
#endif // USEXML
	}

	/* read filename data base*/
	RamDB.CreateDatabase(ListFile);
	NoOfElements=RamDB.GetNoOfElements();

	
	// create in table with references to descriptors in data base
	DBMultiViewDescriptor = new MultiViewDescriptor* [NoOfElements];//KM
	
	{
	  int i;
	  for (i = 0; i < NoOfElements; i++)//KM
	    {

	      // allocate memory for descriptor
	      DBMultiViewDescriptor[i] = new MultiViewDescriptor(0);
	      DBMultiViewDescriptor[i]->addref();

	      // get MultiView interface
	      theMultiViewDescriptorInterface =
		DBMultiViewDescriptor[i]->GetInterface();

	      if (CodingMode == 2)
		{
		  // connect descriptors to coding schemes
		  theMultiViewCSInterface->
		    SetDescriptorInterface(theMultiViewDescriptorInterface);

		  //decode descriptor
		  theMultiViewCSInterface->StartDecode();
		}
	      else
		{
#ifdef USEXML
		  // Get the next descriptor
	      string xsitype;
	      GenericDS MultipleViewRoot=theCollectionInterface->GetDescription("Content");
	      MultipleViewRoot.GetTextAttribute("xsi:type",xsitype);
		  while(xsitype!="ImageType" && !MultipleViewRoot.isNull()){
			  MultipleViewRoot=MultipleViewRoot.GetNextSibling("Content");
			  MultipleViewRoot.GetTextAttribute("xsi:type",xsitype);
	      }
		  if(MultipleViewRoot.isNull())
			  return (unsigned long)-1;
		  GenericDS theImage = MultipleViewRoot.GetDescription("Image");
		  if(theImage.isNull())
			  return (unsigned long)-1;
		  GenericDS theMultipleView = theImage.GetDescription("MultipleView");
		  if(theMultipleView.isNull())
			  return (unsigned long)-1;
	      GenericDSInterfaceABC *DSinterface = theMultipleView.GetInterface();
		  // Import the description from the DDL tree
		  theMultiViewDescriptorInterface->
		    ImportDDL(DSinterface);

	      theCollection.RemoveChild(MultipleViewRoot);
#endif // USEXML
		}
	    }
	}

	if (CodingMode == 2)
	{
		theMultiViewCSInterface->destroy();
		theStreamBuffer.CloseBitStream();
	}
	else
	{
#ifdef USEXML
		theDDLCSInterface->destroy();
#endif // USEXML
	}
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MultiViewClient::Start(char *Query, int NoOfMatches, bool VisibleViewsOnly)
{

        // image media
        MultiMedia **media;
	MultiMediaInterfaceABC** mediaInterface;

	// list with best matches
	MatchList theMatchList;

	// descriptor
	MultiViewDescriptorInterfaceABC *theMultiViewDescriptorInterface;//sth rename to theDescriptorInterface

	MultiViewDescriptorInterfaceABC *DBMultiViewDescriptorInterface;//sth rename to DBDescriptorInterface

	// extraction tool
	MultiViewExtractionTool *theMultiViewExtractionTool;//sth rename to theExtraction
	MultiViewExtractionInterfaceABC *theMultiViewExtractionInterface;//sth rename to theExtractionInterface
	
	// search tool
	MultiViewSearchTool *theMultiViewSearchTool;//sth rename to theSearch
	MultiViewSearchInterfaceABC *theMultiViewSearchInterface;//sth rename to theSearchInterface

	///////////////////
	// get interfaces

	// create media
	//sth using linked list mechanism from Vop could solve to preserve
	//the interface
	media = new MultiMedia*[TotNumberOfViews];
	mediaInterface = new MultiMediaInterfaceABC*[TotNumberOfViews];
	{
	  int i;
	  for(i = 0; i < TotNumberOfViews; i++ )
	    {
	      media[i] = new MultiMedia;
	      mediaInterface[i] = media[i]->GetInterface();
	    }
	}


	// get shape 3D extraction interface
	theMultiViewExtractionTool = new MultiViewExtractionTool(0); // create also descriptor
	theMultiViewExtractionInterface = theMultiViewExtractionTool->GetInterface();

	// get descriptor interface
	theMultiViewDescriptorInterface = theMultiViewExtractionInterface->GetDescriptorInterface();

	// get search interface
	theMultiViewSearchTool = new MultiViewSearchTool(theMultiViewDescriptorInterface);
	theMultiViewSearchInterface = theMultiViewSearchTool->GetInterface();

	// connect query descriptor to search tool
	theMultiViewSearchInterface->SetQueryDescriptorInterface(theMultiViewDescriptorInterface);


	// prepare match list
	if (NoOfMatches > RamDB.GetNoOfElements())//KM
		NoOfMatches = RamDB.GetNoOfElements();//KM;

	//sth to stdout not permitted
	cerr << " NoOf3D-Objects = " << NoOfElements 
			 << " NoOfMatches = " << NoOfMatches << endl;//KM

	theMatchList.SetNumberOfMatches(NoOfMatches);

	// malloc media and load input data for all views
	{
	  int i;
	  for( i = 0; i < 7; i++ ) {
	    char QueryName[256]; 
		//strcpy(QueryName, Query);
		//strcat(QueryName, "_");
		//char cpViewNumber[4];
		//_itoa(i+1, cpViewNumber, 10 );
		//strcat(QueryName, cpViewNumber);
		//strcat(QueryName, ".gif");
		sprintf(QueryName, "%s_%d.gif", Query, i+1);
		RamDB.SetMedia(mediaInterface[i]);
		RamDB.LoadNamedImage(QueryName);

		if(!media[i]) // Attempt load with true visibility flag
		{
			sprintf(QueryName, "%s_%d_t.gif", Query, i+1);
			RamDB.SetMedia(mediaInterface[i]);
			RamDB.LoadNamedImage(QueryName);
		}

		if(!media[i]) // Attempt load with false visibility flag
		{
			sprintf(QueryName, "%s_%d_f.gif", Query, i+1);
			RamDB.SetMedia(mediaInterface[i]);
			RamDB.LoadNamedImage(QueryName);
		}

		if(!media[i]) // can't load! no more views
		{
			cerr << "Setting number of Query Views to " << i << endl;
			theMultiViewDescriptorInterface->SetNoOfViews(i);
			break;
		}
	  }
	}

	// connect media to extraction tool
	theMultiViewExtractionInterface->SetSourceMedia(mediaInterface);

	// extract descriptor from media
	//sth dont write to stdout
	cout << "Extracted: " << Query;
	theMultiViewExtractionInterface->StartExtracting();
	cout << endl;

	
	{
	  int i;
	  for ( i = 0; i < NoOfElements; i++ )//KM
	    {
		cout << i << "/" << NoOfElements << ": " //KM
			   << RamDB.GotoEntry(i)->GetFilename() << endl; //KM

		// get contour shape interface
		DBMultiViewDescriptorInterface =	DBMultiViewDescriptor[i]->GetInterface();
		
		// connect query descriptor to search tool
		theMultiViewSearchInterface->SetRefDescriptorInterface(DBMultiViewDescriptorInterface);
		
		// sort result into list of best matches
		if(VisibleViewsOnly)
		{
			// do search with visible flags
			theMatchList.InsertMatch(i, theMultiViewSearchInterface->GetDistanceVIL(VisibleViewsOnly));//JC
		}
		else
		{
			// the HHI method need to be modified to handle visible flags
			theMatchList.InsertMatch(i, theMultiViewSearchInterface->GetDistanceHHI());//KM
		}
	    }
	}

	// output the search results
	{
	  int i;
	  for ( i = 0; i < NoOfMatches; i++)
	    {
		if (theMatchList.GetDBEntryNo(i) == (unsigned long)-1) break;
		printf("%s",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
		fflush(stdout);
		printf("\t%g", theMatchList.GetDistanceValue(i));
		printf("\n");
	    }
	}
		
	theMultiViewSearchInterface->destroy();
	theMultiViewExtractionInterface->destroy();

	
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

//sth delimiter :-(
//sth dont use propritaery interfaces, maybe you can use a parameter from the parameter file
unsigned long MultiViewClient::Start2D(char *Query, int NoOfMatches, bool VisibleViewsOnly)
{

        // image media
	MultiMedia* media = new MultiMedia;
	MultiMediaInterfaceABC *mediaInterface;
	mediaInterface = media->GetInterface();
	//media = new MultiMediaInterfaceABC*[TotNumberOfViews];
	// list with best matches
	MatchList theMatchList;

        // descriptor
//	MultiViewDescriptorInterfaceABC *theMultiViewDescriptorInterface;

	MultiViewDescriptorInterfaceABC *DBMultiViewDescriptorInterface;

	// extraction tool
	//MultiViewExtractionTool *theMultiViewExtractionTool;
	//MultiViewExtractionInterfaceABC *theMultiViewExtractionInterface;
	SUB_EXTRACTION_TOOL* theShape2DExtractionTool;
	SUB_EXTRACTION_INTERFACE_ABC* theShape2DExtractionInterface;
	
	// search tool
	MultiViewSearchTool *theMultiViewSearchTool;
	MultiViewSearchInterfaceABC *theMultiViewSearchInterface;

	///////////////////
	// get interfaces
	// get shape 3D extraction interface
	//theMultiViewExtractionTool = new MultiViewExtractionTool(0); // create also descriptor
	//theMultiViewExtractionInterface = theMultiViewExtractionTool->GetInterface();
	theShape2DExtractionTool = new SUB_EXTRACTION_TOOL(0); // create also descriptor
	theShape2DExtractionInterface = theShape2DExtractionTool->GetInterface();
	SUB_DESCRIPTOR_INTERFACE_ABC* theShape2DDescriptorInterface;
	theShape2DDescriptorInterface = theShape2DExtractionTool->GetDescriptorInterface();
	// get descriptor interface
	//theMultiViewDescriptorInterface = theMultiViewExtractionInterface->GetDescriptorInterface();

	// get search interface
	//theMultiViewSearchTool = new MultiViewSearchTool(theMultiViewDescriptorInterface);
	theMultiViewSearchTool = new MultiViewSearchTool(0);
	theMultiViewSearchInterface = theMultiViewSearchTool->GetInterface();

	// connect query descriptor to search tool
  theMultiViewSearchInterface->Get2DSearchInterface()->SetQueryDescriptorInterface(theShape2DDescriptorInterface);


	// prepare match list
	if (NoOfMatches > RamDB.GetNoOfElements())//KM
		NoOfMatches = RamDB.GetNoOfElements();//KM;

	//sth
	cout << " NoOf3D-Objects = " << NoOfElements 
			 << " NoOfMatches = " << NoOfMatches << endl;//KM

	theMatchList.SetNumberOfMatches(NoOfMatches);

	// malloc media and load input data for all views
	//char QueryName[256]; 
	//strcpy(QueryName, Query);
	//strcat(QueryName, ".gif");
	RamDB.SetMedia(mediaInterface);

	if (!RamDB.LoadNamedImage(Query)) {
		cerr << " No media available " << endl;
		theMultiViewSearchInterface->destroy();
		theShape2DExtractionInterface->destroy();
		mediaInterface->destroy();
		return 0;
	}

	// connect media to extraction tool
	theShape2DExtractionInterface->SetSourceMedia(mediaInterface);

	// extract descriptor from media
	//sth
	cout << "Extracted: " << Query;
	theShape2DExtractionInterface->StartExtracting();
	cout << endl;

	mediaInterface->destroy();

	{
	  int i;
	  for ( i = 0; i < NoOfElements; i++ )//KM
	    {
	      //sth
	      cout << i << "/" << NoOfElements << ": " //KM
		   << RamDB.GotoEntry(i)->GetFilename() << endl;//KM
	      // get contour shape interface
	      DBMultiViewDescriptorInterface =
		DBMultiViewDescriptor[i]->GetInterface();
		
	      // connect query descriptor to search tool
	      theMultiViewSearchInterface->
		SetRefDescriptorInterface(DBMultiViewDescriptorInterface);
		
	      // sort result into list of best matches
	      theMatchList.InsertMatch(i, theMultiViewSearchInterface->
				       GetDistance2D(VisibleViewsOnly));
	    }
	}

	{
	  int i;
	  for ( i = 0; i < NoOfMatches; i++)
	    {
		if (theMatchList.GetDBEntryNo(i) == (unsigned long)-1) break;
		printf("%s",RamDB.GotoEntry(theMatchList.GetDBEntryNo(i))->GetFilename());
		fflush(stdout);
		printf("\t%g", theMatchList.GetDistanceValue(i));
		printf("\n");
	    }
	}
		
	theMultiViewSearchInterface->destroy();
	theShape2DExtractionInterface->destroy();
		
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MultiViewClient::Stop()
{
	return 0;
}
//----------------------------------------------------------------------------
unsigned long MultiViewClient::Close()
{
	
	// free memories
	RamDB.FreeDatabase();
	{
	  int i;
	  for (i=0; i<NoOfElements; i++) {
	    if (DBMultiViewDescriptor[i]) 	DBMultiViewDescriptor[i]->release();
	  }
	}
	
	if(DBMultiViewDescriptor) delete [] DBMultiViewDescriptor;
	NoOfElements =0;
	DBMultiViewDescriptor =0;
	
	return 0;

}


//----------------------------------------------------------------------------
const UUID& MultiViewClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiViewClient::GetName(void)
{ return myName; }



