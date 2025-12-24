///////////////////////////////////////////////////////////////////////////////
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
// Applicable File Name:  AudioSpectralFlatnessSearch.h
//

#ifndef __AUDIOSPECTRALFLATNESSSEARCH_H__
#define __AUDIOSPECTRALFLATNESSSEARCH_H__

/* remove this, only for test purpose*/
#define __SearchAndRetrievalTypeApplication
//#define __ImageProcessingTypeApplication

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class AudioSpectralFlatnessSearchTool;

//=============================================================================
class AudioSpectralFlatnessSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  AudioSpectralFlatnessSearchInterfaceABC();
  virtual ~AudioSpectralFlatnessSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface) = 0;

  virtual double GetDistance(void) = 0;

  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class AudioSpectralFlatnessSearchInterface: 
public AudioSpectralFlatnessSearchInterfaceABC
{
public:
  AudioSpectralFlatnessSearchInterface(AudioSpectralFlatnessSearchTool* aTool);
  virtual ~AudioSpectralFlatnessSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  AudioSpectralFlatnessSearchTool *m_SearchTool;
};

//=============================================================================
class AudioSpectralFlatnessSearchTool: public Search
{
friend class AudioSpectralFlatnessSearchInterface;
public:
  AudioSpectralFlatnessSearchTool();
  AudioSpectralFlatnessSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual AudioSpectralFlatnessSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aAudioSpectralFlatnessDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~AudioSpectralFlatnessSearchTool();

  AudioSpectralFlatnessSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;


  //--- Descriptions

  //--- AudioSpectralFlatnessExamples
  GenericDS QueryAudioSpectralFlatnessType;
  GenericDS QuerySeriesOfVector;
  GenericDS QueryValue;
  GenericDS QueryScaling;
  GenericDS QueryMeanDS;
  GenericDS QueryVarianceDS;

  GenericDS RefAudioSpectralFlatnessType;
  GenericDS RefSeriesOfVector;
  GenericDS RefValue;
  GenericDS RefScaling;
  GenericDS RefMeanDS;
  GenericDS RefVarianceDS;



  //--- Query-Data
  int            QueryVectorSize;
  int            QueryTotalSampleNum;
  int            QueryRatio;
  int            QueryElementNum;

  float *        QueryMean;
  float *        QueryVar;
  unsigned int   QuerySizeOfArrays;
  vector<double> QueryVector;

  //--- Ref-Data
  int            RefVectorSize;
  int            RefTotalSampleNum;
  int            RefRatio;
  int            RefElementNum;

  float *        RefMean;
  float *        RefVar;
  unsigned int   RefSizeOfArrays;
  vector<double> RefVector;

  //--- Common Data
  int VectorDim;



  //--- Functions
  float WeightedEuclidianDistance(const float* refMean,
				  const float* refVar,
				  int numRefVec,
				  const float* queryMean,
				  const float* queryVar,
				  int numQueryVec,
				  int vectorDim);
};

};

#endif //__AUDIOSPECTRALFLATNESSSEARCH_H__
