///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang (Panasonic Singapore Laboratories Pte Ltd)
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
// Applicable File Name:  FaceRecognitionCS.cpp
//
#include "Descriptors/Descriptors.h"
#include "CodingSchemes/CodingSchemes.h"
#include "Utilities/BitBuffer.h"
#include <stdio.h>
#include <sys/types.h>
#ifndef	_VISUALC_
#include <netinet/in.h>
#endif

using namespace XM;

const UUID FaceRecognitionCSInterface::myID = UUID("");
const char *FaceRecognitionCSInterface::myName = "Face Recognition Coding Engine Interface";

const UUID FaceRecognitionCS::myID = UUID("");
const char *FaceRecognitionCS::myName = "Face Recognition Coding Scheme";

const UUID FaceRecognitionCS::valID = UUID("");
const char *FaceRecognitionCS::valName = "";

const UUID FaceRecognitionCSInterfaceABC::myID = UUID("");

FaceRecognitionCSInterfaceABC::FaceRecognitionCSInterfaceABC()
{
  SetInterfaceID(myID);
}

//=============================================================================
FaceRecognitionCSInterface::FaceRecognitionCSInterface(FaceRecognitionCS *aFaceRecognition):
m_FaceRecognitionCS(aFaceRecognition)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
FaceRecognitionCSInterface::~FaceRecognitionCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void FaceRecognitionCSInterface::destroy(void)
{ delete m_FaceRecognitionCS; }


//----------------------------------------------------------------------------
BitBuffer *FaceRecognitionCSInterface::GetEncoderStreamBuffer(void)
{
        return m_FaceRecognitionCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int FaceRecognitionCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_FaceRecognitionCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *FaceRecognitionCSInterface::GetDecoderStreamBuffer(void)
{
        return m_FaceRecognitionCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int FaceRecognitionCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_FaceRecognitionCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC *FaceRecognitionCSInterface::
GetDescriptorInterface(void)
{
        return m_FaceRecognitionCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int FaceRecognitionCSInterface::
SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
		       *aFaceRecognitionDescriptorInterface)
{
        return m_FaceRecognitionCS->
	  SetDescriptorInterface(aFaceRecognitionDescriptorInterface);
}

//----------------------------------------------------------------------------
int FaceRecognitionCSInterface::StartEncode(void)
{
        return m_FaceRecognitionCS->StartEncode();
}

//----------------------------------------------------------------------------
int FaceRecognitionCSInterface::StartDecode(void)
{
        return m_FaceRecognitionCS->StartDecode();
}

//=============================================================================

FaceRecognitionCS::FaceRecognitionCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
FaceRecognitionCS::
FaceRecognitionCS(FaceRecognitionDescriptorInterfaceABC 
			 *aDescriptor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
	/* create descriptor if it does not exist*/
	if (!aDescriptor) {
	  FaceRecognitionDescriptor *descriptor =
	    new FaceRecognitionDescriptor();
	  aDescriptor=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetDescriptorInterface(aDescriptor);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
FaceRecognitionCS::~FaceRecognitionCS()
{
	/* release descriptor */
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& FaceRecognitionCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *FaceRecognitionCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& FaceRecognitionCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* FaceRecognitionCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool FaceRecognitionCS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
BitBuffer * FaceRecognitionCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int FaceRecognitionCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * FaceRecognitionCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int FaceRecognitionCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
FaceRecognitionDescriptorInterfaceABC *FaceRecognitionCS::
GetDescriptorInterface(void)
{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int FaceRecognitionCS::
SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
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
int FaceRecognitionCS::StartEncode(void)
{

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_EncoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}
	FRD     *pLocal_Face_Put;
	int 	i;

	pLocal_Face_Put = m_DescriptorInterface->GetFaceRecognition();
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	for (i=0; i<48; i++)
//	for (i=0; i<49; i++)
		m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->eigenfeature[i], 5);
	/*
	for( i=0; i<17; i++)
		m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->eigenfeature[i], 6);
	for( i=17; i<25; i++)
		m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->eigenfeature[i], 5);
	for( i=25; i<49; i++)
		m_EncoderBuffer->PutBitsLong( (long) pLocal_Face_Put->eigenfeature[i], 4);
	*/

	return 0;
}

//----------------------------------------------------------------------------
int FaceRecognitionCS::StartDecode(void)
{

	/* check if Descriptor and stream buffers are linked
	   to coding scheme*/
	if (!m_DecoderBuffer || !m_DescriptorInterface) {
	  return -1;
	}

	FRD	*pLocalFace;
	int 	i;
	long	Value_Get;

	pLocalFace = new FRD;
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	for (i=0; i<48; i++) {
//	for( i=0; i<49; i++) {
	  m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
	  pLocalFace->eigenfeature[i] = (char)Value_Get;
	  m_DecoderBuffer->TakeBits(5);
	}

	/*
	for( i=0; i < 17; i++) {
	  m_DecoderBuffer->GetBitsLong(&Value_Get, 6);
	  pLocalFace->eigenfeature[i] = (char)Value_Get;
	  m_DecoderBuffer->TakeBits(6);
	}
	for( i=17; i < 25; i++) {
	  m_DecoderBuffer->GetBitsLong(&Value_Get, 5);
	  pLocalFace->eigenfeature[i] = (char)Value_Get;
	  m_DecoderBuffer->TakeBits(5);
	}
	for( i=25; i < 49; i++) {
	  m_DecoderBuffer->GetBitsLong(&Value_Get, 4);
	  pLocalFace->eigenfeature[i] = (char)Value_Get;
	  m_DecoderBuffer->TakeBits(4);
	}
	*/
	m_DescriptorInterface->SetFaceRecognition( pLocalFace );

	delete	[] pLocalFace;
	return 0;
}

//----------------------------------------------------------------------------
FaceRecognitionCSInterfaceABC *FaceRecognitionCS::GetInterface(void)
{ return &m_Interface; }
