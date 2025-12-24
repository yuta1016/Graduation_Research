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
// Applicable File Name:  MotionTrajectorySearch.h
//
#ifndef __MOTIONTRAJECTORYSEARCH_H__
#define __MOTIONTRAJECTORYSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{

/* basic structures */
typedef struct VctDouble {
  double x,y,z;
} VctDouble;

/* Descriptor structure for original "C" routines */
typedef struct GenericTrajD {
  int SpatialReference; /* fixed camera: 0; moving camera, image ref: 1, moving camera, camera ref: 2 */
  unsigned long InstantNumber;
  int TimeStart;   /* miliseconds */ /* not used in XM yet: see with TimeDS */
  int TimeEnd;     /* miliseconds */ /* not used in XM yet: see with TimeDS */
  int *Times;
  VctDouble *Positions;   	/* normalized by image size : [-1,1] */
  VctDouble *Speeds; 		/* normalized by image size : [-1,1] */
  VctDouble *Accelerations;
  int CameraFollowed; 		/* 0 = no, 1 = yes */
  int *Visible;
} GenericTrajD;

// Forward Declarations:
class MotionTrajectorySearchTool;

//=============================================================================
class MotionTrajectorySearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	MotionTrajectorySearchInterfaceABC();
	virtual ~MotionTrajectorySearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface (MotionTrajectoryDescriptorInterfaceABC
	   *aMotionTrajectoryDescriptorInterface) = 0;
	virtual MotionTrajectoryDescriptorInterfaceABC* GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface (MotionTrajectoryDescriptorInterfaceABC
	   *aMotionTrajectoryDescriptorInterface) = 0;

	virtual double GetDistance(void) = 0;
	virtual double GetHighLevelFeatureMatch(void) = 0;

	virtual unsigned long SetQueryType(int queryType) = 0;
	virtual int GetQueryType(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MotionTrajectorySearchInterface:
  public MotionTrajectorySearchInterfaceABC
{
public:
	MotionTrajectorySearchInterface(MotionTrajectorySearchTool* aTool);
	virtual ~MotionTrajectorySearchInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int
	  SetRefDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
	   *aMotionTrajectoryDescriptorInterface);
	virtual MotionTrajectoryDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int
	  SetQueryDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				      *aMotionTrajectoryDescriptorInterface);

	virtual double GetDistance(void);
	virtual double GetHighLevelFeatureMatch(void);

	virtual unsigned long SetQueryType(int queryType);
	virtual int GetQueryType(void);

	static const UUID myID;
	static const char *myName;

private:
	MotionTrajectorySearchTool *m_SearchTool;
};

//=============================================================================
class MotionTrajectorySearchTool: public Search
{
friend class MotionTrajectorySearchInterface;
public:
	MotionTrajectorySearchTool();
	MotionTrajectorySearchTool(MotionTrajectoryDescriptorInterfaceABC
				 *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual MotionTrajectorySearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int 
	  SetRefDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				     *aMotionTrajectoryDescriptorInterface);
	virtual MotionTrajectoryDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int
	  SetQueryDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
				      *aMotionTrajectoryDescriptorInterface);

	virtual double GetDistance(void);
	virtual double GetHighLevelFeatureMatch(void);

	virtual unsigned long SetQueryType(int queryType);
	virtual int GetQueryType(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~MotionTrajectorySearchTool();

	MotionTrajectorySearchInterface m_Interface;
	MotionTrajectoryDescriptorInterfaceABC *m_RefDescriptorInterface;
	MotionTrajectoryDescriptorInterfaceABC *m_QueryDescriptorInterface;

        void KeyPointsAlign (GenericTrajD, GenericTrajD, GenericTrajD*, GenericTrajD*);
        void InterpolateTrajData(int, VctDouble, VctDouble, VctDouble, int, VctDouble*, VctDouble*, VctDouble*);
        void InitializeAlignedTrajFields(GenericTrajD, GenericTrajD);
	unsigned long CountAlignedKeyPointsNumber (GenericTrajD, GenericTrajD);
	void InitializeAlignedTimes (GenericTrajD, GenericTrajD, GenericTrajD, GenericTrajD);
	double match_D1D2_generic (GenericTrajD, GenericTrajD, float, float, float, float, float, float, float, float, float);
	void StartTimeAlign (GenericTrajD*, int);
        int PositiveDoubleToInt (double);
        void TimeNormalize (GenericTrajD*, int);

	int m_QueryType;
};

};


#endif //__MOTIONTRAJECTORYSEARCH_H__
