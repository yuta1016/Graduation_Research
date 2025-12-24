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
// XMTypes.h

#ifndef __XMTYPES_H__
#define __XMTYPES_H__

namespace XM
{

class I_InterfaceABC;
class I_StreamingComponentControlABC;
class I_ExtractionInterfaceABC;
class I_DescriptionCodingInterfaceABC;
class I_ParserInterfaceABC;
class I_EncoderInterfaceABC;
class I_DecoderInterfaceABC;
class I_SystemBackEndABC;
class I_SystemFrontEndABC;
class I_ComponentInfoABC;
class I_DescriptorIdentifierABC;
class I_DescriptorValueABC;
class I_DescriptorAggregationABC;
class I_DDLABC;
class I_SystemQueryABC;
class I_SearchInterfaceABC;


class I_StreamingComponentControl;
class I_ExtractionInterface;
class I_DescriptionCodingInterface;
class I_ParserInterface;
class I_EncoderInterface;
class I_DecoderInterface;
class I_SystemBackEnd;
class I_SystemFrontEnd;
class I_ComponentInfo;
class I_DescriptorIdentifier;
class I_DescriptorValue;
class I_DescriptorAggregation;
class I_DDL;
class I_SystemQuery;
class I_SearchInterface;


class SystemABC;
class DDLScriptABC;
class ParserABC;
class DescriptorABC;
class DescriptionSchemeABC;
class DDLABC;
class DescriptionCodingEngineABC;
class DescriptorExtractorABC;
class EncoderABC;
class DecoderABC;
class ClientApplicationABC;
class ServerApplicationABC;
class SearchABC;

class System;
class DDLScript;
class Parser;
class Descriptor;
class DescriptionScheme;
class DDL;
class DescriptionCodingEngine;
class DescriptorExtractor;
class Encoder;
class Decoder;
class ClientApplication;
class ServerApplication;
class Search;
class EvaluationTool;

class XMStream;
class Buffer;

class MediaDescriptor;
class Feature;
};

#endif //__XMTYPES_H__
