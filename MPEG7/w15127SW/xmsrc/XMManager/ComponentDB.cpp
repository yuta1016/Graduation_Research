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
// ComponentDB.cp



#include "XMManager/ComponentDB.h"
using namespace XM;

#include <stdlib.h>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Implementation of ComponentDatabase

ComponentDatabase::ComponentDatabase()
{
}

//----------------------------------------------------------------------------
ComponentDatabase::~ComponentDatabase()
{
        int i;

	for (i=0; i < m_Ds.size(); i++)
		delete m_Ds[i];

	for (i=0; i < m_DSs.size(); i++)
		delete m_DSs[i];

	for (i=0; i < m_DDLs.size(); i++)
		delete m_DDLs[i];
	
	for (i=0; i < m_CSs.size(); i++)
		delete m_CSs[i];
	
	for (i=0; i < m_Systems.size(); i++)
		delete m_Systems[i];
	
	for (i=0; i < m_DescriptorExtractors.size(); i++)
		delete m_DescriptorExtractors[i];
	
	for (i=0; i < m_Encoders.size(); i++)
		delete m_Encoders[i];
	
	for (i=0; i < m_Decoders.size(); i++)
		delete m_Decoders[i];
	
	for (i=0; i < m_Parsers.size(); i++)
		delete m_Parsers[i];
	
	for (i=0; i < m_ClientApplications.size(); i++)
		delete m_ClientApplications[i];
	
	for (i=0; i < m_ServerApplications.size(); i++)
		delete m_ServerApplications[i];
	
	for (i=0; i < m_SearchUtilities.size(); i++)
		delete m_SearchUtilities[i];

	for (i=0; i < m_EvalTools.size(); i++)
		delete m_EvalTools[i];
}




const std::vector<DescriptorABC *>& ComponentDatabase::GetDs() const 
{ return m_Ds; }

const std::vector<DescriptionSchemeABC *>& ComponentDatabase::GetDSs() const 
{ return m_DSs; }

const std::vector<DescriptionCodingEngineABC *>& ComponentDatabase::GetCSs() const
{ return m_CSs; }

const std::vector<DDLABC *>& ComponentDatabase::GetDDLs() const 
{ return m_DDLs; }

const std::vector<SystemABC *>& ComponentDatabase::GetSystems() const 
{ return m_Systems; }

const std::vector<DescriptorExtractorABC *>& ComponentDatabase::GetExtractors() const 
{ return m_DescriptorExtractors; }

const std::vector<EncoderABC *> ComponentDatabase::GetEncoders() const 
{ return m_Encoders; }

const std::vector<DecoderABC *> ComponentDatabase::GetDecoders() const
{ return m_Decoders; }

const std::vector<SearchABC *> ComponentDatabase::GetSearchUtilities() const
{ return m_SearchUtilities; }

const std::vector<ParserABC *> ComponentDatabase::GetParsers() const
{ return m_Parsers; }

const std::vector<ClientApplicationABC *> ComponentDatabase::GetClientApps() const
{ return m_ClientApplications; }

const std::vector<ServerApplicationABC *> ComponentDatabase::GetServerApps() const
{ return m_ServerApplications; }

const std::vector<EvaluationToolABC *> ComponentDatabase::GetEvalTools() const
{ return m_EvalTools; }
//----------------------------------------------------------------------------
DescriptorABC *ComponentDatabase::CreateD(
	const char *aName,
	const UUID* aUUID,
	int NameOrID)
{
	std::vector<DescriptorABC *>::iterator it = m_Ds.begin();
		
	if (NameOrID == 0)
	{
		while (it != m_Ds.end())
		{
			if (strcmp((*it)->GetName(),aName) == 0)
				return *it;
			
			it++;
		}
	}
	else
	{
		while (it != m_Ds.end())
		{
			if ((*it)->GetObjectID() == *aUUID)
				return *it;

			it++;
		}
	}
 
  return 0;
}



//----------------------------------------------------------------------------
#define CREATEOBJECT(Function, Type, Storage)\
Type *ComponentDatabase::Function(\
	const char *aName,\
	const UUID* aUUID,\
	int NameOrID)\
{\
	std::vector<Type *>::iterator it = Storage.begin();\
		\
	if (NameOrID == 0)\
	{\
		while (it != Storage.end())\
		{\
			if (strcmp((*it)->GetName(), aName) == 0)\
				return *it;\
			\
			it++;\
		}\
	}\
	else\
	{\
		while (it != Storage.end())\
		{\
			if ((*it)->GetObjectID() == *aUUID)\
				return *it;\
			\
			it++;\
		}\
	}\
\
  return 0;\
}

//----------------------------------------------------------------------------
CREATEOBJECT(CreateDS, DescriptionSchemeABC, m_DSs);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateDDL, DDLABC, m_DDLs);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateCS, DescriptionCodingEngineABC, m_CSs);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateSystem, SystemABC, m_Systems);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateExtractor, DescriptorExtractorABC, m_DescriptorExtractors);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateEncoder, EncoderABC, m_Encoders);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateDecoder, DecoderABC, m_Decoders);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateParser, ParserABC, m_Parsers);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateClient, ClientApplicationABC, m_ClientApplications);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateServer, ServerApplicationABC, m_ServerApplications);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateSearch, SearchABC, m_SearchUtilities);
//----------------------------------------------------------------------------
CREATEOBJECT(CreateEvalTool, EvaluationToolABC, m_EvalTools);
//----------------------------------------------------------------------------
void ComponentDatabase::InitDB(void)
{
	int i;

	for (i=0; i < m_Ds.size(); i++)
		delete m_Ds[i];

	for (i=0; i < m_DSs.size(); i++)
		delete m_DSs[i];

	for (i=0; i < m_DDLs.size(); i++)
		delete m_DDLs[i];
	
	for (i=0; i < m_CSs.size(); i++)
		delete m_CSs[i];
	
	for (i=0; i < m_Systems.size(); i++)
		delete m_Systems[i];
	
	for (i=0; i < m_DescriptorExtractors.size(); i++)
		delete m_DescriptorExtractors[i];
	
	for (i=0; i < m_Encoders.size(); i++)
		delete m_Encoders[i];
	
	for (i=0; i < m_Decoders.size(); i++)
		delete m_Decoders[i];
	
	for (i=0; i < m_Parsers.size(); i++)
		delete m_Parsers[i];
	
	for (i=0; i < m_ClientApplications.size(); i++)
		delete m_ClientApplications[i];
	
	for (i=0; i < m_ServerApplications.size(); i++)
		delete m_ServerApplications[i];
	
	for (i=0; i < m_SearchUtilities.size(); i++)
		delete m_SearchUtilities[i];

	for (i=0; i < m_EvalTools.size(); i++)
		delete m_EvalTools[i];

	m_Ds.clear();
	m_DSs.clear();
	m_DDLs.clear();
	m_CSs.clear();
	m_Systems.clear();
	m_DescriptorExtractors.clear();
	m_Encoders.clear();
	m_Decoders.clear();
	m_Parsers.clear();
	m_ClientApplications.clear();
	m_ServerApplications.clear();
	m_SearchUtilities.clear();
	m_EvalTools.clear();

	// To fully implement this class factory, we need to construct all objects 
	// which are to be evaluated.
	InitDs();
	InitDSs();
	InitDDLs();
	InitSystems();
	InitCSs();
	InitExtractors();
	InitEncoders();
	InitDecoders();
	InitParsers();
	InitClients();
	InitServers();
	InitSearch();
	InitETs();
}

