///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShapeCS.h
#ifndef __PERCEPTUAL3DSHAPECS_H__
#define __PERCEPTUAL3DSHAPECS_H__


// Only include if useaudio
#include "Descriptors/Descriptors.h"
#include "Encoders/Encoders.h"
#include "Decoders/Decoders.h"

namespace XM
{


//=============================================================================
class Perceptual3DShapeCSInterfaceABC: public I_InterfaceABC
{
public:
  Perceptual3DShapeCSInterfaceABC();
  virtual ~Perceptual3DShapeCSInterfaceABC() {}

  virtual void destroy()=0;

  static const UUID myID;

  virtual EncoderFileIO *GetEncoderStreamBuffer(void) = 0;
  virtual int SetEncoderStreamBuffer(EncoderFileIO *aBuffer) = 0;
  virtual DecoderFileIO *GetDecoderStreamBuffer(void) = 0;
  virtual int SetDecoderStreamBuffer(DecoderFileIO *aBuffer) = 0;

  virtual Perceptual3DShapeDescriptorInterfaceABC
    *GetDescriptorInterface(void) = 0;
  virtual int SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
				     *aPerceptual3DShapeDescriptorInterfac) = 0;

  virtual int StartEncode() = 0;
  virtual int StartDecode() = 0;
};

class Perceptual3DShapeCS;

//=============================================================================
class Perceptual3DShapeCSInterface: public Perceptual3DShapeCSInterfaceABC
{
public:
  Perceptual3DShapeCSInterface(Perceptual3DShapeCS *anCS);
  virtual ~Perceptual3DShapeCSInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  static const UUID myID;
  static const char *myName;

  virtual EncoderFileIO *GetEncoderStreamBuffer(void);
  virtual int SetEncoderStreamBuffer(EncoderFileIO *aBuffer);
  virtual DecoderFileIO *GetDecoderStreamBuffer(void);
  virtual int SetDecoderStreamBuffer(DecoderFileIO *aBuffer);

  virtual Perceptual3DShapeDescriptorInterfaceABC
    *GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
				     *aPerceptual3DShapeDescriptorInterfac);

  virtual int StartEncode();
  virtual int StartDecode();

private:
	Perceptual3DShapeCS *m_Perceptual3DShapeCS;	
};


//=============================================================================
class Perceptual3DShapeCS: public DescriptionCodingEngine
{
friend class Perceptual3DShapeCSInterface;
public:
 Perceptual3DShapeCS();
 Perceptual3DShapeCS(Perceptual3DShapeDescriptorInterfaceABC *Perceptual3DShape);

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

 virtual Perceptual3DShapeCSInterfaceABC *GetInterface(void);

 // accessor methods
 virtual EncoderFileIO *GetEncoderStreamBuffer(void);
 virtual int SetEncoderStreamBuffer(EncoderFileIO *aBuffer);
 virtual DecoderFileIO *GetDecoderStreamBuffer(void);
 virtual int SetDecoderStreamBuffer(DecoderFileIO *aBuffer);

 virtual Perceptual3DShapeDescriptorInterfaceABC
   *GetDescriptorInterface(void);
 virtual int SetDescriptorInterface(Perceptual3DShapeDescriptorInterfaceABC
				    *aPerceptual3DShapeDescriptorInterface);

 virtual int StartEncode();
 virtual int StartDecode();

private:
 virtual ~Perceptual3DShapeCS();

 Perceptual3DShapeCSInterface m_Interface;
	
 const bool m_isProprietary;
 static const char * valName;
 static const UUID valID;

 // descriptor data
 EncoderFileIO *m_EncoderBuffer;
 DecoderFileIO *m_DecoderBuffer;
 Perceptual3DShapeDescriptorInterfaceABC *m_DescriptorInterface;
};

};

#endif //__PERCEPTUAL3DSHAPECS_H__
