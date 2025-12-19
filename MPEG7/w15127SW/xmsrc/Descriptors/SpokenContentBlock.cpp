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
// Applicable File Name:  SpokenContentBlock.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentBlockDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockDescriptorInterface::myName = "SpokenContentBlock Type Description Interface";

const UUID SpokenContentBlockDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockDescriptor::myName = "SpokenContentBlock Type Descriptor";

const UUID SpokenContentBlockDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentBlockDescriptor::valName = "";

const UUID SpokenContentBlockDescriptorInterfaceABC::myID = UUID();

SpokenContentBlockDescriptorInterfaceABC::SpokenContentBlockDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentBlockDescriptorInterface::SpokenContentBlockDescriptorInterface(
  SpokenContentBlockDescriptor *aHisto): m_SpokenContentBlockDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterface::~SpokenContentBlockDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::addref(void)
{ m_SpokenContentBlockDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::release(void)
{ m_SpokenContentBlockDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetMediaTime(const char* t)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetMediaTime(t);
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptorInterface::GetMediaTime(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetMediaTime());
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetDefaultSpeaker(const char* s)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetDefaultSpeaker(s);
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptorInterface::GetDefaultSpeaker(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetDefaultSpeaker());
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetNum(const unsigned short n)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetNum(n);
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentBlockDescriptorInterface::GetNum(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetNum());
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetNNodes(const unsigned short n)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetNNodes(n);
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentBlockDescriptorInterface::GetNNodes(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetNNodes());
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptorInterface::GetAudio(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetAudio());
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetAudio(const char* a)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetAudio(a);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::SetNode(const unsigned short idx,
                             SpokenContentNodeDescriptor* n)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->SetNode(idx,n);
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptor* SpokenContentBlockDescriptorInterface::GetNode(const unsigned short idx)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  return(m_SpokenContentBlockDescriptor->GetNode(idx));
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentBlockDescriptor != NULL);
  m_SpokenContentBlockDescriptor->Reset();
}

//=============================================================================
SpokenContentBlockDescriptor::SpokenContentBlockDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
mediatime(NULL),
nnodes(0),
num(0),
audio(NULL),
nodes(NULL),
defaultSpeaker(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentBlockDescriptor::SpokenContentBlockDescriptor(SpokenContentBlockDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
mediatime(NULL),
nnodes(0),
num(0),
audio(NULL),
nodes(NULL),
defaultSpeaker(NULL)
{
  SetExposedInterface(&m_Interface);
  SetMediaTime(data.GetMediaTime());
  SetNNodes(data.GetNNodes());
  SetNum(data.GetNum());
  SetAudio(data.GetAudio());
  for(unsigned short i=0;i<data.GetNNodes();i++) SetNode(i,data.GetNode(i));
  SetDefaultSpeaker(data.GetDefaultSpeaker());
}


//----------------------------------------------------------------------------
SpokenContentBlockDescriptor::~SpokenContentBlockDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentBlock delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::Reset(void)
{
  delete [] mediatime;
  mediatime=NULL;
  SetNNodes(0);
  num=0;
  delete [] audio;
  audio=NULL;
  delete [] defaultSpeaker;
  defaultSpeaker=NULL;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentBlockDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentBlockDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentBlockDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetMediaTime(const char* t)
{
  assert(t != NULL);
  delete [] mediatime;
  mediatime=new char[strlen(t)+1];
  strcpy(mediatime,t);
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptor::GetMediaTime(void) const
{
  return(mediatime);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetDefaultSpeaker(const char* s)
{
  assert(s != NULL);
  delete [] defaultSpeaker;
  defaultSpeaker=new char[strlen(s)+1];
  strcpy(defaultSpeaker,s);
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptor::GetDefaultSpeaker(void) const
{
  return(defaultSpeaker);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetNum(const unsigned short n)
{
  num=n;
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentBlockDescriptor::GetNum(void) const
{
  return(num);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetNNodes(const unsigned short n)
{
  unsigned short i;
  if(nodes != NULL) {
    for(i=0;i<nnodes;i++) {
      if(nodes[i] != NULL) nodes[i]->release();
    }
    delete [] nodes;
    nodes=NULL;
  }
  nnodes=n;
  if(n==0) return;
  nodes=new SpokenContentNodeDescriptor*[n];
  for(i=0;i<nnodes;i++) nodes[i]=NULL;
}

//----------------------------------------------------------------------------
const unsigned short SpokenContentBlockDescriptor::GetNNodes(void) const
{
  return(nnodes);
}

//----------------------------------------------------------------------------
const char* SpokenContentBlockDescriptor::GetAudio(void) const
{
  assert(audio != NULL);
  return(audio);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetAudio(const char* a)
{
  assert(a != NULL);
  delete [] audio;
  audio=new char[strlen(a)+1];
  strcpy(audio,a);
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::SetNode(const unsigned short idx,
                             SpokenContentNodeDescriptor* n)
{
  assert(idx<nnodes);
  assert(n != NULL);
  assert(nodes != NULL);
  assert(idx == n->GetNum());
  if(nodes[idx] != NULL) nodes[idx]->release();
  nodes[idx]=n;
  nodes[idx]->addref();
}

//----------------------------------------------------------------------------
SpokenContentNodeDescriptor* SpokenContentBlockDescriptor::GetNode(const unsigned short idx)
{
  assert(idx<nnodes);
  assert(nodes != NULL);
  return(nodes[idx]);
}


//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::Display(void) const 
{
  assert(mediatime != NULL);
  assert(audio     != NULL);
  assert(nodes     != NULL);
  printf("SpokenContentBlock: num=%d audio=[%s] mediatime=[%s] "
         "defaultSpeaker=[%s]",num,audio,
         mediatime,defaultSpeaker);
  for(unsigned short i=0;i<nnodes;i++) {
    printf("%d/%d: ",i,nnodes);
    nodes[i]->Display();
  }
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentBlockDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentBlockDescriptorInterfaceABC *SpokenContentBlockDescriptor::GetInterface(void)
{ return &m_Interface; }



