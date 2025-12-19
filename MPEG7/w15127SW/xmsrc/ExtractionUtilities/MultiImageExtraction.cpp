//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stpehan Herrmann
// Munich University of Technology
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
// Applicable File Name:  MultiImageExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID MultiImageExtractionInterface::myID = UUID("");
const char *MultiImageExtractionInterface::myName =
"MultiImageDescriptorExtractorInterface";

const UUID MultiImageExtractionTool::myID = UUID("");
const char *MultiImageExtractionTool::myName = "MultiImageDescriptorExtractor";

const UUID MultiImageExtractionInterfaceABC::myID = UUID();

//=============================================================================
MultiImageExtractionInterfaceABC::MultiImageExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
MultiImageExtractionInterface::
MultiImageExtractionInterface(MultiImageExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiImageExtractionInterface::~MultiImageExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& MultiImageExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void MultiImageExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int MultiImageExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC* MultiImageExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int MultiImageExtractionInterface::
SetDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aMultiImageDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aMultiImageDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long MultiImageExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long MultiImageExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long MultiImageExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
MultiImageExtractionTool::MultiImageExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
  /* create sub Extractions and
     connect them with descriptor if availabe*/
  ScalableColorExtractionTool
    *aScalableColorExtraction= new ScalableColorExtractionTool();
  ContourShapeExtractionTool
    *aContourShapeExtraction= new ContourShapeExtractionTool();

  /* connect sub Extractions with MultiImage Extraction*/
  if (m_ScalableColorExtraction)
    m_ScalableColorExtraction=aScalableColorExtraction->
					GetInterface();
  if (m_ContourShapeExtraction)
    m_ContourShapeExtraction=aContourShapeExtraction->
					GetInterface();
  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
MultiImageExtractionTool::
MultiImageExtractionTool(MultiImageDescriptorInterfaceABC
			*MultiImage):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL)

{
  /* create descriptor if it does not exist*/
  if (!MultiImage) {
    MultiImageDescriptor *descriptor =
      new MultiImageDescriptor(0,0); /* one argument for each sub-component*/
    MultiImage=descriptor->GetInterface();
  }

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub Extraction and
     connect them with descriptor if availabe*/
  {
    ScalableColorExtractionTool *aScalableColorExtraction;

    if (MultiImage->GetScalableColorDescriptorInterface()) {
      aScalableColorExtraction= new
	ScalableColorExtractionTool(MultiImage->
				 GetScalableColorDescriptorInterface());
    }
    else { 
      aScalableColorExtraction= new ScalableColorExtractionTool();
    }
    if (aScalableColorExtraction) {
      m_ScalableColorExtraction=aScalableColorExtraction->GetInterface();
    }
  }

  {
    ContourShapeExtractionTool *aContourShapeExtraction;

    if (MultiImage->GetContourShapeDescriptorInterface()) {
      aContourShapeExtraction= new
	ContourShapeExtractionTool(MultiImage->
			       GetContourShapeDescriptorInterface());
    }
    else { 
      aContourShapeExtraction= new ContourShapeExtractionTool();
    }
    if (aContourShapeExtraction) {
      m_ContourShapeExtraction=aContourShapeExtraction->GetInterface();
    }
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(MultiImage);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiImageExtractionTool::~MultiImageExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  if (m_ScalableColorExtraction) m_ScalableColorExtraction->destroy();
  if (m_ContourShapeExtraction) m_ContourShapeExtraction->destroy();
}

//----------------------------------------------------------------------------
bool MultiImageExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool MultiImageExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int MultiImageExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  if (m_ScalableColorExtraction)
    m_ScalableColorExtraction->SetSourceMedia(media);
  if (m_ContourShapeExtraction)
    m_ContourShapeExtraction->SetSourceMedia(media);
  return 0;
}


//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC* MultiImageExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int MultiImageExtractionTool::
SetDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aMultiImageDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aMultiImageDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aMultiImageDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

    /* conect sub descriptors with sub Extraction*/
    if ((m_DescriptorInterface->
	 GetScalableColorDescriptorInterface()) &&
	m_ScalableColorExtraction) {
      m_ScalableColorExtraction->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetScalableColorDescriptorInterface());
    }
    if ((m_DescriptorInterface->
	 GetContourShapeDescriptorInterface()) &&
	m_ContourShapeExtraction) {
      m_ContourShapeExtraction->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetContourShapeDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub Extraction*/
    m_ScalableColorExtraction->SetDescriptorInterface(0);
    m_ContourShapeExtraction->SetDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long MultiImageExtractionTool::InitExtracting(void)

{
  /* init extraction of ScalableColor description*/
  m_ScalableColorExtraction->InitExtracting();

  /* init extraction of ContourShape description*/
  m_ContourShapeExtraction->InitExtracting();

  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long MultiImageExtractionTool::StartExtracting(void)

{
  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "MultiImageDescriptionInterface") != 0) return (unsigned long)-1;

  /* extraction of ScalableColor description*/
  m_ScalableColorExtraction->StartExtracting();

  /* extraction of ContourShape description*/
  m_ContourShapeExtraction->StartExtracting();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long MultiImageExtractionTool::PostExtracting(void)

{
  /* post extraction of ScalableColor description*/
  //  m_ScalableColorExtraction->PostExtracting();

  /* post extraction of ContourShape description*/
  //m_ContourShapeExtraction->PostExtracting();

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& MultiImageExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
MultiImageExtractionInterfaceABC *MultiImageExtractionTool::GetInterface(void)
{ return &m_Interface; }

