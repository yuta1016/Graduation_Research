///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai, Karsten Mueller
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
#include "Utilities/readparam.h" 
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID ScalableColorCSInterface::myID = UUID("");
const char *ScalableColorCSInterface::myName =
                         "Scalable Color Coding Interface";

const UUID ScalableColorCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *ScalableColorCS::myName = "ScalableColorCodingScheme";

const UUID ScalableColorCS::valID = UUID("");
const char *ScalableColorCS::valName = "";

const UUID ScalableColorCSInterfaceABC::myID = UUID("");

ScalableColorCSInterfaceABC::ScalableColorCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ScalableColorCSInterface::ScalableColorCSInterface(ScalableColorCS
						     *aScalableColor):
m_ScalableColorCS(aScalableColor)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ScalableColorCSInterface::~ScalableColorCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ScalableColorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ScalableColorCSInterface::destroy(void)
{ delete m_ScalableColorCS; }

//----------------------------------------------------------------------------
BitBuffer *ScalableColorCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ScalableColorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ScalableColorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ScalableColorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ScalableColorCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ScalableColorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ScalableColorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ScalableColorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC
*ScalableColorCSInterface::GetDescriptorInterface(void)

{
        return m_ScalableColorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ScalableColorCSInterface::
SetDescriptorInterface(ScalableColorDescriptorInterfaceABC
		       *aScalableColorDescriptorInterface)
{
        return m_ScalableColorCS->
	  SetDescriptorInterface(aScalableColorDescriptorInterface);
}

//----------------------------------------------------------------------------
int ScalableColorCSInterface::StartEncode(void)
{
        return m_ScalableColorCS->StartEncode();
}
//----------------------------------------------------------------------------
int ScalableColorCSInterface::StartDecode(void)
{
        return m_ScalableColorCS->StartDecode();
}
//=============================================================================

ScalableColorCS::ScalableColorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ScalableColorDescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//=============================================================================

ScalableColorCS::ScalableColorCS(ScalableColorDescriptorInterfaceABC
				   *scalablecolor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ScalableColorDescriptorInterface(0)
{

	/* create descriptor if it does not exist*/
	if (!scalablecolor) {
	  ScalableColorDescriptor *descriptor =
	    new ScalableColorDescriptor();
	  scalablecolor=descriptor->GetInterface();
	}

	/* connect descritors with coding schemes*/
	SetDescriptorInterface(scalablecolor);


	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ScalableColorCS::~ScalableColorCS()
{
	/* release descriptor*/
	if (m_ScalableColorDescriptorInterface)
	  m_ScalableColorDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& ScalableColorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ScalableColorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ScalableColorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ScalableColorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ScalableColorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ScalableColorCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ScalableColorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ScalableColorCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ScalableColorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
ScalableColorDescriptorInterfaceABC *ScalableColorCS::
GetDescriptorInterface(void)
{
       return m_ScalableColorDescriptorInterface;
}

//----------------------------------------------------------------------------
int ScalableColorCS::
SetDescriptorInterface(ScalableColorDescriptorInterfaceABC
		       *aScalableColorDescriptorInterface)
{
        /* check if new value is different from old value*/
        if (m_ScalableColorDescriptorInterface == 
	    aScalableColorDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_ScalableColorDescriptorInterface) 
	  m_ScalableColorDescriptorInterface->release();

	/* add new interface*/
	m_ScalableColorDescriptorInterface = 
	  aScalableColorDescriptorInterface;
	if (m_ScalableColorDescriptorInterface) {
	  m_ScalableColorDescriptorInterface->addref();
	}
	else {
	  return -1;
	}
	
	
	return 0;
}

//----------------------------------------------------------------------------
int ScalableColorCS::StartEncode(void)
{
	//printf("in ScalableColorCs\n");
  if (!m_EncoderBuffer) {
    return -1;
  }

  /* check if descriptor and stream buffer are linked
     to coding scheme*/
        if (!m_EncoderBuffer || !m_ScalableColorDescriptorInterface) {
	  return -1;
	}

	long NumberOfBitplanesDiscarded;
	long NumberOfCoefficients;
	long anzredbits,anzkoeff;
	int no_of_bits,nobits,zcheck;
	long bit,bit_sensor;

	long local_coeff[256];
	long local_sign[256];

	/*putNumberOfCoefficients*/
	
	//if (!NumberOfCoefficients) return 0;
	NumberOfCoefficients=atoi(getpara("NumberOfCoefficients","256"));
	
	if((NumberOfCoefficients!=16)&&(NumberOfCoefficients!=32)&&
	   (NumberOfCoefficients!=64)&&(NumberOfCoefficients!=128)&&
	   (NumberOfCoefficients!=256))
	return -1;
	
	if(NumberOfCoefficients==16) anzkoeff=0;
	if(NumberOfCoefficients==32) anzkoeff=1;
	if(NumberOfCoefficients==64) anzkoeff=2;
	if(NumberOfCoefficients==128) anzkoeff=3;
	if(NumberOfCoefficients==256) anzkoeff=4;
	m_EncoderBuffer->PutBitsLong(anzkoeff,3);
	
	/*put NumberOfRedBits*/
	NumberOfBitplanesDiscarded=m_ScalableColorDescriptorInterface->
	  GetNumberOfBitplanesDiscarded();

	anzredbits = -1;
	if((NumberOfBitplanesDiscarded>=0)&&(NumberOfBitplanesDiscarded<5))
	  anzredbits=NumberOfBitplanesDiscarded;
	if(NumberOfBitplanesDiscarded==6) anzredbits=5;
	if(NumberOfBitplanesDiscarded>6) anzredbits=6;
	m_EncoderBuffer->PutBitsLong(anzredbits,3);
	
	if (NumberOfBitplanesDiscarded>6){
	  for (int i=0; i<NumberOfCoefficients; i++) {
	    nobits=1;
	    m_EncoderBuffer->
	      PutBitsLong(m_ScalableColorDescriptorInterface->GetCoeffSign(i),
			  nobits);	    
	  }
	}
	else{
	  for (int i=0; i<NumberOfCoefficients; i++) {

	    local_coeff[i]=
	      m_ScalableColorDescriptorInterface->GetCoefficient(i);
	    local_sign[i]=m_ScalableColorDescriptorInterface->GetCoeffSign(i);
	    no_of_bits=scalableColorQuantValues[i][1]-
	      1-NumberOfBitplanesDiscarded;

	    //signs are coded and absolute values stored in the local_coeff
	    if(local_coeff[i]<0){
	      local_coeff[i]*=-1;
	    }

	    m_EncoderBuffer->PutBitsLong(local_sign[i],1);
	  }
	  zcheck=0;
	  for (int k=8; k>NumberOfBitplanesDiscarded; k--) {
	    bit_sensor=1<<(k-NumberOfBitplanesDiscarded-1);
	    for (int z=0; z<NumberOfCoefficients; z++){
	      no_of_bits=scalableColorQuantValues[z][1]-k;
	      if(no_of_bits>0){
		bit=((local_coeff[z]&bit_sensor)==0)?0:1;
		m_EncoderBuffer->PutBitsLong(bit,1);
		zcheck++;
	      }
	    }
	  }
	}

	return 0;
}

//----------------------------------------------------------------------------
int ScalableColorCS::StartDecode(void)
{
/*	unsigned long histsize;*/
	
	if (!m_DecoderBuffer) {
	  return -1;
	}

	/* check if descriptor and stream buffer are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_ScalableColorDescriptorInterface) {
	  return -1;
	}

	long NumberOfBitplanesDiscarded;
	
	long NumberOfCoefficients;
	long anzredbits,anzkoeff;
	int no_of_bits,nobits,z,i,zcheck;
	long local_coeff[256];
	long bit;
	long sign[256];

	for(i=0;i<256;i++)
	  local_coeff[i]=0;

	/* decode ScalableHistogram*/
	/*get NumberOfCoefficients*/
	m_DecoderBuffer->GetBitsLong(&anzkoeff,3);
	m_DecoderBuffer->TakeBits(3);
	
	NumberOfCoefficients=-1;
	if(anzkoeff==0) NumberOfCoefficients=16;
	if(anzkoeff==1) NumberOfCoefficients=32;
	if(anzkoeff==2) NumberOfCoefficients=64;
	if(anzkoeff==3) NumberOfCoefficients=128;
	if(anzkoeff==4) NumberOfCoefficients=256;
	if (!NumberOfCoefficients)
		return 0;
	m_ScalableColorDescriptorInterface->
		SetNumberOfCoefficients(NumberOfCoefficients); 
	
	/*get normalization*/
	m_DecoderBuffer->GetBitsLong(&anzredbits,3);
	m_DecoderBuffer->TakeBits(3);
	
	NumberOfBitplanesDiscarded=-1;
	if(anzredbits==0) NumberOfBitplanesDiscarded=0;
	if(anzredbits==1) NumberOfBitplanesDiscarded=1;
	if(anzredbits==2) NumberOfBitplanesDiscarded=2;
	if(anzredbits==3) NumberOfBitplanesDiscarded=3;
	if(anzredbits==4) NumberOfBitplanesDiscarded=4;
	if(anzredbits==5) NumberOfBitplanesDiscarded=6;
	if(NumberOfBitplanesDiscarded==-1) NumberOfBitplanesDiscarded=8;
	
	
	m_ScalableColorDescriptorInterface->
		SetNumberOfBitplanesDiscarded(NumberOfBitplanesDiscarded);
		
	for (i=0; i<NumberOfCoefficients; i++) {
	  nobits=1;
	  m_DecoderBuffer->GetBitsLong(&sign[i],nobits);   
	  m_DecoderBuffer->TakeBits(nobits);
	}

	if(NumberOfBitplanesDiscarded==8){
	  for (i=0; i<NumberOfCoefficients; i++) {
	    m_ScalableColorDescriptorInterface->SetCoeffValue(i,sign[i]);
	    m_ScalableColorDescriptorInterface->SetCoeffSign(i,sign[i]);
	  }
	  return 0;
	}
	else{  
	  nobits=1;
	  zcheck=0;
	  for (z=8; z>NumberOfBitplanesDiscarded;z--){

	    for (i=0; i<NumberOfCoefficients; i++) {

	      no_of_bits=scalableColorQuantValues[i][1]-z;

	      if(no_of_bits>0)
		{
		  m_DecoderBuffer->GetBitsLong(&bit,nobits);
		  m_DecoderBuffer->TakeBits(nobits);
		  if(bit)
		    local_coeff[i]++;
		  if(z>NumberOfBitplanesDiscarded+1)
		    local_coeff[i]<<=1;
		  zcheck++;
		}
	    }
	  }
	

	  for (i=0; i<NumberOfCoefficients; i++) {
	    no_of_bits=scalableColorQuantValues[i][1]-1-NumberOfBitplanesDiscarded;
	    if(no_of_bits>0)
	      local_coeff[i]*=(sign[i]==1)?1:-1;
	    else
	      local_coeff[i]=sign[i];
	  }

	  for (i=0; i<NumberOfCoefficients; i++) {
	    //signs are set based on the sign of the input parameter and not based on 0 or 1 !
	    m_ScalableColorDescriptorInterface->SetCoeffSign(i,sign[i]-1);
	    m_ScalableColorDescriptorInterface->
	      SetCoeffValue(i,local_coeff[i]);

	    //printf("i:%5d  betrag:%5d sign:%5d\n",
	    //i,local_coeff[i],sign[i]);
	  }
	}

	return 0;
}

//----------------------------------------------------------------------------
ScalableColorCSInterfaceABC *ScalableColorCS::GetInterface(void)
{ return &m_Interface; }
