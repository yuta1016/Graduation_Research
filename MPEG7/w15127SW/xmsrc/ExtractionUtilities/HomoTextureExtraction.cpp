///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// Applicable File Name:  HomoTextureExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

#include "homo_texture_extractor.h"	//yjyu - 010217

//static int initextractionwascalled=0;


//=============================================================================
	

using namespace XM;


const UUID HomogeneousTextureExtractionInterface::myID = UUID("a9251f90-8aa6-11d3-9db4-00609791d5bb");
const char *HomogeneousTextureExtractionInterface::myName =
"HomogeneousTextureDescriptorExtractorInterface";

const UUID HomogeneousTextureExtractionTool::myID = UUID("098cec90-6741-11d3-9d88-00609791d5bb");
const char *HomogeneousTextureExtractionTool::myName = "HomogeneousTextureDescriptorExtractor";

const UUID HomogeneousTextureExtractionInterfaceABC::myID = UUID();

//=============================================================================
HomogeneousTextureExtractionInterfaceABC::HomogeneousTextureExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
HomogeneousTextureExtractionInterface::
HomogeneousTextureExtractionInterface(HomogeneousTextureExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HomogeneousTextureExtractionInterface::~HomogeneousTextureExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void HomogeneousTextureExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int HomogeneousTextureExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC* HomogeneousTextureExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int HomogeneousTextureExtractionInterface::
SetDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aHomogeneousTextureDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aHomogeneousTextureDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long HomogeneousTextureExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
HomogeneousTextureExtractionTool::HomogeneousTextureExtractionTool():
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{  
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}

//=============================================================================
HomogeneousTextureExtractionTool::
HomogeneousTextureExtractionTool(HomogeneousTextureDescriptorInterfaceABC
			*aHomogeneousTexture):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  /* create descriptor if it does not exist*/
  if (!aHomogeneousTexture) {
    HomogeneousTextureDescriptor *descriptor =
      new HomogeneousTextureDescriptor;
    aHomogeneousTexture=descriptor->GetInterface();
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aHomogeneousTexture);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HomogeneousTextureExtractionTool::~HomogeneousTextureExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool HomogeneousTextureExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool HomogeneousTextureExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int HomogeneousTextureExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;
  return 0;
}


//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC* HomogeneousTextureExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int HomogeneousTextureExtractionTool::
SetDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aHomogeneousTextureDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aHomogeneousTextureDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aHomogeneousTextureDescriptorInterface;
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
unsigned long HomogeneousTextureExtractionTool::InitExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long HomogeneousTextureExtractionTool::StartExtracting(void)
{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  //---yjyu - 010218
  int HomogeneousTexture_feature[62];
  int flag,i,j;
  //---

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "HomogeneousTextureDescriptionInterface") != 0) return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;


	//---yjyu - 010217
	/////////// 2000.10.11 - yjyu@samsung.com  /* For the same size or larger than 128x128 */
	if(ImageMedia->height < 128 || ImageMedia->width < 128)
	{
		fprintf(stderr,"Image size shold be bigger than 128 x 128.\n");
		return (unsigned long)-1;
	}

	//HomogeneousTexture feature extraction
	CHomogeneousTexture_Extractor HomogeneousTexture_extractor;

	// 2000.10.11 - yjyu@samsung.com
	HomogeneousTexture_extractor.image_height = ImageMedia->height;
	HomogeneousTexture_extractor.image_width = ImageMedia->width;

	if ((ImageMedia->a_chan) &&
	    (m_Media->GetMaxAlpha() > m_Media->GetMinAlpha()))
	{
		HomogeneousTexture_extractor.ArbitraryShape(ImageMedia->a_chan->data->u, ImageMedia->y_chan->data->u, ImageMedia->height, ImageMedia->width );
	}

	HomogeneousTexture_extractor.FeatureExtraction(ImageMedia->y_chan->data->u);

	//receive the parameter for layer -ychoi 010527
	flag = atoi(getpara("layer", "1"));

#ifdef DEBUG1
	if (flag == 0)
		fprintf(stderr,"Homogeneous Texture Feature with base layer, Extracted .. \n" );
	else
		fprintf(stderr,"Homogeneous Texture Feature with full layer, Extracted .. \n" );
#endif

	//saving 62 features & test show
	HomogeneousTexture_feature[0] = HomogeneousTexture_extractor.m_dc;
	HomogeneousTexture_feature[1] = HomogeneousTexture_extractor.m_std;
#ifdef DEBUG1
	fprintf(stderr,"%3d\t%3d\n",
		HomogeneousTexture_feature[0], HomogeneousTexture_feature[1]);
#endif

	for(i=0; i< 5; i++)
		for(j=0; j< 6; j++) {
			HomogeneousTexture_feature[i*6+j+2] = HomogeneousTexture_extractor.mean2[i][j];
#ifdef DEBUG1
		    fprintf(stderr,"%3d\t",
			    HomogeneousTexture_feature[i*6+j+2]);
#endif
		}
	for(i=0; i< 5; i++)
		for(j=0; j< 6; j++) {
			if (flag != 0) { 
				HomogeneousTexture_feature[i*6+j+30+2] = HomogeneousTexture_extractor.dev2[i][j];
#ifdef DEBUG1
				fprintf(stderr,"%3d\t",
				       HomogeneousTexture_feature[i*6+j+30+2]);
#endif
			}
			else
				HomogeneousTexture_feature[i*6+j+30+2] = 0;
		}

#ifdef DEBUG1
	fprintf(stderr, "\n\n");
#endif
	m_DescriptorInterface->SetHomogeneousTextureFeature(flag, HomogeneousTexture_feature);	//yjyu - 010223

//---

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long HomogeneousTextureExtractionTool::PostExtracting(void)

{
  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
HomogeneousTextureExtractionInterfaceABC *HomogeneousTextureExtractionTool::GetInterface(void)
{ return &m_Interface; }
