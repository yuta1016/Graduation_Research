//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Thorsten Heinz
// Fraunhofer IIS
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AudioSpectralFlatnessSearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;


const UUID AudioSpectralFlatnessSearchInterface::myID = UUID("");
const char *AudioSpectralFlatnessSearchInterface::myName = 
                                      "AudioSpectralFlatnessMatchingInterface";

const UUID AudioSpectralFlatnessSearchTool::myID = UUID("");
const char *AudioSpectralFlatnessSearchTool::myName = 
                                      "AudioSpectralFlatnessMatchingTool";

const UUID AudioSpectralFlatnessSearchInterfaceABC::myID = UUID();


//=============================================================================
AudioSpectralFlatnessSearchInterfaceABC::AudioSpectralFlatnessSearchInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
AudioSpectralFlatnessSearchInterface::AudioSpectralFlatnessSearchInterface(AudioSpectralFlatnessSearchTool* aTool):
m_SearchTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AudioSpectralFlatnessSearchInterface::~AudioSpectralFlatnessSearchInterface()
{
}

//----------------------------------------------------------------------------
const UUID& AudioSpectralFlatnessSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AudioSpectralFlatnessSearchInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void AudioSpectralFlatnessSearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int AudioSpectralFlatnessSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *RefAudioSpectralFlatnessDescriptionInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(RefAudioSpectralFlatnessDescriptionInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* AudioSpectralFlatnessSearchInterface::
GetQueryDescriptionInterface(void)

{
  return m_SearchTool->GetQueryDescriptionInterface();
}

//----------------------------------------------------------------------------
int AudioSpectralFlatnessSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
			     *QueryAudioSpectralFlatnessDescriptionInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(QueryAudioSpectralFlatnessDescriptionInterface);
}

//----------------------------------------------------------------------------
double AudioSpectralFlatnessSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//=============================================================================
AudioSpectralFlatnessSearchTool::AudioSpectralFlatnessSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
AudioSpectralFlatnessSearchTool::
AudioSpectralFlatnessSearchTool(GenericDSInterfaceABC
		      *aQueryDescriptionInterface):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
  /* connect Descriptors with Searchs*/
  SetQueryDescriptionInterface(aQueryDescriptionInterface);


  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AudioSpectralFlatnessSearchTool::~AudioSpectralFlatnessSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& AudioSpectralFlatnessSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AudioSpectralFlatnessSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool AudioSpectralFlatnessSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool AudioSpectralFlatnessSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int AudioSpectralFlatnessSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
			   *aRefAudioSpectralFlatnessDescriptionInterface)

{
  m_RefDescriptionInterface =  aRefAudioSpectralFlatnessDescriptionInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* AudioSpectralFlatnessSearchTool::
GetQueryDescriptionInterface(void)

{
  return m_QueryDescriptionInterface;
}

//----------------------------------------------------------------------------
int  AudioSpectralFlatnessSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryAudioSpectralFlatnessDescriptionInterface)

{

  m_QueryDescriptionInterface =  aQueryAudioSpectralFlatnessDescriptionInterface;
  
  /* could also look totally different if
     aQueryAudioSpectralFlatnessDescriptionInterface is not of AudioSpectralFlatness node*/

  return 0;
}

//----------------------------------------------------------------------------
double AudioSpectralFlatnessSearchTool::GetDistance(void)
{
  double dist;
  unsigned int i;
  
  // Check it's all set up ok
  if(m_RefDescriptionInterface==NULL)   return(DBL_MAX);
  if(m_QueryDescriptionInterface==NULL) return(DBL_MAX);

  if(strcmp(m_RefDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);
  if(strcmp(m_QueryDescriptionInterface->GetName(),
	    "GenericDSInterface") != 0) return(DBL_MAX);

  
 
  //--- Extract Query data
  //--- AudioD
  QueryAudioSpectralFlatnessType = 
    m_QueryDescriptionInterface->GetFirstChild();

  if(strcmp(QueryAudioSpectralFlatnessType.GetDSName().c_str(),"AudioD") != 0)
  {
    std::cerr << "Error in XML-File:\n"
	 << "Instead of expected DSName \"AudioD\" found"
	 << QueryAudioSpectralFlatnessType.GetDSName().c_str() << "\n";;
    exit(1);
  }
 
  //--- SeriesOfVector
  QuerySeriesOfVector = 
    QueryAudioSpectralFlatnessType.GetDescription("SeriesOfVector");

  //--- Value
  QueryValue = QuerySeriesOfVector.GetDescription("Value");
  
  QueryValue.GetIntAttribute( "vectorSize", QueryVectorSize); 
  QueryValue.GetIntAttribute( "totalSampleNum", QueryTotalSampleNum); 

  VectorDim = QueryVectorSize;

  //--- Scaling
  QueryScaling = QueryValue.GetDescription("Scaling");
  QueryScaling.GetIntAttribute( "ratio", QueryRatio); 
  QueryScaling.GetIntAttribute( "elementNum", QueryElementNum); 

  //--- Mean
  QueryMeanDS = QueryValue.GetDescription("Mean");
  QueryMeanDS.GetRealVector(QueryVector);

  if( QueryVector.size() !=
      (unsigned int)(QueryElementNum * QueryVectorSize)) {
    std::cerr << "AudioSpectralFlatnessSearchTool::GetDistance(void):\n"
	 << "Query: Number of Mean-Values not correct\n";
    exit(1);
  }
 
  QuerySizeOfArrays = QueryVector.size();
  QueryMean = new float[QuerySizeOfArrays];

  for(i = 0; i < QuerySizeOfArrays; i++)
    QueryMean[i] = QueryVector[i];

  //--- Variance
  QueryVector.clear();
  QueryVarianceDS = QueryValue.GetDescription("Variance");
  QueryVarianceDS.GetRealVector(QueryVector);
 
  if( QueryVector.size() !=
      (unsigned int)(QueryElementNum * QueryVectorSize)) {
    std::cerr << "AudioSpectralFlatnessSearchTool::GetDistance(void):\n"
	 << "Query: Number of Variance-Values not correct\n";
    exit(1);
  }
  QuerySizeOfArrays = QueryVector.size();
  QueryVar          = new float[QuerySizeOfArrays];

  for(i = 0; i < QuerySizeOfArrays; i++)
    QueryVar[i] = QueryVector[i];



  //--- Extract Database data
  //--- AudioD
  RefAudioSpectralFlatnessType = 
    m_RefDescriptionInterface->GetFirstChild();

  if(strcmp(RefAudioSpectralFlatnessType.GetDSName().c_str(),"AudioD") != 0)
  {
    std::cerr << "Error in XML-File:\n"
	 << "Instead of expected DSName \"AudioD\" found"
	 << RefAudioSpectralFlatnessType.GetDSName().c_str() << "\n";;
    exit(1);
  }
 
  //--- SeriesOfVector
  RefSeriesOfVector = 
    RefAudioSpectralFlatnessType.GetDescription("SeriesOfVector");

  //--- Value
  RefValue = RefSeriesOfVector.GetDescription("Value");

  RefValue.GetIntAttribute( "vectorSize", RefVectorSize); 
  RefValue.GetIntAttribute( "totalSampleNum", RefTotalSampleNum); 

  if( RefVectorSize != VectorDim) {
    std::cerr << "AudioSpectralFlatnessSearchTool::GetDistance(void):\n"
	 << "Vector dimensions not identical\n";
    return -1.0f;
  }


  //--- Scaling
  RefScaling = RefValue.GetDescription("Scaling");
  RefScaling.GetIntAttribute( "ratio", RefRatio); 
  RefScaling.GetIntAttribute( "elementNum", RefElementNum); 

  //--- Mean
  RefMeanDS = RefValue.GetDescription("Mean");
  RefMeanDS.GetRealVector(RefVector);
 
  if( RefVector.size() != (unsigned int)(RefElementNum * RefVectorSize)) {
    std::cerr << "AudioSpectralFlatnessSearchTool::GetDistance(void):\n"
	 << "Number of Mean-Values not correct\n";
    return -1.0f;
  }

  RefSizeOfArrays = RefVector.size();
  RefMean = new float[RefSizeOfArrays];

  for(i = 0; i < RefSizeOfArrays; i++)
    RefMean[i] = RefVector[i];

  //--- Variance
  RefVector.clear();
  RefVarianceDS = RefValue.GetDescription("Variance");
  RefVarianceDS.GetRealVector(RefVector);
 
  if( RefVector.size() != (unsigned int)(RefElementNum * RefVectorSize)) {
    std::cerr << "AudioSpectralFlatnessSearchTool::GetDistance(void):\n"
	 << "Number of Variance-Values not correct\n";
    return -1.0f;
  }

  RefSizeOfArrays = RefVector.size();
  RefVar          = new float[RefSizeOfArrays];

  for(i = 0; i < RefSizeOfArrays; i++)
    RefVar[i] = RefVector[i];


  //--- Calculate distance
  dist = WeightedEuclidianDistance(RefMean,
				   RefVar,
				   RefElementNum,
				   QueryMean,
				   QueryVar,
				   QueryElementNum,
				   VectorDim);
 

  //--- tidy up
  delete [] QueryMean;
  delete [] QueryVar;
  delete [] RefMean;
  delete [] RefVar;
  QueryVector.clear();
  RefVector.clear();


  // All done
  return(dist);
}

//----------------------------------------------------------------------------
AudioSpectralFlatnessSearchInterfaceABC *AudioSpectralFlatnessSearchTool::GetInterface(void)
{ return &m_Interface; }


//--- Weighting vector for the mean vectors (unit variance)
static const float meanWeight[] = {
  11.933077, 11.103332, 10.464845, 10.231990, 
    9.743346, 9.645541, 9.644496, 9.672296,
    8.791485, 8.741418, 8.613710, 8.741487, 
    8.126337, 8.257461, 8.053868, 8.387219};



//---Weighting vector for the variannce vectors (unit variance)
const float varWeight[] = {
  69.486160, 69.157463, 73.134132, 73.182343,
    76.521751, 77.552032, 81.201492, 85.420212,
    74.791885, 76.823540, 79.335228, 82.347801,
    65.784485, 69.698914, 67.417160, 67.184540};



//----------------------------------------------------------------------------
float AudioSpectralFlatnessSearchTool::WeightedEuclidianDistance(
						const float* refMean,
						const float* refVar,
						int numRefVec,
						const float* queryMean,
						const float* queryVar,
						int numQueryVec,
						int vectorDim)
{  
  //--- Check wether the vector dimensions are compatible
  if((sizeof(meanWeight)/sizeof(meanWeight[0])) != vectorDim) {
    std::cerr << "Vector dimensions not identical" << std::endl;
    return -1.0f;
  }


  int i, j, k;
  float distance = 0.0f;
  
  //--- Loop over all query vectors (scalable serie)
  for(i = 0; i < numQueryVec; ++i) {
    float minDistance = FLT_MAX;
    //--- Loop over reference fingerprint vectors 
    for(j = 0; j < numRefVec; ++j) {
      float tmpDistance = 0.0f;

      //--- Calculate distance of mean vector 
      for(k = 0; k < vectorDim; ++k) {
	float v1 = queryMean[vectorDim*i + k] * meanWeight[k];
	float v2 = refMean[vectorDim*j + k] * meanWeight[k];
	tmpDistance += (v1-v2)*(v1-v2);
      }

      //--- Calculate distance of variance vector 
      for(k = 0; k < vectorDim; ++k) {
	float v1 = queryVar[vectorDim*i + k] * varWeight[k];
	float v2 = refVar[vectorDim*j + k] * varWeight[k];
	tmpDistance += (v1-v2)*(v1-v2);
      }

      if(tmpDistance < minDistance) {
	minDistance = tmpDistance;
      }
    }

    //--- Cumulate overall distance for all query vectors
    distance += minDistance;
  }

  return sqrt(distance/(vectorDim*numQueryVec));
}
