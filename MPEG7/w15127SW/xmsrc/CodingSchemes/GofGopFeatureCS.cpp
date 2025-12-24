///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio Yamada
// NEC Corp.
// Soo-Jun Park (ETRI), Dong Kwon Park (Dongguk Univ)
// Leszek Cieplinski (Mitsubishi Electric ITE-VIL)
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
// Copyright (c) 1998-2004.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GofGopFeatureCS.cpp
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

const UUID GofGopFeatureCSInterface::myID = UUID("");
const char *GofGopFeatureCSInterface::myName =
                         "GofGopFeatureCodingSchemeInterface";

// create a UUID for your type
const UUID GofGopFeatureCS::myID = UUID("");
const char *GofGopFeatureCS::myName = "GofGopFeatureCodingScheme";

const UUID GofGopFeatureCS::valID = UUID("");
const char *GofGopFeatureCS::valName = "";

const UUID GofGopFeatureCSInterfaceABC::myID = UUID("");

GofGopFeatureCSInterfaceABC::GofGopFeatureCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GofGopFeatureCSInterface::GofGopFeatureCSInterface(GofGopFeatureCS *aGofGopFeature):
m_GofGopFeatureCS(aGofGopFeature)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GofGopFeatureCSInterface::~GofGopFeatureCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& GofGopFeatureCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GofGopFeatureCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void GofGopFeatureCSInterface::destroy(void)
{ delete m_GofGopFeatureCS; }

//----------------------------------------------------------------------------
BitBuffer *GofGopFeatureCSInterface::GetEncoderStreamBuffer(void)
{
  return m_GofGopFeatureCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GofGopFeatureCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GofGopFeatureCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *GofGopFeatureCSInterface::GetDecoderStreamBuffer(void)
{
  return m_GofGopFeatureCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GofGopFeatureCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GofGopFeatureCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
GofGopFeatureDescriptorInterfaceABC
*GofGopFeatureCSInterface::GetDescriptorInterface(void)

{
  return m_GofGopFeatureCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int GofGopFeatureCSInterface::
SetDescriptorInterface(GofGopFeatureDescriptorInterfaceABC
		       *aGofGopFeatureDescriptorInterface)
{
  return m_GofGopFeatureCS->
    SetDescriptorInterface(aGofGopFeatureDescriptorInterface);
}

//----------------------------------------------------------------------------
int GofGopFeatureCSInterface::StartEncode(void)
{
  return m_GofGopFeatureCS->StartEncode();
}

//----------------------------------------------------------------------------
int GofGopFeatureCSInterface::StartDecode(void)
{
  return m_GofGopFeatureCS->StartDecode();
}
//=============================================================================
GofGopFeatureCS::GofGopFeatureCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

  ColorLayoutCS *aColorLayoutCS= new ColorLayoutCS();
  EdgeHistogramCS *aEdgeHistogramCS= new EdgeHistogramCS();
  DominantColorCS *aDominantColorCS= new DominantColorCS(); // added by LC

  if (m_ColorLayoutCS)
    m_ColorLayoutCS=aColorLayoutCS->GetInterface();
  if (m_EdgeHistogramCS)
    m_EdgeHistogramCS=aEdgeHistogramCS->GetInterface();
  if (m_DominantColorCS)
    m_DominantColorCS=aDominantColorCS->GetInterface();

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GofGopFeatureCS::GofGopFeatureCS(GofGopFeatureDescriptorInterfaceABC* aGofGopFeature):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aGofGopFeature) {
    GofGopFeatureDescriptor *descriptor =
      new GofGopFeatureDescriptor(0,0,0);
      aGofGopFeature=descriptor->GetInterface();
  }

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub CS and
     connect them with descriptor if availabe*/
  {
    ColorLayoutCS *aColorLayoutCS;

    if (aGofGopFeature->GetColorLayoutDescriptorInterface()) {
      aColorLayoutCS= new
	ColorLayoutCS(aGofGopFeature->GetColorLayoutDescriptorInterface());
    } else { 
      aColorLayoutCS= new ColorLayoutCS();
    }
    if (aColorLayoutCS) {
      m_ColorLayoutCS=aColorLayoutCS->GetInterface();
    }
  }
  {
    EdgeHistogramCS *aEdgeHistogramCS;

    if (aGofGopFeature->GetEdgeHistogramDescriptorInterface()) {
      aEdgeHistogramCS= new
	EdgeHistogramCS(aGofGopFeature->GetEdgeHistogramDescriptorInterface());
    } else { 
      aEdgeHistogramCS= new EdgeHistogramCS();
    }
    if (aEdgeHistogramCS) {
      m_EdgeHistogramCS=aEdgeHistogramCS->GetInterface();
    }
  }
  { // added by LC
    DominantColorCS *aDominantColorCS;

    if (aGofGopFeature->GetDominantColorDescriptorInterface()) {
      aDominantColorCS= new
	DominantColorCS(aGofGopFeature->GetDominantColorDescriptorInterface());
    } else { 
      aDominantColorCS= new DominantColorCS();
    }
    if (aDominantColorCS) {
      m_DominantColorCS=aDominantColorCS->GetInterface();
    }
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aGofGopFeature);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GofGopFeatureCS::~GofGopFeatureCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  if (m_ColorLayoutCS) m_ColorLayoutCS->destroy();
  if (m_EdgeHistogramCS) m_EdgeHistogramCS->destroy();
  if (m_DominantColorCS) m_DominantColorCS->destroy();
}

//----------------------------------------------------------------------------
const UUID& GofGopFeatureCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GofGopFeatureCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GofGopFeatureCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GofGopFeatureCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GofGopFeatureCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * GofGopFeatureCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int GofGopFeatureCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  if (m_ColorLayoutCS) m_ColorLayoutCS->SetEncoderStreamBuffer(aBuffer);
  if (m_EdgeHistogramCS) m_EdgeHistogramCS->SetEncoderStreamBuffer(aBuffer);
  if (m_DominantColorCS) m_DominantColorCS->SetEncoderStreamBuffer(aBuffer);
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * GofGopFeatureCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int GofGopFeatureCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  if (m_ColorLayoutCS) m_ColorLayoutCS->SetDecoderStreamBuffer(aBuffer);
  if (m_EdgeHistogramCS) m_EdgeHistogramCS->SetDecoderStreamBuffer(aBuffer);
  if (m_DominantColorCS) m_DominantColorCS->SetDecoderStreamBuffer(aBuffer);

  return 0;
}

//----------------------------------------------------------------------------
GofGopFeatureDescriptorInterfaceABC *GofGopFeatureCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int GofGopFeatureCS::
SetDescriptorInterface(GofGopFeatureDescriptorInterfaceABC
		       *aGofGopFeatureDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aGofGopFeatureDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aGofGopFeatureDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

    /* conect sub descriptors with sub CS*/
    if ((m_DescriptorInterface->
	 GetColorLayoutDescriptorInterface()) &&
	m_ColorLayoutCS) {
      m_ColorLayoutCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetColorLayoutDescriptorInterface());
    }
    if ((m_DescriptorInterface->
	 GetEdgeHistogramDescriptorInterface()) &&
	m_EdgeHistogramCS) {
      m_EdgeHistogramCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetEdgeHistogramDescriptorInterface());
    }
    // added by LC
    if ((m_DescriptorInterface->
	 GetDominantColorDescriptorInterface()) &&
	m_DominantColorCS) {
      m_DominantColorCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetDominantColorDescriptorInterface());
    }
  }
  else {
    m_ColorLayoutCS->SetDescriptorInterface(0);
    m_EdgeHistogramCS->SetDescriptorInterface(0);
    m_DominantColorCS->SetDescriptorInterface(0); // added by LC
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int GofGopFeatureCS::StartEncode(void)
{
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"GofGopFeatureCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GofGopFeatureCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  long descriptorID=255;

  if (m_DescriptorInterface->
      GetColorLayoutDescriptorInterface()) {

    if (!m_ColorLayoutCS->GetDescriptorInterface() ||
        !m_ColorLayoutCS->GetEncoderStreamBuffer()) return -1;
    if (m_ColorLayoutCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetColorLayoutDescriptorInterface()) return -1;
    descriptorID=4;
  } else if (m_DescriptorInterface->
        GetEdgeHistogramDescriptorInterface()) {
    if (!m_EdgeHistogramCS->GetDescriptorInterface() ||
        !m_EdgeHistogramCS->GetEncoderStreamBuffer()) return -1;
    if (m_EdgeHistogramCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetEdgeHistogramDescriptorInterface()) return -1;
    descriptorID=8;
  } else if (m_DescriptorInterface-> // added by LC
        GetDominantColorDescriptorInterface()) {
    if (!m_DominantColorCS->GetDescriptorInterface() ||
        !m_DominantColorCS->GetEncoderStreamBuffer()) return -1;
    if (m_DominantColorCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetDominantColorDescriptorInterface()) return -1;
    descriptorID=7;
  } else return -1;

  long aggregation=GetDescriptorInterface()->GetAggregation();
  if(aggregation==0){
    m_EncoderBuffer->PutBitsLong(0L, 1);
  } else {
    m_EncoderBuffer->PutBitsLong(1L, 1);
    m_EncoderBuffer->PutBitsLong(aggregation, 3);
  }
  m_EncoderBuffer->PutBitsLong(descriptorID, 8);

  switch(descriptorID){
  case 4: m_ColorLayoutCS->StartEncode(); break;
  case 7: m_DominantColorCS->StartEncode(); break;  //added by LC
  case 8: m_EdgeHistogramCS->StartEncode(); break;
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int GofGopFeatureCS::StartDecode(void)
{
  long v;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"GofGopFeatureCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GofGopFeatureCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  m_DecoderBuffer->GetBitsLong(&v,1UL);
  m_DecoderBuffer->TakeBits(1UL);
  if(v==1){
    m_DecoderBuffer->GetBitsLong(&v, 3UL);
    m_DecoderBuffer->TakeBits(3UL);
    GetDescriptorInterface()->SetAggregation(v);
  }
  m_DecoderBuffer->GetBitsLong(&v,8UL);
  m_DecoderBuffer->TakeBits(8UL);

  if(v==4){
    if (!m_DescriptorInterface->
        GetColorLayoutDescriptorInterface()) {
      return -1;
    }
    if (!m_ColorLayoutCS->GetDescriptorInterface() ||
        !m_ColorLayoutCS->GetDecoderStreamBuffer()) return -1;
    if (m_ColorLayoutCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetColorLayoutDescriptorInterface()) return -1;
    m_ColorLayoutCS->StartDecode();
  } else if(v==7){ // added by LC
    if (!m_DescriptorInterface->
        GetDominantColorDescriptorInterface()) {
      return -1;
    }
    if (!m_DominantColorCS->GetDescriptorInterface() ||
        !m_DominantColorCS->GetDecoderStreamBuffer()) return -1;
    if (m_DominantColorCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetDominantColorDescriptorInterface()) return -1;
    m_DominantColorCS->StartDecode();
  } else if(v==8){
    if (!m_DescriptorInterface->
        GetEdgeHistogramDescriptorInterface()) {
      return -1;
    }
    if (!m_EdgeHistogramCS->GetDescriptorInterface() ||
        !m_EdgeHistogramCS->GetDecoderStreamBuffer()) return -1;
    if (m_EdgeHistogramCS->GetDescriptorInterface() !=
        m_DescriptorInterface->
        GetEdgeHistogramDescriptorInterface()) return -1;
    m_EdgeHistogramCS->StartDecode();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
GofGopFeatureCSInterfaceABC *GofGopFeatureCS::GetInterface(void)
{ return &m_Interface; }



