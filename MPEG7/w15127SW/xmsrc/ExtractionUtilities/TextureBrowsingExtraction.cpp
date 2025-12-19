//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
// (contributing organizations names)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TextureBrowsingExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID TextureBrowsingExtractionInterface::myID = UUID("");
const char *TextureBrowsingExtractionInterface::myName =
"TextureBrowsingDescriptorExtractorInterface";

const UUID TextureBrowsingExtractionTool::myID = UUID("");
const char *TextureBrowsingExtractionTool::myName = "TextureBrowsingDescriptorExtractor";

const UUID TextureBrowsingExtractionInterfaceABC::myID = UUID();

//=============================================================================
TextureBrowsingExtractionInterfaceABC::TextureBrowsingExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
TextureBrowsingExtractionInterface::
TextureBrowsingExtractionInterface(TextureBrowsingExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TextureBrowsingExtractionInterface::~TextureBrowsingExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void TextureBrowsingExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int TextureBrowsingExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC* TextureBrowsingExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TextureBrowsingExtractionInterface::
SetDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aTextureBrowsingDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aTextureBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long TextureBrowsingExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long TextureBrowsingExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long TextureBrowsingExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
TextureBrowsingExtractionTool::TextureBrowsingExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}

//=============================================================================
TextureBrowsingExtractionTool::
TextureBrowsingExtractionTool(TextureBrowsingDescriptorInterfaceABC
			*aTextureBrowsing):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  /* create descriptor if it does not exist*/
  if (!aTextureBrowsing) {
    TextureBrowsingDescriptor *descriptor =
      new TextureBrowsingDescriptor;
    aTextureBrowsing=descriptor->GetInterface();
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aTextureBrowsing);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TextureBrowsingExtractionTool::~TextureBrowsingExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool TextureBrowsingExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool TextureBrowsingExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int TextureBrowsingExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
TextureBrowsingDescriptorInterfaceABC* TextureBrowsingExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int TextureBrowsingExtractionTool::
SetDescriptorInterface(TextureBrowsingDescriptorInterfaceABC
		       *aTextureBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aTextureBrowsingDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aTextureBrowsingDescriptorInterface;
  if (m_DescriptorInterface)
    m_DescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long TextureBrowsingExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long TextureBrowsingExtractionTool::StartExtracting(void)

{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "TextureBrowsingDescriptionInterface") != 0) return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;

//---yjyu - 010218 ychoi - 010601
	Matrix* Image;
	int PBC[5];

	if(ImageMedia->a_chan)
	{
		ArbitraryShape(ImageMedia->a_chan->data->u, ImageMedia->y_chan->data->u, ImageMedia->height, ImageMedia->width);
	}

	CreateMatrix(&Image,ImageMedia->height,ImageMedia->width);
	Convert2Matrix (ImageMedia->y_chan->data->u,ImageMedia->width,ImageMedia->height, Image);
	PBC_Extraction(Image,ImageMedia->width, ImageMedia->height,PBC);

		//receive the parameter for layer -ychoi 010527
	int flag = atoi(getpara("layer", "1"));

    m_DescriptorInterface->SetComponentNumberFlag(flag);

	//----------	// 2001.06.02 - ychoi
	if (flag == 0) {
		printf ("with base layer; 3 components \n");
		PBC[3] = PBC[4] = 0;
		m_DescriptorInterface->SetBrowsing_Component(PBC);
		for(int i = 0; i < 3; i++)
			fprintf(stderr,"%d\t",PBC[i]);
	}
	else {
		printf ("with full layer; 5 components \n\n");
		m_DescriptorInterface->SetBrowsing_Component(PBC);
		for(int i = 0; i < 5; i++)
			fprintf(stderr,"%d\t",PBC[i]);
	}
	printf("\n");
	//----------
	
	FreeMatrix(Image);//deallocate memory
//---

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long TextureBrowsingExtractionTool::PostExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& TextureBrowsingExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TextureBrowsingExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TextureBrowsingExtractionInterfaceABC *TextureBrowsingExtractionTool::GetInterface(void)
{ return &m_Interface; }

