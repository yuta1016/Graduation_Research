///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  HomoTextureCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif
 

using namespace XM;

const UUID HomogeneousTextureCSInterface::myID = UUID("");
const char *HomogeneousTextureCSInterface::myName =
                         "HomogeneousTextureCodingSchemeInterface";

// create a UUID for your type
const UUID HomogeneousTextureCS::myID = UUID("to be created with xx");
const char *HomogeneousTextureCS::myName = "HomogeneousTextureCodingScheme";

const UUID HomogeneousTextureCS::valID = UUID("");
const char *HomogeneousTextureCS::valName = "";

const UUID HomogeneousTextureCSInterfaceABC::myID = UUID("");

HomogeneousTextureCSInterfaceABC::HomogeneousTextureCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//=============================================================================
HomogeneousTextureCSInterface::HomogeneousTextureCSInterface(HomogeneousTextureCS *aHomogeneousTexture):
m_HomogeneousTextureCS(aHomogeneousTexture)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HomogeneousTextureCSInterface::~HomogeneousTextureCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void HomogeneousTextureCSInterface::destroy(void)
{ delete m_HomogeneousTextureCS; }

//----------------------------------------------------------------------------
BitBuffer *HomogeneousTextureCSInterface::GetEncoderStreamBuffer(void)
{
  return m_HomogeneousTextureCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int HomogeneousTextureCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_HomogeneousTextureCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *HomogeneousTextureCSInterface::GetDecoderStreamBuffer(void)
{
  return m_HomogeneousTextureCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int HomogeneousTextureCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_HomogeneousTextureCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC
*HomogeneousTextureCSInterface::GetDescriptorInterface(void)

{
  return m_HomogeneousTextureCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int HomogeneousTextureCSInterface::
SetDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aHomogeneousTextureDescriptorInterface)
{
  return m_HomogeneousTextureCS->
    SetDescriptorInterface(aHomogeneousTextureDescriptorInterface);
}

//----------------------------------------------------------------------------
int HomogeneousTextureCSInterface::StartEncode(void)
{
  return m_HomogeneousTextureCS->StartEncode();
}

//----------------------------------------------------------------------------
int HomogeneousTextureCSInterface::StartDecode(void)
{
  return m_HomogeneousTextureCS->StartDecode();
}

//=============================================================================
HomogeneousTextureCS::HomogeneousTextureCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

  SetExposedInterface(&m_Interface);
}


//----------------------------------------------------------------------------
HomogeneousTextureCS::HomogeneousTextureCS(HomogeneousTextureDescriptorInterfaceABC* aHomogeneousTexture):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aHomogeneousTexture) {
    HomogeneousTextureDescriptor *descriptor =
      new HomogeneousTextureDescriptor;
      aHomogeneousTexture=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aHomogeneousTexture);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HomogeneousTextureCS::~HomogeneousTextureCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HomogeneousTextureCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& HomogeneousTextureCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* HomogeneousTextureCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool HomogeneousTextureCS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
BitBuffer * HomogeneousTextureCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int HomogeneousTextureCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * HomogeneousTextureCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int HomogeneousTextureCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  return 0;
}

//----------------------------------------------------------------------------
HomogeneousTextureDescriptorInterfaceABC *HomogeneousTextureCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int HomogeneousTextureCS::
SetDescriptorInterface(HomogeneousTextureDescriptorInterfaceABC
		       *aHomogeneousTextureDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aHomogeneousTextureDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aHomogeneousTextureDescriptorInterface;
  if (m_DescriptorInterface)
    m_DescriptorInterface->addref();
  else
	return -1;

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int HomogeneousTextureCS::StartEncode(void)
{
  //---yjyu - 010222
	long a;
  	int flag, c[62];
  //---

  // Is the file IO set up? If not return


  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"HomogeneousTextureCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"HomogeneousTextureCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  //---yjyu - 010222 ychoi - 010531
  	memcpy(c,m_DescriptorInterface->GetHomogeneousTextureFeature(),sizeof(int)*62);
    flag = m_DescriptorInterface->GetHomogeneousTextureFeatureFlag();
	if (flag != 0) {
		m_EncoderBuffer->PutBit(1);
		for(int i=0; i<62; i++) {
			a = c[i];
			m_EncoderBuffer->PutBitsLong(a,8UL);
		}
	}
	else { 
		m_EncoderBuffer->PutBit(0);
		for(int i=0; i<32; i++) {
			a = c[i];
			m_EncoderBuffer->PutBitsLong(a,8UL);
		}
	}

  //---

  // And now I'll dump it into the buffer. This says put the 32 bits
  // of the number stored in v to file.
  //yjyu - 010223  m_EncoderBuffer->PutBitsLong(v,32);

  /* style for puting the bits interpreted as some kind of data*/
  /* m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&v),8*sizeof(long));*/

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int HomogeneousTextureCS::StartDecode(void)
{
    long a;
    int flag;
    int feature[62];

	// Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"HomogeneousTextureCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"HomogeneousTextureCS::StartDecode: Interface not initialised\n");
    return -1;
  }

   flag = m_DecoderBuffer->GetBit(0)>>7;
   m_DecoderBuffer->TakeBits(1UL);
   for(int i=0;i<(32+30*flag); i++) {
     m_DecoderBuffer->GetBitsLong(&a,8UL);
     m_DecoderBuffer->TakeBits(8UL);
     feature[i] = (int)a;
   }

    m_DescriptorInterface->SetHomogeneousTextureFeature(flag, feature);

  // All done
  return 0;
}

//----------------------------------------------------------------------------
HomogeneousTextureCSInterfaceABC *HomogeneousTextureCS::GetInterface(void)
{ return &m_Interface; }



