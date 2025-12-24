///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Samsung Advanced Institute of Technology, Sang-Kyun Kim, Du-Sik Park, 
// Yanglim Choi
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
// Applicable File Name:  CTBrowsing.h

#ifndef __CTBROWSING_H__
#define __CTBROWSING_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

//=============================================================================
class CTBrowsingDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  CTBrowsingDescriptorInterfaceABC();
  virtual ~CTBrowsingDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  virtual int* GetCTBrowsing_Component()=0; 
  virtual void SetCTBrowsing_Component(int *PBC)=0;

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class CTBrowsingDescriptor;

//=============================================================================
class CTBrowsingDescriptorInterface: public CTBrowsingDescriptorInterfaceABC
{
public:
  CTBrowsingDescriptorInterface(CTBrowsingDescriptor *aCTBrowsingDescriptor);
  virtual ~CTBrowsingDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual int* GetCTBrowsing_Component(); 
  virtual void SetCTBrowsing_Component(int *PBC);

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  CTBrowsingDescriptor *m_CTBrowsingDescriptor;
};

//=============================================================================
class CTBrowsingDescriptor: public Descriptor
{
friend class CTBrowsingDescriptorInterface;
public:
  CTBrowsingDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual int* GetCTBrowsing_Component(); 
  virtual void SetCTBrowsing_Component(int *PBC);

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;

  virtual CTBrowsingDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~CTBrowsingDescriptor();
	
  unsigned long m_refcount;

  CTBrowsingDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

 // This is the actual data the D/DSType stores. In this particular
 // dummy example it's just a signed long called m_DummyContents
	int* m_CTBrowsing_Component;
};

};

#endif //__CTBROWSING_H__
