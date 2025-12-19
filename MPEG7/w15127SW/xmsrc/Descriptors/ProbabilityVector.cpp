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
// Applicable File Name:  ProbabilityVector.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID ProbabilityVectorDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorDescriptorInterface::myName = "ProbabilityVector Type Description Interface";

const UUID ProbabilityVectorDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorDescriptor::myName = "ProbabilityVector Type Descriptor";

const UUID ProbabilityVectorDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *ProbabilityVectorDescriptor::valName = "";

const UUID ProbabilityVectorDescriptorInterfaceABC::myID = UUID();

ProbabilityVectorDescriptorInterfaceABC::ProbabilityVectorDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
ProbabilityVectorDescriptorInterface::ProbabilityVectorDescriptorInterface(
  ProbabilityVectorDescriptor *aHisto): m_ProbabilityVectorDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterface::~ProbabilityVectorDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::addref(void)
{ m_ProbabilityVectorDescriptor->addref(); }

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::release(void)
{ m_ProbabilityVectorDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long 
  ProbabilityVectorDescriptorInterface::GetSize(void) const
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  return(m_ProbabilityVectorDescriptor->GetSize());
}

//----------------------------------------------------------------------------
const float
  ProbabilityVectorDescriptorInterface::GetValue(const unsigned long idx) const
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  return(m_ProbabilityVectorDescriptor->GetValue(idx));
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::SetSize(const unsigned long n)
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  m_ProbabilityVectorDescriptor->SetSize(n);
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::SetValue(const float p,const
  unsigned long idx)
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  m_ProbabilityVectorDescriptor->SetValue(p,idx);
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::Display(void) const
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  m_ProbabilityVectorDescriptor->Display();
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptorInterface::Reset(void)
{
  assert(m_ProbabilityVectorDescriptor != NULL);
  m_ProbabilityVectorDescriptor->Reset();
}

//=============================================================================
ProbabilityVectorDescriptor::ProbabilityVectorDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
size(0),
value(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
ProbabilityVectorDescriptor::ProbabilityVectorDescriptor(ProbabilityVectorDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
size(0),
value(NULL)
{
  SetExposedInterface(&m_Interface);
  SetSize(data.GetSize());
  for(unsigned long i=0;i<data.GetSize();i++) SetValue(data.GetValue(i),i);
}


//----------------------------------------------------------------------------
ProbabilityVectorDescriptor::~ProbabilityVectorDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"ProbabilityVector delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::Reset(void)
{
  delete [] value;
  value=NULL;
  size=0;
}

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityVectorDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ProbabilityVectorDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ProbabilityVectorDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ProbabilityVectorDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned long ProbabilityVectorDescriptor::GetSize(void) const
{
  return(size);
}

//----------------------------------------------------------------------------
const float
  ProbabilityVectorDescriptor::GetValue(const unsigned long idx) const
{
  assert(idx<size);
  return(value[idx]);
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::SetSize(const unsigned long n)
{
  Reset();
  size=n;
  value=new float[n];
  for(unsigned long i=0;i<size;i++) value[i]=0.0f;
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::SetValue(const float p,const
  unsigned long idx)
{
  assert(idx<size);
  assert((p>=-0.000001f) && (p<=1.000001f)); // handle rounding errors
  value[idx]=p;
}


//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::Display(void) const 
{
  assert(value != NULL);
  printf("ProbabilityVector: size=%ld\n",size);//sth
  for(unsigned long i=0;i<size;i++) {
    printf("%8.6f ",GetValue(i));
    if((i % 8) == 7) printf("\n");
  }
  if(((size-1) % 8) != 7) printf("\n");
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void ProbabilityVectorDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
ProbabilityVectorDescriptorInterfaceABC *ProbabilityVectorDescriptor::GetInterface(void)
{ return &m_Interface; }



