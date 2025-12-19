//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim
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
// Applicable File Name:  IIColorExtraction.cpp
//

#ifdef _VISUALC_
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#define _mkdir(dir)  mkdir(dir,S_IRWXU|S_IRWXG|S_IRWXO)
#endif
#include <stdio.h>
#include "AddressLib/momusys.h"
#include "AddressLib/inter.h"
#include "AddressLib/intra.h"
#include "AddressLib/fifo.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "ExtractionUtilities/IIColor_Extraction.h"
#include "Utilities/readparam.h"
//#include "buffalloc.h"

#ifndef _MAX_PATH
#define _MAX_PATH 512
#endif

//static int initextractionwascalled=0;

//=============================================================================
	

using namespace XM;

unsigned char* interleaveRGB2 (unsigned char* pixel_R,unsigned char* pixel_G,
			   unsigned char* pixel_B,int size);

const UUID IIColorExtractionInterface::myID = UUID("");
const char *IIColorExtractionInterface::myName =
"IIColorDescriptorExtractorInterface";

const UUID IIColorExtractionTool::myID = UUID("");
const char *IIColorExtractionTool::myName = "IIColorDescriptorExtractor";

const UUID IIColorExtractionInterfaceABC::myID = UUID();

//=============================================================================
IIColorExtractionInterfaceABC::IIColorExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
IIColorExtractionInterface::
IIColorExtractionInterface(IIColorExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
IIColorExtractionInterface::~IIColorExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& IIColorExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void IIColorExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc.
int IIColorExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
IIColorDescriptorInterfaceABC* IIColorExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int IIColorExtractionInterface::
SetDescriptorInterface(IIColorDescriptorInterfaceABC
		       *aIIColorDescriptorInterface)

{
  return m_ExtractionTool->
    SetDescriptorInterface(aIIColorDescriptorInterface);
}

//----------------------------------------------------------------------------
//unsigned long IIColorExtractionInterface::InitExtracting(void)
//
//{
//  return(m_ExtractionTool->InitExtracting());
//}

//----------------------------------------------------------------------------
unsigned long IIColorExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long IIColorExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
IIColorExtractionTool::IIColorExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(NULL),
m_FrameCnt(0)

{
  m_FrameCnt=0;
  SetExposedInterface(&m_Interface);
}

//=============================================================================
IIColorExtractionTool::
IIColorExtractionTool(IIColorDescriptorInterfaceABC
			*aIIColor):
m_Interface(this),
m_DescriptorInterface(NULL),
m_Media(NULL),
m_FrameCnt(0)

{
  /* create descriptor if it does not exist*/
  if (!aIIColor) {
    IIColorDescriptor *descriptor =
      new IIColorDescriptor;
    aIIColor=descriptor->GetInterface();
  }

  /* connect descritors with Extraction*/
  SetDescriptorInterface(aIIColor);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IIColorExtractionTool::~IIColorExtractionTool()
{
  // release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool IIColorExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool IIColorExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int IIColorExtractionTool::SetSourceMedia(MultiMediaInterfaceABC* media)
{
  if(media==NULL) return -1;
  m_Media=media;

  return 0;
}


//----------------------------------------------------------------------------
IIColorDescriptorInterfaceABC* IIColorExtractionTool::
GetDescriptorInterface(void)

{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int IIColorExtractionTool::
SetDescriptorInterface(IIColorDescriptorInterfaceABC
		       *aIIColorDescriptorInterface)

{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == aIIColorDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = aIIColorDescriptorInterface;
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
unsigned long IIColorExtractionTool::StartExtracting(void)

{
  // select media type you require and remove comment
  MomVop *ImageMedia;
  //  AudioType *AudioMedia;  // type of audio media still  needs to be defined

  // First check that it's all correctly initialised
  if (m_DescriptorInterface==NULL) 
	  return (unsigned long)-1;
  if(strcmp(m_DescriptorInterface->GetName(),
	    "IIColorDescriptionInterface") != 0) 
		return (unsigned long)-1;


  if (m_Media==NULL) return (unsigned long)-1;
  ImageMedia=m_Media->GetImage();
  if (ImageMedia==NULL) return (unsigned long)-1;

  //---------------------------------------------
  double pix, piy;
  unsigned char** nlabel;
  double** xyzbuff;
  double** org_xyzbuff;
  double** xybuff;
  int** xybincnt;
  int tTarget = 6500;
  double* xyzbuff1;
  double* xyzbuff2;
  int i,j;
  int bin_size =60;
  unsigned char *RGB, *imagebuff;
  int imsize;

  imsize = ImageMedia->width*ImageMedia->height;

  RGB = interleaveRGB2(ImageMedia->v_chan->data->u, ImageMedia->y_chan->data->u,
		   ImageMedia->u_chan->data->u, imsize);

/*  FILE *fout;
  fout = fopen("out.raw","wb");

  fwrite(RGB,sizeof(char),imsize*3,fout);

  fclose(fout);*/

  	//-- label buffer
  nlabel = IIC_cBuffAlloc2(ImageMedia->height, ImageMedia->width);
	//-- XYZ buffer
  xyzbuff = IIC_dBuffAlloc2(ImageMedia->height, ImageMedia->width*3);
  org_xyzbuff = IIC_dBuffAlloc2(ImageMedia->height, ImageMedia->width*3);
    //-- xy buffer
  xybuff = IIC_dBuffAlloc2(ImageMedia->height, ImageMedia->width*2);
	//-- xy bin count buffer
  xybincnt = nBuffAlloc2(bin_size, bin_size);

  xyzbuff1  = (double *)malloc(imsize*3*sizeof(double));
  xyzbuff2 = (double *)malloc(imsize*3*sizeof(double));


  gamma_init();

  IIC_rgb2xyz_marklabel(RGB, xyzbuff, nlabel, ImageMedia->width, ImageMedia->height);
  for(i=0;i<ImageMedia->height;i++){
	  for(j=0;j<(ImageMedia->width*3);j+=3){
		  org_xyzbuff[i][j+0]=xyzbuff[i][j+0];
		  org_xyzbuff[i][j+1]=xyzbuff[i][j+1];
		  org_xyzbuff[i][j+2]=xyzbuff[i][j+2];
	  } 
  }

  calc_xy(xyzbuff, xybuff, nlabel, ImageMedia->width, ImageMedia->height);

  removenoise_xy(xybuff, nlabel, xybincnt, ImageMedia->width, ImageMedia->height, &pix, &piy);

  interleave_xyzbuff(xyzbuff1, org_xyzbuff, ImageMedia->width, ImageMedia->height);

  temperature_conversion(xyzbuff1, xyzbuff2, imsize*3, ImageMedia->width, ImageMedia->height, pix, piy, tTarget);

  imagebuff = new unsigned char[imsize*3];
  xyz2rgb(imagebuff, xyzbuff2, ImageMedia->width, ImageMedia->height);

  //test
  //test
  FILE *fout2;

  char *szName,szName2[_MAX_PATH],szDir[_MAX_PATH];
  //char ch = '\\';
  char *pdest;
  szName = m_Media->GetImageFileName();

  // search the location of '/' or '\\' in szName
  pdest = strrchr(szName,'/');
  if(pdest == 0)
  {
    pdest = strrchr(szName,'\\');
  }
  int n = 0;
  if(pdest != 0)
  {
    n = pdest - szName + 1;

    // extract the path name from szName
    strncpy(szDir,szName,n-1);
    szDir[n-1] = '\0';
  }
  else
    szDir[n] = '\0';

  // extract the filenane from szName
  int len = strlen(szName);
//  printf("%d\n",len);
  strncpy(szName2,szName+n,len-n-4);
  szName2[len-n-4] = '\0';



  // add 6500K folder after the path name
  //strcat(szDir,"\\6500K\\");
  strcat(szDir,"/6500K/");
#if defined(_WIN32)
  _mkdir(szDir);
#else 
  // add 6500K folder after the path name
  mkdir(szDir, 0777);
#endif



  // create a file for storing
  strcat(szName2,".raw");
  strcat(szDir,szName2);

  // store the file
  fout2 = fopen(szDir,"wb");
  if(!fout2)
  {
    printf("Failed to open file %s\n",szDir);
  }
  else
  {
    fwrite(imagebuff,sizeof(char),imsize*3,fout2);
    fclose(fout2);
  }

  //-- free label buffer 
  for(i=0; i<ImageMedia->height; i++) {
	  free(xyzbuff[i]);
	  free(org_xyzbuff[i]);
	  free(nlabel[i]);
	  free(xybuff[i]);
  }
  for(i=0; i<bin_size; i++) {
	  free(xybincnt[i]);
  }

  free(xyzbuff);
  free(org_xyzbuff);
  free(xyzbuff1);
  free(xyzbuff2);
  free(nlabel);
  free(xybuff);
  free(xybincnt);
  free(imagebuff);

/*  unsigned char *RGB;
  int PCTBC[2],imsize;
  unsigned char** nlabel;
  double** xyzbuff; 
  double pix, piy;
  int* ctemperature;
  ctemperature=new(int);

  imsize = ImageMedia->width*ImageMedia->height;

  RGB = interleaveRGB(ImageMedia->v_chan->data->u, ImageMedia->y_chan->data->u,
		   ImageMedia->u_chan->data->u, imsize);

  FILE *fout;
  fout = fopen("out.raw","wb");

  fwrite(RGB,sizeof(char),imsize*3,fout);

  fclose(fout); 


  //-- label buffer
  nlabel = cBuffAlloc2(ImageMedia->height, ImageMedia->width);
  //-- XYZ buffer
  xyzbuff = dBuffAlloc2(ImageMedia->height, ImageMedia->width*3);

  rgb2xyz_marklabel(RGB, xyzbuff, nlabel, ImageMedia->width, 
	  ImageMedia->height);

  fout = fopen("out2.raw","wb");
  unsigned char *nLabel=new unsigned char[imsize];
  for (int k=0; k<ImageMedia->height; k++)
      for (int j=0; j< ImageMedia->width; j++) 
		  nLabel[k*ImageMedia->width + j] = nlabel[k][j];

  fwrite(nLabel,sizeof(char),imsize,fout);
  free(nLabel);
  fclose(fout); 

  perc_ill(xyzbuff, nlabel, ImageMedia->width, ImageMedia->height, &pix, &piy);
	
  //-- chromaticity -> color temperature conversion
  convert_xy2temp(pix, piy, ctemperature);

  PCTBC_Extraction(ctemperature, PCTBC);

  // print-out result
  cerr << "ctemperatre =" << *ctemperature << "Category = " << PCTBC[0] << "  SubRangeIndex =" << PCTBC[1] <<endl;
 
  m_DescriptorInterface->SetIIColor_Component(PCTBC);

  //-- free label buffer 
  for(int i=0; i<ImageMedia->height; i++) {
 	free(xyzbuff[i]);
	free(nlabel[i]);
  }
  free(xyzbuff);
  free(nlabel); 
  delete ctemperature; */

  // All done
  return 0;
} 

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long IIColorExtractionTool::PostExtracting(void)

{
  // In this specific case you could als write the number of frames here
  //m_DescriptorInterface->SetDummyContents(m_FrameCnt*
  //					  m_DummyExtractionParameter);

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& IIColorExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
IIColorExtractionInterfaceABC *IIColorExtractionTool::GetInterface(void)
{ return &m_Interface; }


//---------------------------------------------------------------------------
unsigned char* interleaveRGB2 (unsigned char* chan_v, 
			   unsigned char* chan_y,
			   unsigned char* chan_u, int size)
{
//  int R, G, B;
  int i;
  unsigned char *pixelarray=new unsigned char[size*3];

  for(i=0;i<size;++i) {   
    pixelarray[3*i]=chan_v[i];
    pixelarray[3*i+1]=chan_y[i];
    pixelarray[3*i+2]=chan_u[i];	
  } 

  return pixelarray;

} 

