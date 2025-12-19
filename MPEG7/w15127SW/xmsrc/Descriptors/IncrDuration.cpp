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
// Applicable File Name:  IncrDuration.cpp
//
#include "Descriptors/Descriptors.h"
using namespace XM;

const UUID IncrDurationDescriptorInterface::myID = UUID("");
const char *IncrDurationDescriptorInterface::myName =
                         "IncrDuration Description Interface";

const UUID IncrDurationDescriptor::myID = UUID("");
const char *IncrDurationDescriptor::myName = "IncrDuration Descriptor";

const UUID IncrDurationDescriptor::valID = UUID("");
const char *IncrDurationDescriptor::valName = "IncrDuration";

const UUID IncrDurationDescriptorInterfaceABC::myID = UUID();

IncrDurationDescriptorInterfaceABC::IncrDurationDescriptorInterfaceABC()
{ SetInterfaceID(myID);}

//=============================================================================
IncrDurationDescriptorInterface::IncrDurationDescriptorInterface(IncrDurationDescriptor *aIncrDuration):
m_IncrDurationDescriptor(aIncrDuration)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
IncrDurationDescriptorInterface::~IncrDurationDescriptorInterface()
{
}
	
//----------------------------------------------------------------------------
const UUID& IncrDurationDescriptorInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IncrDurationDescriptorInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
long IncrDurationDescriptorInterface::GetIncrDuration(void)
{
	return m_IncrDurationDescriptor->GetIncrDuration();
}

//----------------------------------------------------------------------------
unsigned long IncrDurationDescriptorInterface::SetIncrDuration(long duration)	
{
	return m_IncrDurationDescriptor->SetIncrDuration(duration);
}
//----------------------------------------------------------------------------
unsigned long IncrDurationDescriptorInterface::SetTimeUnitInterface(TimeUnitAttrGroupInterfaceABC *timeunit)
{
	return m_IncrDurationDescriptor->SetTimeUnitInterface(timeunit);
}
//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC * IncrDurationDescriptorInterface::GetTimeUnitInterface(void)
{
	return m_IncrDurationDescriptor->GetTimeUnitInterface();
}

//=============================================================================
IncrDurationDescriptor::IncrDurationDescriptor():
m_Interface(this),
m_IncrDuration(0),
m_TimeUnitInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
IncrDurationDescriptor::~IncrDurationDescriptor()
{
/*	if (m_IncrDuration) delete [] m_IncrDuration;*/
}

//----------------------------------------------------------------------------
const UUID& IncrDurationDescriptor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *IncrDurationDescriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
const UUID& IncrDurationDescriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char *IncrDurationDescriptor::GetValueName(void)
{ return valName; }


//----------------------------------------------------------------------------
long IncrDurationDescriptor::GetIncrDuration(void)
{
	return m_IncrDuration;
}

//----------------------------------------------------------------------------
unsigned long IncrDurationDescriptor::SetIncrDuration(long duration)
{
        m_IncrDuration = duration;

	return 0;
}

//----------------------------------------------------------------------------
unsigned long IncrDurationDescriptor::SetTimeUnitInterface(TimeUnitAttrGroupInterfaceABC *timeunit)
{
       m_TimeUnitInterface=timeunit;
       return 0;
}

//----------------------------------------------------------------------------
TimeUnitAttrGroupInterfaceABC *IncrDurationDescriptor::GetTimeUnitInterface(void)
{
       return m_TimeUnitInterface;
}

//----------------------------------------------------------------------------
IncrDurationDescriptorInterfaceABC *IncrDurationDescriptor::GetInterface(void)
{ return &m_Interface; }



