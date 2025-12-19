///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
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
// Applicable File Name:  ParametricObjectMotionCS.h
#ifndef __PARAMETRIC_OBJECT_MOTION_CS__
#define __PARAMETRIC_OBJECT_MOTION_CS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

namespace XM
{


//=============================================================================
class ParametricObjectMotionCSInterfaceABC: public I_InterfaceABC
{
public:
	ParametricObjectMotionCSInterfaceABC();
	virtual ~ParametricObjectMotionCSInterfaceABC() {}

	static const UUID myID;

	virtual void destroy()=0;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
				*aParametricObjectMotionDescriptorInterfac) = 0;
	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class ParametricObjectMotionCS;

//=============================================================================
class ParametricObjectMotionCSInterface: public ParametricObjectMotionCSInterfaceABC
{
public:
	ParametricObjectMotionCSInterface(ParametricObjectMotionCS *anCS);
	virtual ~ParametricObjectMotionCSInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
				*aParametricObjectMotionDescriptorInterfac);
	virtual int StartEncode();
	virtual int StartDecode();

private:
	ParametricObjectMotionCS *m_ParametricObjectMotionCS;	
};


//=============================================================================
class ParametricObjectMotionCS: public DescriptionCodingEngine
{
friend class ParametricObjectMotionCSInterface;
public:
	ParametricObjectMotionCS();
	ParametricObjectMotionCS(ParametricObjectMotionDescriptorInterfaceABC
				 *aDescriptorInterface);

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

	virtual ParametricObjectMotionCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ParametricObjectMotionDescriptorInterfaceABC
				    *aParametricObjectMotionDescriptorInterface);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	virtual ~ParametricObjectMotionCS();

	ParametricObjectMotionCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;
	ParametricObjectMotionDescriptorInterfaceABC
	  *m_DescriptorInterface;
};

};

#endif //__PARAMETRIC_OBJECT_MOTION_CS__
