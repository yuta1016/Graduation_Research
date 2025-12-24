///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Applicable File Name:  AdvancedFaceRecognitionCS.cpp
//

#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include "Utilities/Utilities.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef	WIN32
#include <netinet/in.h>
#endif

using namespace XM;

const UUID AdvancedFaceRecognitionCSInterface::myID = UUID("");
const char *AdvancedFaceRecognitionCSInterface::myName = "AdvancedFaceRecognitionCodingSchemeInterface";

// create a UUID for your type
const UUID AdvancedFaceRecognitionCS::myID = UUID("");
const char *AdvancedFaceRecognitionCS::myName = "AdvancedFaceRecognitionCodingScheme";

const UUID AdvancedFaceRecognitionCS::valID = UUID("");
const char *AdvancedFaceRecognitionCS::valName = "";

const UUID AdvancedFaceRecognitionCSInterfaceABC::myID = UUID("");

AdvancedFaceRecognitionCSInterfaceABC::AdvancedFaceRecognitionCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
AdvancedFaceRecognitionCSInterface::AdvancedFaceRecognitionCSInterface(AdvancedFaceRecognitionCS *aAdvancedFaceRecognition):
m_AdvancedFaceRecognitionCS(aAdvancedFaceRecognition)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionCSInterface::~AdvancedFaceRecognitionCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void AdvancedFaceRecognitionCSInterface::destroy(void)
{ delete m_AdvancedFaceRecognitionCS; }


//----------------------------------------------------------------------------
BitBuffer *AdvancedFaceRecognitionCSInterface::GetEncoderStreamBuffer(void)
{
        return m_AdvancedFaceRecognitionCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_AdvancedFaceRecognitionCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *AdvancedFaceRecognitionCSInterface::GetDecoderStreamBuffer(void)
{
        return m_AdvancedFaceRecognitionCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_AdvancedFaceRecognitionCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC *AdvancedFaceRecognitionCSInterface::
GetDescriptorInterface(void)
{
        return m_AdvancedFaceRecognitionCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCSInterface::
SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
		       *aAdvancedFaceRecognitionDescriptorInterface)
{
        return m_AdvancedFaceRecognitionCS->
	  SetDescriptorInterface(aAdvancedFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCSInterface::StartEncode(void)
{
        return m_AdvancedFaceRecognitionCS->StartEncode();
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCSInterface::StartDecode(void)
{
        return m_AdvancedFaceRecognitionCS->StartDecode();
}

//=============================================================================

AdvancedFaceRecognitionCS::AdvancedFaceRecognitionCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionCS::
AdvancedFaceRecognitionCS(AdvancedFaceRecognitionDescriptorInterfaceABC 
			 *aDescriptor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  /* create descriptor if it does not exist*/
  if (!aDescriptor) {
    AdvancedFaceRecognitionDescriptor *descriptor =
      new AdvancedFaceRecognitionDescriptor();
    aDescriptor=descriptor->GetInterface();
  }
  
  /* connect Descritors with coding schemes*/
  SetDescriptorInterface(aDescriptor);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionCS::~AdvancedFaceRecognitionCS()
{
  /* release descriptor */
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *AdvancedFaceRecognitionCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& AdvancedFaceRecognitionCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* AdvancedFaceRecognitionCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool AdvancedFaceRecognitionCS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
BitBuffer * AdvancedFaceRecognitionCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * AdvancedFaceRecognitionCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionDescriptorInterfaceABC *AdvancedFaceRecognitionCS::
GetDescriptorInterface(void)
{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCS::
SetDescriptorInterface(AdvancedFaceRecognitionDescriptorInterfaceABC
	      *aDescriptorInterface)
{
    /* check if new value is different from old value*/
        if (m_DescriptorInterface == 
	    aDescriptorInterface) return 0;
	
	/* release old interface*/
	if (m_DescriptorInterface) 
	  m_DescriptorInterface->release();

	/* add new interface*/
	m_DescriptorInterface = aDescriptorInterface;
	if (m_DescriptorInterface) {
	  m_DescriptorInterface->addref();
	}
	else {
	  return -1;
	}

	return 0;
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCS::StartEncode(void)
{

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"AdvancedFaceRecognitionCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"AdvancedFaceRecognitionCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  AdvancedFRD     *pLocal_Face_Put;
  int 	iw;
  
  pLocal_Face_Put = m_DescriptorInterface->GetAdvancedFaceRecognition();
  
  m_EncoderBuffer->PutBitsLong((long)pLocal_Face_Put->numOfFourierFeature,6);
  m_EncoderBuffer->PutBitsLong((long)pLocal_Face_Put->numOfCentralFourierFeature,6);
  m_EncoderBuffer->PutBitsLong((long)pLocal_Face_Put->extensionFlag,1);
    
  for ( iw = 0 ; iw < pLocal_Face_Put->numOfFourierFeature ; iw++ )
    m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->FourierFeature[iw], 5);
  for ( iw = 0 ; iw < pLocal_Face_Put->numOfCentralFourierFeature ; iw++ )
    m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->CentralFourierFeature[iw], 5);
  
  if ( pLocal_Face_Put->extensionFlag != 0 ){
    
    m_EncoderBuffer->PutBitsLong((long)pLocal_Face_Put->numOfCompositeFeature, 6);
    m_EncoderBuffer->PutBitsLong((long)pLocal_Face_Put->numOfCentralCompositeFeature, 6);
    
    for ( iw = 0 ; iw < pLocal_Face_Put->numOfCompositeFeature ; iw++ )
      m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->CompositeFeature[iw], 5);
    for ( iw = 0 ; iw < pLocal_Face_Put->numOfCentralCompositeFeature ; iw++ )
      m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->CentralCompositeFeature[iw], 5);
  }
  
  return 0;
}

//----------------------------------------------------------------------------
int AdvancedFaceRecognitionCS::StartDecode(void)
{
  
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"AdvancedFaceRecognitionCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"AdvancedFaceRecognitionCS::StartDecode: Interface not initialised\n");
    return -1;
  }
  
  AdvancedFRD	*pLocalFace;
  int 	iw;
  long	Value_Get;

  pLocalFace = new AdvancedFRD;

  m_DecoderBuffer->GetBitsLong(&Value_Get,6);  
  pLocalFace->numOfFourierFeature = Value_Get;
  m_DecoderBuffer->TakeBits(6);

  m_DecoderBuffer->GetBitsLong(&Value_Get,6);  
  pLocalFace->numOfCentralFourierFeature = Value_Get;
  m_DecoderBuffer->TakeBits(6);

  m_DecoderBuffer->GetBitsLong(&Value_Get,1);  
  pLocalFace->extensionFlag = Value_Get;
  m_DecoderBuffer->TakeBits(1);
  
  for (iw = 0; iw < pLocalFace->numOfFourierFeature ; iw++) {
    m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
    pLocalFace->FourierFeature[iw] = (char)Value_Get;
    m_DecoderBuffer->TakeBits(5);
  }

  for (iw = 0; iw < pLocalFace->numOfCentralFourierFeature; iw++) {
    m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
    pLocalFace->CentralFourierFeature[iw] = (char)Value_Get;
    m_DecoderBuffer->TakeBits(5);
  }

  if ( pLocalFace->extensionFlag != 0 ){

    m_DecoderBuffer->GetBitsLong(&Value_Get,6);  
    pLocalFace->numOfCompositeFeature = Value_Get;
    m_DecoderBuffer->TakeBits(6);

    m_DecoderBuffer->GetBitsLong(&Value_Get,6);  
    pLocalFace->numOfCentralCompositeFeature = Value_Get;
    m_DecoderBuffer->TakeBits(6);
    
    for (iw = 0; iw < pLocalFace->numOfCompositeFeature; iw++) {
      m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
      pLocalFace->CompositeFeature[iw] = (char)Value_Get;
      m_DecoderBuffer->TakeBits(5);
    }
  
    for (iw = 0; iw < pLocalFace->numOfCentralCompositeFeature ; iw++) {
      m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
      pLocalFace->CentralCompositeFeature[iw] = (char)Value_Get;
      m_DecoderBuffer->TakeBits(5);
    }

  }
  
  m_DescriptorInterface->SetAdvancedFaceRecognition( pLocalFace );
  
  delete	[] pLocalFace;
  return 0;
}

//----------------------------------------------------------------------------
AdvancedFaceRecognitionCSInterfaceABC *AdvancedFaceRecognitionCS::GetInterface(void)
{ return &m_Interface; }




