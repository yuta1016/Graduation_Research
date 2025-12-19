///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Philips Research USA, Sylvie Jeannin
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MotionTrajectoryCS.h

// Remarks: this module is not in its final state, as it needs to make use of:
//	- global motion interfaces & coding/decoding routines
//	- time interfaces & coding/decoding routines
//	- coordinates interfaces & coding/decoding routines
// to be added where it is mentioned when available
// In the current status:
// Temporary solutions are provided for coding times and coordinates,.
// Global motion is never coded/decoded.

#ifndef __MotionTrajectoryCS__
#define __MotionTrajectoryCS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"
//COORD #include "SpatialCoordSysCS.h"
#include "TemporalInterpolationCS.h"

namespace XM
{


//=============================================================================
class MotionTrajectoryCSInterfaceABC: public I_InterfaceABC
{
public:
	MotionTrajectoryCSInterfaceABC();
	virtual ~MotionTrajectoryCSInterfaceABC() {}

	virtual void destroy()=0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				*aMotionTrajectoryDescriptorInterface) = 0;
//COORD 	virtual SpatialCoordSysCSInterfaceABC *GetSpatialCoordSysCSInterface(void) = 0;
//COORD 	virtual int SetSpatialCoordSysCSInterface(SpatialCoordSysCSInterfaceABC
//COORD 					     *aSpatialCoordSysCSInterface) = 0;
	virtual TemporalInterpolationCSInterfaceABC *GetTemporalInterpolationCSInterface(void) = 0;
	virtual int SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
					     *aTemporalInterpolationCSInterface) = 0;

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class MotionTrajectoryCS;

//=============================================================================
class MotionTrajectoryCSInterface: public MotionTrajectoryCSInterfaceABC
{
public:
	MotionTrajectoryCSInterface(MotionTrajectoryCS *anCS);
	virtual ~MotionTrajectoryCSInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				*aMotionTrajectoryDescriptorInterface);
//COORD 	virtual SpatialCoordSysCSInterfaceABC *GetSpatialCoordSysCSInterface(void);
//COORD 	virtual int SetSpatialCoordSysCSInterface(SpatialCoordSysCSInterfaceABC
//COORD 					     *aSpatialCoordSysCSInterface);
	virtual TemporalInterpolationCSInterfaceABC *GetTemporalInterpolationCSInterface(void);
	virtual int SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
					     *aTemporalInterpolationCSInterface);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	MotionTrajectoryCS *m_MotionTrajectoryCS;
};

class BitBuffer;
class MotionTrajectoryDescriptor;

//=============================================================================
class MotionTrajectoryCS: public DescriptionCodingEngine
{
friend class MotionTrajectoryCSInterface;
public:
	MotionTrajectoryCS();
	MotionTrajectoryCS(MotionTrajectoryDescriptorInterfaceABC
			 *motiontrajectory);

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

	virtual MotionTrajectoryCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer * GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				    *aMotionTrajectoryDescriptorInterface);
//COORD 	virtual SpatialCoordSysCSInterfaceABC *GetSpatialCoordSysCSInterface(void);
//COORD 	virtual int SetSpatialCoordSysCSInterface(SpatialCoordSysCSInterfaceABC
//COORD 					     *aSpatialCoordSysCSInterface);
	virtual TemporalInterpolationCSInterfaceABC *GetTemporalInterpolationCSInterface(void);
	virtual int SetTemporalInterpolationCSInterface(TemporalInterpolationCSInterfaceABC
					     *aTemporalInterpolationCSInterface);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	virtual ~MotionTrajectoryCS();

	MotionTrajectoryCSInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;
	MotionTrajectoryDescriptorInterfaceABC
	  *m_MotionTrajectoryDescriptorInterface;
//COORD 	SpatialCoordSysCSInterfaceABC *m_SpatialCoordSysCSInterface;
	TemporalInterpolationCSInterfaceABC *m_TemporalInterpolationCSInterface;
};

};

#endif //__MotionTrajectoryCS__
