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
// Applicable File Name:  ImageSignatureExtraction.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_SIGNATURE_EXTRACTION_H__
#define __IMAGE_SIGNATURE_EXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>
#include <list>

typedef struct _IplImage IplImage;
typedef struct CvMat CvMat;
typedef unsigned char ImageSignatureRegionBits[14];



//=============================================================================
namespace XM
{


	// Forward Declarations:
	class ImageSignatureExtractionTool;

	//=============================================================================
	class ImageSignatureExtractionInterfaceABC: public I_InterfaceABC
	{
	public:
		ImageSignatureExtractionInterfaceABC();
		virtual ~ImageSignatureExtractionInterfaceABC() {};

		virtual void destroy() = 0;

		virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
			virtual ImageSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void) = 0;
		virtual int SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC
						*aImageSignatureDescriptorInterface) = 0;
		virtual unsigned long InitExtracting(void) = 0;
		virtual unsigned long StartExtracting(void) = 0;


		static const UUID myID;
	};

	//=============================================================================
	class ImageSignatureExtractionInterface: 
	  public ImageSignatureExtractionInterfaceABC
	{
	public:
		ImageSignatureExtractionInterface(ImageSignatureExtractionTool* aTool);
		virtual ~ImageSignatureExtractionInterface();
		
		virtual const UUID& GetInterfaceID(void);
		virtual const char *GetName(void);

		virtual void destroy();

		virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
			virtual ImageSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void);
		virtual int SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC
						  *aImageSignatureDescriptorInterface);
		virtual unsigned long InitExtracting(void);
		virtual unsigned long StartExtracting(void);

		static const UUID myID;
		static const char *myName;

	private:
		ImageSignatureExtractionTool *m_ExtractionTool;
	};

	//=============================================================================	
	class ImageSignatureExtractionTool: public DescriptorExtractor
	{
	friend class ImageSignatureExtractionInterface;
	public:
		ImageSignatureExtractionTool();
		ImageSignatureExtractionTool(ImageSignatureDescriptorInterfaceABC
									   *contourshape);

		virtual const UUID& GetObjectID(void);
		virtual const char *GetName(void);

		virtual bool SupportsPush(void);
		virtual bool SupportsPull(void);


		virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

	  virtual ImageSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void);
		virtual int SetDescriptorInterface(ImageSignatureDescriptorInterfaceABC
					   * aImageSignatureDescriptorInterface);
		
		virtual unsigned long InitExtracting(void);
		virtual unsigned long StartExtracting(void);

		virtual ImageSignatureExtractionInterfaceABC *GetInterface(void);

		// access is allowed only by class factories for this
		// object.  This avoids having to duplicate the
		// ID definition in multiple locations.  In the future, we may
		// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
		// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
		static const UUID myID;
		static const char * myName;

	private:
		virtual ~ImageSignatureExtractionTool();

		ImageSignatureExtractionInterface m_Interface;
		ImageSignatureDescriptorInterfaceABC *m_DescriptorInterface;
		MultiMediaInterfaceABC *m_Media;
		MomVop *m_ImageMedia;

		// Private functions used for extracting the image signature

		unsigned long ExtractSignatures();
		unsigned long ExtractSignature( ImageSignatureGlobalBits &bits );
		unsigned long ExtractARCSignature( ImageSignatureGlobalBits &bits );
		unsigned long ExtractFeatureSignature( ImageSignatureBits &bits );
		void FindSignature( CvMat *im, ImageSignatureRegionBits bits, unsigned char *select_bits );

		// Private functions
		void subCircle( IplImage* src, IplImage* dst );
		void subCircleFT( CvMat* src, CvMat* dst );
		void coneFPFT( CvMat *center_strip, IplImage *cone, int T, int cone_size, int type );
		void coneFP( CvMat *center_strip, IplImage *cone, int T, int cone_size, int type );
		void vilGrad(IplImage *in, IplImage *out);
		void MomImageToIplImage(MomImage *im_r, MomImage *im_g, MomImage *im_b, IplImage *dst);
		void trace( CvMat *src, CvMat *dst, CvMat *center_strip, CvMat *circles, int func, int type);
		void trace_FT( CvMat *src, CvMat *dst, CvMat *center_strip, CvMat *circles, int func, int type);
		void circus( CvMat *circusVector, double *hash );
		void imagetobit( IplImage *image, char *bitimage );
		void diametrical( const CvMat* src, CvMat* dst, int func );
		double mean(IplImage* src, int ncols, int nrows);
		double tracevector(double* vec, int vc, int traceflag);
		void vilModulus(IplImage* src, IplImage* dst);
		CvMat *lanczos ( float scale );
	};

};

int findFeaturesLaplacian( IplImage *src, IplImage *dst, IplImage *direction, int scale );


#endif //__IMAGE_SIGNATURE_EXTRACTION_H__
