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
// Applicable File Name:  SpokenContentNode.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentNodeDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeDescriptorInterface::myName = "SpokenContentNode Type Description Interface";

const UUID SpokenContentNodeDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeDescriptor::myName = "SpokenContentNode Type Descriptor";

const UUID SpokenContentNodeDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentNodeDescriptor::valName = "";

const UUID SpokenContentNodeDescriptorInterfaceABC::myID = UUID();

SpokenContentNodeDescriptorInterfaceABC::SpokenContentNodeDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentNodeDescriptorInterface::SpokenContentNodeDescriptorInterface(
  SpokenContentNodeDescriptor *aHisto): m_SpokenContentNodeDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterface::~SpokenContentNodeDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::addref(void)
{ m_SpokenContentNodeDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::release(void)
{ m_SpokenContentNodeDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned char SpokenContentNodeDescriptorInterface::GetNWordLinks(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetNWordLinks());
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptor* SpokenContentNodeDescriptorInterface::GetWordLink(const unsigned char idx)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetWordLink(idx));
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetNWordLinks(const unsigned char n)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetNWordLinks(n);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetWordLink(const unsigned char idx,SpokenContentWordLinkDescriptor *l)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetWordLink(idx,l);
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentNodeDescriptorInterface::GetNPhoneLinks(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetNPhoneLinks());
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptor* SpokenContentNodeDescriptorInterface::GetPhoneLink(const unsigned char idx)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetPhoneLink(idx));
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetNPhoneLinks(const unsigned char n)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetNPhoneLinks(n);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetPhoneLink(const unsigned char idx,SpokenContentPhoneLinkDescriptor *l)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetPhoneLink(idx,l);
}


//----------------------------------------------------------------------------
const unsigned short SpokenContentNodeDescriptorInterface::GetNum(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetNum());
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetNum(const unsigned short n)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetNum(n);
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentNodeDescriptorInterface::GetTimeOffset(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetTimeOffset());
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetTimeOffset(const unsigned short t)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetTimeOffset(t);
}

//----------------------------------------------------------------------------
const char* SpokenContentNodeDescriptorInterface::GetSpeaker(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetSpeaker());
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::SetSpeaker(const char* s)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->SetSpeaker(s);
}

//----------------------------------------------------------------------------
const float SpokenContentNodeDescriptorInterface::GetWordProb(const char* w) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetWordProb(w));
}

//----------------------------------------------------------------------------
const float SpokenContentNodeDescriptorInterface::GetPhoneProb(const char* p) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->GetPhoneProb(p));
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptor* SpokenContentNodeDescriptorInterface::FindPhoneLink(const char* phone)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  return(m_SpokenContentNodeDescriptor->FindPhoneLink(phone));
}


//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentNodeDescriptor != NULL);
  m_SpokenContentNodeDescriptor->Reset();
}

//=============================================================================
SpokenContentNodeDescriptor::SpokenContentNodeDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nwordlinks(0),
nphonelinks(0),
num(0),
timeoffset(0),
speaker(NULL),
wordlinks(NULL),
phonelinks(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentNodeDescriptor::SpokenContentNodeDescriptor(SpokenContentNodeDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nwordlinks(0),
nphonelinks(0),
num(0),
timeoffset(0),
speaker(NULL),
wordlinks(NULL),
phonelinks(NULL)
{
  SetExposedInterface(&m_Interface);

  SetNWordLinks(data.GetNWordLinks());
  SetNPhoneLinks(data.GetNPhoneLinks());
  SetNum(data.GetNum());
  SetTimeOffset(data.GetTimeOffset());
  SetSpeaker(data.GetSpeaker());
  unsigned char i;
  for(i=0;i<data.GetNWordLinks();i++) {
    SetWordLink(i,data.GetWordLink(i));
  }
  for(i=0;i<data.GetNPhoneLinks();i++) {
    SetPhoneLink(i,data.GetPhoneLink(i));
  }
}


//----------------------------------------------------------------------------
SpokenContentNodeDescriptor::~SpokenContentNodeDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentNode delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentNodeDescriptor::GetNWordLinks(void) const
{
  return(nwordlinks);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptor* SpokenContentNodeDescriptor::GetWordLink(const unsigned char idx)
{
  assert(idx<nwordlinks);
  assert(wordlinks != NULL);
  assert(wordlinks[idx] != NULL);
  return(wordlinks[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetNWordLinks(const unsigned char n)
{
  // First bin the contents
  unsigned char i;
  if(wordlinks != NULL) {
    for(i=0;i<nwordlinks;i++) {
      if(wordlinks[i] != NULL) wordlinks[i]->release();
    }
    delete [] wordlinks;
    wordlinks=NULL;
  }
  nwordlinks=0;
  if(n==0) return;

  // Now make the new storage
  wordlinks=new SpokenContentWordLinkDescriptor*[n];
  nwordlinks=n;
  for(i=0;i<n;i++) wordlinks[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetWordLink(const unsigned char idx,SpokenContentWordLinkDescriptor *l)
{
  assert(idx<nwordlinks);
  assert(wordlinks != NULL);
  if(wordlinks[idx] != NULL) wordlinks[idx]->release();
  wordlinks[idx]=l;
  wordlinks[idx]->addref();
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentNodeDescriptor::GetNPhoneLinks(void) const
{
  return(nphonelinks);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptor* SpokenContentNodeDescriptor::GetPhoneLink(const unsigned char idx)
{
  assert(idx<nphonelinks);
  assert(phonelinks != NULL);
  assert(phonelinks[idx] != NULL);
  return(phonelinks[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetNPhoneLinks(const unsigned char n)
{
  // First bin the contents
  unsigned char i;
  if(phonelinks != NULL) {
    for(i=0;i<nphonelinks;i++) {
      if(phonelinks[i] != NULL) phonelinks[i]->release();
    }
    delete [] phonelinks;
    phonelinks=NULL;
  }
  nphonelinks=0;
  if(n==0) return;

  // Now make the new storage
  phonelinks=new SpokenContentPhoneLinkDescriptor*[n];
  nphonelinks=n;
  for(i=0;i<n;i++) phonelinks[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetPhoneLink(const unsigned char idx,SpokenContentPhoneLinkDescriptor *l)
{
  assert(idx<nphonelinks);
  assert(phonelinks != NULL);
  if(phonelinks[idx] != NULL) phonelinks[idx]->release();
  phonelinks[idx]=l;
  phonelinks[idx]->addref();
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentNodeDescriptor::GetNum(void) const
{
  return(num);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetNum(const unsigned short n)
{
  num=n;
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentNodeDescriptor::GetTimeOffset(void) const
{
  return(timeoffset);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetTimeOffset(const unsigned short t)
{
  timeoffset=t;
}

//----------------------------------------------------------------------------
const char* SpokenContentNodeDescriptor::GetSpeaker(void) const
{
  return(speaker);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::SetSpeaker(const char* s)
{
  delete [] speaker;
  speaker=NULL;
  if(s==NULL) return;
  speaker=new char[strlen(s)+1];
  strcpy(speaker,s);
}

//----------------------------------------------------------------------------
const float SpokenContentNodeDescriptor::GetWordProb(const char* w) const
{
  float ptop=0.0f;
  bool flag=false;
  for(unsigned char i=0;i<nwordlinks;i++) {
    if(strcmp(w,wordlinks[i]->GetWord())==0) {
      flag=true;
      if(wordlinks[i]->GetLink()->GetProbability()>ptop) {
        ptop=wordlinks[i]->GetLink()->GetProbability();
      }
    }
  }
  assert(flag==true);
  return(ptop);
}

//----------------------------------------------------------------------------
const float SpokenContentNodeDescriptor::GetPhoneProb(const char* p) const
{
  float ptop=0.0f;
  bool flag=false;
  for(unsigned char i=0;i<nphonelinks;i++) {
    if(strcmp(p,phonelinks[i]->GetPhone())==0) {
      flag=true;
      if(phonelinks[i]->GetLink()->GetProbability()>ptop) {
        ptop=phonelinks[i]->GetLink()->GetProbability();
      }
    }
  }
  assert(flag==true);
  return(ptop);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptor* SpokenContentNodeDescriptor::FindPhoneLink(const char* p)
{
  assert(p != NULL);
  for(unsigned char i=0;i<nphonelinks;i++) {
    if(strcmp(p,phonelinks[i]->GetPhone())==0) return(phonelinks[i]);
  }
  return(NULL);
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::Display(void) const
{
  printf("SpokenContentNodeDescriptor: num=%d timeOffset=%d",num,timeoffset);
  if(speaker != NULL) printf(" speaker=%s",speaker);
  printf("\n");
  unsigned char i;
  for(i=0;i<nwordlinks;i++) {
    printf("SpokenContentWordLink %d/%d: ",i,nwordlinks);
    wordlinks[i]->Display();
  }
  for(i=0;i<nphonelinks;i++) {
    printf("SpokenContentPhoneLink %d/%d: ",i,nphonelinks);
    phonelinks[i]->Display();
  }
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::Reset(void)
{
  SetNWordLinks(0);
  SetNPhoneLinks(0);
  nwordlinks=0;
  nphonelinks=0;
  num=0;
  timeoffset=0;
  delete [] speaker;
  speaker=NULL;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentNodeDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentNodeDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentNodeDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentNodeDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentNodeDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentNodeDescriptorInterfaceABC *SpokenContentNodeDescriptor::GetInterface(void)
{ return &m_Interface; }



