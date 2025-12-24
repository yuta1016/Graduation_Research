//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CameraMotionSearch.cpp
//

#include <stdio.h>
#ifndef WIN32
#include <cstring> //LINUX_2011
#endif

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================
	

using namespace XM;

const UUID CameraMotionSearchInterface::myID = UUID("3dd3ec20-a292-11d3-82cc-0050041cc4a9");
const char *CameraMotionSearchInterface::myName = "Camera Motion Matching Interface";

const UUID CameraMotionSearchTool::myID = UUID("3dd3ec21-a292-11d3-82cc-0050041cc4a9");
const char *CameraMotionSearchTool::myName = "Camera Motion Matching Tool";

const UUID CameraMotionSearchInterfaceABC::myID = UUID();

/* Definition of default query parameters (might be extended in the future to tunable parameters) */
const int    DEFAULT_QUERY_ALPHA[NB_CAMERA_OPERATIONS]   = { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 } ;
const int    DEFAULT_QUERY_BETA[NB_CAMERA_OPERATIONS-1]  = { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 } ;
const int    DEFAULT_QUERY_GAMMA                         = 1 ;

//=============================================================================
CameraMotionSearchInterfaceABC::CameraMotionSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
const UUID& CameraMotionSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionSearchInterface::GetName(void)
{ return myName; }

//=============================================================================
CameraMotionSearchInterface::CameraMotionSearchInterface(CameraMotionSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
CameraMotionSearchInterface::~CameraMotionSearchInterface()
{
}

//----------------------------------------------------------------------------
void CameraMotionSearchInterface::destroy(void)
{
	delete m_SearchTool;
}

//----------------------------------------------------------------------------
int CameraMotionSearchInterface::
SetRefDescriptorInterface(CameraMotionDescriptorInterfaceABC *RefCameraMotionDescriptorInterface)

{
  return m_SearchTool->SetRefDescriptorInterface(RefCameraMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
int CameraMotionSearchInterface::
SetQueryDescriptorInterface(CameraMotionDescriptorInterfaceABC
		       *QueryCameraMotionDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryCameraMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
double CameraMotionSearchInterface::GetDistance(void)
{
  return m_SearchTool->GetDistance();
}


//=============================================================================
CameraMotionSearchTool::CameraMotionSearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionSearchTool::
CameraMotionSearchTool(CameraMotionDescriptorInterfaceABC
                       *aQueryDescriptorInterface) :
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0)
{
#ifdef VERBOSE
	fprintf(stderr, "Connect descriptor\n");
#endif

	if (!aQueryDescriptorInterface) {
	 CameraMotionDescriptor *descriptor = new CameraMotionDescriptor();
	  aQueryDescriptorInterface = descriptor->GetInterface();
	}

	SetQueryDescriptorInterface(aQueryDescriptorInterface);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
CameraMotionSearchTool::~CameraMotionSearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& CameraMotionSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *CameraMotionSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool CameraMotionSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool CameraMotionSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int CameraMotionSearchTool::
SetRefDescriptorInterface(CameraMotionDescriptorInterfaceABC
		       *aRefCameraMotionDescriptorInterface)

{
  if (m_RefDescriptorInterface == aRefCameraMotionDescriptorInterface) 
    return 0;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->release();

  m_RefDescriptorInterface = aRefCameraMotionDescriptorInterface;

  if (m_RefDescriptorInterface)
    m_RefDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

//----------------------------------------------------------------------------
int  CameraMotionSearchTool::
SetQueryDescriptorInterface(CameraMotionDescriptorInterfaceABC
		       *aQueryCameraMotionDescriptorInterface)

{
  if (m_QueryDescriptorInterface == aQueryCameraMotionDescriptorInterface) 
    return 0;

  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->release();

  m_QueryDescriptorInterface = aQueryCameraMotionDescriptorInterface;

  if (m_QueryDescriptorInterface)
    m_QueryDescriptorInterface->addref();
  else
    return -1;

  return 0;
}

#ifndef ABS
#define ABS(x)        ((x > 0) ? (x) : (-(x)))
#endif

//----------------------------------------------------------------------------
double CameraMotionSearchTool::GetDistance(void)

{
   double tempo_distance    = 0 ;
   double speed_distance    = 0 ; 
   double duration_distance = 0 ;
   double global_distance   = 0 ;
   int  sum_alpha = 0 ;
   int  sum_beta = 0 ;
   double a ;
   double b ;
   int i ;
   
	long segment_index = 0 ;
	
   /* check for descriptors*/
   if (!m_RefDescriptorInterface) return (unsigned long)-1;
   if (!m_QueryDescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "Camera Motion Descriptor Interface") != 0) {
	  return (double)-1;
	}
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		   "Camera Motion Descriptor Interface") != 0) {
	  return (double)-1;
	}

	/* check for DescriptionMode */
	if ((m_RefDescriptorInterface->GetDescriptionMode() != MIXTURE_MODE)||(m_QueryDescriptorInterface->GetDescriptionMode() != MIXTURE_MODE))
	{
	  fprintf(stderr,"\n\n Sorry, MIXTURE_MODE description of camera motion is currently not supported for distance computation. \n\n") ;
	  return (double)-1;
	}
	
	/* check for num_segments */
	if ((m_RefDescriptorInterface->GetNumSegments() != 1)||(m_QueryDescriptorInterface->GetNumSegments() != 1))
	{
	  fprintf(stderr,"\n\n Sorry, multiple segments description of camera motion is currently not supported for distance computation. \n\n") ;
	  return (double)-1;
	}
	
	/* Temporal presence distance */
	for(i=0; i<NB_CAMERA_OPERATIONS; i++)
	{
		sum_alpha += DEFAULT_QUERY_ALPHA[i] ;
		a = m_RefDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).presence[i] ;
		b = m_QueryDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).presence[i] ;
		if (!((a==0)&(b==0)))
		   tempo_distance += DEFAULT_QUERY_ALPHA[i] * ABS( a - b ) / ( a + b ) ;
	}
	
	if (sum_alpha)
	   tempo_distance /= sum_alpha ;
	
	/* Amount of motion distance */
	for(i=0; i<NB_CAMERA_OPERATIONS-1; i++)
	{
		sum_beta += DEFAULT_QUERY_BETA[i] ;
		a = m_RefDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).speeds[i] ;
		b = m_QueryDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).speeds[i] ;
		if (!((a==0)&(b==0)))
		   speed_distance += DEFAULT_QUERY_BETA[i] * ABS( a - b ) / ( a + b ) ;
	}
	
	if (sum_beta)
	   speed_distance /= sum_beta ;
	   
	/* duration distance */
	a = m_RefDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).duration ;
	b = m_QueryDescriptorInterface->GetCameraMotionSegmentInfo(segment_index).duration ;
	if(a > 0 && b > 0)	// 2013/07/23 - Time is not implemented in DDL, skip if not present.
	{
		duration_distance = DEFAULT_QUERY_GAMMA * ABS( a - b ) / (a + b ) ;
	}
	
	/* Global distance */
	// sth no printf, write to stderre
	printf(" %f / %f / %f \n",tempo_distance,speed_distance,duration_distance) ;
	global_distance = (tempo_distance + speed_distance + duration_distance)/(2+DEFAULT_QUERY_GAMMA) ;
	
	return global_distance ;
}

//----------------------------------------------------------------------------
CameraMotionSearchInterfaceABC *CameraMotionSearchTool::GetInterface(void)
{ return &m_Interface; }

