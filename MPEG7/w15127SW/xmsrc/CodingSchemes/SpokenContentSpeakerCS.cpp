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
// Applicable File Name:  SpokenContentSpeakerCS.cpp
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

const UUID SpokenContentSpeakerCSInterface::myID = UUID("");
const char *SpokenContentSpeakerCSInterface::myName =
                         "SpokenContentSpeaker Type Coding Engine Interface";

const UUID SpokenContentSpeakerCS::myID = UUID("682c9b80-db31-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerCS::myName = "SpokenContentSpeaker Type Coding Scheme";

const UUID SpokenContentSpeakerCS::valID = UUID("");
const char *SpokenContentSpeakerCS::valName = "";

const UUID SpokenContentSpeakerCSInterfaceABC::myID = UUID("");

SpokenContentSpeakerCSInterfaceABC::SpokenContentSpeakerCSInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
SpokenContentSpeakerCSInterface::SpokenContentSpeakerCSInterface(SpokenContentSpeakerCS
						     *aSpokenContentSpeaker):
m_SpokenContentSpeakerCS(aSpokenContentSpeaker)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerCSInterface::~SpokenContentSpeakerCSInterface()
{
}


//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerCSInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerCSInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentSpeakerCSInterface::destroy(void)
{ delete m_SpokenContentSpeakerCS; }

//----------------------------------------------------------------------------
BitBuffer *SpokenContentSpeakerCSInterface::GetEncoderStreamBuffer(void)
{
        return m_SpokenContentSpeakerCS->GetEncoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCSInterface::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentSpeakerCS->SetEncoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
BitBuffer *SpokenContentSpeakerCSInterface::GetDecoderStreamBuffer(void)
{
        return m_SpokenContentSpeakerCS->GetDecoderStreamBuffer();
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCSInterface::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        return m_SpokenContentSpeakerCS->SetDecoderStreamBuffer(aBuffer);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterfaceABC
*SpokenContentSpeakerCSInterface::GetDescriptorInterface(void)

{
        return m_SpokenContentSpeakerCS->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCSInterface::
SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface)
{
        return m_SpokenContentSpeakerCS->
	  SetDescriptorInterface(aSpokenContentSpeakerDescriptorInterface);
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCSInterface::StartEncode(void)
{
        return m_SpokenContentSpeakerCS->StartEncode();
}
//----------------------------------------------------------------------------
int SpokenContentSpeakerCSInterface::StartDecode(void)
{
        return m_SpokenContentSpeakerCS->StartDecode();
}
//=============================================================================

SpokenContentSpeakerCS::SpokenContentSpeakerCS():
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(0),
m_DecoderBuffer(0),
m_SpokenContentSpeakerDescriptorInterface(0)
{
#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif
	SetExposedInterface(&m_Interface);
}

//=============================================================================

SpokenContentSpeakerCS::SpokenContentSpeakerCS(SpokenContentSpeakerDescriptorInterfaceABC* SpokenContentSpeaker):
m_Interface(this),
m_isProprietary(false),
m_EncoderBuffer(NULL),
m_DecoderBuffer(NULL),
m_SpokenContentSpeakerDescriptorInterface(NULL)
{

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors\n");
#endif

	// create descriptor if it does not exist
	if (!SpokenContentSpeaker) {
	  SpokenContentSpeakerDescriptor *descriptor =
	    new SpokenContentSpeakerDescriptor();
	  SpokenContentSpeaker=descriptor->GetInterface();
	}

	// connect Descritors with coding schemes
	SetDescriptorInterface(SpokenContentSpeaker);
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerCS::~SpokenContentSpeakerCS()
{
  // release descriptor
  if (m_SpokenContentSpeakerDescriptorInterface) {
    m_SpokenContentSpeakerDescriptorInterface->release();
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerCS::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerCS::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerCS::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerCS::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentSpeakerCS::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
BitBuffer * SpokenContentSpeakerCS::GetEncoderStreamBuffer(void)
{
	return m_EncoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCS::SetEncoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_EncoderBuffer = aBuffer;
	return 0;
}

//----------------------------------------------------------------------------
BitBuffer * SpokenContentSpeakerCS::GetDecoderStreamBuffer(void)
{
	return m_DecoderBuffer;
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCS::SetDecoderStreamBuffer(BitBuffer *aBuffer)
{
        if (!aBuffer) return -1;

	m_DecoderBuffer = aBuffer;

	return 0;
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterfaceABC *SpokenContentSpeakerCS::
GetDescriptorInterface(void)
{
       return m_SpokenContentSpeakerDescriptorInterface;
}

//----------------------------------------------------------------------------
int SpokenContentSpeakerCS::
SetDescriptorInterface(SpokenContentSpeakerDescriptorInterfaceABC
		       *aSpokenContentSpeakerDescriptorInterface)
{
        // check if new value is different from old value
        if (m_SpokenContentSpeakerDescriptorInterface == 
	    aSpokenContentSpeakerDescriptorInterface) return 0;
	
	// release old interface
	if (m_SpokenContentSpeakerDescriptorInterface) 
	  m_SpokenContentSpeakerDescriptorInterface->release();

	// add new interface
	m_SpokenContentSpeakerDescriptorInterface = 
	  aSpokenContentSpeakerDescriptorInterface;
	if (m_SpokenContentSpeakerDescriptorInterface) {
	  m_SpokenContentSpeakerDescriptorInterface->addref();
	}
	return 0;
}


//----------------------------------------------------------------------------
// This writes the contents of the D/DSType to file as a sequence of bits
int SpokenContentSpeakerCS::StartEncode(void)
{
  // To help debugging
  const char* routine="SpokenContentSpeakerCS::StartEncode:";

  // Is the file IO set up? If not return
  if(m_EncoderBuffer==NULL) {
    fprintf(stderr,"%s Buffer not initialised\n",routine);
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentSpeakerDescriptorInterface==NULL) {
    fprintf(stderr,"%s Interface not initialised\n",routine);
    return -1;
  }

  // ---------------- Handle the attributes first ----------
  unsigned long i,n;
  char* pn;
  unsigned char pl;
  pn=const_cast<char*>(GetDescriptorInterface()->GetPhoneLexiconID());
  if(pn==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    pl=strlen(pn);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                             8*pl*sizeof(char));
  }

  pn=const_cast<char*>(GetDescriptorInterface()->GetWordLexiconID());
  if(pn==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    pl=strlen(pn);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                             8*pl*sizeof(char));
  }

  pn=const_cast<char*>(GetDescriptorInterface()->GetPhoneConfID());
  if(pn==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    pl=strlen(pn);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                             8*pl*sizeof(char));
  }

  pn=const_cast<char*>(GetDescriptorInterface()->GetDefaultWordLatticeId());
  if(pn==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    pl=strlen(pn);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                             8*pl*sizeof(char));
  }

  pn=const_cast<char*>(GetDescriptorInterface()->GetDefaultPhoneLatticeId());
  if(pn==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    pl=strlen(pn);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                             8*sizeof(unsigned char));
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                             8*pl*sizeof(char));
  }

  pn=const_cast<char*>(GetDescriptorInterface()->GetLang());
  pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*pl*sizeof(char));

  pn=const_cast<char*>(GetDescriptorInterface()->GetSpeakerID());
  pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*pl*sizeof(char));

  pn=const_cast<char*>(GetDescriptorInterface()->GetProvenance());
  pl=strlen(pn);
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&pl),
                           8*sizeof(unsigned char));
  m_EncoderBuffer->PutBits(reinterpret_cast<char*> (pn),
                           8*pl*sizeof(char));
 
  // Now write out the individual component if it exists
#if 0 // PNG 
  if(GetDescriptorInterface()->GetIndividual()==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    IndividualDescriptor* ind;
    IndividualDescriptorInterfaceABC* ind_abc;
    IndividualCS* ind_cs;
    ind=GetDescriptorInterface()->GetIndividual();
    ind_abc=ind->GetInterface();
    ind_cs=new IndividualCS();
    ind_cs->SetDescriptorInterface(ind_abc);
    ind_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    ind_cs->StartEncode();
  }
#endif

  // Now write out the extractioninfo component if it exists
#if 0 // PNG 
  if(GetDescriptorInterface()->GetExtractionInfo()==NULL) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    SpokenContentExtractionInfoDescriptor* ext;
    SpokenContentExtractionInfoDescriptorInterfaceABC* ext_abc;
    SpokenContentExtractionInfoCS* ext_cs;
    ext=GetDescriptorInterface()->GetExtractionInfo();
    ext_abc=ext->GetInterface();
    ext_cs=new SpokenContentExtractionInfoCS();
    ext_cs->SetDescriptorInterface(ext_abc);
    ext_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
    ext_cs->StartEncode();
  }
#endif

  // If there's a word lexicon write it our
  if((n=GetDescriptorInterface()->GetNWordIndex())==0) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                             8*sizeof(unsigned long));
    for(i=0;i<n;i++) {
      SpokenContentWordIndexEntryDescriptor* win;
      SpokenContentWordIndexEntryDescriptorInterfaceABC* win_abc;
      SpokenContentWordIndexEntryCS* win_cs;
      win=GetDescriptorInterface()->GetWordIndex(i);
      win_abc=win->GetInterface();
      win_cs=new SpokenContentWordIndexEntryCS();
      win_cs->SetDescriptorInterface(win_abc);
      win_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
      win_cs->StartEncode();
    }
  }

  // If there's a Phone lexicon write it our
  if((n=GetDescriptorInterface()->GetNPhoneIndex())==0) {
    m_EncoderBuffer->PutBit(0);
  }
  else {
    m_EncoderBuffer->PutBit(1);
    m_EncoderBuffer->PutBits(reinterpret_cast<char*> (&n),
                             8*sizeof(unsigned long));
    for(i=0;i<n;i++) {
      SpokenContentPhoneIndexEntryDescriptor* pin;
      SpokenContentPhoneIndexEntryDescriptorInterfaceABC* pin_abc;
      SpokenContentPhoneIndexEntryCS* pin_cs;
      pin=GetDescriptorInterface()->GetPhoneIndex(i);
      pin_abc=pin->GetInterface();
      pin_cs=new SpokenContentPhoneIndexEntryCS();
      pin_cs->SetDescriptorInterface(pin_abc);
      pin_cs->SetEncoderStreamBuffer(m_EncoderBuffer);
      pin_cs->StartEncode();
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
// This reads the contents of the D/DSType from file
int SpokenContentSpeakerCS::StartDecode(void)
{
  // Is the file IO set up? If not return
  if(m_DecoderBuffer==NULL) {
    fprintf(stderr,"SpokenContentSpeakerCS::StartDecode: Buffer not initialised\n");
    return(-1);
  }

  // Is the interface set up?
  if(m_SpokenContentSpeakerDescriptorInterface==NULL) {
    fprintf(stderr,"SpokenContentSpeakerCS::StartDecode: Interface not initialised\n");
    return -1;
  }

  // ---------------- Handle the attributes first ----------
  unsigned long i,n;
  unsigned char l;
  char b[256];
  bool flag;
  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetPhoneLexiconID(b);
  }
  else {
    GetDescriptorInterface()->SetPhoneLexiconID(NULL);
  }

  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetWordLexiconID(b);
  }
  else {
    GetDescriptorInterface()->SetWordLexiconID(NULL);
  }

  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetPhoneConfID(b);
  }
  else {
    GetDescriptorInterface()->SetPhoneConfID(NULL);
  }

  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetDefaultWordLatticeId(b);
  }
  else {
    GetDescriptorInterface()->SetDefaultWordLatticeId(NULL);
  }

  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                             8*sizeof(unsigned char));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                             8*static_cast<long> (l)*sizeof(char));
    m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
    b[l]='\0';
    GetDescriptorInterface()->SetDefaultPhoneLatticeId(b);
  }
  else {
    GetDescriptorInterface()->SetDefaultPhoneLatticeId(NULL);
  }

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetLang(b);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetSpeakerID(b);

  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&l),
                           8*sizeof(unsigned char));
  m_DecoderBuffer->TakeBits(8*sizeof(unsigned char));
  m_DecoderBuffer->GetBits(reinterpret_cast<char*> (b),
                           8*static_cast<long> (l)*sizeof(char));
  m_DecoderBuffer->TakeBits(8*static_cast<long> (l)*sizeof(char));
  b[l]='\0';
  GetDescriptorInterface()->SetProvenance(b);

  // Now do the individual
#if 0 // PNG 
  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    IndividualDescriptor*   ind;
    IndividualDescriptorInterfaceABC* ind_abc;
    IndividualCS* ind_cs;
    ind=new IndividualDescriptor();
    ind_abc=ind->GetInterface();
    ind_cs=new IndividualCS(ind_abc);
    ind_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    ind_cs->StartDecode();
    GetDescriptorInterface()->SetIndividual(ind);
  }
  else {
    GetDescriptorInterface()->SetIndividual(NULL);
  }
#endif

  // Now do the extraction info
#if 0 // PNG 
  flag=(m_DecoderBuffer->GetBit(0) != 0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    SpokenContentExtractionInfoDescriptor*   ext;
    SpokenContentExtractionInfoDescriptorInterfaceABC* ext_abc;
    SpokenContentExtractionInfoCS* ext_cs;
    ext=new SpokenContentExtractionInfoDescriptor();
    ext_abc=ext->GetInterface();
    ext_cs=new SpokenContentExtractionInfoCS(ext_abc);
    ext_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
    ext_cs->StartDecode();
    GetDescriptorInterface()->SetExtractionInfo(ext);
  }
  else {
    GetDescriptorInterface()->SetExtractionInfo(NULL);
  }
#endif

  // Now do the word index if there is one
 flag=(m_DecoderBuffer->GetBit(0)!=0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                             8*sizeof(unsigned long));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
    GetDescriptorInterface()->SetNWordIndex(n);
    for(i=0;i<n;i++) {
      SpokenContentWordIndexEntryDescriptor*   win;
      SpokenContentWordIndexEntryDescriptorInterfaceABC* win_abc;
      SpokenContentWordIndexEntryCS* win_cs;
      win=new SpokenContentWordIndexEntryDescriptor();
      win_abc=win->GetInterface();
      win_cs=new SpokenContentWordIndexEntryCS(win_abc);
      win_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
      win_cs->StartDecode();
      GetDescriptorInterface()->SetWordIndex(i,win);
    }
  }

  // Now do the phone index if there is one
  flag=(m_DecoderBuffer->GetBit(0)!=0);
  m_DecoderBuffer->TakeBits(1);
  if(flag) {
    m_DecoderBuffer->GetBits(reinterpret_cast<char*> (&n),
                             8*sizeof(unsigned long));
    m_DecoderBuffer->TakeBits(8*sizeof(unsigned long));
    GetDescriptorInterface()->SetNPhoneIndex(n);
    for(i=0;i<n;i++) {
      SpokenContentPhoneIndexEntryDescriptor*   pin;
      SpokenContentPhoneIndexEntryDescriptorInterfaceABC* pin_abc;
      SpokenContentPhoneIndexEntryCS* pin_cs;
      pin=new SpokenContentPhoneIndexEntryDescriptor();
      pin_abc=pin->GetInterface();
      pin_cs=new SpokenContentPhoneIndexEntryCS(pin_abc);
      pin_cs->SetDecoderStreamBuffer(m_DecoderBuffer);
      pin_cs->StartDecode();
      GetDescriptorInterface()->SetPhoneIndex(i,pin);
    }
  }

  // All done
  return 0;
}

//----------------------------------------------------------------------------
SpokenContentSpeakerCSInterfaceABC *SpokenContentSpeakerCS::GetInterface(void)
{ return &m_Interface; }



