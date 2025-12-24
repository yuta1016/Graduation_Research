//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Philips Research USA, Sylvie Jeannin
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
// Applicable File Name:  MotionTrajectorySearch.cpp
//

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================


using namespace XM;

#define MAX_DIST DBL_MAX

const UUID MotionTrajectorySearchInterface::myID = UUID("");
const char *MotionTrajectorySearchInterface::myName = "MotionTrajectoryMatchingInterface";

const UUID MotionTrajectorySearchTool::myID = UUID("");
const char *MotionTrajectorySearchTool::myName = "MotionTrajectoryMatchingTool";

const UUID MotionTrajectorySearchInterfaceABC::myID = UUID();

//=============================================================================
MotionTrajectorySearchInterfaceABC::MotionTrajectorySearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MotionTrajectorySearchInterface::MotionTrajectorySearchInterface(MotionTrajectorySearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionTrajectorySearchInterface::~MotionTrajectorySearchInterface()
{
}

//----------------------------------------------------------------------------
void MotionTrajectorySearchInterface::destroy(void)
{ delete m_SearchTool; }

//----------------------------------------------------------------------------
int MotionTrajectorySearchInterface::
SetRefDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *RefMotionTrajectoryDescriptorInterface)

{
  return m_SearchTool->
    SetRefDescriptorInterface(RefMotionTrajectoryDescriptorInterface);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC* MotionTrajectorySearchInterface::
GetQueryDescriptorInterface(void)

{
  return m_SearchTool->GetQueryDescriptorInterface();
}

//----------------------------------------------------------------------------
int MotionTrajectorySearchInterface::
SetQueryDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *QueryMotionTrajectoryDescriptorInterface)

{
  return m_SearchTool->
    SetQueryDescriptorInterface(QueryMotionTrajectoryDescriptorInterface);
}

//----------------------------------------------------------------------------
double MotionTrajectorySearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
double MotionTrajectorySearchInterface::GetHighLevelFeatureMatch(void)
{
  return m_SearchTool->GetHighLevelFeatureMatch();
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectorySearchInterface::SetQueryType(int queryType)
{
  return m_SearchTool->SetQueryType(queryType);
}

//----------------------------------------------------------------------------
int MotionTrajectorySearchInterface::GetQueryType(void)
{
  return m_SearchTool->GetQueryType();
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectorySearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectorySearchInterface::GetName(void)
{ return myName; }


//=============================================================================
MotionTrajectorySearchTool::MotionTrajectorySearchTool():
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_QueryType(0)

{
	SetExposedInterface(&m_Interface);
}

//=============================================================================
MotionTrajectorySearchTool::
MotionTrajectorySearchTool(MotionTrajectoryDescriptorInterfaceABC
			 *aQueryDescriptorInterface):
m_Interface(this),
m_RefDescriptorInterface(0),
m_QueryDescriptorInterface(0),
m_QueryType(0)

{
#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

	/* create descriptor if it does not exist*/
	if (!aQueryDescriptorInterface) {
//COORD	  MotionTrajectoryDescriptor *descriptor =
//COORD	    new MotionTrajectoryDescriptor(0,0);
	  MotionTrajectoryDescriptor *descriptor =	//COORD
	    new MotionTrajectoryDescriptor(0);		//COORD
	  aQueryDescriptorInterface=descriptor->GetInterface();
	}

	/* connect Descritors with coding schemes*/
	SetQueryDescriptorInterface(aQueryDescriptorInterface);

	/* no sub matching tools */

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
MotionTrajectorySearchTool::~MotionTrajectorySearchTool()
{
	/* release descriptor*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectorySearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectorySearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MotionTrajectorySearchTool::SupportsPush(void)
{ return true; }    // ?

//----------------------------------------------------------------------------
bool MotionTrajectorySearchTool::SupportsPull(void)
{ return false; }   // ?

//----------------------------------------------------------------------------
int MotionTrajectorySearchTool::
SetRefDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aRefMotionTrajectoryDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_RefDescriptorInterface ==
	    aRefMotionTrajectoryDescriptorInterface) return 0;

	/* release old interface*/
	if (m_RefDescriptorInterface)
	  m_RefDescriptorInterface->release();

	/* add new interface*/
	m_RefDescriptorInterface =
	  aRefMotionTrajectoryDescriptorInterface;
	if (m_RefDescriptorInterface) {
	  m_RefDescriptorInterface->addref();

	  /* no sub search tools to be connected */
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from
	     sub search tools*/
	  /* not available*/
	  return -1;
	}

        return 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC* MotionTrajectorySearchTool::
GetQueryDescriptorInterface(void)

{
        return m_QueryDescriptorInterface;
}

//----------------------------------------------------------------------------
int  MotionTrajectorySearchTool::
SetQueryDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aQueryMotionTrajectoryDescriptorInterface)

{
        /* check if new value is different from old value*/
        if (m_QueryDescriptorInterface ==
	    aQueryMotionTrajectoryDescriptorInterface) return 0;

	/* release old interface*/
	if (m_QueryDescriptorInterface)
	  m_QueryDescriptorInterface->release();

	/* add new interface*/
	m_QueryDescriptorInterface =
	  aQueryMotionTrajectoryDescriptorInterface;
	if (m_QueryDescriptorInterface) {
	  m_QueryDescriptorInterface->addref();

	  /* conect sub descriptors with sub matching tools */
	  /* not available*/
	}
	else {
	  /* if no descriptor, release also descriptor interfaces from sub matching tools*/
	  /* not available*/
	  return -1;
	}

        return 0;
}

//----------------------------------------------------------------------------
// Several distance calculation functions have to be implemented depending on the application
// e.g.
// for time: comparing traj with aligning their beginning or not; with normalizing their total
// duration or not
// for the rest: customizing the weight between acceleration, speed, positions as the appli may
// be interested only in speed but not in position, etc...
// only one is given here for now. Times are not aligned.

double MotionTrajectorySearchTool::GetDistance(void)

{
        double matching_res;
        GenericTrajD D1, D2, alignedD1, alignedD2;
        float w_x, w_y, w_z, w_vx, w_vy, w_vz, w_ax, w_ay, w_az;
        unsigned long c_kp;
	int delta_t;
	int D1_constant_flag, D2_constant_flag;
	double D1_duration, D2_duration, D1_time_interval, D2_time_interval;

        /* check for descriptors*/
        if (!m_RefDescriptorInterface) return DBL_MAX;
        if (!m_QueryDescriptorInterface) return DBL_MAX;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "MotionTrajectoryDescriptorInterface") != 0) {
			   std::cerr << "RefName" << m_RefDescriptorInterface->GetName() << "\n";
	  return DBL_MAX;
	}
	if (strcmp(m_QueryDescriptorInterface->GetName(),
		   "MotionTrajectoryDescriptorInterface") != 0) {
			   std::cerr << "QueryName" << m_QueryDescriptorInterface->GetName() << "\n";
	  return DBL_MAX;
	}

	/* check for TemporalInterpolation descriptor*/
	if (!m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface())
	  return DBL_MAX;
	if (!m_QueryDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface())
	  return DBL_MAX;

        /* ? specifications of some different possible queries */
	switch ( GetQueryType() ) {
	case 0:   /* query on whole trajectory elements, equally */
	case 4:
	   w_x = w_y = w_z = 1.0;
	   w_vx = w_vy = w_vz = 1.0;
	   w_ax = w_ay = w_az = 1.0;
	   break;
	case 1:   /* query by position only */
	case 5:
	case 8:
	   w_x = w_y = w_z = 1.0;
	   w_vx = w_vy = w_vz = 0.0;
	   w_ax = w_ay = w_az = 0.0;
	   break;
	case 2:   /* query by speed only */
	case 6:
	   w_x = w_y = w_z = 0.0;
	   w_vx = w_vy = w_vz = 1.0;
	   w_ax = w_ay = w_az = 0.0;
	   break;
	case 3:   /* query by acceleration only */
	case 7:
	   w_x = w_y = w_z = 0.0;
	   w_vx = w_vy = w_vz = 0.0;
	   w_ax = w_ay = w_az = 1.0;
	   break;
	default:
	   break;
	}
        if ( (m_RefDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->GetDimension()==2)
          || (m_QueryDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->
	      GetDimension()==2) )
           w_z = w_vz = w_az = 0.0;

        /* interface XM structures to "original C" ones */
        /* note: 	TimeStart & TimeEnd not initialized
        		FunctionID = 3 (not determined) not handeld */

	/* CameraFollowed */
	D1.CameraFollowed = m_RefDescriptorInterface->GetCameraFollows();
	D2.CameraFollowed = m_QueryDescriptorInterface->GetCameraFollows();

	/* SpatialReference */
        //D1.SpatialReference = m_RefDescriptorInterface->GetCoordSystem();
        //D2.SpatialReference = m_QueryDescriptorInterface->GetCoordSystem();
        D1.SpatialReference = 0; //to  change with above (test)
        D2.SpatialReference = 0; //to  change with above (test)

	/* InstantNumber */
        D1.InstantNumber = m_RefDescriptorInterface->
	  GetTemporalInterpolationDescriptorInterface()->GetNumOfKeyPoint();
        D2.InstantNumber = m_QueryDescriptorInterface->
	  GetTemporalInterpolationDescriptorInterface()->GetNumOfKeyPoint();

	/* Corresponding allocations */
	D1.Times = (int *) calloc (D1.InstantNumber, sizeof(int));
	D2.Times = (int *) calloc (D2.InstantNumber, sizeof(int));
	D1.Visible = (int *) calloc (D1.InstantNumber, sizeof(int));
	D2.Visible = (int *) calloc (D2.InstantNumber, sizeof(int));
	D1.Positions = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));
	D2.Positions = (VctDouble *) calloc (D2.InstantNumber, sizeof(VctDouble));
	D1.Speeds = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));
	D2.Speeds = (VctDouble *) calloc (D2.InstantNumber, sizeof(VctDouble));
	D1.Accelerations = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));
	D2.Accelerations = (VctDouble *) calloc (D2.InstantNumber, sizeof(VctDouble));

	/* time-related initialization */
	if (m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetConstantTimeInterval()) {
	  D1_constant_flag = 1;
	  D1_duration = m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetMediaDuration();
	  D1_time_interval = D1_duration / (D1.InstantNumber-1);
	}
	else D1_constant_flag = 0;
	if (m_QueryDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetConstantTimeInterval()) {
	  D2_constant_flag = 1;
	  D2_duration = m_QueryDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetMediaDuration();
	  D2_time_interval = D2_duration / (D2.InstantNumber-1);
	}
	else D2_constant_flag = 0;

	/* Times and Positions - x,y,z */
        for (c_kp=0; c_kp<D1.InstantNumber; c_kp++) {
	  if (D1_constant_flag)
	    (D1.Times)[c_kp] = (int)(c_kp * D1_time_interval);
	  else 
	    (D1.Times)[c_kp] = (int) m_RefDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->
	      GetMediaTimePoint(c_kp);
          (D1.Visible)[c_kp] = 1;
          ((D1.Positions)[c_kp]).x = m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetKeyValue(0,c_kp);
          ((D1.Positions)[c_kp]).y = m_RefDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetKeyValue(1,c_kp);
          if ((m_RefDescriptorInterface->
	       GetTemporalInterpolationDescriptorInterface()->
	       GetDimension()==3) )
	    ((D1.Positions)[c_kp]).z = m_RefDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->
	      GetKeyValue(2,c_kp);
        }
        for (c_kp=0; c_kp<D2.InstantNumber; c_kp++) {
	  if (D2_constant_flag)
	    (D2.Times)[c_kp] = (int)(c_kp * D2_time_interval);
	  else 
	    (D2.Times)[c_kp] = (int) m_QueryDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->
	      GetMediaTimePoint(c_kp);
          (D2.Visible)[c_kp] = 1;
          ((D2.Positions)[c_kp]).x = m_QueryDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetKeyValue(0,c_kp);
          ((D2.Positions)[c_kp]).y = m_QueryDescriptorInterface->
	    GetTemporalInterpolationDescriptorInterface()->
	    GetKeyValue(1,c_kp);
          if ((m_QueryDescriptorInterface->
	       GetTemporalInterpolationDescriptorInterface()->
	       GetDimension()==3) )
	    ((D2.Positions)[c_kp]).z = m_QueryDescriptorInterface->
	      GetTemporalInterpolationDescriptorInterface()->
	      GetKeyValue(2,c_kp);
        }

        /* Speeds and Accelerations - x,y,z */
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(0) ) { /* use linear interpolation */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t;
             ((D1.Accelerations)[c_kp]).x = 0.0;
          }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t;
                ((D1.Accelerations)[c_kp]).x = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).x = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(0,c_kp);
		((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).x * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(0,c_kp) == -1)
                  (D1.Positions)[c_kp].x = -1.0;
        }
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(1) ) {
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t;
             ((D1.Accelerations)[c_kp]).y = 0.0;
           }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t;
                ((D1.Accelerations)[c_kp]).y = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).y = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(1,c_kp);
      		((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).y * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 3)
                  (D1.Positions)[c_kp].y = -1.0;
        }
        if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDimension()==3)
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(2) ) {
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t;
             ((D1.Accelerations)[c_kp]).z = 0.0;
           }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t;
                ((D1.Accelerations)[c_kp]).z = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).z = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(2,c_kp);
		((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).z * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
           if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 3)
                (D1.Positions)[c_kp].z = -1.0;
        }

        if ( m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(0) ) {
        /* use linear interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             ((D2.Speeds)[c_kp]).x = ( ((D2.Positions)[c_kp+1]).x - ((D2.Positions)[c_kp]).x ) / delta_t;
             ((D2.Accelerations)[c_kp]).x = 0.0;
          }
        }
        else {/* use 2nd order interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 1) {
                ((D2.Speeds)[c_kp]).x = ( ((D2.Positions)[c_kp+1]).x - ((D2.Positions)[c_kp]).x ) / delta_t;
                ((D2.Accelerations)[c_kp]).x = 0.0;
             }
             else if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 2) {
                ((D2.Accelerations)[c_kp]).x = m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(0,c_kp);
		((D2.Speeds)[c_kp]).x = ( ((D2.Positions)[c_kp+1]).x - ((D2.Positions)[c_kp]).x ) / delta_t - 0.5 * ((D2.Accelerations)[c_kp]).x * delta_t;
             }
          }
        }   /* correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 3)
                  (D2.Positions)[c_kp].x = -1.0;
        }
        if ( m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(1) ) {
        /* use linear interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             ((D2.Speeds)[c_kp]).y = ( ((D2.Positions)[c_kp+1]).y - ((D2.Positions)[c_kp]).y ) / delta_t;
             ((D2.Accelerations)[c_kp]).y = 0.0;
          }
        }
        else {/* use 2nd order interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 1) {
                ((D2.Speeds)[c_kp]).y = ( ((D2.Positions)[c_kp+1]).y - ((D2.Positions)[c_kp]).y ) / delta_t;
                ((D2.Accelerations)[c_kp]).y = 0.0;
             }
             else if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 2) {
                ((D2.Accelerations)[c_kp]).y = m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(1,c_kp);
      		((D2.Speeds)[c_kp]).y = ( ((D2.Positions)[c_kp+1]).y - ((D2.Positions)[c_kp]).y ) / delta_t - 0.5 * ((D2.Accelerations)[c_kp]).y * delta_t;
             }
          }
        }   /* correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 3)
                  (D2.Positions)[c_kp].y = -1.0;
        }
        if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDimension()==3)
        if ( m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(2) ) {
         /* use linear interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             ((D2.Speeds)[c_kp]).z = ( ((D2.Positions)[c_kp+1]).z - ((D2.Positions)[c_kp]).z ) / delta_t;
             ((D2.Accelerations)[c_kp]).z = 0.0;
          }
        }
        else {/* use 2nd order interpolation */
          for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
             delta_t = (D2.Times)[c_kp+1] - (D2.Times)[c_kp];
             if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 1) {
                ((D2.Speeds)[c_kp]).z = ( ((D2.Positions)[c_kp+1]).z - ((D2.Positions)[c_kp]).z ) / delta_t;
                ((D2.Accelerations)[c_kp]).z = 0.0;
             }
             else if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 2) {
                ((D2.Accelerations)[c_kp]).z = m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(2,c_kp);
		((D2.Speeds)[c_kp]).z = ( ((D2.Positions)[c_kp+1]).z - ((D2.Positions)[c_kp]).z ) / delta_t - 0.5 * ((D2.Accelerations)[c_kp]).z * delta_t;
             }
          }
        }   /* correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D2.InstantNumber-1); c_kp++) {
	  if (m_QueryDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 3)
                  (D2.Positions)[c_kp].z = -1.0;
        }

	//int bibi;
	//cout << "Print tests in matching\n";
        //cout << "Reference\n";
        //for (bibi=0; bibi<D1.InstantNumber; bibi++)
        //cout << (D1.Times)[bibi] << "  " << (D1.Positions)[bibi].x << "  " << (D1.Positions)[bibi].y << "  " << (D1.Positions)[bibi].z << "\n";
        //cout << "Query\n";
        //for (bibi=0; bibi<D2.InstantNumber; bibi++)
        //cout << (D2.Times)[bibi] << "  " << (D2.Positions)[bibi].x << "  " << (D2.Positions)[bibi].y << "  " << (D2.Positions)[bibi].z << "\n";

	/* if query has to be done with aligned start times:
	align start time of reference D on the start time of the query */
	if ( (GetQueryType() == 4) || (GetQueryType() == 5) || (GetQueryType() == 6)
   	  || (GetQueryType() == 7) || (GetQueryType() == 8) )
	  StartTimeAlign (&D1, D2.Times[0]);

	/* if query requires time normalization, do it */
	if ( (GetQueryType() == 8) )
	  TimeNormalize (&D1, (D2.Times[D2.InstantNumber-1]-D2.Times[0]));

	/* align times to be able to do the matching */
	KeyPointsAlign (D1, D2, &alignedD1, &alignedD2);

	/* perform matching*/
        matching_res = match_D1D2_generic (alignedD1, alignedD2, w_x, w_y, w_z, w_vx, w_vy, w_vz, w_ax, w_ay, w_az);

        return matching_res;
}

//----------------------------------------------------------------------------
double MotionTrajectorySearchTool::GetHighLevelFeatureMatch(void)

{
        double res;
        GenericTrajD D1;
        unsigned long c_kp;
	int delta_t;

        /* check for descriptors*/
        if (!m_RefDescriptorInterface) return DBL_MAX;

	if (strcmp(m_RefDescriptorInterface->GetName(),
		   "MotionTrajectoryDescriptorInterface") != 0) {
			   std::cout << "RefName" << m_RefDescriptorInterface->GetName() << "\n";
	  return DBL_MAX;
	}

	/* check for TemporalInterpolation descriptor*/
	if (!m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface())
	  return DBL_MAX;

	/* CameraFollowed */
	D1.CameraFollowed = m_RefDescriptorInterface->GetCameraFollows();

	/* SpatialReference */
        //D1.SpatialReference = m_RefDescriptorInterface->GetCoordSystem();
        D1.SpatialReference = 0; //to be updated when coords integrated

	/* InstantNumber */
        D1.InstantNumber = m_RefDescriptorInterface->
	  GetTemporalInterpolationDescriptorInterface()->GetNumOfKeyPoint();

	/* Corresponding allocations */
	D1.Times = (int *) calloc (D1.InstantNumber, sizeof(int));
	D1.Visible = (int *) calloc (D1.InstantNumber, sizeof(int));
	D1.Positions = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));
	D1.Speeds = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));
	D1.Accelerations = (VctDouble *) calloc (D1.InstantNumber, sizeof(VctDouble));

	/* Times and Positions - x,y,z */
        for (c_kp=0; c_kp<D1.InstantNumber; c_kp++) {
          (D1.Times)[c_kp] = (int) m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetMediaTimePoint(c_kp);
          (D1.Visible)[c_kp] = 1;
          ((D1.Positions)[c_kp]).x = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(0,c_kp);
          ((D1.Positions)[c_kp]).y = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(1,c_kp);
          if ((m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDimension()==3) )
             ((D1.Positions)[c_kp]).z = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetKeyValue(2,c_kp);
        }

        /* Speeds and Accelerations - x,y,z */
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(0) ) { /* use linear interpolation */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t;
             ((D1.Accelerations)[c_kp]).x = 0.0;
          }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t;
                ((D1.Accelerations)[c_kp]).x = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(0,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).x = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(0,c_kp);
		((D1.Speeds)[c_kp]).x = ( ((D1.Positions)[c_kp+1]).x - ((D1.Positions)[c_kp]).x ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).x * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(0,c_kp) == -1)
                  (D1.Positions)[c_kp].x = -1.0;
        }
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(1) ) {
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t;
             ((D1.Accelerations)[c_kp]).y = 0.0;
           }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t;
                ((D1.Accelerations)[c_kp]).y = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).y = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(1,c_kp);
      		((D1.Speeds)[c_kp]).y = ( ((D1.Positions)[c_kp+1]).y - ((D1.Positions)[c_kp]).y ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).y * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(1,c_kp) == 3)
                  (D1.Positions)[c_kp].y = -1.0;
        }
        if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDimension()==3)
        if ( m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetDefaultFunction(2) ) {
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             ((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t;
             ((D1.Accelerations)[c_kp]).z = 0.0;
           }
        }
        else {  /* use second order function */
          for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
             delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
             if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 1) {
                ((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t;
                ((D1.Accelerations)[c_kp]).z = 0.0;
             }
             else if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 2) {
                ((D1.Accelerations)[c_kp]).z = m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetParam(2,c_kp);
		((D1.Speeds)[c_kp]).z = ( ((D1.Positions)[c_kp+1]).z - ((D1.Positions)[c_kp]).z ) / delta_t - 0.5 * ((D1.Accelerations)[c_kp]).z * delta_t;
             }
          }
        } /* D1 speeds and accelerations finished; correct positions for ambiguous trajs */
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
           if (m_RefDescriptorInterface->GetTemporalInterpolationDescriptorInterface()->GetType(2,c_kp) == 3)
                (D1.Positions)[c_kp].z = -1.0;
        }

   	double vx, vy, vx_norm, vy_norm;
   	int time_up = 0, time_down = 0, time_right = 0, time_left = 0;
   	double v_up = 0, v_down = 0, v_left = 0, v_right = 0;
        for (c_kp=0; c_kp<(D1.InstantNumber-1); c_kp++) {
          delta_t = (D1.Times)[c_kp+1] - (D1.Times)[c_kp];
	  vx = (D1.Speeds)[c_kp].x;	  vy = (D1.Speeds)[c_kp].y;
          if ( vx >= 0.0 )  vx_norm = vx;
          else vx_norm = -1.0 * vx;
          if ( vy >= 0.0 )  vy_norm = vy;
          else vy_norm = -1.0 * vy;

          if ( vx_norm >= vy_norm ) {
             if (vx > 0) {
                time_right += delta_t;
                v_right += delta_t * vx_norm;
             }
             if (vx < 0) {
                time_left += delta_t;
                v_left += delta_t * vx_norm;
             }
          }
          if ( vy_norm >= vx_norm ) {
             if (vy > 0) {
                time_down += delta_t;
                v_down += delta_t * vy_norm;
             }
             if (vy < 0) {
                time_up += delta_t;
                v_up += delta_t * vy_norm;
             }
          }
        }

	switch ( GetQueryType() ) {
	case 11:   /* query for objects moving right */
	  res = 1.0 - ((double)time_right / ((D1.Times)[D1.InstantNumber-1] - (D1.Times)[0]));
	  break;
	case 12:   /* query for objects moving left */
	  res = 1.0 - ((double)time_left / ((D1.Times)[D1.InstantNumber-1] - (D1.Times)[0]));
	   break;
	case 13:   /* query for objects moving up */
	  res = 1.0 - ((double)time_up / ((D1.Times)[D1.InstantNumber-1] - (D1.Times)[0]));
	  break;
	case 14:   /* query for objects moving down */
	  res = 1.0 - ((double)time_down / ((D1.Times)[D1.InstantNumber-1] - (D1.Times)[0]));
	  break;
	default:
	   break;
	}

        return res;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectorySearchTool::SetQueryType(int queryType)
{
  return m_QueryType=queryType;
}

//----------------------------------------------------------------------------
int MotionTrajectorySearchTool::GetQueryType(void)
{
  return m_QueryType;
}

//----------------------------------------------------------------------------
MotionTrajectorySearchInterfaceABC *MotionTrajectorySearchTool::GetInterface(void)
{ return &m_Interface; }


//----------------------------------------------------------------------------
// original "C" routines for matching

/* to have Ds aligned in start times */
void MotionTrajectorySearchTool::StartTimeAlign (GenericTrajD *InD, int NewStartTime)
{

  unsigned long i;
  int delta_t;

  if (InD->Times[0] == NewStartTime)
    return;

  delta_t = InD->Times[0] - NewStartTime;

  for (i=0; i<InD->InstantNumber; i++) {
    InD->Times[i] -= delta_t;
  }
  return;

}

int MotionTrajectorySearchTool::PositiveDoubleToInt (double inDouble)
{

  int quantized_in;

  quantized_in = (int) inDouble;
  if ( (inDouble - quantized_in) <= 0.5 )
    return (quantized_in);
  else
    return (quantized_in+1);

}

void MotionTrajectorySearchTool::TimeNormalize (GenericTrajD *InD, int newDuration)
{

  int oldDuration, newInterval;
  double ratioDurations;
  unsigned long i;

  oldDuration = InD->Times[InD->InstantNumber-1]-InD->Times[0];
  if (oldDuration == newDuration)
    return;

  ratioDurations = (double) newDuration / oldDuration;

  for (i=1; i<InD->InstantNumber; i++) {
    newInterval = PositiveDoubleToInt ( ratioDurations * (InD->Times[i] - InD->Times[0]) );
    InD->Times[i] = newInterval + InD->Times[0];
  }

}

void MotionTrajectorySearchTool::KeyPointsAlign (GenericTrajD InD1, GenericTrajD InD2, GenericTrajD *OutD1, GenericTrajD *OutD2)
/* both Ds need to be time aligned because then positions need to be interpolated */
{
  unsigned long OutKeyPointNumber;

/* count new key points number */
  OutKeyPointNumber = CountAlignedKeyPointsNumber (InD1, InD2);

/* allocate memory for new Ds */
  /* times */
  OutD1->Times = (int *) calloc (OutKeyPointNumber, sizeof (int));
  OutD2->Times = (int *) calloc (OutKeyPointNumber, sizeof (int));
  /* positions */
  OutD1->Positions = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  OutD2->Positions = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  /* speeds */
  OutD1->Speeds = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  OutD2->Speeds = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  /* accelerations */
  OutD1->Accelerations = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  OutD2->Accelerations = (VctDouble *) calloc (OutKeyPointNumber, sizeof (VctDouble));
  /* visibility */
  OutD1->Visible = (int *) calloc (OutKeyPointNumber, sizeof (int));
  OutD2->Visible = (int *) calloc (OutKeyPointNumber, sizeof (int));

/* fill values of Ds fields */

  /* SpatialReference */
  OutD1->SpatialReference = InD1.SpatialReference;
  OutD2->SpatialReference = InD2.SpatialReference;

  /* InstantNumber */
  OutD1->InstantNumber = OutKeyPointNumber;
  OutD2->InstantNumber = OutKeyPointNumber;

  /* TimeStart */
  if (InD1.TimeStart < InD2.TimeStart) {
    OutD1->TimeStart = InD1.TimeStart;
    OutD2->TimeStart = InD1.TimeStart;
  }
  else {
    OutD1->TimeStart = InD2.TimeStart;
    OutD2->TimeStart = InD2.TimeStart;
  }

  /* TimeEnd */
  if (InD1.TimeEnd > InD2.TimeEnd) {
    OutD1->TimeEnd = InD1.TimeEnd;
    OutD2->TimeEnd = InD1.TimeEnd;
  }
  else {
    OutD1->TimeEnd = InD2.TimeEnd;
    OutD2->TimeEnd = InD2.TimeEnd;
  }

  /* Times */
  InitializeAlignedTimes (InD1, InD2, *OutD1, *OutD2);
  /* Positions Speeds */
  InitializeAlignedTrajFields (InD1, *OutD1);
  InitializeAlignedTrajFields (InD2, *OutD2);

}

//----------------------------------------------------------------------------
void MotionTrajectorySearchTool::InterpolateTrajData(int t_start, VctDouble p, VctDouble v, VctDouble a, int t, VctDouble *p_res, VctDouble *v_res, VctDouble *a_res)
{
   int dt;

   dt = t - t_start;

   /* positions */
   if (p.x != -1.0)
      p_res->x = p.x + v.x * dt + 0.5 * a.x * dt * dt;
   else p_res->x = -1.0;
   if (p.y != -1.0)
      p_res->y = p.y + v.y * dt + 0.5 * a.y * dt * dt;
   else p_res->y = -1.0;
   if (p.z != -1.0)
      p_res->z = p.z + v.z * dt + 0.5 * a.z * dt * dt;
   else p_res->z = -1.0;

   /* speeds */
   v_res->x = v.x + a.x * dt;
   v_res->y = v.y + a.y * dt;
   v_res->z = v.z + a.z * dt;

   /* accelerations */
   a_res->x = a.x;
   a_res->y = a.y;
   a_res->z = a.z;
}

//----------------------------------------------------------------------------
void MotionTrajectorySearchTool::InitializeAlignedTrajFields(GenericTrajD InD, GenericTrajD OutD)
{
  unsigned long c_kpi, c_kpo;
  VctDouble *c_pi, *c_si, *c_ai, *c_po, *c_so, *c_ao;
  int *c_ti, *c_to;
  int *c_vi, *c_vo;

  /* initializations */
  c_ti = InD.Times;
  c_pi = InD.Positions;
  c_si = InD.Speeds;
  c_ai = InD.Accelerations;
  c_to = OutD.Times;
  c_po = OutD.Positions;
  c_so = OutD.Speeds;
  c_ao = OutD.Accelerations;
  c_vi = InD.Visible;
  c_vo = OutD.Visible;
  c_kpo = 1;

  /* first values if aligned D starts before original one */
  while ( (*c_to) < *(c_ti) ) {
        *c_vo = 0;  /* mark invisible */
        c_to++;  /* go to next interval in new D */
        c_po++;
        c_so++;
        c_ao++;
        c_vo++;
        c_kpo++;
  }

  for (c_kpi=1; c_kpi<InD.InstantNumber; c_kpi++) {   /* for each interval of original D */
     while ((*c_to >= *(c_ti)) && (*c_to < *(c_ti+1)) ) {  /* while new interval is in this one */
        InterpolateTrajData(*c_ti, *c_pi, *c_si, *c_ai, *c_to, c_po, c_so, c_ao); /* interpolate */
        *c_vo = *c_vi; /* keep same visibility */
        c_to++;  /* go to next interval in new D */
        c_po++;
        c_so++;
        c_ao++;
        c_vo++;
        c_kpo++;
     }
     /* go to next interval in original D */
     c_ti++;
     c_pi++;
     c_si++;
     c_ai++;
     c_vi++;
  }

  /* last values: should be non visible */
  while (c_kpo < OutD.InstantNumber) {
     *c_vo = 0;
     c_vo++;
     c_kpo++;
  }

}

//----------------------------------------------------------------------------
unsigned long MotionTrajectorySearchTool::CountAlignedKeyPointsNumber (GenericTrajD D1, GenericTrajD D2)
/* count all different key points between the 2 Descriptors */
{
  unsigned long res; 		/* final key points number */
  int *D1t, *D2t;       /* pointers on time positions of keypoints */
  unsigned long c_kp1, c_kp2;     /* current number of key points for D1 and D2 */

  /* checks */
  if ( (D1.InstantNumber == 0) || (D2.InstantNumber == 0) )    /* check at least 1 k.p. */
    exit(0);

  /* initializations */
  res = 0; 		/* final number of key points */
  c_kp1 = c_kp2 = 1;    /* start with the first key point of each D */
  D1t = D1.Times;       /* initialize pointesr accordingly */
  D2t = D2.Times;

  while (*D1t < *D2t) {  /* count key points of D1 before D2 starts - if exists */
    res++;  /* count this D1 point */
    if (c_kp1<D1.InstantNumber) {  /* go to next D1 point */
    	D1t++;
    	c_kp1++;
    }
    else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	res += (D2.InstantNumber - c_kp2 +1);
    	return (res);
    }
  } /* current D1 key point is now not before first D2 key point */

  while (c_kp2 < D2.InstantNumber) {  /* count key points of D1 while D2 also exists */
    if (*D1t == *D2t) {   /* go to next time for both Ds */
        res++;	/* count this common D1/D2 point */
    	D2t++;  /* go to next D2 point */
    	c_kp2++;
    	if (c_kp1<D1.InstantNumber) { /* go to next D1 point */
    	    D1t++;
    	    c_kp1++;
        }
        else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	    res += (D2.InstantNumber - c_kp2 +1);
    	    return (res);
        }
        while (*D1t < *D2t) {   /* current D1 key point is before new current D2 key point */
           res++;  /* count this D1 point */
           if (c_kp1<D1.InstantNumber) { /* go to next D1 point */
    	      D1t++;
    	      c_kp1++;
    	   }
           else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	      res += (D2.InstantNumber - c_kp2 +1);
    	      return (res);
           }
        }
    }
    else {   /* current D1 key point is after current D2 key point */
   	res++;	/* count this D2 point */
   	D2t++;  /* go to next D2 key point */
    	c_kp2++;
        while (*D1t < *D2t) {   /* current D1 key point is before new current D2 key point */
           res++;  /* count this D1 point */
           if (c_kp1<D1.InstantNumber) { /* go to next D1 point */
    	      D1t++;
    	      c_kp1++;
    	   }
           else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	      res += (D2.InstantNumber - c_kp2 +1);
    	      return (res);
           }
        }
    } /* now current D1 key point is not before D2 key point */
  }

  /* last D2 key point */
  if (*D1t == *D2t) {
    res++;	/* count this common D1/D2 point */
    if (c_kp1<D1.InstantNumber) { /* go to next D1 point */
    	D1t++;
    	c_kp1++;
    }
    else { /* it is also the last D1 point */
    	return (res);
    }
  }
  else {   /* current D1 key point is after current D2 key point */
     res++;	/* count this D2 point */
  }
  /* now add all remaining D1 key points */
  res += (D1.InstantNumber - c_kp1 +1);

  return (res);
}

//----------------------------------------------------------------------------
void MotionTrajectorySearchTool::InitializeAlignedTimes (GenericTrajD InD1, GenericTrajD InD2, GenericTrajD OutD1, GenericTrajD OutD2)
/* initialize new time fields */
{
  int *InD1t, *InD2t;       /* pointers on time positions of keypoints */
  unsigned long c_kp1, c_kp2;     /* current number of key points for D1 and D2 */
  int *OutD1t, *OutD2t;
  unsigned long c_kp;

  /* checks */
  if ( (InD1.InstantNumber == 0) || (InD2.InstantNumber == 0)
  	|| (OutD1.InstantNumber == 0) || (OutD2.InstantNumber == 0) )    /* check at least 1 k.p. */
    exit(0);

  /* initializations */
  c_kp1 = c_kp2 = 1;    /* start with the first key point of each D */
  InD1t = InD1.Times;       /* initialize pointesr accordingly */
  InD2t = InD2.Times;
  OutD1t = OutD1.Times;
  OutD2t = OutD2.Times;
  c_kp = 0;     	/* keypoint number on aligned Ds (not useful, only for checking) */

  while (*InD1t < *InD2t) {  /* count key points of D1 before D2 starts - if exists */
    *OutD1t = *OutD2t = *InD1t;   /* put D1 key point time */
    c_kp++; OutD1t++; OutD2t++;
    if (c_kp1<InD1.InstantNumber) {  /* go to next D1 point */
    	InD1t++;
    	c_kp1++;
    }
    else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	while (c_kp2 < InD2.InstantNumber) {
            *OutD1t = *OutD2t = *InD2t;   /* put D2 key point time */
            c_kp++; OutD1t++; OutD2t++;
    	    c_kp2++; InD2t++;
    	}
    	*OutD1t = *OutD2t = *InD2t;
    	return;
    }
  } /* current D1 key point is now not before first D2 key point */

  while (c_kp2 < InD2.InstantNumber) {
    if (*InD1t == *InD2t) {
        *OutD1t = *OutD2t = *InD2t;   /* put this common D1/D2 key point time */
        c_kp++; OutD1t++; OutD2t++;
        InD2t++;  /* go to next D2 point */
    	c_kp2++;
    	if (c_kp1<InD1.InstantNumber) { /* go to next D1 point */
    	    InD1t++;
    	    c_kp1++;
        }
        else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	    while (c_kp2 < InD2.InstantNumber) {
                *OutD1t = *OutD2t = *InD2t;   /* put D2 key point time */
                c_kp++; OutD1t++; OutD2t++;
    	        c_kp2++; InD2t++;
    	    }
    	    *OutD1t = *OutD2t = *InD2t;
    	    return;
        }
        while (*InD1t < *InD2t) {   /* current D1 key point is before new current D2 key point */
           *OutD1t = *OutD2t = *InD1t;   /* put D1 key point time */
           c_kp++; OutD1t++; OutD2t++;
           if (c_kp1<InD1.InstantNumber) { /* go to next D1 point */
    	      InD1t++;
    	      c_kp1++;
    	   }
           else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	      while (c_kp2 < InD2.InstantNumber) {
                  *OutD1t = *OutD2t = *InD2t;   /* put D2 key point time */
                  c_kp++; OutD1t++; OutD2t++;
    	          c_kp2++; InD2t++;
    	      }
    	      *OutD1t = *OutD2t = *InD2t;
    	      return;
           }
        }
    }
    else {   /* current D1 key point is after current D2 key point */
        *OutD1t = *OutD2t = *InD2t;   /* put D2 key point time */
        c_kp++; OutD1t++; OutD2t++;
   	InD2t++;  /* go to next D2 key point */
    	c_kp2++;
        while (*InD1t < *InD2t) {   /* current D1 key point is before new current D2 key point */
           *OutD1t = *OutD2t = *InD1t;   /* put D1 key point time */
           c_kp++; OutD1t++; OutD2t++;
           if (c_kp1<InD1.InstantNumber) { /* go to next D1 point */
    	      InD1t++;
    	      c_kp1++;
    	   }
           else { /* if there is no more D1 key point add the remaining D2 ones and end */
    	      while (c_kp2 < InD2.InstantNumber) {
                  *OutD1t = *OutD2t = *InD2t;   /* put D2 key point time */
                  c_kp++; OutD1t++; OutD2t++;
    	          c_kp2++; InD2t++;
    	      }
    	      *OutD1t = *OutD2t = *InD2t;
    	      return;
           }
        }
    } /* now current D1 key point is not before D2 key point */
  }

  /* last D2 key point */
  if (*InD1t == *InD2t) {
    *OutD1t = *OutD2t = *InD2t;   /* put this common D1/D2 key point time */
    c_kp++; OutD1t++; OutD2t++;
    if (c_kp1<InD1.InstantNumber) { /* go to next D1 point */
    	InD1t++;
    	c_kp1++;
    }
    else { /* it is also the last D1 point */
    	return;
    }
  }
  else {   /* current D1 key point is after current D2 key point */
     *OutD1t = *OutD2t = *InD2t;   /* put this D2 key point time */
     c_kp++; OutD1t++; OutD2t++;    /* c_kp2 and InD2 not updated as last ones */
  }
  /* now add all remaining D1 key points */
  while (c_kp1 < InD1.InstantNumber) {
     *OutD1t = *OutD2t = *InD1t;   /* put D1 key point time */
     c_kp++; OutD1t++; OutD2t++;
     c_kp1++; InD1t++;
     }
  *OutD1t = *OutD2t = *InD1t;
  while (c_kp1 < OutD1.InstantNumber) {					//SJ-L
     *OutD1t = *OutD2t = 0;   /* fill the rest with 0s */		//SJ-L
     c_kp++; OutD1t++; OutD2t++;					//SJ-L
     c_kp1++; InD1t++;							//SJ-L
     }									//SJ-L


  return;
}

//----------------------------------------------------------------------------
double MotionTrajectorySearchTool::match_D1D2_generic (GenericTrajD D1, GenericTrajD D2, float w_x, float w_y, float w_z, float w_vx, float w_vy, float w_vz, float w_ax, float w_ay, float w_az)
{
 double distance = 0.0;
 double PositionDistance = 0.0,
   SpeedDistance = 0.0,
   AccelerationDistance = 0.0;
 unsigned long c_kp;
 VctDouble *d1_a, *d1_s, *d1_p, *d2_a, *d2_s, *d2_p;
 int *d1_t, *d2_t;
 int *d1_v, *d2_v;
 int duration, matching_duration, query_duration;

 /* initializations */
 d1_a = D1.Accelerations;
 d2_a = D2.Accelerations;
 d1_s = D1.Speeds;
 d2_s = D2.Speeds;
 d1_p = D1.Positions;
 d2_p = D2.Positions;
 d1_t = D1.Times;
 d2_t = D2.Times;
 d1_v = D1.Visible;
 d2_v = D2.Visible;
 matching_duration = 0;
 query_duration = 0;

 /* calculate distances */

 for (c_kp=1; c_kp<D2.InstantNumber; c_kp++) {
   duration = *(d2_t+1) - *d2_t;

   if (*d2_v && (d2_p->x != -1.0) && (d2_p->y != -1.0) && (d2_p->z != -1.0) ) {
      query_duration += duration;
      if (*d1_v && (d1_p->x != -1.0) && (d1_p->y != -1.0) && (d1_p->z != -1.0) ) {
         matching_duration += duration;
         /* positions */
         PositionDistance += w_x * (d1_p->x - d2_p->x) * (d1_p->x - d2_p->x) * duration;
         PositionDistance += w_y * (d1_p->y - d2_p->y) * (d1_p->y - d2_p->y) * duration;
         PositionDistance += w_z * (d1_p->z - d2_p->z) * (d1_p->z - d2_p->z) * duration;
         /* speeds */
         SpeedDistance += w_vx * (d1_s->x - d2_s->x) * (d1_s->x - d2_s->x) * duration;
         SpeedDistance += w_vy * (d1_s->y - d2_s->y) * (d1_s->y - d2_s->y) * duration;
         SpeedDistance += w_vz * (d1_s->z - d2_s->z) * (d1_s->z - d2_s->z) * duration;
         /* accelerations */
         AccelerationDistance += w_ax * (d1_a->x - d2_a->x) * (d1_a->x - d2_a->x) * duration;
         AccelerationDistance += w_ay * (d1_a->y - d2_a->y) * (d1_a->y - d2_a->y) * duration;
         AccelerationDistance += w_az * (d1_a->z - d2_a->z) * (d1_a->z - d2_a->z) * duration;
      }
   }

   d1_a++;   d2_a++;
   d1_s++;   d2_s++;
   d1_p++;   d2_p++;
   d1_t++;   d2_t++;
   d1_v++;   d2_v++;
 }

 if (matching_duration)
   distance = ( PositionDistance + SpeedDistance + AccelerationDistance )
 		* ( (double) query_duration / matching_duration );
 else
   distance = MAX_DIST;

 return (distance);
}

