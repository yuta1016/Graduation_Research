///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens Corp., J. Heuer
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
// Applicable File Name:  ColorHistCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "TimeUnitCS.h"
#include "IncrDurationCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 

using namespace XM;

const UUID IncrDurationCSInterface::myID = UUID("");
const char *IncrDurationCSInterface::myName =
                         "IncrDuration Coding Engine Interface";

const UUID IncrDurationCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *IncrDurationCS::myName = "IncrDuration Coding Scheme";

const UUID IncrDurationCS::valID = UUID("");
const char *IncrDurationCS::valName = "";

const UUID IncrDurationCSInterfaceABC::myID = UUID("");

IncrDurationCSInterfaceABC::IncrDurationCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
IncrDurationCSInterface::IncrDurationCSInterface(IncrDurationCS
						     *aIncrDuration):
m_IncrDurationCS(aIncrDuration)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
IncrDurationCSInterface::~IncrDurationCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& IncrDurationCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IncrDurationCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *IncrDurationCSInterface::GetEncoderStreamBuffer(void)
{
        return m_IncrDurationCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int IncrDurationCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_IncrDurationCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *IncrDurationCSInterface::GetDecoderStreamBuffer(void)
{
        return m_IncrDurationCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
bint IncrDurationCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_IncrDurationCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
IncrDurationDescriptorInterfaceABC
*IncrDurationCSInterface::GetDescriptorInterface(void)

{
        return m_IncrDurationCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int IncrDurationCSInterface::
SetDescriptorInterface(IncrDurationDescriptorInterfaceABC
		       *aIncrDurationDescriptorInterface)
{
        return m_IncrDurationCS->
	  SetDescriptorInterface(aIncrDurationDescriptorInterface);
}

//----------------------------------------------------------------------------
TimeUnitCSInterfaceABC *IncrDurationCSInterface::
GetTimeUnitCSInterface(void)

{
        return m_IncrDurationCS->GetTimeUnitCSInterface();
}

//----------------------------------------------------------------------------
int IncrDurationCSInterface::
SetTimeUnitCSInterface(TimeUnitCSInterfaceABC *aTimeUnitCSInterface)

{
        return m_IncrDurationCS->
	  SetTimeUnitCSInterface(aTimeUnitCSInterface);
}


//----------------------------------------------------------------------------
int IncrDurationCSInterface::StartEncode(void)
{
        return m_IncrDurationCS->StartEncode();
}
//----------------------------------------------------------------------------
int IncrDurationCSInterface::StartDecode(void)
{
        return m_IncrDurationCS->StartDecode();
}
//=============================================================================

IncrDurationCS::IncrDurationCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_IncrDurationDescriptorInterface(0),
m_TimeUnitCSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IncrDurationCS::~IncrDurationCS()
{

}

//----------------------------------------------------------------------------
const UUID& IncrDurationCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IncrDurationCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& IncrDurationCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* IncrDurationCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool IncrDurationCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * IncrDurationCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int IncrDurationCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * IncrDurationCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int IncrDurationCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
IncrDurationDescriptorInterfaceABC *IncrDurationCS::
GetDescriptorInterface(void)
{
       return m_IncrDurationDescriptorInterface;
}

//----------------------------------------------------------------------------
int IncrDurationCS::
SetDescriptorInterface(IncrDurationDescriptorInterfaceABC
		       *aIncrDurationDescriptorInterface)
{
        if (!aIncrDurationDescriptorInterface) return -1;

        m_IncrDurationDescriptorInterface =
	  aIncrDurationDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
TimeUnitCSInterfaceABC *IncrDurationCS::GetTimeUnitCSInterface(void)

{
        return m_TimeUnitCSInterface;
}

//----------------------------------------------------------------------------
int IncrDurationCS::SetTimeUnitCSInterface(TimeUnitCSInterfaceABC
						*aTimeUnitCSInterface)

{
        if (!aTimeUnitCSInterface) return -1;

        m_TimeUnitCSInterface = aTimeUnitCSInterface;
	return 0;
}


//----------------------------------------------------------------------------
int IncrDurationCS::StartEncode(void)
{
	long a;

        if (!m_EncoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_IncrDurationDescriptorInterface) {
	  return -1;
	}

	/* check values for TimeUnit **************************/ 
	/* check if TimeUnit AttrGroup is linked to histogram*/
	if (!m_IncrDurationDescriptorInterface->
	    GetTimeUnitInterface()) {
	  return -1;
	}

	/* check if TimeUnit AttrGroup and stream buffer are linked
	   to TimeUnit AttrGroup coding scheme*/
	if (!m_TimeUnitCSInterface->GetAttrGroupInterface() ||
	    !m_TimeUnitCSInterface->GetEncoderStreamBuffer()) return -1;

	/* check if TimeUnit AttrGroup attached to
	   IncrDuration descriptor
	   is equal to TimeUnit AttrGroup attached to
	   IncrDuration coding */
	if (m_TimeUnitCSInterface->GetAttrGroupInterface() !=
	    m_IncrDurationDescriptorInterface->
	    GetTimeUnitInterface()) return -1;


	/* TimeUnit AttrGroup*/
	m_TimeUnitCSInterface->StartEncode();

	a=m_IncrDurationDescriptorInterface->GetIncrDuration();

	m_EncoderBuffer->PutBitsLong(a,32);


	return 0;
}

//----------------------------------------------------------------------------
int IncrDurationCS::StartDecode(void)
{
	long a;
	
	if (!m_DecoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_IncrDurationDescriptorInterface) {
	  return -1;
	}

	/* check values for TimeUnit **************************/ 
	/* check if TimeUnit AttrGroup is linked to histogram*/
	if (!m_IncrDurationDescriptorInterface->
	    GetTimeUnitInterface()) {
	  return -1;
	}

	/* check if TimeUnit AttrGroup and stream buffer are linked
	   to TimeUnit AttrGroup coding scheme*/
	if (!m_TimeUnitCSInterface->GetAttrGroupInterface() ||
	    !m_TimeUnitCSInterface->GetDecoderStreamBuffer()) return -1;

	/* check if TimeUnit AttrGroup attached to
	   IncrDuration descriptor
	   is equal to TimeUnit AttrGroup attached to
	   IncrDuration coding */
	if (m_TimeUnitCSInterface->GetAttrGroupInterface() !=
	    m_IncrDurationDescriptorInterface->
	    GetTimeUnitInterface()) return -1;


	/* decode TimeUnit AttrGroup description*/
	m_TimeUnitCSInterface->StartDecode();

	m_DecoderBuffer->GetBitsLong(&a,32);
	m_DecoderBuffer->TakeBits(32);
	m_IncrDurationDescriptorInterface->SetIncrDuration(a);

	return 0;
}

//----------------------------------------------------------------------------
IncrDurationCSInterfaceABC *IncrDurationCS::GetInterface(void)
{ return &m_Interface; }



