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
// Applicable File Name:  XMObjects.h
//

#ifndef __XMOBJECTS_H__
#define __XMOBJECTS_H__
//## begin module.includes preserve=yes

#include "BasicArchitecture/XMABCObjects.h"
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMTypes.h"
#include "Utilities/UUIDs.h"   
//#include "Utilities/Streams.h"


namespace XM
{

///////////////////////////////////////////////////////////////////////////////
// About this file
//
// Classes in this unit are designed for a couple of purposes;
// 1.  To act as "prototypical" implementations of the absstract XM objects
// which will serve as an implementation guide for XM implementers
// 2.  To act as test modules to verify compilability of the XM Architecture
// 3.  To act as test modules to verify connectivity within the XM framework
// 4.  To act as default implementations of certain objects (D's, DS's) in their
// proprietary, DDL defined forms.

///////////////////////////////////////////////////////////////////////////////
//
class DDLScript: public DDLScriptABC
{
public:
	DDLScript();
	virtual ~DDLScript();

	virtual std::basic_string<char>& GetDDLRepresentation(void);
	virtual unsigned long SetDDLRepresentation(std::basic_string<char>&);
private:
	std::basic_string<char> m_script;
};


//=============================================================================
// Descriptor is an XMObject that consists of an Attribute-value pair
// (here a DescriptorIdentifier-DescriptorValue pair).
//=============================================================================
class Descriptor: public DescriptorABC
{
public:
	Descriptor();
	virtual ~Descriptor();

	Descriptor& operator=(const Descriptor& rhs);

	// ABC overrides
	virtual I_DescriptorIdentifierABC *GetIdentifier(void);
	virtual I_DescriptorValueABC *GetValue(void);
	virtual I_ComponentInfoABC *GetInfo(void);

	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	// actual descriptor methods
	virtual unsigned long DescribeIdentifier(DDLScript& desc);
	virtual unsigned long DescribeValue(DDLScript& desc);
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	I_DescriptorIdentifier m_IdentIntfc;
	I_DescriptorValue m_ValueInterface;
	I_ComponentInfoD m_InfoInterface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	DDLScript m_Desc;


	// information about the class
};

//=============================================================================
class DescriptionScheme: public DescriptionSchemeABC
{
public:
	DescriptionScheme();
	virtual ~DescriptionScheme();
	
	DescriptionScheme& operator=(const DescriptionScheme& rhs);

	virtual I_DescriptorAggregationABC *GetDescriptorIntfc(void);
	virtual I_ComponentInfoABC *GetInfo(void);

	virtual std::vector<Descriptor>& GetDescriptors(void);
	virtual unsigned long SetDescriptors(std::vector<Descriptor>);
	
	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	bool IsProprietary(void);

	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	I_DescriptorAggregation m_DescriptionInterface;
	I_ComponentInfoDS m_ComponentInterface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	std::vector<Descriptor> m_AggregatedDescriptors;

	DDLScript m_DescScheme;

};

//=============================================================================
class System: public SystemABC
{
public:
	System();
	virtual ~System();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	virtual I_SystemBackEndABC* GetClientInterface(void);
	virtual I_SystemFrontEndABC* GetServerInterface(void);
	virtual I_SystemQuery *GetSystemQuery(void);

//	virtual void SetInputStream(XMStream& inStream);
	unsigned long PushBuffer(std::vector<unsigned char>& aBuff);

	virtual bool PushesTowardClient(void);
//	virtual XMStream& GetOutputStream(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:

	virtual I_EncoderInterfaceABC *GetEncoderInterface();
	virtual I_ParserInterfaceABC *GetParserInterface();

private:
	I_SystemBackEnd m_SystemBackEndInterface;
	I_SystemFrontEnd m_SystemFrontEndInterface;
	I_SystemQuery m_SystemQuery;
	I_EncoderInterfaceABC *m_encInterface;
	I_ParserInterfaceABC *m_ParserInterface;
//	XMStream m_outStream;
//	XMStream *m_inStream;
};

//=============================================================================
class DDL: public DDLABC
{
public:
	DDL();
	virtual ~DDL();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	virtual I_DDLABC* GetDDLInterface(void);
	
	virtual void DescribeDescriptor(Descriptor&);
	virtual void DescribeScheme(DescriptionScheme&);
	virtual DDLScript& GetDDLScript(void);


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

private:
	I_DDL m_DDLInterface;
	DDLScript m_Script;
};

//=============================================================================
class DescriptionCodingEngine: public DescriptionCodingEngineABC
{
public:
	DescriptionCodingEngine();

	virtual ~DescriptionCodingEngine();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	virtual unsigned long SetDescriptor(Descriptor *aDesc);
	// input:  description must be present (using SetDescription)
	// returns -1 if the coding scheme cannot code this descriptor
	virtual unsigned long CodeDescriptor(std::vector<unsigned char>& outBuf);

	virtual unsigned long SetDescriptionScheme(DescriptionScheme *aDS);
	
	// returns -1 if the coding scheme cannot code this scheme
	virtual unsigned long CodeDescriptionScheme(std::vector<unsigned char>& outBuf);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual I_DescriptionCodingInterfaceABC *GetCodingInterface(void);

protected:
	virtual I_EncoderInterfaceABC *GetEncoderInterface();
private:
	
	I_DescriptionCodingInterface m_CodingInterface;
	I_EncoderInterfaceABC *m_encInterface;
};

//=============================================================================
class DescriptorExtractor: public DescriptorExtractorABC
{
public:
	DescriptorExtractor();
	virtual ~DescriptorExtractor();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual I_ExtractionInterfaceABC* GetExtractionInterface(void);
	
	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

//	virtual unsigned long SetSourceMedia(MediaDescriptor&, XMStream& mediaitself);

	virtual unsigned long ExtractFeature(Descriptor* aDesc, I_ExtractionInterfaceABC::ExtractionResult& aResult);
	
	virtual unsigned long StartExtracting(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

private:
	I_ExtractionInterface m_ExtractionInterface;
};

//=============================================================================
class Encoder: public EncoderABC
{
public:
	Encoder();
	virtual ~Encoder();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);
	
	virtual I_EncoderInterfaceABC* GetInterface(void);
	
	virtual unsigned long CodeDescriptor(Descriptor *aDesc);
	virtual unsigned long CodeDescriptionScheme(DescriptionScheme *aDS);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:
	virtual I_SystemFrontEndABC *GetSystemFrontEnd();
	virtual I_DescriptionCodingInterfaceABC *GetCodingInterface();
	virtual I_DDLABC *GetDDL();

private:

	I_EncoderInterface m_EncoderInterface;
	
	I_SystemFrontEndABC *m_SystemFrontEnd;
	I_DescriptionCodingInterfaceABC *m_CodingInterface;
	I_DDLABC *m_DDL;
};

//=============================================================================
class Parser: public ParserABC
{
public:
	Parser();
	virtual ~Parser();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool ReceiveThisInterface(I_InterfaceABC *);
	virtual I_ParserInterfaceABC* GetInterface(void);

	virtual Descriptor* FindDescriptor(UUID& anID);
	virtual DescriptionScheme* FindDescriptionScheme(UUID& anID);
	virtual bool PushesTowardClient(void);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:
	virtual I_SystemBackEndABC *GetSystemBackEnd();

private:
	I_ParserInterface m_ParserInterface;
	I_SystemBackEndABC *m_SystemBackEnd;
};


//=============================================================================
class Decoder: public DecoderABC
{
public:
	Decoder();
	virtual ~Decoder();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);
	virtual I_DecoderInterfaceABC* GetInterface(void);

	virtual bool PushesTowardClient(void);

	virtual unsigned long GetNextDescriptionScheme(DescriptionSchemeABC& aDesc);
	virtual unsigned long GetNextDescription(DescriptorABC& aD);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:
	virtual I_ParserInterfaceABC *GetParserInterface();
private:
	I_DecoderInterface m_DecoderInterface;
	I_ParserInterfaceABC *m_ParserInterface;

};

//=============================================================================
class Search: public SearchABC
{
public:
	Search();
	virtual ~Search();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);
	virtual I_SearchInterfaceABC* GetSearchInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:
	virtual I_SystemQueryABC *GetSystemSearch(void);

private:
	I_SearchInterface m_SearchInterface;
	I_SystemQueryABC *m_SystemSearch;

};


//=============================================================================
class ClientApplication: public ClientApplicationABC
{
public:
	ClientApplication();
	virtual ~ClientApplication();

	virtual I_ClientApplicationABC* GetAppInterface(void);
	virtual I_StreamingComponentControlABC* GetStreamingControl(void);


	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);


	virtual unsigned long Open();
	virtual unsigned long Start();
	virtual unsigned long Stop();
	virtual unsigned long Close();

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
protected:
	virtual I_SearchInterfaceABC *GetSearchInterface();

private:
	I_ClientApplication m_ApplicationIntfc;
	I_ClientComponentControl m_ComponentControl;

	I_SearchInterfaceABC *m_SearchInterface;

};

//=============================================================================
class ServerApplication: public ServerApplicationABC
{
public:
	ServerApplication();
	virtual ~ServerApplication();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);
	virtual I_ServerApplicationABC* GetAppInterface(void);
	virtual I_StreamingComponentControlABC* GetStreamingControl(void);

	virtual unsigned long Open();
	virtual unsigned long Start();
	virtual unsigned long Stop();
	virtual unsigned long Close();

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

protected:
	virtual I_EncoderInterfaceABC *GetEncoderInterface(void);
	virtual I_ExtractionInterfaceABC *GetExtractionInterface(void);

private:
	I_ServerApplication m_ApplicationIntfc;
	I_ServerComponentControl m_ComponentControl;

	I_EncoderInterfaceABC *m_EncoderInterface;
	I_ExtractionInterfaceABC *m_ExtractionInterface;

};


//=============================================================================
class EvaluationTool: public EvaluationToolABC
{
public:
	EvaluationTool();
	virtual ~EvaluationTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool ReceiveThisInterface(I_InterfaceABC *);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;


};

};

#endif //_XMOBJECTS_H__
