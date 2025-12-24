///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul
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
// Applicable File Name:  EdgeHist.h
#ifndef __EDGEHIST_H__
#define __EDGEHIST_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

typedef	struct Edge_Histogram_Descriptor{
  double Local_Edge[80]; 
} EHD;

//=============================================================================
class EdgeHistogramDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  EdgeHistogramDescriptorInterfaceABC();
  virtual ~EdgeHistogramDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;
	
  static const UUID myID;

  virtual EHD*	GetEdgeHistogram()=0;
  virtual char*	GetEdgeHistogramElement()=0;
  virtual void	SetEdgeHistogram( EHD*	pEdge_Histogram )=0;
  virtual void	SetEdgeHistogramElement( char*	pEdge_Histogram )=0;
	
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription)=0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

};

class EdgeHistogramDescriptor;

//=============================================================================
class EdgeHistogramDescriptorInterface: public EdgeHistogramDescriptorInterfaceABC
{
public:
  EdgeHistogramDescriptorInterface(EdgeHistogramDescriptor*
		  aEdgeHistogramDescriptor);
  virtual ~EdgeHistogramDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual EHD*	GetEdgeHistogram();
  virtual char*	GetEdgeHistogramElement();
  virtual void	SetEdgeHistogram( EHD*	pEdge_Histogram );
  virtual void	SetEdgeHistogramElement( char*	pEdge_Histogram );

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  static const UUID myID;
  static const char *myName;


private:
  EdgeHistogramDescriptor *m_Descriptor;
};

//=============================================================================
class EdgeHistogramDescriptor: public Descriptor
{
friend class EdgeHistogramDescriptorInterface;
public:
  EdgeHistogramDescriptor();

  // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual void addref();
  virtual void release();

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);
  virtual EdgeHistogramDescriptorInterfaceABC *GetInterface(void);

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
  virtual EHD*	GetEdgeHistogram();
  virtual char*	GetEdgeHistogramElement();
  virtual void	SetEdgeHistogram(EHD*	pEdge_Histogram);
  virtual void	SetEdgeHistogramElement(char*	pEdge_Histogram);

private:
  virtual ~EdgeHistogramDescriptor();
  unsigned char	m_refcount;
  EdgeHistogramDescriptorInterface    m_Interface;

  const bool m_isProprietary;
  static const char *valName;
  static const UUID valID;

  // actual values:
  EHD	*m_pEdge_Histogram;
  char	*m_pEdge_HistogramElement;

  static double QuantTable[5][8];

};

};

#endif //__EDGEHIST_H__
