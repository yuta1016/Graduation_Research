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
// Applicable File Name:  EvaluationTools.h
//
#ifndef __EVALUATION_TOOLS_H__
#define __EVALUATION_TOOLS_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "../Utilities/UUIDs.h"   
namespace XM
{

//=============================================================================
class P564CodedSizeInterfaceABC: public I_InterfaceABC
{
public:
	P564CodedSizeInterfaceABC();
	virtual ~P564CodedSizeInterfaceABC() {}

	virtual std::vector<unsigned long> IdentifySizes() = 0;
	
	static const UUID myID;
};

class P564CodedSize;

//=============================================================================
class SystemFrontEndProxy: public I_SystemFrontEndABC
{
public:
	SystemFrontEndProxy(P564CodedSize* aCodedSizeEvaluator);
	virtual ~SystemFrontEndProxy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void SetInputStream(XMStream& inStream);
	virtual unsigned long PushBuffer(std::vector<unsigned char>& aBuff);

	
	static const UUID myID;
	static const char * myName;

private:
	P564CodedSize *m_CodedSize;

};

//=============================================================================
class P564CodedSizeInterface: public P564CodedSizeInterfaceABC
{
public:
	P564CodedSizeInterface(P564CodedSize* aCodedSizeEvaluator);
	virtual ~P564CodedSizeInterface();
	
	virtual std::vector<unsigned long> IdentifySizes();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	
	static const UUID myID;
	static const char * myName;
private:
	P564CodedSize *m_CodedSize;
};

//=============================================================================
class P564CodedSize: public EvaluationTool
{
public:
	P564CodedSize();
	virtual ~P564CodedSize();

	virtual P564CodedSizeInterfaceABC *GetSizeInterface();
	virtual I_SystemFrontEndABC *GetSystemFrontEndInterface();

	
	virtual void SetInputStream(XMStream& inStream);
	virtual unsigned long PushBuffer(std::vector<unsigned char>& aBuff);
	virtual std::vector<unsigned long> IdentifySizes();

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
	P564CodedSizeInterface m_SizeInterface;
	SystemFrontEndProxy m_SysFrontInterface;

	std::vector<unsigned long> m_bufferSizes;

};

};

#endif //__EVALUATION_TOOLS_H__
