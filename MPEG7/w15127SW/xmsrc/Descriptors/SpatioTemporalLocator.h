///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatioTemporalLocator.h
#ifndef __SpatioTemporalLocator__
#define __SpatioTemporalLocator__

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

#define MAXNRefrence 10
#define MAXNVertices 30
#define MAXFrame 1000

namespace XM
{


//=============================================================================
class SpatioTemporalLocatorDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	SpatioTemporalLocatorDescriptorInterfaceABC();
	virtual ~SpatioTemporalLocatorDescriptorInterfaceABC() {};

	virtual void addref()=0;
	virtual void release()=0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent)=0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription)=0;

	// Accesssor methods
	virtual unsigned long GetCoordFlag(void)=0;
	virtual unsigned long SetCoordFlag(unsigned long)=0;
	virtual unsigned long GetRef(void)=0;
	virtual unsigned long SetRef(unsigned long)=0;
	virtual unsigned long GetspatialRef(void)=0;
	virtual unsigned long SetspatialRef(unsigned long)=0;

	virtual unsigned long GetNumberOfRefernceRegions(void) = 0;
	virtual unsigned long SetNumberOfRefernceRegions(unsigned long) = 0;
	virtual unsigned long GetTypeOfTrajectory(unsigned long) = 0;
	virtual unsigned long SetTypeOfTrajectory(unsigned long, unsigned long) = 0;
		// Trajectory
	virtual unsigned long GetMediaTimePoint(unsigned long) = 0;
	virtual unsigned long SetMediaTimePoint(unsigned long, unsigned long) = 0;
	virtual unsigned long GetMediaDuration(unsigned long) = 0;
	virtual unsigned long SetMediaDuration(unsigned long, unsigned long) = 0;
	virtual unsigned long Gettype(unsigned long) = 0;
	virtual unsigned long Settype(unsigned long, unsigned long) = 0;
	virtual unsigned long GetmotionModel(unsigned long) = 0;
	virtual unsigned long SetmotionModel(unsigned long, unsigned long) = 0;
	virtual unsigned long GetellipseFlag(unsigned long) =0;
	virtual unsigned long SetellipseFlag(unsigned long, unsigned long) =0;
	virtual unsigned long GetDepthFlag(unsigned long) =0;
	virtual unsigned long SetDepthFlag(unsigned long, unsigned long) =0;

	virtual TemporalInterpolationDescriptorInterfaceABC*
	  GetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long) =0;
	virtual int
	  SetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long,
		TemporalInterpolationDescriptorInterfaceABC
		*aTemporalInterpolationDescriptorInterface) =0;
	virtual RegionLocatorDescriptorInterfaceABC*
	  GetRegionLocatorDescriptorInterface(unsigned long) =0;
	virtual int
	  SetRegionLocatorDescriptorInterface(unsigned long,
			     RegionLocatorDescriptorInterfaceABC
			     *aRegionLocatorDescriptorInterface) =0;

	static const UUID myID;

};

class SpatioTemporalLocatorDescriptor;

//=============================================================================
class SpatioTemporalLocatorDescriptorInterface: public SpatioTemporalLocatorDescriptorInterfaceABC
{
public:
	SpatioTemporalLocatorDescriptorInterface(SpatioTemporalLocatorDescriptor *stemp);
	virtual ~SpatioTemporalLocatorDescriptorInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// Accesssor methods
	virtual unsigned long GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(unsigned long);
	virtual unsigned long GetRef(void);
	virtual unsigned long SetRef(unsigned long);
	virtual unsigned long GetspatialRef(void);
	virtual unsigned long SetspatialRef(unsigned long);

	virtual unsigned long GetNumberOfRefernceRegions(void);
	virtual unsigned long SetNumberOfRefernceRegions(unsigned long);
	virtual unsigned long GetTypeOfTrajectory(unsigned long);
	virtual unsigned long SetTypeOfTrajectory(unsigned long, unsigned long);
		// Trajectory
	virtual unsigned long GetMediaTimePoint(unsigned long);
	virtual unsigned long SetMediaTimePoint(unsigned long, unsigned long);
	virtual unsigned long GetMediaDuration(unsigned long);
	virtual unsigned long SetMediaDuration(unsigned long, unsigned long);
	virtual unsigned long Gettype(unsigned long);
	virtual unsigned long Settype(unsigned long, unsigned long);
	virtual unsigned long GetmotionModel(unsigned long);
	virtual unsigned long SetmotionModel(unsigned long, unsigned long);
	virtual unsigned long GetellipseFlag(unsigned long);
	virtual unsigned long SetellipseFlag(unsigned long, unsigned long);
	virtual unsigned long GetDepthFlag(unsigned long);
	virtual unsigned long SetDepthFlag(unsigned long, unsigned long);

	virtual TemporalInterpolationDescriptorInterfaceABC*
	  GetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long);
	virtual int
	  SetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long,
                        TemporalInterpolationDescriptorInterfaceABC
			*aTemporalInterpolationDescriptorInterface);
	virtual RegionLocatorDescriptorInterfaceABC*
	  GetRegionLocatorDescriptorInterface(unsigned long);
	virtual int
	  SetRegionLocatorDescriptorInterface(unsigned long,
			     RegionLocatorDescriptorInterfaceABC
			     *aRegionLocatorDescriptorInterface);

	static const UUID myID;
	static const char *myName;


private:
	SpatioTemporalLocatorDescriptor *m_SpatioTemporalLocatorDescriptor;
};

//=============================================================================
class SpatioTemporalLocatorDescriptor: public Descriptor
{
friend class SpatioTemporalLocatorDescriptorInterface;
public:
	SpatioTemporalLocatorDescriptor();
	SpatioTemporalLocatorDescriptor(
	  TemporalInterpolationDescriptorInterfaceABC** TemporalInterpolation,
	  RegionLocatorDescriptorInterfaceABC** RegionLocator);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual SpatioTemporalLocatorDescriptorInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	// Accesssor methods
	virtual unsigned long GetCoordFlag(void);
	virtual unsigned long SetCoordFlag(unsigned long);
	virtual unsigned long GetRef(void);
	virtual unsigned long SetRef(unsigned long);
	virtual unsigned long GetspatialRef(void);
	virtual unsigned long SetspatialRef(unsigned long);

	virtual unsigned long GetNumberOfRefernceRegions(void);
	virtual unsigned long SetNumberOfRefernceRegions(unsigned long);
	virtual unsigned long GetTypeOfTrajectory(unsigned long);
	virtual unsigned long SetTypeOfTrajectory(unsigned long, unsigned long);
		// Trajectory
	virtual unsigned long GetMediaTimePoint(unsigned long);
	virtual unsigned long SetMediaTimePoint(unsigned long, unsigned long);
	virtual unsigned long GetMediaDuration(unsigned long);
	virtual unsigned long SetMediaDuration(unsigned long, unsigned long);
	virtual unsigned long Gettype(unsigned long);
	virtual unsigned long Settype(unsigned long, unsigned long);
	virtual unsigned long GetmotionModel(unsigned long);
	virtual unsigned long SetmotionModel(unsigned long, unsigned long);
	virtual unsigned long GetellipseFlag(unsigned long);
	virtual unsigned long SetellipseFlag(unsigned long, unsigned long);
	virtual unsigned long GetDepthFlag(unsigned long);
	virtual unsigned long SetDepthFlag(unsigned long, unsigned long);

	virtual TemporalInterpolationDescriptorInterfaceABC*
	  GetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long);
	virtual int
	  SetTemporalInterpolationDescriptorInterface(unsigned long,
						      unsigned long,
				 TemporalInterpolationDescriptorInterfaceABC
				 *aTemporalInterpolationDescriptorInterface);
	virtual RegionLocatorDescriptorInterfaceABC*
	  GetRegionLocatorDescriptorInterface(unsigned long);
	virtual int
	  SetRegionLocatorDescriptorInterface(unsigned long,
					  RegionLocatorDescriptorInterfaceABC
					  *aRegionLocatorDescriptorInterface);

private:
	virtual ~SpatioTemporalLocatorDescriptor();

	unsigned long  GetMediaTimePointString(int i,char* s);
	unsigned long  GetMediaDurationString(unsigned long ul,char* s);
	unsigned long  ExtractHours(unsigned long second);
	unsigned long  ExtractMinutes(unsigned long second);
	unsigned long  ExtractSeconds(unsigned long second);
	unsigned long  ExtractSeconds(string s);

	unsigned long m_refcount;

	SpatioTemporalLocatorDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:
	unsigned long m_CoordFlag;
	unsigned long m_Ref;
	unsigned long m_spatialRef;

	unsigned long m_NumberOfRefernceRegions;
	unsigned long m_TypeOfTrajectory[MAXNRefrence]; 

	unsigned long m_MediaTimePoint[MAXNRefrence];
	unsigned long m_MediaDuration[MAXNRefrence];
	unsigned long m_type[MAXNRefrence];
	unsigned long m_motionModel[MAXNRefrence];
	unsigned long m_ellipseFlag[MAXNRefrence];
	unsigned long m_DepthFlag[MAXNRefrence];

	RegionLocatorDescriptorInterfaceABC
	  *m_RegionLocatorDescriptorInterface[MAXNRefrence];
	TemporalInterpolationDescriptorInterfaceABC
	  *m_TemporalInterpolationDescriptorInterface[MAXNRefrence*
						     MAXNVertices+2];
};

};

#endif //__SpatioTemporalLocator__
