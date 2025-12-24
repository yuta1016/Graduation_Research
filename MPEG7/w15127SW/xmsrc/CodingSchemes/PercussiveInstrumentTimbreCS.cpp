///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Dummy author
// Dummy company
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
// Applicable File Name:  PercussiveInstrumentTimbreCS.cpp
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

const UUID PercussiveInstrumentTimbreCSInterface::myID = UUID("");
const char *PercussiveInstrumentTimbreCSInterface::myName =
                         "PercussiveInstrumentTimbreCodingSchemeInterface";

// create a UUID for your type
const UUID PercussiveInstrumentTimbreCS::myID = UUID("to be created with xx");
const char *PercussiveInstrumentTimbreCS::myName = "PercussiveInstrumentTimbreCodingScheme";

const UUID PercussiveInstrumentTimbreCS::valID = UUID("");
const char *PercussiveInstrumentTimbreCS::valName = "";

const UUID PercussiveInstrumentTimbreCSInterfaceABC::myID = UUID("");

PercussiveInstrumentTimbreCSInterfaceABC::PercussiveInstrumentTimbreCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
PercussiveInstrumentTimbreCSInterface::PercussiveInstrumentTimbreCSInterface(PercussiveInstrumentTimbreCS *aPercussiveInstrumentTimbre):
m_PercussiveInstrumentTimbreCS(aPercussiveInstrumentTimbre)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreCSInterface::~PercussiveInstrumentTimbreCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void PercussiveInstrumentTimbreCSInterface::destroy(void)
{ delete m_PercussiveInstrumentTimbreCS; }

//----------------------------------------------------------------------------
BitBuffer *PercussiveInstrumentTimbreCSInterface::GetEncoderStreamBuffer(void)
{
  return m_PercussiveInstrumentTimbreCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_PercussiveInstrumentTimbreCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *PercussiveInstrumentTimbreCSInterface::GetDecoderStreamBuffer(void)
{
  return m_PercussiveInstrumentTimbreCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_PercussiveInstrumentTimbreCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC
*PercussiveInstrumentTimbreCSInterface::GetDescriptorInterface(void)

{
  return m_PercussiveInstrumentTimbreCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCSInterface::
SetDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aPercussiveInstrumentTimbreDescriptorInterface)
{
  return m_PercussiveInstrumentTimbreCS->
    SetDescriptorInterface(aPercussiveInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCSInterface::StartEncode(void)
{
  return m_PercussiveInstrumentTimbreCS->StartEncode();
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCSInterface::StartDecode(void)
{
  return m_PercussiveInstrumentTimbreCS->StartDecode();
}
//=============================================================================
PercussiveInstrumentTimbreCS::PercussiveInstrumentTimbreCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{


  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreCS::PercussiveInstrumentTimbreCS(PercussiveInstrumentTimbreDescriptorInterfaceABC* aPercussiveInstrumentTimbre):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aPercussiveInstrumentTimbre) {
      PercussiveInstrumentTimbreDescriptor *descriptor =
	  new PercussiveInstrumentTimbreDescriptor;

      aPercussiveInstrumentTimbre=descriptor->GetInterface();
  }


  /* connect descritors with CS*/
  SetDescriptorInterface(aPercussiveInstrumentTimbre);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreCS::~PercussiveInstrumentTimbreCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *PercussiveInstrumentTimbreCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& PercussiveInstrumentTimbreCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* PercussiveInstrumentTimbreCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool PercussiveInstrumentTimbreCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * PercussiveInstrumentTimbreCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * PercussiveInstrumentTimbreCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreDescriptorInterfaceABC *PercussiveInstrumentTimbreCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int PercussiveInstrumentTimbreCS::
SetDescriptorInterface(PercussiveInstrumentTimbreDescriptorInterfaceABC
		       *aPercussiveInstrumentTimbreDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aPercussiveInstrumentTimbreDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aPercussiveInstrumentTimbreDescriptorInterface;
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
int PercussiveInstrumentTimbreCS::StartEncode(void)
{
  double value;
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"PercussiveInstrumentTimbreCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"PercussiveInstrumentTimbreCS::StartEncode: Interface not initialised\n");
    return -1;
  }


  // Use a size of (8 * sizeof(double)) bits to store double value
  value = GetDescriptorInterface()->GetSpectralCentroid();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetTemporalCentroid();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetLogAttackTime();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int PercussiveInstrumentTimbreCS::StartDecode(void)
{
    double value;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"PercussiveInstrumentTimbreCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"PercussiveInstrumentTimbreCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Read 32 bits the data in from the bit stream
  /* style for reading the bits interpreted as nummeric data type*/
  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetSpectralCentroid(value);
  
  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetTemporalCentroid(value);

  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetLogAttackTime(value);


  // All done
  return 0;
}

//----------------------------------------------------------------------------
PercussiveInstrumentTimbreCSInterfaceABC *PercussiveInstrumentTimbreCS::GetInterface(void)
{ return &m_Interface; }



