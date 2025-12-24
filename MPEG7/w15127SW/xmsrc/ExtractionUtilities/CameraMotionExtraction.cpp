//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips
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
// Applicable File Name:  CameraMotionExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"


//=============================================================================

using namespace XM;


const UUID CameraMotionExtractionInterface::myID = UUID("6fabeca0-ce9a-11d3-82cc-0050041cc4a9");
const char *CameraMotionExtractionInterface::myName = "Camera Motion Extraction Interface";
const UUID CameraMotionExtractionTool::myID = UUID("6fabeca1-ce9a-11d3-82cc-0050041cc4a9");
const char *CameraMotionExtractionTool::myName = "Camera Motion Extraction";


const UUID CameraMotionExtractionInterfaceABC::myID = UUID();

//=============================================================================

CameraMotionExtractionInterfaceABC::CameraMotionExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================

CameraMotionExtractionInterface::CameraMotionExtractionInterface(CameraMotionExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------

CameraMotionExtractionInterface::~CameraMotionExtractionInterface()
{

}

//----------------------------------------------------------------------------

int CameraMotionExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}



//----------------------------------------------------------------------------
int CameraMotionExtractionInterface::
SetDescriptorInterface(CameraMotionDescriptorInterfaceABC
		       *aCameraMotionDescriptorInterface)
{
  return m_ExtractionTool->SetDescriptorInterface(aCameraMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
CameraMotionDescriptorInterfaceABC *CameraMotionExtractionInterface::
	GetDescriptorInterface(void)
{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------

unsigned long CameraMotionExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}



//----------------------------------------------------------------------------

const UUID& CameraMotionExtractionInterface::GetInterfaceID(void)
{ return myID; }



//----------------------------------------------------------------------------

const char *CameraMotionExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void CameraMotionExtractionInterface::destroy(void)
{
	delete m_ExtractionTool;
}

//=============================================================================

CameraMotionExtractionTool::CameraMotionExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionExtractionTool::
CameraMotionExtractionTool(CameraMotionDescriptorInterfaceABC *CameraMotion) :
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)
{
#ifdef VERBOSE
  fprintf(stderr, "Connect descriptor\n");
#endif

  if (!CameraMotion)
  {
    CameraMotionDescriptor *descriptor = new CameraMotionDescriptor();
    CameraMotion = descriptor->GetInterface();
  }

  SetDescriptorInterface(CameraMotion);
  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------

CameraMotionExtractionTool::~CameraMotionExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)  m_DescriptorInterface->release();
}



//----------------------------------------------------------------------------

bool CameraMotionExtractionTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------

bool CameraMotionExtractionTool::SupportsPull(void)
{ return false; }



//----------------------------------------------------------------------------

int CameraMotionExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media->GetVideoFileName();
  return 0;
}


//----------------------------------------------------------------------------
int CameraMotionExtractionTool::
SetDescriptorInterface(CameraMotionDescriptorInterfaceABC
		       *aCameraMotionDescriptorInterface)
{
  if (m_DescriptorInterface == aCameraMotionDescriptorInterface) 
    return 0;

  if (m_DescriptorInterface) m_DescriptorInterface->release();

  m_DescriptorInterface = aCameraMotionDescriptorInterface;

  if (m_DescriptorInterface)
    m_DescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
CameraMotionDescriptorInterfaceABC *CameraMotionExtractionTool::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
unsigned long CameraMotionExtractionTool::StartExtracting(void)
{
	CameraMotionInfo_t cmInfo ;
	SegmentedCameraMotion_t cmSegment ;
	FractionalPresence_t presence ;
	AmountOfMotion_t speeds ;
//	double x_FoeFoc = 0 ; // Not supported
//	double y_FoeFoc = 0 ; // Not supported

	int i ;

	if (!m_Media) return (unsigned long) (-1) ;

	MediaForCameraMotion* mediaCamera = ComputeFromText(m_Media) ;
	cmInfo = ExtractCameraParameters(*mediaCamera) ;

	for (i=0; i<NB_CAMERA_OPERATIONS; i++)
		presence[i] = ((double) cmInfo.presence[i])/100 ;
	for (i=0; i<NB_CAMERA_OPERATIONS-1; i++)
		speeds[i] = cmInfo.speeds[i] ;


	cmSegment = m_DescriptorInterface->
	  CreateCameraMotionInfo(cmInfo.startTime,cmInfo.duration,
				 presence, speeds ) ;
	m_DescriptorInterface->SetNumSegments(1) ;
	m_DescriptorInterface->SetDescriptionMode(MIXTURE_MODE) ;
	m_DescriptorInterface->SetCameraMotionSegmentInfo( cmSegment, 0) ;
		
	return (0);
}


//----------------------------------------------------------------------------

const UUID& CameraMotionExtractionTool::GetObjectID(void)
{ return myID; }



//----------------------------------------------------------------------------

const char *CameraMotionExtractionTool::GetName(void)
{ return myName; }



//----------------------------------------------------------------------------

CameraMotionExtractionInterfaceABC *CameraMotionExtractionTool::GetInterface(void)

{ return &m_Interface; }



