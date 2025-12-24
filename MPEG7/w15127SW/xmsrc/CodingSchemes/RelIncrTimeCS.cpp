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
// Applicable File Name:  RelIncrTimeCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "TimeUnitCS.h"
#include "TimePointCS.h"
#include "RelIncrTimeCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 
 

using namespace XM;

const UUID RelIncrTimeCSInterface::myID = UUID("");
const char *RelIncrTimeCSInterface::myName =
                         "RelIncrTime Coding Engine Interface";

const UUID RelIncrTimeCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *RelIncrTimeCS::myName = "RelIncrTime Coding Scheme";

const UUID RelIncrTimeCS::valID = UUID("");
const char *RelIncrTimeCS::valName = "";

const UUID RelIncrTimeCSInterfaceABC::myID = UUID("");

RelIncrTimeCSInterfaceABC::RelIncrTimeCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
RelIncrTimeCSInterface::RelIncrTimeCSInterface(RelIncrTimeCS
						     *aRelIncrTime):
m_RelIncrTimeCS(aRelIncrTime)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RelIncrTimeCSInterface::~RelIncrTimeCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& RelIncrTimeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelIncrTimeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *RelIncrTimeCSInterface::GetEncoderStreamBuffer(void)
{
        return m_RelIncrTimeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RelIncrTimeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *RelIncrTimeCSInterface::GetDecoderStreamBuffer(void)
{
        return m_RelIncrTimeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RelIncrTimeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
RelIncrTimeDescriptorInterfaceABC
*RelIncrTimeCSInterface::GetDescriptorInterface(void)

{
        return m_RelIncrTimeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::
SetDescriptorInterface(RelIncrTimeDescriptorInterfaceABC
		       *aRelIncrTimeDescriptorInterface)
{
        return m_RelIncrTimeCS->
	  SetDescriptorInterface(aRelIncrTimeDescriptorInterface);
}

//----------------------------------------------------------------------------
TimeUnitCSInterfaceABC *RelIncrTimeCSInterface::
GetTimeUnitCSInterface(void)

{
        return m_RelIncrTimeCS->GetTimeUnitCSInterface();
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::
SetTimeUnitCSInterface(TimeUnitCSInterfaceABC *aTimeUnitCSInterface)

{
        return m_RelIncrTimeCS->
	  SetTimeUnitCSInterface(aTimeUnitCSInterface);
}

//----------------------------------------------------------------------------
TimePointCSInterfaceABC
*RelIncrTimeCSInterface::GetTimeBaseCSInterface(void)

{
        return m_RelIncrTimeCS->GetTimeBaseCSInterface();
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::
SetTimeBaseCSInterface(TimePointCSInterfaceABC
		       *aTimeBaseDescriptorInterface)
{
        return m_RelIncrTimeCS->
	  SetTimeBaseCSInterface(aTimeBaseDescriptorInterface);
}

//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::StartEncode(void)
{
        return m_RelIncrTimeCS->StartEncode();
}
//----------------------------------------------------------------------------
int RelIncrTimeCSInterface::StartDecode(void)
{
        return m_RelIncrTimeCS->StartDecode();
}
//=============================================================================

RelIncrTimeCS::RelIncrTimeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_RelIncrTimeDescriptorInterface(0),
m_TimeUnitCSInterface(0),
m_TimeBaseCSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RelIncrTimeCS::~RelIncrTimeCS()
{

}

//----------------------------------------------------------------------------
const UUID& RelIncrTimeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelIncrTimeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RelIncrTimeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RelIncrTimeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool RelIncrTimeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * RelIncrTimeCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * RelIncrTimeCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
RelIncrTimeDescriptorInterfaceABC *RelIncrTimeCS::
GetDescriptorInterface(void)
{
       return m_RelIncrTimeDescriptorInterface;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::
SetDescriptorInterface(RelIncrTimeDescriptorInterfaceABC
		       *aRelIncrTimeDescriptorInterface)
{
        if (!aRelIncrTimeDescriptorInterface) return -1;

        m_RelIncrTimeDescriptorInterface =
	  aRelIncrTimeDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
TimeUnitCSInterfaceABC *RelIncrTimeCS::GetTimeUnitCSInterface(void)

{
        return m_TimeUnitCSInterface;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::SetTimeUnitCSInterface(TimeUnitCSInterfaceABC
						*aTimeUnitCSInterface)

{
        if (!aTimeUnitCSInterface) return -1;

        m_TimeUnitCSInterface = aTimeUnitCSInterface;
	return 0;
}

//----------------------------------------------------------------------------
TimePointCSInterfaceABC *RelIncrTimeCS::GetTimeBaseCSInterface(void)

{
        return m_TimeBaseCSInterface;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::SetTimeBaseCSInterface(TimePointCSInterfaceABC
						*aTimeBaseCSInterface)

{
        if (!aTimeBaseCSInterface) return -1;

        m_TimeBaseCSInterface = aTimeBaseCSInterface;
	return 0;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::StartEncode(void)
{
	long a,f_tb=1;

        if (!m_EncoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_RelIncrTimeDescriptorInterface) {
	  return -1;
	}

	/* check values for TimeUnit **************************/ 
	/* check if TimeUnit AttrGroup is linked to histogram*/
	if (!m_RelIncrTimeDescriptorInterface->
	    GetTimeUnitInterface()) {
	  return -1;
	}

	/* check if TimeUnit AttrGroup and stream buffer are linked
	   to TimeUnit AttrGroup coding scheme*/
	if (!m_TimeUnitCSInterface->GetAttrGroupInterface() ||
	    !m_TimeUnitCSInterface->GetEncoderStreamBuffer()) return -1;

	/* check if TimeUnit AttrGroup attached to
	   RelIncrTime descriptor
	   is equal to TimeUnit AttrGroup attached to
	   RelIncrTime coding */
	if (m_TimeUnitCSInterface->GetAttrGroupInterface() !=
	    m_RelIncrTimeDescriptorInterface->
	    GetTimeUnitInterface()) return -1;

	/* TimeUnit AttrGroup*/
	m_TimeUnitCSInterface->StartEncode();

	/* check values for TimeBase **************************/ 
	/* check if TimeBase is linked to histogram*/
	if (!m_RelIncrTimeDescriptorInterface->
	    GetTimeBaseInterface()) {
	    f_tb=0;
	    m_EncoderBuffer->PutBitsLong(f_tb,1);
	}
	else {
	  /* check if TimeBase AttrGroup and stream buffer are linked
	     to TimeBase coding scheme*/
	  if (!m_TimeBaseCSInterface->GetDescriptorInterface() ||
	      !m_TimeBaseCSInterface->GetEncoderStreamBuffer()) return -1;
	  
	  /* check if TimeBase attached to
	     RelIncrTime descriptor
	     is equal to TimeBase attached to
	     RelIncrTime coding */
	  if (m_TimeBaseCSInterface->GetDescriptorInterface() !=
	      m_RelIncrTimeDescriptorInterface->
	      GetTimeBaseInterface()) return -1;
	  

	  /* TimeBase Reference*/
	  m_TimeBaseCSInterface->StartEncode();
	}

	a=m_RelIncrTimeDescriptorInterface->GetRelIncrTime();

	m_EncoderBuffer->PutBitsLong(a,32);


	return 0;
}

//----------------------------------------------------------------------------
int RelIncrTimeCS::StartDecode(void)
{
	long a;
	
	if (!m_DecoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_RelIncrTimeDescriptorInterface) {
	  return -1;
	}

	/* check values for TimeUnit **************************/ 
	/* check if TimeUnit AttrGroup is linked to histogram*/
	if (!m_RelIncrTimeDescriptorInterface->
	    GetTimeUnitInterface()) {
	  return -1;
	}

	/* check if TimeUnit AttrGroup and stream buffer are linked
	   to TimeUnit AttrGroup coding scheme*/
	if (!m_TimeUnitCSInterface->GetAttrGroupInterface() ||
	    !m_TimeUnitCSInterface->GetDecoderStreamBuffer()) return -1;

	/* check if TimeUnit AttrGroup attached to
	   RelIncrTime descriptor
	   is equal to TimeUnit AttrGroup attached to
	   RelIncrTime coding */
	if (m_TimeUnitCSInterface->GetAttrGroupInterface() !=
	    m_RelIncrTimeDescriptorInterface->
	    GetTimeUnitInterface()) return -1;

	m_DecoderBuffer->GetBitsLong(&a,1);
	if(a){
	  /* decode TimeUnit AttrGroup description*/
	  m_TimeUnitCSInterface->StartDecode();
	  
	  
	  /* check values for TimeBase **************************/ 
	  /* check if TimeBase is linked to histogram*/
	  if (!m_RelIncrTimeDescriptorInterface->
	      GetTimeBaseInterface()) {
	    return -1;
	  }
	  
	  /* check if TimeBase AttrGroup and stream buffer are linked
	     to TimeBase coding scheme*/
	  if (!m_TimeBaseCSInterface->GetDescriptorInterface() ||
	      !m_TimeBaseCSInterface->GetEncoderStreamBuffer()) return -1;
	  
	  /* check if TimeBase attached to
	     RelIncrTime descriptor
	     is equal to TimeBase attached to
	     RelIncrTime coding */
	  if (m_TimeBaseCSInterface->GetDescriptorInterface() !=
	      m_RelIncrTimeDescriptorInterface->
	      GetTimeBaseInterface()) return -1;
	  
	  m_DecoderBuffer->TakeBits(1);
 
	  /* TimeBase Reference*/
	  m_TimeBaseCSInterface->StartDecode();
	}
	m_DecoderBuffer->GetBitsLong(&a,32);
	m_DecoderBuffer->TakeBits(32);
	m_RelIncrTimeDescriptorInterface->SetRelIncrTime(a);

	return 0;
}

//----------------------------------------------------------------------------
RelIncrTimeCSInterfaceABC *RelIncrTimeCS::GetInterface(void)
{ return &m_Interface; }



