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
// Applicable File Name:  SpokenContentLink.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentLinkDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentLinkDescriptorInterface::myName = "SpokenContentLink Type Description Interface";

const UUID SpokenContentLinkDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLinkDescriptor::myName = "SpokenContentLink Type Descriptor";

const UUID SpokenContentLinkDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentLinkDescriptor::valName = "";

const UUID SpokenContentLinkDescriptorInterfaceABC::myID = UUID();

SpokenContentLinkDescriptorInterfaceABC::SpokenContentLinkDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentLinkDescriptorInterface::SpokenContentLinkDescriptorInterface(
  SpokenContentLinkDescriptor *aHisto): m_SpokenContentLinkDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptorInterface::~SpokenContentLinkDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::addref(void)
{ m_SpokenContentLinkDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::release(void)
{ m_SpokenContentLinkDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentLinkDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLinkDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const float SpokenContentLinkDescriptorInterface::GetProbability(void) const
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  return(m_SpokenContentLinkDescriptor->GetProbability());
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentLinkDescriptorInterface::GetNodeOffset(void) const
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  return(m_SpokenContentLinkDescriptor->GetNodeOffset());
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::SetProbability(const float p)
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  m_SpokenContentLinkDescriptor->SetProbability(p);
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::SetNodeOffset(const unsigned short n)
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  m_SpokenContentLinkDescriptor->SetNodeOffset(n);
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  m_SpokenContentLinkDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentLinkDescriptor != NULL);
  m_SpokenContentLinkDescriptor->Reset();
}

//=============================================================================
SpokenContentLinkDescriptor::SpokenContentLinkDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
probability(0.0f),
node(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentLinkDescriptor::SpokenContentLinkDescriptor(SpokenContentLinkDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
probability(0.0f),
node(0)
{
  SetExposedInterface(&m_Interface);
  SetProbability(data.GetProbability());
  SetNodeOffset(data.GetNodeOffset());
}


//----------------------------------------------------------------------------
SpokenContentLinkDescriptor::~SpokenContentLinkDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentLink delete\n");
#endif
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::Reset(void)
{
  SetProbability(0.0f);
  SetNodeOffset(0);
}

//----------------------------------------------------------------------------
const UUID& SpokenContentLinkDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentLinkDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentLinkDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentLinkDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentLinkDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const float SpokenContentLinkDescriptor::GetProbability(void) const
{return(probability);}

//----------------------------------------------------------------------------
const unsigned short SpokenContentLinkDescriptor::GetNodeOffset(void) const
{return(node);}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::SetProbability(const float pp)
{
  // Need to take into account rounding problems
  float p=pp;
  if((p>1.0f) && (p< 1.00001f)) p=1.0f;
  if((p<0.0f) && (p>-0.00001f)) p=0.0f;
  assert((p>=0.0f) && (p<=1.0f));
  probability=p;
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::SetNodeOffset(const unsigned short n)
{
  node=n;
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::Display(void) const 
{
  printf("SpokenContentLink: prob=%10.8f NodeOffset=%d\n",
         probability,node);
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentLinkDescriptor::release(void)
{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentLinkDescriptorInterfaceABC *SpokenContentLinkDescriptor::GetInterface(void)
{ return &m_Interface; }



