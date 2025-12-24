///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  FaceRecognition.h
#ifndef __FaceRecognition__
#define __FaceRecognition__

namespace XM
{

typedef	struct FaceRecognition_Descriptor{
//  Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int eigenfeature[48];
//	int eigenfeature[49]; 
} FRD;

//=============================================================================
class FaceRecognitionDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	FaceRecognitionDescriptorInterfaceABC();
	virtual ~FaceRecognitionDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;
	
	virtual void  SetValuesFromParameterFile(void) = 0;
	
	virtual FRD*	GetFaceRecognition()=0;
	virtual void	SetFaceRecognition( FRD*	pFaceRecognition )=0;
	
	static const UUID myID;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

};

class FaceRecognitionDescriptor;

//=============================================================================
class FaceRecognitionDescriptorInterface: public FaceRecognitionDescriptorInterfaceABC
{
public:
	FaceRecognitionDescriptorInterface(FaceRecognitionDescriptor *FaceRecognition);
	virtual ~FaceRecognitionDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);


	virtual FRD*	GetFaceRecognition();
	virtual void	SetFaceRecognition( FRD*	pFaceRecognition );

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	static const UUID myID;
	static const char *myName;


private:
	FaceRecognitionDescriptor *m_FaceRecognitionDescriptor;
};

//=============================================================================
class FaceRecognitionDescriptor: public Descriptor
{
friend class FaceRecognitionDescriptorInterface;
public:
	FaceRecognitionDescriptor();


	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual void addref();
	virtual void release();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual FaceRecognitionDescriptorInterfaceABC *GetInterface(void);

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
	virtual FRD*	GetFaceRecognition();
	virtual void	SetFaceRecognition(FRD*	pFaceRecognition);

private:
	virtual ~FaceRecognitionDescriptor();
	unsigned char	m_refcount;
	FaceRecognitionDescriptorInterface    m_Interface;
	
	const bool m_isProprietary;
	static const char *valName;
	static const UUID valID;

	// actual values:
	FRD	*m_pFaceRecognition;
	
};

};

#endif //__FaceRecognition__
