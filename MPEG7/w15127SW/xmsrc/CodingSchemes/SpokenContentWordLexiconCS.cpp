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
// Applicable File Name:  SpokenContentWordLexiconCS.cpp
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

const UUID SpokenContentWordLexiconCSInterface::myID = UUID("");
const char *SpokenContentWordLexiconCSInterface::myName =
                         "SpokenContentWordLexicon Type Coding Engine Interface";

const UUID SpokenContentWordLexiconCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconCS::myName = "SpokenContentWordLexicon Type Coding Scheme";

const UUID SpokenContentWordLexiconCS::valID = UUID("");
const char *SpokenContentWordLexiconCS::valName = "";

const UUID SpokenContentWordLexiconCSInterfaceABC::myID = UUID("");

SpokenContentWordLexiconCSInterfaceABC::SpokenContentWordLexiconCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentWordLexiconCSInterface::SpokenContentWordLexiconCSInterface(SpokenContentWordLexiconCS
						     *aSpokenContentWordLexicon):
m_SpokenContentWordLexiconCS(aSpokenContentWordLexicon)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconCSInterface::~SpokenContentWordLexiconCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentWordLexiconCSInterface::destroy(void)
{ delete m_SpokenContentWordLexiconCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordLexiconCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentWordLexiconCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordLexiconCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentWordLexiconCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentWordLexiconCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentWordLexiconCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterfaceABC
*SpokenContentWordLexiconCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentWordLexiconCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCSInterface::
SetDescriptorInterface(SpokenContentWordLexiconDescriptorInterfaceABC
		       *aSpokenContentWordLexiconDescriptorInterface)
{
        return m_SpokenContentWordLexiconCS->
	  SetDescriptorInterface(aSpokenContentWordLexiconDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCSInterface::StartEncode(void)
{
        return m_SpokenContentWordLexiconCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentWordLexiconCSInterface::StartDecode(void)
{
        return m_SpokenContentWordLexiconCS->StartDecode();
}
//=============================================================================

SpokenContentWordLexiconCS::SpokenContentWordLexiconCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentWordLexiconDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentWordLexiconCS::SpokenContentWordLexiconCS(SpokenContentWordLexiconDescriptorInterfaceABC* SpokenContentWordLexicon):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentWordLexiconDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentWordLexicon) {
	  SpokenContentWordLexiconDescriptor *descriptor =
	    new SpokenContentWordLexiconDescriptor();
	  SpokenContentWordLexicon=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentWordLexicon);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconCS::~SpokenContentWordLexiconCS()
{
  // release descriptor
  if (m_SpokenContentWordLexiconDescriptorInterface) {
    m_SpokenContentWordLexiconDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLexiconCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordLexiconCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordLexiconCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentWordLexiconCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterfaceABC *SpokenContentWordLexiconCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentWordLexiconDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentWordLexiconCS::
SetDescriptorInterface(SpokenContentWordLexiconDescriptorInterfaceABC
		       *aSpokenContentWordLexiconDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentWordLexiconDescriptorInterface == 
	    aSpokenContentWordLexiconDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentWordLexiconDescriptorInterface) 
	  m_SpokenContentWordLexiconDescriptorInterface->release();

	// add new interface
	m_SpokenContentWordLexiconDescriptorInterface = 
	  aSpokenContentWordLexiconDescriptorInterface;
	if (m_SpokenContentWordLexiconDescriptorInterface) {
	  m_SpokenContentWordLexiconDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentWordLexiconCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentWordLexiconCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // Handle the words - assume all words are less that 255 characters in length
  unsigned long n=GetDescriptorInterface()->GetNWords();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(unsigned long i=0;i<n;i++) {
    // Assume each word is of length 255 chars or less
    char* w=const_cast<char*>(GetDescriptorInterface()->GetWord(i));
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
int SpokenContentWordLexiconCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentWordLexiconCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentWordLexiconDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentWordLexiconCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Handle the words - assume all words are less that 255 characters in length
  unsigned long n;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  char *b=new char[256];
  GetDescriptorInterface()->SetNWords(n);
  for(unsigned long i=0;i<n;i++) {
    unsigned char wl;
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&wl),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (wl)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (wl)*sizeof(char));
    b[wl]='\0';
    GetDescriptorInterface()->SetWord(i,b);
  }
  delete [] b;

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
SpokenContentWordLexiconCSInterfaceABC *SpokenContentWordLexiconCS::GetInterface(void)
{ return &m_Interface; }



