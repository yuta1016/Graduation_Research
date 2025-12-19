///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// P. Brasnett
// Mitsubishi Electric R&D Centre Europe
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
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignatureCS.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEOSIGNATURECS__
#define __VIDEOSIGNATURECS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

namespace XM
{

//#===---------------------------------
//#===--- VideoSignatureCSInterfaceABC

class VideoSignatureCSInterfaceABC: public I_InterfaceABC
{
public:
	VideoSignatureCSInterfaceABC();
	virtual ~VideoSignatureCSInterfaceABC() {}

	virtual void destroy() = 0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer() = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *pBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer() = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *pBuffer) = 0;

	virtual VideoSignatureDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
				    *pVideoSignatureDescriptorInterface) = 0;

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;

	//0825
	virtual void SetKeyFrameInfoFlag(int flag)=0;
	virtual int GetKeyFrameInfoFlag()=0;
};

class VideoSignatureCS;

//#===------------------------------
//#===--- VideoSignatureCSInterface

class VideoSignatureCSInterface: public VideoSignatureCSInterfaceABC
{
public:
	VideoSignatureCSInterface(VideoSignatureCS *pCS);
	virtual ~VideoSignatureCSInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer();
	virtual int SetEncoderStreamBuffer(BitBuffer *pBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer();
	virtual int SetDecoderStreamBuffer(BitBuffer *pBuffer);

	virtual VideoSignatureDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
				    *pVideoSignatureDescriptorInterface);

	virtual int StartEncode();
	virtual int StartDecode();

	//0825
	virtual void SetKeyFrameInfoFlag(int flag);
	virtual int GetKeyFrameInfoFlag();

private:
	VideoSignatureCS *m_pVideoSignatureCS;	
};

class BitBuffer;
class VideoSignatureDescriptor;

//#===---------------------
//#===--- VideoSignatureCS

class VideoSignatureCS: public DescriptionCodingEngine
{
friend class VideoSignatureCSInterface;
public:
	VideoSignatureCS();
	VideoSignatureCS(VideoSignatureDescriptorInterfaceABC *contourshape);

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

	virtual VideoSignatureCSInterfaceABC *GetInterface(void);

	// accessor methods
	virtual BitBuffer *GetEncoderStreamBuffer();
	virtual int SetEncoderStreamBuffer(BitBuffer *pBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer();
	virtual int SetDecoderStreamBuffer(BitBuffer *pBuffer);

	virtual VideoSignatureDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
				    *pVideoSignatureDescriptorInterface);

	virtual int StartEncode();
	virtual int StartDecode();

//	virtual void SetCompressionFlag(int flag);

	virtual void SetKeyFrameInfoFlag(int flag);
	virtual int GetKeyFrameInfoFlag();


private:
	virtual ~VideoSignatureCS();

	VideoSignatureCSInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;
	VideoSignatureDescriptorInterfaceABC *m_pVideoSignatureDescriptorInterface;

	int m_KeyFrameFlag;

	unsigned char *LUT_L1_DIST;

	void setLUT_L1_DIST();
	void freeLUT_L1_DIST();
	int L1Distance( unsigned char *signature1, unsigned char *signature2 );
};

};

#endif //__VIDEOSIGNATURECS__
