//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  CameraMotionExtraction.h
//

#ifndef __CAMERA_MOTION_EXTRACTION__
#define __CAMERA_MOTION_EXTRACTION__

#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "camera_motion_extraction.h"
#include <momusys.h>
#include <intra.h>

namespace XM
{

// Forward Declarations:

class CameraMotionExtractionTool;

//=============================================================================

class CameraMotionExtractionInterfaceABC: public I_InterfaceABC
{
public:

	CameraMotionExtractionInterfaceABC();

	virtual void destroy() = 0;

	virtual ~CameraMotionExtractionInterfaceABC() {};
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual CameraMotionDescriptorInterfaceABC  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC
				    *aCameraMotionDescriptorInterface) = 0;

	virtual unsigned long StartExtracting(void) = 0;

	static const UUID myID;
};



//=============================================================================

class CameraMotionExtractionInterface: 

  public CameraMotionExtractionInterfaceABC

{

public:

	CameraMotionExtractionInterface(CameraMotionExtractionTool* aTool);
	virtual ~CameraMotionExtractionInterface();

	virtual void destroy() ;

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) ;
	virtual CameraMotionDescriptorInterfaceABC  *GetDescriptorInterface(void) ;
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC  *aCameraMotionDescriptorInterface);

	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:

	CameraMotionExtractionTool *m_ExtractionTool;

};



//=============================================================================

class CameraMotionExtractionTool: public DescriptorExtractor

{

	friend class CameraMotionExtractionInterface ;
public:

	CameraMotionExtractionTool();
	CameraMotionExtractionTool(CameraMotionDescriptorInterfaceABC *CameraMotion) ;

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) ;

	virtual CameraMotionDescriptorInterfaceABC  *GetDescriptorInterface(void) ;
	virtual int SetDescriptorInterface(CameraMotionDescriptorInterfaceABC
				   * aCameraMotionDescriptorInterface);

	virtual unsigned long StartExtracting(void);
	virtual CameraMotionExtractionInterfaceABC *GetInterface(void);


	static const UUID myID;
	static const char * myName;

private:

	virtual ~CameraMotionExtractionTool();

	CameraMotionExtractionInterface m_Interface;
	CameraMotionDescriptorInterfaceABC *m_DescriptorInterface;
	char * m_Media;   

};



};



#endif //__MotionActivityExtraction__

