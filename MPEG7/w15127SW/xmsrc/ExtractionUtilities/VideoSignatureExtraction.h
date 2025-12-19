///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// P. Brasnett
// Mitsubishi Electric R&D Centre Europe
// K. Iwamoto, R. Oami
// NEC Corp.
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2009.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VideoSignatureExtraction.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VIDEO_SIGNATURE_EXTRACTION_H__
#define __VIDEO_SIGNATURE_EXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>
#include <list>


typedef struct _IplImage IplImage;


struct DIMENSION;
class CTernaryOps;


//=============================================================================
namespace XM
{


	// Forward Declarations:
	class VideoSignatureExtractionTool;

	//=============================================================================
	class VideoSignatureExtractionInterfaceABC: public I_InterfaceABC
	{
	public:
		VideoSignatureExtractionInterfaceABC();
		virtual ~VideoSignatureExtractionInterfaceABC() {};

		virtual void destroy() = 0;

		virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
			virtual VideoSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void) = 0;
		virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
						*aVideoSignatureDescriptorInterface) = 0;
		virtual unsigned long InitExtracting(void) = 0;
		virtual unsigned long StartExtracting(void) = 0;


		static const UUID myID;
	};

	//=============================================================================
	class VideoSignatureExtractionInterface: 
	  public VideoSignatureExtractionInterfaceABC
	{
	public:
		VideoSignatureExtractionInterface(VideoSignatureExtractionTool* aTool);
		virtual ~VideoSignatureExtractionInterface();
		
		virtual const UUID& GetInterfaceID(void);
		virtual const char *GetName(void);

		virtual void destroy();

		virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
			virtual VideoSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void);
		virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
						  *aVideoSignatureDescriptorInterface);
		virtual unsigned long InitExtracting(void);
		virtual unsigned long StartExtracting(void);

		static const UUID myID;
		static const char *myName;

	private:
		VideoSignatureExtractionTool *m_ExtractionTool;
	};

	//=============================================================================	
	class VideoSignatureExtractionTool: public DescriptorExtractor
	{
	friend class VideoSignatureExtractionInterface;
	public:
		VideoSignatureExtractionTool();
		VideoSignatureExtractionTool(VideoSignatureDescriptorInterfaceABC
									   *videosignature);

		virtual const UUID& GetObjectID(void);
		virtual const char *GetName(void);

		virtual bool SupportsPush(void);
		virtual bool SupportsPull(void);


		virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

	  virtual VideoSignatureDescriptorInterfaceABC *
			  GetDescriptorInterface(void);
		virtual int SetDescriptorInterface(VideoSignatureDescriptorInterfaceABC
					   * aVideoSignatureDescriptorInterface);
		
		virtual unsigned long InitExtracting(void);
		virtual unsigned long StartExtracting(void);

		virtual VideoSignatureExtractionInterfaceABC *GetInterface(void);

		// access is allowed only by class factories for this
		// object.  This avoids having to duplicate the
		// ID definition in multiple locations.  In the future, we may
		// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
		// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
		static const UUID myID;
		static const char * myName;

	private:
		virtual ~VideoSignatureExtractionTool();

		VideoSignatureExtractionInterface m_Interface;
		VideoSignatureDescriptorInterfaceABC *m_DescriptorInterface;
		MultiMediaInterfaceABC *m_Media;
		MomVop *m_ImageMedia;

		// Private functions used for extracting the video signature

		unsigned long ExtractSignatures();

		// Private functions
		int initDimensionList(vector<DIMENSION> &dimensionList);
		IplImage *processFrame(momvop *vop, int &frame_counter);
		int ExtractSignature( unsigned char *img, int imgWidth, int imgHeight, std::vector<DIMENSION> &dimensionList, unsigned char *signature, unsigned char &confidence, const CTernaryOps &ternary_ops );

		unsigned char *LUT_L1_DIST;

		void setLUT_L1_DIST();
		void freeLUT_L1_DIST();
		int L1Distance( unsigned char *signature1, unsigned char *signature2 );
	};

};

#endif //__VIDEO_SIGNATURE_EXTRACTION_H__
