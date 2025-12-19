///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// TU-Munich, S. Herrmann
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
// Applicable File Name:  ColorQuant.h
#ifndef __ColorQuant__
#define __ColorQuant__

#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

class ColorQuantizerDescriptor;
class ColorQuantizerDescriptorInterface;

//=============================================================================
class ColorQuantizerDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ColorQuantizerDescriptorInterfaceABC();
	virtual ~ColorQuantizerDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual int GetComponent(int order) = 0;
	virtual int SetComponent(int order, int component) = 0;
	virtual int GetBinNumber(int order) = 0;
	virtual int SetBinNumber(int order, int bin_number) = 0;
	virtual int GetBinNumberByComponent(int component) = 0;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

	static const UUID myID;
};

class ColorQuantizerDescriptor;

//=============================================================================
class ColorQuantizerDescriptorInterface: public ColorQuantizerDescriptorInterfaceABC
{
public:
	ColorQuantizerDescriptorInterface(ColorQuantizerDescriptor
						*aQuant);
	virtual ~ColorQuantizerDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual int GetComponent(int order);
	virtual int SetComponent(int order, int component);
	virtual int GetBinNumber(int order);
	virtual int SetBinNumber(int order, int bin_number);
	virtual int GetBinNumberByComponent(int component);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	static const UUID myID;
	static const char *myName;


private:
	ColorQuantizerDescriptor *m_ColorQuantizerDescriptor;	
};

//=============================================================================
class ColorQuantizerDescriptor: public Descriptor
{
friend class ColorQuantizerDescriptorInterface;
public:
	ColorQuantizerDescriptor();

	// actual descriptor methods
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

	virtual ColorQuantizerDescriptorInterfaceABC *GetInterface(void);

	virtual void addref();
	virtual void release();

	// accessor methods
	virtual int GetComponent(int order);
	virtual int SetComponent(int order, int component);
	virtual int GetBinNumber(int order);
	virtual int SetBinNumber(int order, int bin_number);
	virtual int GetBinNumberByComponent(int component);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	virtual ~ColorQuantizerDescriptor();

	unsigned long m_refcount;

	ColorQuantizerDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data

	int m_Component[3];
	int m_BinNumber[3];
};

};

#endif //__ColorQuant__
