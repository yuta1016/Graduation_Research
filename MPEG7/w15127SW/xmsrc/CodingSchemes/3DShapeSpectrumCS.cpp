///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrumCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID ShapeSpectrumCSInterface::myID = UUID("");
const char *ShapeSpectrumCSInterface::myName =
                         "ShapeSpectrum Coding Engine Interface";

const UUID ShapeSpectrumCS::myID = UUID("");
const char *ShapeSpectrumCS::myName = "ShapeSpectrum Coding Scheme";

const UUID ShapeSpectrumCS::valID = UUID("");
const char *ShapeSpectrumCS::valName = "";

const UUID ShapeSpectrumCSInterfaceABC::myID = UUID("");

ShapeSpectrumCSInterfaceABC::ShapeSpectrumCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
ShapeSpectrumCSInterface::ShapeSpectrumCSInterface(ShapeSpectrumCS *aShapeSpectrumCS):
m_ShapeSpectrumCS(aShapeSpectrumCS)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ShapeSpectrumCSInterface::~ShapeSpectrumCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& ShapeSpectrumCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void ShapeSpectrumCSInterface::destroy(void)
{ delete m_ShapeSpectrumCS; }

//----------------------------------------------------------------------------
BitBuffer *ShapeSpectrumCSInterface::GetEncoderStreamBuffer(void)
{
        return m_ShapeSpectrumCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ShapeSpectrumCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ShapeSpectrumCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *ShapeSpectrumCSInterface::GetDecoderStreamBuffer(void)
{
        return m_ShapeSpectrumCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int ShapeSpectrumCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_ShapeSpectrumCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC
*ShapeSpectrumCSInterface::GetDescriptorInterface(void)

{
        return m_ShapeSpectrumCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int ShapeSpectrumCSInterface::
SetDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aShapeSpectrumDescriptorInterface)
{
        return m_ShapeSpectrumCS->
	  SetDescriptorInterface(aShapeSpectrumDescriptorInterface);
}

//----------------------------------------------------------------------------
int ShapeSpectrumCSInterface::StartEncode(void)
{
        return m_ShapeSpectrumCS->StartEncode();
}

//----------------------------------------------------------------------------
int ShapeSpectrumCSInterface::StartDecode(void)
{
        return m_ShapeSpectrumCS->StartDecode();
}
//=============================================================================

ShapeSpectrumCS::ShapeSpectrumCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ShapeSpectrumDescriptorInterface(0)

{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------

ShapeSpectrumCS::ShapeSpectrumCS(ShapeSpectrumDescriptorInterfaceABC *shapespectrum):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_ShapeSpectrumDescriptorInterface(0)

{
	/* create descriptor if it does not exist*/
	if (!shapespectrum) {
	  ShapeSpectrumDescriptor *descriptor =
	    new ShapeSpectrumDescriptor();
	  shapespectrum = descriptor->GetInterface();
	}

	/* connect descriptor with coding schemes*/
	SetDescriptorInterface(shapespectrum);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
ShapeSpectrumCS::~ShapeSpectrumCS()
{
  if (m_ShapeSpectrumDescriptorInterface)
    m_ShapeSpectrumDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ShapeSpectrumCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ShapeSpectrumCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ShapeSpectrumCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ShapeSpectrumCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * ShapeSpectrumCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int ShapeSpectrumCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * ShapeSpectrumCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int ShapeSpectrumCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
ShapeSpectrumDescriptorInterfaceABC *ShapeSpectrumCS::
GetDescriptorInterface(void)
{
       return m_ShapeSpectrumDescriptorInterface;
}

//----------------------------------------------------------------------------
int ShapeSpectrumCS::
SetDescriptorInterface(ShapeSpectrumDescriptorInterfaceABC
		       *aShapeSpectrumDescriptorInterface)
{
        /* check if new value is different from old value*/
        if (m_ShapeSpectrumDescriptorInterface == 
	    aShapeSpectrumDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_ShapeSpectrumDescriptorInterface) 
	  m_ShapeSpectrumDescriptorInterface->release();

	/* add new interface*/
	m_ShapeSpectrumDescriptorInterface = 
	  aShapeSpectrumDescriptorInterface;
	if (m_ShapeSpectrumDescriptorInterface) {
	  m_ShapeSpectrumDescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
int ShapeSpectrumCS::StartEncode(void)
{
  long a; long shapespectrumNoOfBits;

  long shapespectrumsize=0;
//  int no_of_bits;
  
  if (!m_EncoderBuffer) {
    return -1;
  }
  
  /* check if descriptor and stream buffer are linked
     to coding scheme*/
  if (!m_EncoderBuffer || !m_ShapeSpectrumDescriptorInterface) {
    return -1;
  }
  
  /* encode ShapeSpectrum*/
  /*put no of bins*/
  /*	fprintf(stderr,"Question: No. of bins in histogram descriptor?\n");*/
  shapespectrumsize=m_ShapeSpectrumDescriptorInterface->GetShapeSpectrumSize();
  shapespectrumNoOfBits = m_ShapeSpectrumDescriptorInterface->GetNoOfBits();
  /*	a=(int)htostreaml(histsize);
	m_EncoderBuffer->PutBits((char *)&a,32UL);*/
  if (!shapespectrumsize) {return 0;}
  
  /*put normalization
    a=m_HistogramDescriptorInterface->GetNormalizationFactor();*/
  
  m_EncoderBuffer->PutBitsLong(shapespectrumsize,8);
  m_EncoderBuffer->PutBitsLong(shapespectrumNoOfBits,4);
  
  for (int i=0; i<shapespectrumsize; i++) {
    
    /*put normalization*/
    a=m_ShapeSpectrumDescriptorInterface->GetElement(i);
    m_EncoderBuffer->PutBitsLong(a,shapespectrumNoOfBits);
  }
  
  a=m_ShapeSpectrumDescriptorInterface->GetPlanarElement();
  m_EncoderBuffer->PutBitsLong(a,shapespectrumNoOfBits);

  a=m_ShapeSpectrumDescriptorInterface->GetSingularElement();
  m_EncoderBuffer->PutBitsLong(a,shapespectrumNoOfBits);

  return 0;
}

//----------------------------------------------------------------------------
int ShapeSpectrumCS::StartDecode(void)
{
  int i; 
  long a;
  long shapespectrumsize;
  long shapespectrumNoOfBits;
  
  if (!m_DecoderBuffer) {
    return -1;
  }
  
  /* check if descriptor and stream buffer are linked
     to coding scheme*/
  if (!m_DecoderBuffer || !m_ShapeSpectrumDescriptorInterface) {
    return -1;
  }
  
  /* decode ShapeSpectrum*/
  /*get no of bins*/
  /*	fprintf(stderr,"Question: No. of bins in histogram descriptor?\n");
  shapespectrumsize=m_ShapeSpectrumDescriptorInterface->GetShapeSpectrumSize();*/
  m_DecoderBuffer->GetBitsLong((&shapespectrumsize),8);
  m_DecoderBuffer->TakeBits(8);
  m_ShapeSpectrumDescriptorInterface->SetShapeSpectrumSize((unsigned long)shapespectrumsize);


  /*	a=(int)htostreaml(histsize);
	m_DecoderBuffer->GetBits((char *)&a,32UL);*/
  if (!shapespectrumsize) return 0;
  
  /*get number of bits*/
  m_DecoderBuffer->GetBitsLong(&shapespectrumNoOfBits,4);
  m_DecoderBuffer->TakeBits(4);
  m_ShapeSpectrumDescriptorInterface->SetNoOfBits(shapespectrumNoOfBits);
  
  
  for (i=0; i<shapespectrumsize; i++) {
    
    /*get element*/
    m_DecoderBuffer->GetBitsLong(&a, shapespectrumNoOfBits);
    m_DecoderBuffer->TakeBits(shapespectrumNoOfBits);
    m_ShapeSpectrumDescriptorInterface->SetElement(i,a);
  }
  
  m_DecoderBuffer->GetBitsLong(&a,shapespectrumNoOfBits);
  m_DecoderBuffer->TakeBits(shapespectrumNoOfBits);
  m_ShapeSpectrumDescriptorInterface->SetPlanarElement(a);

  m_DecoderBuffer->GetBitsLong(&a, shapespectrumNoOfBits);
  m_DecoderBuffer->TakeBits(shapespectrumNoOfBits);
  m_ShapeSpectrumDescriptorInterface->SetSingularElement(a);

  return 0;
}

//----------------------------------------------------------------------------
ShapeSpectrumCSInterfaceABC *ShapeSpectrumCS::GetInterface(void)
{ return &m_Interface; }
