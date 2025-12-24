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
// Applicable File Name:  SpokenContentPhoneLink.cpp
//
#include <stdio.h>
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID SpokenContentPhoneLinkDescriptorInterface::myID = UUID("09df7000-db15-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkDescriptorInterface::myName = "SpokenContentPhoneLink Type Description Interface";

const UUID SpokenContentPhoneLinkDescriptor::myID = UUID("def04360-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkDescriptor::myName = "SpokenContentPhoneLink Type Descriptor";

const UUID SpokenContentPhoneLinkDescriptor::valID = UUID("f241a300-dafb-11d2-affe-0080c7e1e76d");
const char *SpokenContentPhoneLinkDescriptor::valName = "";

const UUID SpokenContentPhoneLinkDescriptorInterfaceABC::myID = UUID();

SpokenContentPhoneLinkDescriptorInterfaceABC::SpokenContentPhoneLinkDescriptorInterfaceABC()
{ SetInterfaceID(myID);}


//=============================================================================
SpokenContentPhoneLinkDescriptorInterface::SpokenContentPhoneLinkDescriptorInterface(
  SpokenContentPhoneLinkDescriptor *aHisto): m_SpokenContentPhoneLinkDescriptor(aHisto)
{
  SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterface::~SpokenContentPhoneLinkDescriptorInterface()
{
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::addref(void)
{ m_SpokenContentPhoneLinkDescriptor->addref(); }

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::release(void)
{ m_SpokenContentPhoneLinkDescriptor->release(); }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLinkDescriptorInterface::GetPhone(void) const
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  return(m_SpokenContentPhoneLinkDescriptor->GetPhone());
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptor* SpokenContentPhoneLinkDescriptorInterface::GetLink(void)
{return m_SpokenContentPhoneLinkDescriptor->GetLink();}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::SetPhone(const char* w)
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  m_SpokenContentPhoneLinkDescriptor->SetPhone(w);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::SetPhoneIndex(long idx)
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  m_SpokenContentPhoneLinkDescriptor->SetPhoneIndex(idx);
}

//----------------------------------------------------------------------------
long SpokenContentPhoneLinkDescriptorInterface::GetPhoneIndex() const
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  return m_SpokenContentPhoneLinkDescriptor->GetPhoneIndex();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::SetLink(SpokenContentLinkDescriptor* l)
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  m_SpokenContentPhoneLinkDescriptor->SetLink(l);
}
 
//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::Display(void) const
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  m_SpokenContentPhoneLinkDescriptor->Display();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptorInterface::Reset(void)
{
  assert(m_SpokenContentPhoneLinkDescriptor != NULL);
  m_SpokenContentPhoneLinkDescriptor->Reset();
}

//=============================================================================
SpokenContentPhoneLinkDescriptor::SpokenContentPhoneLinkDescriptor():
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
link(NULL),
phoneIndex(-1), 
phone(NULL)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
// This does the copy constructor
SpokenContentPhoneLinkDescriptor::SpokenContentPhoneLinkDescriptor(SpokenContentPhoneLinkDescriptor& data) :
m_refcount(0),
m_Interface(this),
m_isProprietary(false),
link(NULL),
phoneIndex(-1),
phone(NULL)
{
  SetExposedInterface(&m_Interface);
  Reset();
  SetLink(data.GetLink());
  SetPhone(data.GetPhone());
}


//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptor::~SpokenContentPhoneLinkDescriptor()
{
#ifdef VERBOSE
       fprintf(stderr,"SpokenContentPhoneLink delete\n");
#endif
  Reset();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::Reset(void)
{
  if(link != NULL) link->release();
  link=NULL;
  if(phone != NULL) delete [] phone;
  phone = 0;
}

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SpokenContentPhoneLinkDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& SpokenContentPhoneLinkDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLinkDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool SpokenContentPhoneLinkDescriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const char* SpokenContentPhoneLinkDescriptor::GetPhone(void) const
{
  assert(phone != NULL);
  return(phone);
}

//----------------------------------------------------------------------------
SpokenContentLinkDescriptor* SpokenContentPhoneLinkDescriptor::GetLink(void)
{
  assert(link != NULL);
  return(link);
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::SetLink(SpokenContentLinkDescriptor* l)
{
  if(link != NULL) link->release();
  link=l;
  if(link != NULL) link->addref();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::SetPhone(const char* w)
{
  assert(w != NULL);
  // Bin the whitespace and convert to lower case
  char *t1=new char[strlen(w)+1];
  char *t2=new char[strlen(w)+1];
  if(sscanf(w,"%s%s",t1,t2) != 1) {
    fprintf(stderr,"SpokenContentPhoneLink::SetPhone: "
            "incorrect number of fields\n");
    return;
  }

  if(phone) delete[] phone;
  phone=new char[strlen(t1)+1];
  strcpy(phone,t1);

  delete [] t2;
  delete [] t1;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::SetPhoneIndex(long idx)
{
	phoneIndex = idx;
}

//----------------------------------------------------------------------------
long SpokenContentPhoneLinkDescriptor::GetPhoneIndex() const
{
	return phoneIndex;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::Display(void) const 
{
  printf("SpokenContentPhoneLink: phone=[%s], ",GetPhone());
  link->Display();
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::addref(void)

{
       m_refcount++;
}

//----------------------------------------------------------------------------
void SpokenContentPhoneLinkDescriptor::release(void)

{
  if(!(--m_refcount)) delete this;
}


//----------------------------------------------------------------------------
SpokenContentPhoneLinkDescriptorInterfaceABC *SpokenContentPhoneLinkDescriptor::GetInterface(void)
{ return &m_Interface; }



