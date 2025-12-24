///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// Applicable File Name:  ColorSpaceCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID ColorSpaceCSInterface::myID = UUID("");
const char *ColorSpaceCSInterface::myName =
                         "Color Space Coding Engine Interface";

const UUID ColorSpaceCS::myID = UUID("");
const char *ColorSpaceCS::myName = "Color Space Coding Scheme";

const UUID ColorSpaceCS::valID = UUID("");
const char *ColorSpaceCS::valName = "";

const UUID ColorSpaceCSInterfaceABC::myID = UUID("");

ColorSpaceCSInterfaceABC::ColorSpaceCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
ColorSpaceCSInterface::ColorSpaceCSInterface(ColorSpaceCS *aColorSpace):
m_ColorSpaceCS(aColorSpace)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorSpaceCSInterface::~ColorSpaceCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ColorSpaceCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorSpaceCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ColorSpaceCSInterface::destroy(void)
{ delete m_ColorSpaceCS; }

//----------------------------------------------------------------------------
BitBuffer *ColorSpaceCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ColorSpaceCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorSpaceCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorSpaceCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ColorSpaceCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ColorSpaceCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorSpaceCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorSpaceCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterfaceABC *ColorSpaceCSInterface::
GetDescriptorInterface(void)
{
        return m_ColorSpaceCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ColorSpaceCSInterface::
SetDescriptorInterface(ColorSpaceDescriptorInterfaceABC
		       *aColorSpaceDescriptorInterface)
{
        return m_ColorSpaceCS->
	  SetDescriptorInterface(aColorSpaceDescriptorInterface);
}

//----------------------------------------------------------------------------
int ColorSpaceCSInterface::StartEncode(void)
{
        return m_ColorSpaceCS->StartEncode();
}

//----------------------------------------------------------------------------
int ColorSpaceCSInterface::StartDecode(void)
{
        return m_ColorSpaceCS->StartDecode();
}

//=============================================================================

ColorSpaceCS::ColorSpaceCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorSpaceDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------

ColorSpaceCS::ColorSpaceCS(ColorSpaceDescriptorInterfaceABC *colorspace):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorSpaceDescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!colorspace) {
	  ColorSpaceDescriptor *descriptor =
	    new ColorSpaceDescriptor();
	  colorspace=descriptor->GetInterface();
	}

	/* connect descritor with coding schemes*/
	SetDescriptorInterface(colorspace);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorSpaceCS::~ColorSpaceCS()
{
  if (m_ColorSpaceDescriptorInterface)
    m_ColorSpaceDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ColorSpaceCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorSpaceCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorSpaceCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorSpaceCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorSpaceCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ColorSpaceCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ColorSpaceCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ColorSpaceCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ColorSpaceCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ColorSpaceDescriptorInterfaceABC *ColorSpaceCS::GetDescriptorInterface(void)
{
        return m_ColorSpaceDescriptorInterface;
}

//----------------------------------------------------------------------------
int ColorSpaceCS::
SetDescriptorInterface(ColorSpaceDescriptorInterfaceABC
	      *aColorSpaceDescriptorInterface)
{
        /* check if new value is different from old value*/
        if (m_ColorSpaceDescriptorInterface == 
	    aColorSpaceDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_ColorSpaceDescriptorInterface) 
	  m_ColorSpaceDescriptorInterface->release();

	/* add new interface*/
	m_ColorSpaceDescriptorInterface = 
	  aColorSpaceDescriptorInterface;
	if (m_ColorSpaceDescriptorInterface) {
	  m_ColorSpaceDescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
int ColorSpaceCS::StartEncode(void)
{
        long a;
	int putres;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_ColorSpaceDescriptorInterface) {
	  return -1;
	}

	a=m_ColorSpaceDescriptorInterface->GetColorReferenceFlag();
	putres=m_EncoderBuffer->PutBitsLong(a,1);

	switch (m_ColorSpaceDescriptorInterface->GetDescriptionColorSpace()) {
	case RGB:
	  a=0;
	  putres=m_EncoderBuffer->PutBitsLong(a,4);
	  break;
	case YCRCB:
	  a=1;
	  m_EncoderBuffer->PutBitsLong(a,4);
	  break;
	case HSV:
	  a=2;
	  m_EncoderBuffer->PutBitsLong(a,4);
	  break;
	case HMMD: //added by LGCIT
	  a=3;
	  m_EncoderBuffer->PutBitsLong(a,4);
	  break;
	case LinearMatrix:
	  a=4;
	  m_EncoderBuffer->PutBitsLong(a,4);
	  {
	    for (int yindex=0; yindex<3; yindex++) {
	      for (int xindex=0; xindex<3; xindex++) {
		float value=m_ColorSpaceDescriptorInterface->
		  GetMatrixElement(yindex,xindex);
		if(value<0) {
		  a=1;
		  m_EncoderBuffer->PutBitsLong(a,1);
		  a=(long)(value*-16384.0);
		}
		else {
		  a=0;
		  m_EncoderBuffer->PutBitsLong(a,1);
		  a=(long)(value*16384.0);
		}
		m_EncoderBuffer->PutBitsLong(a,15);
	      }
	    }
	  }
	  break;
	case Monochrome:
	  a=5;
	  putres=m_EncoderBuffer->PutBitsLong(a,4);
	  break;
	default:
	  a=15;
	  m_EncoderBuffer->PutBitsLong(a,4);
	}
	return 0;
}

//----------------------------------------------------------------------------
int ColorSpaceCS::StartDecode(void)
{
        long a;
	float value;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_ColorSpaceDescriptorInterface) {
	  return -1;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_ColorSpaceDescriptorInterface->SetColorReferenceFlag(a);

	m_DecoderBuffer->GetBitsLong(&a,4);
	m_DecoderBuffer->TakeBits(4);
	switch (a) {
	case 0:
	  m_ColorSpaceDescriptorInterface->SetDescriptionColorSpace(RGB);
	  break;
	case 1:
	  m_ColorSpaceDescriptorInterface->SetDescriptionColorSpace(YCRCB);
	  break;
	case 2:
	  m_ColorSpaceDescriptorInterface->SetDescriptionColorSpace(HSV);
	  break;
	case 3:
	  m_ColorSpaceDescriptorInterface->SetDescriptionColorSpace(HMMD);//added by LGCIT
	  break;
	case 4:
	  m_ColorSpaceDescriptorInterface->
	    SetDescriptionColorSpace(LinearMatrix);
	  {
	    for (int yindex=0; yindex<3; yindex++) {
	      for (int xindex=0; xindex<3; xindex++) {
		m_DecoderBuffer->GetBitsLong(&a,1);
		m_DecoderBuffer->TakeBits(1);
		if(a==1) {
		  m_DecoderBuffer->GetBitsLong(&a,15);
		  m_DecoderBuffer->TakeBits(15);
		  value=(float)a/-16384.0;
		}
		else
		  {
		    m_DecoderBuffer->GetBitsLong(&a,15);
		    m_DecoderBuffer->TakeBits(15);
		    value=(float)a/16384.0;
		  }  
		m_ColorSpaceDescriptorInterface->
		  SetMatrixElement(yindex,xindex,value);
}
	    }
	  }
	  break;
	case 5:
	  m_ColorSpaceDescriptorInterface->
	    SetDescriptionColorSpace(Monochrome);
	  break;
	default:
	  fprintf(stderr,"Warning :unknown Color Space, set to RGB\n");
	  m_ColorSpaceDescriptorInterface->SetDescriptionColorSpace(RGB);
	}
	return 0;
}

//----------------------------------------------------------------------------
ColorSpaceCSInterfaceABC *ColorSpaceCS::GetInterface(void)
{ return &m_Interface; }



