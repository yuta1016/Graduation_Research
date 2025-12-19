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
// XMABCInterfaces.cpp

#include "XMABCInterfaces.h"

using namespace XM;

const char * I_InterfaceABC::myName = "Abstract Interface";
const UUID I_InterfaceABC::myID = UUID("c3fccb20-dc34-11d2-affe-0080c7e1e76d");

const char * I_StreamingComponentControlABC::myName = "Abstract Component Control Interface";
const UUID I_StreamingComponentControlABC::myID = UUID("fd41efe0-dc35-11d2-affe-0080c7e1e76d");

const char * I_ExtractionInterfaceABC::myName = "Abstract Extraction Interface";
const UUID I_ExtractionInterfaceABC::myID = UUID("1a19aa40-dc36-11d2-affe-0080c7e1e76d");

const char * I_DescriptionCodingInterfaceABC::myName = "Abstract Coding Interface";
const UUID I_DescriptionCodingInterfaceABC::myID = UUID("277cca00-dc36-11d2-affe-0080c7e1e76d");

const char * I_ClientApplicationABC::myName = "Abstract Client App Interface";
const UUID I_ClientApplicationABC::myID = UUID("3dfb74c0-dc36-11d2-affe-0080c7e1e76d");

const char * I_ServerApplicationABC::myName = "Abstract Server App Interface";
const UUID I_ServerApplicationABC::myID = UUID("464c0ea0-dc36-11d2-affe-0080c7e1e76d");

const char * I_ParserInterfaceABC::myName = "Abstract Parser Interface";
const UUID I_ParserInterfaceABC::myID = UUID("52e5c3e0-dc36-11d2-affe-0080c7e1e76d");

const char * I_EncoderInterfaceABC::myName = "Abstract Encoder Interface";
const UUID I_EncoderInterfaceABC::myID = UUID("5e5c07c0-dc36-11d2-affe-0080c7e1e76d");

const char * I_DecoderInterfaceABC::myName = "Abstract Decoder Interface";
const UUID I_DecoderInterfaceABC::myID = UUID("6a6f7600-dc36-11d2-affe-0080c7e1e76d");

const char * I_DescriptorAggregationABC::myName = "Abstract Descriptor Aggregation Interface";
const UUID I_DescriptorAggregationABC::myID = UUID("71de3b60-dc36-11d2-affe-0080c7e1e76d");

const char * I_ComponentInfoABC::myName = "Abstract Component Info Interface";
const UUID I_ComponentInfoABC::myID = UUID("7dc0d5a0-dc36-11d2-affe-0080c7e1e76d");

const char * I_DescriptorIdentifierABC::myName = "Abstract Description Identifier Interface";
const UUID I_DescriptorIdentifierABC::myID = UUID("87abe5a0-dc36-11d2-affe-0080c7e1e76d");

const char * I_DescriptorValueABC::myName = "Abstract Descriptor Value Interface";
const UUID I_DescriptorValueABC::myID = UUID("903fa300-dc36-11d2-affe-0080c7e1e76d");

const char * I_SystemBackEndABC::myName = "Abstract System Client Interface";
const UUID I_SystemBackEndABC::myID = UUID("977c0dc0-dc36-11d2-affe-0080c7e1e76d");

const char * I_SystemQueryABC::myName = "Abstract System Query Interface";
const UUID I_SystemQueryABC::myID = UUID("a0f32640-dc36-11d2-affe-0080c7e1e76d");

const char *I_SystemFrontEndABC::myName = "Abstract System Front End Interface";
const UUID I_SystemFrontEndABC::myID = UUID("a9b01680-dc36-11d2-affe-0080c7e1e76d");

const char *I_SearchInterfaceABC::myName = "Abstract Search Interface";
const UUID I_SearchInterfaceABC::myID = UUID("b0c1c7c0-dc36-11d2-affe-0080c7e1e76d");

const char *I_DDLABC::myName = "Abstract DDL Interface";
const UUID I_DDLABC::myID = UUID("b8415600-dc36-11d2-affe-0080c7e1e76d");

const char *I_EvaluationInterfaceABC::myName = "Abstract Evaluation Interface";
const UUID I_EvaluationInterfaceABC::myID = UUID("b8415601-dc36-11d2-affe-0080c7e1e76d");

I_EvaluationInterfaceABC::I_EvaluationInterfaceABC()
{ SetInterfaceID(myID); }

I_InterfaceABC::I_InterfaceABC()
{ SetInterfaceID(myID); }

I_StreamingComponentControlABC::I_StreamingComponentControlABC()
{ SetInterfaceID(myID); }

I_ExtractionInterfaceABC::I_ExtractionInterfaceABC()
{ SetInterfaceID(myID); }

I_DescriptionCodingInterfaceABC::I_DescriptionCodingInterfaceABC()
{ SetInterfaceID(myID); }

I_ClientApplicationABC::I_ClientApplicationABC()
{ SetInterfaceID(myID); }

I_ServerApplicationABC::I_ServerApplicationABC()
{ SetInterfaceID(myID); }

I_ParserInterfaceABC::I_ParserInterfaceABC()
{ SetInterfaceID(myID); }

I_EncoderInterfaceABC::I_EncoderInterfaceABC()
{ SetInterfaceID(myID); }

I_DecoderInterfaceABC::I_DecoderInterfaceABC()
{ SetInterfaceID(myID); }

I_DescriptorAggregationABC::I_DescriptorAggregationABC()
{ SetInterfaceID(myID); }

I_ComponentInfoABC::I_ComponentInfoABC()
{ SetInterfaceID(myID); }

I_DescriptorIdentifierABC::I_DescriptorIdentifierABC()
{ SetInterfaceID(myID); }

I_DescriptorValueABC::I_DescriptorValueABC()
{ SetInterfaceID(myID); }

I_SystemBackEndABC::I_SystemBackEndABC()
{ SetInterfaceID(myID); }

I_SystemQueryABC::I_SystemQueryABC()
{ SetInterfaceID(myID); }

I_SystemFrontEndABC::I_SystemFrontEndABC()
{ SetInterfaceID(myID); }

I_SearchInterfaceABC::I_SearchInterfaceABC()
{ SetInterfaceID(myID); }

I_DDLABC::I_DDLABC()
{ SetInterfaceID(myID); }

//=============================================================================
std::vector<UUID> I_InterfaceABC::GetInterfaceIDs(void)
{
	return containedInterfaces;
}

//----------------------------------------------------------------------------
void I_InterfaceABC::SetInterfaceID(const UUID& anID)
{
	containedInterfaces.push_back(anID);
}



