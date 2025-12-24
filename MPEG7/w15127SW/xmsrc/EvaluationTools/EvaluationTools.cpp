///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
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
// Applicable File Name:  EvaluationTools.cpp
//

#include "EvaluationTools.h"

using namespace XM;

const UUID P564CodedSizeInterface::myID = UUID("eeb94521-db33-11d2-affe-0080c7e1e76d");
const char *P564CodedSizeInterface::myName = "D/DS Size EvaluationTool Interface";

const UUID SystemFrontEndProxy::myID = UUID("8e7cd740-dbdc-11d2-affe-0080c7e1e76d");
const char *SystemFrontEndProxy::myName = "Evaluation proxy system front end";

const UUID P564CodedSize::myID = UUID("eeb94520-db33-11d2-affe-0080c7e1e76d");
const char *P564CodedSize::myName = "D/DS Size EvaluationTool";

const UUID P564CodedSizeInterfaceABC::myID = UUID();

//=============================================================================
P564CodedSizeInterfaceABC::P564CodedSizeInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SystemFrontEndProxy::SystemFrontEndProxy(P564CodedSize* aCodedSizeEvaluator):
m_CodedSize(aCodedSizeEvaluator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SystemFrontEndProxy::~SystemFrontEndProxy()
{
}

//----------------------------------------------------------------------------
void SystemFrontEndProxy::SetInputStream(XMStream& inStream)
{
	m_CodedSize->SetInputStream(inStream);
}

//----------------------------------------------------------------------------
unsigned long SystemFrontEndProxy::PushBuffer(std::vector<unsigned char>& aBuff)
{
	return m_CodedSize->PushBuffer(aBuff);
}

//----------------------------------------------------------------------------
const UUID& SystemFrontEndProxy::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SystemFrontEndProxy::GetName(void)
{ return myName; }


//=============================================================================
P564CodedSizeInterface::P564CodedSizeInterface(P564CodedSize* aCodedSizeEvaluator):
m_CodedSize(aCodedSizeEvaluator)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
P564CodedSizeInterface::~P564CodedSizeInterface()
{
}

//----------------------------------------------------------------------------
std::vector<unsigned long> P564CodedSizeInterface::IdentifySizes()
{
	return m_CodedSize->IdentifySizes();
}

//----------------------------------------------------------------------------
const UUID& P564CodedSizeInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *P564CodedSizeInterface::GetName(void)
{ return myName; }


//=============================================================================
P564CodedSize::P564CodedSize():
m_SizeInterface(this),
m_SysFrontInterface(this)
{
	SetExposedInterface(&m_SizeInterface);
	SetExposedInterface(&m_SysFrontInterface);
}

//----------------------------------------------------------------------------
P564CodedSize::~P564CodedSize()
{
}

//----------------------------------------------------------------------------
std::vector<unsigned long>P564CodedSize::IdentifySizes()
{
	return m_bufferSizes;
}

//----------------------------------------------------------------------------
void P564CodedSize::SetInputStream(XMStream& inStream)
{
	return;
}

//----------------------------------------------------------------------------
unsigned long P564CodedSize::PushBuffer(std::vector<unsigned char>& aBuff)
{
	m_bufferSizes.push_back(aBuff.size());
	return 0;
}

//----------------------------------------------------------------------------
const UUID& P564CodedSize::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *P564CodedSize::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
P564CodedSizeInterfaceABC *P564CodedSize::GetSizeInterface(void)
{ return &m_SizeInterface; }

//----------------------------------------------------------------------------
I_SystemFrontEndABC *P564CodedSize::GetSystemFrontEndInterface()
{ 
	return &m_SysFrontInterface; 
}


