///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Tisserand Patrice
// IRCAM
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
// Applicable File Name:  HarmonicInstrumentTimbreCS.cpp
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

const UUID HarmonicInstrumentTimbreCSInterface::myID = UUID("");
const char *HarmonicInstrumentTimbreCSInterface::myName =
                         "HarmonicInstrumentTimbreCodingSchemeInterface";

// create a UUID for your type
const UUID HarmonicInstrumentTimbreCS::myID = UUID("to be created with xx");
const char *HarmonicInstrumentTimbreCS::myName = "HarmonicInstrumentTimbreCodingScheme";

const UUID HarmonicInstrumentTimbreCS::valID = UUID("");
const char *HarmonicInstrumentTimbreCS::valName = "";

const UUID HarmonicInstrumentTimbreCSInterfaceABC::myID = UUID("");

HarmonicInstrumentTimbreCSInterfaceABC::HarmonicInstrumentTimbreCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
HarmonicInstrumentTimbreCSInterface::HarmonicInstrumentTimbreCSInterface(HarmonicInstrumentTimbreCS *aHarmonicInstrumentTimbre):
m_HarmonicInstrumentTimbreCS(aHarmonicInstrumentTimbre)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreCSInterface::~HarmonicInstrumentTimbreCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void HarmonicInstrumentTimbreCSInterface::destroy(void)
{ delete m_HarmonicInstrumentTimbreCS; }

//----------------------------------------------------------------------------
BitBuffer *HarmonicInstrumentTimbreCSInterface::GetEncoderStreamBuffer(void)
{
  return m_HarmonicInstrumentTimbreCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_HarmonicInstrumentTimbreCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *HarmonicInstrumentTimbreCSInterface::GetDecoderStreamBuffer(void)
{
  return m_HarmonicInstrumentTimbreCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_HarmonicInstrumentTimbreCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC
*HarmonicInstrumentTimbreCSInterface::GetDescriptorInterface(void)

{
  return m_HarmonicInstrumentTimbreCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCSInterface::
SetDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aHarmonicInstrumentTimbreDescriptorInterface)
{
  return m_HarmonicInstrumentTimbreCS->
    SetDescriptorInterface(aHarmonicInstrumentTimbreDescriptorInterface);
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCSInterface::StartEncode(void)
{
  return m_HarmonicInstrumentTimbreCS->StartEncode();
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCSInterface::StartDecode(void)
{
  return m_HarmonicInstrumentTimbreCS->StartDecode();
}
//=============================================================================
HarmonicInstrumentTimbreCS::HarmonicInstrumentTimbreCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  /* create sub CS and
     connect them with descriptor if availabe*/
  SubHarmonicInstrumentTimbreACS *aSubHarmonicInstrumentTimbreACS= new SubHarmonicInstrumentTimbreACS();
  SubHarmonicInstrumentTimbreBCS *aSubHarmonicInstrumentTimbreBCS= new SubHarmonicInstrumentTimbreBCS();

  /* connect sub CS with HarmonicInstrumentTimbreCS*/
  if (m_SubHarmonicInstrumentTimbreACS)
    m_SubHarmonicInstrumentTimbreACS=aSubHarmonicInstrumentTimbreACS->GetInterface();
  //    SetSubHarmonicInstrumentTimbreACSInterface(aSubHarmonicInstrumentTimbreACS->GetInterface());
  if (m_SubHarmonicInstrumentTimbreBCS)
    m_SubHarmonicInstrumentTimbreBCS=aSubHarmonicInstrumentTimbreBCS->GetInterface();
  //  SetSubHarmonicInstrumentTimbreBCSInterface(aSubHarmonicInstrumentTimbreBCS->GetInterface());
#endif /* __HasSubTypes*/

  SetExposedInterface(&m_Interface);
}
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
#endif /* __HasSubTypes*/

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreCS::HarmonicInstrumentTimbreCS(HarmonicInstrumentTimbreDescriptorInterfaceABC* aHarmonicInstrumentTimbre):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aHarmonicInstrumentTimbre) {
      HarmonicInstrumentTimbreDescriptor *descriptor =
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
	  new HarmonicInstrumentTimbreDescriptor(0,0); /* one argument for each sub-component*/
#else
      new HarmonicInstrumentTimbreDescriptor;
#endif
      aHarmonicInstrumentTimbre=descriptor->GetInterface();
  }

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub CS and
     connect them with descriptor if availabe*/
  {
    SubHarmonicInstrumentTimbreACS *aSubHarmonicInstrumentTimbreACS;

    if (HarmonicInstrumentTimbre->GetSubHarmonicInstrumentTimbreADescriptorInterface()) {
      aSubHarmonicInstrumentTimbreACS= new
	SubHarmonicInstrumentTimbreACS(HarmonicInstrumentTimbre->GetSubHarmonicInstrumentTimbreADescriptorInterface());
    }
    else { 
      aSubHarmonicInstrumentTimbreACS= new SubHarmonicInstrumentTimbreACS();
    }
    if (aSubHarmonicInstrumentTimbreACS) {
      m_SubHarmonicInstrumentTimbreACS=aSubHarmonicInstrumentTimbreACS->GetInterface();
      //      SetSubHarmonicInstrumentTimbreACSInterface(m_SubHarmonicInstrumentTimbreACS);
    }
  }

  {
    SubHarmonicInstrumentTimbreBCS *aSubHarmonicInstrumentTimbreBCS;

    if (HarmonicInstrumentTimbre->GetSubHarmonicInstrumentTimbreBDescriptorInterface()) {
      aSubHarmonicInstrumentTimbreBCS= new
	SubHarmonicInstrumentTimbreBCS(HarmonicInstrumentTimbre->GetSubHarmonicInstrumentTimbreBDescriptorInterface());
    }
    else { 
      aSubHarmonicInstrumentTimbreBCS= new SubHarmonicInstrumentTimbreBCS();
    }
    if (aSubHarmonicInstrumentTimbreBCS) {
      m_SubHarmonicInstrumentTimbreBCS=aSubHarmonicInstrumentTimbreBCS->GetInterface();
      //      SetSubHarmonicInstrumentTimbreBCSInterface(m_SubHarmonicInstrumentTimbreBCS);
    }
  }
#endif /* __HasSubTypes*/

  /* connect descritors with CS*/
  SetDescriptorInterface(aHarmonicInstrumentTimbre);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreCS::~HarmonicInstrumentTimbreCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  if (m_SubHarmonicInstrumentTimbreACS) m_SubHarmonicInstrumentTimbreACS->destroy();
  if (m_SubHarmonicInstrumentTimbreBCS) m_SubHarmonicInstrumentTimbreBCS->destroy();
#endif /* __HasSubTypes*/
}

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HarmonicInstrumentTimbreCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& HarmonicInstrumentTimbreCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* HarmonicInstrumentTimbreCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool HarmonicInstrumentTimbreCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * HarmonicInstrumentTimbreCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  if (m_SubHarmonicInstrumentTimbreACS) m_SubHarmonicInstrumentTimbreACS->SetEncoderStreamBuffer(aBuffer);
  if (m_SubHarmonicInstrumentTimbreBCS) m_SubHarmonicInstrumentTimbreBCS->SetEncoderStreamBuffer(aBuffer);
#endif /* __HasSubTypes*/
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * HarmonicInstrumentTimbreCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
  if (m_SubHarmonicInstrumentTimbreACS) m_SubHarmonicInstrumentTimbreACS->SetDecoderStreamBuffer(aBuffer);
  if (m_SubHarmonicInstrumentTimbreBCS) m_SubHarmonicInstrumentTimbreBCS->SetDecoderStreamBuffer(aBuffer);
#endif /* __HasSubTypes*/

  return 0;
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreDescriptorInterfaceABC *HarmonicInstrumentTimbreCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int HarmonicInstrumentTimbreCS::
SetDescriptorInterface(HarmonicInstrumentTimbreDescriptorInterfaceABC
		       *aHarmonicInstrumentTimbreDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aHarmonicInstrumentTimbreDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aHarmonicInstrumentTimbreDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* conect sub descriptors with sub CS*/
    if ((m_DescriptorInterface->
	 GetSubHarmonicInstrumentTimbreADescriptorInterface()) &&
	m_SubHarmonicInstrumentTimbreACS) {
      m_SubHarmonicInstrumentTimbreACS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetSubHarmonicInstrumentTimbreADescriptorInterface());
    }
    if ((m_DescriptorInterface->
	 GetSubHarmonicInstrumentTimbreBDescriptorInterface()) &&
	m_SubHarmonicInstrumentTimbreBCS) {
      m_SubHarmonicInstrumentTimbreBCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetSubHarmonicInstrumentTimbreBDescriptorInterface());
    }
#endif /* __HasSubTypes*/
  }
  else {
#ifdef __HasSubTypes /*include this section if sub descriptors exist,
		       remove this section if no sub-descriptors exist*/
    /* if no descriptor, release also descriptor interfaces from
       sub CS*/
    m_SubHarmonicInstrumentTimbreACS->SetDescriptorInterface(0);
    m_SubHarmonicInstrumentTimbreBCS->SetDescriptorInterface(0);
#endif /* __HasSubTypes*/
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int HarmonicInstrumentTimbreCS::StartEncode(void)
{
  double value;
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"HarmonicInstrumentTimbreCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"HarmonicInstrumentTimbreCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  // Use a size of (8 * sizeof(double)) bits to store double value
  value = GetDescriptorInterface()->GetHarmonicSpectralCentroid();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetHarmonicSpectralDeviation();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetHarmonicSpectralSpread();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetHarmonicSpectralVariation();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));

  value = GetDescriptorInterface()->GetLogAttackTime();
  m_EncoderBuffer->PutBits((char *)&value , 8 * sizeof(double));


  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int HarmonicInstrumentTimbreCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"HarmonicInstrumentTimbreCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"HarmonicInstrumentTimbreCS::StartDecode: Interface not initialised\n");
    return -1;
  }



  /* style for reading the bits interpreted as nummeric data type*/
  double value;
  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetHarmonicSpectralCentroid(value);
  
  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetHarmonicSpectralDeviation(value);

  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetHarmonicSpectralSpread(value);
  
  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetHarmonicSpectralVariation(value);

  m_DecoderBuffer->GetBits((char *)&value , 8 * sizeof(double));
  m_DecoderBuffer->TakeBits(8 * sizeof(double));
  GetDescriptorInterface()->SetLogAttackTime(value);



  // All done
  return 0;
}

//----------------------------------------------------------------------------
HarmonicInstrumentTimbreCSInterfaceABC *HarmonicInstrumentTimbreCS::GetInterface(void)
{ return &m_Interface; }



