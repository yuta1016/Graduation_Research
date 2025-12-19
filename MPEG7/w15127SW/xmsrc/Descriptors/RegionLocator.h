///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, F. Sanahuja
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
// Applicable File Name:  RegionLocator.h
#ifndef __REGION_LOCATOR__
#define __REGION_LOCATOR__

#include "Descriptors/Descriptors.h"
#include "momusys.h"
#include "vopio.h"
#include <iostream>

// MACROS //
#ifndef PIX_xy
#define PIX_xy(image, a, b)  (*((image->data->u) + ( (b) * (image->x) ) + (a)) )
#endif

#ifndef ABS
#define  ABS(x)                (((x) < 0) ? -(x) : (x))
#endif

#ifndef SIGN
#define  SIGN(x)               (((x) < 0) ? -1 : 1)
#endif

enum segmentPos {LEFT, RIGHT};
typedef enum segmentPos segPos;

namespace XM
{
	typedef struct loc {
		bool invers;
		unsigned long NoVert;
		unsigned long *Vertices;
        } polyline;

        typedef struct vert{
            Short	**VL;		/* list of vertices */
            UShort 	nVertices;	/* number of vertices */
            MomImage 	*oriImage;
            MomImage	*recImage;
            
        }vertices;




//=============================================================================
class RegionLocatorDescriptorInterfaceABC : public I_InterfaceABC
{
public:
	RegionLocatorDescriptorInterfaceABC();
	virtual ~RegionLocatorDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual unsigned long GetCoordFlag(void)=0;
	virtual unsigned long SetCoordFlag(unsigned long)=0;
	virtual unsigned long GetSpatialReference(void)=0;
	virtual unsigned long SetSpatialReference(unsigned long)=0;
	virtual unsigned long GetOneLocator(void)=0;
	virtual unsigned long SetOneLocator(unsigned long OneLocator)=0;
	virtual char *GetCoordRef(void)=0;
	virtual unsigned long SetCoordRef(char *)=0;

	virtual int GetRepresentation(int component) = 0;
	virtual int SetRepresentation(int XBits, int YBits) = 0;
	virtual void ResetDescriptor(void)=0;
	virtual int GetNumberOfPolygons(void) = 0;
	virtual int GetNumberOfBoxes(void) = 0;
        virtual int SetInverseFlag(int B_P, int index, bool flag) = 0;
	virtual int GetNumberOfVertices(int B_P, int index) = 0;
	virtual int SetLocator(int B_P, bool invers, int NoV,
			       unsigned int* vertices) = 0;
	virtual struct loc GetLocator(int B_P, int index) = 0;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) =0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) =0;


	//sth please only normative parts here.JH: corrected.


	static const UUID myID;
};

class RegionLocatorDescriptor;

//=============================================================================
class RegionLocatorDescriptorInterface: public RegionLocatorDescriptorInterfaceABC
{
public:
	RegionLocatorDescriptorInterface(RegionLocatorDescriptor *region);
	virtual ~RegionLocatorDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(unsigned long);
	virtual unsigned long GetSpatialReference(void);
	virtual unsigned long SetSpatialReference(unsigned long);
	virtual unsigned long GetOneLocator(void);
	virtual unsigned long SetOneLocator(unsigned long);
	virtual char *GetCoordRef(void);
	virtual unsigned long SetCoordRef(char *);

	virtual int GetRepresentation(int component);
	virtual int SetRepresentation(int XBits, int YBits);
	virtual void ResetDescriptor();
	virtual int GetNumberOfPolygons(void);
	virtual int GetNumberOfBoxes(void);
	virtual int SetInverseFlag(int B_P, int index, bool flag);
	virtual int GetNumberOfVertices(int B_P, int index);
	virtual int SetLocator(int B_P, bool invers, int NoV,
			       unsigned int* vertices);
	virtual struct loc GetLocator(int B_P, int index);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	
        static const UUID myID;
	static const char *myName;

private:
	RegionLocatorDescriptor *m_RegionLocatorDescriptor;
};



//=============================================================================
class RegionLocatorDescriptor : public Descriptor
{
friend class RegionLocatorDescriptorInterface;
public:
	RegionLocatorDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char *myName;

	virtual RegionLocatorDescriptorInterfaceABC *GetInterface(void);

	// Accesssor methods
	virtual unsigned long GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(unsigned long);
	virtual unsigned long GetSpatialReference(void);
	virtual unsigned long SetSpatialReference(unsigned long);
	virtual unsigned long GetOneLocator(void);
	virtual unsigned long SetOneLocator(unsigned long OneLocator);
	virtual char *GetCoordRef(void);
	virtual unsigned long SetCoordRef(char *);

	virtual int GetRepresentation(int component);
	virtual int SetRepresentation(int XBits, int YBits);
	virtual void ResetDescriptor();
	virtual int GetNumberOfPolygons(void);
	virtual int GetNumberOfBoxes(void);
	virtual int SetInverseFlag(int B_P, int index, bool flag);
	virtual int GetNumberOfVertices(int B_P, int index);
	virtual int SetLocator(int B_P, bool invers, int NoV,
			       unsigned int* vertices);
	virtual struct loc GetLocator(int B_P, int index);
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);



private:

        // private methods

	virtual ~RegionLocatorDescriptor();

	unsigned long m_refcount;
        

	RegionLocatorDescriptorInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:
	unsigned long m_CoordFlag;
	unsigned long m_SpatialReference;
	unsigned long m_OneLocator;
	char *m_CoordRef;

	int m_XRepresentation, m_YRepresentation;
	int m_B_P_Selector;
	int m_NoBox;
	int m_NoPoly;
        struct loc *m_BoxData,*m_PolyData;

};

};

#endif // __REGION_Locator__
