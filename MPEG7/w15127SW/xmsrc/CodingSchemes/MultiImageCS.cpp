///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of Technology
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
// Applicable File Name:  MultiImageCS.cpp
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

const UUID MultiImageCSInterface::myID = UUID("");
const char *MultiImageCSInterface::myName =
                         "MultiImageCodingSchemeInterface";

// create a UUID for your type
const UUID MultiImageCS::myID = UUID("to be created with xx");
const char *MultiImageCS::myName = "MultiImageCodingScheme";

const UUID MultiImageCS::valID = UUID("");
const char *MultiImageCS::valName = "";

const UUID MultiImageCSInterfaceABC::myID = UUID("");

MultiImageCSInterfaceABC::MultiImageCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
MultiImageCSInterface::MultiImageCSInterface(MultiImageCS *aMultiImage):
m_MultiImageCS(aMultiImage)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MultiImageCSInterface::~MultiImageCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& MultiImageCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MultiImageCSInterface::destroy(void)
{ delete m_MultiImageCS; }

//----------------------------------------------------------------------------
BitBuffer *MultiImageCSInterface::GetEncoderStreamBuffer(void)
{
  return m_MultiImageCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MultiImageCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_MultiImageCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *MultiImageCSInterface::GetDecoderStreamBuffer(void)
{
  return m_MultiImageCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int MultiImageCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_MultiImageCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC
*MultiImageCSInterface::GetDescriptorInterface(void)

{
  return m_MultiImageCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int MultiImageCSInterface::
SetDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aMultiImageDescriptorInterface)
{
  return m_MultiImageCS->
    SetDescriptorInterface(aMultiImageDescriptorInterface);
}

//----------------------------------------------------------------------------
int MultiImageCSInterface::StartEncode(void)
{
  return m_MultiImageCS->StartEncode();
}

//----------------------------------------------------------------------------
int MultiImageCSInterface::StartDecode(void)
{
  return m_MultiImageCS->StartDecode();
}
//=============================================================================
MultiImageCS::MultiImageCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

  /* create sub CS and
     connect them with descriptor if availabe*/
  ScalableColorCS *aScalableColorCS= new ScalableColorCS();
  ContourShapeCS *aContourShapeCS= new ContourShapeCS();

  /* connect sub CS with MultiImageCS*/
  if (aScalableColorCS)
    m_ScalableColorCS=aScalableColorCS->GetInterface();
  if (aContourShapeCS)
    m_ContourShapeCS=aContourShapeCS->GetInterface();

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiImageCS::MultiImageCS(MultiImageDescriptorInterfaceABC* MultiImage):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!MultiImage) {
    MultiImageDescriptor *descriptor =
      new MultiImageDescriptor(0,0); /* one argument for each sub-component*/
    MultiImage=descriptor->GetInterface();
  }

  /* connect descritors with CS*/
  // m15122
  //SetDescriptorInterface(MultiImage);

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub CS and
     connect them with descriptor if availabe*/
  {
    ScalableColorCS *aScalableColorCS;

    if (MultiImage->GetScalableColorDescriptorInterface()) {
      aScalableColorCS= new
	ScalableColorCS(MultiImage->GetScalableColorDescriptorInterface());
    }
    else { 
      aScalableColorCS= new ScalableColorCS();
    }
    if (aScalableColorCS) {
      m_ScalableColorCS=aScalableColorCS->GetInterface();
    }
  }

  {
    ContourShapeCS *aContourShapeCS;

    if (MultiImage->GetContourShapeDescriptorInterface()) {
      aContourShapeCS= new
	ContourShapeCS(MultiImage->GetContourShapeDescriptorInterface());
    }
    else { 
      aContourShapeCS= new ContourShapeCS();
    }
    if (aContourShapeCS) {
      m_ContourShapeCS=aContourShapeCS->GetInterface();
    }
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(MultiImage);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MultiImageCS::~MultiImageCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  if (m_ScalableColorCS) m_ScalableColorCS->destroy();
  if (m_ContourShapeCS) m_ContourShapeCS->destroy();
}

//----------------------------------------------------------------------------
const UUID& MultiImageCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MultiImageCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& MultiImageCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* MultiImageCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool MultiImageCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * MultiImageCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int MultiImageCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  if (m_ScalableColorCS) m_ScalableColorCS->SetEncoderStreamBuffer(aBuffer);
  if (m_ContourShapeCS) m_ContourShapeCS->SetEncoderStreamBuffer(aBuffer);
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * MultiImageCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int MultiImageCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  if (m_ScalableColorCS) m_ScalableColorCS->SetDecoderStreamBuffer(aBuffer);
  if (m_ContourShapeCS) m_ContourShapeCS->SetDecoderStreamBuffer(aBuffer);

  return 0;
}

//----------------------------------------------------------------------------
MultiImageDescriptorInterfaceABC *MultiImageCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int MultiImageCS::
SetDescriptorInterface(MultiImageDescriptorInterfaceABC
		       *aMultiImageDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aMultiImageDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aMultiImageDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();

    /* conect sub descriptors with sub CS*/
    if ((m_DescriptorInterface->
	 GetScalableColorDescriptorInterface()) &&
	m_ScalableColorCS) {
      m_ScalableColorCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetScalableColorDescriptorInterface());
    }
    if ((m_DescriptorInterface->
	 GetContourShapeDescriptorInterface()) &&
	m_ContourShapeCS) {
      m_ContourShapeCS->
	SetDescriptorInterface(m_DescriptorInterface->
			       GetContourShapeDescriptorInterface());
    }
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub CS*/
    m_ScalableColorCS->SetDescriptorInterface(0);
    m_ContourShapeCS->SetDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int MultiImageCS::StartEncode(void)
{
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"MultiImageCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"MultiImageCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  /* check values for ScalableColor **************************/ 
  /* check if ScalableColor descriptor is linked to MultiImageA*/
  if (!m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) {
    return -1;
  }

  /* check if ScalableColor descritor and stream buffer are linked
     to ScalableColor CS*/
  if (!m_ScalableColorCS->GetDescriptorInterface() ||
      !m_ScalableColorCS->GetEncoderStreamBuffer()) return -1;

  /* check if ScalableColor descriptor attached to
     MultiImage descriptor
     is equal to ScalableColor descritor attached to
     ScalableColor CS */
  if (m_ScalableColorCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) return -1;

  /* check values for ContourShape **************************/ 
  /* check if ContourShape descriptor is linked to MultiImageB*/
  if (!m_DescriptorInterface->
      GetContourShapeDescriptorInterface()) {
    return -1;
  }

  /* check if ContourShape descritor and stream buffer are linked
     to ContourShape CS*/
  if (!m_ContourShapeCS->GetDescriptorInterface() ||
      !m_ContourShapeCS->GetEncoderStreamBuffer()) return -1;

  /* check if ContourShape descriptor attached to
     MultiImage descriptor
     is equal to ContourShape descritor attached to
     ContourShape CS */
  if (m_ContourShapeCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetContourShapeDescriptorInterface()) return -1;

  /* encode ScalableColor description*/
  m_ScalableColorCS->StartEncode();

  /* encode ContourShape description*/
  m_ContourShapeCS->StartEncode();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int MultiImageCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"MultiImageCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"MultiImageCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  /* check values for ScalableColor **************************/ 
  /* check if ScalableColor descriptor is linked to MultiImageA*/
  if (!m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) {
    return -1;
  }

  /* check if ScalableColor descritor and stream buffer are linked
     to ScalableColor CS*/
  if (!m_ScalableColorCS->GetDescriptorInterface() ||
      !m_ScalableColorCS->GetDecoderStreamBuffer()) return -1;

  /* check if ScalableColor descriptor attached to
     MultiImage descriptor
     is equal to ScalableColor descritor attached to
     ScalableColor CS */
  if (m_ScalableColorCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) return -1;

  /* check values for ContourShape **************************/ 
  /* check if ContourShape descriptor is linked to MultiImageB*/
  if (!m_DescriptorInterface->
      GetContourShapeDescriptorInterface()) {
    return -1;
  }

  /* check if ContourShape descritor and stream buffer are linked
     to ContourShape CS*/
  if (!m_ContourShapeCS->GetDescriptorInterface() ||
      !m_ContourShapeCS->GetDecoderStreamBuffer()) return -1;

  /* check if ContourShape descriptor attached to
     MultiImage descriptor
     is equal to ContourShape descritor attached to
     ContourShape CS */
  if (m_ContourShapeCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetContourShapeDescriptorInterface()) return -1;

  /* encode ScalableColor description*/
  m_ScalableColorCS->StartDecode();

  /* encode ContourShape description*/
  m_ContourShapeCS->StartDecode();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
MultiImageCSInterfaceABC *MultiImageCS::GetInterface(void)
{ return &m_Interface; }



