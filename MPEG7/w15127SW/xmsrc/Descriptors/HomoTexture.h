// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  HomoTextureDescriptor.h
//
#ifndef _HOMO_TEXTURE_H_
#define _HOMO_TEXTURE_H_

#include "Descriptors/Descriptors.h"

namespace XM
{

//=============================================================================
class HomogeneousTextureDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  HomogeneousTextureDescriptorInterfaceABC();
  virtual ~HomogeneousTextureDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  //---yjyu - 010222
	virtual void SetHomogeneousTextureFeature( int, int* ) = 0;
	virtual int* GetHomogeneousTextureFeature( ) = 0 ;
	virtual int GetHomogeneousTextureFeatureFlag( ) = 0 ;
  //---

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class HomogeneousTextureDescriptor;

//=============================================================================
class HomogeneousTextureDescriptorInterface: public HomogeneousTextureDescriptorInterfaceABC
{
public:
  HomogeneousTextureDescriptorInterface(HomogeneousTextureDescriptor
			       *aHomogeneousTextureDescriptor);
  virtual ~HomogeneousTextureDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  //---yjyu - 010222
	virtual void SetHomogeneousTextureFeature( int, int* ) ;
	virtual int* GetHomogeneousTextureFeature( ) ;
	virtual int GetHomogeneousTextureFeatureFlag( );
  //---

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  HomogeneousTextureDescriptor *m_Descriptor;
};

//=============================================================================
class HomogeneousTextureDescriptor: public Descriptor
{
friend class HomogeneousTextureDescriptorInterface;
public:
  HomogeneousTextureDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  //---yjyu - 010222
	virtual void SetHomogeneousTextureFeature(int flag, int* HomogeneousTextureFeature);
	virtual int* GetHomogeneousTextureFeature() ;
	virtual int GetHomogeneousTextureFeatureFlag();
  //---

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

  virtual HomogeneousTextureDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~HomogeneousTextureDescriptor();
	
  unsigned long m_refcount;

  HomogeneousTextureDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

 // This is the actual data the D/DSType stores.
  int energydeviationflag;
  int feature[62];

};

};

#endif //_HOMO_TEXTURE_H_
