//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// Applicable File Name:  HarmonicInstrumentTimbreExtraction.cpp
//

#include <stdio.h>
#include <stdlib.h> // For malloc definition
#include <string.h> // For strlen definition
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

#include "engine.h" // Matlab Engine Header

#define LENGTH_BUFFER 40
//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID HarmonicInstrumentTimbreExtractionInterface::myID = UUID("");
const char *HarmonicInstrumentTimbreExtractionInterface::myName =
"HarmonicInstrumentTimbreDescriptorExtractorInterface";

const UUID HarmonicInstrumentTimbreExtractionTool::myID = UUID("");
const char *HarmonicInstrumentTimbreExtractionTool::myName = "HarmonicInstrumentTimbreDescriptorExtractor";

const UUID HarmonicInstrumentTimbreExtractionInterfaceABC::myID = UUID();

//=============================================================================
HarmonicInstrumentTimbreExtractionInterfaceABC::HarmonicInstrumentTimbreExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
HarmonicInstrumentTimbreExtractionInterface::
HarmonicInstrumentTimbreExtractionInterface(HarmonicInstrumentTimbreExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreExtractionInterface::~HarmonicInstrumentTimbreExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int HarmonicInstrumentTimbreExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC* HarmonicInstrumentTimbreExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreExtractionInterface::
SetDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aHarmonicInstrumentTimbreDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aHarmonicInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreExtractionInterface::InitExtracting(void)
{
  return(m_ExtractionTool->InitExtracting());
}


//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreExtractionInterface::InitExtracting(std::string mediaName)
{
  return(m_ExtractionTool->InitExtracting(mediaName));
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long HarmonicInstrumentTimbreExtractionInterface::PostExtracting(void)
{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
HarmonicInstrumentTimbreExtractionTool::HarmonicInstrumentTimbreExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
HarmonicInstrumentTimbreExtractionTool::
HarmonicInstrumentTimbreExtractionTool(HarmonicInstrumentTimbreDescriptorInterfaceABC
			*aHarmonicInstrumentTimbre):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{

  /* create descriptor if it does not exist*/
  if (!aHarmonicInstrumentTimbre) {
    HarmonicInstrumentTimbreDescriptor *descriptor =
      new HarmonicInstrumentTimbreDescriptor;
      aHarmonicInstrumentTimbre=descriptor->GetInterface();
  }


  /* connect descritors with Extraction*/
  SetDescriptorInterface(aHarmonicInstrumentTimbre);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreExtractionTool::~HarmonicInstrumentTimbreExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int HarmonicInstrumentTimbreExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC* HarmonicInstrumentTimbreExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreExtractionTool::
SetDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aHarmonicInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aHarmonicInstrumentTimbreDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aHarmonicInstrumentTimbreDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long HarmonicInstrumentTimbreExtractionTool::InitExtracting(void)

{
    //m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","InstrumentTimbre");
    m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","HarmonicInstrumentTimbre");
  m_ExtractionEngine = engOpen("\0"); // Start MATLAB Engine 
  return(0UL);
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long HarmonicInstrumentTimbreExtractionTool::InitExtracting(std::string mediaName)
{
    //m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","InstrumentTimbre");
    m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","HarmonicInstrumentTimbre");
    m_MediaName = mediaName;
    m_ExtractionEngine = engOpen("\0"); // Start MATLAB Engine

    return(0UL);
}


//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long HarmonicInstrumentTimbreExtractionTool::StartExtracting(void)
{
    // select media type you require and remove comment
    //MomVop *ImageMedia;
    //AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL)
      return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "HarmonicInstrumentTimbreDescriptionInterface") != 0)
      return (unsigned long)-1;

  if (m_Media==NULL) 
      return (unsigned long)-1;


  std::string cmdLine("[hsc, hsd, hss, hsv, lat] = ");
  cmdLine += m_ExtractionMatlabScript;
  cmdLine += "('" + m_MediaName + "');" ;

  engEvalString(m_ExtractionEngine, cmdLine.c_str());

  double value;

  mxArray* hsc = NULL;
  hsc = engGetArray(m_ExtractionEngine, "hsc");
  value = mxGetScalar(hsc);
  m_DescriptorInterface->SetHarmonicSpectralCentroid(value);

  mxArray* hsd = NULL;
  hsd = engGetArray(m_ExtractionEngine, "hsd");
  value = mxGetScalar(hsd);
  m_DescriptorInterface->SetHarmonicSpectralDeviation(value);

  mxArray* hss = NULL;
  hss = engGetArray(m_ExtractionEngine, "hss");
  value = mxGetScalar(hss);
  m_DescriptorInterface->SetHarmonicSpectralSpread(value);

  mxArray* hsv = NULL;
  hsv = engGetArray(m_ExtractionEngine, "hsv");
  value = mxGetScalar(hsv);
  m_DescriptorInterface->SetHarmonicSpectralVariation(value);

  mxArray* lat = NULL;
  lat = engGetArray(m_ExtractionEngine, "lat");
  value = mxGetScalar(lat);
  m_DescriptorInterface->SetLogAttackTime(value);

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long HarmonicInstrumentTimbreExtractionTool::PostExtracting(void)

{

  // In this specific case you could als write the number of frames here
  //m_DescriptorInterface->SetDummyContents(m_FrameCnt*
  //					  m_DummyExtractionParameter);

    engClose(m_ExtractionEngine);
  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreExtractionInterfaceABC *HarmonicInstrumentTimbreExtractionTool::GetInterface(void)
{ return &m_Interface; }

	
