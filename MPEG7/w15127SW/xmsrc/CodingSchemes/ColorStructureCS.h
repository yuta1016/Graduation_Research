////////////////////////////////////////////////////////////////////////
//
// ColorStructureCS.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000-2003
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#ifndef __COLORSTRUCTURECS_H__
#define __COLORSTRUCTURECS_H__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"

namespace XM
{


//=============================================================================
class ColorStructureCSInterfaceABC: public I_InterfaceABC
{
public:
	ColorStructureCSInterfaceABC();
	virtual ~ColorStructureCSInterfaceABC() {}

	virtual void destroy()=0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

	virtual ColorStructureDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
						 *aColorStructureDescriptorInterfac) = 0;

	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;
};

class ColorStructureCS;

//=============================================================================
class ColorStructureCSInterface: public ColorStructureCSInterfaceABC
{
public:
	ColorStructureCSInterface(ColorStructureCS *anCS);
	virtual ~ColorStructureCSInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

	virtual ColorStructureDescriptorInterfaceABC
		*GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
						 *aColorStructureDescriptorInterfac);

	virtual int StartEncode();
	virtual int StartDecode();

private:
	ColorStructureCS *m_ColorStructureCS;	
};


//=============================================================================
class ColorStructureCS: public DescriptionCodingEngine
{
friend class ColorStructureCSInterface;
public:
 ColorStructureCS();
 ColorStructureCS(ColorStructureDescriptorInterfaceABC
			 *ColorStructure);

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

 virtual ColorStructureCSInterfaceABC *GetInterface(void);

 // accessor methods
 virtual BitBuffer *GetEncoderStreamBuffer(void);
 virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
 virtual BitBuffer *GetDecoderStreamBuffer(void);
 virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

 virtual ColorStructureDescriptorInterfaceABC
	 *GetDescriptorInterface(void);
 virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
						*aColorStructureDescriptorInterface);


 virtual int StartEncode();
 virtual int StartDecode();

private:
 virtual ~ColorStructureCS();

 ColorStructureCSInterface m_Interface;
	
 const bool m_isProprietary;
 static const char * valName;
 static const UUID valID;

 // descriptor data
 BitBuffer *m_EncoderBuffer;
 BitBuffer *m_DecoderBuffer;
 ColorStructureDescriptorInterfaceABC *m_DescriptorInterface;
};

};

#endif //__COLORSTRUCTURECS_H__
