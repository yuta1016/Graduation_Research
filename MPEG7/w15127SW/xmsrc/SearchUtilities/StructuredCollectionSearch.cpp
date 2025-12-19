//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
//
// (contributing organizations names)

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
// Applicable File Name:  StructuredCollectionSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream>
#include <cstring> //LINUX_2011
#endif

#ifdef _VISUALC_

#define STREAMNS(stream) stream
#else
#define STREAMNS(stream) std::stream
#endif

//=============================================================================

using namespace XM;


const UUID StructuredCollectionSearchInterface::myID = UUID("");
const char *StructuredCollectionSearchInterface::myName = "StructuredCollectionMatchingInterface";

const UUID StructuredCollectionSearchTool::myID = UUID("");
const char *StructuredCollectionSearchTool::myName = "StructuredCollectionMatchingTool";

const UUID StructuredCollectionSearchInterfaceABC::myID = UUID();


//=============================================================================
StructuredCollectionSearchInterfaceABC::StructuredCollectionSearchInterfaceABC()
{
	SetInterfaceID(myID);
}


//=============================================================================
StructuredCollectionSearchInterface::StructuredCollectionSearchInterface(StructuredCollectionSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
StructuredCollectionSearchInterface::~StructuredCollectionSearchInterface()
{
}

//----------------------------------------------------------------------------
void StructuredCollectionSearchInterface::destroy()
{  delete m_SearchTool; }

//----------------------------------------------------------------------------
int StructuredCollectionSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *RefGenericDSInterface)
{
  return m_SearchTool->
    SetRefDescriptionInterface(RefGenericDSInterface);
}

//----------------------------------------------------------------------------
int StructuredCollectionSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *QueryGenericDSInterface)
{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double StructuredCollectionSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& StructuredCollectionSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
StructuredCollectionSearchTool::StructuredCollectionSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	// Ana: ColorHistogramDescriptor is not being used to matched the mean
	// color histograms

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StructuredCollectionSearchTool::StructuredCollectionSearchTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)
{
	SetQueryDescriptionInterface(theParent);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
StructuredCollectionSearchTool::~StructuredCollectionSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& StructuredCollectionSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *StructuredCollectionSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool StructuredCollectionSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool StructuredCollectionSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int StructuredCollectionSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)
{
  m_RefDescriptionInterface = aRefGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int  StructuredCollectionSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
  m_QueryDescriptionInterface=aQueryGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
double StructuredCollectionSearchTool::GetDistance(void)
{
	// ------------------------------------------------------------------------
	// Variables declaration
	// ------------------------------------------------------------------------
	
	double distance = 0.0;

	GenericDS ColStructRefDS, ColStructQueryDS;
	GenericDSInterfaceABC *pColStructRefDS, *pColStructQueryDS;
	string ColStructRefName, ColStructQueryName;


	// ------------------------------------------------------------------------
	// Check Descriptor schemes
	// ------------------------------------------------------------------------

    if (m_RefDescriptionInterface == NULL) {
		fprintf(stderr, "Null Reference DS\n");
		return DBL_MAX; 
	}
    if (m_QueryDescriptionInterface == NULL) {
		fprintf(stderr, "Null Query DS\n");
		return DBL_MAX;
	}

	if (!strcmp(m_RefDescriptionInterface->GetName(),
		   "GenericDSInterface")) {
	  return DBL_MAX;
	}
	if (!strcmp(m_QueryDescriptionInterface->GetName(),
		   "GenericDSInterface")) {
	  return DBL_MAX;
	}

	// Read StructuredCollection DS
	ColStructRefDS = m_RefDescriptionInterface->GetFirstChild();
	pColStructRefDS = ColStructRefDS.GetInterface();

	ColStructQueryDS = m_QueryDescriptionInterface->GetFirstChild();
	pColStructQueryDS = ColStructQueryDS.GetInterface();

	// Check the elements are collection structures
	if ( m_QueryDescriptionInterface->GetNumberOfChildren()!=1 ||
	     m_RefDescriptionInterface->GetNumberOfChildren()!=1) {
		fprintf(stderr, "Lacking StructuredCollection DS\n");
		return DBL_MAX;
	} else if (pColStructRefDS->isNull() || pColStructQueryDS->isNull()) {
		fprintf(stderr, "Null StructuredCollection DS\n");
		return DBL_MAX;
	} else {
		// Check names of root elements
		ColStructRefName = pColStructRefDS->GetDSName();
		ColStructQueryName = pColStructQueryDS->GetDSName();
		if ( ColStructRefName.compare(ColStructQueryName) ||
					ColStructRefName.compare("StructuredCollection") ) {
			fprintf(stderr, "Wrong DS name\n");
			return DBL_MAX;
		}
	}


	// ------------------------------------------------------------------------
	// Perform matching
	// ------------------------------------------------------------------------
	distance = StructuredCollectionMatching(pColStructRefDS,
					       pColStructQueryDS);


	// ------------------------------------------------------------------------
	// Print the Result
	// ------------------------------------------------------------------------
	//fprintf( stderr, "Distance between collections = %f\n", distance);


	return distance;
}

//----------------------------------------------------------------------------
double StructuredCollectionSearchTool::StructuredCollectionMatching(
	GenericDSInterfaceABC *pRefDSerface,
	GenericDSInterfaceABC *pQueryDSerface)
{
	double distance = 0.0;

	int iNumClassesRef, iNumClassesQuery;
	int *piNumImgsPerClassRef = NULL, *piNumImgsPerClassQuery = NULL;

	char **ppcClassLabelRef = NULL, **ppcClassLabelQuery = NULL;
	char ***pppcImagesPerClassRef = NULL, ***pppcImagesPerClassQuery = NULL;

	VectorR *pvrMeanColHistPerClassRef = NULL, *pvrMeanColHistPerClassQuery = NULL;
	VectorR *pvrVarColHistPerClassRef = NULL, *pvrVarColHistPerClassQuery = NULL;

	int i, j;


	// ------------------------------------------------------------------------
	// Decode the DSs to obtain the list of color histogram statistics 
	// for reference and query collection structures
	// ------------------------------------------------------------------------

	if (StructuredCollectionDecoding(pRefDSerface, &iNumClassesRef, &piNumImgsPerClassRef, 
			&ppcClassLabelRef, &pppcImagesPerClassRef, &pvrMeanColHistPerClassRef, 
			&pvrVarColHistPerClassRef) < 0) {
		fprintf(stderr, 
			"StructuredCollectionMatching: Error decoding Ref StructuredCollectionDS\n");
		distance = DBL_MAX;
	}

	if (StructuredCollectionDecoding(pQueryDSerface, &iNumClassesQuery, &piNumImgsPerClassQuery, 
			&ppcClassLabelQuery, &pppcImagesPerClassQuery, &pvrMeanColHistPerClassQuery,
			&pvrVarColHistPerClassQuery) < 0) {
		fprintf(stderr, 
			"StructuredCollectionMatching: Error decoding Query StructuredCollectionDS\n");
		distance = DBL_MAX;
	}


	// ------------------------------------------------------------------------
	// Testing
	// ------------------------------------------------------------------------


	// ------------------------------------------------------------------------
	// Calculate distance
	// ------------------------------------------------------------------------
	
	if (distance < DBL_MAX) {
		distance = ColorHistogramStatisticsMatching(
						iNumClassesRef, pvrMeanColHistPerClassRef, 
						iNumClassesQuery, pvrMeanColHistPerClassQuery);
	}


	// ------------------------------------------------------------------------
	// Release memory
	// ------------------------------------------------------------------------

	if (ppcClassLabelRef!=NULL) {
		for (i=0; i<iNumClassesRef; i++) {
			if (ppcClassLabelRef[i]!=NULL) {
				delete [] ppcClassLabelRef[i];
			}
		}
		delete [] ppcClassLabelRef;
	}
	if (pppcImagesPerClassRef!=NULL) {
		for (i=0; i<iNumClassesRef; i++) {
			if (pppcImagesPerClassRef[i]!=NULL) {
				for (j=0; j<piNumImgsPerClassRef[i]; j++) {
					if (pppcImagesPerClassRef[i][j]!=NULL) {
						delete [] pppcImagesPerClassRef[i][j];
					}
				}
				delete [] pppcImagesPerClassRef[i];
			}
		}
		delete [] pppcImagesPerClassRef;
	}
	if (piNumImgsPerClassRef!=NULL) {
		delete [] piNumImgsPerClassRef;
	}
	if (pvrMeanColHistPerClassRef!=NULL) {
		delete [] pvrMeanColHistPerClassRef;
	}
	if (pvrVarColHistPerClassRef!=NULL) {
		delete [] pvrVarColHistPerClassRef;
	}
	if (ppcClassLabelQuery!=NULL) {
		for (i=0; i<iNumClassesQuery; i++) {
			if (ppcClassLabelQuery[i]!=NULL) {
				delete [] ppcClassLabelQuery[i];
			}
		}
		delete [] ppcClassLabelQuery;
	}
	if (pppcImagesPerClassQuery!=NULL) {
		for (i=0; i<iNumClassesQuery; i++) {
			if (pppcImagesPerClassQuery[i]!=NULL) {
				for (j=0; j<piNumImgsPerClassQuery[i]; j++) {
					if (pppcImagesPerClassQuery[i][j]!=NULL) {
						delete [] pppcImagesPerClassQuery[i][j];
					}
				}
				delete [] pppcImagesPerClassQuery[i];
			}
		}
		delete [] pppcImagesPerClassQuery;
	}
	if (piNumImgsPerClassQuery!=NULL) {
		delete [] piNumImgsPerClassQuery;
	}
	if (pvrMeanColHistPerClassQuery!=NULL) {
		delete [] pvrMeanColHistPerClassQuery;
	}
	if (pvrVarColHistPerClassQuery!=NULL) {
		delete [] pvrVarColHistPerClassQuery;
	}


	return distance;
}

//----------------------------------------------------------------------------
int StructuredCollectionSearchTool::StructuredCollectionDecoding(
	GenericDSInterfaceABC *pDSInt, 
	int *piNumClasses, int **ppiNumImgsPerClass, 
	char ***pppcClassLabel, char ****ppppcImagesPerClass,
	VectorR **ppvrMeanColHistPerClass, VectorR **ppvrVarColHistPerClass)
{
	// ------------------------------------------------------------------------
	//   Variables declaration
	// ------------------------------------------------------------------------

	int iNumClasses;
	int *piNumImgsPerClass = NULL;

	char **ppcClassLabel = NULL;
	char ***pppcImagesPerClass = NULL;

	VectorR *pvrMeanColHistPerClass = NULL;
	VectorR *pvrVarColHistPerClass = NULL;

	const int STRING_MAXSIZE = 4096;
	char c_str[STRING_MAXSIZE];
	string str;
	int i, j, size;
	

	// ------------------------------------------------------------------------
	//   Decode StructuredCollection description to obtain decription
	//   of classes in the collection (e.g. class labels, and color 
	//   histogram statistics)
	// ------------------------------------------------------------------------

	// Number of classes is the number of ClusterModels minus two:
	// the root collection model and the graph
	iNumClasses = pDSInt->GetNumberOfChildren() - 2;

	// Allocate memory
	piNumImgsPerClass = new int[iNumClasses];
	ppcClassLabel = new char*[iNumClasses];
	pppcImagesPerClass = new char**[iNumClasses];
	pvrMeanColHistPerClass = new VectorR[iNumClasses];
	pvrVarColHistPerClass = new VectorR[iNumClasses];

	// Root CollectionModel DS
	GenericDS RColClusterDS = pDSInt->GetChild(0);
	if ( RColClusterDS.GetDSName().compare("CollectionModel") ) {
		fprintf(stderr, 
			"CollectionModel DS not found as expected in StructuredCollection DS\n");
		return -1;
	}

	// All ClusterModel
	for (i=0; i<iNumClasses; i++) {

		// ClusterModel
		GenericDS ColClusterDS = pDSInt->GetChild(i+1);
		if ( ColClusterDS.GetDSName().compare("ClusterModel") ) {
			fprintf(stderr, 
				"ClusterModel DS not found as expected in StructuredCollection DS\n");
			return -1;
		}

		// Label
		GenericDS LabelDS = ColClusterDS.GetFirstChild();
		if ( LabelDS.GetDSName().compare("Label") ) {
			fprintf(stderr, "Label DS not found as expected in ClusterModel DS\n");
			return -1;
		}
		LabelDS.GetTextValue(str);
		ppcClassLabel[i] = new char[STRING_MAXSIZE];
		strcpy(ppcClassLabel[i], str.c_str());

		// ContentCollection (Collection)
		GenericDS CollectionDS = LabelDS.GetNextSibling();
		if ( CollectionDS.GetDSName().compare("Collection") ) {
			fprintf(stderr, 
				"Collection DS not found as expected in ClusterModel DS\n");
			return -1;
		}
		CollectionDS.GetTextAttribute("xsi:type", str);
		if ( str.compare("ContentCollectionType") ) {
			fprintf(stderr, 
				"ContentCollection DS not found as expected in ClusterModel DS\n");
			return -1;
		}

		// Representative icons in Summarization DS
		GenericDS RepIconsDS = CollectionDS.GetFirstChild();
		if ( RepIconsDS.GetDSName().compare("Summarization") ) {
			fprintf(stderr, 
				"Summarization DS not found as expected in ContentCollection DS\n");
			return -1;
		}

		// No checking names from now and on

		// Locator to images in clusters
		// Allocate memory
		// Number of images is the number of Content DSs minus 1: the summarization
		piNumImgsPerClass[i] = CollectionDS.GetNumberOfChildren() - 1;
		pppcImagesPerClass[i] = new char*[piNumImgsPerClass[i]];

		for (j=0; j<piNumImgsPerClass[i]; j++) {

			// Content DS
			GenericDS ContentDS = CollectionDS.GetChild(j+1);

			// Segment DSs (Still Region)
			GenericDS ImageDS = ContentDS.GetFirstChild();

			// Location of image using Media Information DS in Segment DS
			GenericDS MediaLocatorDS = ImageDS.GetFirstChild();
			GenericDS MediaUriDS = MediaLocatorDS.GetFirstChild();

			pppcImagesPerClass[i][j] = new char[STRING_MAXSIZE];	
			MediaUriDS.GetTextValue(str);
			strcpy(pppcImagesPerClass[i][j], str.c_str());
		}

		// DescriptorModel for scalable color
		GenericDS DescriptorModelDS = CollectionDS.GetNextSibling();
		if ( DescriptorModelDS.GetDSName().compare("DescriptorModel") ) {
			fprintf(stderr, 
				"DescriptorModelDS DS not found as expected in ClusterModel DS\n");
			return -1;
		}
		GenericDS DescriptorDS = DescriptorModelDS.GetFirstChild();
		if ( DescriptorDS.GetDSName().compare("Descriptor") ) {
			fprintf(stderr, 
				"DescriptorDS DS not found as expected in DescriptorModel DS\n");
			return -1;
		}
		DescriptorDS.GetTextAttribute("xsi:type", str);
		if ( str.compare("ScalableColorType") ) {
			fprintf(stderr, 
				"ScalableColor DS not found as expected in DescriptorModel DS\n");
			return -1;
		}

		// ProbabilityModel (ProbabilityDistribution)
		GenericDS ProbabilityModelDS = DescriptorModelDS.GetNextSibling();
		if ( ProbabilityModelDS.GetDSName().compare("ProbabilityModel") ) {
			fprintf(stderr, 
				"ProbabilityModel DS not found as expected in ProbabilityModelClass DS\n");
			return -1;
		}
		ProbabilityModelDS.GetTextAttribute("xsi:type", str);
		if ( str.compare("ProbabilityDistributionType") ) {
			fprintf(stderr, 
				"ProbabilityDistribution DS not found as expected in ProbabilityModelClass DS\n");
			return -1;
		}

		// Mean for color histogram
		GenericDS MeanDS = ProbabilityModelDS.GetFirstChild();
		if ( MeanDS.GetDSName().compare("Mean") ) {
			fprintf(stderr, "Mean DS not found as expected in ClusterStatistics DS\n");
			return -1;
		}
		MeanDS.GetIntAttribute("dim", size);
		MeanDS.GetTextValue(str);
		strcpy(c_str, str.c_str());

		VectorR mean(size);
		float *m = mean.data_nc();
		//strstream vstrM(c_str /*, strlen(c_str)*/);
		STREAMNS(strstream) vstrM(c_str , strlen(c_str));
		for (j=0; j<size; j++) {
			vstrM >> *m++;
		}
		pvrMeanColHistPerClass[i] = mean;
		
		// Variance for color histogram
		GenericDS VarDS = MeanDS.GetNextSibling();
		if ( VarDS.GetDSName().compare("Variance") ) {
			fprintf(stderr, "Variance DS not found as expected in ClusterStatistics DS\n");
			return -1;
		}
		VarDS.GetIntAttribute("dim", size);
		VarDS.GetTextValue(str);
		strcpy(c_str, str.c_str());

		VectorR var(size);
		float *v = var.data_nc();
		STREAMNS(istrstream) vstrV(c_str, strlen(c_str));
		for (j=0; j<size; j++) {
			vstrV >> *v++;
		}
		pvrVarColHistPerClass[i] = var;

	}

	// Graph
	GenericDS GraphDS = pDSInt->GetChild(i+1);
	if ( GraphDS.GetDSName().compare("Graph") ) {
		fprintf(stderr, 
			"Graph DS not found as expected in StructuredCollection DS\n");
		return -1;
	}

	// Collection Decomposition
	GenericDS CollectionDecompDS = GraphDS.GetFirstChild();
	if ( CollectionDecompDS.GetDSName().compare("Relation") ) {
		fprintf(stderr, 
			"Relation DS not found as expected in Graph DS\n");
		return -1;
	}
	CollectionDecompDS.GetTextAttribute("xsi:type", str);
	if ( str.compare("CollectionDecompositionType") ) {
		fprintf(stderr, 
			"CollectionDecomposition DS not found as expected in Graph DS\n");
		return -1;
	}

	// Checking number of arguments in collection decomposition
	// The values of the source and filler attributes of Collection 
	// Decomposition and  
	// Arguments could be checked
	if ( iNumClasses != CollectionDecompDS.GetNumberOfChildren()-1) {
		fprintf(stderr, "Wrong number of Argument DSs in CollectionDecomposition DS\n");
		return -1;
	}


	// ------------------------------------------------------------------------
	//   Return parameters
	// ------------------------------------------------------------------------

	(*piNumClasses) = iNumClasses;
	(*ppiNumImgsPerClass) = piNumImgsPerClass;
	(*pppcClassLabel) = ppcClassLabel;
	(*ppppcImagesPerClass) = pppcImagesPerClass;
	(*ppvrMeanColHistPerClass) = pvrMeanColHistPerClass;
	(*ppvrVarColHistPerClass) = pvrVarColHistPerClass;

	return 0;
}

//----------------------------------------------------------------------------
double StructuredCollectionSearchTool::ColorHistogramStatisticsMatching(
	int iNumClassesRef, VectorR *pvrMeanColHistPerClassRef, 
	int iNumClassesQuery, VectorR *pvrMeanColHistPerClassQuery)
{
	double distance = 0.0;
	int i, j;


	// For each query class obtain minimum distance to reference classes
	for (i=0; i<iNumClassesQuery; i++) {
		VectorR a = pvrMeanColHistPerClassQuery[i];
		a.normalize();

		// Minimum distance from query class i to reference classes
		double minDist = DBL_MAX;
		for (j=0; j<iNumClassesRef; j++) {
			
			VectorR b = pvrMeanColHistPerClassRef[j];
			b.normalize();

			double dist = (double) VectorR::mse(a, b);
			minDist = (dist < minDist) ? dist : minDist;
		}

		// Accumulate minimum distances to calculate mean
		distance += minDist;
	}

	// Average min distances
	distance /= iNumClassesQuery;


	return distance;
}

//----------------------------------------------------------------------------
StructuredCollectionSearchInterfaceABC *StructuredCollectionSearchTool::GetInterface(void)
{ return &m_Interface; }

