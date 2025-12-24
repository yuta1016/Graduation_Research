///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Modified by Vincent Hsu, June 11, 2001
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
// Applicable File Name:  GoFGoPColor.h


#ifndef __GoFGoPColor__
#define __GoFGoPColor__
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

typedef enum {Average,Median,Intersection,Reserved} TAggregation;

namespace XM
{

class  ScalableColorDescriptorInterfaceABC;  //vin

//=============================================================================
class GoFGoPColorDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  GoFGoPColorDescriptorInterfaceABC();
  virtual ~GoFGoPColorDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  /* only needed for manual connection with sub components*/
  virtual ScalableColorDescriptorInterfaceABC
    *GetScalableColorDescriptorInterface(void) = 0;
  virtual unsigned long
    SetScalableColorDescriptorInterface(ScalableColorDescriptorInterfaceABC
				       *aScalableColorDescriptorInterface) = 0;

  virtual TAggregation GetAggregation(void)=0;
  virtual void SetAggregation(const TAggregation Aggregation)=0;

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class GoFGoPColorDescriptor;

//=============================================================================
class GoFGoPColorDescriptorInterface: public GoFGoPColorDescriptorInterfaceABC
{
public:
  GoFGoPColorDescriptorInterface(GoFGoPColorDescriptor
			       *aGoFGoPColorDescriptor);
  virtual ~GoFGoPColorDescriptorInterface();

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

  virtual TAggregation GetAggregation(void);
  virtual void SetAggregation(const TAggregation Aggregation);

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  GoFGoPColorDescriptor *m_Descriptor;
};

//=============================================================================
class GoFGoPColorDescriptor: public Descriptor
{
friend class GoFGoPColorDescriptorInterface;
public:
  GoFGoPColorDescriptor();

  GoFGoPColorDescriptor(ScalableColorDescriptorInterfaceABC *aScalableColor);

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

  virtual TAggregation GetAggregation(void);
  virtual void SetAggregation(const TAggregation Aggregation);

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

  virtual GoFGoPColorDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~GoFGoPColorDescriptor();
	
  unsigned long m_refcount;

  GoFGoPColorDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

  ScalableColorDescriptorInterfaceABC *m_ScalableColorDescriptorInterface;

  // This is the actual data the D/DSType stores. 
  TAggregation m_Aggregation;
};

};

#endif //__GoFGoPColor__
