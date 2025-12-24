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
// Applicable File Name:  TimePointCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "TimePointCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 
#include <math.h>
 

using namespace XM;

const UUID TimePointCSInterface::myID = UUID("");
const char *TimePointCSInterface::myName =
                         "TimePoint Coding Engine Interface";

const UUID TimePointCS::myID = UUID("");
const char *TimePointCS::myName = "TimePoint Coding Scheme";

const UUID TimePointCS::valID = UUID("");
const char *TimePointCS::valName = "";

const UUID TimePointCSInterfaceABC::myID = UUID("");

TimePointCSInterfaceABC::TimePointCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
TimePointCSInterface::TimePointCSInterface(TimePointCS *aTimePoint):
m_TimePointCS(aTimePoint)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimePointCSInterface::~TimePointCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& TimePointCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimePointCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *TimePointCSInterface::GetEncoderStreamBuffer(void)
{
        return m_TimePointCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimePointCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TimePointCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *TimePointCSInterface::GetDecoderStreamBuffer(void)
{
        return m_TimePointCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int TimePointCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_TimePointCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC *TimePointCSInterface::
GetDescriptorInterface(void)
{
        return m_TimePointCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int TimePointCSInterface::
SetDescriptorInterface(TimePointDescriptorInterfaceABC
		       *aTimePointDescriptorInterface)
{
        return m_TimePointCS->
	  SetDescriptorInterface(aTimePointDescriptorInterface);
}

//----------------------------------------------------------------------------
int TimePointCSInterface::StartEncode(void)
{
        return m_TimePointCS->StartEncode();
}

//----------------------------------------------------------------------------
int TimePointCSInterface::StartDecode(void)
{
        return m_TimePointCS->StartDecode();
}

//=============================================================================

TimePointCS::TimePointCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_TimePointDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
TimePointCS::~TimePointCS()
{

}

//----------------------------------------------------------------------------
const UUID& TimePointCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimePointCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimePointCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimePointCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimePointCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * TimePointCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int TimePointCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * TimePointCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int TimePointCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC *TimePointCS::GetDescriptorInterface(void)
{
        return m_TimePointDescriptorInterface;
}

//----------------------------------------------------------------------------
int TimePointCS::
SetDescriptorInterface(TimePointDescriptorInterfaceABC
	      *aTimePointDescriptorInterface)
{
        if (!aTimePointDescriptorInterface) return -1;

        m_TimePointDescriptorInterface = aTimePointDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
int TimePointCS::StartEncode(void)
{
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_TimePointDescriptorInterface) {
	  return -1;
	}
	a=m_TimePointDescriptorInterface->GetAttribute('n');
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
	
	a=m_TimePointDescriptorInterface->GetAttribute('T');
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
	a=m_TimePointDescriptorInterface->GetTimePointElement('Y');
	m_EncoderBuffer->PutBitsLong(a,16);
	if(nn_flag) {
	  a=m_TimePointDescriptorInterface->GetTimePointElement('M');
	  m_EncoderBuffer->PutBitsLong(a,4);	  
	  a=m_TimePointDescriptorInterface->GetTimePointElement('D');
	  m_EncoderBuffer->PutBitsLong(a,5);	
	  a=m_TimePointDescriptorInterface->GetTimePointElement('h');
	  m_EncoderBuffer->PutBitsLong(a,5);
	  a=m_TimePointDescriptorInterface->GetTimePointElement('m');
	  m_EncoderBuffer->PutBitsLong(a,6);	  
	  a=m_TimePointDescriptorInterface->GetTimePointElement('s');
	  m_EncoderBuffer->PutBitsLong(a,6);	
	  a=m_TimePointDescriptorInterface->GetTimePointElement('n');
	  m_EncoderBuffer->PutBitsLong(a,nn_flag);
	}	
	else {
	  f=0;
	  a=m_TimePointDescriptorInterface->GetTimePointElement('M');
	  if(a<0) {
	    f++;	  
	    a=m_TimePointDescriptorInterface->GetTimePointElement('D');
	    if(a<0) {
	      f++;	
	      a=m_TimePointDescriptorInterface->GetTimePointElement('h');
	      if(a<0) {
		f++;
		a=m_TimePointDescriptorInterface->GetTimePointElement('m');
		if(a<0) {
		  f++;	  
		  a=m_TimePointDescriptorInterface->GetTimePointElement('s');
		  if(a<0) {
		    f++;
		    a=m_TimePointDescriptorInterface->GetTimePointElement('n');
		    if(a<0) f++;
		  }
		}
	      }
	    }
	  }
	
	  m_EncoderBuffer->PutBitsLong(f,3);

	  if(f>0){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('M');
	    m_EncoderBuffer->PutBitsLong(a,4);	  
	  }
	  if(f>1){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('D');
	    m_EncoderBuffer->PutBitsLong(a,5);	
	  }
	  if(f>2){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('h');
	    m_EncoderBuffer->PutBitsLong(a,5);
	  }
	  if(f>3){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('m');
	    m_EncoderBuffer->PutBitsLong(a,6);	  
	  }
	  if(f>4){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('s');
	    m_EncoderBuffer->PutBitsLong(a,6);	
	  }
	  if(f>5 && nn_flag){
	    a=m_TimePointDescriptorInterface->GetTimePointElement('n');
	    m_EncoderBuffer->PutBitsLong(a,nn_flag);	
	  }
	}
	return 0;
}


//----------------------------------------------------------------------------
int TimePointCS::StartDecode(void)
{
     
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_TimePointDescriptorInterface) {
	  return -1;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,16);
	  m_DecoderBuffer->TakeBits(16);
	  m_TimePointDescriptorInterface->SetAttribute('n',a);
	  nn_flag=int (ceil(log(a)/0.30102999566));
	}
	else{
	  nn_flag=m_TimePointDescriptorInterface->GetAttribute('n');
	  if(nn_flag<0) nn_flag=0;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,12);
	  m_DecoderBuffer->TakeBits(12);
	  m_TimePointDescriptorInterface->SetAttribute('T',a-1200);
	}
	else
	  m_TimePointDescriptorInterface->SetAttribute('T',0);

	m_DecoderBuffer->GetBitsLong(&a,16);
	m_DecoderBuffer->TakeBits(16);
	m_TimePointDescriptorInterface->SetTimePointElement('Y',a);

	
	if(nn_flag) {
	  m_DecoderBuffer->GetBitsLong(&a,4);
	  m_DecoderBuffer->TakeBits(4);
	  m_TimePointDescriptorInterface->SetTimePointElement('M',a);
	  m_DecoderBuffer->GetBitsLong(&a,5);
	  m_DecoderBuffer->TakeBits(5);
	  m_TimePointDescriptorInterface->SetTimePointElement('D',a);
	  m_DecoderBuffer->GetBitsLong(&a,5);
	  m_DecoderBuffer->TakeBits(5);
	  m_TimePointDescriptorInterface->SetTimePointElement('h',a);
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_TimePointDescriptorInterface->SetTimePointElement('m',a);
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_TimePointDescriptorInterface->SetTimePointElement('s',a);
	  m_DecoderBuffer->GetBitsLong(&a,nn_flag);
	  m_DecoderBuffer->TakeBits(nn_flag);
	  m_TimePointDescriptorInterface->SetTimePointElement('n',a);

	}	
	else {
	  m_DecoderBuffer->GetBitsLong(&f,3);
	  m_DecoderBuffer->TakeBits(3);

	  if(f>0){
	    m_DecoderBuffer->GetBitsLong(&a,4);
	    m_DecoderBuffer->TakeBits(4);
	    m_TimePointDescriptorInterface->SetTimePointElement('M',a);
	  }
	  if(f>1){
	    m_DecoderBuffer->GetBitsLong(&a,5);
	    m_DecoderBuffer->TakeBits(5);
	    m_TimePointDescriptorInterface->SetTimePointElement('D',a);
	  }
	  if(f>2){
	    m_DecoderBuffer->GetBitsLong(&a,5);
	    m_DecoderBuffer->TakeBits(5);
	    m_TimePointDescriptorInterface->SetTimePointElement('h',a); 
	  }
	  if(f>3){
	    m_DecoderBuffer->GetBitsLong(&a,6);
	    m_DecoderBuffer->TakeBits(6);
	    m_TimePointDescriptorInterface->SetTimePointElement('m',a);
	  }
	  if(f>4){
	    m_DecoderBuffer->GetBitsLong(&a,6);
	    m_DecoderBuffer->TakeBits(6);
	    m_TimePointDescriptorInterface->SetTimePointElement('s',a);
	  }
	  if((f>5)&&nn_flag){
	    m_DecoderBuffer->GetBitsLong(&a,nn_flag);
	    m_DecoderBuffer->TakeBits(nn_flag);
	    m_TimePointDescriptorInterface->SetTimePointElement('n',a);
	  }


	}
	return 0;
}




//----------------------------------------------------------------------------
TimePointCSInterfaceABC *TimePointCS::GetInterface(void)
{ return &m_Interface; }



