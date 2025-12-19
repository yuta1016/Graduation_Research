///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentLinkCS.cpp
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

const UUID SpokenContentLinkCSInterface::myID = UUID("");
const char *SpokenContentLinkCSInterface::myName =
                         "SpokenContentLink Type Coding Engine Interface";

const UUID SpokenContentLinkCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentLinkCS::myName = "SpokenContentLink Type Coding Scheme";

const UUID SpokenContentLinkCS::valID = UUID("");
const char *SpokenContentLinkCS::valName = "";

const UUID SpokenContentLinkCSInterfaceABC::myID = UUID("");

SpokenContentLinkCSInterfaceABC::SpokenContentLinkCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentLinkCSInterface::SpokenContentLinkCSInterface(SpokenContentLinkCS
						     *aSpokenContentLink):
m_SpokenContentLinkCS(aSpokenContentLink)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLinkCSInterface::~SpokenContentLinkCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentLinkCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLinkCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentLinkCSInterface::destroy(void)
{ delete m_SpokenContentLinkCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLinkCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentLinkCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLinkCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLinkCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLinkCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentLinkCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLinkCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLinkCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptorInterfaceABC
*SpokenContentLinkCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentLinkCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentLinkCSInterface::
SetDescriptorInterface(SpokenContentLinkDescriptorInterfaceABC
		       *aSpokenContentLinkDescriptorInterface)
{
        return m_SpokenContentLinkCS->
	  SetDescriptorInterface(aSpokenContentLinkDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentLinkCSInterface::StartEncode(void)
{
        return m_SpokenContentLinkCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentLinkCSInterface::StartDecode(void)
{
        return m_SpokenContentLinkCS->StartDecode();
}
//=============================================================================

SpokenContentLinkCS::SpokenContentLinkCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentLinkDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentLinkCS::SpokenContentLinkCS(SpokenContentLinkDescriptorInterfaceABC* SpokenContentLink):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentLinkDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentLink) {
	  SpokenContentLinkDescriptor *descriptor =
	    new SpokenContentLinkDescriptor();
	  SpokenContentLink=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentLink);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentLinkCS::~SpokenContentLinkCS()
{
  // release descriptor
  if (m_SpokenContentLinkDescriptorInterface) {
    m_SpokenContentLinkDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLinkCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLinkCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLinkCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLinkCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLinkCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentLinkCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLinkCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentLinkCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLinkCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptorInterfaceABC *SpokenContentLinkCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentLinkDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentLinkCS::
SetDescriptorInterface(SpokenContentLinkDescriptorInterfaceABC
		       *aSpokenContentLinkDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentLinkDescriptorInterface == 
	    aSpokenContentLinkDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentLinkDescriptorInterface) 
	  m_SpokenContentLinkDescriptorInterface->release();

	// add new interface
	m_SpokenContentLinkDescriptorInterface = 
	  aSpokenContentLinkDescriptorInterface;
	if (m_SpokenContentLinkDescriptorInterface) {
	  m_SpokenContentLinkDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentLinkCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentLinkCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLinkDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Get the values to write
  float p=GetDescriptorInterface()->GetProbability();
  unsigned short n=GetDescriptorInterface()->GetNodeOffset();

  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&p),
                           8*sizeof(float));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned short));

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentLinkCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentLinkCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLinkDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentLinkCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the values
  float p;
  unsigned short n;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&p),8*sizeof(float));
  m_DecoderBuffer->TakeBits(8*sizeof(float));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));

  // Store the values
  GetDescriptorInterface()->SetProbability(p);
  GetDescriptorInterface()->SetNodeOffset(n);

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentLinkCSInterfaceABC *SpokenContentLinkCS::GetInterface(void)
{ return &m_Interface; }



