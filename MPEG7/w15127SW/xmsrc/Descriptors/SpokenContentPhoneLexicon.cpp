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
// Applicable File Name:  SpokenContentPhoneLexicon.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentPhoneLexiconDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconDescriptorInterface::myName = "SpokenContentPhoneLexicon Type Description Interface";

const UUID SpokenContentPhoneLexiconDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconDescriptor::myName = "SpokenContentPhoneLexicon Type Descriptor";

const UUID SpokenContentPhoneLexiconDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLexiconDescriptor::valName = "";

const UUID SpokenContentPhoneLexiconDescriptorInterfaceABC::myID = UUID();

SpokenContentPhoneLexiconDescriptorInterfaceABC::SpokenContentPhoneLexiconDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentPhoneLexiconDescriptorInterface::SpokenContentPhoneLexiconDescriptorInterface(
  SpokenContentPhoneLexiconDescriptor *aHisto): m_SpokenContentPhoneLexiconDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterface::~SpokenContentPhoneLexiconDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::addref(void)
{ m_SpokenContentPhoneLexiconDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::release(void)
{ m_SpokenContentPhoneLexiconDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptor* SpokenContentPhoneLexiconDescriptorInterface::GetLexicon(void)
{return m_SpokenContentPhoneLexiconDescriptor->GetLexicon();}

//----------------------------------------------------------------------------
const unsigned long SpokenContentPhoneLexiconDescriptorInterface::GetNPhones(void) const
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  return(m_SpokenContentPhoneLexiconDescriptor->GetNPhones());
}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconDescriptorInterface::GetPhone(const unsigned long idx) const
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  return(m_SpokenContentPhoneLexiconDescriptor->GetPhone(idx));
}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconDescriptorInterface::GetPhonesetName(void) const
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  return(m_SpokenContentPhoneLexiconDescriptor->GetPhonesetName());
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::SetLexicon(SpokenContentLexiconDescriptor* l)
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->SetLexicon(l);
}
 
//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::SetNPhones(const unsigned long n)
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->SetNPhones(n);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::SetPhone(const unsigned long idx,const char* w)
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->SetPhone(idx,w);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::SetPhonesetName(const char* p)
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->SetPhonesetName(p);
}

//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconDescriptorInterface::Exists(const char* x) const
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  return(m_SpokenContentPhoneLexiconDescriptor->Exists(x));
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentPhoneLexiconDescriptor != NULL);
  m_SpokenContentPhoneLexiconDescriptor->Reset();
}

//=============================================================================
SpokenContentPhoneLexiconDescriptor::SpokenContentPhoneLexiconDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
lexicon(NULL),
nphones(0),
phones(NULL),
phoneset_name(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentPhoneLexiconDescriptor::SpokenContentPhoneLexiconDescriptor(SpokenContentPhoneLexiconDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
lexicon(NULL),
nphones(0),
phones(NULL),
phoneset_name(NULL)
{
  SetExposedInterface(&m_Interface);
  SetLexicon(data.GetLexicon());
  SetPhonesetName(data.GetPhonesetName());
  SetNPhones(data.GetNPhones());
  for(unsigned long i=0;i<data.GetNPhones();i++) {
    SetPhone(i,data.GetPhone(i));
  }
}


//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptor::~SpokenContentPhoneLexiconDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentPhoneLexicon delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::Reset(void)
{
  if(lexicon != NULL) {
    lexicon->release();
    lexicon=NULL;
  }
  if(phones != NULL) {
    for(unsigned long i=0;i<nphones;i++) {
      assert(phones[i] != NULL);
      delete [] phones[i];
    }
    delete [] phones;
    phones=NULL;
  }
  delete [] phoneset_name;
  phoneset_name=NULL;
  nphones=0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLexiconDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLexiconDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptor* SpokenContentPhoneLexiconDescriptor::GetLexicon(void)
{
  assert(lexicon != NULL);
  return(lexicon);
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentPhoneLexiconDescriptor::GetNPhones(void) const
{return(nphones);}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconDescriptor::GetPhone(unsigned long i) const
{
  assert(i<nphones);
  return(phones[i]);
}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLexiconDescriptor::GetPhonesetName(void) const
{
  assert(phoneset_name != NULL);
  return(phoneset_name);
}


//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::SetLexicon(SpokenContentLexiconDescriptor* l)
{
  if(lexicon != NULL) lexicon->release();
  lexicon=l;
  if(lexicon != NULL) lexicon->addref();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::SetNPhones(const unsigned long n)
{
  unsigned long i;
  if(phones != NULL) {
    for(i=0;i<nphones;i++) delete [] phones[i];
    delete [] phones;
    phones=NULL;
  }
  nphones=n;
  if(nphones==0) return;
  phones=new char*[nphones];
  for(i=0;i<nphones;i++) phones[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::SetPhone(const unsigned long idx,
  const char* p)
{
  assert(p != NULL);
  assert(idx<nphones);
  assert(phones != NULL);
  if(phones[idx] != NULL) {
    delete [] phones[idx];
    phones[idx]=NULL;
  }

  char *t1=new char[strlen(p)+1];
  char *t2=new char[strlen(p)+1];
  if(sscanf(p,"%s%s",t1,t2) != 1) {
    fprintf(stderr,"SpokenContentPhoneLexiconDescriptor::AddPhone: "
            "incorrect number of fields\n");
    return;
  }

  phones[idx]=new char[strlen(t1)+1];
  strcpy(phones[idx],t1);
  delete [] t2;
  delete [] t1;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::SetPhonesetName(const char* p)
{
  assert(p != NULL);
  delete [] phoneset_name;
  phoneset_name=new char[strlen(p)+1];
  strcpy(phoneset_name,p);
}

//----------------------------------------------------------------------------
bool SpokenContentPhoneLexiconDescriptor::Exists(const char* x) const
{
  assert(x != NULL);
  for(unsigned long i=0;i<nphones;i++) {
    if(strcmp(x,phones[i])==0) return(true);
  }
  return(false);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::Display(void) const 
{
  assert(lexicon        != NULL);
  assert(phones         != NULL);
  assert(phoneset_name  != NULL);
  printf("SpokenContentPhoneLexicon: name=[%s]\n",phoneset_name);
  lexicon->Display();
  printf("Phone:\n");
  for(unsigned long i=0;i<nphones;i++) {
    printf("[%s] ",phones[i]);
    if((i % 5) == 4) printf("\n");
  }
  if(((nphones-1) % 5) != 4) printf("\n");
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLexiconDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentPhoneLexiconDescriptorInterfaceABC *SpokenContentPhoneLexiconDescriptor::GetInterface(void)
{ return &m_Interface; }



