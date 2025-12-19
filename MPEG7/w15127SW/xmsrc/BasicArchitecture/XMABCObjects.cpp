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
// XMABCObjects.cpp

#include "XMABCObjects.h"

using namespace XM;
///////////////////////////////////////////////////////////////////////////////
unsigned long ConnectibleObject::AttachInterface(I_InterfaceABC * intfc)
{
	std::vector<UUID> intfcids = intfc->GetInterfaceIDs();

	bool found = false;

	for (int i=0; i < m_InputInterfaces.size(); i++)
	{
		if (m_InputInterfaces[i].second == false)
		{
			for (int j=0; j < intfcids.size(); j++)
			{
				UUID temp = m_InputInterfaces[i].first;
				UUID temp2 = intfcids[j];

				if (m_InputInterfaces[i].first == intfcids[j])
				{
					// this interface is indeed supported
					m_InputInterfaces[i].second = true;
					//SetExposedInterface(intfc);
					ReceiveThisInterface(intfc);
					found = true;
					break;
				}
			}	

			if (found == true)
				break;
		}
	}

	if (found)
		return 0;
	else
		return 1;
}

//----------------------------------------------------------------------------
void ConnectibleObject::SetExposedInterface(I_InterfaceABC* intfc)
{
	m_ExposedInterfaces.push_back(intfc);
}

//----------------------------------------------------------------------------
std::vector<I_InterfaceABC*> ConnectibleObject::GetExposedInterfaces(void)
{
	return m_ExposedInterfaces;
}

//----------------------------------------------------------------------------
void ConnectibleObject::SetInputInterfaces(const UUID& aUUID)
{
	std::pair<UUID, bool> myPair;
	myPair.first = aUUID;
	myPair.second = false;

	m_InputInterfaces.push_back(myPair);
}

//----------------------------------------------------------------------------
std::vector<UUID> ConnectibleObject::GetInputInterfaces(void)
{
	std::vector<UUID> retInterfaces;

	for (int i=0; i < m_InputInterfaces.size(); i++)
	{
		retInterfaces.push_back(m_InputInterfaces[i].first);
	}

	return retInterfaces;
}

DDLScriptABC::DDLScriptABC(){ };
DDLScriptABC::~DDLScriptABC(){ };

XMObject::XMObject(){ };
XMObject::~XMObject(){ };

ConnectibleObject::ConnectibleObject(){ };
ConnectibleObject::~ConnectibleObject(){ };

DescriptorABC::DescriptorABC(){ };
DescriptorABC::~DescriptorABC(){ };

DescriptionSchemeABC::DescriptionSchemeABC(){ };
DescriptionSchemeABC::~DescriptionSchemeABC(){ };

SystemABC::SystemABC(){ };
SystemABC::~SystemABC(){ };

DDLABC::DDLABC(){ };
DDLABC::~DDLABC(){ };

DescriptionCodingEngineABC::DescriptionCodingEngineABC(){ };
DescriptionCodingEngineABC::~DescriptionCodingEngineABC(){ };

DescriptorExtractorABC::DescriptorExtractorABC(){ };
DescriptorExtractorABC::~DescriptorExtractorABC(){ };

EncoderABC::EncoderABC(){ };
EncoderABC::~EncoderABC(){ };

ParserABC::ParserABC(){ };
ParserABC::~ParserABC(){ };

DecoderABC::DecoderABC(){ };
DecoderABC::~DecoderABC(){ };

SearchABC::SearchABC(){ };
SearchABC::~SearchABC(){ };

ClientApplicationABC::ClientApplicationABC(){ };
ClientApplicationABC::~ClientApplicationABC(){ };

ServerApplicationABC::ServerApplicationABC(){ };
ServerApplicationABC::~ServerApplicationABC(){ };

EvaluationToolABC::EvaluationToolABC() { };
EvaluationToolABC::~EvaluationToolABC() { };
