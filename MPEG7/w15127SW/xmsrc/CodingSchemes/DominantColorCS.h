///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, University of California, Santa Barbara.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorCS.h
#ifndef __DominantColorCS__
#define __DominantColorCS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"
#include "CodingSchemes/CodingSchemes.h"

namespace XM
{


//=============================================================================
class DominantColorCSInterfaceABC: public I_InterfaceABC
{
public:
  DominantColorCSInterfaceABC();
  virtual ~DominantColorCSInterfaceABC() {}

  virtual void destroy()=0;

  static const UUID myID;

  virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
  virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
  virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
  virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;

  virtual DominantColorDescriptorInterfaceABC
    *GetDescriptorInterface(void) = 0;
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterfac) = 0;

  virtual ColorSpaceCSInterfaceABC *GetColorSpaceCSInterface(void) = 0;
  virtual int SetColorSpaceCSInterface(ColorSpaceCSInterfaceABC
				       *aColorSpaceCSInterface) = 0;
  virtual ColorQuantizerCSInterfaceABC
    *GetColorQuantizerCSInterface(void) = 0;
  virtual int SetColorQuantizerCSInterface(ColorQuantizerCSInterfaceABC
					   *aColorQuantizercCSInterface) = 0;
  virtual int StartEncode() = 0;
  virtual int StartDecode() = 0;
};

class DominantColorCS;

//=============================================================================
class DominantColorCSInterface: public DominantColorCSInterfaceABC
{
public:
  DominantColorCSInterface(DominantColorCS *anCS);
  virtual ~DominantColorCSInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  static const UUID myID;
  static const char *myName;

  virtual BitBuffer *GetEncoderStreamBuffer(void);
  virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
  virtual BitBuffer *GetDecoderStreamBuffer(void);
  virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

  virtual DominantColorDescriptorInterfaceABC *GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterfac);
	
  virtual ColorSpaceCSInterfaceABC *GetColorSpaceCSInterface(void);
  virtual int SetColorSpaceCSInterface(ColorSpaceCSInterfaceABC
				       *aColorSpaceCSInterface);
  virtual ColorQuantizerCSInterfaceABC
    *GetColorQuantizerCSInterface(void);
  virtual int SetColorQuantizerCSInterface(ColorQuantizerCSInterfaceABC
					   *aColorQuantizercCSInterface);
  virtual int StartEncode();
  virtual int StartDecode();

private:
  DominantColorCS *m_DominantColorCS;	
};


//=============================================================================
class DominantColorCS: public DescriptionCodingEngine
{
friend class DominantColorCSInterface;
public:
  DominantColorCS();
  DominantColorCS(DominantColorDescriptorInterfaceABC *dc);

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

  virtual DominantColorCSInterfaceABC *GetInterface(void);

  // accessor methods
  virtual BitBuffer *GetEncoderStreamBuffer(void);
  virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
  virtual BitBuffer *GetDecoderStreamBuffer(void);
  virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);

  virtual DominantColorDescriptorInterfaceABC *GetDescriptorInterface(void);
  virtual int SetDescriptorInterface(DominantColorDescriptorInterfaceABC
				     *aDominantColorDescriptorInterface);

  virtual ColorSpaceCSInterfaceABC *GetColorSpaceCSInterface(void);
  virtual int SetColorSpaceCSInterface(ColorSpaceCSInterfaceABC
				       *aColorSpaceCSInterface);
  virtual ColorQuantizerCSInterfaceABC
    *GetColorQuantizerCSInterface(void);
  virtual int SetColorQuantizerCSInterface(ColorQuantizerCSInterfaceABC
					   *aColorQuantizercCSInterface);
  virtual int StartEncode();
  virtual int StartDecode();

private:
  virtual ~DominantColorCS();

  DominantColorCSInterface m_Interface;
	
  const bool m_isProprietary;
  static const char * valName;
  static const UUID valID;

  // descriptor data
  BitBuffer *m_EncoderBuffer;
  BitBuffer *m_DecoderBuffer;
  DominantColorDescriptorInterfaceABC *m_DominantColorDescriptorInterface;
  ColorSpaceCSInterfaceABC *m_ColorSpaceCSInterface;
  ColorQuantizerCSInterfaceABC *m_ColorQuantizerCSInterface;

};

};

#endif //__DominantColorCS__
