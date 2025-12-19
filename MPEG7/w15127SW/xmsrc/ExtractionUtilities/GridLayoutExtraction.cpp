////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by 
// Karol Wnukowicz (Visual Atoms) 
// Stavros Paschalakis (Visual Atoms) 
// in the course of development of the MPEG-7 Standard (ISO/IEC 15938). 
// This software module is an implementation of a part 
// of one or more MPEG-7 Standard tools as specified by the MPEG-7 Standard. 
// ISO/IEC gives users of the MPEG-7 Standard free license to this software
// module or modifications thereof for use in hardware or software products
// claiming conformance to the MPEG-7 Standard. 
// Those intending to use this software module in hardware or software products 
// are advised that its use may infringe existing patents. The original 
// developer of this software module and his/her company, the subsequent 
// editors and their companies, and ISO/IEC have no liability for use of this 
// software module or modifications thereof in an implementation.
// Copyright is not released for non MPEG-7 Standard conforming products. 
// <CN1> retains full right to use the code for his/her own purpose, 
// assign or donate the code to a third party and to inhibit third parties 
// from using the code for non MPEG-7 Standard conforming products. 
// This copyright notice must be included in all copies or derivative works. 
// Copyright (c)2003-2015
//
////////////////////////////////////////////////////////////////////////////////
//
// Applicable File Name:  GridLayoutExtraction.cpp
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


const UUID GridLayoutExtractionInterface::myID = UUID("");
const char *GridLayoutExtractionInterface::myName =
"GridLayoutDescriptorExtractorInterface";

const UUID GridLayoutExtractionTool::myID = UUID("");
const char *GridLayoutExtractionTool::myName = "GridLayoutDescriptorExtractor";

const UUID GridLayoutExtractionInterfaceABC::myID = UUID();

//=============================================================================
GridLayoutExtractionInterfaceABC::GridLayoutExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
GridLayoutExtractionInterface::
GridLayoutExtractionInterface(GridLayoutExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GridLayoutExtractionInterface::~GridLayoutExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& GridLayoutExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void GridLayoutExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int GridLayoutExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
GridLayoutDescriptorInterfaceABC* GridLayoutExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int GridLayoutExtractionInterface::
SetDescriptorInterface(GridLayoutDescriptorInterfaceABC
		       *aGridLayoutDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aGridLayoutDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long GridLayoutExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long GridLayoutExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long GridLayoutExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
GridLayoutExtractionTool::GridLayoutExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_numOfPartX(0),
m_numOfPartY(0)

{  
  SetExposedInterface(&m_Interface);
}

//=============================================================================
GridLayoutExtractionTool::
GridLayoutExtractionTool(GridLayoutDescriptorInterfaceABC
			*aGridLayout):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_numOfPartX(0),
m_numOfPartY(0)

{
  /* create descriptor if it does not exist*/
  if (!aGridLayout) {
    GridLayoutDescriptor *descriptor =
      new GridLayoutDescriptor();
      aGridLayout=descriptor->GetInterface();
  }


  /* connect descritors with Extraction*/
  SetDescriptorInterface(aGridLayout);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GridLayoutExtractionTool::~GridLayoutExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool GridLayoutExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool GridLayoutExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int GridLayoutExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
GridLayoutDescriptorInterfaceABC* GridLayoutExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int GridLayoutExtractionTool::
SetDescriptorInterface(GridLayoutDescriptorInterfaceABC
		       *aGridLayoutDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aGridLayoutDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aGridLayoutDescriptorInterface;
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
unsigned long GridLayoutExtractionTool::InitExtracting(void)

{
  /* example for initializing extraction paramters*/
	m_numOfPartX=atoi(getpara("numOfPartX","2"));
	m_numOfPartY=atoi(getpara("numOfPartY","2"));

  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long GridLayoutExtractionTool::StartExtracting(void)

{
	// set up extraction tool and the interface
	GL_SUB_EXTRACTION *theExtractionTool					= NULL;
	GL_SUB_EXTRACTION_INTERFACE *theExtractionInterface		= NULL;

  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "GridLayoutDescriptionInterface") != 0) return (unsigned long)-1;

  theExtractionTool = new GL_SUB_EXTRACTION(0);
  theExtractionInterface = theExtractionTool->GetInterface();

  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;
  int cnt = m_numOfPartX * m_numOfPartY;
  int img_width = ImageMedia->width;
  int img_height = ImageMedia->height;
  int sub_width = img_width/m_numOfPartX;
  int sub_height = img_height/m_numOfPartY;
  int i, j;

  MultiMedia *subMedia = new MultiMedia();
  MultiMediaInterfaceABC *subMediaInterface=subMedia->GetInterface();

  theExtractionInterface->InitExtracting();
  theExtractionInterface->SetSourceMedia(subMediaInterface);

  m_DescriptorInterface->SetPartNumber(m_numOfPartX, m_numOfPartY);

  for(j = 0; j < m_numOfPartY; j++)
  {
	  for(i = 0; i < m_numOfPartX; i++)
	  {
		GL_SUB_DESCRIPTOR *subDescr = new GL_SUB_DESCRIPTOR();
		MomVop *subImage = m_Media->GetSubImage(i*sub_width, j*sub_height, sub_width, sub_height);
		subMediaInterface->FreeImage();
		subMediaInterface->SetImage(subImage);
		theExtractionTool->SetDescriptorInterface(subDescr->GetInterface());
		theExtractionTool->StartExtracting();
		m_DescriptorInterface->DeleteContents(i, j);
		m_DescriptorInterface->SetContents(i, j, subDescr->GetInterface());
	  }
  }

  theExtractionInterface->PostExtracting();

  theExtractionInterface->destroy();
  subMediaInterface->destroy();
  
  // write extracted data to Descriptor
  //m_DescriptorInterface->SetDummyContents(val);//TOBEMODIFIED in your Application


  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long GridLayoutExtractionTool::PostExtracting(void)

{

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& GridLayoutExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GridLayoutExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
GridLayoutExtractionInterfaceABC *GridLayoutExtractionTool::GetInterface(void)
{ return &m_Interface; }

