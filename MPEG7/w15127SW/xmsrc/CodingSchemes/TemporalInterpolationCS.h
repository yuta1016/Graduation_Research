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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TemporalInterpolationCS.h
#ifndef __TemporalInterpolationCS__
#define __TemporalInterpolationCS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

namespace XM
{


//=============================================================================
class TemporalInterpolationCSInterfaceABC: public I_InterfaceABC
{
public:
	TemporalInterpolationCSInterfaceABC();
	virtual ~TemporalInterpolationCSInterfaceABC() {}

	virtual void destroy()=0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual TemporalInterpolationDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
				    *aTemporalInterpolationDescriptorInterface) = 0;
	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;

};

class TemporalInterpolationCS;

//=============================================================================
class TemporalInterpolationCSInterface: public TemporalInterpolationCSInterfaceABC
{
public:
	TemporalInterpolationCSInterface(TemporalInterpolationCS *anCS);
	virtual ~TemporalInterpolationCSInterface();
	
	static const UUID myID;
	static const char *myName;

	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual TemporalInterpolationDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
				    *aTemporalInterpolationDescriptorInterface);
	virtual int StartEncode();
	virtual int StartDecode();

private:
	TemporalInterpolationCS *m_TemporalInterpolationCS;	
};

class BitBuffer;
class TemporalInterpolationDescriptor;

//=============================================================================
class TemporalInterpolationCS: public DescriptionCodingEngine
{
friend class TemporalInterpolationCSInterface;
public:
	TemporalInterpolationCS();
	TemporalInterpolationCS(TemporalInterpolationDescriptorInterfaceABC *TemporalInterpolation);

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

	virtual TemporalInterpolationCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual TemporalInterpolationDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(TemporalInterpolationDescriptorInterfaceABC
				    *aTemporalInterpolationDescriptorInterface);
	virtual int StartEncode();
	virtual int StartDecode();

private:
	virtual ~TemporalInterpolationCS();

	TemporalInterpolationCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;

	TemporalInterpolationDescriptorInterfaceABC *m_TemporalInterpolationDescriptorInterface;

	unsigned long EncodeTimePoint(double t);
	unsigned long EncodeDuration(double t);
	double DecodeTimePoint();
	double DecodeDuration();

public:

};

};

#endif //__TemporalInterpolationCS__
