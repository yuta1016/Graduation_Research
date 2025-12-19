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
// Applicable File Name:  PercussiveInstrumentTimbreExtraction.cpp
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

#include <engine.h> // Matlab Engine Header

#define LENGTH_BUFFER 40 // USE FOR char * memory allocation in GetCurrentValue
//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID PercussiveInstrumentTimbreExtractionInterface::myID = UUID("");
const char *PercussiveInstrumentTimbreExtractionInterface::myName =
"PercussiveInstrumentTimbreDescriptorExtractorInterface";

const UUID PercussiveInstrumentTimbreExtractionTool::myID = UUID("");
const char *PercussiveInstrumentTimbreExtractionTool::myName = "PercussiveInstrumentTimbreDescriptorExtractor";

const UUID PercussiveInstrumentTimbreExtractionInterfaceABC::myID = UUID();

//=============================================================================
PercussiveInstrumentTimbreExtractionInterfaceABC::PercussiveInstrumentTimbreExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
PercussiveInstrumentTimbreExtractionInterface::
PercussiveInstrumentTimbreExtractionInterface(PercussiveInstrumentTimbreExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreExtractionInterface::~PercussiveInstrumentTimbreExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int PercussiveInstrumentTimbreExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC* PercussiveInstrumentTimbreExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreExtractionInterface::
SetDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aPercussiveInstrumentTimbreDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aPercussiveInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreExtractionInterface::InitExtracting(void)
{
  return(m_ExtractionTool->InitExtracting());
}


//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreExtractionInterface::InitExtracting(std::string mediaName)
{
  return(m_ExtractionTool->InitExtracting(mediaName));
}

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long PercussiveInstrumentTimbreExtractionInterface::PostExtracting(void)
{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
PercussiveInstrumentTimbreExtractionTool::PercussiveInstrumentTimbreExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
PercussiveInstrumentTimbreExtractionTool::
PercussiveInstrumentTimbreExtractionTool(PercussiveInstrumentTimbreDescriptorInterfaceABC
			*aPercussiveInstrumentTimbre):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)
{

  /* create descriptor if it does not exist*/
  if (!aPercussiveInstrumentTimbre) {
    PercussiveInstrumentTimbreDescriptor *descriptor =
      new PercussiveInstrumentTimbreDescriptor;
      aPercussiveInstrumentTimbre=descriptor->GetInterface();
  }


  /* connect descritors with Extraction*/
  SetDescriptorInterface(aPercussiveInstrumentTimbre);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreExtractionTool::~PercussiveInstrumentTimbreExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int PercussiveInstrumentTimbreExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC* PercussiveInstrumentTimbreExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreExtractionTool::
SetDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aPercussiveInstrumentTimbreDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aPercussiveInstrumentTimbreDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aPercussiveInstrumentTimbreDescriptorInterface;
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
unsigned long PercussiveInstrumentTimbreExtractionTool::InitExtracting(void)

{
  m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","PercussiveInstrumentTimbre");
  m_ExtractionEngine = engOpen("\0"); // Start Matlab Engine
  return(0UL);
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long PercussiveInstrumentTimbreExtractionTool::InitExtracting(std::string mediaName)
{
    m_ExtractionMatlabScript = getpara("ExtractionMatlabScript","PercussiveInstrumentTimbre");
    m_MediaName = mediaName;
    m_ExtractionEngine = engOpen("\0");
  return(0UL);
}


//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long PercussiveInstrumentTimbreExtractionTool::StartExtracting(void)
{
    // select media type you require and remove comment

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL)
      return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "PercussiveInstrumentTimbreDescriptionInterface") != 0)
      return (unsigned long)-1;

  if (m_Media==NULL) 
      return (unsigned long)-1;

  // Set for 
  std::string cmdLine("[lat, sc, tc] = ");
  cmdLine += m_ExtractionMatlabScript;
  cmdLine += "('" + m_MediaName + "');" ;

  engEvalString(m_ExtractionEngine, cmdLine.c_str());

  double value;

  mxArray* lat = NULL;
  lat = engGetArray(m_ExtractionEngine, "lat");
  value = mxGetScalar(lat);
  m_DescriptorInterface->SetLogAttackTime(value);

  mxArray* sc = NULL;
  sc = engGetArray(m_ExtractionEngine, "sc");
  value = mxGetScalar(sc);
  m_DescriptorInterface->SetSpectralCentroid(value);

  mxArray* tc = NULL;
  tc = engGetArray(m_ExtractionEngine, "tc");
  value = mxGetScalar(tc);
  m_DescriptorInterface->SetTemporalCentroid(value);


  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long PercussiveInstrumentTimbreExtractionTool::PostExtracting(void)

{

  // In this specific case you could als write the number of frames here
  //m_DescriptorInterface->SetDummyContents(m_FrameCnt*
  //					  m_DummyExtractionParameter);
    engClose(m_ExtractionEngine);
  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreExtractionInterfaceABC *PercussiveInstrumentTimbreExtractionTool::GetInterface(void)
{ return &m_Interface; }

	
