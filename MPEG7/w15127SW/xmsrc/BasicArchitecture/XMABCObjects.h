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
// Applicable File Name:  XMABCObjects.h
//

#ifndef __XMABCOBJECTS_H__
#define __XMABCOBJECTS_H__
//## begin module.includes preserve=yes

#include "BasicArchitecture/XMInterfaces.h"
#include "XMTypes.h"

#include <vector>
#include <string>

namespace XM
{

//=============================================================================
// If the DDL is an XML derivative the script will be based on some text.
// N2ote - this script will change and is provided for completeness only!
class DDLScriptABC
{
public:
	DDLScriptABC();
	virtual ~DDLScriptABC();
	virtual std::basic_string<char>& GetDDLRepresentation(void) = 0;
	virtual unsigned long SetDDLRepresentation(std::basic_string<char>&) = 0;
	// maybe our DDL object should be expected to do checking on 
	// submitted DDL syntax? 
};

//=============================================================================
class XMObject
{
public:
	XMObject();
	virtual ~XMObject();

	virtual const UUID& GetObjectID(void) = 0;
	virtual const char *GetName(void) = 0;
};

//=============================================================================
class ConnectibleObject: public XMObject
{	
public:
	ConnectibleObject();
	virtual ~ConnectibleObject();

	// the connectibleObjecdt MAY receive this interface
	virtual unsigned long AttachInterface(I_InterfaceABC *);
	virtual std::vector<I_InterfaceABC*> GetExposedInterfaces(void);

protected:
	// A derived interface SHALL call this method for each of the
	// interface types it supports.
	virtual void SetInputInterfaces(const UUID&);

	virtual std::vector<UUID> GetInputInterfaces(void);

	// the connectibleObject SHALL receive this interface
	virtual bool ReceiveThisInterface(I_InterfaceABC *) = 0;
	
	 // A derived interface SHALL call this method on initialization
	virtual void SetExposedInterface(I_InterfaceABC* );

private:
	
	std::vector< std::pair<UUID,bool> > m_InputInterfaces;
	std::vector< I_InterfaceABC *> m_ExposedInterfaces;

};

//=============================================================================
// Descriptor is an XMObject that consists of an Attribute-value pair
// (here a DescriptorIdentifier-DescriptorValue pair).
//=============================================================================
class DescriptorABC: public ConnectibleObject
{
public:
	DescriptorABC();
	virtual ~DescriptorABC();

	virtual I_DescriptorIdentifierABC *GetIdentifier(void) = 0;
	virtual I_DescriptorValueABC *GetValue(void) = 0;
	virtual I_ComponentInfoABC *GetInfo(void) = 0;

};

//=============================================================================
class DescriptionSchemeABC: public ConnectibleObject
{
public:
	DescriptionSchemeABC();
	virtual ~DescriptionSchemeABC();

	virtual I_DescriptorAggregationABC *GetDescriptorIntfc(void) = 0;
	virtual I_ComponentInfoABC *GetInfo(void) = 0;
};

//=============================================================================
class SystemABC: public ConnectibleObject
{
public:
	SystemABC();
	virtual ~SystemABC();

	virtual I_SystemBackEndABC* GetClientInterface(void) = 0;
	virtual I_SystemFrontEndABC* GetServerInterface(void) = 0;
	virtual I_SystemQuery *GetSystemQuery(void) = 0;

};

//=============================================================================
class DDLABC: public ConnectibleObject
{
public:
	DDLABC();
	virtual ~DDLABC();

	virtual I_DDLABC* GetDDLInterface(void) = 0;
};

//=============================================================================
class DescriptionCodingEngineABC: public ConnectibleObject
{
public:

	DescriptionCodingEngineABC();
	virtual ~DescriptionCodingEngineABC();
	
	virtual I_DescriptionCodingInterfaceABC *GetCodingInterface(void) = 0;

};

//=============================================================================
class DescriptorExtractorABC: public ConnectibleObject
{
public:
	DescriptorExtractorABC();
	virtual ~DescriptorExtractorABC();

	virtual I_ExtractionInterfaceABC* GetExtractionInterface(void) = 0;

};

//=============================================================================
class EncoderABC: public ConnectibleObject
{
public:
	EncoderABC();
	virtual ~EncoderABC();

	virtual I_EncoderInterfaceABC* GetInterface(void) = 0;
	
};

//=============================================================================
class ParserABC: public ConnectibleObject
{
public:
	ParserABC();
	virtual ~ParserABC();

	virtual I_ParserInterfaceABC* GetInterface(void) = 0;
	
};

//=============================================================================
class DecoderABC: public ConnectibleObject
{
public:
	DecoderABC();
	virtual ~DecoderABC();

	virtual I_DecoderInterfaceABC* GetInterface(void) = 0;
};

//=============================================================================
class SearchABC: public ConnectibleObject
{
public:
	SearchABC();
	virtual ~SearchABC();

	virtual I_SearchInterfaceABC* GetSearchInterface(void) = 0;
};

//=============================================================================
class ClientApplicationABC: public ConnectibleObject
{
public:
	ClientApplicationABC();
	virtual ~ClientApplicationABC();

	virtual I_ClientApplicationABC* GetAppInterface(void) = 0;
	virtual I_StreamingComponentControlABC* GetStreamingControl(void) = 0;
};

//=============================================================================
class ServerApplicationABC: public ConnectibleObject
{
public:
	ServerApplicationABC();
	virtual ~ServerApplicationABC();

	virtual I_ServerApplicationABC* GetAppInterface(void) = 0;
	virtual I_StreamingComponentControlABC* GetStreamingControl(void) = 0;
};

//=============================================================================
class EvaluationToolABC: public ConnectibleObject
{
public:
	EvaluationToolABC();
	virtual ~EvaluationToolABC();
};

};

#endif //_XMABCOBJECTS_H__
