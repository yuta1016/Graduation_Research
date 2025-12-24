///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Siemens AG, F. Sanahuja
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
// Applicable File Name:  RegionLocatorCS.h
#ifndef __REGION_LOCATOR_CS__
#define __REGION_LOCATOR_CS__

#include "Descriptors/Descriptors.h"
#include "Utilities/BitBuffer.h"
//sth seams to be contradiction
#include "DescriptionSchemes/GenericDS.h"//sth ordering problem of headers
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "CodingSchemes/GenericDSCS.h"//sth ordering problem of headers
#include "CodingSchemes/CodingSchemes.h"

namespace XM
{


//=============================================================================
class RegionLocatorCSInterfaceABC: public I_InterfaceABC
{
public:
	RegionLocatorCSInterfaceABC();
	virtual ~RegionLocatorCSInterfaceABC() {}

	virtual void destroy()=0;

	static const UUID myID;

	virtual BitBuffer *GetEncoderStreamBuffer(void) = 0;
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual BitBuffer *GetDecoderStreamBuffer(void) = 0;
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer) = 0;
	virtual int SetEncoderFileName(char *filename) = 0;//sth
 	virtual int SetDecoderFileName(char *filename) = 0;//sth

	virtual RegionLocatorDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
				    *aRegionLocatorDescriptorInterface) = 0;
	virtual int StartEncode() = 0;
	virtual int StartDecode() = 0;

	virtual int PostEncode() = 0;//sth
	virtual int PreDecode() = 0;//sth
};

class RegionLocatorCS;

//=============================================================================
class RegionLocatorCSInterface: public RegionLocatorCSInterfaceABC
{
public:
	RegionLocatorCSInterface(RegionLocatorCS *anCS);
	virtual ~RegionLocatorCSInterface();
	
	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);
	virtual int SetEncoderFileName(char *filename);
 	virtual int SetDecoderFileName(char *filename);
	

	virtual RegionLocatorDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
				    *aRegionLocatorDescriptorInterface);
	virtual int StartEncode();
	virtual int StartDecode();

	virtual int PostEncode(void);
	virtual int PreDecode(void);
                
private:
	RegionLocatorCS *m_RegionLocatorCS;	
  
};

class BitBuffer;
class RegionLocatorDescriptor;

//=============================================================================
class RegionLocatorCS: public DescriptionCodingEngine
{
friend class RegionLocatorCSInterface;
public:
         RegionLocatorCS();
	 RegionLocatorCS(RegionLocatorDescriptorInterfaceABC *regionLocator);


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

	virtual RegionLocatorCSInterfaceABC *GetInterface(void);

	// accessor methods

	virtual BitBuffer *GetEncoderStreamBuffer(void);
	virtual int SetEncoderStreamBuffer(BitBuffer *aBuffer);
	virtual BitBuffer *GetDecoderStreamBuffer(void);
	virtual int SetDecoderStreamBuffer(BitBuffer *aBuffer);
	virtual int SetEncoderFileName(char *filename);
 	virtual int SetDecoderFileName(char *filename);

	virtual RegionLocatorDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(RegionLocatorDescriptorInterfaceABC
				    *aRegionLocatorDescriptorInterface);
	virtual int StartEncode();
	virtual int StartDecode();

	virtual int PostEncode(void);
 	virtual int PreDecode(void);

private:
	virtual ~RegionLocatorCS();

        int readComponents(long *vertex, long octant,long xDRI, long yDRI);
	unsigned short 	getMajorAndMinorComponents( long x, long y, \
                                                    unsigned short octant, \
                                                    unsigned long *majorC, \
                                                    unsigned long *minorC);
	unsigned short 	getOctant(long x, long y);
        int getRangeExt (int x);
	RegionLocatorCSInterface m_Interface;
	
	const bool m_isProprietary;
	int m_DSize;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	BitBuffer *m_EncoderBuffer;
	BitBuffer *m_DecoderBuffer;

	RegionLocatorDescriptorInterfaceABC *m_DescriptorInterface;
 	GenericDS m_DSDocument;
	GenericDSInterfaceABC *m_DSInterface;
 	GenericDSCSInterfaceABC *m_DSCSInterface;

};

};

#endif //__Region_Locator_CS__
