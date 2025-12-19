///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
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
// Applicable File Name:  Descriptors.h
#ifndef __ColorSpace__
#define __ColorSpace__

#include "DescriptionSchemes/DescriptionSchemes.h"

typedef enum {RGB,YCRCB,HSV,HMMD,LinearMatrix,Monochrome} ColorSpaceType;

namespace XM
{

//=============================================================================
class ColorSpaceDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ColorSpaceDescriptorInterfaceABC();
	virtual ~ColorSpaceDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;
	virtual bool GetColorReferenceFlag(void) = 0;
	virtual unsigned long SetColorReferenceFlag(bool colorReferenceFlag) = 0;
	virtual ColorSpaceType GetDescriptionColorSpace(void) = 0;
	virtual unsigned long SetDescriptionColorSpace(ColorSpaceType) = 0;
	virtual float *GetColorSpaceMatrix() = 0;
	virtual float GetMatrixElement(int xindex,int yindex) = 0;
	virtual unsigned long SetMatrixElement(int xindex,int yindex, float value) = 0;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
	static const UUID myID;
};

class ColorSpaceDescriptor;

//=============================================================================
class ColorSpaceDescriptorInterface: public ColorSpaceDescriptorInterfaceABC
{
public:
	ColorSpaceDescriptorInterface(ColorSpaceDescriptor *aColorSpace);
	virtual ~ColorSpaceDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);
	virtual bool GetColorReferenceFlag(void);
	virtual unsigned long SetColorReferenceFlag(bool colorReferenceFlag);
	virtual ColorSpaceType GetDescriptionColorSpace(void);
	virtual unsigned long SetDescriptionColorSpace(ColorSpaceType);	
	virtual float *GetColorSpaceMatrix();
	virtual float GetMatrixElement(int xindex,int yindex);
	virtual unsigned long SetMatrixElement(int xindex,int yindex, float value);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	static const UUID myID;
	static const char *myName;


private:
	ColorSpaceDescriptor *m_ColorSpaceDescriptor;	
};

//=============================================================================
class ColorSpaceDescriptor: public Descriptor
{
friend class ColorSpaceDescriptorInterface;
public:
	ColorSpaceDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ColorSpaceDescriptorInterfaceABC *GetInterface(void);

	// accessor methods

	virtual bool GetColorReferenceFlag(void);
	virtual unsigned long SetColorReferenceFlag(bool colorReferenceFlag);
	virtual ColorSpaceType GetDescriptionColorSpace(void);
	virtual unsigned long SetDescriptionColorSpace(ColorSpaceType);	
	virtual float *GetColorSpaceMatrix();
	virtual float GetMatrixElement(int xindex,int yindex);
	virtual unsigned long SetMatrixElement(int xindex,int yindex, float value);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	virtual ~ColorSpaceDescriptor();

	unsigned long m_refcount;

	ColorSpaceDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	bool		m_ColorReferenceFlag;
	ColorSpaceType  m_DescriptionColorSpace;
	float           m_ColorTransMatrix[3][3];
	// 3x3 matrix for LinearMatrix color space
};

};

#endif //__ColorSpace__
