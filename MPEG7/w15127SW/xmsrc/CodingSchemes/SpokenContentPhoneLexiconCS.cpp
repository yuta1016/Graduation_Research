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
// Applicable File Name:  SpokenContentPhoneLexiconCS.cpp
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

const UUID SpokenContentPhoneLexiconCSInterface::myID = UUID("");
const char *SpokenContentPhoneLexiconCSInterface::myName =
                         "SpokenContentPhoneLexicon Type Coding Engine Interface";

const UUID SpokenContentPhoneLexiconCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconCS::myName = "SpokenContentPhoneLexicon Type Coding Scheme";

const UUID SpokenContentPhoneLexiconCS::valID = UUID("");
const char *SpokenContentPhoneLexiconCS::valName = "";

const UUID SpokenContentPhoneLexiconCSInterfaceABC::myID = UUID("");

SpokenContentPhoneLexiconCSInterfaceABC::SpokenContentPhoneLexiconCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentPhoneLexiconCSInterface::SpokenContentPhoneLexiconCSInterface(SpokenContentPhoneLexiconCS
						     *aSpokenContentPhoneLexicon):
m_SpokenContentPhoneLexiconCS(aSpokenContentPhoneLexicon)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconCSInterface::~SpokenContentPhoneLexiconCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconCSInterface::destroy(void)
{ delete m_SpokenContentPhoneLexiconCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneLexiconCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentPhoneLexiconCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneLexiconCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentPhoneLexiconCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentPhoneLexiconCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentPhoneLexiconCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterfaceABC
*SpokenContentPhoneLexiconCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentPhoneLexiconCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCSInterface::
SetDescriptorInterface(SpokenContentPhoneLexiconDescriptorInterfaceABC
		       *aSpokenContentPhoneLexiconDescriptorInterface)
{
        return m_SpokenContentPhoneLexiconCS->
	  SetDescriptorInterface(aSpokenContentPhoneLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCSInterface::StartEncode(void)
{
        return m_SpokenContentPhoneLexiconCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCSInterface::StartDecode(void)
{
        return m_SpokenContentPhoneLexiconCS->StartDecode();
}
//=============================================================================

SpokenContentPhoneLexiconCS::SpokenContentPhoneLexiconCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentPhoneLexiconDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentPhoneLexiconCS::SpokenContentPhoneLexiconCS(SpokenContentPhoneLexiconDescriptorInterfaceABC* SpokenContentPhoneLexicon):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentPhoneLexiconDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentPhoneLexicon) {
	  SpokenContentPhoneLexiconDescriptor *descriptor =
	    new SpokenContentPhoneLexiconDescriptor();
	  SpokenContentPhoneLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentPhoneLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconCS::~SpokenContentPhoneLexiconCS()
{
  // release descriptor
  if (m_SpokenContentPhoneLexiconDescriptorInterface) {
    m_SpokenContentPhoneLexiconDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneLexiconCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentPhoneLexiconCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterfaceABC *SpokenContentPhoneLexiconCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentPhoneLexiconDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentPhoneLexiconCS::
SetDescriptorInterface(SpokenContentPhoneLexiconDescriptorInterfaceABC
		       *aSpokenContentPhoneLexiconDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentPhoneLexiconDescriptorInterface == 
	    aSpokenContentPhoneLexiconDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentPhoneLexiconDescriptorInterface) 
	  m_SpokenContentPhoneLexiconDescriptorInterface->release();

	// add new interface
	m_SpokenContentPhoneLexiconDescriptorInterface = 
	  aSpokenContentPhoneLexiconDescriptorInterface;
	if (m_SpokenContentPhoneLexiconDescriptorInterface) {
	  m_SpokenContentPhoneLexiconDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentPhoneLexiconCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentPhoneLexiconCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the name
  char* pn=const_cast<char*>(GetDescriptorInterface()->GetPhonesetName());
  unsigned char pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*pl*sizeof(char));

  // Handle the phones - assume all phones are less that 255 characters in
  // length
  unsigned long n=GetDescriptorInterface()->GetNPhones();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(unsigned long i=0;i<n;i++) {
    // Assume each phone is of length 255 chars or less
    char* w=const_cast<char*>(GetDescriptorInterface()->GetPhone(i));
    unsigned char wl=strlen(w);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&wl),
                           8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (w),
                           8*static_cast<long> (wl)*sizeof(char));
  }

  // Now write out the lexicon component
  SpokenContentLexiconDescriptor* lex;
  SpokenContentLexiconDescriptorInterfaceABC* lex_abc;
  SpokenContentLexiconCS* lex_cs;
  lex=GetDescriptorInterface()->GetLexicon();
  lex_abc=lex->GetInterface();
  lex_cs=new SpokenContentLexiconCS();
  lex_cs->SetDescriptorInterface(lex_abc);
  lex_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
  lex_cs->StartEncode();
  
  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentPhoneLexiconCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentPhoneLexiconCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentPhoneLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentPhoneLexiconCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the phoneset name
  char b[256];
  unsigned char l;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*l*sizeof(char));
  m_DecoderBuffer->TakeBits(8*l*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetPhonesetName(b);

  // Handle the words - assume all words are less that 255 characters in length
  unsigned long n;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNPhones(n);
  for(unsigned long i=0;i<n;i++) {
    unsigned char wl;
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&wl),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (wl)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (wl)*sizeof(char));
    b[wl]='\0';
    GetDescriptorInterface()->SetPhone(i,b);
  }

  // Now do the lexicon
  SpokenContentLexiconDescriptor*   lex;
  SpokenContentLexiconDescriptorInterfaceABC* lex_abc;
  SpokenContentLexiconCS* lex_cs;
  lex=new SpokenContentLexiconDescriptor();
  lex_abc=lex->GetInterface();
  lex_cs=new SpokenContentLexiconCS(lex_abc);
  lex_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
  lex_cs->StartDecode();
  GetDescriptorInterface()->SetLexicon(lex);

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconCSInterfaceABC *SpokenContentPhoneLexiconCS::GetInterface(void)
{ return &m_Interface; }



