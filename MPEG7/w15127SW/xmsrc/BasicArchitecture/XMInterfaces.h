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
// XMInterfaces.h


#ifndef __XMINTERFACES_H__
#define __XMINTERFACES_H__
//## begin module.includes preserve=yes

#include "BasicArchitecture/XMTypes.h"
#include "BasicArchitecture/XMABCInterfaces.h"
#include "Utilities/UUIDs.h"  
#include "Utilities/Streams.h" 


#include <vector>

namespace XM
{
//=============================================================================
class I_StreamingComponentControl: public I_StreamingComponentControlABC
{
public:
	I_StreamingComponentControl();
	virtual ~I_StreamingComponentControl();

	virtual unsigned long Open();
	virtual unsigned long Start();
	virtual unsigned long Stop();
	virtual unsigned long Close();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:

};


//=============================================================================
class I_ExtractionInterface: public I_ExtractionInterfaceABC
{
public:
	I_ExtractionInterface(DescriptorExtractor *);
	virtual ~I_ExtractionInterface();


	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

//	virtual unsigned long SetSourceMedia(MediaDescriptor&, XMStream& mediaitself);

	virtual unsigned long ExtractFeature(Descriptor* aDesc, ExtractionResult& aResult);
	
	virtual unsigned long StartExtracting(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);


	static const UUID myID;
	static const char *myName;

private:
	DescriptorExtractor *m_Extractor;
};

//=============================================================================
class I_DescriptionCodingInterface: public I_DescriptionCodingInterfaceABC
{
public:
	I_DescriptionCodingInterface(DescriptionCodingEngine *);
	virtual ~I_DescriptionCodingInterface();

	
	virtual unsigned long SetDescriptor(Descriptor *aDesc);
	// input:  description must be present (using SetDescription)
	// returns -1 if the coding scheme cannot code this descriptor
	virtual unsigned long CodeDescriptor(std::vector<unsigned char>& outBuf);

	virtual unsigned long SetDescriptionScheme(DescriptionScheme *aDS);
	
	// returns -1 if the coding scheme cannot code this scheme
	virtual unsigned long CodeDescriptionScheme(std::vector<unsigned char>& outBuf);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

private:
	DescriptionCodingEngine *m_Engine;
};

//=============================================================================
class I_ClientApplication: public I_ClientApplicationABC
{
public:
	I_ClientApplication(ClientApplication *anApp);
	virtual ~I_ClientApplication();
	
	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	ClientApplication *m_Application;
};

//=============================================================================
class I_ServerApplication: public I_ServerApplicationABC
{
public:
	I_ServerApplication(ServerApplication *);
	virtual ~I_ServerApplication();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	ServerApplication *m_Application;

};


//=============================================================================
class I_ParserInterface: public I_ParserInterfaceABC
{
public:
	I_ParserInterface(Parser *aParser);
	virtual ~I_ParserInterface();

	virtual bool PushesTowardClient(void);

	virtual Descriptor* FindDescriptor(UUID& anID);
	virtual DescriptionScheme* FindDescriptionScheme(UUID& anID);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	Parser *m_Parser;

};

//=============================================================================
class I_EncoderInterface: public I_EncoderInterfaceABC
{
public:
	I_EncoderInterface(Encoder *aEncoder);
	virtual ~I_EncoderInterface();
	
	// an Encoder takes descriptions and media and forms a stream / buffer
	//
	virtual unsigned long CodeDescriptor(Descriptor *aDesc);
	virtual unsigned long CodeDescriptionScheme(DescriptionScheme *aDS);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	Encoder *m_Encoder;
};

//=============================================================================
class I_DecoderInterface: public I_DecoderInterfaceABC
{
public:
	I_DecoderInterface(Decoder *aDecoder);
	virtual ~I_DecoderInterface();

	virtual unsigned long GetNextDescriptionScheme(DescriptionSchemeABC& aDesc);
	virtual unsigned long GetNextDescription(DescriptorABC& aD);

	virtual bool PushesTowardClient(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

private:
	Decoder *m_Decoder;
};

//=============================================================================
class I_DescriptorAggregation: public I_DescriptorAggregationABC
{
public:
	I_DescriptorAggregation(DescriptionScheme *aScheme);
	virtual ~I_DescriptorAggregation();

	virtual std::vector<Descriptor>& GetDescriptors(void);
	virtual unsigned long SetDescriptors(std::vector<Descriptor>);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

private:
	DescriptionScheme *m_Scheme;

};

//=============================================================================
class I_ComponentInfoD: public I_ComponentInfoABC
{
public:
	I_ComponentInfoD(Descriptor *aDesc);
	virtual ~I_ComponentInfoD();

	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	Descriptor *m_D;

};

//=============================================================================
class I_ComponentInfoDS: public I_ComponentInfoABC
{
public:
	I_ComponentInfoDS(DescriptionScheme *aScheme);
	virtual ~I_ComponentInfoDS();

	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	DescriptionScheme *m_DS;

};
//=============================================================================
// Identifiers are very primitive entitites which define semantics of D's.
// Most MPEG-7 identifiers will be little more than DescriptorIdentifiers with
// different UUIDs  The DDL however has the ability to define and extend
// the identifier.  In the descriptor given in requirements, the identifier would
class I_DescriptorIdentifier: public I_DescriptorIdentifierABC
{
public:
	I_DescriptorIdentifier(Descriptor* anIdent);
	virtual ~I_DescriptorIdentifier();

	virtual unsigned long DescribeIdentifer(DDLScript& desc);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	Descriptor *m_Descriptor;
};

//=============================================================================
// The instantiation of the D is only achieved after the value of the
// attributes of this class have been assigned.  In the descriptor given in
// requirements, the value would be [int, int, int].  As with the attribute the
// DDL has the ability to create and extend DescriptorValues using DDLScript.
class I_DescriptorValue: public I_DescriptorValueABC
{
public:
	I_DescriptorValue(Descriptor *anIdent);
	virtual ~I_DescriptorValue();

	virtual unsigned long DescribeValue(DDLScript& desc);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

private:
	Descriptor *m_Descriptor;
	
};

//=============================================================================
class I_SystemBackEnd: public I_SystemBackEndABC
{
public:
	I_SystemBackEnd(System *aSys);
	virtual ~I_SystemBackEnd();
	// is the system push or pull (push systems are defined where
	// data is pushed into the parser/decoder

	virtual bool PushesTowardClient(void);
//	virtual XMStream& GetOutputStream(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

private:
	System *m_System;

};

//=============================================================================
class I_SystemFrontEnd: public I_SystemFrontEndABC
{
public:
	I_SystemFrontEnd(System *aSys);
	virtual ~I_SystemFrontEnd();

//	virtual void SetInputStream(XMStream& inStream);
	virtual unsigned long PushBuffer(std::vector<unsigned char>& aBuff);


	// is the system push or pull (push systems are defined where
	// data is pushed into the parser/decoder

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	System *m_System;
};

//=============================================================================
class I_SearchInterface: public I_SearchInterfaceABC
{
public:
	I_SearchInterface(Search *aSearch);
	virtual ~I_SearchInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	Search *m_Search;
	
};


//=============================================================================
class I_SystemQuery: public I_SystemQueryABC
{
public:
	I_SystemQuery(System *);
	virtual ~I_SystemQuery();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	System *m_System;

};

//=============================================================================
class I_DDL: public I_DDLABC
{
public:
	I_DDL(DDL* aDDL);
	virtual ~I_DDL();

	virtual void DescribeDescriptor(Descriptor&);
	virtual void DescribeScheme(DescriptionScheme&);
	virtual DDLScript& GetDDLScript(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	
	static const UUID myID;
	static const char *myName;
private:
	DDL *m_DDL;
};

//=============================================================================
class I_ServerComponentControl: public I_StreamingComponentControl
{
public:
	I_ServerComponentControl(ServerApplication * app);
	virtual ~I_ServerComponentControl();

	virtual unsigned long Open();
	virtual unsigned long Start();
	virtual unsigned long Stop();
	virtual unsigned long Close();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	ServerApplication *m_Server;
};

//=============================================================================
class I_ClientComponentControl: public I_StreamingComponentControl
{
public:
	I_ClientComponentControl(ClientApplication *app);
	virtual ~I_ClientComponentControl();

	virtual unsigned long Open();
	virtual unsigned long Start();
	virtual unsigned long Stop();
	virtual unsigned long Close();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	ClientApplication *m_Client;
};

//=============================================================================
class I_EvaluationInterface: public I_EvaluationInterfaceABC
{
public:
	I_EvaluationInterface(EvaluationTool *tool);
	virtual ~I_EvaluationInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
private:
	EvaluationTool *m_EvalTool;

};

};


#endif //__XMINTERFACES_H__
