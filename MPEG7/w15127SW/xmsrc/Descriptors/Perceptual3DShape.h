///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShape.h

#ifndef __PERCEPTUAL3DSHAPE_H__
#define __PERCEPTUAL3DSHAPE_H__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

namespace XM
{

typedef struct P3DS_Components
{
  unsigned long Volume;
  unsigned long Center_X, Center_Y, Center_Z;
  unsigned long PCA_Axis_1X, PCA_Axis_1Y, PCA_Axis_1Z, PCA_Axis_2X, PCA_Axis_2Y, PCA_Axis_2Z;//these are PCA_Axis_1X, PCA_Axis_1Y, PCA_Axis_1Z, PCA_Axis_2X, PCA_Axis_2Y, PCA_Axis_2Z in the standard.
  unsigned long Max_1, Max_2, Max_3;//these are 
  unsigned long Convexity;
} P3DS_COMPONENTS;

//=============================================================================
class Perceptual3DShapeDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  Perceptual3DShapeDescriptorInterfaceABC();
  virtual ~Perceptual3DShapeDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  virtual int GetPerceptual3DBitsPerAttribute(void)=0;
  virtual int SetPerceptual3DBitsPerAttribute(int nBitsPerAttribute)=0;
  virtual int GetPerceptual3DShapeNNodes(void)=0;
  virtual int SetPerceptual3DShapeNNodes(int nNodes)=0;
  virtual long* GetPerceptual3DShapeIsConnected(void)=0;
  virtual P3DS_COMPONENTS* GetPerceptual3DShapeNodesAll(void)=0;

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class Perceptual3DShapeDescriptor;

//=============================================================================
class Perceptual3DShapeDescriptorInterface: public Perceptual3DShapeDescriptorInterfaceABC
{
public:
  Perceptual3DShapeDescriptorInterface(Perceptual3DShapeDescriptor
			       *aPerceptual3DShapeDescriptor);
  virtual ~Perceptual3DShapeDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual int GetPerceptual3DBitsPerAttribute(void);
  virtual int SetPerceptual3DBitsPerAttribute(int nBitsPerAttribute);
  virtual int GetPerceptual3DShapeNNodes(void);
  virtual int SetPerceptual3DShapeNNodes(int nNodes);
  virtual long* GetPerceptual3DShapeIsConnected(void);
  virtual P3DS_COMPONENTS* GetPerceptual3DShapeNodesAll(void);

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
  static const UUID myID;
  static const char *myName;

private:
  Perceptual3DShapeDescriptor *m_Perceptual3DShapeDescriptor;
};

//=============================================================================
class Perceptual3DShapeDescriptor: public Descriptor
{
friend class Perceptual3DShapeDescriptorInterface;
public:
  Perceptual3DShapeDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual int GetPerceptual3DBitsPerAttribute(void);
  virtual int SetPerceptual3DBitsPerAttribute(int nBitsPerAttribute);
  virtual int GetPerceptual3DShapeNNodes(void);
  virtual int SetPerceptual3DShapeNNodes(int nNodes);
  virtual long* GetPerceptual3DShapeIsConnected(void);
  virtual P3DS_COMPONENTS* GetPerceptual3DShapeNodesAll(void);

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;

  virtual Perceptual3DShapeDescriptorInterfaceABC *GetInterface(void);

private:
  virtual ~Perceptual3DShapeDescriptor();
	
  unsigned long m_refcount;

  Perceptual3DShapeDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

 // Real data, corresponding to the standard.
  unsigned long m_Perceptual3DShape_numberOfNodes;
  unsigned long m_Perceptual3DShape_BitsPerAttribute;
  long* m_Perceptual3DShape_IsConnected;
  P3DS_COMPONENTS* m_Perceptual3DShape_NodesAll;
};

};

#endif //__PERCEPTUAL3DSHAPE_H__
