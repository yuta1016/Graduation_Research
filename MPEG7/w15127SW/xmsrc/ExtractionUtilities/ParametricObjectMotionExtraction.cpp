//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// (contributing organizations names)
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
// Applicable File Name:  ParametricObjectMotionExtraction.cpp
//

#include <stdio.h>
#include "momusys.h"
#include "ExtractionUtilities/gme_for_iso.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "SearchUtilities/ParametricObjectMotionUtilities.h"

using namespace XM;

const UUID ParametricObjectMotionExtractionInterface::myID = UUID("8379c090-982c-11d3-922e-004005513c7a");
const char *ParametricObjectMotionExtractionInterface::myName = "Parametric Object Motion Extraction Interface";

const UUID ParametricObjectMotionExtractionTool::myID = UUID("8565caf0-982c-11d3-922e-004005513c7a");
const char *ParametricObjectMotionExtractionTool::myName = "Parametric Object Motion Extraction Descriptor Extractor";

const UUID ParametricObjectMotionExtractionInterfaceABC::myID = UUID("7fda35c0-982c-11d3-922e-004005513c7a");

//=============================================================================
ParametricObjectMotionExtractionInterfaceABC::ParametricObjectMotionExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
ParametricObjectMotionExtractionInterface::ParametricObjectMotionExtractionInterface(ParametricObjectMotionExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionInterface::~ParametricObjectMotionExtractionInterface()
{
}

//----------------------------------------------------------------------------
void ParametricObjectMotionExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface::
SetSourceMedia(MomVop *ReferenceVOP, MomVop *CurrentVOP)
{
  return m_ExtractionTool->SetSourceMedia(ReferenceVOP, CurrentVOP);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC 
*ParametricObjectMotionExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionInterface::
SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *aParametricObjectMotionDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aParametricObjectMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
unsigned long ParametricObjectMotionExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
ParametricObjectMotionExtractionTool::ParametricObjectMotionExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_ReferenceVOP(0),
m_CurrentVOP(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionTool::
ParametricObjectMotionExtractionTool(
			     ParametricObjectMotionDescriptorInterfaceABC
			     *aDescriptorInterface):
m_Interface(this),
m_DescriptorInterface(0),
m_ReferenceVOP(0),
m_CurrentVOP(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aDescriptorInterface) {
	  ParametricObjectMotionDescriptor *descriptor =
	    new ParametricObjectMotionDescriptor();
	  aDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(aDescriptorInterface);

	/* create sub extraction tools and
	   connect them with descriptor if availabe*/
	/* no sub extraction tools (see how ColorHistCS if needed)*/

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionTool::~ParametricObjectMotionExtractionTool()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool ParametricObjectMotionExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool ParametricObjectMotionExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool::SetSourceMedia(MomVop *ReferenceVOP, MomVop *CurrentVOP)
{
  if ((!ReferenceVOP)||(!CurrentVOP)) return -1;

  m_ReferenceVOP=ReferenceVOP;
  m_CurrentVOP=CurrentVOP;
  return 0;
}


//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC*
ParametricObjectMotionExtractionTool::
GetDescriptorInterface(void)

{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionExtractionTool::
SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *aDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_DescriptorInterface == 
	    aDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
	  aDescriptorInterface;
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
unsigned long ParametricObjectMotionExtractionTool::StartExtracting(void)

{
  Int i;
  Int motion_model;
#ifdef USEXML3
  double parameter[12];
#else
  Double parameter[12];
#endif // USEXML3

  /* check for descriptor*/
  if (!m_DescriptorInterface) return (unsigned long)-1;
 
  if (strcmp(m_DescriptorInterface->GetName(),
	     "Parametric Object Motion Description Interface") != 0) {
    return (unsigned long)-1;
  }
 
  /* check for media*/
  if ((!m_ReferenceVOP)||(!m_CurrentVOP)) {
    return (unsigned long)-1;
  }
  motion_model = m_DescriptorInterface->GetModelCode();
 
  GlobalMotionEstimation(m_ReferenceVOP, m_CurrentVOP , motion_model, parameter);
  

 if ( motion_model == 4 )
   motion_model = 12;
 else
   motion_model = 2*(motion_model+1);
 
 for (i=0;i<motion_model;i++)
   m_DescriptorInterface->SetMotionParameterValue( i, parameter[i] );

//  GetXYCenter(m_ReferenceVOP,&x,&y);
// the x and y shoul be the origin of the system x=0,y=0 in this case

  m_DescriptorInterface->SetXOrigin(0.0);
  m_DescriptorInterface->SetYOrigin(0.0);


 return 0;
}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
ParametricObjectMotionExtractionInterfaceABC *ParametricObjectMotionExtractionTool::GetInterface(void)
{ return &m_Interface; }

