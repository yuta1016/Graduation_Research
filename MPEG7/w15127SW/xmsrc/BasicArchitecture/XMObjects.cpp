//## begin module.includes preserve=yes
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
// XMObjects.cpp

#include "BasicArchitecture/XMObjects.h"
#include "Utilities/Streams.h"



using namespace XM;


const UUID Descriptor::myID = UUID("4f447de0-c614-11d2-affe-0080c7e1e76d");
const char *Descriptor::myName = "Proprietary Descriptor Value";

const UUID Descriptor::valID = UUID("d04bb580-c71c-11d2-affe-0080c7e1e76d");
const char *Descriptor::valName = "Prototypical DDL Defined D";

const UUID DescriptionScheme::myID = UUID("4f447de1-c614-11d2-affe-0080c7e1e76d");
const char *DescriptionScheme::myName = "Proprietary DDL Defined DS";

const UUID DescriptionScheme::valID = UUID("d04bb590-c71c-11d2-affe-0080c7e1e76d");
const char *DescriptionScheme::valName = "Prototypical DDL Defined DS";

const UUID DescriptionCodingEngine::myID = UUID("4f447de2-c614-11d2-affe-0080c7e1e76d");
const char *DescriptionCodingEngine::myName = "Proto Coding Engine";

const UUID DDL::myID = UUID("4f447de3-c614-11d2-affe-0080c7e1e76d");
const char *DDL::myName = "Proto DDL";

const UUID System::myID = UUID("4f447de4-c614-11d2-affe-0080c7e1e76d");
const char *System::myName = "Proto System Layer";

const UUID DescriptorExtractor::myID = UUID("4f447de5-c614-11d2-affe-0080c7e1e76d");
const char *DescriptorExtractor::myName = "Proto Feature Extractor";

const UUID Encoder::myID = UUID("4f447de6-c614-11d2-affe-0080c7e1e76d");
const char *Encoder::myName = "Proto Encoder";

const UUID Decoder::myID = UUID("4f447de7-c614-11d2-affe-0080c7e1e76d");
const char *Decoder::myName = "Proto Decoder";

const UUID Parser::myID = UUID("4f447de8-c614-11d2-affe-0080c7e1e76d");
const char *Parser::myName = "Proto Parser";

const UUID Search::myID = UUID("4f447de9-c614-11d2-affe-0080c7e1e76d");
const char *Search::myName = "Proto Search";

const UUID ClientApplication::myID = UUID("4f447dea-c614-11d2-affe-0080c7e1e76d");
const char *ClientApplication::myName = "Proto Client Application";

const UUID ServerApplication::myID = UUID("4f447deb-c614-11d2-affe-0080c7e1e76d");
const char *ServerApplication::myName = "Proto Server Application";

const UUID EvaluationTool::myID = UUID("76678d60-db20-11d2-affe-0080c7e1e76d");
const char *EvaluationTool::myName = "Proto Evaluation Tool";

///////////////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4355)

Descriptor::Descriptor():
m_IdentIntfc(this),
m_ValueInterface(this),
m_InfoInterface(this),
m_isProprietary(true)
{
}

//----------------------------------------------------------------------------
Descriptor::~Descriptor()
{
}

//----------------------------------------------------------------------------
Descriptor& Descriptor::operator=(const Descriptor& rhs)
{
	if (this == &rhs)
		return *this;


	return *this;
}
//----------------------------------------------------------------------------
I_DescriptorIdentifierABC *Descriptor::GetIdentifier(void)
{ return &m_IdentIntfc;  }

//----------------------------------------------------------------------------
I_DescriptorValueABC *Descriptor::GetValue(void)
{ return &m_ValueInterface; }

//----------------------------------------------------------------------------
I_ComponentInfoABC *Descriptor::GetInfo(void)
{ return &m_InfoInterface; }
	
//----------------------------------------------------------------------------
unsigned long Descriptor::DescribeIdentifier(DDLScript& desc)
{
	throw("not implemented");

	return 0;
}

//----------------------------------------------------------------------------
unsigned long Descriptor::DescribeValue(DDLScript& desc)
{
	throw("not implemented");

	return 0;
}

//----------------------------------------------------------------------------
bool Descriptor::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const UUID& Descriptor::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* Descriptor::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
const UUID& Descriptor::GetObjectID(void)
{	return myID; }

//----------------------------------------------------------------------------
const char *Descriptor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Descriptor::ReceiveThisInterface(I_InterfaceABC *intfc)
{ return true; }

///////////////////////////////////////////////////////////////////////////////
DescriptionScheme::DescriptionScheme():
m_DescriptionInterface(this),
m_ComponentInterface(this),
m_isProprietary(false)
{
}

//----------------------------------------------------------------------------
DescriptionScheme::~DescriptionScheme()
{
}

//----------------------------------------------------------------------------
DescriptionScheme& DescriptionScheme::operator=(const DescriptionScheme& rhs)
{
	if (this == &rhs)
		return *this;

	// copy all the descriptors.
	m_AggregatedDescriptors = rhs.m_AggregatedDescriptors;

	return *this;
}

//----------------------------------------------------------------------------
I_DescriptorAggregationABC *DescriptionScheme::GetDescriptorIntfc(void)
{
	return &m_DescriptionInterface;
}

//----------------------------------------------------------------------------
std::vector<Descriptor>& DescriptionScheme::GetDescriptors(void)
{
	return m_AggregatedDescriptors;
}

//----------------------------------------------------------------------------
unsigned long DescriptionScheme::SetDescriptors(std::vector<Descriptor> desc)
{
	m_AggregatedDescriptors = desc;

	return 0;
}

//----------------------------------------------------------------------------
I_ComponentInfoABC *DescriptionScheme::GetInfo(void)
{
	return &m_ComponentInterface;
}

//----------------------------------------------------------------------------
bool DescriptionScheme::IsProprietary(void)
{ return m_isProprietary; }

//----------------------------------------------------------------------------
const UUID& DescriptionScheme::GetValueID(void)
{ return valID; }

//----------------------------------------------------------------------------
const char* DescriptionScheme::GetValueName(void)
{ return valName; }

//----------------------------------------------------------------------------
const UUID& DescriptionScheme::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DescriptionScheme::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool DescriptionScheme::ReceiveThisInterface(I_InterfaceABC *intfc)
{  return true;	}

///////////////////////////////////////////////////////////////////////////////
DescriptionCodingEngine::DescriptionCodingEngine():
m_CodingInterface(this),
m_encInterface(0)
{
	SetExposedInterface(&m_CodingInterface);
	SetInputInterfaces(I_EncoderInterfaceABC::myID);
}

//----------------------------------------------------------------------------
DescriptionCodingEngine::~DescriptionCodingEngine()
{
}

//----------------------------------------------------------------------------
const UUID& DescriptionCodingEngine::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DescriptionCodingEngine::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool DescriptionCodingEngine::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_EncoderInterfaceABC::myID)
		{
			m_encInterface = static_cast<I_EncoderInterfaceABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_DescriptionCodingInterfaceABC *DescriptionCodingEngine::GetCodingInterface(void)
{
	return &m_CodingInterface;
}

//----------------------------------------------------------------------------
unsigned long DescriptionCodingEngine::SetDescriptor(Descriptor *aDesc)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long DescriptionCodingEngine::CodeDescriptor(std::vector<unsigned char>& outBuf)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long DescriptionCodingEngine::SetDescriptionScheme(DescriptionScheme *aDS)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long DescriptionCodingEngine::CodeDescriptionScheme(std::vector<unsigned char>& outBuf)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
I_EncoderInterfaceABC *DescriptionCodingEngine::GetEncoderInterface()
{ return m_encInterface; }

///////////////////////////////////////////////////////////////////////////////
System::System():
m_SystemBackEndInterface(this),
m_SystemFrontEndInterface(this),
m_SystemQuery(this),
m_encInterface(0),
m_ParserInterface(0)
{
	SetExposedInterface(&m_SystemBackEndInterface);
	SetExposedInterface(&m_SystemFrontEndInterface);
	SetExposedInterface(&m_SystemQuery);

	SetInputInterfaces(I_EncoderInterfaceABC::myID);
	SetInputInterfaces(I_ParserInterfaceABC::myID);
}

//----------------------------------------------------------------------------
System::~System()
{
}

//----------------------------------------------------------------------------
const UUID& System::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *System::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool System::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_EncoderInterface::myID)
		{
			m_encInterface = static_cast<I_EncoderInterfaceABC *>(intfc);
			break;
		}
		
		if (intfcIds[i] == I_ParserInterface::myID)
		{
			m_ParserInterface = static_cast<I_ParserInterfaceABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_SystemBackEndABC* System::GetClientInterface(void)
{ return &m_SystemBackEndInterface; }
//----------------------------------------------------------------------------
I_SystemFrontEndABC* System::GetServerInterface(void)
{ return &m_SystemFrontEndInterface; }
//----------------------------------------------------------------------------
I_SystemQuery *System::GetSystemQuery(void)
{ return &m_SystemQuery; }

//----------------------------------------------------------------------------
//void System::SetInputStream(XMStream& inStream)
//{
//	m_inStream = &inStream;
//}

//----------------------------------------------------------------------------
unsigned long System::PushBuffer(std::vector<unsigned char>& aBuff)
{
	throw("Not Implemented");
	return 0;
}

//----------------------------------------------------------------------------
bool System::PushesTowardClient(void)
{
	throw("Not Implemented");
	return false;
}
//----------------------------------------------------------------------------
//XMStream& System::GetOutputStream(void)
//{
//	return m_outStream;
//}

//----------------------------------------------------------------------------
I_EncoderInterfaceABC *System::GetEncoderInterface()
{ return m_encInterface; }

//----------------------------------------------------------------------------
I_ParserInterfaceABC *System::GetParserInterface()
{ return m_ParserInterface; }

///////////////////////////////////////////////////////////////////////////////
DDL::DDL():
m_DDLInterface(this)
{
	SetExposedInterface(&m_DDLInterface);
}

//----------------------------------------------------------------------------
DDL::~DDL()
{
}

//----------------------------------------------------------------------------
const UUID& DDL::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DDL::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool DDL::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	return false;
}

//----------------------------------------------------------------------------
I_DDLABC* DDL::GetDDLInterface(void)
{ return &m_DDLInterface; }

//----------------------------------------------------------------------------
void DDL::DescribeDescriptor(Descriptor&)
{
	throw("Not Implemented");
}
//----------------------------------------------------------------------------
void DDL::DescribeScheme(DescriptionScheme&)
{
	throw("Not Implemented");
}

//----------------------------------------------------------------------------
DDLScript& DDL::GetDDLScript(void)
{
	return m_Script;
}


///////////////////////////////////////////////////////////////////////////////
DDLScript::DDLScript()
{
}
//----------------------------------------------------------------------------
DDLScript::~DDLScript()
{
}

//----------------------------------------------------------------------------
std::basic_string<char>& DDLScript::GetDDLRepresentation(void)
{
	return m_script;
}
//----------------------------------------------------------------------------
unsigned long DDLScript::SetDDLRepresentation(std::basic_string<char>& text)
{
	m_script = text;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
DescriptorExtractor::DescriptorExtractor():
m_ExtractionInterface(this)
{
	SetExposedInterface(&m_ExtractionInterface);
}

//----------------------------------------------------------------------------
DescriptorExtractor::~DescriptorExtractor()
{
}

//----------------------------------------------------------------------------
const UUID& DescriptorExtractor::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *DescriptorExtractor::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool DescriptorExtractor::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();


	return false;
}

//----------------------------------------------------------------------------
bool DescriptorExtractor::SupportsPush(void)
{
	return true;
}

//----------------------------------------------------------------------------
bool DescriptorExtractor::SupportsPull(void)
{
	return true;
}

//----------------------------------------------------------------------------
//unsigned long DescriptorExtractor::SetSourceMedia(MediaDescriptor&, XMStream& mediaitself)
//{
//	throw("Not Implemented");
//	return 0;
//}

//----------------------------------------------------------------------------
unsigned long DescriptorExtractor::ExtractFeature(Descriptor* aDesc, I_ExtractionInterfaceABC::ExtractionResult& aResult)
{
	throw("Not Implemented");
	return 0;
}

//----------------------------------------------------------------------------
unsigned long DescriptorExtractor::StartExtracting(void)
{
	throw("Not Implemented");
	return 0;
}

//----------------------------------------------------------------------------
I_ExtractionInterfaceABC* DescriptorExtractor::GetExtractionInterface(void)
{ return &m_ExtractionInterface; }


///////////////////////////////////////////////////////////////////////////////
Encoder::Encoder():
m_EncoderInterface(this),
m_SystemFrontEnd(0),
m_CodingInterface(0),
m_DDL(0)
{
	SetExposedInterface(&m_EncoderInterface);

	SetInputInterfaces(I_DescriptionCodingInterfaceABC::myID);
	SetInputInterfaces(I_DDLABC::myID);
	SetInputInterfaces(I_SystemFrontEndABC::myID);
	
}

//----------------------------------------------------------------------------
Encoder::~Encoder()
{
}

//----------------------------------------------------------------------------
const UUID& Encoder::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Encoder::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Encoder::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_DescriptionCodingInterfaceABC::myID)
		{
			m_CodingInterface = static_cast<I_DescriptionCodingInterfaceABC *>(intfc);
			break;
		}
		if (intfcIds[i] == I_SystemFrontEndABC::myID)
		{
			m_SystemFrontEnd = static_cast<I_SystemFrontEndABC *>(intfc);
			break;
		}
		if (intfcIds[i] == I_DDLABC::myID)
		{
			m_DDL = static_cast<I_DDLABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_SystemFrontEndABC *Encoder::GetSystemFrontEnd()
{ return m_SystemFrontEnd; }
//----------------------------------------------------------------------------
I_DescriptionCodingInterfaceABC *Encoder::GetCodingInterface()
{ return m_CodingInterface; }
//----------------------------------------------------------------------------
I_DDLABC *Encoder::GetDDL()
{ return m_DDL; }
//----------------------------------------------------------------------------
I_EncoderInterfaceABC* Encoder::GetInterface(void)
{ return &m_EncoderInterface; }
//----------------------------------------------------------------------------
unsigned long Encoder::CodeDescriptor(Descriptor *aDesc)
{
	// send descriptor to the coding scheme
	GetCodingInterface()->SetDescriptor(aDesc);

	std::vector<unsigned char> outBuf;
	GetCodingInterface()->CodeDescriptor(outBuf);
	
	GetSystemFrontEnd()->PushBuffer(outBuf);
	return 0;
}
//----------------------------------------------------------------------------
unsigned long Encoder::CodeDescriptionScheme(DescriptionScheme *aDS)
{
	throw("Not Implemented");
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
Parser::Parser():
m_ParserInterface(this),
m_SystemBackEnd(0)
{
	SetExposedInterface(&m_ParserInterface);

	SetInputInterfaces(I_SystemBackEndABC::myID);
	
}

//----------------------------------------------------------------------------
Parser::~Parser()
{
}

//----------------------------------------------------------------------------
const UUID& Parser::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Parser::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Parser::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_SystemBackEnd::myID)
		{
			m_SystemBackEnd = static_cast<I_SystemBackEndABC *>(intfc);
			break;
		}
	}

	return true;
}
//----------------------------------------------------------------------------
I_SystemBackEndABC *Parser::GetSystemBackEnd()
{ return m_SystemBackEnd; }

//----------------------------------------------------------------------------
I_ParserInterfaceABC* Parser::GetInterface(void)
{ return &m_ParserInterface; }

//----------------------------------------------------------------------------
Descriptor* Parser::FindDescriptor(UUID& anID)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
DescriptionScheme* Parser::FindDescriptionScheme(UUID& anID)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
bool Parser::PushesTowardClient(void)
{
	throw("Not Implemented");
	return false;
}


///////////////////////////////////////////////////////////////////////////////
Decoder::Decoder():
m_DecoderInterface(this),
m_ParserInterface(0)
{
	SetExposedInterface(&m_DecoderInterface);

	SetInputInterfaces(I_ParserInterfaceABC::myID);
	
}

//----------------------------------------------------------------------------
Decoder::~Decoder()
{
}

//----------------------------------------------------------------------------
const UUID& Decoder::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Decoder::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Decoder::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_ParserInterface::myID)
		{
			m_ParserInterface = static_cast<I_ParserInterfaceABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_ParserInterfaceABC *Decoder::GetParserInterface()
{ return m_ParserInterface; }

//----------------------------------------------------------------------------
I_DecoderInterfaceABC* Decoder::GetInterface(void)
{ return &m_DecoderInterface; }
//----------------------------------------------------------------------------
unsigned long Decoder::GetNextDescriptionScheme(DescriptionSchemeABC& aDesc)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
unsigned long Decoder::GetNextDescription(DescriptorABC& aD)
{
	throw("Not Implemented");
	return 0;
}
//----------------------------------------------------------------------------
bool Decoder::PushesTowardClient(void)
{
	throw("Not Implemented");
	return false;
}



///////////////////////////////////////////////////////////////////////////////
EvaluationTool::EvaluationTool()
{
}

//----------------------------------------------------------------------------
EvaluationTool::~EvaluationTool()
{
}

//----------------------------------------------------------------------------
const UUID& EvaluationTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *EvaluationTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool EvaluationTool::ReceiveThisInterface(I_InterfaceABC *intfc)
{

	return true;
}

///////////////////////////////////////////////////////////////////////////////
Search::Search():
m_SearchInterface(this),
m_SystemSearch(0)
{
	SetExposedInterface(&m_SearchInterface);

 	SetInputInterfaces(I_SystemQueryABC::myID);
	
}

//----------------------------------------------------------------------------
Search::~Search()
{
}

//----------------------------------------------------------------------------
const UUID& Search::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *Search::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool Search::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_SystemQuery::myID)
		{
			m_SystemSearch = static_cast<I_SystemQueryABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_SystemQueryABC *Search::GetSystemSearch(void)
{ return m_SystemSearch; }

I_SearchInterfaceABC* Search::GetSearchInterface(void)
{ return &m_SearchInterface; }


///////////////////////////////////////////////////////////////////////////////
ClientApplication::ClientApplication():
m_ApplicationIntfc(this),
m_ComponentControl(this),
m_SearchInterface(0)
{
	SetExposedInterface(&m_ApplicationIntfc);
	SetExposedInterface(&m_ComponentControl);

	SetInputInterfaces(I_SearchInterface::myID);
	
}

//----------------------------------------------------------------------------
ClientApplication::~ClientApplication()
{
}

//----------------------------------------------------------------------------
const UUID& ClientApplication::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ClientApplication::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool ClientApplication::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_SearchInterface::myID)
		{
			m_SearchInterface = static_cast<I_SearchInterfaceABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_SearchInterfaceABC *ClientApplication::GetSearchInterface()
{ return m_SearchInterface; }

//----------------------------------------------------------------------------
I_StreamingComponentControlABC* ClientApplication::GetStreamingControl(void)
{ return &m_ComponentControl; }

//----------------------------------------------------------------------------
bool ClientApplication::SupportsPush(void)
{
	return true;
}
//----------------------------------------------------------------------------
bool ClientApplication::SupportsPull(void)
{
	return true;
}
//----------------------------------------------------------------------------
I_ClientApplicationABC* ClientApplication::GetAppInterface(void)
{ return &m_ApplicationIntfc; }


//----------------------------------------------------------------------------
unsigned long ClientApplication::Open()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ClientApplication::Start()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ClientApplication::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ClientApplication::Close()
{
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
ServerApplication::ServerApplication():
m_ApplicationIntfc(this),
m_ComponentControl(this),
m_EncoderInterface(0),
m_ExtractionInterface(0)
{
	SetExposedInterface(&m_ApplicationIntfc);
	SetExposedInterface(&m_ComponentControl);

	SetInputInterfaces(I_EncoderInterfaceABC::myID);
	SetInputInterfaces(I_ExtractionInterfaceABC::myID);
	
}

//----------------------------------------------------------------------------
ServerApplication::~ServerApplication()
{
}

//----------------------------------------------------------------------------
const UUID& ServerApplication::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *ServerApplication::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
I_StreamingComponentControlABC* ServerApplication::GetStreamingControl(void)
{ return &m_ComponentControl; }

//----------------------------------------------------------------------------
bool ServerApplication::ReceiveThisInterface(I_InterfaceABC *intfc)
{
	std::vector<UUID> intfcIds = intfc->GetInterfaceIDs();

	for (int i=0; i < intfcIds.size(); i++)
	{
		if (intfcIds[i] == I_EncoderInterfaceABC::myID)
		{
			m_EncoderInterface = static_cast<I_EncoderInterfaceABC *>(intfc);
			break;
		}
		if (intfcIds[i] == I_ExtractionInterfaceABC::myID)
		{
			m_ExtractionInterface = static_cast<I_ExtractionInterfaceABC *>(intfc);
			break;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
I_ServerApplicationABC* ServerApplication::GetAppInterface(void)
{ return &m_ApplicationIntfc; }

//----------------------------------------------------------------------------
I_EncoderInterfaceABC *ServerApplication::GetEncoderInterface(void)
{ return m_EncoderInterface; }

//----------------------------------------------------------------------------
I_ExtractionInterfaceABC *ServerApplication::GetExtractionInterface(void)
{ return m_ExtractionInterface; }

//----------------------------------------------------------------------------
unsigned long ServerApplication::Open()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ServerApplication::Start()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ServerApplication::Stop()
{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long ServerApplication::Close()
{
	return 0;
}

