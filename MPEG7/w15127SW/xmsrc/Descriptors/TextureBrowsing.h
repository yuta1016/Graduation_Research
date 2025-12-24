///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California Santa Barbara. 
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// Applicable File Name:  TextureBrowsing.h

#ifndef __TEXTUREDESCRIPTOR_H__
#define __TEXTUREDESCRIPTOR_H__

#include "Descriptors/Descriptors.h"

namespace XM
{

//=============================================================================
class TextureBrowsingDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  TextureBrowsingDescriptorInterfaceABC();
  virtual ~TextureBrowsingDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  //---yjyu - 010218
  virtual int GetComponentNumberFlag()=0;
  virtual int* GetBrowsing_Component()=0;
  virtual void SetComponentNumberFlag(int ComponentNumber)=0;
  virtual void SetBrowsing_Component(int *PBC)=0;
  //---
  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class TextureBrowsingDescriptor;

//=============================================================================
class TextureBrowsingDescriptorInterface: public TextureBrowsingDescriptorInterfaceABC
{
public:
  TextureBrowsingDescriptorInterface(TextureBrowsingDescriptor
			       *aTextureBrowsingDescriptor);
  virtual ~TextureBrowsingDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  //---yjyu - 010218
//  	virtual void  SetValuesFromParameterFile(void);

  virtual int GetComponentNumberFlag();
  virtual int* GetBrowsing_Component();
  virtual void SetComponentNumberFlag(int ComponentNumber);
  virtual void SetBrowsing_Component(int* PBC);
  //---

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  TextureBrowsingDescriptor *m_Descriptor;
};

//=============================================================================
class TextureBrowsingDescriptor: public Descriptor
{
friend class TextureBrowsingDescriptorInterface;
public:
  TextureBrowsingDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

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

  virtual TextureBrowsingDescriptorInterfaceABC *GetInterface(void);

  //---yjyu - 010218
  virtual int GetComponentNumberFlag();
  virtual int* GetBrowsing_Component();
  virtual void SetComponentNumberFlag(int ComponentNumber);
  virtual void SetBrowsing_Component(int* PBC);
  //---

private:
  virtual ~TextureBrowsingDescriptor();
	
  unsigned long m_refcount;

  TextureBrowsingDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

 // This is the actual data the D/DSType stores.
  	int m_ComponentNumberFlag;
	int* m_Browsing_Component;
};

};

#endif //__TextureBrowsing_H__
