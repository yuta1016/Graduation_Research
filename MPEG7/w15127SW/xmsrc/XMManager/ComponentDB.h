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
// ComponentDB.h

#ifndef __COMPONENTDB_H__
#define __COMPONENTDB_H__


//## begin module.includes preserve=yes

#include "BasicArchitecture/XMABCObjects.h"

#include <vector>

namespace XM
{

//## end module.includes preserve=yes
class ComponentDatabase
{
public:
	ComponentDatabase();
	virtual ~ComponentDatabase();

	virtual void InitDB();

	virtual DescriptorABC * CreateD(const char *aName, const UUID* aUUID, int NameOrID);

	virtual DescriptionSchemeABC * CreateDS(const char *aName, const UUID* aUUID, int NameOrID);

	virtual DDLABC * CreateDDL(const char *aName, const UUID* aUUID, int NameOrID);

	virtual SystemABC * CreateSystem(const char *aName, const UUID* aUUID, int NameOrID);

	virtual DescriptionCodingEngineABC *CreateCS(const char *aName, const UUID* aUUID, int NameOrID);

	virtual DescriptorExtractorABC * CreateExtractor(const char *aName, const UUID* aUUID, int NameOrID);

	virtual EncoderABC *CreateEncoder(const char *aName, const UUID* aUUID, int NameOrID);

	virtual ParserABC *CreateParser(const char *aName, const UUID* aUUID, int NameOrID);

	virtual DecoderABC *CreateDecoder(const char *aName, const UUID* aUUID, int NameOrID);

	virtual ClientApplicationABC *CreateClient(const char *aName, const UUID* aUUID, int NameOrID);

	virtual ServerApplicationABC *CreateServer(const char *aName, const UUID* aUUID, int NameOrID);

	virtual SearchABC *CreateSearch(const char *aName, const UUID* aUUID, int NameOrID);

	virtual EvaluationToolABC *CreateEvalTool(const char *aName, const UUID* aUUID, int NameOrID);



	virtual const std::vector<DescriptorABC *>& GetDs() const;
	virtual const std::vector<DescriptionSchemeABC *>& GetDSs() const ;
	virtual const std::vector<DescriptionCodingEngineABC *>& GetCSs() const;
	virtual const std::vector<DDLABC *>& GetDDLs() const ;
	virtual const std::vector<SystemABC *>& GetSystems() const ;
	virtual const std::vector<DescriptorExtractorABC *>& GetExtractors() const ;
	virtual const std::vector<EncoderABC *> GetEncoders() const ;
	virtual const std::vector<DecoderABC *> GetDecoders() const;
	virtual const std::vector<SearchABC *> GetSearchUtilities() const;
	virtual const std::vector<ParserABC *> GetParsers() const;
	virtual const std::vector<ClientApplicationABC *> GetClientApps() const;
	virtual const std::vector<ServerApplicationABC *> GetServerApps() const;
	virtual const std::vector<EvaluationToolABC *> GetEvalTools() const;

protected:
	
	virtual void InitDs();
	virtual void InitDSs();
	virtual void InitDDLs();
	virtual void InitCSs();
	virtual void InitSystems();
	virtual void InitExtractors();
	virtual void InitEncoders();
	virtual void InitDecoders();
	virtual void InitParsers();
	virtual void InitClients();
	virtual void InitServers();
	virtual void InitSearch();
	virtual void InitETs();

private:
	std::vector<DescriptorABC *> m_Ds;

	std::vector<DescriptionSchemeABC *> m_DSs;

	std::vector<DescriptionCodingEngineABC *> m_CSs;

	std::vector<DDLABC *> m_DDLs;

	std::vector<SystemABC *> m_Systems;

	std::vector<DescriptorExtractorABC *> m_DescriptorExtractors;

	std::vector<EncoderABC *> m_Encoders;

	std::vector<DecoderABC *> m_Decoders;

	std::vector<SearchABC *> m_SearchUtilities;

	std::vector<ParserABC *> m_Parsers;

	std::vector<ClientApplicationABC *> m_ClientApplications;

	std::vector<ServerApplicationABC *> m_ServerApplications;

	std::vector<EvaluationToolABC *> m_EvalTools;


};

};


#endif //__COMPONENTDB_H__
 
