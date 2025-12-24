///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Akio YAMADA NEC Corp.
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
// Applicable File Name:  GridLayout.h

#ifndef __GRID_LAYOUT_H__
#define __GRID_LAYOUT_H__

#include "Descriptors/Descriptors.h"

namespace XM
{
	// Define the Visual Descriptor to be placed in the GridLayout Descriptor
#if 0
#define GL_SUB_DESCRIPTOR DominantColorDescriptor
#define GL_SUB_DESCRIPTOR_INTERFACE DominantColorDescriptorInterfaceABC
#define GL_SUB_DESCRIPTOR_TYPE "DominantColorType"
#define GL_SUB_EXTRACTION DominantColorExtractionTool
#define GL_SUB_EXTRACTION_INTERFACE DominantColorExtractionInterfaceABC
#else
#define GL_SUB_DESCRIPTOR ColorLayoutDescriptor
#define GL_SUB_DESCRIPTOR_INTERFACE ColorLayoutDescriptorInterfaceABC
#define GL_SUB_DESCRIPTOR_TYPE "ColorLayoutType"
#define GL_SUB_EXTRACTION ColorLayoutExtractionTool
#define GL_SUB_EXTRACTION_INTERFACE ColorLayoutExtractionInterfaceABC
#endif

//=============================================================================
class GridLayoutDescriptorInterfaceABC: public I_InterfaceABC
{
public:
  GridLayoutDescriptorInterfaceABC();
  virtual ~GridLayoutDescriptorInterfaceABC() {};

  virtual void addref()=0;
  virtual void release()=0;

  static const UUID myID;

  virtual GL_SUB_DESCRIPTOR_INTERFACE *GetContents(int x_pos, int y_pos)=0;
  virtual void DeleteContents(int x_pos, int y_pos)=0;
  virtual int SetContents(const int x_pos, const int y_pos, GL_SUB_DESCRIPTOR_INTERFACE *content)=0;

  virtual int GetPartNumberH(void)=0;
  virtual int GetPartNumberV(void)=0;
  virtual void SetPartNumber(const int part_number_h, const int part_number_v)=0;
  virtual char *GetContentName(void)=0;

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class GridLayoutDescriptor;

//=============================================================================
class GridLayoutDescriptorInterface: public GridLayoutDescriptorInterfaceABC
{
public:
  GridLayoutDescriptorInterface(GridLayoutDescriptor
			       *aGridLayoutDescriptor);
  virtual ~GridLayoutDescriptorInterface();

  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual GL_SUB_DESCRIPTOR_INTERFACE *GetContents(int x_pos, int y_pos);
  virtual void DeleteContents(int x_pos, int y_pos);
  virtual int SetContents(const int x_pos, const int y_pos, GL_SUB_DESCRIPTOR_INTERFACE *content);

  virtual int GetPartNumberH(void);
  virtual int GetPartNumberV(void);
  virtual void SetPartNumber(const int part_number_h, const int part_number_v);
  virtual char *GetContentName(void);

  static const UUID myID;
  static const char *myName;

  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
private:
  GridLayoutDescriptor *m_Descriptor;
};

//=============================================================================
class GridLayoutDescriptor: public Descriptor
{
friend class GridLayoutDescriptorInterface;
public:
  GridLayoutDescriptor();

 // actual descriptor methods
  virtual bool IsProprietary(void);
  virtual const UUID& GetValueID(void);
  virtual const char* GetValueName(void);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual void addref();
  virtual void release();

  virtual GL_SUB_DESCRIPTOR_INTERFACE *GetContents(int x_pos, int y_pos);
  virtual void DeleteContents(int x_pos, int y_pos);
  virtual int SetContents(const int x_pos, const int y_pos, GL_SUB_DESCRIPTOR_INTERFACE *content);

  virtual int GetPartNumberH(void);
  virtual int GetPartNumberV(void);
  virtual void SetPartNumber(const int part_number_h, const int part_number_v);
  virtual char *GetContentName(void);

  // access is allowed only by class factories for this
  // object.  This avoids having to duplicate the
  // ID definition in multiple locations.  In the future, we may
  // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
  // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
  static const UUID myID;
  static const char * myName;

  virtual GridLayoutDescriptorInterfaceABC *GetInterface(void);
  virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
  virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
  virtual ~GridLayoutDescriptor();
	
  unsigned long m_refcount;

  GridLayoutDescriptorInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

 // This is the actual data the D/DSType stores. In this particular
 // dummy example it's just a signed long called m_DummyContents

  GL_SUB_DESCRIPTOR_INTERFACE **m_ContentDescriptors;
  int m_part_number_h, m_part_number_v;
  char m_content_name[256];
};

};

#endif //__GRID_LAYOUT_H__
