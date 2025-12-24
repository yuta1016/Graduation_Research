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
// Applicable File Name:  SpokenContentSpeaker.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentSpeakerDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerDescriptorInterface::myName = "SpokenContentSpeaker Type Description Interface";

const UUID SpokenContentSpeakerDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerDescriptor::myName = "SpokenContentSpeaker Type Descriptor";

const UUID SpokenContentSpeakerDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentSpeakerDescriptor::valName = "";

const UUID SpokenContentSpeakerDescriptorInterfaceABC::myID = UUID();

SpokenContentSpeakerDescriptorInterfaceABC::SpokenContentSpeakerDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentSpeakerDescriptorInterface::SpokenContentSpeakerDescriptorInterface(
  SpokenContentSpeakerDescriptor *aHisto): m_SpokenContentSpeakerDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterface::~SpokenContentSpeakerDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::addref(void)
{ m_SpokenContentSpeakerDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::release(void)
{ m_SpokenContentSpeakerDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
#if 0 // PNG
IndividualDescriptor* SpokenContentSpeakerDescriptorInterface::GetIndividual(void)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetIndividual());
}
#endif

//----------------------------------------------------------------------------
const unsigned long SpokenContentSpeakerDescriptorInterface::GetNPhoneIndex(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetNPhoneIndex());
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentSpeakerDescriptorInterface::GetNWordIndex(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetNWordIndex());
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptor* SpokenContentSpeakerDescriptorInterface::GetPhoneIndex(const unsigned long idx)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetPhoneIndex(idx));
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptor* SpokenContentSpeakerDescriptorInterface::GetWordIndex(const unsigned long idx)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetWordIndex(idx));
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetWordLexiconID(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetWordLexiconID());
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetPhoneLexiconID(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetPhoneLexiconID());
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetLang(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetLang());
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetProvenance(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetProvenance());
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetDefaultWordLatticeId(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetDefaultWordLatticeId());
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetDefaultPhoneLatticeId(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetDefaultPhoneLatticeId());
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptor* SpokenContentSpeakerDescriptorInterface::FindWordIndexEntry(const unsigned char n,const char** x)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->FindWordIndexEntry(n,x));
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptor* SpokenContentSpeakerDescriptorInterface::FindPhoneIndexEntry(const unsigned char n,const char** x)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->FindPhoneIndexEntry(n,x));
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetDefaultWordLatticeId(const char* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetDefaultWordLatticeId(i);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetDefaultPhoneLatticeId(const char* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetDefaultPhoneLatticeId(i);
}

//----------------------------------------------------------------------------
#if 0 // PNG
void SpokenContentSpeakerDescriptorInterface::SetIndividual(IndividualDescriptor* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetIndividual(i);
}
#endif

#if 0 // PNG
//----------------------------------------------------------------------------
SpokenContentExtractionInfoDescriptor* SpokenContentSpeakerDescriptorInterface::GetExtractionInfo(void)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetExtractionInfo());
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetExtractionInfo(SpokenContentExtractionInfoDescriptor* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetExtractionInfo(i);
}
#endif

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetNWordIndex(const unsigned long n)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetNWordIndex(n);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetNPhoneIndex(const unsigned long n)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetNPhoneIndex(n);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetWordIndex(const unsigned long n,SpokenContentWordIndexEntryDescriptor* e)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetWordIndex(n,e);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetPhoneIndex(const unsigned long n,SpokenContentPhoneIndexEntryDescriptor* e)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetPhoneIndex(n,e);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetWordLexiconID(const char* s)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetWordLexiconID(s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetPhoneLexiconID(const char* s)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetPhoneLexiconID(s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetLang(const char* s)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetLang(s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetProvenance(const char* s)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetProvenance(s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetPhoneConfID(const char* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetPhoneConfID(i);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetPhoneConfID(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetPhoneConfID());
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::SetSpeakerID(const char* i)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->SetSpeakerID(i);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptorInterface::GetSpeakerID(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  return(m_SpokenContentSpeakerDescriptor->GetSpeakerID());
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentSpeakerDescriptor != NULL);
  m_SpokenContentSpeakerDescriptor->Reset();
}

//=============================================================================
SpokenContentSpeakerDescriptor::SpokenContentSpeakerDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
// PNG individual(NULL),
// PNG extractionInfo(NULL),
nwordindex(0),
word_index(NULL),
nphoneindex(0),
phone_index(NULL),
defaultWordLatticeId(NULL),
defaultPhoneLatticeId(NULL),
phone_lexicon_id(NULL),
word_lexicon_id(NULL),
phone_conf_id(NULL),
lang(NULL),
provenance(NULL),
speakerId(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentSpeakerDescriptor::SpokenContentSpeakerDescriptor(SpokenContentSpeakerDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
// PNG individual(NULL),
// PNG extractionInfo(NULL),
nwordindex(0),
word_index(NULL),
nphoneindex(0),
phone_index(NULL),
defaultWordLatticeId(NULL),
defaultPhoneLatticeId(NULL),
phone_lexicon_id(NULL),
word_lexicon_id(NULL),
phone_conf_id(NULL),
lang(NULL),
provenance(NULL),
speakerId(NULL)
{
  SetExposedInterface(&m_Interface);

  // PNG SetIndividual(data.GetIndividual());
  // PNG SetExtractionInfo(data.GetExtractionInfo());
  SetNWordIndex(data.GetNWordIndex());
  SetNPhoneIndex(data.GetNPhoneIndex());
  unsigned long i;
  for(i=0;i<nwordindex;i++) SetWordIndex(i,data.GetWordIndex(i));
  for(i=0;i<nphoneindex;i++) SetPhoneIndex(i,data.GetPhoneIndex(i));
  SetDefaultWordLatticeId(data.GetDefaultWordLatticeId());
  SetDefaultPhoneLatticeId(data.GetDefaultPhoneLatticeId());
  SetPhoneLexiconID(data.GetPhoneLexiconID());
  SetWordLexiconID(data.GetWordLexiconID());
  SetPhoneConfID(data.GetPhoneConfID());
  SetLang(data.GetLang());
  SetProvenance(data.GetProvenance());
  SetSpeakerID(data.GetSpeakerID());
}


//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptor::~SpokenContentSpeakerDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentSpeaker delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::Reset(void)
{
#if 0 // PNG
  if(individual != NULL) {
    individual->release();
    individual=NULL;
  }

  if(extractionInfo != NULL) {
    extractionInfo->release();
    extractionInfo=NULL;
  }
#endif

  if(word_index != NULL) {
    for(unsigned long i=0;i<nwordindex;i++) {
      if(word_index[i] != NULL) {
        word_index[i]->release();
        word_index[i]=NULL;
      }
    }
    delete [] word_index;
    word_index=NULL;
  }

  nphoneindex=0;
  if(phone_index != NULL) {
    for(unsigned long i=0;i<nphoneindex;i++) {
      if(phone_index[i] != NULL) {
        phone_index[i]->release();
        phone_index[i]=NULL;
      }
    }
    delete [] phone_index;
    phone_index=NULL;
  }
  nphoneindex=0;

  if(phone_lexicon_id != NULL) {
    delete [] phone_lexicon_id;
    phone_lexicon_id=NULL;
  }
  if(word_lexicon_id != NULL) {
    delete [] word_lexicon_id;
    word_lexicon_id=NULL;
  }
  if(phone_conf_id != NULL) {
    delete [] phone_conf_id;
    phone_conf_id=NULL;
  }
  if(speakerId != NULL) {
    delete [] speakerId;
    speakerId=NULL;
  }
  if(lang != NULL) {
    delete [] lang;
    lang=NULL;
  }
  if(provenance != NULL) {
    delete [] provenance;
    provenance=NULL;
  }
}

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentSpeakerDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentSpeakerDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentSpeakerDescriptor::IsProprietary(void)
{ return m_isProprietary; }

#if 0 // PNG
//----------------------------------------------------------------------------
IndividualDescriptor* SpokenContentSpeakerDescriptor::GetIndividual(void)
{
  return(individual);
}

//----------------------------------------------------------------------------
SpokenContentExtractionInfoDescriptor* SpokenContentSpeakerDescriptor::GetExtractionInfo(void)
{
  return(extractionInfo);
}
#endif

//----------------------------------------------------------------------------
const unsigned long SpokenContentSpeakerDescriptor::GetNPhoneIndex(void) const
{
  return(nphoneindex);
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentSpeakerDescriptor::GetNWordIndex(void) const
{
  return(nwordindex);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptor* SpokenContentSpeakerDescriptor::GetPhoneIndex(const unsigned long idx)
{
  assert(idx<nphoneindex);
  assert(phone_index != NULL);
  assert(phone_index[idx] != NULL);
  return(phone_index[idx]);
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptor* SpokenContentSpeakerDescriptor::GetWordIndex(const unsigned long idx)
{
  assert(idx<nwordindex);
  assert(word_index != NULL);
  assert(word_index[idx] != NULL);
  return(word_index[idx]);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetWordLexiconID(void) const
{
  return(word_lexicon_id); // This can be null if no id
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetPhoneConfID(void) const
{
  return(phone_conf_id); // This can be null if no id
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetSpeakerID(void) const
{
  assert(speakerId != NULL);
  return(speakerId);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetPhoneLexiconID(void) const
{
  return(phone_lexicon_id); // This can be null if no id
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetLang(void) const
{
  assert(lang != NULL);
  return(lang);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetProvenance(void) const
{
  assert(provenance != NULL);
  return(provenance);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetDefaultWordLatticeId(void) const
{
  return(defaultWordLatticeId);
}

//----------------------------------------------------------------------------
const char* SpokenContentSpeakerDescriptor::GetDefaultPhoneLatticeId(void) const
{
  return(defaultPhoneLatticeId);
}

//----------------------------------------------------------------------------
// Find an index entry matching this key. If can't find one then return
// NULL.
SpokenContentWordIndexEntryDescriptor* SpokenContentSpeakerDescriptor::FindWordIndexEntry(const unsigned char n,const char** x)
{
  assert(x != NULL);
  assert(n != 0);
  for(unsigned long i=0;i<nwordindex;i++) {
    SpokenContentWordIndexEntryDescriptor* e=word_index[i];
    assert(e->GetNKey() == n);
    bool flag=true;
    for(unsigned char in=0;in<n;in++) {
      if(strcmp(x[in],e->GetKey(in))!=0) {
        flag=false;
        break;
      }
    }
    if(flag) return(e);
  }
  return(NULL);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptor* SpokenContentSpeakerDescriptor::FindPhoneIndexEntry(const unsigned char n,const char** x)
{
  assert(x != NULL);
  assert(n != 0);
  for(unsigned long i=0;i<nphoneindex;i++) {
    SpokenContentPhoneIndexEntryDescriptor* e=phone_index[i];
    if(e->GetNKey()<n) continue; // key is too short
    bool flag=true;
    for(unsigned char in=0;in<n;in++) {
      if(strcmp(x[in],e->GetKey(in))!=0) {
        flag=false;
        break;
      }
    }
    if(flag) return(e);
  }
  return(NULL);
}

#if 0 // PNG
//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetIndividual(IndividualDescriptor* i)
{
  if(individual != NULL) {
    individual ->release();
  }
  individual=i;
  if(individual != NULL) individual->addref();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetExtractionInfo(
  SpokenContentExtractionInfoDescriptor *i)
{
  if(extractionInfo != NULL) {
    extractionInfo->release();
  }
  extractionInfo=i;
  if(extractionInfo != NULL) extractionInfo->addref();
}
#endif

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetNWordIndex(const unsigned long n)
{
  assert(n != 0);

  // Wipe out the old
  unsigned long i;
  if(word_index != NULL) {
    for(i=0;i<n;i++) {
      if(word_index[i] != NULL) word_index[i]->release();
    }
  }
  delete [] word_index;
  word_index=NULL;

  // Set up the new
  word_index=new SpokenContentWordIndexEntryDescriptor*[n];
  nwordindex=n;
  for(i=0;i<n;i++) word_index[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetNPhoneIndex(const unsigned long n)
{
  assert(n != 0);

  // Wipe out the old
  unsigned long i;
  if(phone_index != NULL) {
    for(i=0;i<n;i++) {
      if(phone_index[i] != NULL) phone_index[i]->release();
    }
  }
  delete [] phone_index;
  phone_index=NULL;

  // Set up the new
  phone_index=new SpokenContentPhoneIndexEntryDescriptor*[n];
  nphoneindex=n;
  for(i=0;i<n;i++) phone_index[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetWordIndex(const unsigned long n,SpokenContentWordIndexEntryDescriptor* e)
{
  assert(n<nwordindex);
  assert(e != NULL);
  assert(word_index != NULL);
  if(word_index[n] != NULL) word_index[n]->release();
  word_index[n]=e;
  e->addref();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetPhoneIndex(const unsigned long n,SpokenContentPhoneIndexEntryDescriptor* e)
{
  assert(n<nphoneindex);
  assert(e != NULL);
  assert(phone_index != NULL);
  if(phone_index[n] != NULL) phone_index[n]->release();
  phone_index[n]=e;
  e->addref();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetWordLexiconID(const char* s)
{
  if(word_lexicon_id != NULL) {
    delete [] word_lexicon_id;
    word_lexicon_id=NULL;
  }
  if(s != NULL) {
    word_lexicon_id=new char[strlen(s)+1];
    strcpy(word_lexicon_id,s);
  }
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetPhoneConfID(const char* s)
{
  if(phone_conf_id != NULL) {
    delete [] phone_conf_id;
    phone_conf_id=NULL;
  }
  if(s != NULL) {
    phone_conf_id=new char[strlen(s)+1];
    strcpy(phone_conf_id,s);
  }
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetPhoneLexiconID(const char* s)
{
  if(phone_lexicon_id != NULL) {
    delete [] phone_lexicon_id;
    phone_lexicon_id=NULL;
  }
  if(s != NULL) {
    phone_lexicon_id=new char[strlen(s)+1];
    strcpy(phone_lexicon_id,s);
  }
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetSpeakerID(const char* s)
{
  if(speakerId != NULL) {
    delete [] speakerId;
    speakerId=NULL;
  }
  if(s != NULL) {
    speakerId=new char[strlen(s)+1];
    strcpy(speakerId,s);
  }
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetLang(const char* s)
{
  assert(s != NULL);
  if(lang != NULL) delete [] lang;
  lang=new char[strlen(s)+1];
  strcpy(lang,s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetProvenance(const char* s)
{
  assert(s != NULL);
  if(provenance != NULL) delete [] provenance;
  provenance=new char[strlen(s)+1];
  strcpy(provenance,s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetDefaultWordLatticeId(const char* s)
{
  delete [] defaultWordLatticeId;
  defaultWordLatticeId=NULL;
  if(s==NULL) return;
  defaultWordLatticeId=new char[strlen(s)+1];
  strcpy(defaultWordLatticeId,s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::SetDefaultPhoneLatticeId(const char* s)
{
  delete [] defaultPhoneLatticeId;
  defaultPhoneLatticeId=NULL;
  if(s==NULL) return;
  defaultPhoneLatticeId=new char[strlen(s)+1];
  strcpy(defaultPhoneLatticeId,s);
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::Display(void) const 
{
  printf("SpokenContentSpeaker: xml:lang=[%s] provenance=[%s] id=[%s]",
    GetLang(),GetProvenance(),GetSpeakerID());
  if(GetPhoneLexiconID()!=NULL) {
    printf(" phoneLexiconRef=[%s]",GetPhoneLexiconID());
  }
  if(GetWordLexiconID()!=NULL) {
    printf(" wordLexiconRef=[%s]",GetWordLexiconID());
  }
  if(GetPhoneConfID()!=NULL) {
    printf(" confusionInfoRef=[%s]",GetPhoneConfID());
  }
  printf("\n");
  if(defaultWordLatticeId != NULL) {
    printf("defaultWordLatticeId=%s\n",defaultWordLatticeId);
  }
  if(defaultPhoneLatticeId != NULL) {
    printf("defaultPhoneLatticeId=%s\n",defaultPhoneLatticeId);
  }
#if 0 // PNG
  if(individual != NULL)     individual->Display();
  else                       printf("//No individual\n");
  if(extractionInfo != NULL) extractionInfo->Display();
  else                       printf("//No extraction\n");
#endif
  unsigned long i;
  printf("There are %ld word index elements and %ld phone index element\n",
	 GetNWordIndex(),GetNPhoneIndex());//sth
  for(i=0;i<GetNWordIndex();i++) word_index[i]->Display();
  for(i=0;i<GetNPhoneIndex();i++) phone_index[i]->Display();
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentSpeakerDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentSpeakerDescriptorInterfaceABC *SpokenContentSpeakerDescriptor::GetInterface(void)
{ return &m_Interface; }



