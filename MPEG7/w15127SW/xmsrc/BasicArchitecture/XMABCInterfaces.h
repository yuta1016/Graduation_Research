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
// XMABCInterfaces.h


#ifndef __XMABCINTERFACES_H__
#define __XMABCINTERFACES_H__
//## begin module.includes preserve=yes

#include "XMTypes.h"
#include "Utilities/UUIDs.h"
//#include "Utilities/Streams.h" 
#include <vector>

namespace XM
{


//=============================================================================
class I_InterfaceABC
{
public:
	I_InterfaceABC();
	virtual ~I_InterfaceABC() {};
	// An interface may have one or more interface ID's, to distinguish various
	// specializations of an interface.

	// this function is not guaranteed to return interfaces ordered according
	// to the inheritance hierarchy!  It is also not guaranteed to return 
	// a single ID per object (subclassed objects may very well be associated with
	// multiple ID's, through Is-A relationships.
	virtual std::vector<UUID> GetInterfaceIDs(void);

	// get the ID of this object
	virtual const UUID& GetInterfaceID(void) = 0;
	virtual const char *GetName(void) = 0;

	static const UUID myID;
	static const char *myName;

protected:	
	// all derived classes SHALL call this function.
	virtual void SetInterfaceID(const UUID& anID);

private:
	std::vector<UUID> containedInterfaces;

};

//=============================================================================
class I_StreamingComponentControlABC: public I_InterfaceABC
{
public:
	I_StreamingComponentControlABC();
	virtual ~I_StreamingComponentControlABC() {}

	virtual unsigned long Open() = 0;
	virtual unsigned long Start() = 0;
	virtual unsigned long Stop() = 0;
	virtual unsigned long Close() = 0;

	static const UUID myID;
	static const char *myName;


};

//=============================================================================
class I_ExtractionInterfaceABC: public I_InterfaceABC
{
public:
	I_ExtractionInterfaceABC();
	virtual ~I_ExtractionInterfaceABC() {}
	//result has one of the following values
	enum ExtractionResult
	{
		ExtractionResultSINGLEFOUND = 0,
		ExtractionResultMOREFOUND = 1,
		ExtractionResultNONEFOUND = 2
	};

	virtual bool SupportsPush(void) = 0;
	virtual bool SupportsPull(void) = 0;

	
//	virtual unsigned long SetSourceMedia(MediaDescriptor&, XMStream& mediaitself) = 0;
	
	// Extraction engines are either push or pull (either we query with a specific 
	// descriptor and wait till it is found) or we query for all the descriptors and
	// wait till they are found.  
	virtual unsigned long ExtractFeature(Descriptor* aDesc, ExtractionResult& aResult) = 0;
	virtual unsigned long StartExtracting(void) = 0;

	static const UUID myID;
	static const char *myName;

};

//=============================================================================
class I_DescriptionCodingInterfaceABC: public I_InterfaceABC
{
public:
	I_DescriptionCodingInterfaceABC();
	virtual ~I_DescriptionCodingInterfaceABC() {}
	
	virtual unsigned long SetDescriptor(Descriptor *aDesc) = 0;
	// input:  description must be present (using SetDescription)
	// returns -1 if the coding scheme cannot code this descriptor
	virtual unsigned long CodeDescriptor(std::vector<unsigned char>& outBuf) = 0;

	virtual unsigned long SetDescriptionScheme(DescriptionScheme *aDS) = 0;
	
	// returns -1 if the coding scheme cannot code this scheme
	virtual unsigned long CodeDescriptionScheme(std::vector<unsigned char>& outBuf) = 0;

	static const UUID myID;
	static const char *myName;

};

//=============================================================================
class I_ClientApplicationABC: public I_InterfaceABC
{
public:
	I_ClientApplicationABC();
	virtual ~I_ClientApplicationABC() {}
	
	virtual bool SupportsPush(void) = 0;
	virtual bool SupportsPull(void) = 0;
	
	static const UUID myID;
	static const char *myName;

};

//=============================================================================
class I_ServerApplicationABC: public I_InterfaceABC
{
public:
	I_ServerApplicationABC();
	virtual ~I_ServerApplicationABC() {}

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_ParserInterfaceABC: public I_InterfaceABC
{
public:
	I_ParserInterfaceABC();
	virtual ~I_ParserInterfaceABC() {}

	virtual Descriptor* FindDescriptor(UUID& anID) = 0;
	virtual DescriptionScheme* FindDescriptionScheme(UUID& anID) = 0;

	virtual bool PushesTowardClient(void) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_EncoderInterfaceABC: public I_InterfaceABC
{
public:
	// an Encoder takes descriptions and media and forms a stream / buffer
	// 
	I_EncoderInterfaceABC();
	virtual ~I_EncoderInterfaceABC() {}

	virtual unsigned long CodeDescriptor(Descriptor *aDesc) = 0;
	virtual unsigned long CodeDescriptionScheme(DescriptionScheme *aDS) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_DecoderInterfaceABC: public I_InterfaceABC
{
public:
	I_DecoderInterfaceABC();
	virtual ~I_DecoderInterfaceABC() {}

	virtual unsigned long GetNextDescriptionScheme(DescriptionSchemeABC& aDesc) = 0;
	virtual unsigned long GetNextDescription(DescriptorABC& aD) = 0;

	virtual bool PushesTowardClient(void) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_DescriptorAggregationABC: public I_InterfaceABC
{
public:
	I_DescriptorAggregationABC();
	virtual ~I_DescriptorAggregationABC() {}

	virtual std::vector<Descriptor>& GetDescriptors(void) = 0;
	virtual unsigned long SetDescriptors(std::vector<Descriptor>) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_ComponentInfoABC: public I_InterfaceABC
{
public:
	I_ComponentInfoABC();
	virtual ~I_ComponentInfoABC() {}

	virtual bool IsProprietary(void) = 0;
	virtual const UUID& GetValueID(void) = 0;
	virtual const char* GetValueName(void) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
// Identifiers are very primitive entitites which define semantics of D's.
// Most MPEG-7 identifiers will be little more than DescriptorIdentifiers with
// different UUIDs  The DDL however has the ability to define and extend
// the identifier.  In the descriptor given in requirements, the identifier would
class I_DescriptorIdentifierABC: public I_InterfaceABC
{
public:
	I_DescriptorIdentifierABC();
	virtual ~I_DescriptorIdentifierABC() {}

	virtual unsigned long DescribeIdentifer(DDLScript& desc) = 0;
	static const UUID myID;
	static const char *myName;
};

//=============================================================================
// The instantiation of the D is only achieved after the value of the
// attributes of this class have been assigned.  In the descriptor given in
// requirements, the value would be [int, int, int].  As with the attribute the
// DDL has the ability to create and extend DescriptorValues using DDLScript.
class I_DescriptorValueABC: public I_InterfaceABC
{
public:
	I_DescriptorValueABC();
	virtual ~I_DescriptorValueABC() {}
	virtual unsigned long DescribeValue(DDLScript& desc) = 0;	
	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_SystemBackEndABC: public I_InterfaceABC
{
public:
	// is the system push or pull (push systems are defined where
	// data is pushed into the parser/decoder
	I_SystemBackEndABC();
	virtual ~I_SystemBackEndABC() {}

	virtual bool PushesTowardClient(void) = 0;
//	virtual XMStream& GetOutputStream(void) = 0;

	static const UUID myID;
	static const char *myName;
};


//=============================================================================
class I_SystemQueryABC: public I_InterfaceABC
{
public:
	I_SystemQueryABC();
	virtual ~I_SystemQueryABC() {}
	static const UUID myID;
	static const char *myName;

};

//=============================================================================
class I_SystemFrontEndABC: public I_InterfaceABC
{
public:
	// is the system push or pull (push systems are defined where
	// data is pushed into the parser/decoder
	I_SystemFrontEndABC();
	virtual ~I_SystemFrontEndABC() {}

//	virtual void SetInputStream(XMStream& inStream) = 0;
	virtual unsigned long PushBuffer(std::vector<unsigned char>& aBuff) = 0;
	static const UUID myID;
	static const char *myName;

};

//=============================================================================
class I_SearchInterfaceABC: public I_InterfaceABC
{
public:
	I_SearchInterfaceABC();
	virtual ~I_SearchInterfaceABC() {}

	static const UUID myID;
	static const char *myName;
	
};

//=============================================================================
class I_DDLABC: public I_InterfaceABC
{
public:
	I_DDLABC();
	virtual ~I_DDLABC() {}

	virtual void DescribeDescriptor(Descriptor&) = 0;
	virtual void DescribeScheme(DescriptionScheme&) = 0;
	virtual DDLScript& GetDDLScript(void) = 0;

	static const UUID myID;
	static const char *myName;
};

//=============================================================================
class I_EvaluationInterfaceABC: public I_InterfaceABC
{
public:
	I_EvaluationInterfaceABC();
	virtual ~I_EvaluationInterfaceABC() {};

	static const UUID myID;
	static const char *myName;

};

};


#endif //__XMABCINTERFACES_H__
