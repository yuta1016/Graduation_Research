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
// Applicable File Name:  ProbabilityMatrix.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID ProbabilityMatrixDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixDescriptorInterface::myName = "ProbabilityMatrix Type Description Interface";

const UUID ProbabilityMatrixDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixDescriptor::myName = "ProbabilityMatrix Type Descriptor";

const UUID ProbabilityMatrixDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *ProbabilityMatrixDescriptor::valName = "";

const UUID ProbabilityMatrixDescriptorInterfaceABC::myID = UUID();

ProbabilityMatrixDescriptorInterfaceABC::ProbabilityMatrixDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
ProbabilityMatrixDescriptorInterface::ProbabilityMatrixDescriptorInterface(
  ProbabilityMatrixDescriptor *aHisto): m_ProbabilityMatrixDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterface::~ProbabilityMatrixDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::addref(void)
{ m_ProbabilityMatrixDescriptor->addref(); }

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::release(void)
{ m_ProbabilityMatrixDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const unsigned long 
  ProbabilityMatrixDescriptorInterface::GetRows(void) const
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  return(m_ProbabilityMatrixDescriptor->GetRows());
}

//----------------------------------------------------------------------------
const unsigned long 
  ProbabilityMatrixDescriptorInterface::GetCols(void) const
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  return(m_ProbabilityMatrixDescriptor->GetCols());
}

//----------------------------------------------------------------------------
const float
  ProbabilityMatrixDescriptorInterface::GetValue(const unsigned long r,
    const unsigned long c) const
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  return(m_ProbabilityMatrixDescriptor->GetValue(r,c));
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::SetSize(const unsigned long r,const
  unsigned long c)
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  m_ProbabilityMatrixDescriptor->SetSize(r,c);
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::SetValue(const float p,const
  unsigned long r,const unsigned long c)
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  m_ProbabilityMatrixDescriptor->SetValue(p,r,c);
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::Display(void) const
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  m_ProbabilityMatrixDescriptor->Display();
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptorInterface::Reset(void)
{
  assert(m_ProbabilityMatrixDescriptor != NULL);
  m_ProbabilityMatrixDescriptor->Reset();
}

//=============================================================================
ProbabilityMatrixDescriptor::ProbabilityMatrixDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
rows(0),
cols(0),
value(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
ProbabilityMatrixDescriptor::ProbabilityMatrixDescriptor(ProbabilityMatrixDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
rows(0),
cols(0),
value(NULL)
{
  SetExposedInterface(&m_Interface);
  SetSize(data.GetRows(),data.GetCols());
  for(unsigned long r=0;r<data.GetRows();r++) {
    for(unsigned long c=0;c<data.GetCols();c++) {
      SetValue(data.GetValue(r,c),r,c);
    }
  }
}


//----------------------------------------------------------------------------
ProbabilityMatrixDescriptor::~ProbabilityMatrixDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"ProbabilityMatrix delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::Reset(void)
{
  delete [] value;
  value=NULL;
  rows=0;
  cols=0;
}

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ProbabilityMatrixDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& ProbabilityMatrixDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* ProbabilityMatrixDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool ProbabilityMatrixDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const unsigned long ProbabilityMatrixDescriptor::GetRows(void) const
{
  return(rows);
}

//----------------------------------------------------------------------------
const unsigned long ProbabilityMatrixDescriptor::GetCols(void) const
{
  return(cols);
}

//----------------------------------------------------------------------------
const float
  ProbabilityMatrixDescriptor::GetValue(const unsigned long r,const unsigned long c) const
{
  assert(r<rows);
  assert(c<cols);
  return(value[c+cols*r]);
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::SetSize(const unsigned long r,const unsigned long c)
{
  assert(r != 0);
  assert(c != 0);
  Reset();
  rows=r;
  cols=c;
  value=new float[r*c];
  for(unsigned long i=0;i<rows*cols;i++) value[i]=0.0f;
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::SetValue(const float p,const
  unsigned long r,const unsigned long c)
{
  assert(r<rows);
  assert(c<cols);
  assert((p>=-0.000001f) && (p<=1.000001f)); // handle rounding errors
  value[c+r*cols]=p;
}


//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::Display(void) const 
{
  assert(value != NULL);
  printf("ProbabilityMatrix: size=(rows=%ld,cols=%ld)\n",rows,cols);//sth
  for(unsigned long r=0;r<rows;r++) {
    printf("r=%ld: ",r);//sth
    for(unsigned long c=0;c<cols;c++) {
      printf("%8.6f ",GetValue(r,c));
      if((c % 8) == 7) printf("\n");
    }
    if(((cols-1) % 8) != 7) printf("\n");
  }
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void ProbabilityMatrixDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
ProbabilityMatrixDescriptorInterfaceABC *ProbabilityMatrixDescriptor::GetInterface(void)
{ return &m_Interface; }



