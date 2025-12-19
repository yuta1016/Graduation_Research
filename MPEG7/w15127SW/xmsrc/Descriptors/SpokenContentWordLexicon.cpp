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
// Applicable File Name:  SpokenContentWordLexicon.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentWordLexiconDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconDescriptorInterface::myName = "SpokenContentWordLexicon Type Description Interface";

const UUID SpokenContentWordLexiconDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconDescriptor::myName = "SpokenContentWordLexicon Type Descriptor";

const UUID SpokenContentWordLexiconDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLexiconDescriptor::valName = "";

const UUID SpokenContentWordLexiconDescriptorInterfaceABC::myID = UUID();

SpokenContentWordLexiconDescriptorInterfaceABC::SpokenContentWordLexiconDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentWordLexiconDescriptorInterface::SpokenContentWordLexiconDescriptorInterface(
  SpokenContentWordLexiconDescriptor *aHisto): m_SpokenContentWordLexiconDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterface::~SpokenContentWordLexiconDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::addref(void)
{ m_SpokenContentWordLexiconDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::release(void)
{ m_SpokenContentWordLexiconDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentWordLexiconDescriptorInterface::GetNWords(void) const
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  return(m_SpokenContentWordLexiconDescriptor->GetNWords());
}

//----------------------------------------------------------------------------
const char* SpokenContentWordLexiconDescriptorInterface::GetWord(const unsigned long idx) const
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  return(m_SpokenContentWordLexiconDescriptor->GetWord(idx));
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptor* SpokenContentWordLexiconDescriptorInterface::GetLexicon(void)
{return m_SpokenContentWordLexiconDescriptor->GetLexicon();}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::SetWord(const unsigned long idx,const char* w)
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  m_SpokenContentWordLexiconDescriptor->SetWord(idx,w);
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::SetNWords(const unsigned long n)
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  m_SpokenContentWordLexiconDescriptor->SetNWords(n);
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::SetLexicon(SpokenContentLexiconDescriptor* l)
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  m_SpokenContentWordLexiconDescriptor->SetLexicon(l);
}
 
//----------------------------------------------------------------------------
bool SpokenContentWordLexiconDescriptorInterface::Exists(const char* x) const
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  return(m_SpokenContentWordLexiconDescriptor->Exists(x));
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  m_SpokenContentWordLexiconDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentWordLexiconDescriptor != NULL);
  m_SpokenContentWordLexiconDescriptor->Reset();
}

//=============================================================================
SpokenContentWordLexiconDescriptor::SpokenContentWordLexiconDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
lexicon(NULL),
nwords(0),
words(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentWordLexiconDescriptor::SpokenContentWordLexiconDescriptor(SpokenContentWordLexiconDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
lexicon(NULL),
nwords(0),
words(NULL)
{
  SetExposedInterface(&m_Interface);
  Reset();
  SetLexicon(data.GetLexicon());
  SetNWords(data.GetNWords());
  for(unsigned long i=0;i<data.GetNWords();i++) {
    SetWord(i,data.GetWord(i));
  }
}


//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptor::~SpokenContentWordLexiconDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentWordLexicon delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::Reset(void)
{
  if(lexicon != NULL) lexicon->release();
  lexicon=NULL;
  if(words != NULL) {
    for(unsigned long i=0;i<nwords;i++) {
      assert(words[i] != NULL);
      delete [] words[i];
    }
    delete [] words;
  }
  words=NULL;
  nwords=0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLexiconDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLexiconDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLexiconDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordLexiconDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentWordLexiconDescriptor::GetNWords(void) const
{return(nwords);}

//----------------------------------------------------------------------------
const char* SpokenContentWordLexiconDescriptor::GetWord(unsigned long i) const
{
  assert(i<nwords);
  return(words[i]);
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptor* SpokenContentWordLexiconDescriptor::GetLexicon(void)
{
  assert(lexicon != NULL);
  return(lexicon);
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::SetLexicon(SpokenContentLexiconDescriptor* l)
{
  if(lexicon != NULL) lexicon->release();
  lexicon=l;
  if(lexicon != NULL) lexicon->addref();
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::SetNWords(const unsigned long n)
{
  unsigned long i;
  if(words != NULL) {
    for(i=0;i<nwords;i++) delete [] words[i];
    words=NULL;
  }
  nwords=n;
  if(n==0) return;
  words=new char*[nwords];
  for(i=0;i<nwords;i++) words[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::SetWord(const unsigned long idx,
  const char* w)
{
  assert(w != NULL);
  assert(idx<nwords);
  assert(words != NULL);

  // Get rid of the old word if it exists
  if(words[idx] != NULL) delete [] words[idx];
  words[idx]=NULL;

  // Bin the whitespace around w and convert to lower case
  char *t1=new char[strlen(w)+1];
  char *t2=new char[strlen(w)+1];
  if(sscanf(w,"%s%s",t1,t2) != 1) {
    fprintf(stderr,"SpokenContentPhoneLexiconDescriptor::AddWord: "
            "incorrect number of fields\n");
    return;
  }
  char *c=t1;
  while(*c != '\0') {
    *c=tolower(*c);
    c++;
  }
  
  words[idx]=new char[strlen(t1)+1];
  strcpy(words[idx],t1);

  delete [] t2;
  delete [] t1;
}

//----------------------------------------------------------------------------
// See if the word is in the list - not cpu efficient!
bool SpokenContentWordLexiconDescriptor::Exists(const char* x) const
{
  assert(x != NULL);
  for(unsigned long i=0;i<nwords;i++) {
    if(strcmp(x,words[i])==0) return(true);
  }
  return(false);
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::Display(void) const 
{
  assert(lexicon != NULL);
  printf("SpokenContentWordsLexicon:\n");
  lexicon->Display();
  printf("Word:\n");
  for(unsigned long i=0;i<nwords;i++) {
    printf("[%s] ",words[i]);
    if((i % 5) == 4) printf("\n");
  }
  if(((nwords-1) % 5) != 4) printf("\n");
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentWordLexiconDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentWordLexiconDescriptorInterfaceABC *SpokenContentWordLexiconDescriptor::GetInterface(void)
{ return &m_Interface; }



