///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric R&D Centre Europe
// P. Brasnett, W. Price
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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ImageSignature.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_SIGNATURE__
#define __IMAGE_SIGNATURE__

//#===--- Typedefs 

//#===--- Defines


#define IMAGESIGNATURE_BITS				7040
#define IMAGESIGNATURE_BYTES			((IMAGESIGNATURE_BITS + 7) >> 3)

#define IMAGESIGNATURE_BITS_GLOBAL		512
#define IMAGESIGNATURE_BYTES_GLOBAL		((IMAGESIGNATURE_BITS_GLOBAL + 7) >> 3)

#define IMAGESIGNATURE_BITS_HASH     32
#define IMAGESIGNATURE_BYTES_HASH    ((IMAGESIGNATURE_BITS_HASH + 7) >> 3)

#define IMAGESIGNATURE_BITS_FEATURE		64
#define IMAGESIGNATURE_BYTES_FEATURE	((IMAGESIGNATURE_BITS_FEATURE + 7) >> 3)

#define IMAGESIGNATURE_BITS_HASH_2        8
#define IMAGESIGNATURE_BYTES_HASH_2       ((IMAGESIGNATURE_BITS_HASH_2 + 7) >> 3)

namespace XM
{

typedef unsigned char ImageSignatureGlobalBits[IMAGESIGNATURE_BYTES_GLOBAL];
typedef unsigned char ImageSignatureGlobalHash[IMAGESIGNATURE_BYTES_HASH];
typedef unsigned char ImageSignatureFeatureBits[IMAGESIGNATURE_BYTES_FEATURE];
typedef unsigned char ImageSignatureBits[IMAGESIGNATURE_BYTES];
typedef unsigned char ImageSignatureFeatureHash[IMAGESIGNATURE_BYTES_HASH_2];


//#===-----------------------------------------
//#===--- ImageSignatureDescriptorInterfaceABC

class ImageSignatureDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	ImageSignatureDescriptorInterfaceABC();
	virtual ~ImageSignatureDescriptorInterfaceABC() {}

	virtual void addref()=0;
	virtual void release()=0;

	virtual void SetGlobalSignature(ImageSignatureGlobalBits bits) = 0;
	virtual void GetGlobalSignature(ImageSignatureGlobalBits &bits) const = 0;

	virtual void SetAspectSignature(ImageSignatureGlobalBits bits) = 0;
	virtual void GetAspectSignature(ImageSignatureGlobalBits &bits) const = 0;

	virtual void SetFeatureSignature(ImageSignatureBits bits) = 0;
	virtual void GetFeatureSignature(ImageSignatureBits &bits) const = 0;

	static const UUID myID;
	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription) = 0;
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription) = 0;
};

class ImageSignatureDescriptor;

//#===--------------------------------------
//#===--- ImageSignatureDescriptorInterface

class ImageSignatureDescriptorInterface: public ImageSignatureDescriptorInterfaceABC
{
public:
	ImageSignatureDescriptorInterface(ImageSignatureDescriptor *pImageSignature);
	virtual ~ImageSignatureDescriptorInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	static const UUID myID;
	static const char *myName;

	virtual void SetGlobalSignature(ImageSignatureGlobalBits bits);
	virtual void GetGlobalSignature(ImageSignatureGlobalBits &bits) const;

	virtual void SetAspectSignature(ImageSignatureGlobalBits bits);
	virtual void GetAspectSignature(ImageSignatureGlobalBits &bits) const;

	virtual void SetFeatureSignature(ImageSignatureBits bits);
	virtual void GetFeatureSignature(ImageSignatureBits &bits) const;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

private:
	ImageSignatureDescriptor *m_pImageSignatureDescriptor;	
};

//#===-----------------------------
//#===--- ImageSignatureDescriptor

class ImageSignatureDescriptor: public Descriptor
{
friend class ImageSignatureDescriptorInterface;
public:
	ImageSignatureDescriptor();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual ImageSignatureDescriptorInterfaceABC *GetInterface(void);

	// accessor methods
	virtual void SetGlobalSignature(ImageSignatureGlobalBits bits);
	virtual void GetGlobalSignature(ImageSignatureGlobalBits &bits) const;

	virtual void SetAspectSignature(ImageSignatureGlobalBits bits);
	virtual void GetAspectSignature(ImageSignatureGlobalBits &bits) const;

	virtual void SetFeatureSignature(ImageSignatureBits bits);
	virtual void GetFeatureSignature(ImageSignatureBits &bits) const;

private:
	virtual ~ImageSignatureDescriptor();

	unsigned long m_refcount;

	ImageSignatureDescriptorInterface m_Interface;
	
	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// descriptor data
	ImageSignatureGlobalBits m_bits;
	ImageSignatureGlobalBits m_aspect_bits;
	ImageSignatureBits m_feature_bits;


};

};

#endif // __IMAGE_SIGNATURE__
