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
// Applicable File Name:  RelIncrTime.cpp
//
#include "Descriptors/Descriptors.h"

using namespace XM;

const UUID RelIncrTimeDescriptorInterface::myID = UUID("");
const char *RelIncrTimeDescriptorInterface::myName =
                         "RelIncrTime Description Interface";

const UUID RelIncrTimeDescriptor::myID = UUID("");
const char *RelIncrTimeDescriptor::myName = "RelIncrTime Descriptor";

const UUID RelIncrTimeDescriptor::valID = UUID("");
const char *RelIncrTimeDescriptor::valName = "RelIncrTime";

const UUID RelIncrTimeDescriptorInterfaceABC::myID = UUID();

RelIncrTimeDescriptorInterfaceABC::RelIncrTimeDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
RelIncrTimeDescriptorInterface::RelIncrTimeDescriptorInterface(RelIncrTimeDescriptor *aRelIncrTime):
m_RelIncrTimeDescriptor(aRelIncrTime)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
RelIncrTimeDescriptorInterface::~RelIncrTimeDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& RelIncrTimeDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelIncrTimeDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
long RelIncrTimeDescriptorInterface::GetRelIncrTime(void)
{
	return m_RelIncrTimeDescriptor->GetRelIncrTime();
}

//----------------------------------------------------------------------------
unsigned long RelIncrTimeDescriptorInterface::SetRelIncrTime(long rel_incr_time)	
{
	return m_RelIncrTimeDescriptor->SetRelIncrTime(rel_incr_time);
}
//----------------------------------------------------------------------------
unsigned long RelIncrTimeDescriptorInterface::SetTimeUnitInterface(TimeUnitAttrGroupInterfaceABC *time_unit)
{
	return m_RelIncrTimeDescriptor->SetTimeUnitInterface(time_unit);
}
//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC * RelIncrTimeDescriptorInterface::GetTimeUnitInterface(void)
{
	return m_RelIncrTimeDescriptor->GetTimeUnitInterface();
}
//----------------------------------------------------------------------------
unsigned long RelIncrTimeDescriptorInterface::SetTimeBaseInterface(TimePointDescriptorInterfaceABC *time_base)
{
	return m_RelIncrTimeDescriptor->SetTimeBaseInterface(time_base);
}
//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC * RelIncrTimeDescriptorInterface::GetTimeBaseInterface(void)
{
	return m_RelIncrTimeDescriptor->GetTimeBaseInterface();
}
//=============================================================================
RelIncrTimeDescriptor::RelIncrTimeDescriptor():
m_Interface(this),
m_TimeUnitInterface(0),
m_TimeBaseInterface(0),
m_RelIncrTime(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
RelIncrTimeDescriptor::~RelIncrTimeDescriptor()
{
/*	if (m_RelIncrTime) delete [] m_RelIncrTime;*/
}

//----------------------------------------------------------------------------
const UUID& RelIncrTimeDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *RelIncrTimeDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& RelIncrTimeDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char *RelIncrTimeDescriptor::GetValueName(void)
{ return valName; }


//----------------------------------------------------------------------------
long RelIncrTimeDescriptor::GetRelIncrTime(void)
{
	return m_RelIncrTime;
}

//----------------------------------------------------------------------------
bunsigned long RelIncrTimeDescriptor::SetRelIncrTime(long offset)
{
        m_RelIncrTime = offset;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long RelIncrTimeDescriptor::SetTimeUnitInterface(TimeUnitAttrGroupInterfaceABC *timeunit)
{
       m_TimeUnitInterface=timeunit;
       return 0;
}

//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC *RelIncrTimeDescriptor::GetTimeUnitInterface(void)
{
       return m_TimeUnitInterface;
}
//----------------------------------------------------------------------------
unsigned long RelIncrTimeDescriptor::SetTimeBaseInterface(TimePointDescriptorInterfaceABC *time_base)
{
        m_TimeBaseInterface=time_base;
	return 0;
}
//----------------------------------------------------------------------------
TimePointDescriptorInterfaceABC * RelIncrTimeDescriptor::GetTimeBaseInterface(void)
{
	return m_TimeBaseInterface;
}
//----------------------------------------------------------------------------
RelIncrTimeDescriptorInterfaceABC *RelIncrTimeDescriptor::GetInterface(void)
{ return &m_Interface; }



