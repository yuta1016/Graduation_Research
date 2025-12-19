///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ShapeVariation.h

#ifndef __SHAPE_VARIATION_H__
#define __SHAPE_VARIATION_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

#define ANGULAR	9
#define RADIAL 	4

namespace XM
{	
//=============================================================================
class ShapeVariationDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ShapeVariationDescriptorInterfaceABC();
	virtual ~ShapeVariationDescriptorInterfaceABC() {};
	
	virtual void addref()=0;
	virtual void release()=0;
	
	static const UUID myID;		
			
	virtual bool	SetLVMElement(char p, char r, char value) = 0;
	virtual bool	SetLVMElement(char p, char r, double value) = 0;
	virtual char	GetLVMElement(char p, char r) = 0;
	virtual double	GetLVMValue(char p, char r) = 0;
	virtual bool	SetHVMElement(char p, char r, char value) = 0;
	virtual bool	SetHVMElement(char p, char r, double value) = 0;
	virtual char	GetHVMElement(char p, char r) = 0;
	virtual double	GetHVMValue(char p, char r) = 0;
	virtual bool	SetSTDElement(char p, char r, char value) = 0;
	virtual bool	SetSTDElement(char p, char r, double value) = 0;
	virtual char	GetSTDElement(char p, char r) = 0;
	virtual double	GetSTDValue(char p, char r) = 0;
	
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class ShapeVariationDescriptor;

//=============================================================================
class ShapeVariationDescriptorInterface: public ShapeVariationDescriptorInterfaceABC
{
public:
	ShapeVariationDescriptorInterface(ShapeVariationDescriptor *aShapeVariationDescriptor);
	virtual ~ShapeVariationDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	
	virtual void addref();
	virtual void release();
		
	virtual bool	SetLVMElement(char p, char r, char value);
	virtual bool	SetLVMElement(char p, char r, double value);
	virtual char	GetLVMElement(char p, char r);
	virtual double	GetLVMValue(char p, char r);
	virtual bool	SetHVMElement(char p, char r, char value);
	virtual bool	SetHVMElement(char p, char r, double value);
	virtual char	GetHVMElement(char p, char r);
	virtual double	GetHVMValue(char p, char r);
	virtual bool	SetSTDElement(char p, char r, char value);
	virtual bool	SetSTDElement(char p, char r, double value);
	virtual char	GetSTDElement(char p, char r);
	virtual double	GetSTDValue(char p, char r);
	
	
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	
	static const UUID myID;
	static const char *myName;
	
private:
	ShapeVariationDescriptor *m_Descriptor;
};

//=============================================================================
class ShapeVariationDescriptor: public Descriptor
{
friend  class ShapeVariationDescriptorInterface;
public:
	ShapeVariationDescriptor();
	
	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	
	virtual void addref();
	virtual void release();
		
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
	
	virtual ShapeVariationDescriptorInterfaceABC *GetInterface(void);

	bool	SetLVMElement(char p, char r, char value);
	bool	SetLVMElement(char p, char r, double value);
	char	GetLVMElement(char p, char r);
	double	GetLVMValue(char p, char r);
	bool	SetHVMElement(char p, char r, char value);
	bool	SetHVMElement(char p, char r, double value);
	char	GetHVMElement(char p, char r);
	double	GetHVMValue(char p, char r);
	bool	SetSTDElement(char p, char r, char value);
	bool	SetSTDElement(char p, char r, double value);
	char	GetSTDElement(char p, char r);
	double	GetSTDValue(char p, char r);

	char	Quantize(double val);
	
private:
	virtual ~ShapeVariationDescriptor();
	
	unsigned long m_refcount;
	
	ShapeVariationDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;
	
	// This is the actual data the D/DSType stores. In this particular
	// dummy example it's just a signed long called m_DummyContents
	
	char m_StaticShapeVariation[ANGULAR][RADIAL];		// An array of Static Shape Variation feature
	char m_DynamicShapeVariation[ANGULAR][RADIAL];		// An array of Dynamic Shape Variation feature
	char m_StatisticalVariation[ANGULAR][RADIAL];		// An array of Statistical Variation feature
	
	static double QuantTable[17];
	static double IQuantTable[16];
};
	
};

#endif //__SHAPE_VARIATION_H__
