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
// Applicable File Name:  DurationCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "DurationCS.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif 
#include <math.h>
 

using namespace XM;

const UUID DurationCSInterface::myID = UUID("");
const char *DurationCSInterface::myName =
                         "Duration Coding Engine Interface";

const UUID DurationCS::myID = UUID("");
const char *DurationCS::myName = "Duration Coding Scheme";

const UUID DurationCS::valID = UUID("");
const char *DurationCS::valName = "";

const UUID DurationCSInterfaceABC::myID = UUID("");

DurationCSInterfaceABC::DurationCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
DurationCSInterface::DurationCSInterface(DurationCS *aDuration):
m_DurationCS(aDuration)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DurationCSInterface::~DurationCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& DurationCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DurationCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
BitBuffer *DurationCSInterface::GetEncoderStreamBuffer(void)
{
        return m_DurationCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int DurationCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_DurationCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *DurationCSInterface::GetDecoderStreamBuffer(void)
{
        return m_DurationCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int DurationCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_DurationCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
DurationDescriptorInterfaceABC *DurationCSInterface::
GetDescriptorInterface(void)
{
        return m_DurationCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int DurationCSInterface::
SetDescriptorInterface(DurationDescriptorInterfaceABC
		       *aDurationDescriptorInterface)
{
        return m_DurationCS->
	  SetDescriptorInterface(aDurationDescriptorInterface);
}

//----------------------------------------------------------------------------
int DurationCSInterface::StartEncode(void)
{
        return m_DurationCS->StartEncode();
}

//----------------------------------------------------------------------------
int DurationCSInterface::StartDecode(void)
{
        return m_DurationCS->StartDecode();
}

//=============================================================================

DurationCS::DurationCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DurationDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DurationCS::~DurationCS()
{

}

//----------------------------------------------------------------------------
const UUID& DurationCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DurationCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& DurationCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* DurationCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool DurationCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * DurationCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int DurationCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * DurationCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int DurationCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
DurationDescriptorInterfaceABC *DurationCS::GetDescriptorInterface(void)
{
        return m_DurationDescriptorInterface;
}

//----------------------------------------------------------------------------
int DurationCS::
SetDescriptorInterface(DurationDescriptorInterfaceABC
	      *aDurationDescriptorInterface)
{
        if (!aDurationDescriptorInterface) return -1;

        m_DurationDescriptorInterface = aDurationDescriptorInterface;
	return 0;
}

//----------------------------------------------------------------------------
int DurationCS::StartEncode(void)
{
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_DurationDescriptorInterface) {
	  return -1;
	}
	a=m_DurationDescriptorInterface->GetAttribute('n');
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
	
	a=m_DurationDescriptorInterface->GetAttribute('T');
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
	a=m_DurationDescriptorInterface->GetDurationElement('N');
	m_EncoderBuffer->PutBitsLong(a,1);

	a=m_DurationDescriptorInterface->GetDurationElement('D');
	if(!a<0)f+=16;	
	a=m_DurationDescriptorInterface->GetDurationElement('h');
	if(!a<0)f+=8;
	a=m_DurationDescriptorInterface->GetDurationElement('m');
	if(!a<0)f+=4; 
	a=m_DurationDescriptorInterface->GetDurationElement('s');
	if(!a<0)f+=2;
	a=m_DurationDescriptorInterface->GetDurationElement('n');
	if(!a<0)f+=1;
	m_EncoderBuffer->PutBitsLong(f,5);

	if(f&16){
	  a=m_DurationDescriptorInterface->GetDurationElement('D');
	  m_EncoderBuffer->PutBitsLong(a,31);	
	}
	if(f&8){
	  a=m_DurationDescriptorInterface->GetDurationElement('h');
	  m_EncoderBuffer->PutBitsLong(a,5);
	}
	if(f&4){
	    a=m_DurationDescriptorInterface->GetDurationElement('m');
	    m_EncoderBuffer->PutBitsLong(a,6);	  
	}
	if(f&2){
	  a=m_DurationDescriptorInterface->GetDurationElement('s');
	  m_EncoderBuffer->PutBitsLong(a,6);	
	}
	if(f&1 && nn_flag){
	  // the check for nn_flag is not expressed in the DDL
	  a=m_DurationDescriptorInterface->GetDurationElement('n');
	  m_EncoderBuffer->PutBitsLong(a,nn_flag);
	}

	return 0;
}


//----------------------------------------------------------------------------
int DurationCS::StartDecode(void)
{
     
        long a,f;
	int nn_flag=0;
	

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_DurationDescriptorInterface) {
	  return -1;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,16);
	  m_DecoderBuffer->TakeBits(16);
	  m_DurationDescriptorInterface->SetAttribute('n',a);
	  nn_flag=int (ceil(log(a)/0.30102999566));
	}
	else{
	  // nn_Fraction could have been set using former specification
	  nn_flag=m_DurationDescriptorInterface->GetAttribute('n');
	  if(nn_flag<0) nn_flag=0;
	}

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	if(a){
	  m_DecoderBuffer->GetBitsLong(&a,12);
	  m_DecoderBuffer->TakeBits(12);
	  m_DurationDescriptorInterface->SetAttribute('T',a-1200);
	}
	
        else
          m_DurationDescriptorInterface->SetAttribute('T',0);

	m_DecoderBuffer->GetBitsLong(&a,1);
	m_DecoderBuffer->TakeBits(1);
	m_DurationDescriptorInterface->SetDurationElement('N',a);

	m_DecoderBuffer->GetBitsLong(&f,1);
	m_DecoderBuffer->TakeBits(1);	


	if(f&16){
	  m_DecoderBuffer->GetBitsLong(&a,31);
	  m_DecoderBuffer->TakeBits(31);
	  m_DurationDescriptorInterface->SetDurationElement('D',a);
	}
	if(f&8){
	  m_DecoderBuffer->GetBitsLong(&a,5);
	  m_DecoderBuffer->TakeBits(5);
	  m_DurationDescriptorInterface->SetDurationElement('h',a);
	}
	if(f&4){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_DurationDescriptorInterface->SetDurationElement('m',a);
	}
	if(f&2){
	  m_DecoderBuffer->GetBitsLong(&a,6);
	  m_DecoderBuffer->TakeBits(6);
	  m_DurationDescriptorInterface->SetDurationElement('s',a);
	}
	if((f&1)&&nn_flag){
	  m_DecoderBuffer->GetBitsLong(&a,nn_flag);
	  m_DecoderBuffer->TakeBits(nn_flag);
	  m_DurationDescriptorInterface->SetDurationElement('n',a);
	}


	return 0;
}




//----------------------------------------------------------------------------
DurationCSInterfaceABC *DurationCS::GetInterface(void)
{ return &m_Interface; }



