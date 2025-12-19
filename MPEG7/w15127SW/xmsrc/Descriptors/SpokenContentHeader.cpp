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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpokenContentHeader.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentHeaderDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderDescriptorInterface::myName = "SpokenContentHeader Type Description Interface";

const UUID SpokenContentHeaderDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderDescriptor::myName = "SpokenContentHeader Type Descriptor";

const UUID SpokenContentHeaderDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentHeaderDescriptor::valName = "";

const UUID SpokenContentHeaderDescriptorInterfaceABC::myID = UUID();

SpokenContentHeaderDescriptorInterfaceABC::SpokenContentHeaderDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentHeaderDescriptorInterface::SpokenContentHeaderDescriptorInterface(
  SpokenContentHeaderDescriptor *aHisto): m_SpokenContentHeaderDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterface::~SpokenContentHeaderDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::addref(void)
{ m_SpokenContentHeaderDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::release(void)
{ m_SpokenContentHeaderDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptorInterface::GetNWordLexicon(
  void) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetNWordLexicon());
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptor* SpokenContentHeaderDescriptorInterface::GetWordLexicon(const unsigned long idx)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetWordLexicon(idx));
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetNWordLexicon(const unsigned long n)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetNWordLexicon(n);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetWordLexicon(const unsigned long idx,SpokenContentWordLexiconDescriptor* l)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetWordLexicon(idx,l);
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptorInterface::GetNPhoneLexicon(
  void) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetNPhoneLexicon());
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptor* SpokenContentHeaderDescriptorInterface::GetPhoneLexicon(const unsigned long idx)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetPhoneLexicon(idx));
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetNPhoneLexicon(const unsigned long n)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetNPhoneLexicon(n);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetPhoneLexicon(const unsigned long idx,SpokenContentPhoneLexiconDescriptor* l)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetPhoneLexicon(idx,l);
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptorInterface::GetNSpeaker(
  void) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetNSpeaker());
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptor* SpokenContentHeaderDescriptorInterface::GetSpeaker(const unsigned long idx)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetSpeaker(idx));
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetNSpeaker(const unsigned long n)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetNSpeaker(n);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetSpeaker(const unsigned long idx,SpokenContentSpeakerDescriptor* l)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetSpeaker(idx,l);
}

//----------------------------------------------------------------------------
// Given an id, find the corresponding speaker. If you can't find it return -1.
// otherwise return its index.
long SpokenContentHeaderDescriptorInterface::FindSpeaker(const char* refid) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->FindSpeaker(refid));
}

//----------------------------------------------------------------------------
// Given an id, find the corresponding word lexicon. If you can't find 
// it return -1. otherwise return its index.
long SpokenContentHeaderDescriptorInterface::FindWordLexicon(const char* refid) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->FindWordLexicon(refid));
}

//----------------------------------------------------------------------------
// Given an id, find the corresponding phone lexicon. If you can't find 
// it return -1.otherwise return its index.
long SpokenContentHeaderDescriptorInterface::FindPhoneLexicon(const char* refid) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->FindPhoneLexicon(refid));
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->Reset();
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetNConfusions(
  const unsigned long n)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetNConfusions(n);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentHeaderDescriptorInterface::GetNConfusions(void)
  const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetNConfusions());
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptorInterface::SetConfusion(
  const unsigned long idx,SpokenContentConfusionInfoDescriptor* l)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  m_SpokenContentHeaderDescriptor->SetConfusion(idx,l);
}

//----------------------------------------------------------------------------
long SpokenContentHeaderDescriptorInterface::FindConfusion(const char* refid) 
  const
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->FindConfusion(refid));
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptor* 
 SpokenContentHeaderDescriptorInterface::GetConfusion(const unsigned long idx)
{
  assert(m_SpokenContentHeaderDescriptor != NULL);
  return(m_SpokenContentHeaderDescriptor->GetConfusion(idx));
}


//=============================================================================
SpokenContentHeaderDescriptor::SpokenContentHeaderDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nwordlexicon(0),
nphonelexicon(0),
nspeaker(0),
nconfusions(0),
wordlexicon(NULL),
phonelexicon(NULL),
speaker(NULL),
confusions(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentHeaderDescriptor::SpokenContentHeaderDescriptor(SpokenContentHeaderDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nwordlexicon(0),
nphonelexicon(0),
nspeaker(0),
nconfusions(0),
wordlexicon(NULL),
phonelexicon(NULL),
speaker(NULL),
confusions(NULL)
{
  SetExposedInterface(&m_Interface);

  SetNWordLexicon(data.GetNWordLexicon());
  SetNPhoneLexicon(data.GetNPhoneLexicon());
  SetNSpeaker(data.GetNSpeaker());
  SetNConfusions(data.GetNConfusions());
  unsigned long i;
  for(i=0;i<data.GetNWordLexicon();i++) {
    SetWordLexicon(i,data.GetWordLexicon(i));
  }
  for(i=0;i<data.GetNPhoneLexicon();i++) {
    SetPhoneLexicon(i,data.GetPhoneLexicon(i));
  }
  for(i=0;i<data.GetNSpeaker();i++) {
    SetSpeaker(i,data.GetSpeaker(i));
  }
  for(i=0;i<data.GetNConfusions();i++) {
    SetConfusion(i,data.GetConfusion(i));
  }

}


//----------------------------------------------------------------------------
SpokenContentHeaderDescriptor::~SpokenContentHeaderDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentHeader delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptor::GetNWordLexicon(
  void) const
{
  return(nwordlexicon);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptor* SpokenContentHeaderDescriptor::GetWordLexicon(const unsigned long idx)
{
  assert(wordlexicon != NULL);
  assert(idx<nwordlexicon);
  assert(wordlexicon[idx] != NULL);
  return(wordlexicon[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetNWordLexicon(const unsigned long n)
{
  unsigned long i;
  if(wordlexicon != NULL) {
    for(i=0;i<nwordlexicon;i++) {
      if(wordlexicon[i] != NULL) {
        wordlexicon[i]->release();
        wordlexicon[i]=NULL;
      }
    }
    delete [] wordlexicon;
    wordlexicon=NULL;
  }
  nwordlexicon=n;
  if(n != 0) {
    wordlexicon=new SpokenContentWordLexiconDescriptor*[n];
    for(i=0;i<n;i++) wordlexicon[i]=NULL;
  }
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetWordLexicon(const unsigned long idx,SpokenContentWordLexiconDescriptor* l)
{
  assert(wordlexicon != NULL);
  assert(idx<nwordlexicon);
  assert(l != NULL);
  if(wordlexicon[idx] != NULL) wordlexicon[idx]->release();
  wordlexicon[idx]=l;
  wordlexicon[idx]->addref();
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptor::GetNPhoneLexicon(
  void) const
{
  return(nphonelexicon);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptor* SpokenContentHeaderDescriptor::GetPhoneLexicon(const unsigned long idx)
{
  assert(phonelexicon != NULL);
  assert(idx<nphonelexicon);
  assert(phonelexicon[idx] != NULL);
  return(phonelexicon[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetNPhoneLexicon(const unsigned long n)
{
  unsigned long i;
  if(phonelexicon != NULL) {
    for(i=0;i<nphonelexicon;i++) {
      if(phonelexicon[i] != NULL) {
        phonelexicon[i]->release();
        phonelexicon[i]=NULL;
      }
    }
    delete [] phonelexicon;
    phonelexicon=NULL;
  }
  nphonelexicon=n;
  if(n != 0) {
    phonelexicon=new SpokenContentPhoneLexiconDescriptor*[n];
    for(i=0;i<n;i++) phonelexicon[i]=NULL;
  }
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetPhoneLexicon(const unsigned long idx,SpokenContentPhoneLexiconDescriptor* l)
{
  assert(phonelexicon != NULL);
  assert(idx<nphonelexicon);
  assert(l != NULL);
  if(phonelexicon[idx] != NULL) phonelexicon[idx]->release();
  phonelexicon[idx]=l;
  phonelexicon[idx]->addref();
}


//----------------------------------------------------------------------------
const unsigned long SpokenContentHeaderDescriptor::GetNSpeaker(
  void) const
{
  return(nspeaker);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptor* SpokenContentHeaderDescriptor::GetSpeaker(const unsigned long idx)
{
  assert(speaker != NULL);
  assert(idx<nspeaker);
  assert(speaker[idx] != NULL);
  return(speaker[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetNSpeaker(const unsigned long n)
{
  assert(n != 0);
  unsigned long i;
  if(speaker != NULL) {
    for(i=0;i<nspeaker;i++) {
      if(speaker[i] != NULL) {
        speaker[i]->release();
        speaker[i]=NULL;
      }
    }
    delete [] speaker;
    speaker=NULL;
  }
  nspeaker=n;
  speaker=new SpokenContentSpeakerDescriptor*[n];
  for(i=0;i<n;i++) speaker[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetSpeaker(const unsigned long idx,SpokenContentSpeakerDescriptor* l)
{
  assert(speaker != NULL);
  assert(idx<nspeaker);
  assert(l != NULL);
  if(speaker[idx] != NULL) speaker[idx]->release();
  speaker[idx]=l;
  speaker[idx]->addref();
}


//----------------------------------------------------------------------------
// Given an id, find the corresponding speaker. If you can't find it return -1.
// otherwise return its index.
long SpokenContentHeaderDescriptor::FindSpeaker(const char* refid) const
{
  assert(refid != NULL);
  if(refid[0] == '#')
  {
	  for(unsigned long i=0;i<nspeaker;i++) {
		if(strcmp(&refid[1],speaker[i]->GetSpeakerID())==0) {
		  return(static_cast<long> (i));
		}
	  }
  }
  //sth to stdout not allowed
  printf("Looking for [%s] in %ld: ",refid,nspeaker);
  for(int j=0;j<nspeaker;j++) printf("[%s] ",speaker[j]->GetSpeakerID());//sth
  printf("\n");//sth
  return(-1);
}

//----------------------------------------------------------------------------
// Given an id, find the corresponding word lexicon. If you can't find 
// it return -1. otherwise return its index.
long SpokenContentHeaderDescriptor::FindWordLexicon(const char* refid) const
{
  assert(refid != NULL);
  if(refid[0] == '#')
  {
	  for(unsigned long i=0;i<nwordlexicon;i++) {
		if(strcmp(&refid[1],wordlexicon[i]->GetLexicon()->GetLexiconID())==0) {
		  return(static_cast<long> (i));
		}
	  }
  }
  //sth to stdout not allowed 
  fprintf(stderr, "Looking for [%s] in %d: ",refid,nwordlexicon);
  for(int j=0;j<nwordlexicon;j++) fprintf(stderr, "[%s] ",wordlexicon[j]->GetLexicon()->GetLexiconID());//sth
  fprintf(stderr, "\n");//sth
  return(-1);
}

//----------------------------------------------------------------------------
// Given an id, find the corresponding phone lexicon. If you can't find 
// it return -1.otherwise return its index.
long SpokenContentHeaderDescriptor::FindPhoneLexicon(const char* refid) const
{
  assert(refid != NULL);
  if(refid[0] == '#')
  {
	  for(unsigned long i=0;i<nphonelexicon;i++) {
		if(strcmp(&refid[1],phonelexicon[i]->GetLexicon()->GetLexiconID())==0) {
		  return(static_cast<long> (i));
		}
	  }
  }
  //sth to stdout not allowed
  fprintf(stderr, "Looking for [%s] in %d: ",refid,nphonelexicon);
  for(int j=0;j<nphonelexicon;j++) fprintf(stderr, "[%s] ",phonelexicon[j]->GetLexicon()->GetLexiconID());//sth
  fprintf(stderr, "\n");//sth
  return(-1);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::Display(void) const
{
  assert(speaker != NULL);
  assert((wordlexicon != NULL) || (phonelexicon != NULL));
  printf("SpokenContentHeaderDescriptor:\n");//sth
  unsigned long i;
  for(i=0;i<nspeaker;i++) {
    printf("SpokenContentSpeaker %ld/%ld\n",i,nspeaker);
    speaker[i]->Display();
  }
  for(i=0;i<nwordlexicon;i++) {
    printf("SpokenContentWordLexicon %ld/%ld\n",i,nwordlexicon);
    wordlexicon[i]->Display();
  }
  for(i=0;i<nphonelexicon;i++) {
    printf("SpokenContentPhoneLexicon %ld/%ld\n",i,nphonelexicon);
    phonelexicon[i]->Display();
  }
  for(i=0;i<nconfusions;i++) {
    printf("SpokenContentConfusionInfo %ld/%ld\n",i,nconfusions);
    confusions[i]->Display();
  }
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::Reset(void)
{
  unsigned long i;
  if(wordlexicon != NULL) {
    for(i=0;i<nwordlexicon;i++) {
      if(wordlexicon[i] != NULL) {
        wordlexicon[i]->release();
        wordlexicon[i]=NULL;
      }
    }
    delete [] wordlexicon;
    wordlexicon=NULL;
  }
  nwordlexicon=0;

  if(phonelexicon != NULL) {
    for(i=0;i<nphonelexicon;i++) {
      if(phonelexicon[i] != NULL) {
        phonelexicon[i]->release();
        phonelexicon[i]=NULL;
      }
    }
    delete [] phonelexicon;
    phonelexicon=NULL;
  }
  nphonelexicon=0;

  if(speaker != NULL) {
    for(i=0;i<nspeaker;i++) {
      if(speaker[i] != NULL) {
        speaker[i]->release();
        speaker[i]=NULL;
      }
    }
    delete [] speaker;
    speaker=NULL;
  }
  nspeaker=0;
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetConfusion(const unsigned long idx,SpokenContentConfusionInfoDescriptor* l)
{
  assert(confusions != NULL);
  assert(idx<nconfusions);
  assert(l != NULL);
  if(confusions[idx] != NULL) confusions[idx]->release();
  confusions[idx]=l;
  confusions[idx]->addref();
}

//----------------------------------------------------------------------------
unsigned long SpokenContentHeaderDescriptor::GetNConfusions(void) const
{
  return(nconfusions);
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::SetNConfusions(
  const unsigned long n)
{
  unsigned long i;
  if(confusions != NULL) {
    for(i=0;i<nconfusions;i++) {
      if(confusions[i] != NULL) {
        confusions[i]->release();
        confusions[i]=NULL;
      }
    }
    delete [] confusions;
    confusions=NULL;
  }
  nconfusions=n;
  if(n != 0) {
    confusions=new SpokenContentConfusionInfoDescriptor*[n];
    for(i=0;i<n;i++) confusions[i]=NULL;
  }
}

//----------------------------------------------------------------------------
long SpokenContentHeaderDescriptor::FindConfusion(const char* refid) 
  const
{
  assert(refid != NULL);
  if(refid[0] == '#')
  {
	  for(unsigned long i=0;i<nconfusions;i++) {
		if(strcmp(&refid[1],confusions[i]->GetConfusionID())==0) {
		  return(static_cast<long> (i));
		}
	  }
  }
  return(-1);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptor* 
 SpokenContentHeaderDescriptor::GetConfusion(const unsigned long idx)
{
  assert(confusions != NULL);
  assert(idx<nconfusions);
  assert(confusions[idx] != NULL);
  return(confusions[idx]);
}



//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentHeaderDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentHeaderDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentHeaderDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentHeaderDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentHeaderDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentHeaderDescriptorInterfaceABC *SpokenContentHeaderDescriptor::GetInterface(void)
{ return &m_Interface; }



