//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
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
// Applicable File Name:  MotionActivityExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "fifo.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//knk 
#include "motion_activity_extraction.h" //Initialize_Descriptor(MotionActivityDescriptorInfo *pMAD,int spatial_flag,int temporal_flag, int direction_flag);
//=============================================================================
	

using namespace XM;


const UUID MotionActivityExtractionInterface::myID = UUID("");
const char *MotionActivityExtractionInterface::myName = "Motion Activity Extraction Interface";

const UUID MotionActivityExtractionTool::myID = UUID("");
const char *MotionActivityExtractionTool::myName = "Motion Activity Extractor";

const UUID MotionActivityExtractionInterfaceABC::myID = UUID();

//=============================================================================
MotionActivityExtractionInterfaceABC::MotionActivityExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MotionActivityExtractionInterface::MotionActivityExtractionInterface(MotionActivityExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionActivityExtractionInterface::~MotionActivityExtractionInterface()
{
}

//----------------------------------------------------------------------------
int MotionActivityExtractionInterface::SetSourceMedia(MultiMediaInterfaceABC
						      *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
int MotionActivityExtractionInterface::
SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *aMotionActivityDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aMotionActivityDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long MotionActivityExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
//unsigned long MotionActivityExtractionInterface::InitExtracting(void)
//
//{
//  return m_ExtractionTool->InitExtracting();
//}

//------------------------------------------------------------------- for SLMA
unsigned long MotionActivityExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
const UUID& MotionActivityExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
MotionActivityExtractionTool::MotionActivityExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivityExtractionTool::
MotionActivityExtractionTool(MotionActivityDescriptorInterfaceABC
			     *motionactivity):
m_Interface(this),
m_DescriptorInterface(0),
m_Media(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!motionactivity) {
	  MotionActivityDescriptor *descriptor =
	    new MotionActivityDescriptor();
	  motionactivity=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(motionactivity);

	/* create sub extraction tools and
	   connect them with descriptor if availabe*/
	/* no sub extraction tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionActivityExtractionTool::~MotionActivityExtractionTool()
{
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();

}

//----------------------------------------------------------------------------
bool MotionActivityExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool MotionActivityExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int MotionActivityExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media->GetImage();
  return 0;
}


//----------------------------------------------------------------------------
int MotionActivityExtractionTool::
SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
		       *aMotionActivityDescriptorInterface)

{
       /* check if new value is different from old value*/
        if (m_DescriptorInterface == 
	    aMotionActivityDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aMotionActivityDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();

	  /* conect sub descriptors with sub coding schemes*/
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub extraction tools schemes*/
	  /* not available*/
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionActivityExtractionTool::StartExtracting(void)

{
	int i;
	int division_block; // for SLMA

        /* check for descriptor*/
        if (!m_DescriptorInterface) return (unsigned long)-1;

	if (strcmp(m_DescriptorInterface->GetName(),
		   "Motion Activity Descriptor Interface") != 0) {
	  return (unsigned long)-1;
	}

	/* check for media*/
	if (!m_Media) return (unsigned long)-1;

	/* perform extraction of feature from media*/

	/* call extraction macro*/
	{
	  MotionActivityDescriptorInfo info;

	  //Initialize info
	  //direction_flag = 1
	  //spatial_distribution_flag = 1
	  //temporal_distribution_flag = 1
	  Initialize_Descriptor(&info, 1, 1, 1, 1); // for SLMA

	  /*June 22, 2001 */
	  /* Ajay Divakaran */
	  /* Call Frame Level Descriptor Computation for 
	  intensity, direction, spatial and temporal attributes
	  Since we use static variables, the intermediate computations 
	  are accumulated from frame to frame as the shot is traversed
	  */

	  FrameDescriptor(m_Media,&info);
	  //	  ShotDescriptor( m_Media, &info);
#ifndef SPALOCNUMBER
#define SPALOCNUMBER 1
#endif
	  ShotDescriptorSLMA(m_Media, &info, SPALOCNUMBER); // for SLMA

	  //Setting extracted value
	  m_DescriptorInterface->SetIntensity(info.intensity);
	  m_DescriptorInterface->SetDirectionFlag(info.direction_flag);
	  m_DescriptorInterface->SetSpatialDistributionFlag(info.spatial_distribution_flag);
	  m_DescriptorInterface->
	    SetSpatialLocalizedDistributionFlag(
                 info.spatial_localized_distribution_flag); // for SLMA
	  m_DescriptorInterface->SetTemporalDistributionFlag(info.temporal_distribution_flag);
	  if( info.direction_flag == 1 )
	  {
		  m_DescriptorInterface->SetDominantDirection(info.dominant_direction);
	  }
	  if( info.spatial_distribution_flag == 1 )
	  {
		for( i=0; i<NUM_Nsr_Nmr_Nlr; i++)
		{
			m_DescriptorInterface->SetSpatialParameters(i, info.spatial_parameters[i]);
		}
	  }
	  if(info.spatial_localized_distribution_flag==1){ // for SLMA
		  // KW 2013/07/25 - SetSpaLocNumber was missed
		  m_DescriptorInterface->SetSpaLocNumber(SPALOCNUMBER);
		  /*
		  m_DescriptorInterface->SetSpaLocNumber(info.spa_loc_number);
		  division_block=pow(4.0, (double)(info.spa_loc_number+1.0));
		  */
		  division_block=(int)(pow(4.0, (double)(SPALOCNUMBER+1.0)));
		  for(i=0;i<division_block;i++){
		    m_DescriptorInterface->
		      SetSpatialLocalizedParameter(i,
			info.spatial_localized_parameter[i]);
		  }
	  }
	  if( info.temporal_distribution_flag == 1 )
	  {
		for( i=0; i<NUM_N0_N1_N2_N3_N4; i++)
		{
			m_DescriptorInterface->SetTemporalParameters(i, info.temporal_parameters[i]);
		}
	  }
	}
	return 0;
}

//----------------------------------------------------------------------------
//unsigned long MotionActivityExtractionTool::InitExtracting(void)
//{
//// m_ActivityInfo=activity_initinfo();
// if(m_ActivityInfo==NULL)
//    return 0;
// else
//  return 1;
//}

//------------------------------------------------------------------ for SLMA
unsigned long MotionActivityExtractionTool::PostExtracting(void)
{
	InitializeSLMAdata();
	return 0;
}

//----------------------------------------------------------------------------
const UUID& MotionActivityExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionActivityExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionActivityExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
MotionActivityExtractionInterfaceABC *MotionActivityExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
MotionActivityDescriptorInterfaceABC *MotionActivityExtractionTool::GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
MotionActivityDescriptorInterfaceABC *MotionActivityExtractionInterface::GetDescriptorInterface(void)
{
  return m_ExtractionTool->GetDescriptorInterface();
}
