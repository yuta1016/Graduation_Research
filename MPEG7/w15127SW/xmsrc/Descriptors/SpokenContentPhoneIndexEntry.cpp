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
// Applicable File Name:  SpokenContentPhoneIndexEntry.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentPhoneIndexEntryDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryDescriptorInterface::myName = "SpokenContentPhoneIndexEntry Type Description Interface";

const UUID SpokenContentPhoneIndexEntryDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryDescriptor::myName = "SpokenContentPhoneIndexEntry Type Descriptor";

const UUID SpokenContentPhoneIndexEntryDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneIndexEntryDescriptor::valName = "";

const UUID SpokenContentPhoneIndexEntryDescriptorInterfaceABC::myID = UUID();

SpokenContentPhoneIndexEntryDescriptorInterfaceABC::SpokenContentPhoneIndexEntryDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentPhoneIndexEntryDescriptorInterface::SpokenContentPhoneIndexEntryDescriptorInterface(
  SpokenContentPhoneIndexEntryDescriptor *aHisto): m_SpokenContentPhoneIndexEntryDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterface::~SpokenContentPhoneIndexEntryDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::addref(void)
{ m_SpokenContentPhoneIndexEntryDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::release(void)
{ m_SpokenContentPhoneIndexEntryDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentPhoneIndexEntryDescriptorInterface::GetNEntries(void) const
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  return(m_SpokenContentPhoneIndexEntryDescriptor->GetNEntries());
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptor* SpokenContentPhoneIndexEntryDescriptorInterface::GetEntry(const unsigned long idx)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  return(m_SpokenContentPhoneIndexEntryDescriptor->GetEntry(idx));
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentPhoneIndexEntryDescriptorInterface::GetNKey(void) const
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  return(m_SpokenContentPhoneIndexEntryDescriptor->GetNKey());
}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneIndexEntryDescriptorInterface::GetKey(const unsigned char idx) const
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  return(m_SpokenContentPhoneIndexEntryDescriptor->GetKey(idx));
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::SetEntry(const unsigned long idx,SpokenContentIndexEntryDescriptor* e)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->SetEntry(idx,e);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::SetKey(const unsigned char idx,const char* k)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->SetKey(idx,k);
}

//----------------------------------------------------------------------------
long SpokenContentPhoneIndexEntryDescriptorInterface::GetKeyIndex(unsigned char idx) const
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  return m_SpokenContentPhoneIndexEntryDescriptor->GetKeyIndex(idx);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::SetKeyIndex(unsigned char idx, long phoneIndex)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->SetKeyIndex(idx,phoneIndex);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::SetNKey(const unsigned char n)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->SetNKey(n);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::SetNEntries(const unsigned long n)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->SetNEntries(n);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentPhoneIndexEntryDescriptor != NULL);
  m_SpokenContentPhoneIndexEntryDescriptor->Reset();
}

//=============================================================================
SpokenContentPhoneIndexEntryDescriptor::SpokenContentPhoneIndexEntryDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nentries(0),
entries(NULL),
nkey_elements(0),
key_elements(NULL), 
key_indices(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentPhoneIndexEntryDescriptor::SpokenContentPhoneIndexEntryDescriptor(SpokenContentPhoneIndexEntryDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nentries(0),
entries(NULL),
nkey_elements(0),
key_elements(NULL), 
key_indices(0)
{
  SetExposedInterface(&m_Interface);
  SetNEntries(data.GetNEntries());
  SetNKey(data.GetNKey());
  for(unsigned long i=0;i<nentries;i++) SetEntry(i,data.GetEntry(i));
  for(unsigned char j=0;j<nkey_elements;j++) SetKey(j,data.GetKey(j));
}


//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptor::~SpokenContentPhoneIndexEntryDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentPhoneIndexEntry delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::Reset(void)
{
  if(entries != NULL) {
    for(unsigned long i=0;i<nentries;i++) entries[i]->release();
    delete [] entries;
    entries=NULL;
  }
  if(key_elements != NULL) {
    for(unsigned char j=0;j<nkey_elements;j++) delete [] key_elements[j];
    delete [] key_elements;
    key_elements=NULL;
  }
  if(key_indices) {
	  delete[] key_indices;
	  key_indices = 0;
  }
  nentries=0;
  nkey_elements=0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneIndexEntryDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneIndexEntryDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneIndexEntryDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneIndexEntryDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentPhoneIndexEntryDescriptor::GetNEntries(void) const
{
  return(nentries);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptor* SpokenContentPhoneIndexEntryDescriptor::GetEntry(const unsigned long idx)
{
  assert(entries != NULL);
  assert(idx<nentries);
  assert(entries[idx] != NULL);
  return(entries[idx]);
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentPhoneIndexEntryDescriptor::GetNKey(void) const
{
  return(nkey_elements);
}

//----------------------------------------------------------------------------
const char* SpokenContentPhoneIndexEntryDescriptor::GetKey(unsigned char idx) const
{
  assert(key_elements != NULL);
  assert(idx<nkey_elements);
  assert(key_elements[idx] != NULL);
  return(key_elements[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::SetEntry(const unsigned long idx,SpokenContentIndexEntryDescriptor* e)
{
  assert(idx<nentries);
  assert(e != NULL);
  if(entries[idx] != NULL) entries[idx]->release();
  entries[idx]=e;
  entries[idx]->addref();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::SetKey(const unsigned char idx,const char* k)
{
  assert(idx<nkey_elements);
  assert(k != NULL);
  assert(key_elements != NULL);
  if(key_elements[idx]) delete[] key_elements[idx];
  key_elements[idx]=new char[strlen(k)+1];
  strcpy(key_elements[idx],k);
}

//----------------------------------------------------------------------------
long SpokenContentPhoneIndexEntryDescriptor::GetKeyIndex(unsigned char idx) const
{
	assert(idx < nkey_elements);
	return key_indices[idx];
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::SetKeyIndex(unsigned char idx, long phoneIndex)
{
	assert(idx < nkey_elements);
	key_indices[idx] = phoneIndex;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::SetNKey(const unsigned char n)
{
  if(key_elements != NULL) {
    for(unsigned char i=0;i<nkey_elements;i++) {
      if(key_elements[i]) delete [] key_elements[i];
    }
    delete [] key_elements;
    key_elements=NULL;
  }
  if(key_indices) delete[] key_indices;
  nkey_elements=n;
  key_elements=new char*[nkey_elements];
  key_indices=new long[nkey_elements];
  for(unsigned char j=0;j<nkey_elements;j++) 
  {
	  key_elements[j]=NULL;
	  key_indices[j] = -1;
  }
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::SetNEntries(const unsigned long n)
{
  if(entries != NULL) {
    for(unsigned long i=0;i<nentries;i++) {
      if(entries[i] != NULL) entries[i]->release();
    }
    delete [] entries;
    entries=NULL;
  }
  entries=new SpokenContentIndexEntryDescriptor*[n];
  nentries=n;
  for(unsigned long j=0;j<nentries;j++) entries[j]=NULL;
}


//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::Display(void) const 
{
  assert(entries        != NULL);
  assert(key_elements   != NULL);
  printf("SpokenContentPhoneIndexEntry: key=[ ");
  if(key_elements != NULL) {
    for(unsigned char i=0;i<nkey_elements;i++) {
      if(key_elements[i] != NULL) printf("%s ",key_elements[i]);
    }
  }
  printf("]\n");
  if(entries != NULL) {
    for(unsigned long j=0;j<nentries;j++) {
      if(entries[j] != NULL) entries[j]->Display();
    }
  }
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneIndexEntryDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentPhoneIndexEntryDescriptorInterfaceABC *SpokenContentPhoneIndexEntryDescriptor::GetInterface(void)
{ return &m_Interface; }



