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
// Applicable File Name:  SpokenContentLexicon.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentLexiconDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconDescriptorInterface::myName = "SpokenContentLexicon Type Description Interface";

const UUID SpokenContentLexiconDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconDescriptor::myName = "SpokenContentLexicon Type Descriptor";

const UUID SpokenContentLexiconDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLexiconDescriptor::valName = "";

const UUID SpokenContentLexiconDescriptorInterfaceABC::myID = UUID();

SpokenContentLexiconDescriptorInterfaceABC::SpokenContentLexiconDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentLexiconDescriptorInterface::SpokenContentLexiconDescriptorInterface(
  SpokenContentLexiconDescriptor *aHisto): m_SpokenContentLexiconDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterface::~SpokenContentLexiconDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::addref(void)
{ m_SpokenContentLexiconDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::release(void)
{ m_SpokenContentLexiconDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconDescriptorInterface::GetName(void)
{ return myName; }

#if 0 // PNG
//----------------------------------------------------------------------------
SpokenContentExtractionInfoDescriptor* 
  SpokenContentLexiconDescriptorInterface::GetLexiconDecoder(void)
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  return(m_SpokenContentLexiconDescriptor->GetLexiconDecoder());
}
#endif

//----------------------------------------------------------------------------
const char* 
  SpokenContentLexiconDescriptorInterface::GetLexiconID(void) const
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  return(m_SpokenContentLexiconDescriptor->GetLexiconID());
}

//----------------------------------------------------------------------------
const unsigned long 
  SpokenContentLexiconDescriptorInterface::GetLexiconNumOriginalEntries(void) const
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  return(m_SpokenContentLexiconDescriptor->GetLexiconNumOriginalEntries());
}

#if 0 // PNG
//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::SetLexiconDecoder(
  SpokenContentExtractionInfoDescriptor* d)
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  m_SpokenContentLexiconDescriptor->SetLexiconDecoder(d);
}
#endif

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::SetLexiconID(const char* i)
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  m_SpokenContentLexiconDescriptor->SetLexiconID(i);
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::SetLexiconNumOriginalEntries(const unsigned long n)
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  m_SpokenContentLexiconDescriptor->SetLexiconNumOriginalEntries(n);
}


//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  m_SpokenContentLexiconDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentLexiconDescriptor != NULL);
  m_SpokenContentLexiconDescriptor->Reset();
}

//=============================================================================
SpokenContentLexiconDescriptor::SpokenContentLexiconDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
// PNG lexicon_decoder(NULL),
lexicon_id(NULL),
lexicon_num_original_entries(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentLexiconDescriptor::SpokenContentLexiconDescriptor(SpokenContentLexiconDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
// PNG lexicon_decoder(NULL),
lexicon_id(NULL),
lexicon_num_original_entries(0)
{
  SetExposedInterface(&m_Interface);
  // PNG  SetLexiconDecoder(data.GetLexiconDecoder());
  SetLexiconID(data.GetLexiconID());
  SetLexiconNumOriginalEntries(data.GetLexiconNumOriginalEntries());
}


//----------------------------------------------------------------------------
SpokenContentLexiconDescriptor::~SpokenContentLexiconDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentLexicon delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::Reset(void)
{
  // PNG  SetLexiconDecoder(NULL);
  SetLexiconID(NULL);
  SetLexiconNumOriginalEntries(0);
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLexiconDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLexiconDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLexiconDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLexiconDescriptor::IsProprietary(void)
{ return m_isProprietary; }

#if 0 // PNG
//----------------------------------------------------------------------------
SpokenContentExtractionInfoDescriptor* SpokenContentLexiconDescriptor::GetLexiconDecoder(void)
{
  return(lexicon_decoder);
}
#endif

//----------------------------------------------------------------------------
const char* SpokenContentLexiconDescriptor::GetLexiconID(void) const
{
  return(lexicon_id);
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentLexiconDescriptor::GetLexiconNumOriginalEntries(void) const
{
  return(lexicon_num_original_entries);
}

#if 0 // PNG
//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::SetLexiconDecoder(SpokenContentExtractionInfoDescriptor* d)
{
  if(lexicon_decoder != NULL) lexicon_decoder->release();
  lexicon_decoder=d;
}
#endif

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::SetLexiconID(const char* i)
{
  delete [] lexicon_id;
  lexicon_id=NULL;
  if(i==NULL) return;
  lexicon_id=new char[strlen(i)+1];
  strcpy(lexicon_id,i);
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::SetLexiconNumOriginalEntries(const unsigned long n)
{
  lexicon_num_original_entries=n;
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::Display(void) const 
{
  assert(lexicon_id != NULL);
  printf("SpokenContentLexicon: id=[%s]",lexicon_id);
  if(lexicon_num_original_entries != 0) {
    printf(", numOriginalEntries=%d",lexicon_num_original_entries);
  }
  printf("\n");
  // PNG  if(lexicon_decoder != NULL) lexicon_decoder->Display();
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentLexiconDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentLexiconDescriptorInterfaceABC *SpokenContentLexiconDescriptor::GetInterface(void)
{ return &m_Interface; }



