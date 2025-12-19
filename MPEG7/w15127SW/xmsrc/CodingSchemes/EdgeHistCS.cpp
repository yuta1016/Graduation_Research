///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul
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
// Applicable File Name:  EdgeHistCS.cpp
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

const UUID EdgeHistogramCSInterface::myID = UUID("");
const char *EdgeHistogramCSInterface::myName = 
                            "EdgeHistogramCodingSchemeInterface";

// create a UUID for your type
const UUID EdgeHistogramCS::myID = UUID("to be created with xx");
const char *EdgeHistogramCS::myName = "EdgeHistogramCodingScheme";

const UUID EdgeHistogramCS::valID = UUID("");
const char *EdgeHistogramCS::valName = "";

const UUID EdgeHistogramCSInterfaceABC::myID = UUID("");

EdgeHistogramCSInterfaceABC::EdgeHistogramCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
EdgeHistogramCSInterface::EdgeHistogramCSInterface(EdgeHistogramCS *aEdgeHistogram):m_EdgeHistogramCS(aEdgeHistogram)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
EdgeHistogramCSInterface::~EdgeHistogramCSInterface()
{
}

//----------------------------------------------------------------------------
const UUID& EdgeHistogramCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void EdgeHistogramCSInterface::destroy(void)
{ delete m_EdgeHistogramCS; }


//----------------------------------------------------------------------------
BitBuffer *EdgeHistogramCSInterface::GetEncoderStreamBuffer(void)
{
  return m_EdgeHistogramCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int EdgeHistogramCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_EdgeHistogramCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *EdgeHistogramCSInterface::GetDecoderStreamBuffer(void)
{
  return m_EdgeHistogramCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int EdgeHistogramCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  return m_EdgeHistogramCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC
*EdgeHistogramCSInterface::GetDescriptorInterface(void)

{
  return m_EdgeHistogramCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int EdgeHistogramCSInterface::
SetDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
		       *aEdgeHistogramDescriptorInterface)
{
  return m_EdgeHistogramCS->
    SetDescriptorInterface(aEdgeHistogramDescriptorInterface);
}

//----------------------------------------------------------------------------
int EdgeHistogramCSInterface::StartEncode(void)
{
  return m_EdgeHistogramCS->StartEncode();
}

//----------------------------------------------------------------------------
int EdgeHistogramCSInterface::StartDecode(void)
{
  return m_EdgeHistogramCS->StartDecode();
}

//=============================================================================
EdgeHistogramCS::EdgeHistogramCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_DescriptorInterface(0)
{
  SetExposedInterface(&m_Interface);
}

//---------------------------------------------------------------------------
EdgeHistogramCS::EdgeHistogramCS(
		EdgeHistogramDescriptorInterfaceABC* aEdgeHistogram):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_DescriptorInterface(NULL)
{
  /* create descriptor if it does not exist*/
  if (!aEdgeHistogram) {
    EdgeHistogramDescriptor *descriptor = new EdgeHistogramDescriptor;
    aEdgeHistogram=descriptor->GetInterface();
  }

  /* connect Descritors with CS*/
  SetDescriptorInterface(aEdgeHistogram);

  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
EdgeHistogramCS::~EdgeHistogramCS()
{
	/* release descriptor */
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& EdgeHistogramCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EdgeHistogramCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& EdgeHistogramCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* EdgeHistogramCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool EdgeHistogramCS::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
BitBuffer * EdgeHistogramCS::GetEncoderStreamBuffer(void)
{
  return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int EdgeHistogramCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_EncoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
BitBuffer * EdgeHistogramCS::GetDecoderStreamBuffer(void)
{
  return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int EdgeHistogramCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
  if (!aBuffer) return -1;

  m_DecoderBuffer = aBuffer;
  return 0;
}

//----------------------------------------------------------------------------
EdgeHistogramDescriptorInterfaceABC *EdgeHistogramCS::
GetDescriptorInterface(void)
{
  return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int EdgeHistogramCS::
SetDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
	      *aEdgeHistogramDescriptorInterface)
{
  /* check if new value is different from old value*/
  if (m_DescriptorInterface == 
    aEdgeHistogramDescriptorInterface) return 0;
	
  /* release old interface*/
  if (m_DescriptorInterface) 
    m_DescriptorInterface->release();

  /* add new interface*/
  m_DescriptorInterface =
    aEdgeHistogramDescriptorInterface;
  if (m_DescriptorInterface) {
    m_DescriptorInterface->addref();
  }
  else {
    return -1;
  }

  return 0;
}

//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int EdgeHistogramCS::StartEncode(void)
{
  char *pLocal_Edge_Put;
  int i;
  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"EdgeHistogramCS::StartEncode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"EdgeHistogramCS::StartEncode: Interface not initialised\n");
    return -1;
  }

  pLocal_Edge_Put = m_DescriptorInterface->GetEdgeHistogramElement();
  for( i=0; i<80; i++)
    m_EncoderBuffer->PutBitsLong( (long) pLocal_Edge_Put[i], 3);

  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int EdgeHistogramCS::StartDecode(void)
{
  char	*pLocalEdge;
  int 	i;
  long	Value_Get;

  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"EdgeHistogramCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_DescriptorInterface==NULL) {
    fprintf(stderr,"EdgeHistogramCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  pLocalEdge = NULL;
  pLocalEdge = new char[80];

  for( i=0; i < 80; i++) {
    m_DecoderBuffer->GetBitsLong(&Value_Get, 3);
    pLocalEdge[i] = (char)Value_Get;
    m_DecoderBuffer->TakeBits(3);
  }
  m_DescriptorInterface->SetEdgeHistogramElement( pLocalEdge );

  delete [] pLocalEdge;
  
  // All done
  return 0;
}

//----------------------------------------------------------------------------
EdgeHistogramCSInterfaceABC *EdgeHistogramCS::GetInterface(void)
{ return &m_Interface; }
