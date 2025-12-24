//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
// Sharp corporation, Yasuaki Tokumo, Shuichi Watanabe.
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
// Applicable File Name:  MotionActivityExtraction.h
//
#ifndef __MotionActivityExtraction__
#define __MotionActivityExtraction__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "motion_activity_extraction.h"
#include "Media/Media.h"

namespace XM
{

// Forward Declarations:
class MotionActivityExtractionTool;

//=============================================================================
class MotionActivityExtractionInterfaceABC: public I_InterfaceABC
{
public:
	MotionActivityExtractionInterfaceABC();
	virtual ~MotionActivityExtractionInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual MotionActivityDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
				    *aMotionActivityDescriptorInterface) = 0;
	virtual unsigned long StartExtracting(void) = 0;
//	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long PostExtracting(void) = 0; // for SLMA

	static const UUID myID;
};

//=============================================================================
class MotionActivityExtractionInterface: 
  public MotionActivityExtractionInterfaceABC
{
public:
	MotionActivityExtractionInterface(MotionActivityExtractionTool* aTool);
	virtual ~MotionActivityExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual MotionActivityDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
					  *aMotionActivityDescriptorInterface);
	virtual unsigned long StartExtracting(void);
//	virtual unsigned long InitExtracting(void);
	virtual unsigned long PostExtracting(void); // for SLMA

	static const UUID myID;
	static const char *myName;

private:
	MotionActivityExtractionTool *m_ExtractionTool;
};

//=============================================================================
class MotionActivityExtractionTool: public DescriptorExtractor
{
friend class MotionActivityExtractionInterface;
public:
	MotionActivityExtractionTool();
	MotionActivityExtractionTool(MotionActivityDescriptorInterfaceABC
				     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

	virtual MotionActivityDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionActivityDescriptorInterfaceABC
				   * aMotionActivityDescriptorInterface);
	
	virtual unsigned long StartExtracting(void);
//	virtual unsigned long InitExtracting(void);
	virtual unsigned long PostExtracting(void); // for SLMA

	virtual MotionActivityExtractionInterfaceABC *GetInterface(void);


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~MotionActivityExtractionTool();

	MotionActivityExtractionInterface m_Interface;
	MotionActivityDescriptorInterfaceABC *m_DescriptorInterface;
	MomVop *m_Media;
	MotionActivityDescriptorInfo *m_ActivityInfo;
	
};

};

#endif //__MotionActivityExtraction__
