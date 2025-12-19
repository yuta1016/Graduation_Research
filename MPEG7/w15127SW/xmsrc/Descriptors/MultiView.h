///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiView.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MultiView__
#define __MultiView__

//#===--- Typedefs 

// define the 2D ShapeDescriptors to change it easily

//sth potential conflict with other components
#define SUB_DESCRIPTOR ContourShapeDescriptor
#define SUB_EXTRACTION_TOOL ContourShapeExtractionTool
#define SUB_SEARCH_TOOL ContourShapeSearchTool
#define SUB_CS ContourShapeCS
#define SUB_DESCRIPTOR ContourShapeDescriptor
#define SUB_DESCRIPTOR_INTERFACE_ABC ContourShapeDescriptorInterfaceABC
#define SUB_EXTRACTION_INTERFACE_ABC ContourShapeExtractionInterfaceABC
#define SUB_SEARCH_INTERFACE_ABC ContourShapeSearchInterfaceABC
#define SUB_CS_INTERFACE_ABC ContourShapeCSInterfaceABC
#define SUB_DECRIPTOR_TYPE ContourShapeType
/*
#define SUB_DESCRIPTOR RegionShapeDescriptor
#define SUB_EXTRACTION_TOOL RegionShapeExtractionTool
#define SUB_SEARCH_TOOL RegionShapeSearchTool
#define SUB_CS RegionShapeCS
#define SUB_DESCRIPTOR RegionShapeDescriptor
#define SUB_DESCRIPTOR_INTERFACE_ABC RegionShapeDescriptorInterfaceABC
#define SUB_EXTRACTION_INTERFACE_ABC RegionShapeExtractionInterfaceABC
#define SUB_SEARCH_INTERFACE_ABC RegionShapeSearchInterfaceABC
#define SUB_CS_INTERFACE_ABC RegionShapeCSInterfaceABC
*/

#define VISIBLEVIEWONLYSEARCH 1
#define ANYVIEWSEARCH 0

//#===--- Defines
//template <class T> T min(T x, T y)   { return (x < y ? x : y); }
//template <class T> T max(T x, T y)   { return (x < y ? y : x); }
const int TotNumberOfViews = 7;
namespace XM
{

class SUB_DESCRIPTOR_INTERFACE_ABC;
//sth delimiters :-(
//#===-----------------------------------------
//#===--- MultiViewDescriptorInterfaceABC

class MultiViewDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	MultiViewDescriptorInterfaceABC();
	virtual ~MultiViewDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual SUB_DESCRIPTOR_INTERFACE_ABC *Get2DDescriptorInterface(int i) = 0;
	virtual unsigned long Set2DDescriptorInterface(SUB_DESCRIPTOR_INTERFACE_ABC *, int) = 0;
	
	// access members for multi-view descriptor.
	virtual void SetNoOfViews(unsigned char count ) = 0;
	virtual unsigned char GetNoOfViews(void) const = 0;
	virtual void SetVisibleViewFlag(int i, bool value) = 0;
	virtual bool GetVisibleViewFlag(int i) = 0;
	virtual void SetFixedViewsFlag(bool value) = 0;
	virtual bool GetFixedViewsFlag(void) = 0;
	
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;

	static const UUID myID;
};

class MultiViewDescriptor;

//#===--------------------------------------
//#===--- MultiViewDescriptorInterface

class MultiViewDescriptorInterface: public MultiViewDescriptorInterfaceABC
{
public:
	MultiViewDescriptorInterface(MultiViewDescriptor *pMultiView);
	virtual ~MultiViewDescriptorInterface();
	
	virtual void addref();
	virtual void release();

	virtual SUB_DESCRIPTOR_INTERFACE_ABC *Get2DDescriptorInterface(int i);
	virtual unsigned long Set2DDescriptorInterface(SUB_DESCRIPTOR_INTERFACE_ABC *Interface2d, int i);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;

	
	// access members for multi-view descriptor.
	virtual void SetNoOfViews(unsigned char count );
	virtual unsigned char GetNoOfViews(void) const;
	virtual void SetVisibleViewFlag(int i, bool value);
	virtual bool GetVisibleViewFlag(int i);
	virtual void SetFixedViewsFlag(bool value);
	virtual bool GetFixedViewsFlag(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	MultiViewDescriptor *m_pMultiViewDescriptor;	
};

//#===-----------------------------
//#===--- MultiViewDescriptor

class MultiViewDescriptor: public Descriptor
{
friend class MultiViewDescriptorInterface;
public:

	MultiViewDescriptor();
	MultiViewDescriptor(SUB_DESCRIPTOR_INTERFACE_ABC **Descriptor2d);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual MultiViewDescriptorInterfaceABC *GetInterface(void);

	// access members for multi-view descriptor.
	virtual void SetNoOfViews(unsigned char count );
	virtual unsigned char GetNoOfViews(void) const;
	virtual void SetVisibleViewFlag(int i, bool value);
	virtual bool GetVisibleViewFlag(int i);
	virtual void SetFixedViewsFlag(bool value);
	virtual bool GetFixedViewsFlag(void);

	// accessor methods for subdescriptor
	virtual SUB_DESCRIPTOR_INTERFACE_ABC *Get2DDescriptorInterface(int i);
	virtual unsigned long Set2DDescriptorInterface(SUB_DESCRIPTOR_INTERFACE_ABC *interface2d, int i);

private:

	virtual ~MultiViewDescriptor();

	unsigned long m_refcount;

	MultiViewDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// Multi-view descriptor data	
	unsigned char m_iNumberOfViews;

	// The visibility flags for each view
	bool m_pVisibleViewFlags[8];

	// Fixed(HHI method of view extraction) or Unknown Views
	bool m_bFixedViewsFlag;

	// link to subdescriptor (here array of pointers is requiered for n views)
	SUB_DESCRIPTOR_INTERFACE_ABC** m_p2DDescriptorInterface;

};

};

#endif // __MultiView__
