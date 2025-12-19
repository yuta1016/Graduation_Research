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
// Applicable File Name:  TimeUnitCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "TimeUnitCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 
#include <math.h>
 

using namespace XM;

const UUID TimeUnitCSInterface::myID = UUID("");
const char *TimeUnitCSInterface::myName =
                         "TimeUnit Coding Engine Interface";

const UUID TimeUnitCS::myID = UUID("");
const char *TimeUnitCS::myName = "TimeUnit Coding Scheme";

const UUID TimeUnitCS::valID = UUID("");
const char *TimeUnitCS::valName = "";

const UUID TimeUnitCSInterfaceABC::myID = UUID("");

TimeUnitCSInterfaceABC::TimeUnitCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
TimeUnitCSInterface::TimeUnitCSInterface(TimeUnitCS *aTimeUnit):
m_TimeUnitCS(aTimeUnit)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeUnitCSInterface::~TimeUnitCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TimeUnitCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeUnitCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *TimeUnitCSInterface::GetEncoderStreamBuffer(void)
{
        return m_TimeUnitCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimeUnitCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TimeUnitCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *TimeUnitCSInterface::GetDecoderStreamBuffer(void)
{
        return m_TimeUnitCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimeUnitCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TimeUnitCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC *TimeUnitCSInterface::
GetAttrGroupInterface(void)
{
        return m_TimeUnitCS->GetAttrGroupInterface();
}

//----------------------------------------------------------------------------
int TimeUnitCSInterface::
SetAttrGroupInterface(TimeUnitAttrGroupInterfaceABC
		       *aTimeUnitAttrGroupInterface)
{
        return m_TimeUnitCS->
	  SetAttrGroupInterface(aTimeUnitAttrGroupInterface);
}

//----------------------------------------------------------------------------
int TimeUnitCSInterface::StartEncode(void)
{
        return m_TimeUnitCS->StartEncode();
}

//----------------------------------------------------------------------------
int TimeUnitCSInterface::StartDecode(void)
{
        return m_TimeUnitCS->StartDecode();
}

//=============================================================================

TimeUnitCS::TimeUnitCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_TimeUnitAttrGroupInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimeUnitCS::~TimeUnitCS()
{

}

//----------------------------------------------------------------------------
const UUID& TimeUnitCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeUnitCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimeUnitCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimeUnitCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimeUnitCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * TimeUnitCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int TimeUnitCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * TimeUnitCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int TimeUnitCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC *TimeUnitCS::GetAttrGroupInterface(void)
{
        return m_TimeUnitAttrGroupInterface;
}

//----------------------------------------------------------------------------
int TimeUnitCS::
SetAttrGroupInterface(TimeUnitAttrGroupInterfaceABC
	      *aTimeUnitAttrGroupInterface)
{
        if (!aTimeUnitAttrGroupInterface) return -1;

        m_TimeUnitAttrGroupInterface = aTimeUnitAttrGroupInterface;
	return 0;
}

//----------------------------------------------------------------------------
int TimeUnitCS::StartEncode(void)
{
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_TimeUnitAttrGroupInterface) {
	  return -1;
	}
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('F');
	if(a<0)
	  {
	    f=0;
	    m_EncoderBuffer->PutBitsLong(f,1);
	  }
	else {
	  nn_flag=int (ceil(log(a)/0.30102999566));
	  f=1;
	  m_EncoderBuffer->PutBitsLong(f,1);
	  m_EncoderBuffer->PutBitsLong(a,16);
	}
	
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('T');
	if(a==-1)
	  {
	    f=0;
	    m_EncoderBuffer->PutBitsLong(f,1);
	  }
	else {
	  f=1;
	  m_EncoderBuffer->PutBitsLong(f,1);
	  a+=1200; // Check if -12 is smallest value!!
	  m_EncoderBuffer->PutBitsLong(a,12);
	  // value range 0-2500
	}
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('N');
	m_EncoderBuffer->PutBitsLong(a,1);

	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('D');
	if(!a<0)f+=16;	
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('h');
	if(!a<0)f+=8;
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('m');
	if(!a<0)f+=4; 
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('s');
	if(!a<0)f+=2;
	a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('n');
	if(!a<0)f+=1;
	m_EncoderBuffer->PutBitsLong(f,5);

	if(f&16){
	  a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('D');
	  m_EncoderBuffer->PutBitsLong(a,31);	
	}
	if(f&8){
	  a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('h');
	  m_EncoderBuffer->PutBitsLong(a,5);
	}
	if(f&4){
	    a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('m');
	    m_EncoderBuffer->PutBitsLong(a,6);	  
	}
	if(f&2){
	  a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('s');
	  m_EncoderBuffer->PutBitsLong(a,6);	
	}
	if(f&1 && nn_flag){
	  // the check for nn_flag is not expressed in the DDL
	  a=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('n');
	  m_EncoderBuffer->PutBitsLong(a,nn_flag);
	}

	return 0;
}


//----------------------------------------------------------------------------
int TimeUnitCS::StartDecode(void)
{
     
        long a,f;
	int nn_flag=0;
	

	/* check if AttrGroup and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_TimeUnitAttrGroupInterface) {
	  return -1;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,16);
	  m_DecoderBuffer->TakeBits(16);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('F',a);
	  nn_flag=int (ceil(log(a)/0.30102999566));
	}
	else{
	  // nn_Fraction could have been set using former specification
	  nn_flag=m_TimeUnitAttrGroupInterface->GetTimeUnitElement('F');
	  if(nn_flag<0) nn_flag=0;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,12);
	  m_DecoderBuffer->TakeBits(12);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('T',a-1200);
	}
	else {
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('T',0);
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_TimeUnitAttrGroupInterface->SetTimeUnitElement('N',a);

	m_DecoderBuffer->GetBitsLong(&f,1);
	m_DecoderBuffer->TakeBits(1);	


	if(f&16){
	  m_DecoderBuffer->GetBitsLong(&a,31);
	  m_DecoderBuffer->TakeBits(31);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('D',a);
	}
	if(f&8){
	  m_DecoderBuffer->GetBitsLong(&a,5);
	  m_DecoderBuffer->TakeBits(5);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('h',a);
	}
	if(f&4){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('m',a);
	}
	if(f&2){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('s',a);
	}
	if((f&1)&&nn_flag){
	  m_DecoderBuffer->GetBitsLong(&a,nn_flag);
	  m_DecoderBuffer->TakeBits(nn_flag);
	  m_TimeUnitAttrGroupInterface->SetTimeUnitElement('n',a);
	}


	return 0;
}




//----------------------------------------------------------------------------
TimeUnitCSInterfaceABC *TimeUnitCS::GetInterface(void)
{ return &m_Interface; }



