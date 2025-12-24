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
// Applicable File Name:  SpokenContentLattice.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentLatticeDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeDescriptorInterface::myName = "SpokenContentLattice Type Description Interface";

const UUID SpokenContentLatticeDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeDescriptor::myName = "SpokenContentLattice Type Descriptor";

const UUID SpokenContentLatticeDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLatticeDescriptor::valName = "";

const UUID SpokenContentLatticeDescriptorInterfaceABC::myID = UUID();

SpokenContentLatticeDescriptorInterfaceABC::SpokenContentLatticeDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentLatticeDescriptorInterface::SpokenContentLatticeDescriptorInterface(
  SpokenContentLatticeDescriptor *aHisto): m_SpokenContentLatticeDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterface::~SpokenContentLatticeDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::addref(void)
{ m_SpokenContentLatticeDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::release(void)
{ m_SpokenContentLatticeDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned short SpokenContentLatticeDescriptorInterface::GetNBlocks(void) const
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  return(m_SpokenContentLatticeDescriptor->GetNBlocks());
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::SetNBlocks(const unsigned short n)
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  m_SpokenContentLatticeDescriptor->SetNBlocks(n);
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptor* SpokenContentLatticeDescriptorInterface::GetBlock(const unsigned short idx)
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  return(m_SpokenContentLatticeDescriptor->GetBlock(idx));
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::SetBlock(const unsigned short idx,SpokenContentBlockDescriptor* b)
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  m_SpokenContentLatticeDescriptor->SetBlock(idx,b);
}

//----------------------------------------------------------------------------
const char* SpokenContentLatticeDescriptorInterface::GetLatticeID(void) const
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  return(m_SpokenContentLatticeDescriptor->GetLatticeID());
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::SetLatticeID(const char* i)
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  m_SpokenContentLatticeDescriptor->SetLatticeID(i);
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  m_SpokenContentLatticeDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentLatticeDescriptor != NULL);
  m_SpokenContentLatticeDescriptor->Reset();
}

//=============================================================================
SpokenContentLatticeDescriptor::SpokenContentLatticeDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nblocks(0),
lattice_id(NULL),
blocks(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentLatticeDescriptor::SpokenContentLatticeDescriptor(SpokenContentLatticeDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
nblocks(0),
lattice_id(NULL),
blocks(NULL)
{
  SetExposedInterface(&m_Interface);

  SetNBlocks(data.GetNBlocks());
  SetLatticeID(data.GetLatticeID());
  for(unsigned short i=0;i<nblocks;i++) SetBlock(i,data.GetBlock(i));
}


//----------------------------------------------------------------------------
SpokenContentLatticeDescriptor::~SpokenContentLatticeDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentLattice delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentLatticeDescriptor::GetNBlocks(void) const
{
  return(nblocks);
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::SetNBlocks(const unsigned short n)
{
  unsigned short i;
  if(blocks != NULL) {
    for(i=0;i<nblocks;i++) blocks[i]->release();
    delete [] blocks;
    blocks=NULL;
  }
  nblocks=n;
  if(n==0) return;
  blocks=new SpokenContentBlockDescriptor*[n];
  for(i=0;i<n;i++) blocks[i]=NULL;
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptor* SpokenContentLatticeDescriptor::GetBlock(const unsigned short idx)
{
  assert(idx<nblocks);
  assert(blocks != NULL);
  assert(blocks[idx] != NULL);
  return(blocks[idx]);
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::SetBlock(const unsigned short idx,
  SpokenContentBlockDescriptor* b)
{
  assert(idx<nblocks);
  assert(blocks != NULL);
  assert(b != NULL);
  assert(idx == b->GetNum());
  if(blocks[idx] != NULL) blocks[idx]->release();
  blocks[idx]=b;  
  blocks[idx]->addref();
}

//----------------------------------------------------------------------------
const char* SpokenContentLatticeDescriptor::GetLatticeID(void) const
{
  assert(lattice_id != NULL);
  return(lattice_id);
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::SetLatticeID(const char* i)
{
  assert(i != NULL);
  delete [] lattice_id;
  lattice_id=new char[strlen(i)+1];
  strcpy(lattice_id,i);
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::Display(void) const
{
  assert(lattice_id != NULL);
  assert(blocks     != NULL);
  printf("SpokenContentLatticeDescriptor: id=[%s]\n",lattice_id);
  for(unsigned short i=0;i<nblocks;i++) {
    printf("%d/%d :",i,nblocks);
    blocks[i]->Display();
  }
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::Reset(void)
{
  delete [] lattice_id;
  lattice_id=NULL;
  SetNBlocks(0);
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLatticeDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLatticeDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLatticeDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLatticeDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentLatticeDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentLatticeDescriptorInterfaceABC *SpokenContentLatticeDescriptor::GetInterface(void)
{ return &m_Interface; }



