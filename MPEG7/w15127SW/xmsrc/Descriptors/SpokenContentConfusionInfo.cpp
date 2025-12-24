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
// Applicable File Name:  SpokenContentConfusionInfo.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentConfusionInfoDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoDescriptorInterface::myName = "SpokenContentConfusionInfo Type Description Interface";

const UUID SpokenContentConfusionInfoDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoDescriptor::myName = "SpokenContentConfusionInfo Type Descriptor";

const UUID SpokenContentConfusionInfoDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentConfusionInfoDescriptor::valName = "";

const UUID SpokenContentConfusionInfoDescriptorInterfaceABC::myID = UUID();

SpokenContentConfusionInfoDescriptorInterfaceABC::SpokenContentConfusionInfoDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentConfusionInfoDescriptorInterface::SpokenContentConfusionInfoDescriptorInterface(
  SpokenContentConfusionInfoDescriptor *aHisto): m_SpokenContentConfusionInfoDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterface::~SpokenContentConfusionInfoDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::addref(void)
{ m_SpokenContentConfusionInfoDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::release(void)
{ m_SpokenContentConfusionInfoDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned short SpokenContentConfusionInfoDescriptorInterface::GetSize(void) 
  const
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetSize());
}


//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* 
  SpokenContentConfusionInfoDescriptorInterface::GetOccuranceProb(void)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetOccuranceProb());
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* SpokenContentConfusionInfoDescriptorInterface::GetInsertionProb(void)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetInsertionProb());
}

//----------------------------------------------------------------------------
const float SpokenContentConfusionInfoDescriptorInterface::GetUnconditionalInsertionProb(void) const
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetUnconditionalInsertionProb());
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* SpokenContentConfusionInfoDescriptorInterface::GetOmissionProb(void)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetOmissionProb());
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptor* SpokenContentConfusionInfoDescriptorInterface::GetConfusionProb(void)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetConfusionProb());
}


//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetSize(const unsigned short n)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetSize(n);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetOccuranceProb(ProbabilityVectorDescriptor* v)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetOccuranceProb(v);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetInsertionProb(ProbabilityVectorDescriptor* v)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetInsertionProb(v);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetUnconditionalInsertionProb(const float p)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetUnconditionalInsertionProb(p);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetOmissionProb(ProbabilityVectorDescriptor* v)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetOmissionProb(v);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetConfusionProb(ProbabilityMatrixDescriptor* v)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetConfusionProb(v);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::SetConfusionID(const char* s)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->SetConfusionID(s);
}

//----------------------------------------------------------------------------
const char* SpokenContentConfusionInfoDescriptorInterface::GetConfusionID(void) const
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  return(m_SpokenContentConfusionInfoDescriptor->GetConfusionID());
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentConfusionInfoDescriptor != NULL);
  m_SpokenContentConfusionInfoDescriptor->Reset();
}

//=============================================================================
SpokenContentConfusionInfoDescriptor::SpokenContentConfusionInfoDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
size(0),
occurance(NULL),
insertion(NULL),
unconditional(0.0f),
omission(NULL),
confusion(NULL),
confusionId(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentConfusionInfoDescriptor::SpokenContentConfusionInfoDescriptor(SpokenContentConfusionInfoDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
size(0),
occurance(NULL),
insertion(NULL),
unconditional(0.0f),
omission(NULL),
confusion(NULL),
confusionId(NULL)
{
  SetExposedInterface(&m_Interface);
  SetSize(data.GetSize());
  SetOccuranceProb(data.GetOccuranceProb());
  SetInsertionProb(data.GetInsertionProb());
  SetUnconditionalInsertionProb(data.GetUnconditionalInsertionProb());
  SetConfusionProb(data.GetConfusionProb());
  SetConfusionID(data.GetConfusionID());
}


//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptor::~SpokenContentConfusionInfoDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentConfusionInfo delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::Reset(void)
{
  if(occurance != NULL) {
    occurance->release();
    occurance=NULL;
  }
  if(insertion != NULL) {
    insertion->release();
    insertion=NULL;
  }
  if(omission != NULL) {
    omission->release();
    omission=NULL;
  }
  if(confusion != NULL) {
    confusion->release();
    confusion=NULL;
  }
  if(confusionId != NULL) {
    delete [] confusionId;
    confusionId=NULL;
  }
  size=0;
  unconditional=0.0f;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentConfusionInfoDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentConfusionInfoDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentConfusionInfoDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentConfusionInfoDescriptor::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
const unsigned short SpokenContentConfusionInfoDescriptor::GetSize(void) const
{
  return(size);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* SpokenContentConfusionInfoDescriptor::GetOccuranceProb(void)
{
  assert(occurance != NULL);
  return(occurance);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* SpokenContentConfusionInfoDescriptor::GetInsertionProb(void)
{
  assert(insertion != NULL);
  return(insertion);
}

//----------------------------------------------------------------------------
const float SpokenContentConfusionInfoDescriptor::GetUnconditionalInsertionProb(void) const
{
  return(unconditional);
}

//----------------------------------------------------------------------------
ProbabilityVectorDescriptor* SpokenContentConfusionInfoDescriptor::GetOmissionProb(void)
{
  assert(omission != NULL);
  return(omission);
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptor* SpokenContentConfusionInfoDescriptor::GetConfusionProb(void)
{
  assert(confusion != NULL);
  return(confusion);
}


//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetSize(const unsigned short n)
{
  Reset();
  size=n;
  occurance=new ProbabilityVectorDescriptor();
  occurance->addref();
  occurance->SetSize(size);
  insertion=new ProbabilityVectorDescriptor();
  insertion->addref();
  insertion->SetSize(size);
  omission=new ProbabilityVectorDescriptor();
  omission->addref();
  omission->SetSize(size);
  confusion=new ProbabilityMatrixDescriptor();
  confusion->addref();
  confusion->SetSize(size,size);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetOccuranceProb(ProbabilityVectorDescriptor* v)
{
  assert(v != NULL);
  assert(v->GetSize() == size);
  if(occurance != NULL) occurance->release();
  occurance=v;
  occurance->addref();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetInsertionProb(ProbabilityVectorDescriptor* v)
{
  assert(v != NULL);
  assert(v->GetSize() == size);
  if(insertion != NULL) insertion->release();
  insertion=v;
  insertion->addref();
}


//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetUnconditionalInsertionProb(const float p)
{
  assert((p>=-0.00001f) && (p<=1.00001f)); // Allow rounding errors
  unconditional=p;
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetOmissionProb(ProbabilityVectorDescriptor* v)
{
  assert(v != NULL);
  assert(v->GetSize() == size);
  if(omission != NULL) omission->release();
  omission=v;
  omission->addref();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetConfusionProb(ProbabilityMatrixDescriptor* v)
{
  assert(v != NULL);
  assert(v->GetRows() == size);
  assert(v->GetCols() == size);
  if(confusion != NULL) confusion->release();
  confusion=v;
  confusion->addref();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::SetConfusionID(const char* s)
{
  assert(s != NULL);
  delete [] confusionId;
  confusionId=new char[strlen(s)+1];
  strcpy(confusionId,s);
}

//----------------------------------------------------------------------------
const char* SpokenContentConfusionInfoDescriptor::GetConfusionID(void) const
{
  assert(confusionId != NULL);
  return(confusionId);
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::Display(void) const 
{
  assert(size != 0);
  assert(occurance != NULL);
  assert(insertion != NULL);
  assert(omission  != NULL);
  assert(confusion != NULL);
  assert(confusionId != NULL);

  printf("SpokenContentConfusionInfo: id=[%s] size=%d, ",GetConfusionID(),
         GetSize());
  occurance->Display();
  printf("Insertion: unconditional=%8.6f:\n",unconditional);
  insertion->Display();
  omission->Display();
  confusion->Display();
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentConfusionInfoDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentConfusionInfoDescriptorInterfaceABC *SpokenContentConfusionInfoDescriptor::GetInterface(void)
{ return &m_Interface; }



