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
// Applicable File Name:  SpokenContentWordLink.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentWordLinkDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkDescriptorInterface::myName = "SpokenContentWordLink Type Description Interface";

const UUID SpokenContentWordLinkDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkDescriptor::myName = "SpokenContentWordLink Type Descriptor";

const UUID SpokenContentWordLinkDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordLinkDescriptor::valName = "";

const UUID SpokenContentWordLinkDescriptorInterfaceABC::myID = UUID();

SpokenContentWordLinkDescriptorInterfaceABC::SpokenContentWordLinkDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentWordLinkDescriptorInterface::SpokenContentWordLinkDescriptorInterface(
  SpokenContentWordLinkDescriptor *aHisto): m_SpokenContentWordLinkDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterface::~SpokenContentWordLinkDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::addref(void)
{ m_SpokenContentWordLinkDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::release(void)
{ m_SpokenContentWordLinkDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLinkDescriptorInterface::GetWord(void) const
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  return(m_SpokenContentWordLinkDescriptor->GetWord());
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptor* SpokenContentWordLinkDescriptorInterface::GetLink(void)
{return m_SpokenContentWordLinkDescriptor->GetLink();}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::SetWord(const char* w)
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  m_SpokenContentWordLinkDescriptor->SetWord(w);
}

//----------------------------------------------------------------------------
long SpokenContentWordLinkDescriptorInterface::GetWordIndex() const
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  return m_SpokenContentWordLinkDescriptor->GetWordIndex();
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::SetWordIndex(long idx)
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  m_SpokenContentWordLinkDescriptor->SetWordIndex(idx);
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::SetLink(SpokenContentLinkDescriptor* l)
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  m_SpokenContentWordLinkDescriptor->SetLink(l);
}
 
//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  m_SpokenContentWordLinkDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentWordLinkDescriptor != NULL);
  m_SpokenContentWordLinkDescriptor->Reset();
}

//=============================================================================
SpokenContentWordLinkDescriptor::SpokenContentWordLinkDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
link(NULL),
word(NULL), 
wordIndex(-1)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentWordLinkDescriptor::SpokenContentWordLinkDescriptor(SpokenContentWordLinkDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
link(NULL),
word(NULL),
wordIndex(-1)
{
  SetExposedInterface(&m_Interface);
  Reset();
  SetLink(data.GetLink());
  SetWord(data.GetWord());
}


//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptor::~SpokenContentWordLinkDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentWordLink delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::Reset(void)
{
  if(link != NULL) link->release();
  link=NULL;
  if(word != NULL) delete [] word;
  word = 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordLinkDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordLinkDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLinkDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordLinkDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const char* SpokenContentWordLinkDescriptor::GetWord(void) const
{
  assert(word != NULL);
  return(word);
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptor* SpokenContentWordLinkDescriptor::GetLink(void)
{
  assert(link != NULL);
  return(link);
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::SetLink(SpokenContentLinkDescriptor* l)
{
  if(link != NULL) link->release();
  link=l;
  if(link != NULL) link->addref();
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::SetWord(const char* w)
{
  assert(w != NULL);

  // w should be one word. Whitespace is unimportant. Convert to lowercase
  char *t1=new char[strlen(w)+1];
  char *t2=new char[strlen(w)+1];
  if(sscanf(w,"%s%s",t1,t2) != 1) {
    fprintf(stderr,"SpokenContentPhoneLexiconDescriptor::SetWord: "
            "incorrect number of fields\n");
    return;
  }
  char *c=t1;
  while(*c != '\0') {
    *c=tolower(*c);
    c++;
  }

  if(word) delete[] word;
  word=new char[strlen(t1)+1];
  strcpy(word,t1);
  delete[] t2;
  delete[] t1;
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::SetWordIndex(long idx)
{
	wordIndex = idx;
}

//----------------------------------------------------------------------------
long SpokenContentWordLinkDescriptor::GetWordIndex() const
{
	return wordIndex;
}


//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::Display(void) const 
{
  printf("SpokenContentWordsLink: word=[%s], ",GetWord());
  link->Display();
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentWordLinkDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentWordLinkDescriptorInterfaceABC *SpokenContentWordLinkDescriptor::GetInterface(void)
{ return &m_Interface; }



