///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (coordinates, duration)
// - bugfix in memory management
// - direct float encoding
//
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ParametricObjectMotionCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
#include "SearchUtilities/ParametricObjectMotionUtilities.h"
 

using namespace XM;

const UUID ParametricObjectMotionCSInterface::myID = UUID("");
const char *ParametricObjectMotionCSInterface::myName =
                         "Parametric Object Motion Engine Interface";

const UUID ParametricObjectMotionCS::myID = UUID("");
const char *ParametricObjectMotionCS::myName = "Parametric Object Motion Coding Scheme";

const UUID ParametricObjectMotionCS::valID = UUID("");
const char *ParametricObjectMotionCS::valName = "";

const UUID ParametricObjectMotionCSInterfaceABC::myID = UUID("");

ParametricObjectMotionCSInterfaceABC::ParametricObjectMotionCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ParametricObjectMotionCSInterface::ParametricObjectMotionCSInterface(ParametricObjectMotionCS
						     *aParametricObjectMotion):
m_ParametricObjectMotionCS(aParametricObjectMotion)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ParametricObjectMotionCSInterface::~ParametricObjectMotionCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ParametricObjectMotionCSInterface::destroy(void)
{ delete m_ParametricObjectMotionCS; }

//----------------------------------------------------------------------------
BitBuffer *ParametricObjectMotionCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ParametricObjectMotionCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ParametricObjectMotionCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ParametricObjectMotionCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ParametricObjectMotionCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ParametricObjectMotionCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC
*ParametricObjectMotionCSInterface::GetDescriptorInterface(void)

{
        return m_ParametricObjectMotionCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCSInterface::
SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
		       *aParametricObjectMotionDescriptorInterface)
{
        return m_ParametricObjectMotionCS->
	  SetDescriptorInterface(aParametricObjectMotionDescriptorInterface);
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCSInterface::StartEncode(void)
{
        return m_ParametricObjectMotionCS->StartEncode();
}
//----------------------------------------------------------------------------
int ParametricObjectMotionCSInterface::StartDecode(void)
{
        return m_ParametricObjectMotionCS->StartDecode();
}
//=============================================================================

ParametricObjectMotionCS::ParametricObjectMotionCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================

ParametricObjectMotionCS::
ParametricObjectMotionCS(ParametricObjectMotionDescriptorInterfaceABC 
			 *aDescriptor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)

{
	/* create descriptor if it does not exist*/
	if (!aDescriptor) {
	  ParametricObjectMotionDescriptor *descriptor =
	    new ParametricObjectMotionDescriptor();
	  aDescriptor=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(aDescriptor);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ParametricObjectMotionCS::~ParametricObjectMotionCS()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ParametricObjectMotionCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ParametricObjectMotionCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ParametricObjectMotionCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ParametricObjectMotionCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ParametricObjectMotionCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ParametricObjectMotionCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ParametricObjectMotionDescriptorInterfaceABC *ParametricObjectMotionCS::
GetDescriptorInterface(void)
{
       return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCS::
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
	m_DescriptorInterface = aDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}



//----------------------------------------------------------------------------
int ParametricObjectMotionCS::StartEncode(void)
{
  int mCode,i;
  float fl;			// Aljoscha 12/13/00
  int Duration,noofpar;		// Aljoscha 12/13/00
  unsigned long CoordFlag, CoordRef, SpatialReference;	// Aljoscha 12/13/00
  double *param,xorg,yorg;


  if (!m_EncoderBuffer) {
    return -1;
  }
  
  /* check if descriptor and stream buffer are linked
     to coding scheme*/
  if (!m_EncoderBuffer || !m_DescriptorInterface) {
    return -1;
  }

  mCode=m_DescriptorInterface->GetModelCode();
  m_EncoderBuffer->PutBitsLong(mCode,3);

  CoordFlag=m_DescriptorInterface->GetCoordFlag();     // Aljoscha 12/13/00 start
  m_EncoderBuffer->PutBitsLong(CoordFlag,1UL);

  if ( CoordFlag )
    {
      CoordRef = m_DescriptorInterface->GetCoordRef();
      m_EncoderBuffer->PutBitsLong(CoordRef,8UL);

      SpatialReference = m_DescriptorInterface->GetSpatialReference();
      m_EncoderBuffer->PutBitsLong(SpatialReference,1UL);
    }
  else
    {
      xorg=m_DescriptorInterface->GetXOrigin();
      yorg=m_DescriptorInterface->GetYOrigin();
      fl=(float)xorg;
      m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));
      fl=(float)yorg;
      m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));  
    }

  Duration=m_DescriptorInterface->GetDuration();
  m_EncoderBuffer->PutBitsLong(Duration,16);				  // Aljoscha 12/13/00 end

  switch (mCode){
  case Translational:noofpar=2;;break;
  case Rotation_Scale:noofpar=4;;break;
  case Affine:noofpar=6;;break;
  case Perspective:noofpar=8;;break;
  case Quadric:noofpar=12;;break;
  default:noofpar=0;
  }
  // Aljoscha 12/13/00
  param=m_DescriptorInterface->GetMotionParameters();	// malloc removed
  // pointing to nowhere
  for (i=0;i<noofpar;i++){
    fl = (float)param[i];			// Aljoscha 12/13/00
    m_EncoderBuffer->PutBits((char *)(&fl),8*sizeof(float));// Aljoscha 12/13/00
  }

  return 0;
}

//----------------------------------------------------------------------------
int ParametricObjectMotionCS::StartDecode(void)
{
  int noofpar,i;
  long a;
  float fl;	//sth schon mal gedruckt?					// Aljoscha 12/13/00


  if (!m_DecoderBuffer) {
    return -1;
  }
  
  /* check if descriptor and stream buffer are linked
     to coding scheme*/
  if (!m_DecoderBuffer || !m_DescriptorInterface) {
    return -1;
  }

  m_DecoderBuffer->GetBitsLong((&a),3);
  m_DecoderBuffer->TakeBits(3);
  m_DescriptorInterface->SetModelCode((int)a);

  switch ((int)a){
  case Translational:noofpar=2;break;
  case Rotation_Scale:noofpar=4;break;
  case Affine:noofpar=6;break;
  case Perspective:noofpar=8;break;
  case Quadric:noofpar=12;break;
  default:noofpar=0;
  }

  m_DecoderBuffer->GetBitsLong(&a,1UL);								// Aljoscha 12/13/00 start
  m_DecoderBuffer->TakeBits(1UL);
  m_DescriptorInterface->SetCoordFlag(a);

	if ( a ) // CoordFlag is set to 1
		{
		  m_DecoderBuffer->GetBitsLong(&a,8UL);
			m_DecoderBuffer->TakeBits(8UL);
			m_DescriptorInterface->SetCoordRef(a);

			m_DecoderBuffer->GetBitsLong(&a,1UL);
		  m_DecoderBuffer->TakeBits(1UL);
			m_DescriptorInterface->SetSpatialReference(a);
		}
	else
		{
			fl = 0.0;
		  m_DecoderBuffer->GetBits((char *)(&fl),8*sizeof(float));
			m_DecoderBuffer->TakeBits(8*sizeof(float));
			m_DescriptorInterface->SetXOrigin((double)fl);

			fl = 0.0;
			m_DecoderBuffer->GetBits((char *)(&fl),8*sizeof(float));
			m_DecoderBuffer->TakeBits(8*sizeof(float));
			m_DescriptorInterface->SetYOrigin((double)fl);
		}

  m_DecoderBuffer->GetBitsLong((&a),16);
  m_DecoderBuffer->TakeBits(16);
  m_DescriptorInterface->SetDuration(a);

  for (i=0;i<noofpar;i++){
		fl = 0.0;
    m_DecoderBuffer->GetBits((char *)(&fl),8*sizeof(float));
    m_DecoderBuffer->TakeBits(8*sizeof(float));
    m_DescriptorInterface->SetMotionParameterValue(i,(double)fl);
  }		//sth not good for printers :-(																								// Aljoscha 12/13/00 end

  return 0;
}

//----------------------------------------------------------------------------
ParametricObjectMotionCSInterfaceABC *ParametricObjectMotionCS::GetInterface(void)
{ return &m_Interface; }



