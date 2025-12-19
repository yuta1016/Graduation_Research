//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CTBrowsingExtraction.cpp
//

#include <stdio.h>
#include "AddressLib/momusys.h"
#include "AddressLib/inter.h"
#include "AddressLib/intra.h"
#include "AddressLib/fifo.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"

//static int initextractionwascalled=0;

//=============================================================================
	

using namespace XM;

unsigned char* interleaveRGB (unsigned char* pixel_R,unsigned char* pixel_G,
			   unsigned char* pixel_B,int size);

const UUID CTBrowsingExtractionInterface::myID = UUID("");
const char *CTBrowsingExtractionInterface::myName =
"CTBrowsingDescriptorExtractorInterface";

const UUID CTBrowsingExtractionTool::myID = UUID("");
const char *CTBrowsingExtractionTool::myName = "CTBrowsingDescriptorExtractor";

const UUID CTBrowsingExtractionInterfaceABC::myID = UUID();

//=============================================================================
CTBrowsingExtractionInterfaceABC::CTBrowsingExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
CTBrowsingExtractionInterface::
CTBrowsingExtractionInterface(CTBrowsingExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CTBrowsingExtractionInterface::~CTBrowsingExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void CTBrowsingExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int CTBrowsingExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC* CTBrowsingExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int CTBrowsingExtractionInterface::
SetDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aCTBrowsingDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aCTBrowsingDescriptorInterface);
}

//----------------------------------------------------------------------------
//unsigned long CTBrowsingExtractionInterface::InitExtracting(void)
//
//{
//  return(m_ExtractionTool->InitExtracting());
//}

//----------------------------------------------------------------------------
unsigned long CTBrowsingExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long CTBrowsingExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
CTBrowsingExtractionTool::CTBrowsingExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(NULL),
m_FrameCnt(0)

{
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}

//=============================================================================
CTBrowsingExtractionTool::
CTBrowsingExtractionTool(CTBrowsingDescriptorInterfaceABC
			*aCTBrowsing):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  /* create descriptor if it does not exist*/
  if (!aCTBrowsing) {
    CTBrowsingDescriptor *descriptor =
      new CTBrowsingDescriptor;
    aCTBrowsing=descriptor->GetInterface();
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aCTBrowsing);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CTBrowsingExtractionTool::~CTBrowsingExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool CTBrowsingExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool CTBrowsingExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int CTBrowsingExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;

  return 0;
}


//----------------------------------------------------------------------------
CTBrowsingDescriptorInterfaceABC* CTBrowsingExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int CTBrowsingExtractionTool::
SetDescriptorInterface(CTBrowsingDescriptorInterfaceABC
		       *aCTBrowsingDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aCTBrowsingDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aCTBrowsingDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}


//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long CTBrowsingExtractionTool::StartExtracting(void)

{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) 
	  return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "CTBrowsingDescriptionInterface") != 0) 
		return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;

  //---------------------------------------------
  unsigned char *RGB;
  int PCTBC[2],imsize;
  unsigned char** nlabel;
  double** xyzbuff; 
  double pix, piy;
  int* ctemperature;
  ctemperature=new(int);
//  MomVop *rgbvop;


  imsize = ImageMedia->width*ImageMedia->height;

/*  rgbvop=initvop(ImageMedia->width,ImageMedia->height,_444_,
 		 UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
  convertYUV_To_RGB(rgbvop,ImageMedia);

  RGB = interleaveRGB(rgbvop->v_chan->data->u, rgbvop->y_chan->data->u,
		   rgbvop->u_chan->data->u, imsize); */

  RGB = interleaveRGB(ImageMedia->v_chan->data->u, ImageMedia->y_chan->data->u,
		   ImageMedia->u_chan->data->u, imsize);

  //-- label buffer
  nlabel = cBuffAlloc2(ImageMedia->height, ImageMedia->width);
  //-- XYZ buffer
  xyzbuff = dBuffAlloc2(ImageMedia->height, ImageMedia->width*3);

  rgb2xyz_marklabel(RGB, xyzbuff, nlabel, ImageMedia->width, 
	  ImageMedia->height);

  perc_ill(xyzbuff, nlabel, ImageMedia->width, ImageMedia->height, &pix, &piy);
	
  //-- chromaticity -> color temperature conversion
  convert_xy2temp(pix, piy, ctemperature);

  PCTBC_Extraction(ctemperature, PCTBC);

  // print-out result
  std::cerr << "Category = " << PCTBC[0] << "  SubRangeIndex =" << PCTBC[1] <<std::endl;
 
  m_DescriptorInterface->SetCTBrowsing_Component(PCTBC);

  //-- free label buffer 
  for(int i=0; i<ImageMedia->height; i++) {
 	free(xyzbuff[i]);
	free(nlabel[i]);
  }
  free(xyzbuff);
  free(nlabel); 
  delete ctemperature;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long CTBrowsingExtractionTool::PostExtracting(void)

{
  // In this specific case you could als write the number of frames here
  //m_DescriptorInterface->SetDummyContents(m_FrameCnt*
  //					  m_DummyExtractionParameter);

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& CTBrowsingExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CTBrowsingExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
CTBrowsingExtractionInterfaceABC *CTBrowsingExtractionTool::GetInterface(void)
{ return &m_Interface; }


//---------------------------------------------------------------------------
unsigned char* interleaveRGB (unsigned char* chan_v, 
			   unsigned char* chan_y,
			   unsigned char* chan_u, int size)
{
//  int R, G, B;
  int i;
  unsigned char *pixelarray=new unsigned char [3*size];
//  double R,G,B;
//  double x, y, z;
//  double scale;

/*  for(i=0;i<size;++i) {    
	x = chan_y[i] - 16;
	y = chan_u[i] - 128;
	z = chan_v[i] - 128;

	scale = 1.0/256.;
	//-- for Rec. 601 RGB(NTSC primaries with illuminant C white point)
	pixelarray[3*i] = unsigned char (scale * (298.082*x + 0.0*y + 408.583*z)) ;
	pixelarray[3*i+1] = unsigned char (scale * (298.082*x - 100.291*y - 208.120*z)) ;
	pixelarray[3*i+2] = unsigned char (scale * (298.082*x + 516.411*y + 0.0*z)) ;
  } */

/* for(i=0;i<size;++i) {    
    R = chan_y[i] + 1.402*(chan_v[i]-128);
	G = chan_y[i] - 0.34414*(chan_u[i]-128) - 0.71414*(chan_v[i]-128);
	B = chan_y[i] + 1.772*(chan_u[i]-128);
	if (R < 0) R=0;
	if (R > 255) R = 255;
	if (G < 0) G=0;
	if (G > 255) G = 255;
	if (B < 0) B=0;
	if (B > 255) B = 255;
	pixelarray[3*i]=(unsigned char)R;
    pixelarray[3*i+1]=(unsigned char)G;
    pixelarray[3*i+2]=(unsigned char)B; 
  } */

  for(i=0;i<size;++i) {   
    pixelarray[3*i]=chan_v[i];
    pixelarray[3*i+1]=chan_y[i];
    pixelarray[3*i+2]=chan_u[i];	
  } 

  return pixelarray;

}

