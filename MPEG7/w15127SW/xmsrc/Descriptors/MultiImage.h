///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of Technology
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiImage.h

#ifndef __MULTIIMAGE_H__
#define __MULTIIMAGE_H__

#include "Descriptors/Descriptors.h"

namespace XM
{

//=============================================================================
class MultiImageDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  MultiImageDescriptorInterfaceABC();
  virtual ~MultiImageDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  /* only needed for manual connection with sub components*/
  virtual ScalableColorDescriptorInterfaceABC
    *GetScalableColorDescriptorInterface(void) = 0;
  virtual unsigned long
    SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
			       *aScalableColorDescriptorInterface) = 0;

  virtual ContourShapeDescriptorInterfaceABC
    *GetContourShapeDescriptorInterface(void) = 0;
  virtual unsigned long
    SetContourShapeDescriptorInterface(ContourShapeDescriptorInterfaceABC
			       *aContourShapeDescriptorInterface) = 0;

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class MultiImageDescriptor;

//=============================================================================
class MultiImageDescriptorInterface: public MultiImageDescriptorInterfaceABC
{
public:
  MultiImageDescriptorInterface(MultiImageDescriptor
			       *aMultiImageDescriptor);
  virtual ~MultiImageDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  /* only needed for manual connection with sub components*/
  virtual ScalableColorDescriptorInterfaceABC
    *GetScalableColorDescriptorInterface(void);
  virtual unsigned long
    SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
					 *aScalableColorDescriptorInterface);

  virtual ContourShapeDescriptorInterfaceABC
    *GetContourShapeDescriptorInterface(void);
  virtual unsigned long
    SetContourShapeDescriptorInterface(ContourShapeDescriptorInterfaceABC
				       *aContourShapeDescriptorInterface);

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  MultiImageDescriptor *m_Descriptor;
};

//=============================================================================
class MultiImageDescriptor: public Descriptor
{
friend class MultiImageDescriptorInterface;
public:
  MultiImageDescriptor();

  MultiImageDescriptor(ScalableColorDescriptorInterfaceABC *aScalableColor,
		      ContourShapeDescriptorInterfaceABC *aContourShape);

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  /* only needed for manual connection with sub components*/
  virtual ScalableColorDescriptorInterfaceABC
    *GetScalableColorDescriptorInterface(void);
  virtual unsigned long
    SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
				     *aScalableColorDescriptorInterface);

  virtual ContourShapeDescriptorInterfaceABC
    *GetContourShapeDescriptorInterface(void);
  virtual unsigned long
    SetContourShapeDescriptorInterface(ContourShapeDescriptorInterfaceABC
				     *aContourShapeDescriptorInterface);

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

  virtual MultiImageDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~MultiImageDescriptor();
	
  unsigned long m_refcount;

  MultiImageDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

  ScalableColorDescriptorInterfaceABC *m_ScalableColorDescriptorInterface;
  ContourShapeDescriptorInterfaceABC *m_ContourShapeDescriptorInterface;

 // This is the actual data the D/DSType stores. In this particular
 // dummy example it's just a signed long called m_DummyContents
};

};

#endif //__MULTIIMAGE_H__
