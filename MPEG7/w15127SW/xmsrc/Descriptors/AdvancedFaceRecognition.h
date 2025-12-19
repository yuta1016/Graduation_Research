//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AdvancedFaceRecognition.h
//


#ifndef __AdvancedFaceRecognition_H__
#define __AdvancedFaceRecognition_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

#define _FD_DIM_ 96
typedef	struct AdvancedFaceRecognition_Descriptor{
  int numOfFourierFeature;
  int numOfCentralFourierFeature;
  int extensionFlag;
  int FourierFeature[_FD_DIM_];
  int CentralFourierFeature[_FD_DIM_];
  int numOfCompositeFeature;
  int numOfCentralCompositeFeature;
  int CompositeFeature[_FD_DIM_];
  int CentralCompositeFeature[_FD_DIM_];
} AdvancedFRD;

//=============================================================================
class AdvancedFaceRecognitionDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	AdvancedFaceRecognitionDescriptorInterfaceABC();
	virtual ~AdvancedFaceRecognitionDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;
	virtual void  SetValuesFromParameterFile(void) = 0;
	
	virtual AdvancedFRD*	GetAdvancedFaceRecognition()=0;
	virtual void	SetAdvancedFaceRecognition( AdvancedFRD*	pAdvancedFaceRecognition )=0;
	
	static const UUID myID;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

};

class AdvancedFaceRecognitionDescriptor;

//=============================================================================
class AdvancedFaceRecognitionDescriptorInterface: public AdvancedFaceRecognitionDescriptorInterfaceABC
{
public:
	AdvancedFaceRecognitionDescriptorInterface(AdvancedFaceRecognitionDescriptor *AdvancedFaceRecognition);
	virtual ~AdvancedFaceRecognitionDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();
	virtual void  SetValuesFromParameterFile(void);

	virtual AdvancedFRD*	GetAdvancedFaceRecognition();
	virtual void	SetAdvancedFaceRecognition( AdvancedFRD*	pAdvancedFaceRecognition );

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	static const UUID myID;
	static const char *myName;


private:
	AdvancedFaceRecognitionDescriptor *m_AdvancedFaceRecognitionDescriptor;
};

//=============================================================================
class AdvancedFaceRecognitionDescriptor: public Descriptor
{
friend class AdvancedFaceRecognitionDescriptorInterface;
public:
	AdvancedFaceRecognitionDescriptor();


	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual void addref();
	virtual void release();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual AdvancedFaceRecognitionDescriptorInterfaceABC *GetInterface(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char *myName;

	// Accesssor methods
	virtual AdvancedFRD*	GetAdvancedFaceRecognition();
	virtual void	SetAdvancedFaceRecognition(AdvancedFRD*	pAdvancedFaceRecognition);

private:
	virtual ~AdvancedFaceRecognitionDescriptor();
	unsigned char	m_refcount;
	AdvancedFaceRecognitionDescriptorInterface    m_Interface;
	
	const bool m_isProprietary;
	static const char *valName;
	static const UUID valID;

	// actual values:
	AdvancedFRD	*m_pAdvancedFaceRecognition;
	
};

};

#endif //__AdvancedFaceRecognition_H__
