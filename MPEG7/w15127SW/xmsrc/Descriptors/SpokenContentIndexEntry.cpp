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
// Applicable File Name:  SpokenContentIndexEntry.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentIndexEntryDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryDescriptorInterface::myName = "SpokenContentIndexEntry Type Description Interface";

const UUID SpokenContentIndexEntryDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryDescriptor::myName = "SpokenContentIndexEntry Type Descriptor";

const UUID SpokenContentIndexEntryDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentIndexEntryDescriptor::valName = "";

const UUID SpokenContentIndexEntryDescriptorInterfaceABC::myID = UUID();

SpokenContentIndexEntryDescriptorInterfaceABC::SpokenContentIndexEntryDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentIndexEntryDescriptorInterface::SpokenContentIndexEntryDescriptorInterface(
  SpokenContentIndexEntryDescriptor *aHisto): m_SpokenContentIndexEntryDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterface::~SpokenContentIndexEntryDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::addref(void)
{ m_SpokenContentIndexEntryDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::release(void)
{ m_SpokenContentIndexEntryDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned short SpokenContentIndexEntryDescriptorInterface::GetNode(void) const
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  return m_SpokenContentIndexEntryDescriptor->GetNode();
}

//----------------------------------------------------------------------------
const unsigned long SpokenContentIndexEntryDescriptorInterface::GetBlock(void) const
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  return m_SpokenContentIndexEntryDescriptor->GetBlock();
}

//----------------------------------------------------------------------------
const char* SpokenContentIndexEntryDescriptorInterface::GetLattice(void) const
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  return m_SpokenContentIndexEntryDescriptor->GetLattice();
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::SetNode(const unsigned short n)
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  m_SpokenContentIndexEntryDescriptor->SetNode(n);
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::SetBlock(const unsigned long b)
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  m_SpokenContentIndexEntryDescriptor->SetBlock(b);
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::SetLattice(const char* l)
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  m_SpokenContentIndexEntryDescriptor->SetLattice(l);
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  m_SpokenContentIndexEntryDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentIndexEntryDescriptor != NULL);
  m_SpokenContentIndexEntryDescriptor->Reset();
}

//=============================================================================
SpokenContentIndexEntryDescriptor::SpokenContentIndexEntryDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
node(0),
block(0),
lattice(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentIndexEntryDescriptor::SpokenContentIndexEntryDescriptor(SpokenContentIndexEntryDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
node(0),
block(0),
lattice(NULL)
{
  SetExposedInterface(&m_Interface);
  SetNode(data.GetNode());
  SetBlock(data.GetBlock());
  SetLattice(data.GetLattice());
}


//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptor::~SpokenContentIndexEntryDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentIndexEntry delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::Reset(void)
{
  SetNode(0);
  SetBlock(0);
  SetLattice(NULL);
}

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentIndexEntryDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentIndexEntryDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentIndexEntryDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentIndexEntryDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned short SpokenContentIndexEntryDescriptor::GetNode(void) const
{return(node);}

//----------------------------------------------------------------------------
const unsigned long SpokenContentIndexEntryDescriptor::GetBlock(void) const
{return(block);}

//----------------------------------------------------------------------------
// Note the lattice may be null indicating no lattice specified
const char* SpokenContentIndexEntryDescriptor::GetLattice(void) const
{
  return(lattice);
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::SetNode(const unsigned short n) 
{
  node=n;
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::SetBlock(const unsigned long b)
{
  block=b;
}

//----------------------------------------------------------------------------
// Note the lattice may be null indicating no lattice specified
void SpokenContentIndexEntryDescriptor::SetLattice(const char* l)
{
  delete [] lattice;
  lattice=NULL;
  if(l==NULL) return;
  lattice=new char[strlen(l)+1];
  strcpy(lattice,l);
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::Display(void) const 
{
  printf("SpokenContentIndexEntry:\n");
  printf("Node=%d, Block=%ld ",node,block);//sth
  if(lattice != NULL) printf("Lattice=[%s]",lattice);
  printf("\n");
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentIndexEntryDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentIndexEntryDescriptorInterfaceABC *SpokenContentIndexEntryDescriptor::GetInterface(void)
{ return &m_Interface; }



