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
// Applicable File Name:  RelTimeCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "RelTimeCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 
#include <math.h>
 

using namespace XM;

const UUID RelTimeCSInterface::myID = UUID("");
const char *RelTimeCSInterface::myName =
                         "RelTime Coding Engine Interface";

const UUID RelTimeCS::myID = UUID("");
const char *RelTimeCS::myName = "RelTime Coding Scheme";

const UUID RelTimeCS::valID = UUID("");
const char *RelTimeCS::valName = "";

const UUID RelTimeCSInterfaceABC::myID = UUID("");

RelTimeCSInterfaceABC::RelTimeCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
RelTimeCSInterface::RelTimeCSInterface(RelTimeCS *aRelTime):
m_RelTimeCS(aRelTime)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RelTimeCSInterface::~RelTimeCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& RelTimeCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelTimeCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *RelTimeCSInterface::GetEncoderStreamBuffer(void)
{
        return m_RelTimeCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RelTimeCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *RelTimeCSInterface::GetDecoderStreamBuffer(void)
{
        return m_RelTimeCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_RelTimeCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
RelTimeDescriptorInterfaceABC *RelTimeCSInterface::
GetDescriptorInterface(void)
{
        return m_RelTimeCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::
SetDescriptorInterface(RelTimeDescriptorInterfaceABC
		       *aRelTimeDescriptorInterface)
{
        return m_RelTimeCS->
	  SetDescriptorInterface(aRelTimeDescriptorInterface);
}

//----------------------------------------------------------------------------
TimePointCSInterfaceABC
*RelTimeCSInterface::GetTimeBaseCSInterface(void)

{
        return m_RelTimeCS->GetTimeBaseCSInterface();
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::
SetTimeBaseCSInterface(TimePointCSInterfaceABC
		       *aTimeBaseDescriptorInterface)
{
        return m_RelTimeCS->
	  SetTimeBaseCSInterface(aTimeBaseDescriptorInterface);
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::StartEncode(void)
{
        return m_RelTimeCS->StartEncode();
}

//----------------------------------------------------------------------------
int RelTimeCSInterface::StartDecode(void)
{
        return m_RelTimeCS->StartDecode();
}

//=============================================================================

RelTimeCS::RelTimeCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_RelTimeDescriptorInterface(0),
m_TimeBaseCSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RelTimeCS::~RelTimeCS()
{

}

//----------------------------------------------------------------------------
const UUID& RelTimeCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelTimeCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RelTimeCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RelTimeCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool RelTimeCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * RelTimeCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int RelTimeCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * RelTimeCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int RelTimeCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
RelTimeDescriptorInterfaceABC *RelTimeCS::GetDescriptorInterface(void)
{
        return m_RelTimeDescriptorInterface;
}

//----------------------------------------------------------------------------
int RelTimeCS::
SetDescriptorInterface(RelTimeDescriptorInterfaceABC
	      *aRelTimeDescriptorInterface)
{
        if (!aRelTimeDescriptorInterface) return -1;

        m_RelTimeDescriptorInterface = aRelTimeDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
TimePointCSInterfaceABC *RelTimeCS::GetTimeBaseCSInterface(void)

{
        return m_TimeBaseCSInterface;
}

//----------------------------------------------------------------------------
int RelTimeCS::SetTimeBaseCSInterface(TimePointCSInterfaceABC
						*aTimeBaseCSInterface)

{
        if (!aTimeBaseCSInterface) return -1;

        m_TimeBaseCSInterface = aTimeBaseCSInterface;
	return 0;
}

//----------------------------------------------------------------------------
int RelTimeCS::StartEncode(void)
{
        long a,f;
	int nn_flag=0;
	



	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_RelTimeDescriptorInterface) {
	  return -1;
	}

	/* check values for TimeBase **************************/ 
	/* check if TimeBase is linked to histogram*/
	if (!m_RelTimeDescriptorInterface->
	    GetTimeBaseInterface()) {
	  f=0;
	  m_EncoderBuffer->PutBitsLong(f,1);
	}
	else{
	  f=1;
	  m_EncoderBuffer->PutBitsLong(f,1);
	  /* check if TimeBase AttrGroup and stream buffer are linked
	     to TimeBase coding scheme*/
	  if (!m_TimeBaseCSInterface->GetDescriptorInterface() ||
	      !m_TimeBaseCSInterface->GetEncoderStreamBuffer()) return -1;
	  
	  /* check if TimeBase attached to
	     RelTime descriptor
	     is equal to TimeBase attached to
	     RelTime coding */
	  if (m_TimeBaseCSInterface->GetDescriptorInterface() !=
	      m_RelTimeDescriptorInterface->
	      GetTimeBaseInterface()) return -1;

	  /* TimeBase Reference*/
	  m_TimeBaseCSInterface->StartEncode();
	}

	a=m_RelTimeDescriptorInterface->GetAttribute('n');
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
	
	a=m_RelTimeDescriptorInterface->GetAttribute('T');
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
	a=m_RelTimeDescriptorInterface->GetRelTimeElement('N');
	m_EncoderBuffer->PutBitsLong(a,1);

	a=m_RelTimeDescriptorInterface->GetRelTimeElement('D');
	if(!a<0)f+=16;	
	a=m_RelTimeDescriptorInterface->GetRelTimeElement('h');
	if(!a<0)f+=8;
	a=m_RelTimeDescriptorInterface->GetRelTimeElement('m');
	if(!a<0)f+=4; 
	a=m_RelTimeDescriptorInterface->GetRelTimeElement('s');
	if(!a<0)f+=2;
	a=m_RelTimeDescriptorInterface->GetRelTimeElement('n');
	if(!a<0)f+=1;
	m_EncoderBuffer->PutBitsLong(f,5);

	if(f&16){
	  a=m_RelTimeDescriptorInterface->GetRelTimeElement('D');
	  m_EncoderBuffer->PutBitsLong(a,31);	
	}
	if(f&8){
	  a=m_RelTimeDescriptorInterface->GetRelTimeElement('h');
	  m_EncoderBuffer->PutBitsLong(a,5);
	}
	if(f&4){
	    a=m_RelTimeDescriptorInterface->GetRelTimeElement('m');
	    m_EncoderBuffer->PutBitsLong(a,6);	  
	}
	if(f&2){
	  a=m_RelTimeDescriptorInterface->GetRelTimeElement('s');
	  m_EncoderBuffer->PutBitsLong(a,6);	
	}
	if(f&1 && nn_flag){
	  // the check for nn_flag is not expressed in the DDL
	  a=m_RelTimeDescriptorInterface->GetRelTimeElement('n');
	  m_EncoderBuffer->PutBitsLong(a,nn_flag);
	}

	return 0;
}


//----------------------------------------------------------------------------
int RelTimeCS::StartDecode(void)
{
     
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_RelTimeDescriptorInterface) {
	  return -1;
	}
	m_DecoderBuffer->GetBitsLong(&a,1);
	if(a) {
	  /* check values for TimeBase **************************/ 
	  /* check if TimeBase is linked to histogram*/
	  if (!m_RelTimeDescriptorInterface->
	      GetTimeBaseInterface()) {
	    return -1;
	  }
	  
	  /* check if TimeBase AttrGroup and stream buffer are linked
	     to TimeBase coding scheme*/
	  if (!m_TimeBaseCSInterface->GetDescriptorInterface() ||
	      !m_TimeBaseCSInterface->GetEncoderStreamBuffer()) return -1;
	  
	  /* check if TimeBase attached to
	     RelTime descriptor
	     is equal to TimeBase attached to
	     RelTime coding */
	  if (m_TimeBaseCSInterface->GetDescriptorInterface() !=
	      m_RelTimeDescriptorInterface->
	      GetTimeBaseInterface()) return -1;
	  
	  m_DecoderBuffer->TakeBits(1);
 
	  /* TimeBase Reference*/
	  m_TimeBaseCSInterface->StartDecode();
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,31);
	  m_DecoderBuffer->TakeBits(31);
	  m_RelTimeDescriptorInterface->SetAttribute('n',a);
	  nn_flag=int (ceil(log(a)/0.30102999566));
	}
	else{
	  // nn_Fraction could have been set using former specification
	  nn_flag=m_RelTimeDescriptorInterface->GetAttribute('n');
	  if(nn_flag<0) nn_flag=0;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,12);
	  m_DecoderBuffer->TakeBits(12);
	  m_RelTimeDescriptorInterface->SetAttribute('T',a-1200);
	}

	else
	  m_RelTimeDescriptorInterface->SetAttribute('T',0);

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_RelTimeDescriptorInterface->SetRelTimeElement('N',a);

	m_DecoderBuffer->GetBitsLong(&f,1);
	m_DecoderBuffer->TakeBits(1);	


	if(f&16){
	  m_DecoderBuffer->GetBitsLong(&a,31);
	  m_DecoderBuffer->TakeBits(31);
	  m_RelTimeDescriptorInterface->SetRelTimeElement('D',a);
	}
	if(f&8){
	  m_DecoderBuffer->GetBitsLong(&a,5);
	  m_DecoderBuffer->TakeBits(5);
	  m_RelTimeDescriptorInterface->SetRelTimeElement('h',a);
	}
	if(f&4){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_RelTimeDescriptorInterface->SetRelTimeElement('m',a);
	}
	if(f&2){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_RelTimeDescriptorInterface->SetRelTimeElement('s',a);
	}
	if((f&1)&&nn_flag){
	  m_DecoderBuffer->GetBitsLong(&a,nn_flag);
	  m_DecoderBuffer->TakeBits(nn_flag);
	  m_RelTimeDescriptorInterface->SetRelTimeElement('n',a);
	}


	return 0;
}




//----------------------------------------------------------------------------
RelTimeCSInterfaceABC *RelTimeCS::GetInterface(void)
{ return &m_Interface; }



