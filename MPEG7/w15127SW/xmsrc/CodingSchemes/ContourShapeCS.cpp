///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric
// ITE-VIL, W.Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeCS.cpp
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

const XM::UUID ContourShapeCSInterface::myID = XM::UUID("");
const char *ContourShapeCSInterface::myName = "Contour-Based Shape Coding Engine Interface";

const XM::UUID ContourShapeCS::myID = XM::UUID("");
const char *ContourShapeCS::myName = "Contour-Based Shape Coding Scheme";

const XM::UUID ContourShapeCS::valID = XM::UUID("");
const char *ContourShapeCS::valName = "Contour-Based Shape Coding Scheme";

const XM::UUID ContourShapeCSInterfaceABC::myID = XM::UUID("");

//=============================================================================
ContourShapeCSInterfaceABC::ContourShapeCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
ContourShapeCSInterfaceABC *ContourShapeCS::GetInterface(void)
{
  return &m_Interface;
}

//=============================================================================
ContourShapeCSInterface::~ContourShapeCSInterface()
{
}

//-----------------------------------------------------------------------------
ContourShapeCSInterface::ContourShapeCSInterface(ContourShapeCS *pContourShape) :
m_pContourShapeCS(pContourShape)
{
  SetInterfaceID(myID);
}

//-----------------------------------------------------------------------------
const XM::UUID& ContourShapeCSInterface::GetInterfaceID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ContourShapeCSInterface::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
void ContourShapeCSInterface::destroy(void)
{
  delete m_pContourShapeCS;
}

//-----------------------------------------------------------------------------
BitBuffer *ContourShapeCSInterface::GetEncoderStreamBuffer()
{
  return m_pContourShapeCS->GetEncoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int ContourShapeCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pContourShapeCS->SetEncoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
BitBuffer *ContourShapeCSInterface::GetDecoderStreamBuffer()
{
  return m_pContourShapeCS->GetDecoderStreamBuffer();
}

//-----------------------------------------------------------------------------
int ContourShapeCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_pContourShapeCS->SetDecoderStreamBuffer(aBuffer);
}

//-----------------------------------------------------------------------------
ContourShapeDescriptorInterfaceABC *ContourShapeCSInterface::GetDescriptorInterface(void)
{
  return m_pContourShapeCS->GetDescriptorInterface();
}

//-----------------------------------------------------------------------------
int ContourShapeCSInterface::SetDescriptorInterface(ContourShapeDescriptorInterfaceABC *pContourShapeDescriptorInterface)
{
  return m_pContourShapeCS->SetDescriptorInterface(pContourShapeDescriptorInterface);
}

//-----------------------------------------------------------------------------
int ContourShapeCSInterface::StartEncode(void)
{
  return m_pContourShapeCS->StartEncode();
}

//-----------------------------------------------------------------------------
int ContourShapeCSInterface::StartDecode(void)
{
  return m_pContourShapeCS->StartDecode();
}

//=============================================================================
ContourShapeCS::ContourShapeCS()
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pContourShapeDescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
ContourShapeCS::ContourShapeCS(ContourShapeDescriptorInterfaceABC
                               *contourshape)
: m_Interface(this)
, m_isProprietary(false)
, m_EncoderBuffer(0)
, m_DecoderBuffer(0)
, m_pContourShapeDescriptorInterface(0)
{
  if (!contourshape)
  {
    ContourShapeDescriptor *descriptor = new ContourShapeDescriptor();
    contourshape = descriptor->GetInterface();
  }

  SetDescriptorInterface(contourshape);

  SetExposedInterface(&m_Interface);
}

//-----------------------------------------------------------------------------
ContourShapeCS::~ContourShapeCS()
{
  if (m_pContourShapeDescriptorInterface)
    m_pContourShapeDescriptorInterface->release();
}

//-----------------------------------------------------------------------------
const XM::UUID& ContourShapeCS::GetObjectID(void)
{
  return myID;
}

//-----------------------------------------------------------------------------
const char *ContourShapeCS::GetName(void)
{
  return myName;
}

//-----------------------------------------------------------------------------
const XM::UUID& ContourShapeCS::GetValueID(void)
{
  return valID;
}

//-----------------------------------------------------------------------------
const char* ContourShapeCS::GetValueName(void)
{
  return valName;
}

//-----------------------------------------------------------------------------
bool ContourShapeCS::IsProprietary(void)
{
  return m_isProprietary;
}

//-----------------------------------------------------------------------------
BitBuffer *ContourShapeCS::GetEncoderStreamBuffer()
{
  return m_EncoderBuffer;
}

//-----------------------------------------------------------------------------
int ContourShapeCS::SetEncoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_EncoderBuffer = pBuffer;
  return 0;
}

//-----------------------------------------------------------------------------
BitBuffer *ContourShapeCS::GetDecoderStreamBuffer()
{
  return m_DecoderBuffer;
}

//-----------------------------------------------------------------------------
int ContourShapeCS::SetDecoderStreamBuffer(BitBuffer *pBuffer)
{
  if (!pBuffer) return -1;

  m_DecoderBuffer = pBuffer;
  return 0;
}

//-----------------------------------------------------------------------------
ContourShapeDescriptorInterfaceABC *ContourShapeCS::GetDescriptorInterface(void)
{
  return m_pContourShapeDescriptorInterface;
}

//-----------------------------------------------------------------------------
int ContourShapeCS::SetDescriptorInterface(ContourShapeDescriptorInterfaceABC *pContourShapeDescriptorInterface)
{
  if (m_pContourShapeDescriptorInterface ==
      pContourShapeDescriptorInterface) return 0;

  if (m_pContourShapeDescriptorInterface)
    m_pContourShapeDescriptorInterface->release();

  m_pContourShapeDescriptorInterface = pContourShapeDescriptorInterface;
  if (m_pContourShapeDescriptorInterface)
  {
    m_pContourShapeDescriptorInterface->addref();
  }
  else
  {
    return -1;
  }

  return 0;
}

//-----------------------------------------------------------------------------
int ContourShapeCS::StartEncode(void)
{
  // check if Descriptor and stream buffers are linked to coding scheme
  if (!m_EncoderBuffer || !m_pContourShapeDescriptorInterface)
  {
    return -1;
  }

  unsigned long lC, lE, lY;

  // CSS Peaks Count
  unsigned int nPeaks = m_pContourShapeDescriptorInterface->GetNoOfPeaks() & CONTOURSHAPE_CSSPEAKMASK;
  m_EncoderBuffer->PutBitsLong(nPeaks, CONTOURSHAPE_CSSPEAKBITS);

  // Global Curvature Vector
  m_pContourShapeDescriptorInterface->GetGlobalCurvature(lC, lE);
  m_EncoderBuffer->PutBitsLong((long)lC, CONTOURSHAPE_CBITS);
  m_EncoderBuffer->PutBitsLong((long)lE, CONTOURSHAPE_EBITS);
  if (nPeaks) 
  {
    // Prototype Curvature Vector
    m_pContourShapeDescriptorInterface->GetPrototypeCurvature(lC, lE);
    m_EncoderBuffer->PutBitsLong((long)lC, CONTOURSHAPE_CBITS);
    m_EncoderBuffer->PutBitsLong((long)lE, CONTOURSHAPE_EBITS);
  }

  // Highest Peak Y
  lY = m_pContourShapeDescriptorInterface->GetHighestPeakY();
  m_EncoderBuffer->PutBitsLong((long)lY, CONTOURSHAPE_YBITS);

  // Peak values
  for (unsigned int c = 1; c < nPeaks; c++)
  {
    unsigned short iX, iY;
    m_pContourShapeDescriptorInterface->GetPeak(c, iX, iY);
    m_EncoderBuffer->PutBitsLong((long)iX, CONTOURSHAPE_XBITS);
    m_EncoderBuffer->PutBitsLong((long)iY, CONTOURSHAPE_YnBITS);
  }

  return 0;
} // StartEncode

//-----------------------------------------------------------------------------
int ContourShapeCS::StartDecode(void)
{
  if (!m_DecoderBuffer || !m_pContourShapeDescriptorInterface)
    return -1;

  unsigned long lC, lE, lX, lY;

  // get peak count
  unsigned int nPeaks;
  m_DecoderBuffer->GetBitsLong((long *)&lX, CONTOURSHAPE_CSSPEAKBITS);
  m_DecoderBuffer->TakeBits(CONTOURSHAPE_CSSPEAKBITS);
  m_pContourShapeDescriptorInterface->SetNoOfPeaks(nPeaks = lX);

  // get global curvature vector
  m_DecoderBuffer->GetBitsLong((long *)&lC, CONTOURSHAPE_CBITS);
  m_DecoderBuffer->TakeBits(CONTOURSHAPE_CBITS);
  m_DecoderBuffer->GetBitsLong((long *)&lE, CONTOURSHAPE_EBITS);
  m_DecoderBuffer->TakeBits(CONTOURSHAPE_EBITS);
  m_pContourShapeDescriptorInterface->SetGlobalCurvature(lC, lE);
  if (nPeaks)
  {
    // get prototype curvature vector
    m_DecoderBuffer->GetBitsLong((long *)&lC, CONTOURSHAPE_CBITS);
    m_DecoderBuffer->TakeBits(CONTOURSHAPE_CBITS);
    m_DecoderBuffer->GetBitsLong((long *)&lE, CONTOURSHAPE_EBITS);
    m_DecoderBuffer->TakeBits(CONTOURSHAPE_EBITS);
    m_pContourShapeDescriptorInterface->SetPrototypeCurvature(lC, lE);
  }

  // highest peak y
  m_DecoderBuffer->GetBitsLong((long *)&lY, CONTOURSHAPE_YBITS);
  m_DecoderBuffer->TakeBits(CONTOURSHAPE_YBITS);
  m_pContourShapeDescriptorInterface->SetHighestPeakY((unsigned short)lY);

  // peak values
  m_pContourShapeDescriptorInterface->SetPeak(0, 0, lY);
  for (unsigned int c = 1; c < nPeaks; c++)
  {
    m_DecoderBuffer->GetBitsLong((long *)&lX, CONTOURSHAPE_XBITS);
    m_DecoderBuffer->TakeBits(CONTOURSHAPE_XBITS);
    m_DecoderBuffer->GetBitsLong((long *)&lY, CONTOURSHAPE_YnBITS);
    m_DecoderBuffer->TakeBits(CONTOURSHAPE_YnBITS);
    m_pContourShapeDescriptorInterface->SetPeak(c,
						(unsigned short)lX,
						(unsigned short)lY);
  }

  return 0;
}
