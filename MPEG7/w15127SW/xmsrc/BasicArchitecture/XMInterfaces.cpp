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
// XMInterfaces.cpp

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
//#include "Utilities/Streams.h"


using namespace XM;

const UUID I_DescriptorIdentifier::myID = UUID("af9e48c0-c721-11d2-affe-0080c7e1e76d");
const char *I_DescriptorIdentifier::myName = "D Ident Interface";

const UUID I_DescriptorValue::myID = UUID("c08a4120-c721-11d2-affe-0080c7e1e76d");
const char *I_DescriptorValue::myName = "D Value Interface";

const UUID I_ComponentInfoD::myID = UUID("cfa1c7a0-c721-11d2-affe-0080c7e1e76d");
const char *I_ComponentInfoD::myName = "D Identifiers Interface";

const UUID I_ComponentInfoDS::myID = UUID("cfa1c7a1-c721-11d2-affe-0080c7e1e76d");
const char *I_ComponentInfoDS::myName = "DS Identifiers Interface";

const UUID I_StreamingComponentControl::myID = UUID("cfa1c7a2-c721-11d2-affe-0080c7e1e76d");
const char *I_StreamingComponentControl::myName = "Streaming Component Control Interface";

const UUID I_ExtractionInterface::myID = UUID("cfa1c7a3-c721-11d2-affe-0080c7e1e76d");
const char *I_ExtractionInterface::myName = "Extraction Interface";

const UUID I_DescriptionCodingInterface::myID = UUID("cfa1c7a4-c721-11d2-affe-0080c7e1e76d");
const char *I_DescriptionCodingInterface::myName = "Description Coding Interface";

const UUID I_ClientApplication::myID = UUID("cfa1c7a5-c721-11d2-affe-0080c7e1e76d");
const char *I_ClientApplication::myName = "Client Interface";

const UUID I_ServerApplication::myID = UUID("cfa1c7a6-c721-11d2-affe-0080c7e1e76d");
const char *I_ServerApplication::myName = "Server Interface";

const UUID I_ParserInterface::myID = UUID("cfa1c7a7-c721-11d2-affe-0080c7e1e76d");
const char *I_ParserInterface::myName = "Parser Interface";

const UUID I_EncoderInterface::myID = UUID("cfa1c7a8-c721-11d2-affe-0080c7e1e76d");
const char *I_EncoderInterface::myName = "Encoder Interface";

const UUID I_DecoderInterface::myID = UUID("cfa1c7a9-c721-11d2-affe-0080c7e1e76d");
const char *I_DecoderInterface::myName = "Decoder Interface";

const UUID I_DescriptorAggregation::myID = UUID("cfa1c7aa-c721-11d2-affe-0080c7e1e76d");
const char *I_DescriptorAggregation::myName = "Descriptor Aggregate Interface";

const UUID I_SystemBackEnd::myID = UUID("cfa1c7ab-c721-11d2-affe-0080c7e1e76d");
const char *I_SystemBackEnd::myName = "System Client Side Interface";

const UUID I_SystemFrontEnd::myID = UUID("cfa1c7ac-c721-11d2-affe-0080c7e1e76d");
const char *I_SystemFrontEnd::myName = "System Server Side Interface";

const UUID I_DDL::myID = UUID("cfa1c7ad-c721-11d2-affe-0080c7e1e76d");
const char *I_DDL::myName = "DDL Interface";

const UUID I_SearchInterface::myID = UUID("cfa1c7ae-c721-11d2-affe-0080c7e1e76d");
const char *I_SearchInterface::myName = "Search Interface";

const UUID I_SystemQuery::myID = UUID("cfa1c7af-c721-11d2-affe-0080c7e1e76d");
const char *I_SystemQuery::myName = "Query System Interface";

const UUID I_ServerComponentControl::myID = UUID("cfa1c7b0-c721-11d2-affe-0080c7e1e76d");
const char *I_ServerComponentControl::myName = "Server Navigation";

const UUID I_ClientComponentControl::myID = UUID("cfa1c7b1-c721-11d2-affe-0080c7e1e76d");
const char *I_ClientComponentControl::myName = "Client Navigation";

const UUID I_EvaluationInterface::myID = UUID("cfa1c7b2-c721-11d2-affe-0080c7e1e76d");
const char *I_EvaluationInterface::myName = "Evaluation Interface";

//=============================================================================
I_DescriptorIdentifier::I_DescriptorIdentifier(Descriptor* aD):
m_Descriptor(aD)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
I_DescriptorIdentifier::~I_DescriptorIdentifier()
{
}

//----------------------------------------------------------------------------
unsigned long I_DescriptorIdentifier::DescribeIdentifer(DDLScript& desc)
{
	return m_Descriptor->DescribeIdentifier(desc);
}

//----------------------------------------------------------------------------
const UUID& I_DescriptorIdentifier::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *I_DescriptorIdentifier::GetName(void)
{ return myName; }


//=============================================================================
I_DescriptorValue::I_DescriptorValue(Descriptor *aD):
m_Descriptor(aD)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
I_DescriptorValue::~I_DescriptorValue()
{
}

//----------------------------------------------------------------------------
unsigned long I_DescriptorValue::DescribeValue(DDLScript& desc)
{
	return m_Descriptor->DescribeValue(desc);
}

//----------------------------------------------------------------------------
const UUID& I_DescriptorValue::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *I_DescriptorValue::GetName(void)
{ return myName; }

//=============================================================================
I_ComponentInfoD::I_ComponentInfoD(Descriptor *aD):
m_D(aD)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
I_ComponentInfoD::~I_ComponentInfoD()
{
}

//----------------------------------------------------------------------------
bool I_ComponentInfoD::IsProprietary(void)
{
	return m_D->IsProprietary();
}

//----------------------------------------------------------------------------
const UUID& I_ComponentInfoD::GetValueID(void)
{
	return m_D->GetValueID();
}

//----------------------------------------------------------------------------
const char* I_ComponentInfoD::GetValueName(void)
{
	return m_D->GetValueName();
}

//----------------------------------------------------------------------------
const UUID& I_ComponentInfoD::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *I_ComponentInfoD::GetName(void)
{  return myName; }


//=============================================================================
I_ComponentInfoDS::I_ComponentInfoDS(DescriptionScheme *aDS):
m_DS(aDS)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
I_ComponentInfoDS::~I_ComponentInfoDS()
{
}

//----------------------------------------------------------------------------
bool I_ComponentInfoDS::IsProprietary(void)
{
	return m_DS->IsProprietary();
}

//----------------------------------------------------------------------------
const UUID& I_ComponentInfoDS::GetValueID(void)
{
	return m_DS->GetValueID();
}

//----------------------------------------------------------------------------
const char* I_ComponentInfoDS::GetValueName(void)
{
	return m_DS->GetValueName();
}

//----------------------------------------------------------------------------
const UUID& I_ComponentInfoDS::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *I_ComponentInfoDS::GetName(void)
{  return myName; }


//=============================================================================
I_DescriptorAggregation::I_DescriptorAggregation(DescriptionScheme *aScheme):
m_Scheme(aScheme)
{
}

I_DescriptorAggregation::~I_DescriptorAggregation()
{
}

std::vector<Descriptor>& I_DescriptorAggregation::GetDescriptors(void)
{ return m_Scheme->GetDescriptors(); }

unsigned long I_DescriptorAggregation::SetDescriptors(std::vector<Descriptor> desc)
{ return m_Scheme->SetDescriptors(desc); }

const UUID& I_DescriptorAggregation::GetInterfaceID(void)
{ return myID; }

const char *I_DescriptorAggregation::GetName(void)
{ return myName; }

//=============================================================================
I_DDL::I_DDL(DDL* aDDL):
m_DDL(aDDL)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
I_DDL::~I_DDL() {}

//----------------------------------------------------------------------------
void I_DDL::DescribeDescriptor(Descriptor& aD)
{
	m_DDL->DescribeDescriptor(aD);
}
//----------------------------------------------------------------------------
void I_DDL::DescribeScheme(DescriptionScheme& aDS)
{
	m_DDL->DescribeScheme(aDS);
}
//----------------------------------------------------------------------------
DDLScript& I_DDL::GetDDLScript(void)
{
	return m_DDL->GetDDLScript();
}
//----------------------------------------------------------------------------
const UUID& I_DDL::GetInterfaceID(void)
{	return myID; }

//----------------------------------------------------------------------------
const char *I_DDL::GetName(void)
{	return myName; }

//=============================================================================
I_SystemQuery::I_SystemQuery(System *aSystem):
m_System(aSystem)
{
	SetInterfaceID(myID);
}

I_SystemQuery::~I_SystemQuery()
{}

const UUID& I_SystemQuery::GetInterfaceID(void)
{ return myID; }

const char *I_SystemQuery::GetName(void)
{ return myName; }
//=============================================================================
I_SearchInterface::I_SearchInterface(Search *aSearch):
m_Search(aSearch)
{
	SetInterfaceID(myID);
}

I_SearchInterface::~I_SearchInterface()
{}

const UUID& I_SearchInterface::GetInterfaceID(void)
{ return myID; }

const char *I_SearchInterface::GetName(void)
{ return myName; }
//=============================================================================
I_SystemFrontEnd::I_SystemFrontEnd(System *aSys):
m_System(aSys)
{
	SetInterfaceID(myID);
}

I_SystemFrontEnd::~I_SystemFrontEnd()
{ }

//void I_SystemFrontEnd::SetInputStream(XMStream& inStream)
//{
//	m_System->SetInputStream(inStream);
//}

unsigned long I_SystemFrontEnd::PushBuffer(std::vector<unsigned char>& aBuff)
{
	return m_System->PushBuffer(aBuff);
}


const UUID& I_SystemFrontEnd::GetInterfaceID(void)
{ return myID; }

const char *I_SystemFrontEnd::GetName(void)
{ return myName; }
//=============================================================================
I_SystemBackEnd::I_SystemBackEnd(System *aSys):
m_System(aSys)
{
	SetInterfaceID(myID);
}

I_SystemBackEnd::~I_SystemBackEnd()
{}

bool I_SystemBackEnd::PushesTowardClient(void)
{
	return m_System->PushesTowardClient(); 
}

//XMStream& I_SystemBackEnd::GetOutputStream(void)
//{ 
//	return m_System->GetOutputStream(); 
//}

const UUID& I_SystemBackEnd::GetInterfaceID(void)
{ return myID; }

const char *I_SystemBackEnd::GetName(void)
{ return myName; }
//=============================================================================
I_DecoderInterface::I_DecoderInterface(Decoder *aDec):
m_Decoder(aDec)
{
	SetInterfaceID(myID);
}

I_DecoderInterface::~I_DecoderInterface()
{}

unsigned long I_DecoderInterface::GetNextDescriptionScheme(DescriptionSchemeABC& aDesc)
{ return m_Decoder->GetNextDescriptionScheme(aDesc); }

unsigned long I_DecoderInterface::GetNextDescription(DescriptorABC& aD)
{ return m_Decoder->GetNextDescription(aD); }

const UUID& I_DecoderInterface::GetInterfaceID(void)
{ return myID; }

const char *I_DecoderInterface::GetName(void)
{ return myName; }

bool I_DecoderInterface::PushesTowardClient(void)
{
	return m_Decoder->PushesTowardClient();
}

//=============================================================================
I_EncoderInterface::I_EncoderInterface(Encoder *aEnc):
m_Encoder(aEnc)
{
	SetInterfaceID(myID);
}

I_EncoderInterface::~I_EncoderInterface()
{}

unsigned long I_EncoderInterface:: CodeDescriptor(Descriptor *aDesc)
{ return m_Encoder->CodeDescriptor(aDesc); }

unsigned long I_EncoderInterface::CodeDescriptionScheme(DescriptionScheme *aDS)
{ return m_Encoder->CodeDescriptionScheme(aDS); }

const UUID& I_EncoderInterface::GetInterfaceID(void)
{ return myID; }

const char *I_EncoderInterface::GetName(void)
{ return myName; }
//=============================================================================
I_ParserInterface::I_ParserInterface(Parser *aPars):
m_Parser(aPars)
{
	SetInterfaceID(myID);
}

I_ParserInterface::~I_ParserInterface()
{}

bool I_ParserInterface::PushesTowardClient(void)
{ return m_Parser->PushesTowardClient(); }

Descriptor* I_ParserInterface::FindDescriptor(UUID& anID)
{ return m_Parser->FindDescriptor(anID); }

DescriptionScheme* I_ParserInterface::FindDescriptionScheme(UUID& anID)
{ return m_Parser->FindDescriptionScheme(anID); }

const UUID& I_ParserInterface::GetInterfaceID(void)
{ return myID; }

const char *I_ParserInterface::GetName(void)
{ return myName; }

//=============================================================================
I_ServerApplication::I_ServerApplication(ServerApplication *anApp):
m_Application(anApp)
{
	SetInterfaceID(myID);
}

I_ServerApplication::~I_ServerApplication()
{}

const UUID& I_ServerApplication::GetInterfaceID(void)
{ return myID; }

const char *I_ServerApplication::GetName(void)
{ return myName; }
//=============================================================================
I_ClientApplication::I_ClientApplication(ClientApplication *anApp):
m_Application(anApp)
{
	SetInterfaceID(myID);
}

I_ClientApplication::~I_ClientApplication()
{}

bool I_ClientApplication::SupportsPush(void)
{ return m_Application->SupportsPush(); }

bool I_ClientApplication::SupportsPull(void)
{ return m_Application->SupportsPull(); }

const UUID& I_ClientApplication::GetInterfaceID(void)
{ return myID; }

const char *I_ClientApplication::GetName(void)
{ return myName; }

//=============================================================================
I_DescriptionCodingInterface::I_DescriptionCodingInterface(DescriptionCodingEngine *anEngine):
m_Engine(anEngine)
{
	SetInterfaceID(myID);
}

I_DescriptionCodingInterface::~I_DescriptionCodingInterface()
{}

unsigned long I_DescriptionCodingInterface::SetDescriptor(Descriptor *aDesc)
{ return m_Engine->SetDescriptor(aDesc); }

unsigned long I_DescriptionCodingInterface::CodeDescriptor(std::vector<unsigned char>& outBuf)
{ return m_Engine->CodeDescriptor(outBuf); }

unsigned long I_DescriptionCodingInterface::SetDescriptionScheme(DescriptionScheme *aDS)
{ return m_Engine->SetDescriptionScheme(aDS); }

unsigned long I_DescriptionCodingInterface::CodeDescriptionScheme(std::vector<unsigned char>& outBuf)
{ return m_Engine->CodeDescriptionScheme(outBuf); }


const UUID& I_DescriptionCodingInterface::GetInterfaceID(void)
{ return myID; }

const char *I_DescriptionCodingInterface::GetName(void)
{ return myName; }

//=============================================================================
I_ExtractionInterface::I_ExtractionInterface(DescriptorExtractor *anExtractor):
m_Extractor(anExtractor)
{
	SetInterfaceID(myID);
}

I_ExtractionInterface::~I_ExtractionInterface()
{}

bool I_ExtractionInterface::SupportsPush(void)
{
	return m_Extractor->SupportsPush();
}

bool I_ExtractionInterface::SupportsPull(void)
{
	return m_Extractor->SupportsPull();
}

//unsigned long I_ExtractionInterface::SetSourceMedia(MediaDescriptor& am, XMStream& mediaitself)
//{
//	return m_Extractor->SetSourceMedia(am, mediaitself);
//}

unsigned long I_ExtractionInterface::ExtractFeature(Descriptor* aDesc, I_ExtractionInterfaceABC::ExtractionResult& aResult)
{
	return m_Extractor->ExtractFeature(aDesc, aResult);
}

unsigned long I_ExtractionInterface::StartExtracting(void)
{ 
	return m_Extractor->StartExtracting(); 
}

const UUID& I_ExtractionInterface::GetInterfaceID(void)
{ return myID; }

const char *I_ExtractionInterface::GetName(void)
{ return myName; }

//=============================================================================
I_StreamingComponentControl::I_StreamingComponentControl()
{
	SetInterfaceID(myID);
}

I_StreamingComponentControl::~I_StreamingComponentControl()
{}

unsigned long I_StreamingComponentControl::Open()
{ 
	return 0;
}

unsigned long I_StreamingComponentControl::Start()
{
	return 0;
}

unsigned long I_StreamingComponentControl::Stop()
{
	return 0;
}

unsigned long I_StreamingComponentControl::Close()
{
	return 0;
}

const UUID& I_StreamingComponentControl::GetInterfaceID(void)
{ return myID; }

const char *I_StreamingComponentControl::GetName(void)
{ return myName; }


//=============================================================================
I_ServerComponentControl::I_ServerComponentControl(ServerApplication *app):
m_Server(app)
{
	SetInterfaceID(myID);
}

I_ServerComponentControl::~I_ServerComponentControl()
{}

unsigned long I_ServerComponentControl::Open()
{ return m_Server->Open(); }

unsigned long I_ServerComponentControl::Start()
{ return m_Server->Start(); }

unsigned long I_ServerComponentControl::Stop()
{ return m_Server->Stop(); }

unsigned long I_ServerComponentControl::Close()
{ return m_Server->Close(); }

const UUID& I_ServerComponentControl::GetInterfaceID(void)
{ return myID; }

const char *I_ServerComponentControl::GetName(void)
{ return myName; }


//===========================================================================
I_ClientComponentControl::I_ClientComponentControl(ClientApplication *app):
m_Client(app)
{
	SetInterfaceID(myID);
}

I_ClientComponentControl::~I_ClientComponentControl()
{}

unsigned long I_ClientComponentControl::Open()
{ return m_Client->Open(); }

unsigned long I_ClientComponentControl::Start()
{ return m_Client->Start(); }

unsigned long I_ClientComponentControl::Stop()
{ return m_Client->Stop(); }

unsigned long I_ClientComponentControl::Close()
{ return m_Client->Close(); }

const UUID& I_ClientComponentControl::GetInterfaceID(void)
{ return myID; }

const char *I_ClientComponentControl::GetName(void)
{ return myName; }

//===========================================================================
I_EvaluationInterface::I_EvaluationInterface(EvaluationTool *tool):
m_EvalTool(tool)
{
	SetInterfaceID(myID);
}

I_EvaluationInterface::~I_EvaluationInterface()
{
}

const UUID& I_EvaluationInterface::GetInterfaceID(void)
{ return myID; }

const char *I_EvaluationInterface::GetName(void)
{ return myName; }
