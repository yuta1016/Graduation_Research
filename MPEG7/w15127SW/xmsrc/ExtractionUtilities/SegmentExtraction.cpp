//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Javier Ruiz Hidalgo - University Politechnic of Catalonia
// Ana B. Benitez - Columbia University
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
// Applicable File Name:  SegmentExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

//=============================================================================

using namespace XM;


const UUID SegmentExtractionInterface::myID = UUID("");
const char *SegmentExtractionInterface::myName = "SegmentDSExtractionInterface";

const UUID SegmentExtractionTool::myID = UUID("");
const char *SegmentExtractionTool::myName = "SegmentExtraction";
const int	SegmentExtractionTool::STRING_MAXSIZE= 4096;		// maximum size of the strings

const UUID SegmentExtractionInterfaceABC::myID = UUID();


//=============================================================================
SegmentExtractionInterfaceABC::SegmentExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SegmentExtractionInterface::SegmentExtractionInterface(SegmentExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SegmentExtractionInterface::~SegmentExtractionInterface()
{
}

//----------------------------------------------------------------------------
int SegmentExtractionInterface::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  return m_ExtractionTool->SetSourceMedia(mvFS, fp, pcFramesName, pcMasksName);
}

//----------------------------------------------------------------------------
int SegmentExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SegmentExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SegmentExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& SegmentExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
SegmentExtractionTool::SegmentExtractionTool():
m_Interface(this),
m_DSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SegmentExtractionTool::~SegmentExtractionTool()
{

}

//----------------------------------------------------------------------------
bool SegmentExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool SegmentExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SegmentExtractionTool::SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, 
	char *pcMasksName)
{
  
	m_mvFS			= mvFS;
	m_fp			= fp;
	m_pcFramesName	= pcFramesName;
	m_pcMasksName	= pcMasksName;

	return(0);
}

//----------------------------------------------------------------------------
int SegmentExtractionTool::SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DSInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentExtractionTool::InitExtracting(void)
{
	// Do not need this 
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SegmentExtractionTool::StartExtracting(void)
{

	// Get the XML DOM Node from the GenericDSInterface
        //sth direct usage of DOM functions not permitted,
        // sth use GenericDS instead
        DOM_Document DOMDoc;
	DOMDoc = m_DSInterface->GetDOMDocument();

	// Start parsing the tree file.
	RecursiveParse( 1, (DOM_Element &)DOMDoc);

	return 0;
}

//----------------------------------------------------------------------------
const UUID& SegmentExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SegmentExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SegmentExtractionInterfaceABC *SegmentExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int SegmentExtractionTool::ReadSegmentInfoTree( FILE *fp, int iSegmentID, char *pcSegmentType, 
	int*piSegmentNumChildren, int **ppiSegmentChildIDList, char *pcDecompType, char *pcOverlap, 
	char *pcGap, char *pcSegmentAnno)
{
	
	int iID;
	int iChild;
	
	// Go to the beggining of the file
	fseek( fp, 0, SEEK_SET); 

	// Scan line by line until the SegmentID is found
	while (fscanf( fp, "%d %s %d", &iID, pcSegmentType, piSegmentNumChildren)==3) {

		if (iID==iSegmentID) {
			
			// Read children
			if ((*piSegmentNumChildren)==0) {
				(*ppiSegmentChildIDList) = NULL;
			} else {
				(*ppiSegmentChildIDList) = new int[(*piSegmentNumChildren)];
				for (iChild=0; iChild<(*piSegmentNumChildren); iChild++) {
					fscanf( fp, "%d", &(*ppiSegmentChildIDList)[iChild]);
				}

				// Read Decomposition Type
				fscanf( fp, " %s %s %s", pcDecompType, pcOverlap, pcGap);
			}
			
			// Read Annotation (rest of the line)
			fgets( pcSegmentAnno, STRING_MAXSIZE, fp);
			if (strlen(pcSegmentAnno)>0) {
				pcSegmentAnno[strlen(pcSegmentAnno)-1] = '\0';	// remove last newline
			}

			return(0);
			
		} else {
		
			// Move to the begining of the next line or the end of file
			int c = getc(fp);
			while(c!='\n' && c!= EOF) {
				c = getc(fp);
			}
		}

	}

	// Couldn't read anymore, finish tree file
	return(-1);
}

//----------------------------------------------------------------------------
void SegmentExtractionTool::ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, 
	char *pcSegmentType, int *piSegmentNumFrames, int **ppiSegmentFramesList, 
	int **ppiSegmentMasksList)
{

	int i,k;
	int iNumFrames;					// Number of frames 
	unsigned char *pucMediaPtr;		// Pointer to Segment row in the media FS file
	int *piFramesList;				// List of frames
	int *piMasksList;				// List of Masks
	
	// Go to line iSegmentID of the media and read it
	if ( (iSegmentID-1)>=int(mediaFS->y_chan->y)) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't find the SegmentID in the FS file.");
		exit(-1);
	}
	
	// Count Number of Frames
	pucMediaPtr = (mediaFS->y_chan->data->u) + ((iSegmentID-1)*mediaFS->y_chan->x); 
	for (iNumFrames=0, i=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]>0) { iNumFrames++;}
	}
										
	// Check for no frames
	if (iNumFrames==0) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Found a Segment with No frames!!!");
		exit(-1);
	}

	// Create frame vector list
	piFramesList = new int[iNumFrames];	// frame vector list
	piMasksList  = new int[iNumFrames];	// mask vector list
					
	// 'i' is the x coordinate (correspond to frame number)
	// 'k' is the pointer inside piFrameList
	for (i=0,k=0; i<int(mediaFS->y_chan->x); i++) {
		if (pucMediaPtr[i]>0) { 
			piFramesList[k] = i;
			if (pucMediaPtr[i]==255) {
				piMasksList[k] = iSegmentID;
			} else {
				piMasksList[k] = int(pucMediaPtr[i]);
			}
			k++;
		}
	}
					
	// Return data
	(*piSegmentNumFrames)	= iNumFrames;
	(*ppiSegmentFramesList) = piFramesList;
	(*ppiSegmentMasksList)	= piMasksList;

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::RecursiveParse(int iSegmentID, DOM_Element &domParentElement)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	char	pcSegmentType[STRING_MAXSIZE];	// Type of segment ("VideoSegment", "MovingRegion" or "StillRegion")
	int		iSegmentNumChildren;			// Number of Children
	int		*piSegmentChildIDList = NULL;	// Vector list with Children's ID
	char	pcDecompType[STRING_MAXSIZE];
	char	pcOverlap[STRING_MAXSIZE];
	char	pcGap[STRING_MAXSIZE];
	char	pcSegmentAnno[STRING_MAXSIZE];	// Segment AnnotationDS

	int i;	// guess what this is for... :-)

	int iSegmentNumFrames = 0;
	int *piSegmentFramesList = NULL;
	int *piSegmentMasksList  = NULL;

	DOM_Document	domDoc;
	DOM_Element		domSegmentDecompositionElement;
	DOM_Element		domSegmentElement;


	// ------------------------------------------------------------------------
	// Read Segment Info from the tree file
	// ------------------------------------------------------------------------
	if (ReadSegmentInfoTree( m_fp, iSegmentID, pcSegmentType, &iSegmentNumChildren, 
			&piSegmentChildIDList, 	pcDecompType, pcOverlap, pcGap, pcSegmentAnno)!=0) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't find SegmendID = %d in the tree file.", iSegmentID);
		exit(-1);
	}

	// ------------------------------------------------------------------------
	// Read Segment Info from FS file
	// ------------------------------------------------------------------------
	ReadSegmentInfoFS( m_mvFS, iSegmentID, pcSegmentType, &iSegmentNumFrames, 
		&piSegmentFramesList, &piSegmentMasksList);
	
	
	// ------------------------------------------------------------------------
	// Create a new child of the parent node (for this Segment)
	// ------------------------------------------------------------------------
	//domDoc = domParentElement.getOwnerDocument();	 // You can not do this if domParentElement is a proper DOM_Document
	domDoc = m_DSInterface->GetDOMDocument();

	domSegmentElement = domDoc.createElement(pcSegmentType);	

	// ------------------------------------------------------------------------
	// Decide what to do for each Segment Class
	// ------------------------------------------------------------------------
	if (strncmp("VideoSegment", pcSegmentType, 12)==0) {

		ExtractionOfVideoSegmentDescriptors(domSegmentElement, iSegmentID, iSegmentNumFrames,
			piSegmentFramesList, m_pcFramesName);

	} else if (strncmp("MovingRegion", pcSegmentType,12)==0) {

		ExtractionOfMovingRegionDescriptors(domSegmentElement, iSegmentID, iSegmentNumFrames,
			piSegmentFramesList, piSegmentMasksList, m_pcFramesName, m_pcMasksName);
	
	} else if (strncmp("StillRegion", pcSegmentType,11)==0) {

		ExtractionOfStillRegionDescriptors(domSegmentElement, iSegmentID, piSegmentFramesList[0], 
			piSegmentMasksList[0], m_pcFramesName, m_pcMasksName);

	}

	// ------------------------------------------------------------------------
	// Append new node to parent one
	// ------------------------------------------------------------------------
	domParentElement.appendChild(domSegmentElement);

	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
	fprintf( stderr, "\nRead Segment %d, type \"%s\", with %d children.\n\n", 
		iSegmentID, pcSegmentType, iSegmentNumChildren);
	
	// ------------------------------------------------------------------------
	// Do the same for each children
	// ------------------------------------------------------------------------
	if (iSegmentNumChildren>0) {

		// Create a SegmentDS Type for Children
		domSegmentDecompositionElement = domDoc.createElement("SegmentDecomposition");

		// Set Decomposition Type
		domSegmentDecompositionElement.setAttribute("DecompositionType", pcDecompType);
		domSegmentDecompositionElement.setAttribute("Overlap", pcOverlap);
		domSegmentDecompositionElement.setAttribute("Gap", pcGap);
		
		// Attach it to Segment Node	
		domSegmentElement.appendChild(domSegmentDecompositionElement);		

		// Do the recursivity
		for (i=0; i<iSegmentNumChildren; i++) {
			RecursiveParse( piSegmentChildIDList[i], domSegmentDecompositionElement);
		}

	}


	// ------------------------------------------------------------------------
	// Release memory
	// ------------------------------------------------------------------------
	if (piSegmentChildIDList!=NULL) {
		delete [] piSegmentChildIDList;
	}
	if (piSegmentFramesList!=NULL) {
		delete [] piSegmentFramesList;
	}
	if (piSegmentMasksList!=NULL) {
		delete [] piSegmentMasksList;
	}

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::ExtractionOfStillRegionDescriptors(DOM_Element &domElement, 
		int id, int frame, int mask,
		char *framespath, char *maskspath)
{
	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	char str[STRING_MAXSIZE];

	// Set ID Attribute
	sprintf(str, "SR%d", id);
	domElement.setAttribute("id", str);

	// boolean SpatialConnectivity
	MomVop *mvMask;
	MediaDB RamDB;
	bool bSpatialConnected = true;
		
	sprintf(str, maskspath, mask,frame);
	mvMask = RamDB.LoadNamedImage(str);

	if (!mvMask) {
		fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't open media %s.\n",str);
		exit(-1);
	}

	bSpatialConnected = SpatialConnectivity4(mvMask);

	if (bSpatialConnected) {
		domElement.setAttribute("SpatialConnectivity", "true");
	} else {
		domElement.setAttribute("SpatialConnectivity", "false");
	}

	// Internal Ds

	// At the moment, we can not create an MomVop structure from an image and its associated
	// mask. So no ColorHistogramExtraction is done.
	//ColorHistogramDescriptorDOMExtraction( media, domElement);

	// The ContourShape uses a masks as input so it can be used. There is still a problem
	// with non-connected component as the algorithmonly supports one connected component
	// but the input mask can have any.
	ContourShapeDescriptorDOMExtraction( mvMask, domElement);

	freevop(mvMask);

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::ExtractionOfMovingRegionDescriptors(DOM_Element &domElement, 
	int id, int nframes, int *frames, int *masks, char *framespath, char *maskspath)
{
	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	char str[STRING_MAXSIZE];

	// Set ID Attribute
	sprintf(str, "MR%d", id);
	domElement.setAttribute("id", str);

	// boolean TemporalConnectivity;
	bool tConnected = true; 
	unsigned int i;
	for (i=1; i<nframes; i++) {
		if (frames[i] != frames[0]+i) {
			tConnected = false;
			break;
		}
	}
	if (tConnected) {
		domElement.setAttribute("TemporalConnectivity", "true");
	} else {
		domElement.setAttribute("TemporalConnectivity", "false");
	}
	// boolean SpatialConnectivity
	MomVop *media;
	MediaDB RamDB;
	bool bSpatialConnected = true;
	fprintf( stderr, "Computing SpatialConnectivity of MovingRegion.\n");
	for (i=0; i<nframes; i++) {
		
		fprintf( stderr, "%d - ", frames[i]);

		sprintf(str, maskspath, masks[i],frames[i]);
		media = RamDB.LoadNamedImage(str);
		if (!media) {
			fprintf( stderr, "ERROR(SegmentExtractionTool): Couldn't open media %s.\n",str);
			exit(-1);
		}
		bSpatialConnected = SpatialConnectivity4(media);
		if (bSpatialConnected==false) {
			break;
		}
		freevop(media);
	}
	fprintf(stderr, "\n");
	if (bSpatialConnected) {
		domElement.setAttribute("SpatialConnectivity", "true");
	} else {
		domElement.setAttribute("SpatialConnectivity", "false");
	}


	// Internal Ds
	// At the moment, we can not create an MomVop structure from an image and its associated
	// mask. So no ColorHistogramExtraction is done.
	//GoFColorHistogramDescriptorDOMExtraction( nframes, frames, masks, framespath, maskspath, domElement);

	// This needs the alpha channel information also, but for the moment we try settings
	// the input as the binary masks only, unfortunately the output is mostly 0's..
	ParametricObjectMotionDescriptorDOMExtraction(nframes, frames, masks, framespath, maskspath, domElement);

}


void SegmentExtractionTool::ExtractionOfVideoSegmentDescriptors(DOM_Element &domElement, 
	int id, int nframes, int *frames, char* path)
{
	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	char str[STRING_MAXSIZE];

	// Set ID Attribute
	sprintf(str, "VS%d", id);
	domElement.setAttribute("id", str);

	// boolean TemporalConnectivity;
	bool bConnected = true; 
	unsigned int i;
	for (i=1; i<nframes; i++) {
		if (frames[i] != frames[0]+i) {
			bConnected = false;
			break;
		}
	}
	if (bConnected) {
		domElement.setAttribute("TemporalConnectivity", "true");
	} else {
		domElement.setAttribute("TemporalConnectivity", "false");
	}


	// Internal Ds
	GoFColorHistogramDescriptorDOMExtraction( nframes, frames, NULL, path, NULL, domElement);

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::ParametricObjectMotionDescriptorDOMExtraction(int nframes, 
		int *frames, int *masks, char *framespath, char *maskspath, DOM_Element &domElement)
{

	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	char str[STRING_MAXSIZE];
	int  i;
	
	fprintf( stderr, "Starting ParametricObjectMotionDescriptorDOMExtraction()\n");
	
	// ListFile should contains the two images for each "object"
	// should add here option file which contain the model type

	int opt_modelCode=2;
  
	// image media
    MomVop *media1;
    MomVop *media2;

	// Image data base
	MediaDB RamDB;

    // descriptor and sub-descriptor

	ParametricObjectMotionDescriptor theParametricObjectMotionDescription;
	ParametricObjectMotionDescriptorInterfaceABC *theParametricObjectMotionDescriptorInterface;

	/* extraction tool*/
	ParametricObjectMotionExtractionTool theExtractionTool;
	ParametricObjectMotionExtractionInterfaceABC  *theExtractionInterface;
	
	/* coding schemes*/
	ParametricObjectMotionCS theParametricObjectMotionCS;
	ParametricObjectMotionCSInterfaceABC *theParametricObjectMotionCSInterface;

	theParametricObjectMotionDescriptorInterface = theParametricObjectMotionDescription.GetInterface();


	/* connect descriptor to extraction tool*/
	theExtractionInterface=theExtractionTool.GetInterface();
    theExtractionInterface->SetDescriptorInterface(theParametricObjectMotionDescriptorInterface);

	theParametricObjectMotionDescriptorInterface->SetModelCode(opt_modelCode);

	/* connect descriptor to coding scheme*/
	theParametricObjectMotionCSInterface=theParametricObjectMotionCS.GetInterface();
	theParametricObjectMotionCSInterface->SetDescriptorInterface(theParametricObjectMotionDescriptorInterface);

	/* connect stream to coding scheme*/
	//theParametricObjectMotionCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	//theStreamBuffer.StartBitStream(Bitstream);

	//RamDB.CreateDatabase(ListFile);

	for (i=1; i<nframes; i++) {

		// malloc media and load input data

		// As said before, two input binary masks are used as input but this should be changed
		// to read a set of frames and associated masks
		sprintf(str, maskspath, masks[i], frames[i]);
		media1 = RamDB.LoadNamedImage(str);
		sprintf(str, maskspath, masks[i-1], frames[i-1]);
		media2 = RamDB.LoadNamedImage(str);
	  
		if ((!media1)||(!media2)) {
			fprintf( stderr, "ERROR(SegmentExtraction): Could not open media.\n");
			break;
		}

		fprintf( stderr, "(%d,%d) - ", frames[i-1], frames[i]);


		// here add alpha if not available in Vop structure

		UChar *UCrefTest = (UChar *) GetMomImageData(GetMomVopA(media1));
		int width = (GetMomVopWidth(media1));
		int height = (GetMomVopHeight(media1));
		int ii,isAlpha=0;
		for (ii=0;ii<height*width;ii++) {
			if (UCrefTest[ii]) {
				isAlpha=1;
				break;
			}
		}
		UChar *UCcurrTest = (UChar *) GetMomImageData(GetMomVopA(media2));
		width = (GetMomVopWidth(media2));
		height = (GetMomVopHeight(media2));
		for (ii=0;ii<height*width;ii++) {
			if (UCcurrTest[ii]) {
				isAlpha=1;
				break;
			}
		}
		if (!isAlpha){
			UChar *UCref = (UChar *) GetMomImageData(GetMomVopY(media1));
			width = (GetMomVopWidth(media1));
			height = (GetMomVopHeight(media1));
			unsigned char *pixelval;
			for (ii=0,pixelval=(unsigned char *)media1->a_chan->f;ii<height*width;*(pixelval++)=(unsigned char)UCref[ii],ii++);
			UChar *UCcurr = (UChar *) GetMomImageData(GetMomVopY(media2));
			width = (GetMomVopWidth(media2));
			height = (GetMomVopHeight(media2));
			for (ii=0,pixelval=(unsigned char *)media2->a_chan->f;ii<height*width;*(pixelval++)=(unsigned char)UCcurr[ii],ii++);
		}

		// end add alpha if not available in Vop structure

		// Connect media to extraction tool*/
		theExtractionInterface=theExtractionTool.GetInterface();
		theExtractionInterface->SetSourceMedia(media2,media1);
	  
		// extract descriptor form media*/
		theExtractionInterface->StartExtracting();
	  
		// Code descriptor data
		//theParametricObjectMotionCSInterface->StartEncode();

		// Free media
		freevop(media1);
		freevop(media2);

		DOM_Element ParametricObjectMotionNode;
		ParametricObjectMotionNode = ParametricObjectMotionDescriptorDOMElement(doc, 
				theParametricObjectMotionDescriptorInterface);
		domElement.appendChild(ParametricObjectMotionNode);

	}

	//theStreamBuffer.CloseBitStream();
}

//----------------------------------------------------------------------------
void SegmentExtractionTool::ContourShapeDescriptorDOMExtraction(MomVop *media,
						     DOM_Element &domElement)
{
	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	// descriptor and sub-descriptor
	ContourShapeDescriptor theContourShapeDescription;
	ContourShapeDescriptorInterfaceABC
	  *theContourShapeDescriptorInterface;

	// extraction tool
	ContourShapeExtractionTool theExtractionTool;
	ContourShapeExtractionInterfaceABC
	  *theExtractionInterface;
	
	// coding schemes
	ContourShapeCS theContourShapeCS;
	ContourShapeCSInterfaceABC *theContourShapeCSInterface;

	// stream data base
	//EncoderFileIO theStreamBuffer(0x1000);

	// Image data base
	//MediaDB RamDB;

	// link descriptor to extraction tool
	theExtractionInterface=theExtractionTool.GetInterface();
	theContourShapeDescriptorInterface=
	  theContourShapeDescription.GetInterface();
        theExtractionInterface->
	  SetDescriptorInterface(theContourShapeDescriptorInterface);

	// link descriptor to coding scheme
	theContourShapeCSInterface=theContourShapeCS.GetInterface();
	theContourShapeCSInterface->
	  SetDescriptorInterface(theContourShapeDescriptorInterface);

	theContourShapeCSInterface=theContourShapeCS.GetInterface();
	theContourShapeCSInterface->
	  SetDescriptorInterface(theContourShapeDescriptorInterface);

	// link stream to coding scheme
/*	theContourShapeCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	theStreamBuffer.StartBitStream(Bitstream);

	RamDB.CreateDatabase(ListFile);
	while (1) {

	  // malloc media and load input data
	  media=RamDB.DBLoadImage();
*/
	if (!media) return;

	  // link media to extraction tool
	  theExtractionInterface=theExtractionTool.GetInterface();
	  theExtractionInterface->SetSourceMedia(media);
	  
	  // extract descriptor form media
	  theExtractionInterface->StartExtracting();

	  // code descriptor data
/*	  theContourShapeCSInterface->StartEncode();

	  // free media
	  freevop(media);
	}

	theStreamBuffer.CloseBitStream();
	
	return 0;
*/

	DOM_Element ContourShapeNode;
	ContourShapeNode = ContourShapeDescriptorDOMElement(doc, theContourShapeDescriptorInterface);
	domElement.appendChild(ContourShapeNode);

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::
ColorHistogramDescriptorDOMExtraction(MomVop *media, DOM_Element &domElement)
{
	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	/* Image data base */
	MediaDB RamDB;

        /* descriptor and sub-descriptor*/
        ColorSpaceDescriptor theColorSpaceDescription;
        ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface;

	ColorQuantizerDescriptor  theColorQuantizerDescription;
	ColorQuantizerDescriptorInterfaceABC
	  *theColorQuantizerDescriptorInterface;

        HistogramDescriptor theHistogramDescription;
        HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface;

	ColorHistogramDescriptor theColorHistogramDescription;
	ColorHistogramDescriptorInterfaceABC
	  *theColorHistogramDescriptorInterface;

	/* extraction tool*/
	ColorHistogramExtractionTool theExtractionTool;
	ColorHistogramExtractionInterfaceABC
	  *theExtractionInterface;
	
	/* coding schemes*/
	ColorSpaceCS theColorSpaceCS;
	ColorSpaceCSInterfaceABC *theColorSpaceCSInterface;

	ColorQuantizerCS theColorQuantizerCS;
	ColorQuantizerCSInterfaceABC *theColorQuantizerCSInterface;

	HistogramCS theHistogramCS;
	HistogramCSInterfaceABC *theHistogramCSInterface;

	ColorHistogramCS theColorHistogramCS;
	ColorHistogramCSInterfaceABC *theColorHistogramCSInterface;

	/* set color space*/
	theColorSpaceDescriptorInterface=
	  theColorSpaceDescription.GetInterface();

	/* set quantizer*/
	theColorQuantizerDescriptorInterface=
	  theColorQuantizerDescription.GetInterface();

	/* set histogram*/
	theHistogramDescriptorInterface=
	  theHistogramDescription.GetInterface();

	/* connect colorspace and quantizer to histogram descriptor*/
	theColorHistogramDescriptorInterface=
	  theColorHistogramDescription.GetInterface();
	theColorHistogramDescriptorInterface->
	  SetColorSpaceDescriptorInterface(theColorSpaceDescriptorInterface);
	theColorHistogramDescriptorInterface->
	  SetColorQuantizerDescriptorInterface(
		      theColorQuantizerDescriptorInterface);
	theColorHistogramDescriptorInterface->
	  SetHistogramDescriptorInterface(
		      theHistogramDescriptorInterface);


	/* connect descriptor to extraction tool*/
	theExtractionInterface=theExtractionTool.GetInterface();
        theExtractionInterface->
	  SetDescriptorInterface(theColorHistogramDescriptorInterface);

	/* connect descriptor to coding scheme*/
	theColorSpaceCSInterface=theColorSpaceCS.GetInterface();
	theColorSpaceCSInterface->
	  SetDescriptorInterface(theColorSpaceDescriptorInterface);

	theColorQuantizerCSInterface=theColorQuantizerCS.GetInterface();
	theColorQuantizerCSInterface->
	  SetDescriptorInterface(theColorQuantizerDescriptorInterface);

	theHistogramCSInterface=theHistogramCS.GetInterface();
	theHistogramCSInterface->
	  SetDescriptorInterface(theHistogramDescriptorInterface);

	theColorHistogramCSInterface=theColorHistogramCS.GetInterface();
	theColorHistogramCSInterface->
	  SetDescriptorInterface(theColorHistogramDescriptorInterface);
	theColorHistogramCSInterface->
	  SetColorSpaceCSInterface(theColorSpaceCSInterface);
	theColorHistogramCSInterface->
	  SetColorQuantizerCSInterface(theColorQuantizerCSInterface);
	theColorHistogramCSInterface->
	  SetHistogramCSInterface(theHistogramCSInterface);
	

	/* connect stream to coding scheme*/
/*	theColorSpaceCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theColorQuantizerCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theHistogramCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theColorHistogramCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	theStreamBuffer.StartBitStream(Bitstream);

	RamDB.CreateDatabase(ListFile);
*/
	/* set predifined descriptor values*/
/*	theColorSpaceDescriptorInterface->SetDescriptionColorSpace(YCRCB);*/
/*	theColorSpaceDescriptorInterface->SetDescriptionColorSpace(RGB);*/
	theColorSpaceDescriptorInterface->SetDescriptionColorSpace(HSV);
	theColorQuantizerDescriptorInterface->SetColorQuantizerType(Linear);
	theColorQuantizerDescriptorInterface->SetNoOfBins(4,4,4);
/*	fprintf(stderr,"Question: histogram size better set"
		" automatically in extraction?\n");
	theHistogramDescriptorInterface->SetHistogramSize(4*4*4);*/

	  /* malloc media and load input data*/
	  //media=RamDB.DBLoadImage();
	  if (!media) return;

	  /* connect media to extraction tool*/
	  theExtractionInterface=theExtractionTool.GetInterface();
	  theExtractionInterface->SetSourceMedia(media);
	  
	  /* extract descriptor form media*/
	  theExtractionInterface->StartExtracting();

	  /* code descriptor data*/
	 // theColorHistogramCSInterface->StartEncode();


	//theStreamBuffer.CloseBitStream();
	DOM_Element ColorHistogramDescriptorNode = ColorHistogramDescriptorDOMElement(doc, 
		theColorHistogramDescriptorInterface);
	domElement.appendChild(ColorHistogramDescriptorNode);

}

//----------------------------------------------------------------------------
void SegmentExtractionTool::GoFColorHistogramDescriptorDOMExtraction( int nframes, int *frames, 
	int *masks, char *path, char *maskspath, DOM_Element &domElement)
{

	DOM_Document doc;
	doc = m_DSInterface->GetDOMDocument();

	int i;
	char str[STRING_MAXSIZE];

	fprintf( stderr, "Starting GoFColorHistogramDescriptorDOMExtraction()\n");
		
	MomVop *media;
	MediaDB RamDB;

	// stream data base (not needed)
	//EncoderFileIO theStreamBuffer(0x1000);

	// descriptor and sub-descriptors
	ColorSpaceDescriptor theColorSpaceDescription;
	ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface;

	ColorQuantizerDescriptor theColorQuantizerDescription;
	ColorQuantizerDescriptorInterfaceABC *theColorQuantizerDescriptorInterface;

    HistogramDescriptor theHistogramDescription;
	HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface;

    ColorHistogramDescriptor theColorHistogramDescription;
    ColorHistogramDescriptorInterfaceABC *theColorHistogramDescriptorInterface;

	GoFColorHistogramDescriptor theGoFColorHistogramDescription;
	GoFColorHistogramDescriptorInterfaceABC *theGoFColorHistogramDescriptorInterface;

	// Extraction tool
	GoFColorHistogramExtractionTool theExtractionTool;
	GoFColorHistogramExtractionInterfaceABC *theExtractionInterface;
	
	// Coding Schemes (not needed)
	ColorSpaceCS theColorSpaceCS;
	ColorSpaceCSInterfaceABC *theColorSpaceCSInterface;

	ColorQuantizerCS theColorQuantizerCS;
	ColorQuantizerCSInterfaceABC *theColorQuantizerCSInterface;

	HistogramCS theHistogramCS;
	HistogramCSInterfaceABC *theHistogramCSInterface;

	ColorHistogramCS theColorHistogramCS;
	ColorHistogramCSInterfaceABC *theColorHistogramCSInterface;

	GoFColorHistogramCS theGoFColorHistogramCS;
	GoFColorHistogramCSInterfaceABC *theGoFColorHistogramCSInterface;

	// Set color space
	theColorSpaceDescriptorInterface = theColorSpaceDescription.GetInterface();

	// set quantizer
	theColorQuantizerDescriptorInterface = theColorQuantizerDescription.GetInterface();

	// set histogram
	theHistogramDescriptorInterface = theHistogramDescription.GetInterface();

	//  set color histogram
	theColorHistogramDescriptorInterface = theColorHistogramDescription.GetInterface();

	// set GoF color histogram
	theGoFColorHistogramDescriptorInterface = theGoFColorHistogramDescription.GetInterface();
	
	// connect colorspace and quantizer to color histogram descriptors
	theGoFColorHistogramDescriptorInterface->SetColorHistogramDescriptorInterface(theColorHistogramDescriptorInterface);
	theColorHistogramDescriptorInterface->SetColorSpaceDescriptorInterface(theColorSpaceDescriptorInterface);
	theColorHistogramDescriptorInterface->SetColorQuantizerDescriptorInterface(theColorQuantizerDescriptorInterface);
	theColorHistogramDescriptorInterface->SetHistogramDescriptorInterface(theHistogramDescriptorInterface);

	// connect descriptor to extraction tool
	theExtractionInterface=theExtractionTool.GetInterface();
    theExtractionInterface->SetDescriptorInterface(theGoFColorHistogramDescriptorInterface);

	// Connect descriptors to coding schemes
	theColorSpaceCSInterface=theColorSpaceCS.GetInterface();
	theColorSpaceCSInterface->SetDescriptorInterface(theColorSpaceDescriptorInterface);

	theColorQuantizerCSInterface =theColorQuantizerCS.GetInterface();
	theColorQuantizerCSInterface->SetDescriptorInterface(theColorQuantizerDescriptorInterface);

	theHistogramCSInterface=theHistogramCS.GetInterface();
	theHistogramCSInterface->SetDescriptorInterface(theHistogramDescriptorInterface);

	theColorHistogramCSInterface=theColorHistogramCS.GetInterface();
	theColorHistogramCSInterface->SetDescriptorInterface(theColorHistogramDescriptorInterface);
	theColorHistogramCSInterface->SetColorSpaceCSInterface(theColorSpaceCSInterface);
	theColorHistogramCSInterface->SetColorQuantizerCSInterface(theColorQuantizerCSInterface);
	theColorHistogramCSInterface->SetHistogramCSInterface(theHistogramCSInterface);
		
	theGoFColorHistogramCSInterface=theGoFColorHistogramCS.GetInterface();
	theGoFColorHistogramCSInterface->SetDescriptorInterface(theGoFColorHistogramDescriptorInterface);
	theGoFColorHistogramCSInterface->SetColorHistogramCSInterface(theColorHistogramCSInterface);

/*
	// connect stream to coding scheme

	theColorSpaceCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theColorQuantizerCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theHistogramCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theColorHistogramCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);
	theGoFColorHistogramCSInterface->SetEncoderStreamBuffer(&theStreamBuffer);

	theStreamBuffer.StartBitStream(Bitstream);

	RamDB.CreateDatabase(ListFile);
	int NoOfElements = RamDB.GetNoOfElements();
*/


	/* allocate an array of histograms to be used for GoF
	   histogram computation */

	int NoOfElements = nframes;

	HistogramDescriptor *HistogramArray;
	HistogramArray = new HistogramDescriptor [NoOfElements];

	/* set predefined descriptor values */

	theColorSpaceDescriptorInterface->SetDescriptionColorSpace(HSV);
	theColorQuantizerDescriptorInterface->SetColorQuantizerType(Linear);
	theColorQuantizerDescriptorInterface->SetNoOfBins(4,4,4);
/*	fprintf(stderr,"Question: histogram size better set"
		" automatically in extraction?\n");
	theHistogramDescriptorInterface->SetHistogramSize(4*4*4);*/
	theGoFColorHistogramDescriptorInterface->SetGoFColorHistogramType(AVGH);
	
	for(i=0; i<NoOfElements; i++) {
		
		/* malloc media and load input data */
		if (masks==NULL) {
			// No masks 
			sprintf(str, path, frames[i]);
			media = RamDB.LoadNamedImage(str);
		} else {
			// MASKS .. how????
			fprintf( stderr, "ERROR(SegmentExtractionTool): Sorry but at the moment I don not know how to use masks.\n");
			exit(-1);

		}
		
		
		if (!media) {
			fprintf( stderr, "ERROR(SegmentExtractionTool): Could not open media.\n");
			exit(-1);
		}


		fprintf( stderr, "%d - ", frames[i]);

		/* get the interfaces */
	  theHistogramDescriptorInterface = HistogramArray[i].GetInterface();

	  /* connect everything together */
	  theColorHistogramDescriptorInterface->
	      SetHistogramDescriptorInterface(theHistogramDescriptorInterface); 

	  /* connect media to extraction tool */
	  theExtractionInterface = theExtractionTool.GetInterface();
	  theExtractionInterface->SetSourceMedia(media);

	  /* extract descriptor from media */
	  theExtractionInterface->StartExtracting();

	  /* free media */
	  freevop(media);
	  
	}

	theHistogramDescriptorInterface = theHistogramDescription.GetInterface();
        theColorHistogramDescriptorInterface->
	    SetHistogramDescriptorInterface(theHistogramDescriptorInterface);

	/* pass the histogram array and the GoF descriptor to the final 
	   extraction function */

	theExtractionInterface->ComputeGoFColorHist(HistogramArray, i, 2);

/*
	// code descriptor data
	theGoFColorHistogramCSInterface->StartEncode();

	theStreamBuffer.CloseBitStream();
*/

	// Create GoFColorHistogramDescriptorNode and append child to root element

	DOM_Element GoFColorHistogramDescriptorNode = GoFColorHistogramDescriptorDOMElement(doc, 
		theGoFColorHistogramDescriptorInterface);
	domElement.appendChild(GoFColorHistogramDescriptorNode);
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::GoFColorHistogramDescriptorDOMElement(DOM_Document doc, 
GoFColorHistogramDescriptorInterfaceABC *theGoFColorHistogramDescriptorInterface)
{
	// DOM for GoFColorHistogramD

	DOM_Element GoFColorHistogramDescriptorNode;
	GoFColorHistogramDescriptorNode = (DOM_Element)doc.createElement("GoFGoPHistogramD");

	switch (theGoFColorHistogramDescriptorInterface->GetGoFColorHistogramType()) {
    case AVGH:
		GoFColorHistogramDescriptorNode.setAttribute("HistogramTypeInfo", "Average");
		break;
    case MEDH:
		GoFColorHistogramDescriptorNode.setAttribute("HistogramTypeInfo", "Median");
		break;
    case INTH:
		GoFColorHistogramDescriptorNode.setAttribute("HistogramTypeInfo", "Intersection");
		break;
    case RH:
		GoFColorHistogramDescriptorNode.setAttribute("HistogramTypeInfo", "Reserved");
		break;
    default:
		GoFColorHistogramDescriptorNode.setAttribute("HistogramTypeInfo", "Default???");
		break;
    }	

	DOM_Element ColorHistogramDescriptorNode = ColorHistogramDescriptorDOMElement(doc, 
		theGoFColorHistogramDescriptorInterface->GetColorHistogramDescriptorInterface());
	GoFColorHistogramDescriptorNode.appendChild(ColorHistogramDescriptorNode);

	return GoFColorHistogramDescriptorNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::ColorHistogramDescriptorDOMElement(DOM_Document doc, 
ColorHistogramDescriptorInterfaceABC *theColorHistogramDescriptorInterface)
{
	// DOM for ColorHistogramD
	
	DOM_Element ColorHistogramDescriptorNode;
	ColorHistogramDescriptorNode = (DOM_Element)doc.createElement("ColorHistogramD");

	DOM_Element ColorSpaceDescriptorNode = ColorSpaceDescriptorDOMElement(doc, 
		theColorHistogramDescriptorInterface->GetColorSpaceDescriptorInterface());
	ColorHistogramDescriptorNode.appendChild(ColorSpaceDescriptorNode);

	DOM_Element ColorQuantizerDescriptorNode = ColorQuantizerDescriptorDOMElement(doc, 
		theColorHistogramDescriptorInterface->GetColorQuantizerDescriptorInterface());
	ColorHistogramDescriptorNode.appendChild(ColorQuantizerDescriptorNode);

	DOM_Element HistogramDescriptorNode = HistogramDescriptorDOMElement(doc, 
		theColorHistogramDescriptorInterface->GetHistogramDescriptorInterface());
	ColorHistogramDescriptorNode.appendChild(HistogramDescriptorNode);

	return ColorHistogramDescriptorNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::ColorSpaceDescriptorDOMElement(DOM_Document doc, 
ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface)
{
	// DOM for ColorSpaceD

	DOM_Element ColorSpaceDescriptorNode;
	ColorSpaceDescriptorNode = (DOM_Element)doc.createElement("ColorSpaceD");

	DOM_Element LinearMatrixNode;
	int yindex, xindex;

	switch (theColorSpaceDescriptorInterface->GetDescriptionColorSpace()) {
	case RGB:	  
		ColorSpaceDescriptorNode.setAttribute("Space", "RGB");
		break;
	case YCRCB:
		ColorSpaceDescriptorNode.setAttribute("Space", "YCrCb");
		break;
	case HSV:
		ColorSpaceDescriptorNode.setAttribute("Space", "HSV");
		break;
	case HMMD: //added by LGCIT
		ColorSpaceDescriptorNode.setAttribute("Space", "HMMD");
		break;
	case Linear_Matrix:
		ColorSpaceDescriptorNode.setAttribute("Space", "LinearMatrix");

		LinearMatrixNode = (DOM_Element)doc.createElement("LinearMatrix");
		ColorSpaceDescriptorNode.appendChild(LinearMatrixNode);
		for (yindex=0; yindex<3; yindex++) {
			for (xindex=0; xindex<3; xindex++) {
				DOM_Element ColorTransMatValueNode = CreateIntegerElement(doc, "ColorTransMatValue", 
					theColorSpaceDescriptorInterface->GetMatrixElement(xindex,yindex));
				LinearMatrixNode.appendChild(ColorTransMatValueNode);
			}
		}
		break;
	default:
		ColorSpaceDescriptorNode.setAttribute("ColorSpaceType", "Default???");
		break;
	}

	return ColorSpaceDescriptorNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::ColorQuantizerDescriptorDOMElement(DOM_Document doc, 
ColorQuantizerDescriptorInterfaceABC *theColorQuantizerDescriptorInterface)
{
		char str[STRING_MAXSIZE];

		// DOM for ColorQuantizationD

	DOM_Element ColorQuantizationDescriptorNode;

	int index, k, ki, l, li, m, mi;
	DOM_Element bin_number_1Node;
	DOM_Element color_valueNode;

	switch (theColorQuantizerDescriptorInterface->GetColorQuantizerType()) {
	case Linear:
		ColorQuantizationDescriptorNode = (DOM_Element)doc.createElement("LinearColorQuantizationD");
		ColorQuantizationDescriptorNode.setAttribute("Quantization", "linear");

		for (index=0; index<3; index++) {
			DOM_Element bin_numberNode = CreateIntegerElement(doc, "bin_number", theColorQuantizerDescriptorInterface->GetNoOfBins(index));
			ColorQuantizationDescriptorNode.appendChild(bin_numberNode);
		}
		break;
	case NonLinear:
		ColorQuantizationDescriptorNode = (DOM_Element)doc.createElement("NonLinearColorQuantizationD");
		ColorQuantizationDescriptorNode.setAttribute("Quantization", "non_linear");

		k = theColorQuantizerDescriptorInterface->GetNonLinBins1();
		bin_number_1Node = CreateIntegerElement(doc, "bin_number_1", k);
		ColorQuantizationDescriptorNode.appendChild(bin_number_1Node);
			
		for(ki=0; ki<k; ki++) {
			DOM_Element color_value_1Node = CreateIntegerElement(doc, "color_value_1", 
				theColorQuantizerDescriptorInterface->GetNonLinColorValue1(ki));
			ColorQuantizationDescriptorNode.appendChild(color_value_1Node);

			l = theColorQuantizerDescriptorInterface->GetNonLinBins2(ki);
			DOM_Element bin_number_2Node = CreateIntegerElement(doc, "bin_number_2", l);
			ColorQuantizationDescriptorNode.appendChild(bin_number_2Node);

			for(li=0; li<l; li++) {
				DOM_Element color_value_2Node = CreateIntegerElement(doc, "color_value_2", 
					theColorQuantizerDescriptorInterface->GetNonLinColorValue2(ki,li));
				ColorQuantizationDescriptorNode.appendChild(color_value_2Node);

				m=theColorQuantizerDescriptorInterface->GetNonLinBins3(ki,li);
				DOM_Element bin_number_3Node = CreateIntegerElement(doc, "bin_number_3", m);
				ColorQuantizationDescriptorNode.appendChild(bin_number_3Node);

				for(mi=0; mi<m; mi++) {
					DOM_Element color_value_3Node = CreateIntegerElement( doc, "color_value_3", 
						theColorQuantizerDescriptorInterface->GetNonLinColorValue3(ki,li,mi));
					ColorQuantizationDescriptorNode.appendChild(color_value_3Node);
				}
			}
		}
		break;
	case LookupTable:
		ColorQuantizationDescriptorNode = (DOM_Element)doc.createElement("LUTColorQuantizationD");
		ColorQuantizationDescriptorNode.setAttribute("Quantization", "lookup_table");
		
		k = theColorQuantizerDescriptorInterface->GetLookupBins();
	
		ColorQuantizationDescriptorNode.setAttribute("bin_number", Str(k, str));

		for( ki=0; ki<k; ki++) {
			for(index=0; index<3; index++) {
				color_valueNode = CreateIntegerElement( doc, "color_value", 
					theColorQuantizerDescriptorInterface->GetLookupValue(ki,index));
				ColorQuantizationDescriptorNode.appendChild(color_valueNode);

			}
		}
		break;
	default:
		ColorQuantizationDescriptorNode = (DOM_Element)doc.createElement("ColorQuantizationD");
		ColorQuantizationDescriptorNode.setAttribute("Quantization", "Default???");
	}

	return ColorQuantizationDescriptorNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::HistogramDescriptorDOMElement(DOM_Document doc, 
HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface)
{
	char str[STRING_MAXSIZE];

	// DOM for HistogramD

	DOM_Element HistogramDescriptorNode;
	HistogramDescriptorNode = (DOM_Element)doc.createElement("HistogramD");
	
	HistogramDescriptorNode.setAttribute("NumberHistogramBins", Str(theHistogramDescriptorInterface->GetHistogramSize(), str));

	if (theHistogramDescriptorInterface->GetHistogramSize()) {

		HistogramDescriptorNode.setAttribute("HistogramNormFactor", Str(theHistogramDescriptorInterface->GetNormalizationFactor(), str));

		for (int i=0; i<theHistogramDescriptorInterface->GetHistogramSize(); i++) {

			DOM_Element HistogramValueNode;
			HistogramValueNode = CreateIntegerElement(doc, "HistogramValue", theHistogramDescriptorInterface->GetElement(i));
			HistogramDescriptorNode.appendChild(HistogramValueNode);
		}

	}

	return HistogramDescriptorNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::ParametricObjectMotionDescriptorDOMElement(DOM_Document doc, 
ParametricObjectMotionDescriptorInterfaceABC *theParametricObjectMotionDescriptorInterface)
{
	char str[STRING_MAXSIZE];

	// Write DOM Node
	int mCode,j;
	int noofpar;
	double *param,xorg,yorg;

	DOM_Element ParametricObjectMotionNode;
	ParametricObjectMotionNode = doc.createElement("ParametricObjectMotion");
		
	mCode = theParametricObjectMotionDescriptorInterface->GetModelCode();
	ParametricObjectMotionNode.setAttribute("ModelType", Str(mCode, str));

	xorg = theParametricObjectMotionDescriptorInterface->GetXOrigin();
	yorg = theParametricObjectMotionDescriptorInterface->GetYOrigin();
	ParametricObjectMotionNode.setAttribute("Xorigin", Str(xorg,str));
	ParametricObjectMotionNode.setAttribute("Yorigin", Str(yorg,str));

	switch (mCode){
		case Translational:
			noofpar=2;
			break;
		case Rotation_Scale:
			noofpar=4;
			break;
		case Affine:noofpar=6;
			break;
		case Perspective:
			noofpar=8;
			break;
		case Quadric:
			noofpar=12;
			break;
		default:
			noofpar=0;
	}

	
	param = theParametricObjectMotionDescriptorInterface->GetMotionParameters();
	for (j=0; j<noofpar; j++){
		DOM_Element MotionParametersNode;
		MotionParametersNode = doc.createElement("MotionParameters");
		ParametricObjectMotionNode.appendChild(MotionParametersNode);
		DOM_Text realValueNode;
		realValueNode = doc.createTextNode(Str(param[j],str));
		MotionParametersNode.appendChild(realValueNode);			
	}

	// There is a problem with the TimeDS information of the POM Descriptor.
	// At the moment, there is no time information carried into the POM Descriptor.
	DOM_Element TimeDSNode;
	TimeDSNode = doc.createElement("TimeDS");
	ParametricObjectMotionNode.appendChild(TimeDSNode);
	// THIS SHOULD CHANGE AS THE PARAMETRIC OBJECT MOTION HAS ONLY A START/END INTEGERS!!
	DOM_Element StartTimeNode;
	StartTimeNode = CreateIntegerElement(doc, "start", theParametricObjectMotionDescriptorInterface->GetStartTime());
	TimeDSNode.appendChild(StartTimeNode);
	DOM_Element EndTimeNode;
	EndTimeNode = CreateIntegerElement(doc, "end", theParametricObjectMotionDescriptorInterface->GetEndTime());
	TimeDSNode.appendChild(EndTimeNode);

	return ParametricObjectMotionNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::ContourShapeDescriptorDOMElement(DOM_Document doc, 
ContourShapeDescriptorInterfaceABC *theContourShapeDescriptorInterface)
{

	char str[STRING_MAXSIZE];

	// CSS Peaks Count
	DOM_Element ContourShapeNode;
	ContourShapeNode = doc.createElement("ContourShapeD");

	// DOM CODING SCHEME
	unsigned long lA, lB;

	unsigned int nPeaks = theContourShapeDescriptorInterface->GetNoOfPeaks() & CONTOURSHAPE_CSSPEAKMASK;
	ContourShapeNode.setAttribute("PeakCount",Str(nPeaks,str));

	// Highest Peak Y
	lA = theContourShapeDescriptorInterface->GetHighestPeakY();
	ContourShapeNode.setAttribute("HighestPeak",Str(lA,str));
	
	// Global Curvature Vector
	DOM_Element GlobalCurvatureVectorNode;
	GlobalCurvatureVectorNode = doc.createElement("GlobalCurvatureVector");
	ContourShapeNode.appendChild(GlobalCurvatureVectorNode);

	theContourShapeDescriptorInterface->GetGlobalCurvature(lA, lB);
	
	DOM_Element gcv1Node;
	gcv1Node = doc.createElement("gcv1");
	DOM_Text gcv1Value;
	gcv1Value = doc.createTextNode(Str(lA,str));
	GlobalCurvatureVectorNode.appendChild(gcv1Node);
	gcv1Node.appendChild(gcv1Value);

	DOM_Element gcv2Node;
	gcv2Node = doc.createElement("gcv2");
	DOM_Text gcv2Value;
	gcv2Value = doc.createTextNode(Str(lB,str));
	GlobalCurvatureVectorNode.appendChild(gcv2Node);
	gcv2Node.appendChild(gcv2Value);


	if (nPeaks)  {
    
		// Prototype Curvature Vector
		DOM_Element PrototypeCurvatureVectorNode;
		PrototypeCurvatureVectorNode = doc.createElement("PrototypeCurvatureVector");
		ContourShapeNode.appendChild(PrototypeCurvatureVectorNode);

	    theContourShapeDescriptorInterface->GetPrototypeCurvature(lA, lB);
	
		DOM_Element pcv1Node;
		pcv1Node = doc.createElement("pcv1");
		DOM_Text pcv1Value;
		pcv1Value = doc.createTextNode(Str(lA,str));
		PrototypeCurvatureVectorNode.appendChild(pcv1Node);
		pcv1Node.appendChild(pcv1Value);

		DOM_Element pcv2Node;
		pcv2Node = doc.createElement("pcv2");
		DOM_Text pcv2Value;
		pcv2Value = doc.createTextNode(Str(lB,str));
		PrototypeCurvatureVectorNode.appendChild(pcv2Node);
		pcv2Node.appendChild(pcv2Value);

	}

	// Peak values
	for (unsigned int c = 1; c < nPeaks; c++) {
    
		unsigned short iA, iB;
		DOM_Element ContourPeakNode;
		ContourPeakNode = doc.createElement("ContourPeak");
		ContourShapeNode.appendChild(ContourPeakNode);

		theContourShapeDescriptorInterface->GetPeak(c, iA, iB);
		
		DOM_Element xpeakNode;
		xpeakNode = CreateIntegerElement(doc, "xpeak", (int)iA) ;
		ContourPeakNode.appendChild(xpeakNode);

		DOM_Element ypeakNode;
		ypeakNode = CreateIntegerElement(doc, "ypeak", (int)iB) ;
		ContourPeakNode.appendChild(ypeakNode);

	}


	return ContourShapeNode;
}

//----------------------------------------------------------------------------
DOM_Element SegmentExtractionTool::CreateIntegerElement(DOM_Document &doc, 
														char* elemName, int value) 
{
	DOM_Element Node;
	Node = (DOM_Element)doc.createElement(elemName);

	char intValue[STRING_MAXSIZE];
	sprintf(intValue, "%d", value);
	DOM_Text intValueNode;
	intValueNode = doc.createTextNode(intValue);
	Node.appendChild(intValueNode);
	
	return Node;
}

//----------------------------------------------------------------------------
bool SegmentExtractionTool::SpatialConnectivity4(MomVop *mvMedia)
{
	// Search in the mvMedia (y_channel) for Spatial Connectivity.
	MomImage *miMask = mvMedia->y_chan;
	unsigned char *data = miMask->data->u;

	int pnt, x, y, pnt2, xx, yy;
	int *stack;
	int ptr;
	unsigned char *tmp;

	bool b = true; // return value
	
	// Set Up a stack and a temp image
	ptr = 0;
	stack = new int[miMask->x*miMask->y];
	tmp = new unsigned char[miMask->x*miMask->y];

	// Set tmp image to 0
	for(pnt=0; pnt<(miMask->x*miMask->y); pnt++) {
		tmp[pnt]=0;
	}

	// Find the first mask point
	for (pnt=0; pnt<(miMask->x*miMask->y); pnt++)
	{
		if (data[pnt]>0) {
			// Fill up the stack
			stack[0] = pnt;
			ptr = 1;
			break;
		}
	}
	
	// Start floading the tmp image
	while(ptr>0) {

		pnt = stack[--ptr];
		tmp[pnt]=1;

		x = int(pnt%(miMask->x));
		y = int(ceil(pnt/miMask->x));

		// Left
		xx=x-1; yy=y; 
		if (xx>=0) {
			pnt2 = yy*miMask->x + xx;
			if ( (data[pnt2]>0) && (tmp[pnt2]==0) ) {
				stack[ptr++] = pnt2;
			}
		}
		// Right
		xx=x+1;yy=y;
		if (xx<miMask->x) {
			pnt2 = yy*miMask->x + xx;
			if ( (data[pnt2]>0) && (tmp[pnt2]==0) ) {
				stack[ptr++] = pnt2;
			}
		}
		// Up
		xx=x;yy=y-1;
		if (yy>=0) {
			pnt2 = yy*miMask->x + xx;
			if ( (data[pnt2]>0) && (tmp[pnt2]==0) ) {
				stack[ptr++] = pnt2;
			}
		}
		// Down
		xx=x;yy=y+1;
		if (yy<miMask->y) {
			pnt2 = yy*miMask->x + xx;
			if ( (data[pnt2]>0) && (tmp[pnt2]==0) ) {
				stack[ptr++] = pnt2;
			}
		}
	}
	
	// Now tmp and data should be equal, if there is a tmp==0 but data>0 then is not
	// 4 connected.
	for (pnt=0; pnt<(miMask->x*miMask->y); pnt++) {
		if ( (tmp[pnt]==0) && (data[pnt]>0) )  { b=false;}
		if ( (tmp[pnt]==1) && (data[pnt]==0) ) {fprintf( stderr, "ERROR(SpatialConnectivity4): Something wierd happended here :-?.\n");exit(-1);}
	}

	// Delete stack and tmp image
	delete [] stack;
	delete [] tmp;

	return(b);
}

//----------------------------------------------------------------------------
char *SegmentExtractionTool::Str(double d, char *pc) 
{
	sprintf( pc, "%f", d);
	return pc;
}

//----------------------------------------------------------------------------
char *SegmentExtractionTool::Str(int i, char *pc) 
{
	sprintf( pc, "%d", i);
	return pc;
}

//----------------------------------------------------------------------------
char *SegmentExtractionTool::Str(unsigned int ui, char *pc) 
{
	sprintf( pc, "%u", ui);
	return pc;
}

//----------------------------------------------------------------------------
char *SegmentExtractionTool::Str(unsigned long ul, char *pc) 
{
	sprintf( pc, "%lu", ul);
	return pc;
}

//----------------------------------------------------------------------------
char *SegmentExtractionTool::Str(long l, char *pc) 
{
	sprintf( pc, "%ld", l);
	return pc;
}
