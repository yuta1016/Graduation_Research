///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Sylvie Jeannin - Philips Research USA
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
// Applicable File Name:  MotionTrajectory.h
#ifndef __MotionTrajectory__
#define __MotionTrajectory__

#include "DescriptionSchemes/DescriptionSchemes.h"	//0625
#include "Descriptors/TemporalInterpolation.h"
//COORD#include "Descriptors/SpatialCoordSys.h"

namespace XM
{

//=============================================================================
class MotionTrajectoryDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	MotionTrajectoryDescriptorInterfaceABC();
	virtual ~MotionTrajectoryDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent)=0;		//0625
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription)=0;	//0625

	// Accessor for Camera Follows
	virtual int GetCameraFollows(void) = 0;
	virtual unsigned long SetCameraFollows(int) = 0;

	// Accesssor for Coord Flag
	virtual int GetCoordFlag(void) = 0;
	virtual unsigned long SetCoordFlag(int) = 0;

	// Accesssor for Coord Ref
	virtual int GetCoordRef(void) = 0;
	virtual unsigned long SetCoordRef(int) = 0;

	// Accesssor for Spatial Reference
	virtual int GetSpatialReference(void) = 0;
	virtual unsigned long SetSpatialReference(int) = 0;

	// Accesssor for Units
	virtual int GetUnits(void) = 0;
	virtual unsigned long SetUnits(int) = 0;

	// Accesssor for Coord Coding Length
	virtual int GetCoordCodingLength(void) = 0;
	virtual unsigned long SetCoordCodingLength(int) = 0;

	// Accesssor for xRepr
	virtual int GetXRepr(void) = 0;
	virtual unsigned long SetXRepr(int) = 0;

	// Accesssor for yRepr
	virtual int GetYRepr(void) = 0;
	virtual unsigned long SetYRepr(int) = 0;

	// Coordinate system and units
	//virtual UnitDescriptorInterfaceABC*            Get...
	//virtual LocalCoordSysDescriptorInterfaceABC*   Get...
	//virtual GlobalImageWarpDescriptorInterfaceABC* Get...
//COORD	virtual SpatialCoordSysDescriptorInterfaceABC
//COORD	  *GetSpatialCoordSysDescriptorInterface(void) = 0;
//COORD	virtual unsigned long SetSpatialCoordSysDescriptorInterface(
//COORD          SpatialCoordSysDescriptorInterfaceABC *) = 0;

	// Interpolation data
	virtual TemporalInterpolationDescriptorInterfaceABC
	  *GetTemporalInterpolationDescriptorInterface(void) = 0;
	virtual unsigned long SetTemporalInterpolationDescriptorInterface(
          TemporalInterpolationDescriptorInterfaceABC *) = 0;

	static const UUID myID;

};

class MotionTrajectoryDescriptor;

//=============================================================================
class MotionTrajectoryDescriptorInterface: public MotionTrajectoryDescriptorInterfaceABC
{
public:
	MotionTrajectoryDescriptorInterface(MotionTrajectoryDescriptor *aTraj);
	virtual ~MotionTrajectoryDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);		//0625
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);	//0625

	// Accesssor for Camera Follows
	virtual int GetCameraFollows(void);
	virtual unsigned long SetCameraFollows(int);

	// Accesssor for Coord Flag
	virtual int GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(int);

	// Accesssor for Coord Ref
	virtual int GetCoordRef(void);
	virtual unsigned long SetCoordRef(int);

	// Accesssor for Spatial Reference
	virtual int GetSpatialReference(void);
	virtual unsigned long SetSpatialReference(int);

	// Accesssor for Units
	virtual int GetUnits(void);
	virtual unsigned long SetUnits(int);

	// Accesssor for Coord Coding Length
	virtual int GetCoordCodingLength(void);
	virtual unsigned long SetCoordCodingLength(int);

	// Accesssor for xRepr
	virtual int GetXRepr(void);
	virtual unsigned long SetXRepr(int);

	// Accesssor for yRepr
	virtual int GetYRepr(void);
	virtual unsigned long SetYRepr(int);

	// Coordinate system and units
	//virtual UnitDescriptorInterfaceABC*            Get...
	//virtual LocalCoordSysDescriptorInterfaceABC*   Get...
	//virtual GlobalImageWarpDescriptorInterfaceABC* Get...
//COORD	virtual SpatialCoordSysDescriptorInterfaceABC
//COORD	  *GetSpatialCoordSysDescriptorInterface(void);
//COORD	virtual unsigned long SetSpatialCoordSysDescriptorInterface(
//COORD          SpatialCoordSysDescriptorInterfaceABC *);

	// Interpolation data
	virtual TemporalInterpolationDescriptorInterfaceABC
	  *GetTemporalInterpolationDescriptorInterface(void);
	virtual unsigned long SetTemporalInterpolationDescriptorInterface(
          TemporalInterpolationDescriptorInterfaceABC *);

	static const UUID myID;
	static const char *myName;


private:
	MotionTrajectoryDescriptor *m_MotionTrajectoryDescriptor;
};

//=============================================================================
class MotionTrajectoryDescriptor: public Descriptor
{
friend class MotionTrajectoryDescriptorInterface;
public:
	MotionTrajectoryDescriptor();
	MotionTrajectoryDescriptor(TemporalInterpolationDescriptorInterfaceABC *interp);
//COORD		SpatialCoordSysDescriptorInterfaceABC *coordSys,
//	virtual ~MotionTrajectoryDescriptor();       //  PUT IN PRIVATE

	virtual void addref();
	virtual void release();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual MotionTrajectoryDescriptorInterfaceABC *GetInterface(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// Accesssor for Camera Follows
	virtual int GetCameraFollows(void);
	virtual unsigned long SetCameraFollows(int);

	// Accesssor for Coord Flag
	virtual int GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(int);

	// Accesssor for Coord Ref
	virtual int GetCoordRef(void);
	virtual unsigned long SetCoordRef(int);

	// Accesssor for Spatial Reference
	virtual int GetSpatialReference(void);
	virtual unsigned long SetSpatialReference(int);

	// Accesssor for Units
	virtual int GetUnits(void);
	virtual unsigned long SetUnits(int);

	// Accesssor for Coord Coding Length
	virtual int GetCoordCodingLength(void);
	virtual unsigned long SetCoordCodingLength(int);

	// Accesssor for xRepr
	virtual int GetXRepr(void);
	virtual unsigned long SetXRepr(int);

	// Accesssor for yRepr
	virtual int GetYRepr(void);
	virtual unsigned long SetYRepr(int);

	//coordinates:
	//virtual UnitDescriptorInterfaceABC*            Get...
	//virtual LocalCoordSysDescriptorInterfaceABC*   Get...
	//virtual GlobalImageWarpDescriptorInterfaceABC* Get...
//COORD	virtual SpatialCoordSysDescriptorInterfaceABC
//COORD	  *GetSpatialCoordSysDescriptorInterface(void);
//COORD	virtual unsigned long SetSpatialCoordSysDescriptorInterface(
//COORD          SpatialCoordSysDescriptorInterfaceABC *);

	//temporal interpolation
	virtual TemporalInterpolationDescriptorInterfaceABC
	  *GetTemporalInterpolationDescriptorInterface(void);
	virtual unsigned long SetTemporalInterpolationDescriptorInterface(
          TemporalInterpolationDescriptorInterfaceABC *);

private:
  	virtual ~MotionTrajectoryDescriptor();

	unsigned long m_refcount;

	MotionTrajectoryDescriptorInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:
	int m_CameraFollows; // 0 if not specified, 3 if Camera follows object, 2 if not
	int m_CoordFlag; // 1 if S2DCS is referenced, 0 if units are instantiated in Trajectory 
	int m_CoordRef; // reference to the adequate S2DCS
	int m_SpatialReference; // 1 if coords are image local, 0 if not
	int m_Units; // 0 if coords normalized by W&H, 1 by H, 2 by W, 3 meters
	int m_CoordCodingLength; // 1 if not default length
	int m_XRepr; // length of x binary spatial coordinates coding 
	int m_YRepr; // length of y binary spatial coordinates coding

	// coordinates, units, interpolation
//COORD	SpatialCoordSysDescriptorInterfaceABC *m_SpatialCoordSysInterface;
	//SpatialCoordSysDescriptor *m_SpatialCoordSysDescriptor;
	TemporalInterpolationDescriptorInterfaceABC *m_TemporalInterpolationInterface;
	//TemporalInterpolationDescriptor *m_TemporalInterpolationDescriptor;
};

};

#endif //__MotionTrajectory__
