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
// Applicable File Name:  SpokenContentBlockCS.cpp
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

const UUID SpokenContentBlockCSInterface::myID = UUID("");
const char *SpokenContentBlockCSInterface::myName =
                         "SpokenContentBlock Type Coding Engine Interface";

const UUID SpokenContentBlockCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockCS::myName = "SpokenContentBlock Type Coding Scheme";

const UUID SpokenContentBlockCS::valID = UUID("");
const char *SpokenContentBlockCS::valName = "";

const UUID SpokenContentBlockCSInterfaceABC::myID = UUID("");

SpokenContentBlockCSInterfaceABC::SpokenContentBlockCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentBlockCSInterface::SpokenContentBlockCSInterface(SpokenContentBlockCS
						     *aSpokenContentBlock):
m_SpokenContentBlockCS(aSpokenContentBlock)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentBlockCSInterface::~SpokenContentBlockCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentBlockCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentBlockCSInterface::destroy(void)
{ delete m_SpokenContentBlockCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentBlockCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentBlockCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentBlockCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentBlockCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentBlockCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentBlockCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentBlockCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentBlockCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterfaceABC
*SpokenContentBlockCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentBlockCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentBlockCSInterface::
SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
		       *aSpokenContentBlockDescriptorInterface)
{
        return m_SpokenContentBlockCS->
	  SetDescriptorInterface(aSpokenContentBlockDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentBlockCSInterface::StartEncode(void)
{
        return m_SpokenContentBlockCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentBlockCSInterface::StartDecode(void)
{
        return m_SpokenContentBlockCS->StartDecode();
}
//=============================================================================

SpokenContentBlockCS::SpokenContentBlockCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentBlockDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentBlockCS::SpokenContentBlockCS(SpokenContentBlockDescriptorInterfaceABC* SpokenContentBlock):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentBlockDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentBlock) {
	  SpokenContentBlockDescriptor *descriptor =
	    new SpokenContentBlockDescriptor();
	  SpokenContentBlock=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentBlock);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentBlockCS::~SpokenContentBlockCS()
{
  // release descriptor
  if (m_SpokenContentBlockDescriptorInterface) {
    m_SpokenContentBlockDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentBlockCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentBlockCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentBlockCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentBlockCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentBlockCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentBlockCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterfaceABC *SpokenContentBlockCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentBlockDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentBlockCS::
SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
		       *aSpokenContentBlockDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentBlockDescriptorInterface == 
	    aSpokenContentBlockDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentBlockDescriptorInterface) 
	  m_SpokenContentBlockDescriptorInterface->release();

	// add new interface
	m_SpokenContentBlockDescriptorInterface = 
	  aSpokenContentBlockDescriptorInterface;
	if (m_SpokenContentBlockDescriptorInterface) {
	  m_SpokenContentBlockDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentBlockCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentBlockCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentBlockDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the integers
  unsigned short us=GetDescriptorInterface()->GetNNodes();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  us=GetDescriptorInterface()->GetNum();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));

  // Handle the strings
  char* pn=const_cast<char*>(GetDescriptorInterface()->GetMediaTime());
  unsigned char pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*static_cast<long> (pl)*sizeof(char));
  pn=const_cast<char*>(GetDescriptorInterface()->GetAudio());
  pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*static_cast<long> (pl)*sizeof(char));
  pn=const_cast<char*>(GetDescriptorInterface()->GetDefaultSpeaker());
  pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*static_cast<long> (pl)*sizeof(char));

  // Handle the nodes
  for(unsigned short i=0;i<GetDescriptorInterface()->GetNNodes();i++) {
    SpokenContentNodeDescriptor* node;
    SpokenContentNodeDescriptorInterfaceABC* node_abc;
    SpokenContentNodeCS* node_cs;
    node=GetDescriptorInterface()->GetNode(i);
    node_abc=node->GetInterface();
    node_cs=new SpokenContentNodeCS();
    node_cs->SetDescriptorInterface(node_abc);
    node_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    node_cs->StartEncode();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentBlockCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentBlockCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentBlockDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentBlockCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Handle the integers
  unsigned short us;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  GetDescriptorInterface()->SetNNodes(us);
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&us),
                           8*sizeof(unsigned short));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned short));
  GetDescriptorInterface()->SetNum(us);

  // Now handle the strings
  char b[256];
  unsigned char l;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetMediaTime(b);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetAudio(b);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetDefaultSpeaker(b);

  // Now do the nodes
  for(unsigned short i=0;i<GetDescriptorInterface()->GetNNodes();i++) {
    SpokenContentNodeDescriptor*   node;
    SpokenContentNodeDescriptorInterfaceABC* node_abc;
    SpokenContentNodeCS* node_cs;
    node=new SpokenContentNodeDescriptor();
    node_abc=node->GetInterface();
    node_cs=new SpokenContentNodeCS(node_abc);
    node_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    node_cs->StartDecode();
    GetDescriptorInterface()->SetNode(i,node);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentBlockCSInterfaceABC *SpokenContentBlockCS::GetInterface(void)
{ return &m_Interface; }



