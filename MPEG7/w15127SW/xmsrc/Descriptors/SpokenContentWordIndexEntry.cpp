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
// Applicable File Name:  SpokenContentWordIndexEntry.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentWordIndexEntryDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordIndexEntryDescriptorInterface::myName = "SpokenContentWordIndexEntry Type Description Interface";

const UUID SpokenContentWordIndexEntryDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordIndexEntryDescriptor::myName = "SpokenContentWordIndexEntry Type Descriptor";

const UUID SpokenContentWordIndexEntryDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentWordIndexEntryDescriptor::valName = "";

const UUID SpokenContentWordIndexEntryDescriptorInterfaceABC::myID = UUID();

SpokenContentWordIndexEntryDescriptorInterfaceABC::SpokenContentWordIndexEntryDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentWordIndexEntryDescriptorInterface::SpokenContentWordIndexEntryDescriptorInterface(
  SpokenContentWordIndexEntryDescriptor *aHisto): m_SpokenContentWordIndexEntryDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptorInterface::~SpokenContentWordIndexEntryDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::addref(void)
{ m_SpokenContentWordIndexEntryDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::release(void)
{ m_SpokenContentWordIndexEntryDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordIndexEntryDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentWordIndexEntryDescriptorInterface::GetNEntries(void) const
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  return(m_SpokenContentWordIndexEntryDescriptor->GetNEntries());
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptor* SpokenContentWordIndexEntryDescriptorInterface::GetEntry(const unsigned long idx)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  return(m_SpokenContentWordIndexEntryDescriptor->GetEntry(idx));
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentWordIndexEntryDescriptorInterface::GetNKey(void) const
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  return(m_SpokenContentWordIndexEntryDescriptor->GetNKey());
}

//----------------------------------------------------------------------------
const char* SpokenContentWordIndexEntryDescriptorInterface::GetKey(const unsigned char idx) const
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  return(m_SpokenContentWordIndexEntryDescriptor->GetKey(idx));
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::SetKeyIndex(unsigned char idx, long wordIndex)
{
	assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
	m_SpokenContentWordIndexEntryDescriptor->SetKeyIndex(idx, wordIndex);
}

//----------------------------------------------------------------------------
long SpokenContentWordIndexEntryDescriptorInterface::GetKeyIndex(unsigned char idx) const
{
	assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
	return(m_SpokenContentWordIndexEntryDescriptor->GetKeyIndex(idx));
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::SetEntry(const unsigned long idx,SpokenContentIndexEntryDescriptor* e)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->SetEntry(idx,e);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::SetKey(const unsigned char idx,const char* k)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->SetKey(idx,k);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::SetNKey(const unsigned char n)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->SetNKey(n);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::SetNEntries(const unsigned long n)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->SetNEntries(n);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentWordIndexEntryDescriptor != NULL);
  m_SpokenContentWordIndexEntryDescriptor->Reset();
}

//=============================================================================
SpokenContentWordIndexEntryDescriptor::SpokenContentWordIndexEntryDescriptor():
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
SpokenContentWordIndexEntryDescriptor::SpokenContentWordIndexEntryDescriptor(SpokenContentWordIndexEntryDescriptor& data) :
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
SpokenContentWordIndexEntryDescriptor::~SpokenContentWordIndexEntryDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentWordIndexEntry delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::Reset(void)
{
  if(entries != NULL) {
    for(unsigned long i=0;i<nentries;i++) entries[i]->release();
    delete [] entries;
    entries=NULL;
  }
  if(key_elements != NULL) {
    for(unsigned char j=0;j<nkey_elements;j++) 
	{
		if(key_elements[j])
			delete []key_elements[j];
	}
    delete [] key_elements;
    key_elements=NULL;
  }
  if(key_indices)
  {
	  delete[] key_indices;
	  key_indices = 0;
  }
  nentries=0;
  nkey_elements=0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentWordIndexEntryDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentWordIndexEntryDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentWordIndexEntryDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentWordIndexEntryDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned long SpokenContentWordIndexEntryDescriptor::GetNEntries(void) const
{
  return(nentries);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptor* SpokenContentWordIndexEntryDescriptor::GetEntry(const unsigned long idx)
{
  assert(entries != NULL);
  assert(idx<nentries);
  assert(entries[idx] != NULL);
  return(entries[idx]);
}

//----------------------------------------------------------------------------
const unsigned char SpokenContentWordIndexEntryDescriptor::GetNKey(void) const
{
  return(nkey_elements);
}

//----------------------------------------------------------------------------
const char* SpokenContentWordIndexEntryDescriptor::GetKey(const unsigned char idx) const
{
  assert(key_elements != NULL);
  assert(idx<nkey_elements);
  assert(key_elements[idx] != NULL);
  return(key_elements[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::SetEntry(const unsigned long idx,SpokenContentIndexEntryDescriptor* e)
{
  assert(idx<nentries);
  assert(e != NULL);
  if(entries[idx] != NULL) entries[idx]->release();
  entries[idx]=e;
  entries[idx]->addref();
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::SetKey(const unsigned char idx,const char* k)
{
  assert(idx<nkey_elements);
  assert(k != NULL);
  assert(key_elements != NULL);
  if(key_elements[idx]) delete[] key_elements[idx];
  key_elements[idx]=new char[strlen(k)+1];
  strcpy(key_elements[idx],k);
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::SetKeyIndex(unsigned char idx, long wordIndex)
{
	assert(idx < nkey_elements);
	key_indices[idx] = wordIndex;
}

//----------------------------------------------------------------------------
long SpokenContentWordIndexEntryDescriptor::GetKeyIndex(unsigned char idx) const
{
	assert(idx < nkey_elements);
	return key_indices[idx];
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::SetNKey(const unsigned char n)
{
  if(key_elements != NULL) {
    for(unsigned char i=0;i<nkey_elements;i++) 
	{
		if(key_elements[i])
			delete[] key_elements[i];
	}
    delete [] key_elements;
    key_elements=NULL;
  }
  if(key_indices) delete[] key_indices;
  nkey_elements=n;
  key_elements=new char*[nkey_elements];
  key_indices = new long[nkey_elements];
  for(unsigned char j=0;j<nkey_elements;j++) 
  {
	  key_elements[j]=NULL;
	  key_indices[j] = 0;
  }
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::SetNEntries(const unsigned long n)
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
void SpokenContentWordIndexEntryDescriptor::Display(void) const 
{
  assert(entries        != NULL);
  assert(key_elements   != NULL);
  printf("SpokenContentWordIndexEntry: key=[ ");
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
void SpokenContentWordIndexEntryDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentWordIndexEntryDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentWordIndexEntryDescriptorInterfaceABC *SpokenContentWordIndexEntryDescriptor::GetInterface(void)
{ return &m_Interface; }



