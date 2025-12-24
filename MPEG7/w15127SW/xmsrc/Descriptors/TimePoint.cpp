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
// Applicable File Name:  TimePoint.cpp
//
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID TimePointDescriptorInterface::myID = UUID("");
const char *TimePointDescriptorInterface::myName =
                         "TimePoint Description Interface";

const UUID TimePointDescriptor::myID = UUID("");
const char *TimePointDescriptor::myName = "TimePoint Descriptor";

const UUID TimePointDescriptor::valID = UUID("");
const char *TimePointDescriptor::valName = "TimePoint";

const UUID TimePointDescriptorInterfaceABC::myID = UUID();

TimePointDescriptorInterfaceABC::TimePointDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TimePointDescriptorInterface::TimePointDescriptorInterface(TimePointDescriptor *aTimePoint):
m_TimePointDescriptor(aTimePoint)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimePointDescriptorInterface::~TimePointDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& TimePointDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimePointDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TimePointD TimePointDescriptorInterface::GetDescriptionTimePoint(void)
{
	return m_TimePointDescriptor->GetDescriptionTimePoint();
}

//----------------------------------------------------------------------------
unsigned long TimePointDescriptorInterface::SetDescriptionTimePoint(TimePointD timepoint)	
{
	return m_TimePointDescriptor->SetDescriptionTimePoint(timepoint);
}
//----------------------------------------------------------------------------
unsigned long TimePointDescriptorInterface::SetTimePointElement(char index,
							      int value)
{
	return m_TimePointDescriptor->SetTimePointElement(index, value);
}
//----------------------------------------------------------------------------
int TimePointDescriptorInterface::GetTimePointElement(char index)
{
	return m_TimePointDescriptor->GetTimePointElement(index);
}
//----------------------------------------------------------------------------
unsigned long TimePointDescriptorInterface::SetAttribute(char index,
							      int value)
{
	return m_TimePointDescriptor->SetAttribute(index, value);
}
//----------------------------------------------------------------------------
int TimePointDescriptorInterface::GetAttribute(char index)
{
	return m_TimePointDescriptor->GetAttribute(index);
}
//=============================================================================
TimePointDescriptor::TimePointDescriptor():
m_Interface(this),
m_isProprietary(false),
m_DescriptionTimePoint()
{
	SetExposedInterface(&m_Interface);
	m_attributesTimePoint.TZD=0;
	m_attributesTimePoint.nnFraction=0;
	m_DescriptionTimePoint.Y=0;
	m_DescriptionTimePoint.M=-1;
	m_DescriptionTimePoint.D=-1;
	m_DescriptionTimePoint.h=-1;
	m_DescriptionTimePoint.m=-1;
	m_DescriptionTimePoint.s=-1;
	m_DescriptionTimePoint.nn=-1;
}

//----------------------------------------------------------------------------
TimePointDescriptor::~TimePointDescriptor()
{
/*	if (m_TimePoint) delete [] m_TimePoint;*/
}

//----------------------------------------------------------------------------
const UUID& TimePointDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimePointDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimePointDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimePointDescriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimePointDescriptor::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
TimePointD TimePointDescriptor::GetDescriptionTimePoint(void)
{
	return m_DescriptionTimePoint;
}

//----------------------------------------------------------------------------
unsigned long TimePointDescriptor::SetDescriptionTimePoint(TimePointD timepoint)
{
        m_DescriptionTimePoint = timepoint;

	return 0;
}


//----------------------------------------------------------------------------
unsigned long TimePointDescriptor::SetTimePointElement(char index,
						     int value)
{
  int esc=0;

  switch (index) {
    case'Y':
      m_DescriptionTimePoint.Y=value;
    break;
    case'M':
      if(value>0 && value<=12)
	m_DescriptionTimePoint.M=value;
      else esc=1;
    break;
    case'D':
      if(value>0 && value<=31)
	m_DescriptionTimePoint.D=value;
      else esc=1;
   break;
    case'h':
      if(value>=0 && value<=24)
	m_DescriptionTimePoint.h=value;
      else esc=1;
   break;
    case'm':
      if(value>=0 && value<=59)
	m_DescriptionTimePoint.m=value;
      else esc=1;
    break;
    case's':
      if(value>=0 && value<=59)
	m_DescriptionTimePoint.s=value;
      else esc=1;
    break;
    case'n':
      if(value>=0 && value<*m_attributesTimePoint.nnFraction)
	m_DescriptionTimePoint.nn=value;
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
int TimePointDescriptor::GetTimePointElement(char index)
{
  switch (index) {
    case'Y':
      return m_DescriptionTimePoint.Y;
    break;
    case'M':
      return m_DescriptionTimePoint.M;
    break;
    case'D':
      return m_DescriptionTimePoint.D;
    break;
    case'h':
      return m_DescriptionTimePoint.h;
    break;
    case'm':
      return m_DescriptionTimePoint.m;
    break;
    case's':
      return m_DescriptionTimePoint.s;
    break;
    case'n':
      return m_DescriptionTimePoint.nn;
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
unsigned long TimePointDescriptor::SetAttribute(char index,
						     int value)
{
  int esc=0;

  switch (index) {

    case'T':
      if(!m_attributesTimePoint.TZD)
	m_attributesTimePoint.TZD= new int;
      if(value>=-1200 && value<=1300)
	*m_attributesTimePoint.TZD=value;
      else esc=1;
    break;
    case'n':
      if(!m_attributesTimePoint.nnFraction)
	m_attributesTimePoint.nnFraction= new int;
      if(value>=0)
	*m_attributesTimePoint.nnFraction=value;
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
int TimePointDescriptor::GetAttribute(char index)
{
  switch (index) {

    case'T':
      if(!m_attributesTimePoint.TZD)
	return -1;
      return *m_attributesTimePoint.TZD;
    break;
    case'n':
      if(!m_attributesTimePoint.nnFraction)
	return -1;
      return *m_attributesTimePoint.nnFraction;
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
TimePointDescriptorInterfaceABC *TimePointDescriptor::GetInterface(void)
{ return &m_Interface; }



