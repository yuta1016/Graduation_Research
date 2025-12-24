///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio Yamada
// NEC Corp.
// Soo-Jun Park (ETRI), Dong Kwon Park (Dongguk Univ)
// Leszek Cieplinski (Mitsubishi Electric ITE-VIL)
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
// Copyright (c) 1998-2004.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GofGopFeature.h

#ifndef __GOFCOPFEATURE_H__
#define __GOFCOPFEATURE_H__

#include "Descriptors/Descriptors.h"

namespace XM
{

  // forward declaration
  class ColorLayoutDescriptorInterfaceABC;
  class DominantColorDescriptorInterfaceABC;
  class EdgeHistogramDescriptorInterfaceABC;

//=============================================================================
class GofGopFeatureDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  GofGopFeatureDescriptorInterfaceABC();
  virtual ~GofGopFeatureDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  virtual ColorLayoutDescriptorInterfaceABC
    *GetColorLayoutDescriptorInterface(void) = 0;
  virtual EdgeHistogramDescriptorInterfaceABC // added by dkpark
    *GetEdgeHistogramDescriptorInterface(void) = 0;
  virtual DominantColorDescriptorInterfaceABC // added by LC
    *GetDominantColorDescriptorInterface(void) = 0;
  virtual unsigned long
    SetColorLayoutDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				       *aColorLayoutDescriptorInterface) = 0;
  virtual unsigned long // added by dkpark
    SetEdgeHistogramDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
				       *aEdgeHistogramDescriptorInterface) = 0;
  virtual unsigned long // added by LC
    SetDominantColorDescriptorInterface(DominantColorDescriptorInterfaceABC
				       *aDominantColorDescriptorInterface) = 0;

  virtual long GetAggregation(void)=0;
  virtual void SetAggregation(const long val)=0;

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class GofGopFeatureDescriptor;

//=============================================================================
class GofGopFeatureDescriptorInterface: public GofGopFeatureDescriptorInterfaceABC
{
public:
  GofGopFeatureDescriptorInterface(GofGopFeatureDescriptor
			       *aGofGopFeatureDescriptor);
  virtual ~GofGopFeatureDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual ColorLayoutDescriptorInterfaceABC
    *GetColorLayoutDescriptorInterface(void);
  virtual EdgeHistogramDescriptorInterfaceABC // added by dkpark
    *GetEdgeHistogramDescriptorInterface(void);
  virtual DominantColorDescriptorInterfaceABC // added by LC
    *GetDominantColorDescriptorInterface(void);
  virtual unsigned long
    SetColorLayoutDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				       *aColorLayoutDescriptorInterface);
  virtual unsigned long // added by dkpark
    SetEdgeHistogramDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
				       *aEdgeHistogramDescriptorInterface);
  virtual unsigned long // added by LC
    SetDominantColorDescriptorInterface(DominantColorDescriptorInterfaceABC
				       *aDominantColorDescriptorInterface);

  virtual long GetAggregation(void);
  virtual void SetAggregation(const long val);

  virtual unsigned long
    ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;

private:
  GofGopFeatureDescriptor *m_Descriptor;
};

//=============================================================================
class GofGopFeatureDescriptor: public Descriptor
{
friend class GofGopFeatureDescriptorInterface;
public:
  GofGopFeatureDescriptor();

  GofGopFeatureDescriptor(ColorLayoutDescriptorInterfaceABC *aColorLayout);
  GofGopFeatureDescriptor(EdgeHistogramDescriptorInterfaceABC *aEdgeHistogram);// added by dkpark
  GofGopFeatureDescriptor(DominantColorDescriptorInterfaceABC *aEdgeHistogram);// added by LC
  GofGopFeatureDescriptor(ColorLayoutDescriptorInterfaceABC *aColorLayout, EdgeHistogramDescriptorInterfaceABC *aEdgeHistogram, DominantColorDescriptorInterfaceABC *aDominantColor);// added by dkpark and LC
 
 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual ColorLayoutDescriptorInterfaceABC
    *GetColorLayoutDescriptorInterface(void);
  virtual EdgeHistogramDescriptorInterfaceABC // added by dkpark
    *GetEdgeHistogramDescriptorInterface(void);
  virtual DominantColorDescriptorInterfaceABC // added by LC
    *GetDominantColorDescriptorInterface(void);
  virtual unsigned long
    SetColorLayoutDescriptorInterface(ColorLayoutDescriptorInterfaceABC
				     *aColorLayoutDescriptorInterface);
  virtual unsigned long // added by dkpark
    SetEdgeHistogramDescriptorInterface(EdgeHistogramDescriptorInterfaceABC
				     *aEdgeHistogramDescriptorInterface);
  virtual unsigned long // added by LC
    SetDominantColorDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterface);

  virtual long GetAggregation(void);
  virtual void SetAggregation(const long val);

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

  virtual GofGopFeatureDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~GofGopFeatureDescriptor();
	
  unsigned long m_refcount;

  GofGopFeatureDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

  ColorLayoutDescriptorInterfaceABC *m_ColorLayoutDescriptorInterface;
  EdgeHistogramDescriptorInterfaceABC *m_EdgeHistogramDescriptorInterface; // added by dkpark
  DominantColorDescriptorInterfaceABC *m_DominantColorDescriptorInterface; // added by LC

 // This is the actual data the D/DSType stores. In this particular
 // dummy example it's just a signed long called m_DummyContents
  long m_Aggregation;

};

};

#endif //__GOFCOPFEATURE_H__
