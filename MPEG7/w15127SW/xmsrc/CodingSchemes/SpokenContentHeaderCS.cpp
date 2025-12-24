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
// Applicable File Name:  SpokenContentHeaderCS.cpp
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

const UUID SpokenContentHeaderCSInterface::myID = UUID("");
const char *SpokenContentHeaderCSInterface::myName =
                         "SpokenContentHeader Type Coding Engine Interface";

const UUID SpokenContentHeaderCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderCS::myName = "SpokenContentHeader Type Coding Scheme";

const UUID SpokenContentHeaderCS::valID = UUID("");
const char *SpokenContentHeaderCS::valName = "";

const UUID SpokenContentHeaderCSInterfaceABC::myID = UUID("");

SpokenContentHeaderCSInterfaceABC::SpokenContentHeaderCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentHeaderCSInterface::SpokenContentHeaderCSInterface(SpokenContentHeaderCS
						     *aSpokenContentHeader):
m_SpokenContentHeaderCS(aSpokenContentHeader)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentHeaderCSInterface::~SpokenContentHeaderCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentHeaderCSInterface::destroy(void)
{ delete m_SpokenContentHeaderCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentHeaderCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentHeaderCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentHeaderCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentHeaderCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentHeaderCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentHeaderCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterfaceABC
*SpokenContentHeaderCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentHeaderCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCSInterface::
SetDescriptorInterface(SpokenContentHeaderDescriptorInterfaceABC
		       *aSpokenContentHeaderDescriptorInterface)
{
        return m_SpokenContentHeaderCS->
	  SetDescriptorInterface(aSpokenContentHeaderDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCSInterface::StartEncode(void)
{
        return m_SpokenContentHeaderCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentHeaderCSInterface::StartDecode(void)
{
        return m_SpokenContentHeaderCS->StartDecode();
}
//=============================================================================

SpokenContentHeaderCS::SpokenContentHeaderCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentHeaderDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentHeaderCS::SpokenContentHeaderCS(SpokenContentHeaderDescriptorInterfaceABC* SpokenContentHeader):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentHeaderDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentHeader) {
	  SpokenContentHeaderDescriptor *descriptor =
	    new SpokenContentHeaderDescriptor();
	  SpokenContentHeader=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentHeader);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentHeaderCS::~SpokenContentHeaderCS()
{
  // release descriptor
  if (m_SpokenContentHeaderDescriptorInterface) {
    m_SpokenContentHeaderDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentHeaderCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentHeaderCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentHeaderCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentHeaderCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterfaceABC *SpokenContentHeaderCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentHeaderDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentHeaderCS::
SetDescriptorInterface(SpokenContentHeaderDescriptorInterfaceABC
		       *aSpokenContentHeaderDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentHeaderDescriptorInterface == 
	    aSpokenContentHeaderDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentHeaderDescriptorInterface) 
	  m_SpokenContentHeaderDescriptorInterface->release();

	// add new interface
	m_SpokenContentHeaderDescriptorInterface = 
	  aSpokenContentHeaderDescriptorInterface;
	if (m_SpokenContentHeaderDescriptorInterface) {
	  m_SpokenContentHeaderDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentHeaderCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentHeaderCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentHeaderDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }
  unsigned long i,n;

  // Handle the word lexicon
  n=GetDescriptorInterface()->GetNWordLexicon();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(i=0;i<n;i++) {
    SpokenContentWordLexiconDescriptor* wlx;
    SpokenContentWordLexiconDescriptorInterfaceABC* wlx_abc;
    SpokenContentWordLexiconCS* wlx_cs;
    wlx=GetDescriptorInterface()->GetWordLexicon(i);
    wlx_abc=wlx->GetInterface();
    wlx_cs=new SpokenContentWordLexiconCS();
    wlx_cs->SetDescriptorInterface(wlx_abc);
    wlx_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    wlx_cs->StartEncode();
  }

  // Handle the phone lexicon
  n=GetDescriptorInterface()->GetNPhoneLexicon();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(i=0;i<n;i++) {
    SpokenContentPhoneLexiconDescriptor* plx;
    SpokenContentPhoneLexiconDescriptorInterfaceABC* plx_abc;
    SpokenContentPhoneLexiconCS* plx_cs;
    plx=GetDescriptorInterface()->GetPhoneLexicon(i);
    plx_abc=plx->GetInterface();
    plx_cs=new SpokenContentPhoneLexiconCS();
    plx_cs->SetDescriptorInterface(plx_abc);
    plx_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    plx_cs->StartEncode();
  }

  // Handle the speakers
  n=GetDescriptorInterface()->GetNSpeaker();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(i=0;i<n;i++) {
    SpokenContentSpeakerDescriptor* spk;
    SpokenContentSpeakerDescriptorInterfaceABC* spk_abc;
    SpokenContentSpeakerCS* spk_cs;
    spk=GetDescriptorInterface()->GetSpeaker(i);
    spk_abc=spk->GetInterface();
    spk_cs=new SpokenContentSpeakerCS();
    spk_cs->SetDescriptorInterface(spk_abc);
    spk_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    spk_cs->StartEncode();
  }

  // Handle the confusions
  n=GetDescriptorInterface()->GetNConfusions();
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  for(i=0;i<n;i++) {
    SpokenContentConfusionInfoDescriptor* ci;
    SpokenContentConfusionInfoDescriptorInterfaceABC* ci_abc;
    SpokenContentConfusionInfoCS* ci_cs;
    ci=GetDescriptorInterface()->GetConfusion(i);
    ci_abc=ci->GetInterface();
    ci_cs=new SpokenContentConfusionInfoCS();
    ci_cs->SetDescriptorInterface(ci_abc);
    ci_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    ci_cs->StartEncode();
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentHeaderCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentHeaderCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentHeaderDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentHeaderCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // Get the word lexicons
  unsigned long i,n;
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNWordLexicon(n);
  for(i=0;i<n;i++) {
    SpokenContentWordLexiconDescriptor*   wlx;
    SpokenContentWordLexiconDescriptorInterfaceABC* wlx_abc;
    SpokenContentWordLexiconCS* wlx_cs;
    wlx=new SpokenContentWordLexiconDescriptor();
    wlx_abc=wlx->GetInterface();
    wlx_cs=new SpokenContentWordLexiconCS(wlx_abc);
    wlx_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    wlx_cs->StartDecode();
    GetDescriptorInterface()->SetWordLexicon(i,wlx);
  }

  // Get the phone lexicons
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNPhoneLexicon(n);
  for(i=0;i<n;i++) {
    SpokenContentPhoneLexiconDescriptor*   plx;
    SpokenContentPhoneLexiconDescriptorInterfaceABC* plx_abc;
    SpokenContentPhoneLexiconCS* plx_cs;
    plx=new SpokenContentPhoneLexiconDescriptor();
    plx_abc=plx->GetInterface();
    plx_cs=new SpokenContentPhoneLexiconCS(plx_abc);
    plx_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    plx_cs->StartDecode();
    GetDescriptorInterface()->SetPhoneLexicon(i,plx);
  }

  // Get the speakers
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNSpeaker(n);
  for(i=0;i<n;i++) {
    SpokenContentSpeakerDescriptor*   spk;
    SpokenContentSpeakerDescriptorInterfaceABC* spk_abc;
    SpokenContentSpeakerCS* spk_cs;
    spk=new SpokenContentSpeakerDescriptor();
    spk_abc=spk->GetInterface();
    spk_cs=new SpokenContentSpeakerCS(spk_abc);
    spk_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    spk_cs->StartDecode();
    GetDescriptorInterface()->SetSpeaker(i,spk);
  }

  // Get the speakers
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                           8*sizeof(unsigned long));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
  GetDescriptorInterface()->SetNConfusions(n);
  for(i=0;i<n;i++) {
    SpokenContentConfusionInfoDescriptor*   ci;
    SpokenContentConfusionInfoDescriptorInterfaceABC* ci_abc;
    SpokenContentConfusionInfoCS* ci_cs;
    ci=new SpokenContentConfusionInfoDescriptor();
    ci_abc=ci->GetInterface();
    ci_cs=new SpokenContentConfusionInfoCS(ci_abc);
    ci_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    ci_cs->StartDecode();
    GetDescriptorInterface()->SetConfusion(i,ci);
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentHeaderCSInterfaceABC *SpokenContentHeaderCS::GetInterface(void)
{ return &m_Interface; }



