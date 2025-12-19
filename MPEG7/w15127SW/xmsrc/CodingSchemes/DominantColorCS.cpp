///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, University of California, Santa Barbara.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorCS.cpp
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

const UUID DominantColorCSInterface::myID = UUID("");
const char *DominantColorCSInterface::myName =
                         "Dominant Color Coding Engine Interface";

const UUID DominantColorCS::myID = UUID("");
const char *DominantColorCS::myName = "Dominant Color Coding Scheme";

const UUID DominantColorCS::valID = UUID("");
const char *DominantColorCS::valName = "";

const UUID DominantColorCSInterfaceABC::myID = UUID("");

//=============================================================================
DominantColorCSInterfaceABC::DominantColorCSInterfaceABC()

{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorCSInterface::DominantColorCSInterface(DominantColorCS *aDominantColor):
  m_DominantColorCS(aDominantColor)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DominantColorCSInterface::~DominantColorCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& DominantColorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DominantColorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void DominantColorCSInterface::destroy(void)
{ delete m_DominantColorCS; }

//----------------------------------------------------------------------------
BitBuffer *DominantColorCSInterface::GetEncoderStreamBuffer(void)
{
  return m_DominantColorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_DominantColorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *DominantColorCSInterface::GetDecoderStreamBuffer(void)
{
  return m_DominantColorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_DominantColorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC *DominantColorCSInterface::
GetDescriptorInterface(void)
{
  return m_DominantColorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::
SetDescriptorInterface(DominantColorDescriptorInterfaceABC
		       *aDominantColorDescriptorInterface)
{
  return m_DominantColorCS->
    SetDescriptorInterface(aDominantColorDescriptorInterface);
}

//----------------------------------------------------------------------------
ColorSpaceCSInterfaceABC *DominantColorCSInterface::
GetColorSpaceCSInterface(void)

{
  return m_DominantColorCS->GetColorSpaceCSInterface();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::
SetColorSpaceCSInterface(ColorSpaceCSInterfaceABC *aColorSpaceCSInterface)

{
  return m_DominantColorCS->
    SetColorSpaceCSInterface(aColorSpaceCSInterface);
}

//----------------------------------------------------------------------------
ColorQuantizerCSInterfaceABC *DominantColorCSInterface::
GetColorQuantizerCSInterface(void)

{
  return m_DominantColorCS->GetColorQuantizerCSInterface();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::
SetColorQuantizerCSInterface(ColorQuantizerCSInterfaceABC 
			     *aColorQuantizerCSInterface)

{
  return m_DominantColorCS->
    SetColorQuantizerCSInterface(aColorQuantizerCSInterface);
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::StartEncode(void)
{
  return m_DominantColorCS->StartEncode();
}

//----------------------------------------------------------------------------
int DominantColorCSInterface::StartDecode(void)
{
  return m_DominantColorCS->StartDecode();
}

//=============================================================================

//----------------------------------------------------------------------------
DominantColorCS::DominantColorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DominantColorDescriptorInterface(0),
m_ColorSpaceCSInterface(0),
m_ColorQuantizerCSInterface(0)
{
  ColorSpaceCS *m_ColorSpaceCS;
  ColorQuantizerCS *m_ColorQuantizerCS;

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub coding schemes */

  m_ColorSpaceCS= new ColorSpaceCS();
  m_ColorQuantizerCS= new ColorQuantizerCS();

  /* connect sub coding schemes with color dc coding scheme*/
  if (m_ColorSpaceCS)
    SetColorSpaceCSInterface(m_ColorSpaceCS->GetInterface());
  if (m_ColorQuantizerCS)
    SetColorQuantizerCSInterface(m_ColorQuantizerCS->GetInterface());

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorCS::DominantColorCS(DominantColorDescriptorInterfaceABC *dc):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DominantColorDescriptorInterface(0),
m_ColorSpaceCSInterface(0),
m_ColorQuantizerCSInterface(0)
{
  ColorSpaceCS *m_ColorSpaceCS;
  ColorQuantizerCS *m_ColorQuantizerCS;

  /* create descriptor if it does not exist */
  if( !dc ) {
    DominantColorDescriptor *descriptor = new DominantColorDescriptor(0,0);
    dc = descriptor->GetInterface();
  }

  /* connect descriptor with coding scheme */
  SetDescriptorInterface(dc);

  /* create sub coding schemes and
     connect them with descriptor if availabe*/

  if (dc->GetColorSpaceDescriptorInterface()) {
    m_ColorSpaceCS= new ColorSpaceCS(dc->
				     GetColorSpaceDescriptorInterface());
  }
  else { 
    m_ColorSpaceCS= new ColorSpaceCS();
  }

  if (dc->GetColorQuantizerDescriptorInterface()) {
    m_ColorQuantizerCS= new
      ColorQuantizerCS(dc->GetColorQuantizerDescriptorInterface());
  }
  else { 
    m_ColorQuantizerCS= new ColorQuantizerCS();
  }

  /* connect sub coding schemes with color dc coding scheme*/
  if (m_ColorSpaceCS)
    SetColorSpaceCSInterface(m_ColorSpaceCS->GetInterface());
  if (m_ColorQuantizerCS)
    SetColorQuantizerCSInterface(m_ColorQuantizerCS->GetInterface());

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
DominantColorCS::~DominantColorCS()
{

  /* destroy sub coding schemes*/
  if (m_ColorSpaceCSInterface) m_ColorSpaceCSInterface->destroy();
  if (m_ColorQuantizerCSInterface)
    m_ColorQuantizerCSInterface->destroy();

  /* release descriptor */
  if( m_DominantColorDescriptorInterface )
    m_DominantColorDescriptorInterface->release();

}

//----------------------------------------------------------------------------
const UUID& DominantColorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DominantColorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& DominantColorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* DominantColorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool DominantColorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * DominantColorCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int DominantColorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  /* if sub coding schemes exist apply stream also to them*/
  if (m_ColorSpaceCSInterface) m_ColorSpaceCSInterface->
				 SetEncoderStreamBuffer(aBuffer);
  if (m_ColorQuantizerCSInterface) m_ColorQuantizerCSInterface->
				      SetEncoderStreamBuffer(aBuffer);

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * DominantColorCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int DominantColorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;

  /* connect stream buffer also with sub coding schemes*/
  if (m_ColorSpaceCSInterface) m_ColorSpaceCSInterface->
				 SetDecoderStreamBuffer(aBuffer);
  if (m_ColorQuantizerCSInterface) m_ColorQuantizerCSInterface->
				     SetDecoderStreamBuffer(aBuffer);

  return 0;
}

//----------------------------------------------------------------------------
DominantColorDescriptorInterfaceABC *DominantColorCS::GetDescriptorInterface(void)
{
  return m_DominantColorDescriptorInterface;
}

//----------------------------------------------------------------------------
int DominantColorCS::
SetDescriptorInterface(DominantColorDescriptorInterfaceABC
	      *aDominantColorDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DominantColorDescriptorInterface == aDominantColorDescriptorInterface)
    return 0;
	
  /* release old interface*/
  if (m_DominantColorDescriptorInterface) 
    m_DominantColorDescriptorInterface->release();

  /* add new interface*/
  m_DominantColorDescriptorInterface = aDominantColorDescriptorInterface;
  if (m_DominantColorDescriptorInterface) {
    m_DominantColorDescriptorInterface->addref();

    /* conect sub descriptors with sub coding schemes*/
    if ((m_DominantColorDescriptorInterface->
	 GetColorSpaceDescriptorInterface()) &&
	m_ColorSpaceCSInterface) {
      m_ColorSpaceCSInterface->
	SetDescriptorInterface(m_DominantColorDescriptorInterface->
			       GetColorSpaceDescriptorInterface());
    }
    if ((m_DominantColorDescriptorInterface->
	 GetColorQuantizerDescriptorInterface()) &&
	m_ColorQuantizerCSInterface) {
      m_ColorQuantizerCSInterface->
	SetDescriptorInterface(m_DominantColorDescriptorInterface->
			       GetColorQuantizerDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub coding schemes*/
    m_ColorSpaceCSInterface->SetDescriptorInterface(0);
    m_ColorQuantizerCSInterface->SetDescriptorInterface(0);
    return -1;
  }
  
  return 0;
}

//----------------------------------------------------------------------------
ColorSpaceCSInterfaceABC *DominantColorCS::GetColorSpaceCSInterface(void)

{
  return m_ColorSpaceCSInterface;
}

//----------------------------------------------------------------------------
int DominantColorCS::SetColorSpaceCSInterface(ColorSpaceCSInterfaceABC
						*aColorSpaceCSInterface)

{
  if (!aColorSpaceCSInterface) return -1;

  m_ColorSpaceCSInterface = aColorSpaceCSInterface;
  return 0;
}

//----------------------------------------------------------------------------
ColorQuantizerCSInterfaceABC *DominantColorCS::
GetColorQuantizerCSInterface(void)

{
  return m_ColorQuantizerCSInterface;
}

//----------------------------------------------------------------------------
int DominantColorCS::
SetColorQuantizerCSInterface(ColorQuantizerCSInterfaceABC
			     *aColorQuantizerCSInterface)

{
  if (!aColorQuantizerCSInterface) return -1;

  m_ColorQuantizerCSInterface = aColorQuantizerCSInterface;
  return 0;
}

//----------------------------------------------------------------------------
int DominantColorCS::StartEncode(void)
{
  dc_t *DC;
  int  dc_number;
  bool colspcpres;
  bool colquantpres;
  bool varpres;
  int  itmp;

  /* check if output buffer exists */
  if( !m_EncoderBuffer )
    return -1;

  /* check if Descriptor and stream buffers are linked
     to coding scheme*/
  if( !m_EncoderBuffer || !m_DominantColorDescriptorInterface )
    return -1;

  /* check values for Color Space **************************/ 
  /* check if color space descriptor is linked to dc*/
  if (!m_DominantColorDescriptorInterface->
      GetColorSpaceDescriptorInterface()) {
    return -1;
  }

  /* check if color space descritor and stream buffer are linked
     to color space coding scheme*/
  if (!m_ColorSpaceCSInterface->GetDescriptorInterface() ||
      !m_ColorSpaceCSInterface->GetEncoderStreamBuffer()) return -1;

  /* check if color space descritor attached to
     color dc descriptor
     is equal to color space descritor attached to
     color space coding */
  if (m_ColorSpaceCSInterface->GetDescriptorInterface() !=
      m_DominantColorDescriptorInterface->
      GetColorSpaceDescriptorInterface()) return -1;

  /* check values for Color Qunatizer ***************************/ 
  /* check if color quantizeer descriptor is linked to dc*/
  if (!m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()) {
    return -1;
  }

  /* check if color quantizer descritor and stream buffer are linked
     to color space coding scheme*/
  if (!m_ColorQuantizerCSInterface->GetDescriptorInterface() ||
      !m_ColorQuantizerCSInterface->GetEncoderStreamBuffer()) return -1;

  /* check if color quantizer descritor attached to
     color dc descriptor
     is equal to color quantizer descritor attached to
     color quantizer coding */
  if (m_ColorQuantizerCSInterface->GetDescriptorInterface() !=
      m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()) return -1;

  /*** write to the bitstream ***/

  /* number of colours */
  dc_number = m_DominantColorDescriptorInterface->GetDominantColorsNumber();
  m_EncoderBuffer->PutBitsLong((long)(dc_number-1), 3);

  colspcpres = m_DominantColorDescriptorInterface->GetColorSpacePresent();
  m_EncoderBuffer->PutBitsLong(colspcpres, 1);

  if(colspcpres)     {
    /* encode color space description*/
    m_ColorSpaceCSInterface->StartEncode();
  }

  colquantpres = m_DominantColorDescriptorInterface->GetColorQuantizationPresent();
  m_EncoderBuffer->PutBitsLong(colquantpres, 1);

  if(colquantpres) {
    /* encode color quantizer description*/
    m_ColorQuantizerCSInterface->StartEncode();
  }

  /* variance indicator */
  varpres = m_DominantColorDescriptorInterface->GetVariancePresent();
  m_EncoderBuffer->PutBitsLong((long)varpres, 1);

  /* spatial coherence */
  itmp = m_DominantColorDescriptorInterface->GetSpatialCoherency();
  m_EncoderBuffer->PutBitsLong((long)itmp, 5);

  /* calculate bits for ColorValueIndex */

  long a;
  int bits0, bits1, bits2;
  int component0, component1, component2;

  component0 = 0;
  component1 = 1;
  component2 = 2; // default RGB

  if(colspcpres) {
    switch(m_DominantColorDescriptorInterface->
	   GetColorSpaceDescriptorInterface()->
	   GetDescriptionColorSpace())
      {
      case RGB:
	component0 = 0; // R
	component1 = 1; // G
	component2 = 2; // B
	break;
      case YCRCB:
	component0 = 3; // Y
	component1 = 4; // Cb
	component2 = 5; // Cr
	break;
      case HSV:
	component0 = 6; // H
	component1 = 7; // S
	component2 = 8; // V
	break;
      case HMMD:
	component0 = 6; // H
	component1 = 12; // Sum
	component2 = 11; // Diff
	break;
      default:
	break;
      }
    }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component0)-1;
  }
  else a = 31;

  for(bits0=12; bits0>0; bits0--) {
    if(a & 0x800) break;
    a<<=1;
  }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component1)-1;
  }
  else a = 31;

  for(bits1=12; bits1>0; bits1--)
    {
      if(a & 0x800) break;
      a<<=1;
    }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component2)-1;
  }
  else a = 31;

  for(bits2=12; bits2>0; bits2--)
    {
      if(a & 0x800) break;
      a<<=1;
    }

  /* dominant colours */
  DC = m_DominantColorDescriptorInterface->GetDominantColors();
  for(int i=0; i<dc_number; i++) {
    m_EncoderBuffer->PutBitsLong((long)(DC[i].weight), 5);
    m_EncoderBuffer->PutBitsLong((long)(DC[i].centroid[0]), bits0);
    m_EncoderBuffer->PutBitsLong((long)(DC[i].centroid[1]), bits1);
    m_EncoderBuffer->PutBitsLong((long)(DC[i].centroid[2]), bits2);
    if( varpres ) {
      m_EncoderBuffer->PutBitsLong((long)(DC[i].variance[0]), 1);
      m_EncoderBuffer->PutBitsLong((long)(DC[i].variance[1]), 1);
      m_EncoderBuffer->PutBitsLong((long)(DC[i].variance[2]), 1);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
int DominantColorCS::StartDecode(void)
{
  int dc_number;
  bool colspcpres;
  bool colquantpres;
  bool varpres;
  int *percents, **colors, **variances;
  long ltmp;

  /* check if output buffer exists */
  if( !m_DecoderBuffer )
    return -1;

  /* check if descriptor and stream buffers are linked
     to coding scheme*/
  if( !m_DecoderBuffer || !m_DominantColorDescriptorInterface )
    return -1;

  /* check values for Color Space **************************/ 
  /* check if color space descriptor is linked to dc*/
  if (!m_DominantColorDescriptorInterface->
      GetColorSpaceDescriptorInterface()) {
    return -1;
  }

  /* check if color space descritor and stream buffer are linked
     to color space coding scheme*/
  if (!m_ColorSpaceCSInterface->GetDescriptorInterface() ||
      !m_ColorSpaceCSInterface->GetDecoderStreamBuffer()) return -1;

  /* check if color space descritor attached to
     color dc descriptor
     is equal to color space descritor attached to
     color space coding */
  if (m_ColorSpaceCSInterface->GetDescriptorInterface() !=
      m_DominantColorDescriptorInterface->
      GetColorSpaceDescriptorInterface()) return -1;

  /* check values for Color Qunatizer ***************************/ 
  /* check if color quantizeer descriptor is linked to dc*/
  if (!m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()) {
    return -1;
  }

  /* check if color quantizer descritor and stream buffer are linked
     to color space coding scheme*/
  if (!m_ColorQuantizerCSInterface->GetDescriptorInterface() ||
      !m_ColorQuantizerCSInterface->GetDecoderStreamBuffer()) return -1;

  /* check if color quantizer descritor attached to
     color dc descriptor
     is equal to color quantizer descritor attached to
     color quantizer coding */
  if (m_ColorQuantizerCSInterface->GetDescriptorInterface() !=
      m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()) return -1;

  /*** read bitstream ***/

  /* get dominant colors number */
  m_DecoderBuffer->GetBitsLong(&ltmp, 3);
  m_DecoderBuffer->TakeBits(3);
  dc_number = (int)ltmp+1;
  m_DominantColorDescriptorInterface->SetDominantColorsNumber(dc_number);

  /* get color space present */
  m_DecoderBuffer->GetBitsLong(&ltmp, 1);
  m_DecoderBuffer->TakeBits(1);
  colspcpres = (bool)ltmp;
  m_DominantColorDescriptorInterface->SetColorSpacePresent(colspcpres);

  if(colspcpres) {
    /* decode color space description*/
    m_ColorSpaceCSInterface->StartDecode();
  }
  
  /* get color quantization present */
  m_DecoderBuffer->GetBitsLong(&ltmp, 1);
  m_DecoderBuffer->TakeBits(1);
  colquantpres = (bool)ltmp;
  m_DominantColorDescriptorInterface->
    SetColorQuantizationPresent(colquantpres);

  if(colquantpres) {
    /* decode color quantizer description*/
    m_ColorQuantizerCSInterface->StartDecode();
  }

  /* get variance indicator */
  m_DecoderBuffer->GetBitsLong(&ltmp, 1);
  m_DecoderBuffer->TakeBits(1);
  varpres = (bool)ltmp;
  m_DominantColorDescriptorInterface->SetVariancePresent(varpres);

  /* get spatial coherence */
  m_DecoderBuffer->GetBitsLong(&ltmp, 5);
  m_DecoderBuffer->TakeBits(5);
  m_DominantColorDescriptorInterface->SetSpatialCoherency((int)ltmp);

  long a;
  int bits0, bits1, bits2;
  int component0, component1, component2;

  component0 = 0;
  component1 = 1;
  component2 = 2;

  if(colspcpres) {
    switch(m_DominantColorDescriptorInterface->
	   GetColorSpaceDescriptorInterface()->
	   GetDescriptionColorSpace())
      {
      case RGB:
	component0 = 0; // R
	component1 = 1; // G
	component2 = 2; // B
	break;
      case YCRCB:
	component0 = 3; // Y
	component1 = 4; // Cb
	component2 = 5; // Cr
	break;
      case HSV:
	component0 = 6; // H
	component1 = 7; // S
	component2 = 8; // V
	break;
      case HMMD:
	component0 = 6; // H
	component1 = 12; // Sum
	component2 = 11; // Diff
	  break;
      default:
	break;
      }
  }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component0)-1;
      }
  else a = 31;

  for(bits0=12; bits0>0; bits0--)
    {
      if(a & 0x800) break;
      a<<=1;
    }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component1)-1;
  }
  else a = 31;

  for(bits1=12; bits1>0; bits1--) {
    if(a & 0x800) break;
    a<<=1;
  }

  if(colquantpres) {
    a = m_DominantColorDescriptorInterface->
      GetColorQuantizerDescriptorInterface()->
      GetBinNumberByComponent(component2)-1;
  }
  else a = 31;

  for(bits2=12; bits2>0; bits2--)
    {
      if(a & 0x800) break;
      a<<=1;
    }

  /* dominant colors */
  percents = new int[dc_number];
  colors = new int*[dc_number];
  {
    int i;
    for(i=0;i<dc_number;i++)
      colors[i]= new int[3];
  }
  if( varpres ) {
    variances = new int*[dc_number];
    {
      int i;
      for(i=0;i<dc_number;i++)
	variances[i]= new int[3];
    }
  }
  {
    int i;
    for(i=0; i<dc_number; i++ ) {
      m_DecoderBuffer->GetBitsLong(&ltmp, 5);
      m_DecoderBuffer->TakeBits(5);
      percents[i] = (int)ltmp;
      m_DecoderBuffer->GetBitsLong(&ltmp, bits0);
      m_DecoderBuffer->TakeBits(bits0);
      colors[i][0] = (int)ltmp;
      m_DecoderBuffer->GetBitsLong(&ltmp, bits1);
      m_DecoderBuffer->TakeBits(bits1);
      colors[i][1] = (int)ltmp;
      m_DecoderBuffer->GetBitsLong(&ltmp, bits2);
      m_DecoderBuffer->TakeBits(bits2);
      colors[i][2] = (int)ltmp;
      if( varpres ) {
	m_DecoderBuffer->GetBitsLong(&ltmp, 1);
	m_DecoderBuffer->TakeBits(1);
	variances[i][0] = (int)ltmp;
	m_DecoderBuffer->GetBitsLong(&ltmp, 1);
	m_DecoderBuffer->TakeBits(1);
	variances[i][1] = (int)ltmp;
	m_DecoderBuffer->GetBitsLong(&ltmp, 1);
	m_DecoderBuffer->TakeBits(1);
	variances[i][2] = (int)ltmp;
      }
    }
  }
  m_DominantColorDescriptorInterface->SetDominantColors(percents, colors,
                                                        variances);

  delete [] percents;
  {
    int i;
    for(i=0;i<dc_number;i++)
      delete [] colors[i];
  }
  delete [] colors;
  if( varpres ) {
    int i;
    for(i=0;i<dc_number;i++)
      delete [] variances[i];
    delete [] variances;
  }

  return 0;
}

//----------------------------------------------------------------------------
DominantColorCSInterfaceABC *DominantColorCS::GetInterface(void)
{ return &m_Interface; }
