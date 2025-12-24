///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric R&D Centre Europe
// P. Brasnett, W. Price
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ImageSignatureCS.cpp
//
///////////////////////////////////////////////////////////////////////////////


#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef _VISUALC_
#include <netinet/in.h>
#endif

using namespace XM;

const int IS_min_feature_cnt = 32;
const int IS_max_feature_cnt = 80;
const int IS_feature_bytes = 11;	// 1-x,1-y, 1-direction, 8-LocalSignature 

const XM::UUID ImageSignatureCSInterface::myID = XM::UUID("");
const char *ImageSignatureCSInterface::myName = "Image Signature Coding Engine Interface";

const XM::UUID ImageSignatureCS::myID = XM::UUID("");
const char *ImageSignatureCS::myName = "Image Signature Coding Scheme";

const XM::UUID ImageSignatureCS::valID = XM::UUID("");
const char *ImageSignatureCS::valName = "Image Signature Coding Scheme";

const XM::UUID ImageSignatureCSInterfaceABC::myID = XM::UUID("");

//=============================================================================
ImageSignatureCSInterfaceABC::ImageSignatureCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
ImageSignatureCSInterfaceABC *ImageSignatureCS::GetInterface(void)
{
  return &m_Interface;
}

//=============================================================================
ImageSignatureCSInterface::~ImageSignatureCSInterface()
{
}

//-----------------------------------------------------------------------------
ImageSignatureCSInterface::ImageSignatureCSInterface(ImageSignatureCS *pImageSignature) :
m_pImageSignatureCS(pImageSignature)
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
const XM::UUID& ImageSignatureCSInterface::GetInterfaceID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ImageSignatureCSInterface::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
void ImageSignatureCSInterface::destroy(void)
{
  delete m_pImageSignatureCS;
}

//-----------------------------------------------------------------------------
BitBuffer *ImageSignatureCSInterface::GetEncoderStreamBuffer()
{
  return m_pImageSignatureCS->GetEncoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int ImageSignatureCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pImageSignatureCS->SetEncoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
BitBuffer *ImageSignatureCSInterface::GetDecoderStreamBuffer()
{
  return m_pImageSignatureCS->GetDecoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int ImageSignatureCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pImageSignatureCS->SetDecoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
ImageSignatureDescriptorInterfaceABC *ImageSignatureCSInterface::GetDescriptorInterface(void)
{
  return m_pImageSignatureCS->GetDescriptorInterface();
}

//-----------------------------------------------------------------------------
int ImageSignatureCSInterface::SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC *pImageSignatureDescriptorInterface)
{
  return m_pImageSignatureCS->SetDescriptorInterface(pImageSignatureDescriptorInterface);
}

//-----------------------------------------------------------------------------
int ImageSignatureCSInterface::StartEncode(void)
{
  return m_pImageSignatureCS->StartEncode();
}

//-----------------------------------------------------------------------------
int ImageSignatureCSInterface::StartDecode(void)
{
  return m_pImageSignatureCS->StartDecode();
}

//=============================================================================
ImageSignatureCS::ImageSignatureCS()
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pImageSignatureDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
ImageSignatureCS::ImageSignatureCS(ImageSignatureDescriptorInterfaceABC
                               *imagesignature)
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pImageSignatureDescriptorInterface(0)
{
  if (!imagesignature)
  {
    ImageSignatureDescriptor *descriptor = new ImageSignatureDescriptor();
    imagesignature = descriptor->GetInterface();
  }

  SetDescriptorInterface(imagesignature);

  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
ImageSignatureCS::~ImageSignatureCS()
{
  if (m_pImageSignatureDescriptorInterface)
    m_pImageSignatureDescriptorInterface->release();
}

//-----------------------------------------------------------------------------
const XM::UUID& ImageSignatureCS::GetObjectID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ImageSignatureCS::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
const XM::UUID& ImageSignatureCS::GetValueID(void)
{
  return valID;
}

//-----------------------------------------------------------------------------
const char* ImageSignatureCS::GetValueName(void)
{
  return valName;
}

//-----------------------------------------------------------------------------
bool ImageSignatureCS::IsProprietary(void)
{
  return m_isProprietary;
}

//-----------------------------------------------------------------------------
BitBuffer *ImageSignatureCS::GetEncoderStreamBuffer()
{
  return m_EncoderBuffer;
}

//-----------------------------------------------------------------------------
int ImageSignatureCS::SetEncoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_EncoderBuffer = pBuffer;
  return 0;
}

//-----------------------------------------------------------------------------
BitBuffer *ImageSignatureCS::GetDecoderStreamBuffer()
{
  return m_DecoderBuffer;
}

//-----------------------------------------------------------------------------
int ImageSignatureCS::SetDecoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_DecoderBuffer = pBuffer;
  return 0;
}

//-----------------------------------------------------------------------------
ImageSignatureDescriptorInterfaceABC *ImageSignatureCS::GetDescriptorInterface(void)
{
  return m_pImageSignatureDescriptorInterface;
}

//-----------------------------------------------------------------------------
int ImageSignatureCS::SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC *pImageSignatureDescriptorInterface)
{
  if (m_pImageSignatureDescriptorInterface ==
      pImageSignatureDescriptorInterface) return 0;

  if (m_pImageSignatureDescriptorInterface)
    m_pImageSignatureDescriptorInterface->release();

  m_pImageSignatureDescriptorInterface = pImageSignatureDescriptorInterface;
  if (m_pImageSignatureDescriptorInterface)
  {
    m_pImageSignatureDescriptorInterface->addref();
  }
  else
  {
    return -1;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int ImageSignatureCS::StartEncode(void)
{
  // check if Descriptor and stream buffers are linked to coding scheme
  if (!m_EncoderBuffer || !m_pImageSignatureDescriptorInterface)
  {
    return -1;
  }

  ImageSignatureGlobalBits bits;
  m_pImageSignatureDescriptorInterface->GetGlobalSignature(bits);
  m_EncoderBuffer->PutBits((char *)&bits[0], IMAGESIGNATURE_BITS_GLOBAL);

  ImageSignatureGlobalBits a_bits;
  m_pImageSignatureDescriptorInterface->GetAspectSignature(a_bits);
  m_EncoderBuffer->PutBits((char *)&a_bits[0], IMAGESIGNATURE_BITS_GLOBAL);

#if 1
  int i;
  long feature_cnt=0;
  ImageSignatureBits f_bits;
  m_pImageSignatureDescriptorInterface->GetFeatureSignature(f_bits);

  for(i = IS_min_feature_cnt; i < IS_max_feature_cnt; i++)
  {
	  int x = f_bits[i*IS_feature_bytes];
	  if(x == 255)
		  break;
	  feature_cnt++;
  }

  m_EncoderBuffer->PutBitsLong(feature_cnt, 8);

  feature_cnt += IS_min_feature_cnt;

  for(i = 0; i < feature_cnt; i++)
  {
	  long Xcoord = f_bits[i*IS_feature_bytes];
	  long Ycoord = f_bits[i*IS_feature_bytes+1];
	  long Direction = f_bits[i*IS_feature_bytes+2];
	  char *LocalSignature = (char*)&f_bits[i*IS_feature_bytes+3];
	  m_EncoderBuffer->PutBitsLong(Xcoord, 8);
	  m_EncoderBuffer->PutBitsLong(Ycoord, 8);
	  m_EncoderBuffer->PutBitsLong(Direction, 4);
	  m_EncoderBuffer->PutBits(LocalSignature, 60);
  }

#else
  unsigned char feature_cnt=(IMAGESIGNATURE_BITS/88)-32;
  m_EncoderBuffer->PutBits((char *)&feature_cnt, 8);
	
  ImageSignatureBits f_bits;
  m_pImageSignatureDescriptorInterface->GetFeatureSignature(f_bits);
  m_EncoderBuffer->PutBits((char *)&f_bits[0], IMAGESIGNATURE_BITS);
#endif // 

  return 0;
} // StartEncode

//-----------------------------------------------------------------------------
int ImageSignatureCS::StartDecode(void)
{
  if (!m_DecoderBuffer || !m_pImageSignatureDescriptorInterface)
    return -1;

  ImageSignatureGlobalBits bits;
  m_DecoderBuffer->GetBits((char *)&bits[0], IMAGESIGNATURE_BITS_GLOBAL);
  m_DecoderBuffer->TakeBits(IMAGESIGNATURE_BITS_GLOBAL);
  m_pImageSignatureDescriptorInterface->SetGlobalSignature(bits);

  ImageSignatureGlobalBits a_bits;
  m_DecoderBuffer->GetBits((char *)&a_bits[0], IMAGESIGNATURE_BITS_GLOBAL);
  m_DecoderBuffer->TakeBits(IMAGESIGNATURE_BITS_GLOBAL);
  m_pImageSignatureDescriptorInterface->SetAspectSignature(a_bits);
#if 1
  int i;
  long feature_cnt=0;
  ImageSignatureBits f_bits;
  memset(f_bits, 0, sizeof(f_bits));

  m_DecoderBuffer->GetBitsLong(&feature_cnt, 8);
  m_DecoderBuffer->TakeBits(8);

  if(feature_cnt > 48)
	  return(-1);

  feature_cnt += IS_min_feature_cnt;

  for(i = 0; i < feature_cnt; i++)
  {
	  long Xcoord;
	  long Ycoord;
	  long Direction;
	  char *LocalSignature;
	  m_DecoderBuffer->GetBitsLong(&Xcoord, 8);
	  m_DecoderBuffer->TakeBits(8);
	  m_DecoderBuffer->GetBitsLong(&Ycoord, 8);
	  m_DecoderBuffer->TakeBits(8);
	  m_DecoderBuffer->GetBitsLong(&Direction, 4);
	  m_DecoderBuffer->TakeBits(4);

	  f_bits[i*IS_feature_bytes] = (unsigned char)Xcoord;
	  f_bits[i*IS_feature_bytes+1] = (unsigned char)Ycoord;
	  f_bits[i*IS_feature_bytes+2] = (unsigned char)Direction;
	  LocalSignature = (char*)&f_bits[i*IS_feature_bytes+3];
	  m_DecoderBuffer->GetBits(LocalSignature, 60);
	  m_DecoderBuffer->TakeBits(60);
  }
  // Set the remaining values as in the extraction module: 255/255/0. This will be skipped in the search module.
  for(; i < IS_max_feature_cnt; i++)
  {
	  f_bits[i*IS_feature_bytes] = 255;
	  f_bits[i*IS_feature_bytes+1] = 255;
	  f_bits[i*IS_feature_bytes+2] = 0;
  }
  m_pImageSignatureDescriptorInterface->SetFeatureSignature(f_bits);
#else
  unsigned char feature_cnt;
  m_DecoderBuffer->GetBits((char *)&feature_cnt, 8);
  m_DecoderBuffer->TakeBits(8);

  ImageSignatureBits f_bits;
  m_DecoderBuffer->GetBits((char *)&f_bits[0], IMAGESIGNATURE_BITS);
  m_DecoderBuffer->TakeBits(IMAGESIGNATURE_BITS);
  m_pImageSignatureDescriptorInterface->SetFeatureSignature(f_bits);
#endif // 

  return 0;
}
