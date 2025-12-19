///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
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
// Applicable File Name:  RegionShapeCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif


using namespace XM;

const UUID RegionShapeCSInterface::myID = UUID("");
const char *RegionShapeCSInterface::myName =
                         "Region Shape Coding Engine Interface";

const UUID RegionShapeCS::myID = UUID("");
const char *RegionShapeCS::myName = "Region Shape Coding Scheme";

const UUID RegionShapeCS::valID = UUID("");
const char *RegionShapeCS::valName = "";

const UUID RegionShapeCSInterfaceABC::myID = UUID("");

//=============================================================================
RegionShapeCSInterfaceABC::RegionShapeCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
RegionShapeCSInterface::RegionShapeCSInterface(RegionShapeCS *aRegionShape):
m_RegionShapeCS(aRegionShape)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RegionShapeCSInterface::~RegionShapeCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& RegionShapeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void RegionShapeCSInterface::destroy(void)
{ delete m_RegionShapeCS; }

//----------------------------------------------------------------------------
BitBuffer *RegionShapeCSInterface::GetEncoderStreamBuffer(void)
{
        return m_RegionShapeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RegionShapeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RegionShapeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *RegionShapeCSInterface::GetDecoderStreamBuffer(void)
{
        return m_RegionShapeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RegionShapeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RegionShapeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
RegionShapeDescriptorInterfaceABC *RegionShapeCSInterface::
GetDescriptorInterface(void)
{
        return m_RegionShapeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RegionShapeCSInterface::
SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
		       *aRegionShapeDescriptorInterface)
{
        return m_RegionShapeCS->
	  SetDescriptorInterface(aRegionShapeDescriptorInterface);
}

//----------------------------------------------------------------------------
int RegionShapeCSInterface::StartEncode(void)
{
        return m_RegionShapeCS->StartEncode();
}

//----------------------------------------------------------------------------
int RegionShapeCSInterface::StartDecode(void)
{
        return m_RegionShapeCS->StartDecode();
}

//=============================================================================

RegionShapeCS::RegionShapeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_RegionShapeDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeCS::RegionShapeCS(RegionShapeDescriptorInterfaceABC *RegionShape):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_RegionShapeDescriptorInterface(0)
{
	if (!RegionShape) {
	  RegionShapeDescriptor *descriptor = new RegionShapeDescriptor();
	  RegionShape = descriptor->GetInterface();
	}
	
	SetDescriptorInterface(RegionShape);
	
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RegionShapeCS::~RegionShapeCS()
{
	if (m_RegionShapeDescriptorInterface)
		m_RegionShapeDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& RegionShapeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RegionShapeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RegionShapeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RegionShapeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool RegionShapeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * RegionShapeCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int RegionShapeCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * RegionShapeCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int RegionShapeCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
RegionShapeDescriptorInterfaceABC *RegionShapeCS::GetDescriptorInterface()
{
        return m_RegionShapeDescriptorInterface;
}

//----------------------------------------------------------------------------
int RegionShapeCS::
SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
	      *aRegionShapeDescriptorInterface)
{
	if (m_RegionShapeDescriptorInterface ==
	    aRegionShapeDescriptorInterface)
	  return 0;

	if (m_RegionShapeDescriptorInterface)
	  m_RegionShapeDescriptorInterface->release();

	m_RegionShapeDescriptorInterface = aRegionShapeDescriptorInterface;

	if (m_RegionShapeDescriptorInterface)
	  m_RegionShapeDescriptorInterface->addref();
	else
	  return -1;

	return 0;
}

//----------------------------------------------------------------------------
int RegionShapeCS::StartEncode(void)
{
	/* check if Descriptor and stream buffers are linked to coding scheme*/
	if (!m_EncoderBuffer || !m_RegionShapeDescriptorInterface) {
	  return -1;
	}

	char i, j;
	long value;
	
	for(i=0 ; i<ART_ANGULAR ; i++) 
	for(j=0 ; j<ART_RADIAL ; j++)
	if(i!=0 || j!= 0) 
	{
		value = m_RegionShapeDescriptorInterface->GetElement(i, j);
		m_EncoderBuffer->PutBitsLong(value, 4UL);
	}

	return 0;
}

//----------------------------------------------------------------------------
int RegionShapeCS::StartDecode(void)
{
	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_RegionShapeDescriptorInterface) {
	  return -1;
	}

	char i, j;
	long value;

	for(i=0 ; i<ART_ANGULAR ; i++) 
	for(j=0 ; j<ART_RADIAL ; j++)
	if(i!=0 || j!= 0)
	{
		m_DecoderBuffer->GetBitsLong(&value, 4UL);
		m_DecoderBuffer->TakeBits(4UL);
		
		m_RegionShapeDescriptorInterface->
		  SetElement((char)i, (char)j, (char)value);
	}
	
// 	m_DecoderBuffer->GetBitsLong(&value, 1UL);
// 	m_DecoderBuffer->TakeBits(1UL);

	return 0;
}

//----------------------------------------------------------------------------
RegionShapeCSInterfaceABC *RegionShapeCS::GetInterface(void)
{ return &m_Interface; }
