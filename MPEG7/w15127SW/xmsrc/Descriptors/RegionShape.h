///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
// (contributing organizations names)
//
// Modified by
// Min-Seok Choi - Hanyang University, Korea
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RegionShape.h
//


#ifndef __REGION_SHAPE__
#define __REGION_SHAPE__

#include "Descriptors/Descriptors.h"

namespace XM
{

#define ART_ANGULAR	12
#define ART_RADIAL 	3


//=============================================================================
class RegionShapeDescriptorInterfaceABC : public I_InterfaceABC
{
public:
	RegionShapeDescriptorInterfaceABC();
	virtual ~RegionShapeDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual bool SetElement(char p, char r, char value) = 0;
	virtual bool SetElement(char p, char r, double value) = 0;
	virtual char GetElement(char p, char r) = 0;
	virtual double GetRealValue(char p, char r) = 0;

	static const UUID myID;
	
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

};

class RegionShapeDescriptor;

//=============================================================================
class RegionShapeDescriptorInterface: public RegionShapeDescriptorInterfaceABC
{
public:
	RegionShapeDescriptorInterface(RegionShapeDescriptor *region);
	virtual ~RegionShapeDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual bool SetElement(char p, char r, char value);
	virtual bool SetElement(char p, char r, double value);
	virtual char GetElement(char p, char r);
	virtual double GetRealValue(char p, char r);

	static const UUID myID;
	static const char *myName;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	RegionShapeDescriptor *m_RegionShapeDescriptor;
};


//=============================================================================
class RegionShapeDescriptor : public Descriptor
{
friend class RegionShapeDescriptorInterface;
public:
	RegionShapeDescriptor();

	virtual void addref();
	virtual void release();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char *myName;

	virtual RegionShapeDescriptorInterfaceABC *GetInterface(void);

	// Accesssor methods
	bool SetElement(char p, char r, char value);
	bool SetElement(char p, char r, double value);
	char GetElement(char p, char r);
	double GetRealValue(char p, char r);

private:
	virtual ~RegionShapeDescriptor();

	unsigned long m_refcount;

	RegionShapeDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:
	char m_ArtDE[ART_ANGULAR][ART_RADIAL];
	             // An array of RegionShape coefficients
	
	static double QuantTable[17];
	static double IQuantTable[16];
};

};

#endif // __REGION_SHAPE__
