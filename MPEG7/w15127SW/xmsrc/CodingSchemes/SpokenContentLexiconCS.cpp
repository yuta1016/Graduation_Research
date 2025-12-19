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
// Applicable File Name:  SpokenContentLexiconCS.cpp
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

const UUID SpokenContentLexiconCSInterface::myID = UUID("");
const char *SpokenContentLexiconCSInterface::myName =
                         "SpokenContentLexicon Type Coding Engine Interface";

const UUID SpokenContentLexiconCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconCS::myName = "SpokenContentLexicon Type Coding Scheme";

const UUID SpokenContentLexiconCS::valID = UUID("");
const char *SpokenContentLexiconCS::valName = "";

const UUID SpokenContentLexiconCSInterfaceABC::myID = UUID("");

SpokenContentLexiconCSInterfaceABC::SpokenContentLexiconCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentLexiconCSInterface::SpokenContentLexiconCSInterface(SpokenContentLexiconCS
						     *aSpokenContentLexicon):
m_SpokenContentLexiconCS(aSpokenContentLexicon)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLexiconCSInterface::~SpokenContentLexiconCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentLexiconCSInterface::destroy(void)
{ delete m_SpokenContentLexiconCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLexiconCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentLexiconCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLexiconCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentLexiconCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentLexiconCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentLexiconCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterfaceABC
*SpokenContentLexiconCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentLexiconCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCSInterface::
SetDescriptorInterface(SpokenContentLexiconDescriptorInterfaceABC
		       *aSpokenContentLexiconDescriptorInterface)
{
        return m_SpokenContentLexiconCS->
	  SetDescriptorInterface(aSpokenContentLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCSInterface::StartEncode(void)
{
        return m_SpokenContentLexiconCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentLexiconCSInterface::StartDecode(void)
{
        return m_SpokenContentLexiconCS->StartDecode();
}
//=============================================================================

SpokenContentLexiconCS::SpokenContentLexiconCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentLexiconDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentLexiconCS::SpokenContentLexiconCS(SpokenContentLexiconDescriptorInterfaceABC* SpokenContentLexicon):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentLexiconDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentLexicon) {
	  SpokenContentLexiconDescriptor *descriptor =
	    new SpokenContentLexiconDescriptor();
	  SpokenContentLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentLexiconCS::~SpokenContentLexiconCS()
{
  // release descriptor
  if (m_SpokenContentLexiconDescriptorInterface) {
    m_SpokenContentLexiconDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLexiconCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLexiconCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentLexiconCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentLexiconCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterfaceABC *SpokenContentLexiconCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentLexiconDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentLexiconCS::
SetDescriptorInterface(SpokenContentLexiconDescriptorInterfaceABC
		       *aSpokenContentLexiconDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentLexiconDescriptorInterface == 
	    aSpokenContentLexiconDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentLexiconDescriptorInterface) 
	  m_SpokenContentLexiconDescriptorInterface->release();

	// add new interface
	m_SpokenContentLexiconDescriptorInterface = 
	  aSpokenContentLexiconDescriptorInterface;
	if (m_SpokenContentLexiconDescriptorInterface) {
	  m_SpokenContentLexiconDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentLexiconCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentLexiconCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Write out the id - assume less that 256 chars
  char* i=const_cast<char*>(GetDescriptorInterface()->GetLexiconID());
  unsigned char li=strlen(i);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&li),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (i),
                           8*static_cast<long> (li)*sizeof(char));
  
  // Put bits to say whether we have a num 
  unsigned long n=GetDescriptorInterface()->GetLexiconNumOriginalEntries();
  if(n==0) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                              8*sizeof(unsigned long));
  }

#if 0 // PNG
  // Put bit to say whether we have a decoder
  if(GetDescriptorInterface()->GetLexiconDecoder()==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    SpokenContentExtractionInfoDescriptor* info;
    SpokenContentExtractionInfoDescriptorInterfaceABC* info_abc;
    SpokenContentExtractionInfoCS* info_cs;
    info=GetDescriptorInterface()->GetLexiconDecoder();
    info_abc=info->GetInterface();
    info_cs=new SpokenContentExtractionInfoCS();
    info_cs->SetDescriptorInterface(info_abc);
    info_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    info_cs->StartEncode();
  }
#endif
  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentLexiconCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentLexiconCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentLexiconCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the id string
  unsigned char li;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&li),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  char *t=new char[li+1];
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (t),
                           8*static_cast<long> (li)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (li)*sizeof(char));
  t[li]='\0';
  GetDescriptorInterface()->SetLexiconID(t);
  delete [] t;

  // Is there a numOriginalEntries?
  bool flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    unsigned long n;
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                             8*sizeof(unsigned long));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
    GetDescriptorInterface()->SetLexiconNumOriginalEntries(n);
  }

#if 0 // PNG
  // Is there a Decoder?
  flag=(m_DecoderBuffer->GetBit(1) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    SpokenContentExtractionInfoDescriptor*   info;
    SpokenContentExtractionInfoDescriptorInterfaceABC* info_abc;
    SpokenContentExtractionInfoCS* info_cs;
    info=new SpokenContentExtractionInfoDescriptor();
    info_abc=info->GetInterface();
    info_cs=new SpokenContentExtractionInfoCS(info_abc);
    info_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    info_cs->StartDecode();
    GetDescriptorInterface()->SetLexiconDecoder(info);
  }
#endif

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentLexiconCSInterfaceABC *SpokenContentLexiconCS::GetInterface(void)
{ return &m_Interface; }



