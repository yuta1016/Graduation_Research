//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Osamu Hori, Toshimitsu Kaneko, Koji Yamamoto
// R&D Center, TOSHIBA Corp.
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SequentialSummaryExtraction.cpp
//

#include <stdio.h>
#ifdef _VISUALC_
#else
#include <unistd.h>
#endif
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "DescriptionSchemes/DescriptionSchemes.h"         /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/readparam.h"
#include "math.h"

//static int initextractionwascalled=0;


//=============================================================================
// sth is it possible to merge with SummarizationExtraction?
using namespace XM;


const UUID SequentialSummaryExtractionInterface::myID = UUID("");
const char *SequentialSummaryExtractionInterface::myName =
"SequentialSummaryDSExtractionInterface";

const UUID SequentialSummaryExtractionTool::myID = UUID("");
const char *SequentialSummaryExtractionTool::myName =
"SequentialSummaryExtraction";

const UUID SequentialSummaryExtractionInterfaceABC::myID = UUID();

//=============================================================================
SequentialSummaryExtractionInterfaceABC::SequentialSummaryExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
SequentialSummaryExtractionInterface::
SequentialSummaryExtractionInterface(SequentialSummaryExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SequentialSummaryExtractionInterface::~SequentialSummaryExtractionInterface()
{
}

//----------------------------------------------------------------------------
const UUID& SequentialSummaryExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SequentialSummaryExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
void SequentialSummaryExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int SequentialSummaryExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC *Media)
{
  return m_ExtractionTool->SetSourceMedia(Media);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC* SequentialSummaryExtractionInterface::
GetDescriptionInterface(void)

{
  return m_ExtractionTool->GetDescriptionInterface();
}

//----------------------------------------------------------------------------
int SequentialSummaryExtractionInterface::
SetDescriptionInterface(GenericDSInterfaceABC
		       *aSequentialSummaryDescriptionInterface)

{
  return m_ExtractionTool->
    SetDescriptionInterface(aSequentialSummaryDescriptionInterface);
}

//----------------------------------------------------------------------------
unsigned long SequentialSummaryExtractionInterface::InitExtracting(void)

{
  return(m_ExtractionTool->InitExtracting());
}

//----------------------------------------------------------------------------
unsigned long SequentialSummaryExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SequentialSummaryExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
SequentialSummaryExtractionTool::SequentialSummaryExtractionTool():
m_Interface(this),
m_DescriptionInterface(NULL)

{
  SetExposedInterface(&m_Interface);
}

//=============================================================================
SequentialSummaryExtractionTool::
SequentialSummaryExtractionTool(GenericDSInterfaceABC
			*SequentialSummary):
m_Interface(this),
m_DescriptionInterface(NULL)

{
  // connect Descritors with Extractions*/
  SetDescriptionInterface(SequentialSummary);
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SequentialSummaryExtractionTool::~SequentialSummaryExtractionTool()
{
	if (!m_pOldImage)
		freevop(m_pOldImage);
	if (!m_pImage)
		freevop(m_pImage);
}

//----------------------------------------------------------------------------
bool SequentialSummaryExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SequentialSummaryExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
// This informs the extractor where the source data is coming from. This
// could be an audio file, image stream etc. Here it's just a filename.
int SequentialSummaryExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *Media)
{
  if(Media==NULL) return -1;

  m_Media	= Media;

  return(0);
}


//----------------------------------------------------------------------------
GenericDSInterfaceABC* SequentialSummaryExtractionTool::
GetDescriptionInterface(void)

{
  return m_DescriptionInterface;
}

//----------------------------------------------------------------------------
int SequentialSummaryExtractionTool::
SetDescriptionInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptionInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long SequentialSummaryExtractionTool::InitExtracting(void)
{
  /* test if parent exists*/
  if (!m_DescriptionInterface)  return (unsigned long)-1;

  /* test if parent is document node. If yes, remove old SequentialSummary*/
  if (m_DescriptionInterface->GetDSName() == "#document") {
    if (!m_SequentialSummary.isNull()) {
      if (m_SequentialSummary.GetDSName() == "SequentialSummary") {
	m_DescriptionInterface->RemoveChild(m_SequentialSummary);
      }
    }
  }

  /* create SequentialSummary*/
  m_SequentialSummary=m_DescriptionInterface->
    CreateChild("SequentialSummary");

  // initialize source & summary video filenames
  strcpy(m_szSourceVideoName, m_Media->GetVideoFileName());
  strcpy(m_szVideoSummaryName, m_szSourceVideoName);
  char* pTmp = strrchr(m_szVideoSummaryName, '.');
  *pTmp = '\0';
  strcat(m_szVideoSummaryName, "_summary.mpg");

  // create source & summary video locator
  GenericDSInterfaceABC* SequentialSummaryInterface = m_SequentialSummary.GetInterface();
  CreateVideoLocatorNode(SequentialSummaryInterface, m_szSourceVideoName, m_szVideoSummaryName);

  /* initialize description values*/
  m_videoSummaryFrame = 0;
  m_srcVideoFrame = 0;
  m_pOldImage = NULL;
  m_pImage = NULL;

  return(0UL);
}

//----------------------------------------------------------------------------
// This uses the m_Media pointer to extract the relevant data and put it
// into the 
unsigned long SequentialSummaryExtractionTool::StartExtracting(void)

{
  // First check that it's all correctly initialised
  if (m_SequentialSummary.isNull()) return (unsigned long)-1;
  if (m_SequentialSummary.GetDSName() != "SequentialSummary") return (unsigned long)-1;

  GenericDSInterfaceABC* SequentialSummaryInterface = m_SequentialSummary.GetInterface();

  // Extract the value we want. For the dummy it's just the number of frames
  // in the Sequence multiplied with the extraction parameter value
  MomVop *imageMedia = m_Media->GetImage();
  if (imageMedia == NULL)
	  return (unsigned long)-1;

  // These values must not change during the extraction
  m_thumbWidth = imageMedia->width/8;
  m_thumbHeight = imageMedia->height/8;

  // Allocate the buffer for the shrinked image if it not allocated
  if (!m_pOldImage)
    m_pOldImage=initvop(m_thumbWidth, m_thumbHeight,_444_,
                        UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
  if (!m_pImage)
    m_pImage=initvop(m_thumbWidth, m_thumbHeight,_444_,
	                 UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);

  if (!m_pOldImage) return (unsigned long)-1;
  if (!m_pImage) return (unsigned long)-1;

  // Extract the thumbnail image and the frame activity
  // I..F..F..I..F..F..I.. (I:ImageLocator, F:FrameActivity)
  m_srcVideoFrame = m_Media->GetImageTime();

  if (!(m_srcVideoFrame % 3)) {
	  // swap image buffer
      MomVop* pTmpVop = m_pOldImage;
      m_pOldImage = m_pImage;
      m_pImage = pTmpVop;

      ShrinkImage(imageMedia, m_pImage);

      if (!(m_srcVideoFrame % 9)) {
        // save image
        char filename[MAXMEDIANAMELEN];
        sprintf(filename, "%s.dir/%ld.ppm", m_szSourceVideoName, m_videoSummaryFrame);
		m_SummaryDB.StoreNamedVop(filename, m_pImage);

		// create DOM node
		CreateImageLocatorVSCNode(SequentialSummaryInterface, m_srcVideoFrame, m_videoSummaryFrame);
		m_videoSummaryFrame++;
	  } else {
        // calc frame activity
        double frameActivity = CalcFrameActivity(m_pOldImage, m_pImage);
		// create DOM node
		CreateFrameActivityVSCNode(SequentialSummaryInterface, m_srcVideoFrame, frameActivity);
	  }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This initialises the extraction process. Bascially it should 
// reset all storage etc. However, as this hasn't got any storage
// it's a bit pointless.
unsigned long SequentialSummaryExtractionTool::PostExtracting(void)
{
  // Encode thumbnail images into a mpeg video file
  char* parmFilename = "encoder.par";
  char ppmFilename[MAXMEDIANAMELEN];

  sprintf(ppmFilename, "%s.dir/%%d", m_szSourceVideoName);

  if (!m_SummaryDB.GenEncoderParFile(parmFilename, ppmFilename,
				 m_videoSummaryFrame, m_thumbWidth, m_thumbHeight)) return 1;

  if (!m_SummaryDB.EncodeNamedMPEGVideo(m_szVideoSummaryName, parmFilename, NULL)) return 1;

  // Remove temporaly files

  // Files used by the encoder
  unlink(parmFilename);
  unlink("-.bits");
  unlink("-.psnr");
  unlink("-.time");

  // Thumbnail files
  for (int i = 0; i < m_videoSummaryFrame; i++) {
    sprintf(ppmFilename, "%s.dir/%d.ppm", m_szSourceVideoName, i);
	unlink(ppmFilename);
  }

  return(0UL);
}

//----------------------------------------------------------------------------
const UUID& SequentialSummaryExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SequentialSummaryExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SequentialSummaryExtractionInterfaceABC *SequentialSummaryExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
void SequentialSummaryExtractionTool::CreateVideoLocatorNode(GenericDSInterfaceABC *parentInterface, char* srcVideoName, char* videoSummaryName)
{
	{
	// <SourceLocator> for Source Video
	GenericDS SourceLocator = parentInterface->CreateChild("SourceLocator");
	GenericDSInterfaceABC *SourceLocatorInterface = SourceLocator.GetInterface();

	//   <MediaUri> for Source Video
	GenericDS MediaUri = SourceLocatorInterface->CreateChild("MediaUri");
	GenericDSInterfaceABC *MediaUriInterface = MediaUri.GetInterface();
	MediaUriInterface->SetValue(srcVideoName);
	}

	{
	// <VideoSegmentLocator> for Video Skim
	GenericDS VideoSummaryLocator = parentInterface->CreateChild("VideoSummaryLocator");
	GenericDSInterfaceABC *VideoSummaryLocatorInterface = VideoSummaryLocator.GetInterface();

	//   <MediaUri> for Video Skim
	GenericDS MediaUri = VideoSummaryLocatorInterface->CreateChild("MediaUri");
	GenericDSInterfaceABC *MediaUriInterface = MediaUri.GetInterface();
	MediaUriInterface->SetValue(videoSummaryName);
	}
}

//----------------------------------------------------------------------------
void SequentialSummaryExtractionTool::CreateImageLocatorVSCNode(GenericDSInterfaceABC *parentInterface, int ipicFrame, int skimFrame)
{
  // <VisualSummaryComponent>
  GenericDS VisualSummaryComponent = parentInterface->CreateChild("VisualSummaryComponent");
  GenericDSInterfaceABC *VisualSummaryComponentInterface = VisualSummaryComponent.GetInterface();

  {
    //   <ComponentSourceTime>
    GenericDS ComponentSourceTime = VisualSummaryComponentInterface->CreateChild("ComponentSourceTime");
    GenericDSInterfaceABC *ComponentSourceTimeInterface = ComponentSourceTime.GetInterface();

    //     <MediaRelIncrTimePoint> for original video
    GenericDS MediaRelIncrTimePoint = ComponentSourceTimeInterface->CreateChild("MediaRelIncrTimePoint");
    GenericDSInterfaceABC *MediaRelIncrTimePointInterface = MediaRelIncrTimePoint.GetInterface();
    MediaRelIncrTimePointInterface->SetAttribute("timeUnit", "PT1N30F");
    MediaRelIncrTimePointInterface->SetAttribute("timeBase", "../../../SourceLocator[1]");
    MediaRelIncrTimePointInterface->SetValue(ipicFrame);
  }

  {
    //   <ImageLocator>
    GenericDS ImageLocator = VisualSummaryComponentInterface->CreateChild("ImageLocator");
    GenericDSInterfaceABC *ImageLocatorInterface = ImageLocator.GetInterface();

    //     <MediaRelIncrTimePoint> for video skim
    GenericDS MediaRelIncrTimePoint = ImageLocatorInterface->CreateChild("MediaRelIncrTimePoint");
    GenericDSInterfaceABC *MediaRelIncrTimePointInterface = MediaRelIncrTimePoint.GetInterface();
    MediaRelIncrTimePointInterface->SetAttribute("timeUnit", "PT1N30F");
    MediaRelIncrTimePointInterface->SetAttribute("timeBase", "../../../VideoSummaryLocator[1]");
    MediaRelIncrTimePointInterface->SetValue(skimFrame);
  }
}

//----------------------------------------------------------------------------
void SequentialSummaryExtractionTool::CreateFrameActivityVSCNode(GenericDSInterfaceABC *parentInterface, int ppicFrame, double frameActivity)
{
  // <VisualSummaryComponent>
  GenericDS VisualSummaryComponent = parentInterface->CreateChild("VisualSummaryComponent");
  GenericDSInterfaceABC *VisualSummaryComponentInterface = VisualSummaryComponent.GetInterface();

  {
    //   <ComponentSourceTime>
    GenericDS ComponentSourceTime = VisualSummaryComponentInterface->CreateChild("ComponentSourceTime");
    GenericDSInterfaceABC *ComponentSourceTimeInterface = ComponentSourceTime.GetInterface();

    //     <MediaRelIncrTimePoint> for original video
    GenericDS MediaRelIncrTimePoint = ComponentSourceTimeInterface->CreateChild("MediaRelIncrTimePoint");
    GenericDSInterfaceABC *MediaRelIncrTimePointInterface = MediaRelIncrTimePoint.GetInterface();
    MediaRelIncrTimePointInterface->SetAttribute("timeUnit", "PT1N30F");
    MediaRelIncrTimePointInterface->SetAttribute("timeBase", "../../../SourceLocator[1]");
    MediaRelIncrTimePointInterface->SetValue(ppicFrame);
  }

  {
	char szFrameActivity[10];
	sprintf(szFrameActivity, "%.1f", frameActivity);

    //   <FrameActivity>
    GenericDS FrameActivity = VisualSummaryComponentInterface->CreateChild("FrameActivity");
    GenericDSInterfaceABC *FrameActivityInterface = FrameActivity.GetInterface();
    FrameActivityInterface->SetValue(szFrameActivity);
  }
}

//----------------------------------------------------------------------------
void SequentialSummaryExtractionTool::ShrinkImage(MomVop* inImage, MomVop* outImage)
{
  // calclate the mean color for each 8*8 block
  // and create a shrinked image
  int y_sum, u_sum, v_sum;
  for (int y = 0; y < outImage->height; y++)
    for (int x = 0; x < outImage->width; x++) {
      // loop inside the 8*8 block
      y_sum = u_sum = v_sum = 0;
      for (int by = 0; by < 8; by++)
        for (int bx = 0; bx < 8; bx++) {
          y_sum += inImage->y_chan->data->u[(x*8+bx)+(y*8+by)*inImage->width];
          u_sum += inImage->u_chan->data->u[(x*8+bx)+(y*8+by)*inImage->width];
          v_sum += inImage->v_chan->data->u[(x*8+bx)+(y*8+by)*inImage->width];
		}
      outImage->y_chan->data->u[x+y*outImage->width] = y_sum / 64; // 8*8
      outImage->u_chan->data->u[x+y*outImage->width] = u_sum / 64;
      outImage->v_chan->data->u[x+y*outImage->width] = v_sum / 64;
	}
}

//----------------------------------------------------------------------------
double SequentialSummaryExtractionTool::CalcFrameActivity(MomVop* oldImage, MomVop* newImage)
{
  double oldImgHist[16];
  double newImgHist[16];

  memset(oldImgHist, 0, sizeof(double)*16);
  memset(newImgHist, 0, sizeof(double)*16);

  // calc histogram
  for (int y = 0; y < m_thumbHeight; y++)
    for (int x = 0; x < m_thumbWidth; x++) {
      oldImgHist[oldImage->y_chan->data->u[x+y*oldImage->width]/16]++;
      newImgHist[newImage->y_chan->data->u[x+y*newImage->width]/16]++;
	}

  // difference between two histograms 
  double diff = 0;
  for (int i = 0; i < 16; i++)
    diff += fabs(newImgHist[i] - oldImgHist[i]);

  double frameActivity = diff * 4.0 / (double)(m_thumbWidth * m_thumbHeight);
  if (frameActivity > 1.0)
	  frameActivity = 1.0;

  return frameActivity;
}
