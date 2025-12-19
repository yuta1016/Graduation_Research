//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  StructuredCollectionExtraction.cpp
//

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream>
#endif

#define STREAMNS(stream) std::stream //LINUX_2011

//=============================================================================

using namespace XM;


const UUID StructuredCollectionExtractionInterface::myID = UUID("");
const char *StructuredCollectionExtractionInterface::myName = "StructuredCollectionDSExtractionInterface";

const UUID StructuredCollectionExtractionTool::myID = UUID("");
const char *StructuredCollectionExtractionTool::myName = "StructuredCollectionExtraction";
const int	StructuredCollectionExtractionTool::STRING_MAXSIZE= 4096;	// maximum size of the strings

const UUID StructuredCollectionExtractionInterfaceABC::myID = UUID();


//=============================================================================
StructuredCollectionExtractionInterfaceABC::StructuredCollectionExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
StructuredCollectionExtractionInterface::StructuredCollectionExtractionInterface(StructuredCollectionExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
StructuredCollectionExtractionInterface::~StructuredCollectionExtractionInterface()
{
}

//----------------------------------------------------------------------------
void StructuredCollectionExtractionInterface::destroy()
{
  delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionInterface::SetSourceMedia(char *pcClassName, 
	char *pcImageName)
{
  return m_ExtractionTool->SetSourceMedia(pcClassName, pcImageName);
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  return m_ExtractionTool->SetDescriptionInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& StructuredCollectionExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
StructuredCollectionExtractionTool::StructuredCollectionExtractionTool():
m_Interface(this),
m_DescriptionInterface(0)
{
	ScalableColorExtractionTool *scExtraction;
	scExtraction = new ScalableColorExtractionTool(0);

	m_ScalableColorExtraction = scExtraction->GetInterface();
	m_ScalableColorDescriptor = m_ScalableColorExtraction->GetDescriptorInterface();
	
	m_Media = new MultiMedia;
	m_MediaInterface = m_Media->GetInterface();
	m_RamDB.SetMedia(m_MediaInterface);

	m_ScalableColorExtraction->SetSourceMedia(m_MediaInterface);
	//m_ScalableColorDescriptor->SetValuesFromParameterFile();

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StructuredCollectionExtractionTool::
StructuredCollectionExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptionInterface(0)
{
	ScalableColorExtractionTool *scExtraction;
	scExtraction = new ScalableColorExtractionTool(0);

	m_ScalableColorExtraction = scExtraction->GetInterface();
	m_ScalableColorDescriptor = m_ScalableColorExtraction->GetDescriptorInterface();

	m_Media = new MultiMedia;
	m_MediaInterface = m_Media->GetInterface();
	m_RamDB.SetMedia(m_MediaInterface);

	m_ScalableColorExtraction->SetSourceMedia(m_MediaInterface);
	m_ScalableColorDescriptor->SetValuesFromParameterFile();

	SetDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StructuredCollectionExtractionTool::~StructuredCollectionExtractionTool()
{
	/* Release descriptor */
	if (m_ScalableColorExtraction) {
	    m_ScalableColorExtraction->destroy();
	    m_ScalableColorExtraction = 0;
	}
	if (m_MediaInterface) {
		m_MediaInterface->destroy();
		m_MediaInterface = 0;
	}
	
	m_RamDB.FreeDatabase();
}	

//----------------------------------------------------------------------------
bool StructuredCollectionExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool StructuredCollectionExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::SetSourceMedia(char *pcClassName, char *pcImageName)
{

  	m_pcClassName = pcClassName;
	m_pcImageName = pcImageName;

	return(0);
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DescriptionInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionExtractionTool::InitExtracting(void)
{

	/* Do not need this */

	return 0;
}

//----------------------------------------------------------------------------
unsigned long StructuredCollectionExtractionTool::StartExtracting(void)
{

	fprintf( stderr, "Extraction of Collection Structure\n");

	ExtractStructuredCollection(m_DescriptionInterface);

	return 0;
}

//----------------------------------------------------------------------------
const UUID& StructuredCollectionExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
StructuredCollectionExtractionInterfaceABC *StructuredCollectionExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
void StructuredCollectionExtractionTool::ExtractStructuredCollection(
											GenericDSInterfaceABC *parentDSInterface)
{

	/* ------------------------------------------------------------------------
	   Variables declaration
	   ------------------------------------------------------------------------ */

	int iNumClasses;
	int *piNumImgsPerClass = NULL;

	char **ppcClassLabel = NULL;
	char ***pppcImagesPerClass = NULL;

	VectorR *pvrMeanSclColPerClass = NULL;
	VectorR *pvrVarSclColPerClass = NULL;

	int i, j;

	/* ------------------------------------------------------------------------
	   Read class files: number of classes, classes names, and images per class
	   ------------------------------------------------------------------------ */
	if ( ReadClassFile(&iNumClasses, &piNumImgsPerClass, &ppcClassLabel, &pppcImagesPerClass)
		 > -1 ) {


	/* ------------------------------------------------------------------------
	   Extract mean and variance scalable color for the images in each class
	   ------------------------------------------------------------------------ */

	if ( ScalableColorStatisticsExtraction(iNumClasses, piNumImgsPerClass, pppcImagesPerClass,
					&pvrMeanSclColPerClass, &pvrVarSclColPerClass)
					> -1 ) {
	

	/* ------------------------------------------------------------------------
	   Create the StructuredCollection DS
	   ------------------------------------------------------------------------ */

		StructuredCollectionEncoding(parentDSInterface, iNumClasses, piNumImgsPerClass, ppcClassLabel,
					pppcImagesPerClass,	pvrMeanSclColPerClass, pvrVarSclColPerClass);

	}
	}

	/* ------------------------------------------------------------------------
	   Release memory
	   ------------------------------------------------------------------------ */

	if (ppcClassLabel!=NULL) {
		for (i=0; i<iNumClasses; i++) {
			if (ppcClassLabel[i]!=NULL) {
				delete [] ppcClassLabel[i];
			}
		}
		delete [] ppcClassLabel;
	}
	if (pppcImagesPerClass!=NULL) {
		for (i=0; i<iNumClasses; i++) {
			if (pppcImagesPerClass[i]!=NULL) {
				for (j=0; j<piNumImgsPerClass[i]; j++) {
					if (pppcImagesPerClass[i][j]!=NULL) {
						delete [] pppcImagesPerClass[i][j];
					}
				}
				delete [] pppcImagesPerClass[i];
			}
		}
		delete [] pppcImagesPerClass;
	}
	if (piNumImgsPerClass!=NULL) {
		delete [] piNumImgsPerClass;
	}
	if (pvrMeanSclColPerClass!=NULL) {
		delete [] pvrMeanSclColPerClass;
	}
	if (pvrVarSclColPerClass!=NULL) {
		delete [] pvrVarSclColPerClass;
	}

}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::ReadClassFile(int *piNumClasses, 
		int **ppiNumImgsPerClass, char ***pppcClassLabel, char ****ppppcImagesPerClass)
					
{
	/* Variable declaration */
	int iNumClasses;
	int *piNumImgsPerClass = NULL;

	char **ppcClassLabel = NULL;
	char ***pppcImagesPerClass = NULL;

	FILE *fd;
	int i, j;
	char str[STRING_MAXSIZE];


	/* ------------------------------------------------------------------------
	   Find out the number of classes 
	   ------------------------------------------------------------------------ */

	/* Open class file */
	fd = fopen( m_pcClassName, "r");
	if (fd == NULL) {
		fprintf( stderr, "ERROR(StructuredCollectionExtraction): Couldn't not find %s file.\n", 
			m_pcClassName);
		return -1;
	}

	/* Read number of classes */
	if (fscanf(fd, "%d", &iNumClasses) == EOF) {
		fprintf( stderr, 
			"ERROR(StructuredCollectionExtraction): Couldn't not read number of classes.\n");
		return -1;
	}

	/* ------------------------------------------------------------------------
	   Read class labels and images for each class from class file
	   ------------------------------------------------------------------------ */

	/* Allocate memory for variables */
	piNumImgsPerClass = new int[iNumClasses];
	ppcClassLabel = new char*[iNumClasses];
	pppcImagesPerClass = new char**[iNumClasses];

	for (i=0; i<iNumClasses; i++) {

		/* Allocate memory for image names */
		ppcClassLabel[i] = new char[STRING_MAXSIZE];

		/* Read class label and number of images in class */	
		if (fscanf(fd, "%s%d", ppcClassLabel[i], &piNumImgsPerClass[i]) == EOF) {
			fprintf( stderr, 
				"ERROR(StructuredCollectionExtraction): Wrong format of file %s.\n",
				m_pcClassName);
			return -1;
		}

		/* Allocate memory for image names */
		pppcImagesPerClass[i] = new char*[piNumImgsPerClass[i]];

		/* Read name of images in class */
		for (j=0; j<piNumImgsPerClass[i]; j++) {
			pppcImagesPerClass[i][j] = new char[STRING_MAXSIZE];
			if (fscanf(fd, "%s", str) == EOF) {
				fprintf( stderr, 
					"ERROR(StructuredCollectionExtraction): Wrong format of file %s.\n",
					m_pcClassName);
				return -1;
			}
			sprintf(pppcImagesPerClass[i][j], m_pcImageName, str);
		}
	}

	fclose(fd);

	/* Testing */
	fprintf(stdout, "%d\n", iNumClasses);
	for (i=0; i<iNumClasses; i++) {
		fprintf(stdout, "%s %d ", ppcClassLabel[i], piNumImgsPerClass[i]);
		for (j=0; j<piNumImgsPerClass[i]; j++) {
			fprintf(stdout, "%s ", pppcImagesPerClass[i][j]);
		}
		fprintf(stdout, "\n");
	}

	/* Return values */
	(*piNumClasses) = iNumClasses;
	(*ppiNumImgsPerClass) = piNumImgsPerClass;
	(*pppcClassLabel) = ppcClassLabel;
	(*ppppcImagesPerClass) = pppcImagesPerClass;

	return 0;
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::ScalableColorStatisticsExtraction(
	int iNumClasses, int *piNumImgsPerClass, char ***pppcImagesPerClass, 
	VectorR **ppvrMeanSclColPerClass, VectorR **ppvrVarSclColPerClass)
{
	VectorR *pvrMeanSclColPerClass, *pvrVarSclColPerClass;
	int i, j;

	// Allocate memory
	pvrMeanSclColPerClass = new VectorR[iNumClasses];
	pvrVarSclColPerClass = new VectorR[iNumClasses];

	// For each class
	for (i=0; i<iNumClasses; i++) {

		// Extract scalable color for all the images in class 
		// Accumulate the mean and the variance scalable color
		if (piNumImgsPerClass[i] > 0) {
			VectorR CH, MCH, VCH;

			if (ScalableColorExtraction(pppcImagesPerClass[i][0], &CH)<0) { 
				return -1; 
			}
			MCH = CH;
			CH *= CH;
			VCH = CH;
			for (j=1; j<piNumImgsPerClass[i]; j++) {
				if (ScalableColorExtraction(pppcImagesPerClass[i][j], &CH)<0) { 
					return -1; 
				}
				MCH += CH;
				CH *= CH;
				VCH += CH;
			}

			// Mean scalable color
			MCH /= piNumImgsPerClass[i];
			pvrMeanSclColPerClass[i] = MCH;

			// Variance scalable color
			VCH /= piNumImgsPerClass[i];
			MCH *= MCH;
			VCH -= MCH;
			pvrVarSclColPerClass[i] = VCH;

			// Testing
			std::cout << "MEAN FOR CLASS " << i << std::endl;
			pvrMeanSclColPerClass[i].write(std::cout);
			std::cout << std::endl;

			std::cout << "VARIANCE FOR CLASS " << i << std::endl;
			pvrVarSclColPerClass[i].write(std::cout);
			std::cout << std::endl;

		} else {
			pvrMeanSclColPerClass[i].clear();
			pvrVarSclColPerClass[i].clear();
		}

	}

	// Return parameters
	(*ppvrMeanSclColPerClass) = pvrMeanSclColPerClass;
	(*ppvrVarSclColPerClass) = pvrVarSclColPerClass;

	return 0;
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::ScalableColorExtraction(char *image, 
	VectorR *pvrSclCol) 
{

	/* ------------------------------------------------------------------------
	   Extract scalable color for image using XM software
	   ------------------------------------------------------------------------ */

	/* malloc media and load input data*/
	if (!m_RamDB.LoadNamedImage(image)) {
		fprintf(stderr, "Image could not be loaded successfully: %s.", image);
		return -1;
	}

	/* select VOP layer*/
	if (m_MediaInterface->GetImage()->a_chan) {
	  m_MediaInterface->SelectVopAlphaLayer(ForegroundFromAlpha);
	}
	else {
	  m_MediaInterface->SelectVopAlphaLayer(BackgroundFromAlpha);
	}


#ifdef XYUVDISP
	  /* display input image*/
	  xyuv_show_simple(0,(unsigned short *)media->y_chan->data->s,
			  (unsigned short *)media->u_chan->data->s,
			  (unsigned short *)media->v_chan->data->s,
			    datasize(media->y_chan)&1,
			    media->y_chan->x,media->y_chan->y,
			    media->u_chan->x,media->u_chan->y,
			    "input");
#endif


	/* init extract descriptor from media */
	m_ScalableColorExtraction->InitExtracting();

	/* extract descriptor from media */
	m_ScalableColorExtraction->StartExtracting();


	/* ------------------------------------------------------------------------
	   Load scalable color values in vector of reals
	   ------------------------------------------------------------------------ */

	int size = m_ScalableColorDescriptor->
	  /*GetScalableHistogramDescriptorInterface()->*/
	  GetNumberOfCoefficients();
	VectorR vrSclCol(size);
	float *g = vrSclCol.data_nc();

	for (int i=0; i<size; i++) {
		*g++ = (float) m_ScalableColorDescriptor->
		  /*GetScalableHistogramDescriptorInterface()->*/
		  GetCoefficient(i);
	}
	
	// Testing 
	//vrSclCol.write(cout);
	//cout << endl;

	/* free media */
	m_MediaInterface->FreeImage();

	// Return parameters
	(*pvrSclCol) = vrSclCol;

	return 0;
}

//----------------------------------------------------------------------------
int StructuredCollectionExtractionTool::StructuredCollectionEncoding(
	GenericDSInterfaceABC *pDSInt,
	int iNumClasses, int *piNumImgsPerClass, char **ppcClassLabel, 
	char ***pppcImagesPerClass,	VectorR *pvrMeanSclColPerClass, 
	VectorR *pvrVarSclColPerClass)
{

	char str[STRING_MAXSIZE];
	int i, j;

	/* ------------------------------------------------------------------------
	   Collection Structure DS
	   ------------------------------------------------------------------------ */

	GenericDS StructuredCollectionDS = pDSInt->CreateChild("StructuredCollection");

	// Namespaces
	StructuredCollectionDS.SetAttribute("xmlns", "http://www.mpeg7.org/2001/MPEG-7_Schema");
	StructuredCollectionDS.SetAttribute("xmlns:xsi", 
		"http://www.w3.org/2000/10/XMLSchema-instance");


	/* ------------------------------------------------------------------------
	   Collection Model DS
	   Represents entire collection
	   ------------------------------------------------------------------------ */

	GenericDS RClusterDS = 
		StructuredCollectionDS.CreateChild("CollectionModel");
	RClusterDS.SetAttribute("id", "CC_0");
	RClusterDS.SetAttribute("confidence", "1.0");

	GenericDS RLabelDS = RClusterDS.CreateChild("Label");
	GenericDS RNameDS = RLabelDS.CreateChild("Name");
	RNameDS.SetAttribute("xml:lan", "en-us");
	RNameDS.SetValue("MPEG-7 Color Image Collection");

	// Collection DS
	GenericDS RCollectionDS = RClusterDS.CreateChild("Collection");
	RCollectionDS.SetAttribute("xsi:type", "ContentCollectionType");
	int iSize = 0;

	for (i=0; i<iNumClasses; i++) {
		for (j=0; j<piNumImgsPerClass[i]; j++) {

			// Content DS
			GenericDS ContentDS = RCollectionDS.CreateChild("Content");
			ContentDS.SetAttribute("xsi:type", "ImageType");

			// Image DS
			GenericDS ImageDS = ContentDS.CreateChild("Image");

			// Location of image using MediaLocator in Segment DS
			GenericDS MediaLocatorDS = ImageDS.CreateChild("MediaLocator");
			GenericDS MediaUriDS = MediaLocatorDS.CreateChild("MediaUri");
			MediaUriDS.SetValue(pppcImagesPerClass[i][j]);

			iSize += piNumImgsPerClass[i];
		}
	}
	RCollectionDS.SetAttribute("size", iSize);


	/* ------------------------------------------------------------------------
	   Collection Cluster DS
	   for each class
	   ------------------------------------------------------------------------ */
		
	for (i=0; i<iNumClasses; i++) {

		// Collection Cluster DS
		GenericDS ClusterDS = StructuredCollectionDS.
		  CreateChild("ClusterModel");
		sprintf(str, "CC_0_%d", i);
		ClusterDS.SetAttribute("id", str);
		ClusterDS.SetAttribute("confidence", "1.0");

		// Label DS
		GenericDS LabelDS = ClusterDS.CreateChild("Label");
		GenericDS NameDS = LabelDS.CreateChild("Name");
		NameDS.SetAttribute("xml:lan", "en-us");
		NameDS.SetValue(ppcClassLabel[i]);

		// Collection DS
		GenericDS CollectionDS = ClusterDS.CreateChild("Collection");
		CollectionDS.SetAttribute("xsi:type", "ContentCollectionType");

		// Summarization information: Representative icon
		GenericDS SummarizationDS = CollectionDS.CreateChild("Summarization");
		GenericDS HierarchicalSummaryDS = SummarizationDS.CreateChild("Summary");
		HierarchicalSummaryDS.SetAttribute("xsi:type", "HierarchicalSummaryType");
		HierarchicalSummaryDS.SetAttribute("components", "keyFrames");
		HierarchicalSummaryDS.SetAttribute("hierarchy", "independent");
		GenericDS SNameDS = HierarchicalSummaryDS.CreateChild("Name");
		NameDS.SetValue("Key Icons");
		GenericDS HightlightSummaryDS = 
			HierarchicalSummaryDS.CreateChild("HightlightSummary");
		GenericDS HightlightSegmentDS = 
			HightlightSummaryDS.CreateChild("HightlightSegment");
		GenericDS ImageLocatorDS = HightlightSegmentDS.CreateChild("KeyFrame");
		GenericDS MediaUriDS = ImageLocatorDS.CreateChild("MediaUri");
		MediaUriDS.SetValue(pppcImagesPerClass[i][0]);

		// For each member of the class, create an Image DS
		// with locator including the image name 
		for (j=0; j<piNumImgsPerClass[i]; j++) {

			// Content DS
			GenericDS ContentDS = CollectionDS.CreateChild("Content");
			ContentDS.SetAttribute("xsi:type", "ImageType");

			// Image DS
			GenericDS ImageDS = ContentDS.CreateChild("Image");

			// Location of image using Media Information DS in Segment DS
			GenericDS MediaLocatorDS = ImageDS.CreateChild("MediaLocator");
			GenericDS MediaUriDS = MediaLocatorDS.CreateChild("MediaUri");
			MediaUriDS.SetValue(pppcImagesPerClass[i][j]);
		}

		//sth use ExportDDL!!
		// Probability DescriptorModel DS
		GenericDS DescriptorModelDS = 
			ClusterDS.CreateChild("DescriptorModel"); 
		GenericDS DescriptorDS = 
			DescriptorModelDS.CreateChild("Descriptor"); 
		DescriptorDS.SetAttribute("xsi:type", "ScalableColorType");
		DescriptorDS.SetAttribute("numberOfCoefficients", 
			pvrMeanSclColPerClass[i].length());
		DescriptorDS.SetAttribute("numberOfBitplancesDiscarded", "0");
		GenericDS CoefficientsDS = 
			DescriptorDS.CreateChild("Coefficients"); 
		CoefficientsDS.SetAttribute("dim", pvrMeanSclColPerClass[i].length());
		strcpy(str, "");
		for (j=0; j<pvrMeanSclColPerClass[i].length(); j++) {
			sprintf(str, "%s %d", str, j);
		}
		CoefficientsDS.SetValue(str);
		GenericDS FieldDS = 
			DescriptorModelDS.CreateChild("Field"); 
		FieldDS.SetValue("../Descriptor/Coefficients");

		// Probability Model DS
		GenericDS ProbabilityModelDS = ClusterDS.CreateChild("ProbabilityModel"); 
		ProbabilityModelDS.SetAttribute("xsi:type", "ProbabilityDistributionType");
		ProbabilityModelDS.SetAttribute("dim", pvrMeanSclColPerClass[i].length());

		// Mean DS for scalable color
		GenericDS MeanDS = ProbabilityModelDS.CreateChild("Mean");
		MeanDS.SetAttribute("dim", pvrMeanSclColPerClass[i].length());
		STREAMNS(strstream) vstrM;
		for (j = 0; j<pvrMeanSclColPerClass[i].length(); j++) {
			vstrM << *(pvrMeanSclColPerClass[i].data(j)) << " ";
		}
		vstrM << '\0';
		MeanDS.SetValue(vstrM.str());
		delete [] vstrM.str();

		// Variance DS for scalable color
		GenericDS VarDS = ProbabilityModelDS.CreateChild("Variance");
		VarDS.SetAttribute("dim", pvrVarSclColPerClass[i].length());
		STREAMNS(strstream) vstrV;
		for (j = 0; j<pvrVarSclColPerClass[i].length(); j++) {
			vstrV << *(pvrVarSclColPerClass[i].data(j)) << " ";
		}
		vstrV << '\0';
		VarDS.SetValue(vstrV.str());
		delete [] vstrV.str();
	}


	/* ------------------------------------------------------------------------
	   Graph DS
	   ------------------------------------------------------------------------ */

	GenericDS GraphDS = StructuredCollectionDS.CreateChild("Graph");


	/* ------------------------------------------------------------------------
	   Cluster Decomposition DS
	   ------------------------------------------------------------------------ */

	GenericDS CollectionDecompositionDS = 
		GraphDS.CreateChild("Relation");
	CollectionDecompositionDS.SetAttribute("xsi:type", "CollectionDecompositionType");
	CollectionDecompositionDS.SetAttribute("decompositionType", "Semantic");
	CollectionDecompositionDS.SetAttribute("overlaps", "false");
	CollectionDecompositionDS.SetAttribute("gaps", "false");

	GenericDS SArgumentDS =  CollectionDecompositionDS.CreateChild("Argument");
	SArgumentDS.SetValue("CC_0");

	for (i=0; i<iNumClasses; i++) {
		sprintf(str, "CC_0_%d", i);
		GenericDS TArgumentDS =  CollectionDecompositionDS.CreateChild("Argument");
		TArgumentDS.SetValue(str);
	}

	return 0;
}

