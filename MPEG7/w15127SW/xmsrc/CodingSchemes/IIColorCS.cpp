///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  IIColorCS.cpp
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

const UUID IIColorCSInterface::myID = UUID("");
const char *IIColorCSInterface::myName =
                         "IIColorCodingSchemeInterface";

// create a UUID for your type
const UUID IIColorCS::myID = UUID("to be created with xx");
const char *IIColorCS::myName = "IIColorCodingScheme";

const UUID IIColorCS::valID = UUID("");
const char *IIColorCS::valName = "";

const UUID IIColorCSInterfaceABC::myID = UUID("");

IIColorCSInterfaceABC::IIColorCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
IIColorCSInterface::IIColorCSInterface(IIColorCS *aIIColor):
m_IIColorCS(aIIColor)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
IIColorCSInterface::~IIColorCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& IIColorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void IIColorCSInterface::destroy(void)
{ delete m_IIColorCS; }

//----------------------------------------------------------------------------
BitBuffer *IIColorCSInterface::GetEncoderStreamBuffer(void)
{
  return m_IIColorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int IIColorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_IIColorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *IIColorCSInterface::GetDecoderStreamBuffer(void)
{
  return m_IIColorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int IIColorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_IIColorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
IIColorDescriptorInterfaceABC
*IIColorCSInterface::GetDescriptorInterface(void)

{
  return m_IIColorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int IIColorCSInterface::
SetDescriptorInterface(IIColorDescriptorInterfaceABC
		       *aIIColorDescriptorInterface)
{
  return m_IIColorCS->
    SetDescriptorInterface(aIIColorDescriptorInterface);
}

//----------------------------------------------------------------------------
int IIColorCSInterface::StartEncode(void)
{
  return m_IIColorCS->StartEncode();
}

//----------------------------------------------------------------------------
int IIColorCSInterface::StartDecode(void)
{
  return m_IIColorCS->StartDecode();
}
//=============================================================================
IIColorCS::IIColorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IIColorCS::IIColorCS(IIColorDescriptorInterfaceABC* aIIColor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aIIColor) {
    IIColorDescriptor *descriptor =
    new IIColorDescriptor;
    aIIColor=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aIIColor);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IIColorCS::~IIColorCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& IIColorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IIColorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& IIColorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* IIColorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool IIColorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * IIColorCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int IIColorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * IIColorCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int IIColorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
IIColorDescriptorInterfaceABC *IIColorCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int IIColorCS::
SetDescriptorInterface(IIColorDescriptorInterfaceABC
		       *aIIColorDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aIIColorDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aIIColorDescriptorInterface;
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
int IIColorCS::StartEncode(void)
{

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"IIColorCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"IIColorCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  long* temp;
  int* pctbc;
  temp=new long[2];
  pctbc=new int[2];
  
  pctbc=m_DescriptorInterface->GetIIColor_Component();

  temp[0] = (long)pctbc[0];
  temp[1] = (long)pctbc[1];

  //the scheme stores the 2 components of the PCTBC(8bits)
  m_EncoderBuffer->PutBitsLong(temp[0],2);
  m_EncoderBuffer->PutBitsLong(temp[1],6);

  delete [] pctbc;
  delete [] temp;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int IIColorCS::StartDecode(void)
{

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"IIColorCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"IIColorCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  int* PCTBC;
  long temp;

  PCTBC = new int[2];

  m_DecoderBuffer->GetBitsLong(&temp,2);
  m_DecoderBuffer->TakeBits(2);
  PCTBC[0]=temp;

  m_DecoderBuffer->GetBitsLong(&temp,6);
  m_DecoderBuffer->TakeBits(6);
  PCTBC[1]=temp;

  // Put the data into the D/DSType
  GetDescriptorInterface()->SetIIColor_Component(PCTBC);

  delete [] PCTBC;

  // All done
  return 0;
}

//----------------------------------------------------------------------------
IIColorCSInterfaceABC *IIColorCS::GetInterface(void)
{ return &m_Interface; }



