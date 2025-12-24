///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jason Charlesworth
// Canon Research Centre (Europe) Ltd.
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
// Applicable File Name:  SpokenContentBlockCS.h
#ifndef __SPOKENCONTENTBlock_TYPE_CS__
#define __SPOKENCONTENTBlock_TYPE_CS__

// Only include if using XML
//#ifdef USEXML

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

namespace XM
{


//=============================================================================
class SpokenContentBlockCSInterfaceABC: public I_InterfaceABC
{
public:
	SpokenContentBlockCSInterfaceABC();
	virtual ~SpokenContentBlockCSInterfaceABC() {}

	virtual void destroy()=0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual SpokenContentBlockDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
				*aSpokenContentBlockDescriptorInterfac) = 0;

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class SpokenContentBlockCS;

//=============================================================================
class SpokenContentBlockCSInterface: public SpokenContentBlockCSInterfaceABC
{
public:
	SpokenContentBlockCSInterface(SpokenContentBlockCS *anCS);
	virtual ~SpokenContentBlockCSInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual SpokenContentBlockDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
				*aSpokenContentBlockDescriptorInterfac);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	SpokenContentBlockCS *m_SpokenContentBlockCS;	
};


//=============================================================================
class SpokenContentBlockCS: public DescriptionCodingEngine
{
friend class SpokenContentBlockCSInterface;
public:
	SpokenContentBlockCS();
	SpokenContentBlockCS(SpokenContentBlockDescriptorInterfaceABC
			 *SpokenContentBlock);

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

	virtual SpokenContentBlockCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual SpokenContentBlockDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(SpokenContentBlockDescriptorInterfaceABC
				    *aSpokenContentBlockDescriptorInterface);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	virtual ~SpokenContentBlockCS();

	SpokenContentBlockCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;
	SpokenContentBlockDescriptorInterfaceABC
	  *m_SpokenContentBlockDescriptorInterface;
};

};

//#endif // USEXML
#endif //__SPOKENCONTENTBlock_TYPE_CS__
