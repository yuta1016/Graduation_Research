///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorCS.cpp
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

const UUID GoFGoPColorCSInterface::myID = UUID("");
const char *GoFGoPColorCSInterface::myName =
                         "GoFGoPColorCodingSchemeInterface";

const UUID GoFGoPColorCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76e");
const char *GoFGoPColorCS::myName = "GoFGoPColorCodingScheme";

const UUID GoFGoPColorCS::valID = UUID("");
const char *GoFGoPColorCS::valName = "";

const UUID GoFGoPColorCSInterfaceABC::myID = UUID("");

GoFGoPColorCSInterfaceABC::GoFGoPColorCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
GoFGoPColorCSInterface::GoFGoPColorCSInterface(GoFGoPColorCS *aGoFGoPColor):
m_GoFGoPColorCS(aGoFGoPColor)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
GoFGoPColorCSInterface::~GoFGoPColorCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& GoFGoPColorCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void GoFGoPColorCSInterface::destroy(void)
{ delete m_GoFGoPColorCS; }

//----------------------------------------------------------------------------
BitBuffer *GoFGoPColorCSInterface::GetEncoderStreamBuffer(void)
{
  return m_GoFGoPColorCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GoFGoPColorCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GoFGoPColorCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *GoFGoPColorCSInterface::GetDecoderStreamBuffer(void)
{
  return m_GoFGoPColorCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int GoFGoPColorCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_GoFGoPColorCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC
*GoFGoPColorCSInterface::GetDescriptorInterface(void)

{
  return m_GoFGoPColorCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int GoFGoPColorCSInterface::
SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aGoFGoPColorDescriptorInterface)
{
  return m_GoFGoPColorCS->
    SetDescriptorInterface(aGoFGoPColorDescriptorInterface);
}

//----------------------------------------------------------------------------
int GoFGoPColorCSInterface::StartEncode(void)
{
  return m_GoFGoPColorCS->StartEncode();
}

//----------------------------------------------------------------------------
int GoFGoPColorCSInterface::StartDecode(void)
{
  return m_GoFGoPColorCS->StartDecode();
}
//=============================================================================
GoFGoPColorCS::GoFGoPColorCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{

  /* create sub CS and
     connect them with descriptor if availabe*/
  ScalableColorCS *aScalableColorCS= new ScalableColorCS();

  /* connect sub CS with GoFGoPColorCS*/
  // 2013/02/26 - Bug fix (KW)
  if (aScalableColorCS)
    m_ScalableColorCS=aScalableColorCS->GetInterface();
  //    SetScalableColorCSInterface(aScalableColorCS->GetInterface());

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GoFGoPColorCS::GoFGoPColorCS(GoFGoPColorDescriptorInterfaceABC* aGoFGoPColor):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aGoFGoPColor) {
    GoFGoPColorDescriptor *descriptor =
      new GoFGoPColorDescriptor(0); /* one argument for each sub-component*/
      aGoFGoPColor=descriptor->GetInterface();
  }

#ifdef VERBOSE
  fprintf(stderr,"Connect sub descriptors\n");
#endif

  /* create sub CS and
     connect them with descriptor if availabe*/
  {
    ScalableColorCS *aScalableColorCS;

    if (aGoFGoPColor->GetScalableColorDescriptorInterface()) {
      aScalableColorCS= new
	ScalableColorCS(aGoFGoPColor->GetScalableColorDescriptorInterface());
    }
    else { 
      aScalableColorCS= new ScalableColorCS();
    }
    if (aScalableColorCS) {
      m_ScalableColorCS=aScalableColorCS->GetInterface();
      //      SetScalableColorCSInterface(m_ScalableColorCS);
    }
  }

  /* connect descritors with CS*/
  SetDescriptorInterface(aGoFGoPColor);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
GoFGoPColorCS::~GoFGoPColorCS()
{
  /* release descriptor*/
  if (m_DescriptorInterface)
    m_DescriptorInterface->release();
  if (m_ScalableColorCS) m_ScalableColorCS->destroy();
}

//----------------------------------------------------------------------------
const UUID& GoFGoPColorCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *GoFGoPColorCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& GoFGoPColorCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* GoFGoPColorCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool GoFGoPColorCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * GoFGoPColorCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int GoFGoPColorCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  if (m_ScalableColorCS) m_ScalableColorCS->SetEncoderStreamBuffer(aBuffer);
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * GoFGoPColorCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int GoFGoPColorCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  if (m_ScalableColorCS) m_ScalableColorCS->SetDecoderStreamBuffer(aBuffer);

  return 0;
}

//----------------------------------------------------------------------------
GoFGoPColorDescriptorInterfaceABC *GoFGoPColorCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int GoFGoPColorCS::
SetDescriptorInterface(GoFGoPColorDescriptorInterfaceABC
		       *aGoFGoPColorDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
      aGoFGoPColorDescriptorInterface) return 0;

  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface = 
    aGoFGoPColorDescriptorInterface;
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
  }
  else {
    /* if no descriptor, release also descriptor interfaces from
       sub CS*/
    m_ScalableColorCS->SetDescriptorInterface(0);
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int GoFGoPColorCS::StartEncode(void)
{
  long v;

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"GoFGoPColorCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GoFGoPColorCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  /* check values for ScalableColor **************************/ 
  /* check if ScalableColor descriptor is linked to GoFGoPColorA*/
  if (!m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) {
    return -1;
  }

  /* check if ScalableColor descritor and stream buffer are linked
     to ScalableColor CS*/
  if (!m_ScalableColorCS->GetDescriptorInterface() ||
      !m_ScalableColorCS->GetEncoderStreamBuffer()) return -1;

  /* check if ScalableColor descriptor attached to
     GoFGoPColor descriptor
     is equal to ScalableColor descritor attached to
     ScalableColor CS */
  if (m_ScalableColorCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) return -1;

  // In this simple case I'll just get the value from the D/DSType
  switch (GetDescriptorInterface()->GetAggregation()) {
  case Average:
    v=0;
    break;
  case Median:
    v=1;
    break;
  case Intersection:
    v=2;
    break;
  default:
    v=3;
  }

  // And now I'll dump it into the buffer. This says put the 32 bits
  // of the number stored in v to file.
  m_EncoderBuffer->PutBitsLong(v,2);

  /* encode ScalableColor description*/
  m_ScalableColorCS->StartEncode();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int GoFGoPColorCS::StartDecode(void)
{
  long v;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"GoFGoPColorCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"GoFGoPColorCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  /* check values for ScalableColor **************************/ 
  /* check if ScalableColor descriptor is linked to GoFGoPColorA*/
  if (!m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) {
    return -1;
  }

  /* check if ScalableColor descritor and stream buffer are linked
     to ScalableColor CS*/
  if (!m_ScalableColorCS->GetDescriptorInterface() ||
      !m_ScalableColorCS->GetDecoderStreamBuffer()) return -1;

  /* check if ScalableColor descriptor attached to
     GoFGoPColor descriptor
     is equal to ScalableColor descritor attached to
     ScalableColor CS */
  if (m_ScalableColorCS->GetDescriptorInterface() !=
      m_DescriptorInterface->
      GetScalableColorDescriptorInterface()) return -1;

  // Read 2 bits the data in from the bit stream
  /* style for reading the bits interpreted as nummeric data type*/
   m_DecoderBuffer->GetBitsLong(&v,2);


  // Tell it to move along to the next bit of data - isn't a part of GetBits!
  // to allow variable length decoding
  m_DecoderBuffer->TakeBits(2);

  // Put the data into the D/DSType
  switch (v) {
  case 0:
    GetDescriptorInterface()->SetAggregation(Average);
    break;
  case 1:
    GetDescriptorInterface()->SetAggregation(Median);
    break;
  case 2:
    GetDescriptorInterface()->SetAggregation(Intersection);
    break;
  default:
    GetDescriptorInterface()->SetAggregation(Reserved);
  }

  /* encode ScalableColor description*/
  m_ScalableColorCS->StartDecode();

  // All done
  return 0;
}

//----------------------------------------------------------------------------
GoFGoPColorCSInterfaceABC *GoFGoPColorCS::GetInterface(void)
{ return &m_Interface; }

