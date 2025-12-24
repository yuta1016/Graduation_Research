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
// Applicable File Name:  Duration.cpp
//
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID DurationDescriptorInterface::myID = UUID("");
const char *DurationDescriptorInterface::myName =
                         "Duration Description Interface";

const UUID DurationDescriptor::myID = UUID("");
const char *DurationDescriptor::myName = "Duration Descriptor";

const UUID DurationDescriptor::valID = UUID("");
const char *DurationDescriptor::valName = "Duration";

const UUID DurationDescriptorInterfaceABC::myID = UUID();

DurationDescriptorInterfaceABC::DurationDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
DurationDescriptorInterface::DurationDescriptorInterface(DurationDescriptor *aDuration):
m_DurationDescriptor(aDuration)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
DurationDescriptorInterface::~DurationDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& DurationDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DurationDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
Duration DurationDescriptorInterface::GetDescriptionDuration(void)
{
	return m_DurationDescriptor->GetDescriptionDuration();
}

//----------------------------------------------------------------------------
unsigned long DurationDescriptorInterface::SetDescriptionDuration(Duration duration)	
{
	return m_DurationDescriptor->SetDescriptionDuration(duration);
}
//----------------------------------------------------------------------------
unsigned long DurationDescriptorInterface::SetDurationElement(char index,
							      int value)
{
	return m_DurationDescriptor->SetDurationElement(index, value);
}
//----------------------------------------------------------------------------
int DurationDescriptorInterface::GetDurationElement(char index)
{
	return m_DurationDescriptor->GetDurationElement(index);
}
//----------------------------------------------------------------------------
unsigned long DurationDescriptorInterface::SetAttribute(char index,
							      int value)
{
	return m_DurationDescriptor->SetAttribute(index, value);
}
//----------------------------------------------------------------------------
int DurationDescriptorInterface::GetAttribute(char index)
{
	return m_DurationDescriptor->GetAttribute(index);
}
//=============================================================================
DurationDescriptor::DurationDescriptor():
m_Interface(this),
m_isProprietary(false),
m_DescriptionDuration()
{
	SetExposedInterface(&m_Interface);
	m_attributesDuration.TZD=0;
	m_attributesDuration.nnFraction=0;
	m_DescriptionDuration.negative=0;	
	m_DescriptionDuration.No_D=-1;
	m_DescriptionDuration.h=-1;
	m_DescriptionDuration.m=-1;
	m_DescriptionDuration.s=-1;
	m_DescriptionDuration.nn=-1;
}

//----------------------------------------------------------------------------
DurationDescriptor::~DurationDescriptor()
{
/*	if (m_Duration) delete [] m_Duration;*/
}

//----------------------------------------------------------------------------
const UUID& DurationDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DurationDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& DurationDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* DurationDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool DurationDescriptor::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
Duration DurationDescriptor::GetDescriptionDuration(void)
{
	return m_DescriptionDuration;
}

//----------------------------------------------------------------------------
unsigned long DurationDescriptor::SetDescriptionDuration(Duration duration)
{
        m_DescriptionDuration = duration;

	return 0;
}


//----------------------------------------------------------------------------
unsigned long DurationDescriptor::SetDurationElement(char index,
						     int value)
{
  int esc=0;

  switch (index) {
    case'N':
      m_DescriptionDuration.negative=value;
    break;
    case'D':
      if(value>0)
	m_DescriptionDuration.No_D=value;
      else esc=1;
   break;
    case'h':
      if(value>=0 && value<=24)
	m_DescriptionDuration.h=value;
      else esc=1;
   break;
    case'm':
      if(value>=0 && value<=59)
	m_DescriptionDuration.m=value;
      else esc=1;
    break;
    case's':
      if(value>=0 && value<=59)
	m_DescriptionDuration.s=value;
      else esc=1;
    break;
    case'n':
      if(value>=0 && value<*m_attributesDuration.nnFraction)
	m_DescriptionDuration.nn=value;
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
int DurationDescriptor::GetDurationElement(char index)
{
  switch (index) {
    case'N':
      return m_DescriptionDuration.negative;
    break;
    case'D':
      return m_DescriptionDuration.No_D;
    break;
    case'h':
      return m_DescriptionDuration.h;
    break;
    case'm':
      return m_DescriptionDuration.m;
    break;
    case's':
      return m_DescriptionDuration.s;
    break;
    case'n':
      return m_DescriptionDuration.nn;
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
unsigned long DurationDescriptor::SetAttribute(char index,
						     int value)
{
  int esc=0;

  switch (index) {

    case'T':
      if(!m_attributesDuration.TZD)
	m_attributesDuration.TZD= new int;
      if(value>=-1200 && value<=1300)
	*m_attributesDuration.TZD=value;
      else esc=1;
    break;
    case'n':
      if(!m_attributesDuration.nnFraction)
	m_attributesDuration.nnFraction= new int;
      if(value>=0)
	*m_attributesDuration.nnFraction=value;
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
int DurationDescriptor::GetAttribute(char index)
{
  switch (index) {

    case'T':
      if(!m_attributesDuration.TZD)
	return -1;
      return *m_attributesDuration.TZD;
    break;
    case'n':
      if(!m_attributesDuration.nnFraction)
	return -1;
      return *m_attributesDuration.nnFraction;
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
DurationDescriptorInterfaceABC *DurationDescriptor::GetInterface(void)
{ return &m_Interface; }



