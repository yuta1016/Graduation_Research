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
// Applicable File Name:  TimeUnit.cpp
//
#include "Descriptors/Descriptors.h"
#include "TimeUnit.h"

using namespace XM;

const UUID TimeUnitAttrGroupInterface::myID = UUID("");
const char *TimeUnitAttrGroupInterface::myName =
                         "TimeUnit Description Interface";

const UUID TimeUnitAttrGroup::myID = UUID("");
const char *TimeUnitAttrGroup::myName = "TimeUnit Descriptor";

const UUID TimeUnitAttrGroup::valID = UUID("");
const char *TimeUnitAttrGroup::valName = "TimeUnit";

const UUID TimeUnitAttrGroupInterfaceABC::myID = UUID();

TimeUnitAttrGroupInterfaceABC::TimeUnitAttrGroupInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
TimeUnitAttrGroupInterface::TimeUnitAttrGroupInterface(TimeUnitAttrGroup *aTimeUnit):
m_TimeUnitAttrGroup(aTimeUnit)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
TimeUnitAttrGroupInterface::~TimeUnitAttrGroupInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& TimeUnitAttrGroupInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeUnitAttrGroupInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
TimeUnit TimeUnitAttrGroupInterface::GetDescriptionTimeUnit(void)
{
	return m_TimeUnitAttrGroup->GetDescriptionTimeUnit();
}

//----------------------------------------------------------------------------
unsigned long TimeUnitAttrGroupInterface::SetDescriptionTimeUnit(TimeUnit timeunit)	
{
	return m_TimeUnitAttrGroup->SetDescriptionTimeUnit(timeunit);
}
//----------------------------------------------------------------------------
unsigned long TimeUnitAttrGroupInterface::SetTimeUnitElement(char index,
							      int value)
{
	return m_TimeUnitAttrGroup->SetTimeUnitElement(index, value);
}
//----------------------------------------------------------------------------
int TimeUnitAttrGroupInterface::GetTimeUnitElement(char index)
{
	return m_TimeUnitAttrGroup->GetTimeUnitElement(index);
}

//=============================================================================
TimeUnitAttrGroup::TimeUnitAttrGroup():
m_Interface(this),
m_isProprietary(false),
m_DescriptionTimeUnit()
{
	SetExposedInterface(&m_Interface);
	m_DescriptionTimeUnit.TZD=-1;
	m_DescriptionTimeUnit.nnFraction=-1;
	m_DescriptionTimeUnit.negative=0;	
	m_DescriptionTimeUnit.No_D=-1;
	m_DescriptionTimeUnit.h=-1;
	m_DescriptionTimeUnit.m=-1;
	m_DescriptionTimeUnit.s=-1;
	m_DescriptionTimeUnit.nn=-1;
}

//----------------------------------------------------------------------------
TimeUnitAttrGroup::~TimeUnitAttrGroup()
{
/*	if (m_TimeUnit) delete [] m_TimeUnit;*/
}

//----------------------------------------------------------------------------
const UUID& TimeUnitAttrGroup::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *TimeUnitAttrGroup::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& TimeUnitAttrGroup::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* TimeUnitAttrGroup::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
bool TimeUnitAttrGroup::IsProprietary(void)
{ return m_isProprietary; }


//----------------------------------------------------------------------------
TimeUnit TimeUnitAttrGroup::GetDescriptionTimeUnit(void)
{
	return m_DescriptionTimeUnit;
}

//----------------------------------------------------------------------------
unsigned long TimeUnitAttrGroup::SetDescriptionTimeUnit(TimeUnit timeunit)
{
        m_DescriptionTimeUnit = timeunit;

	return 0;
}


//----------------------------------------------------------------------------
unsigned long TimeUnitAttrGroup::SetTimeUnitElement(char index,
						     int value)
{
  int esc=0;

  switch (index) {
    case'N':
      m_DescriptionTimeUnit.negative=value;
    break;
    case'D':
      if(value>0)
	m_DescriptionTimeUnit.No_D=value;
      else esc=1;
   break;
    case'h':
      if(value>=0 && value<=24)
	m_DescriptionTimeUnit.h=value;
      else esc=1;
   break;
    case'm':
      if(value>=0 && value<=59)
	m_DescriptionTimeUnit.m=value;
      else esc=1;
    break;
    case's':
      if(value>=0 && value<=59)
	m_DescriptionTimeUnit.s=value;
      else esc=1;
    break;
    case'n':
      if(value>=0 && value<m_DescriptionTimeUnit.nnFraction)
	m_DescriptionTimeUnit.nn=value;
      else esc=1;
    break;
    case'T':
      if(value>=-1200 && value<=1300)
	m_DescriptionTimeUnit.TZD=value;
      else esc=1;
    break;
    case'F':
        if(value>=0)
	  m_DescriptionTimeUnit.nnFraction=value;
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
int TimeUnitAttrGroup::GetTimeUnitElement(char index)
{
  switch (index) {
    case'N':
      return m_DescriptionTimeUnit.negative;
    break;
    case'D':
      return m_DescriptionTimeUnit.No_D;
    break;
    case'h':
      return m_DescriptionTimeUnit.h;
    break;
    case'm':
      return m_DescriptionTimeUnit.m;
    break;
    case's':
      return m_DescriptionTimeUnit.s;
    break;
    case'n':
      return m_DescriptionTimeUnit.nn;
    break;
    case'T':
      return m_DescriptionTimeUnit.TZD;
    break;
    case'F':
      return m_DescriptionTimeUnit.nnFraction;
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
TimeUnitAttrGroupInterfaceABC *TimeUnitAttrGroup::GetInterface(void)
{ return &m_Interface; }



