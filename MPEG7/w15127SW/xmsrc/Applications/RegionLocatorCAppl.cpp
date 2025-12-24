///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, F. Sanahuja
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RregionLocatorCAppl.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <vopio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif


#include "Applications/ClientApplications.h"
#include "Descriptors/Descriptors.h"            /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"    /* Naming Convention */
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Decoders/Decoders.h"               /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
#include "RegionLocatorCAppl.h"

using namespace XM;
using namespace std;

const UUID RegionLocatorClient::myID = UUID("");
const char *RegionLocatorClient::myName = "Region Locator Client";

//=============================================================================
RegionLocatorClient::RegionLocatorClient():
RamDB(),
DBDescriptor(0)

{
}

//----------------------------------------------------------------------------
RegionLocatorClient::~RegionLocatorClient()
{
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorClient::Open(char *ListFile, char *Bitstream)
{
	#ifdef USEXML
		GenericDS theDocument;
		GenericDSInterfaceABC *theDocumentInterface;
		GenericDSCS *theDDLCS;
		GenericDSCSInterfaceABC *theDDLCSInterface;
		GenericDS theMpeg7Root, theCollection;
		GenericDSInterfaceABC *theCollectionInterface;
		GenericDS theDDLDescription;

		string xsitype;
	#endif // USEXML

        // Determine binary or XML format
        bool bBinary = true;
	if (strlen(Bitstream) >= 4 && !strcmp(&Bitstream[strlen(Bitstream)-4], ".xml"))
	  bBinary = false;

	/* stream data base*/
	DecoderFileIO *theStreamBuffer;
	if (bBinary)
	  theStreamBuffer= new DecoderFileIO(0xFFFF);

        /* descriptor and sub-descriptor*/
	RegionLocatorDescriptorInterfaceABC
	  *theRegionLocatorDescriptorInterface;

	/* coding schemes*/
	RegionLocatorCS *theRegionLocatorCS;
	RegionLocatorCSInterfaceABC *theRegionLocatorCSInterface;

	/* allocate memory for coding scheme*/
	theRegionLocatorCS = new RegionLocatorCS;

	/* get coding scheme interfaces*/
	theRegionLocatorCSInterface=theRegionLocatorCS->GetInterface();

	// Connect and open the io buffer, or connect the XML file
	if (bBinary)
	{

	  /* connect stream to coding scheme*/
	  theRegionLocatorCSInterface->SetDecoderStreamBuffer(theStreamBuffer);
	  
	  /* open bit stream*/
	  theStreamBuffer->OpenBitStream(Bitstream);
	  
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
	m_nInputElements=RamDB.GetNoOfElements();

	/* create in memory decompressed database*/
	/* allocate memeories*/

	DBDescriptor =
	  new RegionLocatorDescriptor* [m_nInputElements];
	m_DLength= new int [m_nInputElements];
	m_AreaError= new float [m_nInputElements];

	for (int i=0; i < m_nInputElements; i++) {
            
	    /* allocate memory for descriptor*/
	    DBDescriptor[i] =
	      new RegionLocatorDescriptor();
	    DBDescriptor[i]->addref();

            /* get descriptor interface*/
            theRegionLocatorDescriptorInterface=
                DBDescriptor[i]->GetInterface();
            
            /* connect descriptor to coding schemes*/
            theRegionLocatorCSInterface->
                SetDescriptorInterface(theRegionLocatorDescriptorInterface);
            
            /*decode descriptor*/
	    if(bBinary){
	      theRegionLocatorCSInterface->PreDecode();//sth
	      m_DLength[i] = theRegionLocatorCSInterface->StartDecode();
            }
	    else{
#ifdef USEXML
	      GenericDS RegionLocatorRoot=theCollectionInterface->GetDescription("Content");
	      RegionLocatorRoot.GetTextAttribute("xsi:type",xsitype);
	      while(xsitype!="ImageType"){
			  RegionLocatorRoot=RegionLocatorRoot.GetNextSibling("Content");
			  RegionLocatorRoot.GetTextAttribute("xsi:type",xsitype);
	      }
		  GenericDS theImage = RegionLocatorRoot.GetDescription("Image");
		  if(theImage.isNull())
			  return (unsigned long)-1;
		  GenericDS theSpatialLocation = theImage.GetDescription("SpatialLocator");
		  if(theSpatialLocation.isNull())
			  return (unsigned long)-1;
	      GenericDSInterfaceABC *DSinterface = theSpatialLocation.GetInterface();
	      theRegionLocatorDescriptorInterface->ImportDDL(DSinterface);
	      theCollection.RemoveChild(RegionLocatorRoot);
#endif //USEXML
	    }
        }
	theRegionLocatorCSInterface->destroy();
	
	if(bBinary)
	  theStreamBuffer->CloseBitStream();
	else {		
#ifdef USEXML
	  theDDLCSInterface->destroy();
#endif // USEXML
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorClient::Start(char *Query, int NoMatches)
{

  int j;
  // sth proprietary format? JH: returns error measures.
  char *outFile;

  /* image media*/
  MomVop *media;
    
  outFile = getpara("OutputFile", "DefaultOutput.txt");
  // remove output file if exist
  {
    ofstream c(outFile, ios::trunc);
  }

  RegionLocatorDescriptorInterfaceABC
    *theRegionLocatorDescriptorInterface;
  RegionLocatorExtractionTool*         theExtractionTool;
  theExtractionTool=new RegionLocatorExtractionTool(NULL);
  RegionLocatorExtractionInterfaceABC 
    *theRegionLocatorExtractionInterface= theExtractionTool->GetInterface();

    
    
  /* read filename data base*/
  QueryDB.CreateDatabase(Query);
  m_nQueryElements=QueryDB.GetNoOfElements();
    
    
  for(int i = 0; i < m_nQueryElements; i++) {
    // sth why more then one querey image? Location error of each image is calculated.

    MediaDBElement *QueryMediaElement;
        
    QueryMediaElement = QueryDB.GotoEntry(i);
    /* malloc media and load query data*/
    media=QueryDB.DBLoadImage();
    if (!media) return 0;

    /* search in the database the encoded image */
    for(j = 0; j < m_nInputElements; j++) {
      MediaDBElement *DBMediaElement;
      DBMediaElement = RamDB.GotoEntry(j);
      if(!strcmp(DBMediaElement->GetFilename(), \
		 QueryMediaElement->GetFilename())){
	// get descriptor interface 
	theRegionLocatorDescriptorInterface=
	  DBDescriptor[j]->GetInterface();
	MomImage *imLocator;
	// the size of this image must be the same as the input one
	// since there is no utility to allocate a MomImage, 
	// let's do it here
	imLocator = initimg(media->y_chan->x,media->y_chan->y,
			    UCHAR_TYPE);
                
	//define locator
	struct loc locator;

	// sth usually a search object is used to perform this. JH: to what is your question related?

	// check if using box or polygon, only the first of both types
	if( theRegionLocatorDescriptorInterface->GetNumberOfPolygons() == 0){
	  //use box as vertices, but must be set to 4 vertices
	  if(theRegionLocatorDescriptorInterface->GetNumberOfBoxes() == 0){
	    cerr << "Neither boxes nor polygons stored in " << 
	      QueryMediaElement->GetFilename() << endl;
	    exit(-1);
	  }
	  // get the first box and reallocate vertices;
	  unsigned long int *vert = new unsigned long int[4*2];
	  locator = theRegionLocatorDescriptorInterface->GetLocator(0, 0);
	  if(locator.NoVert == 2){
	    vert[0] = locator.Vertices[0];    vert[1] = locator.Vertices[1];
	    vert[2] = locator.Vertices[0];    vert[3] = locator.Vertices[3];
	    vert[4] = locator.Vertices[2];    vert[5] = locator.Vertices[3];
	    vert[6] = locator.Vertices[2];    vert[7] = locator.Vertices[1];
	    delete [] locator.Vertices;
	    locator.Vertices = vert;
	    locator.NoVert = 4;
	  }
	  else if(locator.NoVert == 3){
	    vert[0] = locator.Vertices[0];    vert[1] = locator.Vertices[1];
	    vert[2] = locator.Vertices[2];    vert[3] = locator.Vertices[3];
	    vert[4] = locator.Vertices[4];    vert[5] = locator.Vertices[5];
	    vert[6] = locator.Vertices[4];    vert[7] = locator.Vertices[1];
	    delete [] locator.Vertices;
	    locator.Vertices = vert;
	    locator.NoVert = 4;
	  }
	}
	else{
	  // get the polygonal description
	  locator = theRegionLocatorDescriptorInterface->
	    GetLocator(1, 0);
	}
	// convert locator to vertices
	vertices * vert;
	vert = LocatorToVertices(&locator);
	// get encoded shape 
	theRegionLocatorExtractionInterface->
	  getImageFromVertices(vert, imLocator);
	// free vertices 
	for( int k = 0; k < vert->nVertices; k++){
	  free((Short *) vert->VL[k]);
	}
	free((Short **) vert->VL);

          
	// get error
	float areaError = theRegionLocatorExtractionInterface->
	  ComputeAreaError(imLocator,media->y_chan);
	// print client results
	theRegionLocatorExtractionInterface->
	  printResults(outFile, QueryMediaElement->GetFilename(), m_DLength[j], areaError);
	// print image
	char *ImageName = (char *) malloc(strlen(QueryMediaElement->\
						 GetFilename()) + 10);
	strcpy(ImageName, QueryMediaElement->GetFilename());
	strcpy(strrchr(ImageName, '.'), ".sld");
	//                theRegionLocatorDescriptorInterface->dumpList(ImageName, 3);
	strcpy(strrchr(ImageName, '.'), "_d.pnm");
	//                theRegionLocatorDescriptorInterface->writePnmImage(ImageName);
	free((char *) ImageName);
	freeimg(imLocator);
      }
    }
        
    /* free media*/
    freevop(media);
        
  }
    
    return 0;
}

//----------------------------------------------------------------------------
unsigned long RegionLocatorClient::Stop()
{
//	fprintf(stderr,"Region Locator Client Closed\n");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long RegionLocatorClient::Close()
{
	/* free memories*/
        RamDB.FreeDatabase();
	QueryDB.FreeDatabase();

	{
	  int i;
	  for (i=0; i<m_nInputElements; i++) {
	    if (DBDescriptor[i])
	      DBDescriptor[i]->release();
	  }
	}        

        m_nInputElements = 0;
        m_nQueryElements = 0;
	DBDescriptor =0;

	return 0;
}

	
//----------------------------------------------------------------------------
const UUID& RegionLocatorClient::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionLocatorClient::GetName(void)
{ return myName; }


