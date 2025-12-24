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
// Applicable File Name:  ColorQuantCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "Utilities/readparam.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID ColorQuantizerCSInterface::myID = UUID("");
const char *ColorQuantizerCSInterface::myName =
                         "Color Quantizer Coding Engine Interface";

const UUID ColorQuantizerCS::myID = UUID("");
const char *ColorQuantizerCS::myName = "Color Quantizer Coding Scheme";

const UUID ColorQuantizerCS::valID = UUID("");
const char *ColorQuantizerCS::valName = "";

const UUID ColorQuantizerCSInterfaceABC::myID = UUID("");

ColorQuantizerCSInterfaceABC::ColorQuantizerCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
ColorQuantizerCSInterface::
ColorQuantizerCSInterface(ColorQuantizerCS *aColorQuantizer):
m_ColorQuantizerCS(aColorQuantizer)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorQuantizerCSInterface::~ColorQuantizerCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& ColorQuantizerCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorQuantizerCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ColorQuantizerCSInterface::destroy(void)
{ delete m_ColorQuantizerCS; }

//----------------------------------------------------------------------------
BitBuffer *ColorQuantizerCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ColorQuantizerCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorQuantizerCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorQuantizerCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ColorQuantizerCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ColorQuantizerCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorQuantizerCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorQuantizerCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ColorQuantizerDescriptorInterfaceABC *ColorQuantizerCSInterface::
GetDescriptorInterface(void)
{
        return m_ColorQuantizerCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ColorQuantizerCSInterface::
SetDescriptorInterface(ColorQuantizerDescriptorInterfaceABC
		       *aColorQuantizerDescriptorInterface)
{
        return m_ColorQuantizerCS->
	  SetDescriptorInterface(aColorQuantizerDescriptorInterface);
}

//----------------------------------------------------------------------------
int ColorQuantizerCSInterface::StartEncode(void)
{
        return m_ColorQuantizerCS->StartEncode();
}

//----------------------------------------------------------------------------
int ColorQuantizerCSInterface::StartDecode(void)
{
        return m_ColorQuantizerCS->StartDecode();
}

//=============================================================================

ColorQuantizerCS::ColorQuantizerCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorQuantizerDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------

ColorQuantizerCS::
ColorQuantizerCS(ColorQuantizerDescriptorInterfaceABC *colorquantizer):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ColorQuantizerDescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!colorquantizer) {
	  ColorQuantizerDescriptor *descriptor =
	    new ColorQuantizerDescriptor();
	  colorquantizer=descriptor->GetInterface();
	}

	/* connect descritor with coding schemes*/
	SetDescriptorInterface(colorquantizer);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorQuantizerCS::~ColorQuantizerCS()
{
  if (m_ColorQuantizerDescriptorInterface)
    m_ColorQuantizerDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& ColorQuantizerCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorQuantizerCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorQuantizerCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorQuantizerCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorQuantizerCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ColorQuantizerCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ColorQuantizerCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ColorQuantizerCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ColorQuantizerCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ColorQuantizerDescriptorInterfaceABC *ColorQuantizerCS::
GetDescriptorInterface(void)
{
        return m_ColorQuantizerDescriptorInterface;
}

//----------------------------------------------------------------------------
int ColorQuantizerCS::
SetDescriptorInterface(ColorQuantizerDescriptorInterfaceABC
	      *aColorQuantizerDescriptorInterface)
{
        /* check if new value is different from old value*/
        if (m_ColorQuantizerDescriptorInterface == 
	    aColorQuantizerDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_ColorQuantizerDescriptorInterface) 
	  m_ColorQuantizerDescriptorInterface->release();

	/* add new interface*/
	m_ColorQuantizerDescriptorInterface = 
	  aColorQuantizerDescriptorInterface;
	if (m_ColorQuantizerDescriptorInterface) {
	  m_ColorQuantizerDescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
int ColorQuantizerCS::StartEncode(void)
{
        int i;
	long  a;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_ColorQuantizerDescriptorInterface) {
	  return -1;
	}

	int NumComponents = 3;
	int ColorSpace = atoi(getpara("ColorSpace","0"));
	if(ColorSpace==5) NumComponents = 1; // monochrome

	for(i=0; i<NumComponents; i++)
	{
		a=m_ColorQuantizerDescriptorInterface->GetComponent(i);
		m_EncoderBuffer->PutBitsLong(a,5);
		a=m_ColorQuantizerDescriptorInterface->GetBinNumber(i);
		m_EncoderBuffer->PutBitsLong(a,12);
	}

	return 0;
}

//----------------------------------------------------------------------------
int ColorQuantizerCS::StartDecode(void)
{
	int i;
	long  a;

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_ColorQuantizerDescriptorInterface) {
	  return -1;
	}

	int NumComponents = 3;
	int ColorSpace = atoi(getpara("ColorSpace","0"));
	if(ColorSpace==5) NumComponents = 1; // monochrome

	for(i=0; i<NumComponents; i++)
	{
		m_DecoderBuffer->GetBitsLong(&a,5);
		m_DecoderBuffer->TakeBits(5);
		m_ColorQuantizerDescriptorInterface->SetComponent(i,a);
		m_DecoderBuffer->GetBitsLong(&a,12);
		m_DecoderBuffer->TakeBits(12);
		m_ColorQuantizerDescriptorInterface->SetBinNumber(i,a);
	}

	return 0;
}

//----------------------------------------------------------------------------
ColorQuantizerCSInterfaceABC *ColorQuantizerCS::GetInterface(void)
{ return &m_Interface; }
