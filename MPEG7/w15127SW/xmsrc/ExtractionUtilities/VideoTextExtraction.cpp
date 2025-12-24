///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Gang Wei, Nevenka	Dimitrova, and Lalitha Agnihotri	- Philips Research.
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoTextExtraction.cpp
//


#include <stdio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif
#include "Utilities/readparam.h"
#include "momusys.h"
#include "intra.h"
#include "inter.h"
#include "coortrans.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"              /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

#include "VideoText_Detect.h"

/* remove black pixels at image boundary*/
#define BORDER 2



using namespace XM;

const UUID VideoTextExtractionInterface::myID = UUID("");
const char *VideoTextExtractionInterface::myName = "VideoTextExtractionInterface";

const UUID VideoTextExtractionTool::myID = UUID("");
const char *VideoTextExtractionTool::myName = "VideoTextExtraction";

const UUID VideoTextExtractionInterfaceABC::myID = UUID("");

//=============================================================================
VideoTextExtractionInterfaceABC::VideoTextExtractionInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
VideoTextExtractionInterface::VideoTextExtractionInterface(
                 VideoTextExtractionTool* aTool):
m_ExtractionTool(aTool)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
VideoTextExtractionInterface::~VideoTextExtractionInterface()
{
}

//----------------------------------------------------------------------------
void VideoTextExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int VideoTextExtractionInterface::SetSourceMedia(char * sVideoName)
{
  return m_ExtractionTool->SetSourceMedia(sVideoName);
}

//----------------------------------------------------------------------------
int VideoTextExtractionInterface::SetSourceMedia(MomVop *VOP)
{
	return m_ExtractionTool->SetSourceMedia(VOP);
}

//----------------------------------------------------------------------------
int VideoTextExtractionInterface::SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  return m_ExtractionTool->SetSourceMediaFrameNumber(FrameNo);
}

//----------------------------------------------------------------------------
int VideoTextExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionInterface::StartExtracting(char *sXMLfilename)

{
  return m_ExtractionTool->StartExtracting(sXMLfilename);
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
MomVop* VideoTextExtractionInterface::GetVideoText(void)

{
  return m_ExtractionTool->GetVideoText();
}

//----------------------------------------------------------------------------
const UUID& VideoTextExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoTextExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
VideoTextExtractionTool::VideoTextExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_videoSegmentDSInterface(0),
m_frameNumber(0),
m_VideoTextMedia(0)

{

  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
VideoTextExtractionTool::VideoTextExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_DescriptorInterface(0),
m_videoSegmentDSInterface(0),
m_frameNumber(0),
m_VideoTextMedia(0)

{
  //sth if no connection to DS class this is useless.
  //sth please see in example what should be done here 
/*  ParametricObjectMotionExtractionTool2 *pomextraction;

  pomextraction = new ParametricObjectMotionExtractionTool2(0);
  m_MotionExtraction=pomextraction->GetInterface();

  m_MotionD = m_MotionExtraction->GetDescriptorInterface();

  SetDescriptorInterface(theParent);

  SetExposedInterface(&m_Interface);
*/
}


//----------------------------------------------------------------------------
VideoTextExtractionTool::~VideoTextExtractionTool()
{
  /* release descriptor*/
}

//----------------------------------------------------------------------------
MomVop* VideoTextExtractionTool::GetVideoText(void)
{
	return m_VideoTextMedia;
}

//----------------------------------------------------------------------------
bool VideoTextExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool VideoTextExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int VideoTextExtractionTool::SetSourceMedia(char * sVideoName)

{
	sInVideoFileName=sVideoName;
	return 0;
}

//----------------------------------------------------------------------------
int VideoTextExtractionTool::SetSourceMedia(MomVop *VOP)
{
  if (!VOP) return -1;

  m_VideoTextMedia=VOP;
  return 0;

}
//----------------------------------------------------------------------------
int VideoTextExtractionTool::
SetSourceMediaFrameNumber(unsigned long FrameNo)
{
	m_frameNumber=FrameNo;
	return 0;
}

//----------------------------------------------------------------------------
/*GenericDSInterfaceABC* VideoTextExtractionTool::GetDescriptorInterface(void)

{
   return m_DescriptorInterface;
}
*/
//----------------------------------------------------------------------------
int VideoTextExtractionTool::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionTool::InitExtracting(void)
{
	m_videoSegmentDS=m_DescriptorInterface->CreateChild("VideoSegment");
	m_videoSegmentDSInterface=m_videoSegmentDS.GetInterface();
	return 0;
}



//----------------------------------------------------------------------------
//#define TEXT_FILE_NAME "c:\\temp\\textReport.txt"
#define _XML_OUTPUT
#define _TEXT_DETECT
#define PIXEL short

unsigned long VideoTextExtractionTool::StartExtracting(void)

{
//	FILE * fpTextReport=OpenFileNameMode(TEXT_FILE_NAME, "a");

	int Height=m_VideoTextMedia->height;
	int Width=m_VideoTextMedia->width;
	PIXEL ** imgData=New2D(Height, Width);
	one_D_to_two_D(imgData, Height, Width, 
		m_VideoTextMedia->y_chan->data->u, Height, Width);
//	write_raw_image(imgData, Height, Width, "c:\\temp\\y_test.raw");

	text_detect_2D(imgData, Height, Width, &m_curTextFrame, m_frameNumber);
	m_curTextFrame.model_validate(&m_prevTextFrame);
//	m_prevTextFrame.out_put(fpTextReport);
#ifdef USEXML
/*	if (m_DescriptorInterface->GetDSName()!= "VideoTextExtractionInterface") {
	  return (unsigned long)-1;
	}
*/
/*	GenericDS
		videoSegment_DS=m_DescriptorInterface->CreateChild("VideoSegment");
	GenericDSInterfaceABC
		*videoSegment_DSInterface=videoSegment_DS.GetInterface();
*/
	int i;
	int larger=(Height>Width) ? Height : Width;
	STextModel* textModels=m_prevTextFrame.get_models();
	int textModelNum=m_prevTextFrame.get_model_num();
	double tempVal;
	
	for (i=1; i<=m_prevTextFrame.get_model_num(); i++) 
	{
		if (textModels[i].valid) 
		{
			//create an instance of VideoText
			GenericDS
				videoText_DS=m_videoSegmentDSInterface->CreateChild("VideoText");
			GenericDSInterfaceABC
				*videoText_DSInterface=videoText_DS.GetInterface();
		
			//VideoText--->>textType
			GenericDS
				textType_DS=videoText_DSInterface->CreateChild("TextType");
			GenericDSInterfaceABC
				*textTyep_DSInterface=textType_DS.GetInterface();
			textTyep_DSInterface->SetValue("superimposed");
			
			//VideoText--->>MovingRegion
			GenericDS
				movingRegion_DS=videoText_DSInterface->CreateChild("MovingRegion");
			GenericDSInterfaceABC
				*movingRegion_DSInterface=movingRegion_DS.GetInterface();
			movingRegion_DSInterface->SetAttribute("SpatioalConnectivity", "true");
			movingRegion_DSInterface->SetAttribute("TemporalConnectivity", "true");

			//VideoText--->>MovingRegion--->>MeidiaTime
			GenericDS
				mediaTime_DS=movingRegion_DSInterface->CreateChild("MediaTime");
			GenericDSInterfaceABC
				*mediaTime_DSInterface=mediaTime_DS.GetInterface();

			//VideoText--->>MovingRegion--->>MeidiaTime--->>MediaRelIncrTime
			GenericDS
				mediaRecIncrTime_DS=mediaTime_DSInterface->CreateChild("MediaRelIncrTime");
			GenericDSInterfaceABC
				*mediaRelIncrTime_DSInterface=mediaTime_DS.GetInterface();
			mediaRelIncrTime_DSInterface->SetValue(m_prevFrameNumber);

			//VideoText--->>MovingRegion--->>BoundingBox
			GenericDS
				boundingBox_DS=movingRegion_DSInterface->CreateChild("BoundingBox");
			GenericDSInterfaceABC
				*boundingBox_DSInterface=boundingBox_DS.GetInterface();

			//VideoText--->>MovingRegion--->>BoundingBox--->>BoxHeight
			GenericDS
				boxHeight_DS=boundingBox_DSInterface->CreateChild("BoxHeight");
			GenericDSInterfaceABC
				*boxHeight_DSInterface=boxHeight_DS.GetInterface();
			tempVal=(double)((float)textModels[i].height/(float)larger);
			boxHeight_DSInterface->SetValue(tempVal);

			//VideoText--->>MovingRegion--->>BoundingBox--->>BoxWidth
			GenericDS
				boxWidth_DS=boundingBox_DSInterface->CreateChild("BoxWidth");
			GenericDSInterfaceABC
				*boxWidth_DSInterface=boxWidth_DS.GetInterface();
			tempVal=(double)((float)textModels[i].width/(float)larger);
			boxWidth_DSInterface->SetValue(tempVal);

			//VideoText--->>MovingRegion--->>BoundingBox--->>BoxCenterCol
			GenericDS
				boxCenterCol_DS=boundingBox_DSInterface->CreateChild("BoxCenterCol");
			GenericDSInterfaceABC
				*boxCenterCol_DSInterface=boxCenterCol_DS.GetInterface();
			tempVal=(double)((float)textModels[i].cenCol/(float)larger);
			boxCenterCol_DSInterface->SetValue(tempVal);

			//VideoText--->>MovingRegion--->>BoundingBox--->>BoxCenterRow
			GenericDS
				boxCenterRow_DS=boundingBox_DSInterface->CreateChild("BoxCenterRow");
			GenericDSInterfaceABC
				*boxCenterRow_DSInterface=boxCenterRow_DS.GetInterface();
			tempVal=(double)((float)textModels[i].cenRow/(float)larger);
			boxCenterRow_DSInterface->SetValue(tempVal);
		}
	}
#endif

	m_prevFrameNumber=m_frameNumber;


	Delete2D(imgData, Height);
	m_prevTextFrame=m_curTextFrame;
//	fclose(fpTextReport);
	return 0;
}

//----------------------------------------------------------------------------
//no longer used
unsigned long VideoTextExtractionTool::StartExtracting(char * sXMLFileName)

{

	track_text_to_get_XML(sXMLFileName);
	return 0;
}

//----------------------------------------------------------------------------
//no longer used
void VideoTextExtractionTool::track_text_to_get_XML(char * sXMLFileName)
{

}

//----------------------------------------------------------------------------
//no longer used
void VideoTextExtractionTool::write_XML_header(FILE* fpXML, char* sTitle, int frmHeight, int frmWidth)
{
}

//----------------------------------------------------------------------------
//no longer used
void VideoTextExtractionTool::write_XML_end(FILE* fpXML)
{
}

//----------------------------------------------------------------------------
//no longer used
void VideoTextExtractionTool::output_to_XML(FILE * fpXML, CTextFrame & textFrame, int frmInd, int larger)
{
}

//----------------------------------------------------------------------------
unsigned long VideoTextExtractionTool::PostExtracting(void)

{
  return 0;
}

//----------------------------------------------------------------------------
/*
MomVop* VideoTextExtractionTool::GetVideoText(void)

{
  return m_VideoText;
}
*/

//----------------------------------------------------------------------------
const UUID& VideoTextExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoTextExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
VideoTextExtractionInterfaceABC 
   *VideoTextExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
