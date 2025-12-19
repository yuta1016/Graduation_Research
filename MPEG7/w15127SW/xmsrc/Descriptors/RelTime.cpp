///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, J. Heuer
// (contributing organizations names)
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
// Applicable File Name:  RelTime.cpp
//
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID RelTimeDescriptorInterface::myID = UUID("");
const char *RelTimeDescriptorInterface::myName =
                         "RelTime Description Interface";

const UUID RelTimeDescriptor::myID = UUID("");
const char *RelTimeDescriptor::myName = "RelTime Descriptor";

const UUID RelTimeDescriptor::valID = UUID("");
const char *RelTimeDescriptor::valName = "RelTime";

const UUID RelTimeDescriptorInterfaceABC::myID = UUID();

RelTimeDescriptorInterfaceABC::RelTimeDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
RelTimeDescriptorInterface::RelTimeDescriptorInterface(RelTimeDescriptor *aRelTime):
m_RelTimeDescriptor(aRelTime)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RelTimeDescriptorInterface::~RelTimeDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& RelTimeDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelTimeDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
RelTime RelTimeDescriptorInterface::GetDescriptionRelTime(void)
{
	return m_RelTimeDescriptor->GetDescriptionRelTime();
}

//----------------------------------------------------------------------------
unsigned long RelTimeDescriptorInterface::SetDescriptionRelTime(RelTime duration)	
{
	return m_RelTimeDescriptor->SetDescriptionRelTime(duration);
}
//----------------------------------------------------------------------------
unsigned long RelTimeDescriptorInterface::SetRelTimeElement(char index,
							      int value)
{
	return m_RelTimeDescriptor->SetRelTimeElement(index, value);
}
//----------------------------------------------------------------------------
int RelTimeDescriptorInterface::GetRelTimeElement(char index)
{
	return m_RelTimeDescriptor->GetRelTimeElement(index);
}
//----------------------------------------------------------------------------
unsigned long RelTimeDescriptorInterface::SetAttribute(char index,
							      int value)
{
	return m_RelTimeDescriptor->SetAttribute(index, value);
}
//----------------------------------------------------------------------------
int RelTimeDescriptorInterface::GetAttribute(char index)
{
	return m_RelTimeDescriptor->GetAttribute(index);
}
//----------------------------------------------------------------------------
unsigned long RelTimeDescriptorInterface::SetTimeBaseInterface(
					     TimePointDescriptorInterfaceABC *TimeBase)
{
	return m_RelTimeDescriptor->SetTimeBaseInterface(TimeBase);
}
//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC *RelTimeDescriptorInterface::GetTimeBaseInterface(void)
{
	return m_RelTimeDescriptor->GetTimeBaseInterface();
}
//=============================================================================
RelTimeDescriptor::RelTimeDescriptor():
m_Interface(this),
m_DescriptionRelTime(),
m_TimeBaseInterface(0)
{
	SetExposedInterface(&m_Interface);
	m_attributesRelTime.TZD=0;
	m_attributesRelTime.nnFraction=0;
	m_DescriptionRelTime.negative=0;	
	m_DescriptionRelTime.No_D=-1;
	m_DescriptionRelTime.h=-1;
	m_DescriptionRelTime.m=-1;
	m_DescriptionRelTime.s=-1;
	m_DescriptionRelTime.nn=-1;
}

//----------------------------------------------------------------------------
RelTimeDescriptor::~RelTimeDescriptor()
{
/*	if (m_RelTime) delete [] m_RelTime;*/
}

//----------------------------------------------------------------------------
const UUID& RelTimeDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelTimeDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RelTimeDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* RelTimeDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
RelTime RelTimeDescriptor::GetDescriptionRelTime(void)
{
	return m_DescriptionRelTime;
}

//----------------------------------------------------------------------------
unsigned long RelTimeDescriptor::SetDescriptionRelTime(RelTime rel_time)
{
        m_DescriptionRelTime = rel_time;

	return 0;
}


//----------------------------------------------------------------------------
unsigned long RelTimeDescriptor::SetRelTimeElement(char index,
						     int value)
{
  int esc=0;

  switch (index) {
    case'N':
      m_DescriptionRelTime.negative=value;
    break;
    case'D':
      if(value>0)
	m_DescriptionRelTime.No_D=value;
      else esc=1;
   break;
    case'h':
      if(value>=0 && value<=24)
	m_DescriptionRelTime.h=value;
      else esc=1;
   break;
    case'm':
      if(value>=0 && value<=59)
	m_DescriptionRelTime.m=value;
      else esc=1;
    break;
    case's':
      if(value>=0 && value<=59)
	m_DescriptionRelTime.s=value;
      else esc=1;
    break;
    case'n':
      if(value>=0 && value<*m_attributesRelTime.nnFraction)
	m_DescriptionRelTime.nn=value;
      else esc=1;
    break;

  default:
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  }
  if (esc)
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  else
    return 0;
}

//----------------------------------------------------------------------------
int RelTimeDescriptor::GetRelTimeElement(char index)
{
  switch (index) {
    case'N':
      return m_DescriptionRelTime.negative;
    break;
    case'D':
      return m_DescriptionRelTime.No_D;
    break;
    case'h':
      return m_DescriptionRelTime.h;
    break;
    case'm':
      return m_DescriptionRelTime.m;
    break;
    case's':
      return m_DescriptionRelTime.s;
    break;
    case'n':
      return m_DescriptionRelTime.nn;
    break;

  default:
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  }
}

//----------------------------------------------------------------------------
unsigned long RelTimeDescriptor::SetAttribute(char index,
						     int value)
{
  int esc=0;

  switch (index) {

    case'T':
      if(!m_attributesRelTime.TZD)
	m_attributesRelTime.TZD= new int;
      if(value>=-1200 && value<=1300)
	*m_attributesRelTime.TZD=value;
      else esc=1;
    break;
    case'n':
      if(!m_attributesRelTime.nnFraction)
	m_attributesRelTime.nnFraction= new int;
      if(value>=0)
	*m_attributesRelTime.nnFraction=value;
      else esc=1;
    break;
  default:
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  }
  if (esc)
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  else
    return 0;
}

//----------------------------------------------------------------------------
int RelTimeDescriptor::GetAttribute(char index)
{
  switch (index) {

    case'T':
      if(!m_attributesRelTime.TZD)
	return -1;
      return *m_attributesRelTime.TZD;
    break;
    case'n':
      if(!m_attributesRelTime.nnFraction)
	return -1;
      return *m_attributesRelTime.nnFraction;
    break;
  default:
#ifndef STH
		return -1;
#else
		return (unsigned long)-1;
#endif
  }
}
//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC *RelTimeDescriptor::GetTimeBaseInterface(void)
{ return m_TimeBaseInterface; }
//----------------------------------------------------------------------------
unsigned long RelTimeDescriptor::SetTimeBaseInterface(TimePointDescriptorInterfaceABC* timebase)
{
  m_TimeBaseInterface=timebase;
  return 0;
}
//----------------------------------------------------------------------------
RelTimeDescriptorInterfaceABC *RelTimeDescriptor::GetInterface(void)
{ return &m_Interface; }



