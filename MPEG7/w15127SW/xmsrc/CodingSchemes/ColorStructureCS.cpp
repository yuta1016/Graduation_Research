////////////////////////////////////////////////////////////////////////
//
// ColorStructureCS.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#include <assert.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID ColorStructureCSInterface::myID = UUID("");
const char *ColorStructureCSInterface::myName =
	"ColorStructureCodingSchemeInterface";

const UUID ColorStructureCS::myID = UUID("");
const char *ColorStructureCS::myName = "ColorStructureCodingScheme";

const UUID ColorStructureCS::valID = UUID("");
const char *ColorStructureCS::valName = "";

const UUID ColorStructureCSInterfaceABC::myID = UUID("");

ColorStructureCSInterfaceABC::ColorStructureCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ColorStructureCSInterface::ColorStructureCSInterface(ColorStructureCS
						     *aColorStructure):
m_ColorStructureCS(aColorStructure)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ColorStructureCSInterface::~ColorStructureCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ColorStructureCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ColorStructureCSInterface::destroy(void)
{
	delete m_ColorStructureCS; 
}

//----------------------------------------------------------------------------
BitBuffer *ColorStructureCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ColorStructureCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorStructureCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorStructureCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ColorStructureCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ColorStructureCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ColorStructureCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ColorStructureCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ColorStructureDescriptorInterfaceABC
*ColorStructureCSInterface::GetDescriptorInterface(void)

{
        return m_ColorStructureCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ColorStructureCSInterface::
SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
						*aColorStructureDescriptorInterface)
{
	return m_ColorStructureCS->
		SetDescriptorInterface(aColorStructureDescriptorInterface);
}

//----------------------------------------------------------------------------
int ColorStructureCSInterface::StartEncode(void)
{
        return m_ColorStructureCS->StartEncode();
}

//----------------------------------------------------------------------------
int ColorStructureCSInterface::StartDecode(void)
{
        return m_ColorStructureCS->StartDecode();
}
//=============================================================================
ColorStructureCS::ColorStructureCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorStructureCS::ColorStructureCS(ColorStructureDescriptorInterfaceABC* ColorStructure):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
	/* create descriptor if it does not exist*/
	if (!ColorStructure) {
		ColorStructureDescriptor *descriptor =
			new ColorStructureDescriptor;
		ColorStructure=descriptor->GetInterface();
	}

	/* connect descritors with CS*/
	SetDescriptorInterface(ColorStructure);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ColorStructureCS::~ColorStructureCS()
{
	/* release descriptor*/
	if (m_DescriptorInterface)
		m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ColorStructureCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ColorStructureCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ColorStructureCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ColorStructureCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ColorStructureCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ColorStructureCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ColorStructureCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
	if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ColorStructureCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ColorStructureCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
	if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ColorStructureDescriptorInterfaceABC *ColorStructureCS::
GetDescriptorInterface(void)
{
	return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int ColorStructureCS::
SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
					 *aColorStructureDescriptorInterface)
{
	/* check if new value is different from old value*/
	if (m_DescriptorInterface == 
			aColorStructureDescriptorInterface) return 0;

	/* release old interface*/
	if (m_DescriptorInterface) 
		m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = 
		aColorStructureDescriptorInterface;
	if (m_DescriptorInterface) {
		m_DescriptorInterface->addref();

	}
	else {
		return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int ColorStructureCS::StartEncode(void)
{
	long v;
	int size;

	// Is the file IO set up? If not return
	if(m_EncoderBuffer==NULL) {
		fprintf(stderr,"ColorStructureCS::StartEncode: Buffer not initialised\n");
		return(-1);
	}

	// Is the interface set up?
	if(m_DescriptorInterface==NULL) {
		fprintf(stderr,"ColorStructureCS::StartEncode: Interface not initialised\n");
		return -1;
	}

	// Put the size (log2)
	size = GetDescriptorInterface()->GetSize();
	switch(size)
	{
	case  32:	v=1; break;
	case  64:	v=2; break;
	case 128:	v=3; break;
	case 256:	v=4; break;
	default:	v=0;
	}
	assert(v);
	m_EncoderBuffer->PutBitsLong(v,3);

	// Put the data
	for (int i = 0; i < size; i++)
	{
		v = GetDescriptorInterface()->GetElement(i);
		m_EncoderBuffer->PutBitsLong(v,8);
	}

	return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int ColorStructureCS::StartDecode(void)
{
	long v = 0;
	int size;

	// Is the file IO set up? If not return
	if(m_DecoderBuffer==NULL) {
		fprintf(stderr,"ColorStructureCS::StartDecode: Buffer not initialised\n");
		return(-1);
	}

	// Is the interface set up?
	if(m_DescriptorInterface==NULL) {
		fprintf(stderr,"ColorStructureCS::StartDecode: Interface not initialised\n");
		return -1;
	}


	// Get size
	m_DecoderBuffer->GetBitsLong(&v,3);
	m_DecoderBuffer->TakeBits(3);
	size = 1 << (v + 4);

	// Set size into descriptor
	GetDescriptorInterface()->SetSize(size);

	// Get data
	for (int i = 0; i < size; i++)
	{
		m_DecoderBuffer->GetBitsLong(&v,8);
		m_DecoderBuffer->TakeBits(8);
		GetDescriptorInterface()->SetElement(i,v);
	}

	return 0;
}

//----------------------------------------------------------------------------
ColorStructureCSInterfaceABC *ColorStructureCS::GetInterface(void)
{ return &m_Interface; }



