///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ParametricObjectMotionExtraction.h
//
#ifndef __PARAMETRICOBJECTMOTIONEXTRACTION_H__
#define __PARAMETRICOBJECTMOTIONEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "momusys.h"

namespace XM
{


// Forward Declarations:
class ParametricObjectMotionExtractionTool;

//=============================================================================
class ParametricObjectMotionExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ParametricObjectMotionExtractionInterfaceABC();
	virtual ~ParametricObjectMotionExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MomVop *ReferenceVOP,
				   MomVop *CurrentVOP) = 0;
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(
                            ParametricObjectMotionDescriptorInterfaceABC
			    *aParametricObjectMotionDescriptorInterface) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class ParametricObjectMotionExtractionInterface:
	public ParametricObjectMotionExtractionInterfaceABC
{
public:
	ParametricObjectMotionExtractionInterface(
                        ParametricObjectMotionExtractionTool* aTool);
	virtual ~ParametricObjectMotionExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *ReferenceVOP, MomVop *CurrentVOP);
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
			 ParametricObjectMotionDescriptorInterfaceABC
			 *aParametricObjectMotionDescriptorInterface);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ParametricObjectMotionExtractionTool *m_ExtractionTool;
};

//=============================================================================
class ParametricObjectMotionExtractionTool: public DescriptorExtractor
{
friend class ParametricObjectMotionExtractionInterface;
public:
	ParametricObjectMotionExtractionTool();
	ParametricObjectMotionExtractionTool(
                             ParametricObjectMotionDescriptorInterfaceABC
			     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *ReferenceVOP, MomVop *CurrentVOP);
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
                         ParametricObjectMotionDescriptorInterfaceABC
			 *aParametricObjectMotionDescriptorInterface);
	
	virtual unsigned long StartExtracting(void);

	virtual ParametricObjectMotionExtractionInterfaceABC
	  *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ParametricObjectMotionExtractionTool();

	ParametricObjectMotionExtractionInterface m_Interface;
	ParametricObjectMotionDescriptorInterfaceABC *m_DescriptorInterface;
	MomVop *m_ReferenceVOP;
        MomVop *m_CurrentVOP;	
};

};

#endif //__PARAMETRICOBJECTMOTIONEXTRACTION_H__
