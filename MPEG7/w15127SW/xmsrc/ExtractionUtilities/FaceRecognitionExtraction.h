///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// (Panasonic Singapore Laboratories Pte Ltd)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  FaceRecognitionExtraction.h
//
#ifndef __FaceRecognitionEXTRACTION_H__
#define __FaceRecognitionEXTRACTION_H__


#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include <momusys.h>
#include <intra.h>
#include <ExtractionUtilities/face_detect.h>


//=============================================================================

namespace XM
{

// Forward Declarations:
class FaceRecognitionExtractionTool;

//=============================================================================
class FaceRecognitionExtractionInterfaceABC: public I_InterfaceABC
{
public:
	FaceRecognitionExtractionInterfaceABC();
	virtual ~FaceRecognitionExtractionInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetSourceMedia(MomVop *media) = 0;
	virtual FaceRecognitionDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
				    *aFaceRecognitionDescriptorInterface) = 0;
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class FaceRecognitionExtractionInterface: 
  public FaceRecognitionExtractionInterfaceABC
{
public:
	FaceRecognitionExtractionInterface(
			     FaceRecognitionExtractionTool* aTool);
	virtual ~FaceRecognitionExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MomVop *media);
	virtual FaceRecognitionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int
	  SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
				 *aFaceRecognitionDescriptorInterface);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	FaceRecognitionExtractionTool *m_ExtractionTool;
};

//=============================================================================
class FaceRecognitionExtractionTool: public DescriptorExtractor
{
friend class FaceRecognitionExtractionInterface;
public:
	
	FaceRecognitionExtractionTool();
	FaceRecognitionExtractionTool(
                             FaceRecognitionDescriptorInterfaceABC
			     *aDescriptorInterface);
	
	int round(double x);
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *media);
	virtual FaceRecognitionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int
	  SetDescriptorInterface(FaceRecognitionDescriptorInterfaceABC
				 *aFaceRecognitionDescriptorInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual FaceRecognitionExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT


	virtual bool FaceRecognitionFeatureGeneration(unsigned char* pImage_Y,
					     unsigned long image_width,
					     unsigned long image_height,
					     FRD* pFaceRecognition);
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	char eigenface[2576*48];
//	char eigenface[2576*49];
	unsigned char meanface[2576];

	static const UUID myID;
	static const char * myName;
private:
	virtual ~FaceRecognitionExtractionTool();
	FaceRecognitionExtractionInterface m_Interface;
	FaceRecognitionDescriptorInterfaceABC *m_DescriptorInterface;
	MomVop *m_Media;

	// Added for hard coded FaceRecognition eigenface and meanface parameters
	static char EigenfaceDataTable[2576*48];
	static unsigned char MeanfaceDataTable[2576];

	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
//	int qstep[49];

};

};

#endif //__FaceRecognitionEXTRACTION_H__
