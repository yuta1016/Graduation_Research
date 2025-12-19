////////////////////////////////////////////////////////////////////////
//
// ColorStructure.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#ifndef __COLORSTRUCTURE_H__
#define __COLORSTRUCTURE_H__

#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

//=============================================================================
class ColorStructureDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ColorStructureDescriptorInterfaceABC();
	virtual ~ColorStructureDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;
	
	virtual unsigned long GetSize(void) = 0;
	virtual unsigned long SetSize(unsigned long size) = 0;	
	virtual unsigned long GetElement(unsigned long index) = 0;
	virtual unsigned long SetElement(unsigned long index, int value) = 0;

	static const UUID myID;

	virtual unsigned long
		ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class ColorStructureDescriptor;

//=============================================================================
class ColorStructureDescriptorInterface: public ColorStructureDescriptorInterfaceABC
{
public:
	ColorStructureDescriptorInterface(ColorStructureDescriptor
						 *aColorStructureDescriptor);
	virtual ~ColorStructureDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long GetSize(void);
	virtual unsigned long SetSize(unsigned long size);	
	virtual unsigned long GetElement(unsigned long index);
	virtual unsigned long SetElement(unsigned long index, int value);

	virtual unsigned long
		ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	static const UUID myID;
	static const char *myName;

private:
	ColorStructureDescriptor *m_Descriptor;
};

//=============================================================================
class ColorStructureDescriptor: public Descriptor
{
friend class ColorStructureDescriptorInterface;
public:
	ColorStructureDescriptor();

 // actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long GetSize(void);
	virtual unsigned long SetSize(unsigned long size);	
	virtual unsigned long GetElement(unsigned long index);
	virtual unsigned long SetElement(unsigned long index, int value);

	virtual unsigned long
		ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ColorStructureDescriptorInterfaceABC *GetInterface(void);

private:
	virtual ~ColorStructureDescriptor();
	
	unsigned long m_refcount;

	ColorStructureDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	unsigned long *m_Data;
	unsigned long m_Size;
};

};

#endif //__COLORSTRUCTURE_H__
