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
// Applicable File Name:  SpokenContent.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentDescriptorInterface::myName = "SpokenContent Type Description Interface";

const UUID SpokenContentDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentDescriptor::myName = "SpokenContent Type Descriptor";

const UUID SpokenContentDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentDescriptor::valName = "";

const UUID SpokenContentDescriptorInterfaceABC::myID = UUID();

SpokenContentDescriptorInterfaceABC::SpokenContentDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentDescriptorInterface::SpokenContentDescriptorInterface(
  SpokenContentDescriptor *aHisto): m_SpokenContentDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentDescriptorInterface::~SpokenContentDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::addref(void)
{ m_SpokenContentDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::release(void)
{ m_SpokenContentDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptor* SpokenContentDescriptorInterface::GetHeader(void)
{
  assert(m_SpokenContentDescriptor != NULL);
  return(m_SpokenContentDescriptor->GetHeader());
}

//----------------------------------------------------------------------------
unsigned long SpokenContentDescriptorInterface::GetNLattice(void) const
{
  assert(m_SpokenContentDescriptor != NULL);
  return(m_SpokenContentDescriptor->GetNLattice());
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptor* SpokenContentDescriptorInterface::GetLattice(const unsigned long idx)
{
  assert(m_SpokenContentDescriptor != NULL);
  return(m_SpokenContentDescriptor->GetLattice(idx));
}

//----------------------------------------------------------------------------
const long SpokenContentDescriptorInterface::FindLattice(const char* i)
{
  assert(m_SpokenContentDescriptor != NULL);
  return(m_SpokenContentDescriptor->FindLattice(i));
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::SetHeader(SpokenContentHeaderDescriptor* h)
{
  assert(m_SpokenContentDescriptor != NULL);
  m_SpokenContentDescriptor->SetHeader(h);
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::SetNLattice(const unsigned long n)
{
  assert(m_SpokenContentDescriptor != NULL);
  m_SpokenContentDescriptor->SetNLattice(n);
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::SetLattice(const unsigned long idx,SpokenContentLatticeDescriptor* l)
{
  assert(m_SpokenContentDescriptor != NULL);
  m_SpokenContentDescriptor->SetLattice(idx,l);
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentDescriptor != NULL);
  m_SpokenContentDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentDescriptor != NULL);
  m_SpokenContentDescriptor->Reset();
}

//=============================================================================
SpokenContentDescriptor::SpokenContentDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nlattice(0),
header(NULL),
lattices(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentDescriptor::SpokenContentDescriptor(SpokenContentDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nlattice(0),
header(NULL),
lattices(NULL)
{
  SetExposedInterface(&m_Interface);

  SetNLattice(data.GetNLattice());
  SetHeader(data.GetHeader());
  for(unsigned long i=0;i<nlattice;i++) SetLattice(i,data.GetLattice(i));
}


//----------------------------------------------------------------------------
SpokenContentDescriptor::~SpokenContentDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContent delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
SpokenContentHeaderDescriptor* SpokenContentDescriptor::GetHeader(void)
{
  assert(header != NULL);
  return(header);
}

//----------------------------------------------------------------------------
unsigned long SpokenContentDescriptor::GetNLattice(void) const
{
  return(nlattice);
}

//----------------------------------------------------------------------------
// Find a lattice with the given ID. If there is no such lattice return -1
const long SpokenContentDescriptor::FindLattice(const char* i)
{
  assert(i != NULL);
  for(unsigned long j=0;j<nlattice;j++) {
    if(strcmp(i,lattices[j]->GetLatticeID())==0) return(j);
  }
  return(-1);
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptor* SpokenContentDescriptor::GetLattice(const unsigned long idx)
{
  assert(idx < nlattice);
  assert(lattices != NULL);
  assert(lattices[idx] != NULL);
  return(lattices[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentDescriptor::SetHeader(SpokenContentHeaderDescriptor* h)
{
  assert(h != NULL);
  if(header != NULL) header->release();
  header=h;
  header->addref();
}

//----------------------------------------------------------------------------
void SpokenContentDescriptor::SetNLattice(const unsigned long n)
{
  unsigned long i;
  if(lattices != NULL) {
    for(i=0;i<nlattice;i++) {
      if(lattices[i] != NULL) lattices[i]->release();
    }
    delete [] lattices;
    lattices=NULL;
  }
  nlattice=n;
  if(n==0) return;

  lattices=new SpokenContentLatticeDescriptor*[nlattice];
  for(i=0;i<nlattice;i++) lattices[i]=NULL;
}

//----------------------------------------------------------------------------
void SpokenContentDescriptor::SetLattice(const unsigned long idx,SpokenContentLatticeDescriptor* l)
{
 assert(idx<nlattice);
 assert(lattices != NULL);
 assert(l != NULL);
 if(lattices[idx] != NULL) lattices[idx]->release();
 lattices[idx]=l;
 lattices[idx]->addref();
}


//----------------------------------------------------------------------------
void SpokenContentDescriptor::Display(void) const
{
  assert(header   != NULL);
  assert(lattices != NULL);
  printf("SpokenContentDescriptor:\n");
  header->Display();
  for(unsigned long i=0;i<nlattice;i++) {
    printf("%ld/%ld :",i,nlattice);
    lattices[i]->Display();
  }
}

//----------------------------------------------------------------------------
void SpokenContentDescriptor::Reset(void)
{
  if(header != NULL) header->release();
  header=NULL;
  SetNLattice(0);
}

//----------------------------------------------------------------------------
const UUID& SpokenContentDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpokenContentDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentDescriptorInterfaceABC *SpokenContentDescriptor::GetInterface(void)
{ return &m_Interface; }



