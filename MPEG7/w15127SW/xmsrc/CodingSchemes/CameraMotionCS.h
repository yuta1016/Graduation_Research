///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// Applicable File Name:  CameraMotionCS.h
#ifndef __CameraMotionCS__
#define __CameraMotionCS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

#define TRACK_LEFT_MASK 0x8000
#define TRACK_RIGHT_MASK 0x4000
#define BOOM_DOWN_MASK 0x2000
#define BOOM_UP_MASK 0x1000
#define DOLLY_FORWARD_MASK 0x800
#define DOLLY_BACKWARD_MASK 0x400
#define PAN_LEFT_MASK 0x200
#define PAN_RIGHT_MASK 0x100
#define TILT_DOWN_MASK 0x80
#define TILT_UP_MASK 0x40
#define ROLL_CLOCK_MASK 0x20
#define ROLL_ANTI_MASK 0x10
#define ZOOM_IN_MASK 0x8
#define ZOOM_OUT_MASK 0x4
#define FIXED_MASK 0x2
#define RESERVED_MASK 0x1


namespace XM
{


//=============================================================================
class CameraMotionCSInterfaceABC: public I_InterfaceABC
{
public:
	CameraMotionCSInterfaceABC();
	virtual ~CameraMotionCSInterfaceABC() {}

	static const UUID myID;

	virtual void destroy() = 0;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual CameraMotionDescriptorInterfaceABC *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface) = 0;

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class CameraMotionCS;

//=============================================================================
class CameraMotionCSInterface: public CameraMotionCSInterfaceABC
{
public:
	CameraMotionCSInterface(CameraMotionCS *anCS);
	virtual ~CameraMotionCSInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual CameraMotionDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);
	

	virtual int StartEncode();
	virtual int StartDecode();

private:
	CameraMotionCS *m_CameraMotionCS;	
};


//=============================================================================
class CameraMotionCS: public DescriptionCodingEngine
{
friend class CameraMotionCSInterface;
public:
	CameraMotionCS();
	CameraMotionCS(CameraMotionDescriptorInterfaceABC *CameraMotion);

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual CameraMotionCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual CameraMotionDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);

	virtual int StartEncode();
	virtual int StartDecode();

private:

	virtual ~CameraMotionCS();
	CameraMotionCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;
	CameraMotionDescriptorInterfaceABC *m_CameraMotionDescriptorInterface;
};

};

#endif //__CameraMotionCS__
