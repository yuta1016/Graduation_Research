///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Philips Research USA, Sylvie Jeannin
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
// Applicable File Name:  MotionTrajectoryExtraction.h
//
#ifndef __MOTIONTRAJECTORYEXTRACTION_H__
#define __MOTIONTRAJECTORYEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"
#include "momusys.h"                                      /* Naming Convention */
#include "Media/Media.h"
#include "TemporalInterpolationExtraction.h"
//#include <intra.h>

namespace XM
{

// Forward Declarations:
class MotionTrajectoryExtractionTool;
//other classes needed - TBD

//=============================================================================
class MotionTrajectoryExtractionInterfaceABC: public I_InterfaceABC
{
public:
	MotionTrajectoryExtractionInterfaceABC();
	virtual ~MotionTrajectoryExtractionInterfaceABC() {};

	virtual void destroy()=0;

//	virtual int SetSourceMedia(T4DKeyPoint *media) = 0;
	virtual int SetSourceMedia(MultiMedia *media) = 0;
	virtual int SetSourceMediaInterface(MultiMediaInterfaceABC *mediaInterface) = 0;
	// I don't understand where following function should be used //sth
	virtual int SetSourceMediaKeyPointNumber(unsigned long CurrentPoint)=0;
	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		 *aMotionTrajectoryDescriptorInterface) = 0;

	virtual TemporalInterpolationExtractionInterfaceABC
	  *GetTemporalInterpolationExtractionInterface(void) = 0;
	virtual int
	  SetTemporalInterpolationExtractionInterface(
		 TemporalInterpolationExtractionInterfaceABC
		 *aTemporalInterpolationExtractionInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(int KeyPointsNumber) = 0;
	virtual unsigned long InterpolateKeypoints(int KeyPointsNumber) = 0;


	static const UUID myID;
};

//=============================================================================
class MotionTrajectoryExtractionInterface:
  public MotionTrajectoryExtractionInterfaceABC
{
public:
	MotionTrajectoryExtractionInterface(MotionTrajectoryExtractionTool* aTool);
	virtual ~MotionTrajectoryExtractionInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

//	virtual int SetSourceMedia(T4DKeyPoint *media);    //same comments as above
	virtual int SetSourceMedia(MultiMedia *media);
	virtual int SetSourceMediaInterface(MultiMediaInterfaceABC *mediaInterface);
	virtual int SetSourceMediaKeyPointNumber(unsigned long CurrentPoint);
	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
					  *aMotionTrajectoryDescriptorInterface);
	virtual TemporalInterpolationExtractionInterfaceABC
	  *GetTemporalInterpolationExtractionInterface(void);
	virtual int
	  SetTemporalInterpolationExtractionInterface(
             TemporalInterpolationExtractionInterfaceABC
	     *aTemporalInterpolationExtractionInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(int KeyPointsNumber);
	virtual unsigned long InterpolateKeypoints(int KeyPointsNumber);

	static const UUID myID;
	static const char *myName;

private:
	MotionTrajectoryExtractionTool *m_ExtractionTool;
};

//=============================================================================
class MotionTrajectoryExtractionTool: public DescriptorExtractor
{
friend class MotionTrajectoryExtractionInterface;
public:
	MotionTrajectoryExtractionTool();
	MotionTrajectoryExtractionTool(MotionTrajectoryDescriptorInterfaceABC
				     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

/*	virtual unsigned long SetSourceMedia(MediaDescriptor&,
	                                     XMStream& mediaitself);*/
	//virtual int SetSourceMedia(T4DKeyPoint *media);
	virtual int SetSourceMedia(MultiMedia *media);   // same comments as above
	virtual int SetSourceMediaInterface(MultiMediaInterfaceABC *mediaInterface);   // same comments as above
	virtual int SetSourceMediaKeyPointNumber(unsigned long CurrentPoint);

/*	virtual unsigned long ExtractFeature(ColorHistogramDescriptor* aDesc,
	             I_ExtractionInterfaceABC::ExtractionResult& aResult);*/
	virtual MotionTrajectoryDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				   * aMotionTrajectoryDescriptorInterface);
	virtual TemporalInterpolationExtractionInterfaceABC
	  *GetTemporalInterpolationExtractionInterface(void);
	virtual int
	  SetTemporalInterpolationExtractionInterface(
            TemporalInterpolationExtractionInterfaceABC
	    *aTemporalInterpolationExtractionInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(int KeyPointsNumber);
	virtual unsigned long InterpolateKeypoints(int KeyPointsNumber);

	virtual MotionTrajectoryExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~MotionTrajectoryExtractionTool();

	MotionTrajectoryExtractionInterface m_Interface;
	MotionTrajectoryDescriptorInterfaceABC *m_DescriptorInterface;
	TemporalInterpolationExtractionInterfaceABC *m_TemporalInterpolationExtractionInterface;
	//T4DKeyPoint *m_Media; // changed from Stephan again: SHOULD NOT BE 1 KEY POINT
	//MultiMedia *m_Media; // m_Media
	MultiMediaInterfaceABC *m_Media;
	unsigned long m_KeyPoint;
	int m_counter;

};

};

#endif //__MOTIONTRAJECTORYEXTRACTION_H__
