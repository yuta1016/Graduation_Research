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
// Applicable File Name:  MotionTrajectoryExtraction.cpp
//

#include <stdio.h>
#include <momusys.h>
#include <inter.h>
#include <intra.h>
#include <fifo.h>
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "ExtractionUtilities/ExtractionUtilities.h"

//=============================================================================


using namespace XM;


const UUID MotionTrajectoryExtractionInterface::myID = UUID();
const char *MotionTrajectoryExtractionInterface::myName = "MotionTrajectoryExtractionInterface";

const UUID MotionTrajectoryExtractionTool::myID = UUID();
const char *MotionTrajectoryExtractionTool::myName = "MotionTrajectoryDescriptorExtractor";

const UUID MotionTrajectoryExtractionInterfaceABC::myID = UUID();

//=============================================================================
MotionTrajectoryExtractionInterfaceABC::MotionTrajectoryExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MotionTrajectoryExtractionInterface::MotionTrajectoryExtractionInterface(MotionTrajectoryExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MotionTrajectoryExtractionInterface::~MotionTrajectoryExtractionInterface()
{
}

//----------------------------------------------------------------------------
//int MotionTrajectoryExtractionInterface::SetSourceMedia(T4DKeyPoint *media)
int MotionTrajectoryExtractionInterface::SetSourceMedia(MultiMedia *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionInterface::SetSourceMediaInterface(MultiMediaInterfaceABC *mediaInterface)
{
  return m_ExtractionTool->SetSourceMediaInterface(mediaInterface);
}

//----------------------------------------------------------------------------
// I don't understand why this function should be used
int MotionTrajectoryExtractionInterface::
SetSourceMediaKeyPointNumber(unsigned long CurrentPoint)
{
  return m_ExtractionTool->
    SetSourceMediaKeyPointNumber(CurrentPoint);
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC* MotionTrajectoryExtractionInterface::
GetDescriptorInterface(void)

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionInterface::
SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aMotionTrajectoryDescriptorInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aMotionTrajectoryDescriptorInterface);
}

//sth should not be required, but generated in constructor
//----------------------------------------------------------------------------
TemporalInterpolationExtractionInterfaceABC
*MotionTrajectoryExtractionInterface::
GetTemporalInterpolationExtractionInterface(void)
{
  return m_ExtractionTool->GetTemporalInterpolationExtractionInterface();
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionInterface::
SetTemporalInterpolationExtractionInterface(
  TemporalInterpolationExtractionInterfaceABC
  *aTemporalInterpolationExtractionInterface)
{
  return m_ExtractionTool->
    SetTemporalInterpolationExtractionInterface(aTemporalInterpolationExtractionInterface);
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionInterface::
StartExtracting(int KeyPointsNumber)
{
  return m_ExtractionTool->StartExtracting(KeyPointsNumber);
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionInterface::InterpolateKeypoints(int KeyPointsNumber)
{
  return m_ExtractionTool->InterpolateKeypoints(KeyPointsNumber);
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
void MotionTrajectoryExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }


//=============================================================================
MotionTrajectoryExtractionTool::MotionTrajectoryExtractionTool():
m_Interface(this),
m_DescriptorInterface(0),
m_TemporalInterpolationExtractionInterface(0),
m_Media(0),
m_KeyPoint(0)

{
        TemporalInterpolationExtractionTool *m_TemporalInterpolationExtraction;

#ifdef VERBOSE
        fprintf(stderr,"Connect sub descriptors (extraction) \n");
#endif

	/* create sub extraction schemes */
	m_TemporalInterpolationExtraction= new TemporalInterpolationExtractionTool();

	/* connect sub extraction schemes with motion trajectory extraction scheme*/
	if (m_TemporalInterpolationExtraction)
	  SetTemporalInterpolationExtractionInterface(m_TemporalInterpolationExtraction->GetInterface());

	SetExposedInterface(&m_Interface);
	m_counter = 0;
}

//=============================================================================
MotionTrajectoryExtractionTool::
MotionTrajectoryExtractionTool(MotionTrajectoryDescriptorInterfaceABC
			     *motiontrajectory):
m_Interface(this),
m_DescriptorInterface(0),
m_TemporalInterpolationExtractionInterface(0),
m_Media(0),
m_KeyPoint(0)

{

   TemporalInterpolationExtractionTool *m_TemporalInterpolationExtraction;

#ifdef VERBOSE
        fprintf(stderr,"Connect descriptor\n");
#endif

   /* create descriptor if it does not exist*/
   if (!motiontrajectory) {
//COORD	  MotionTrajectoryDescriptor *descriptor =
//COORD	    new MotionTrajectoryDescriptor(0,0);
      MotionTrajectoryDescriptor *descriptor = new MotionTrajectoryDescriptor(0);
      motiontrajectory=descriptor->GetInterface();
   }

   /* connect Descriptor with extraction scheme */
   SetDescriptorInterface(motiontrajectory);

   /* create sub extraction tools and connect them with descriptor if availabe*/
   if (motiontrajectory->GetTemporalInterpolationDescriptorInterface()) {
	m_TemporalInterpolationExtraction =
	  new TemporalInterpolationExtractionTool(motiontrajectory->GetTemporalInterpolationDescriptorInterface());
   }
   else {
	m_TemporalInterpolationExtraction= new TemporalInterpolationExtractionTool();
   }

   if (m_TemporalInterpolationExtraction)
	SetTemporalInterpolationExtractionInterface(m_TemporalInterpolationExtraction->GetInterface());

   SetExposedInterface(&m_Interface);
   m_counter = 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryExtractionTool::~MotionTrajectoryExtractionTool()
{
        /* destroy sub extraction schemes*/
	if (m_TemporalInterpolationExtractionInterface)
	  m_TemporalInterpolationExtractionInterface->destroy();

	/* release descriptor*/
	if (m_DescriptorInterface)
	  m_DescriptorInterface->release();
}

//----------------------------------------------------------------------------
bool MotionTrajectoryExtractionTool::SupportsPush(void)
{ return true; }      	// ?
//----------------------------------------------------------------------------
bool MotionTrajectoryExtractionTool::SupportsPull(void)
{ return false; }	// ?

//----------------------------------------------------------------------------
//int MotionTrajectoryExtractionTool::SetSourceMedia(T4DKeyPoint *media)
int MotionTrajectoryExtractionTool::SetSourceMedia(MultiMedia *media)
{
  if (!media) return -1;

//  m_Media=media;          NOT VALID IF n_Media is an interface
  return 0;
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionTool::SetSourceMediaInterface(MultiMediaInterfaceABC *mediaInterface)
{
  if (!mediaInterface) return -1;

  m_Media=mediaInterface;
  return 0;
}

//----------------------------------------------------------------------------
/*  ?not necessary... : I do not extract 1 key point by 1 key point */
/* rather I launch the extraction on the whole stuff => should be the whole file also given */
int MotionTrajectoryExtractionTool::SetSourceMediaKeyPointNumber(unsigned long
								 CurrentPoint)
{
  m_KeyPoint=CurrentPoint;
  return 0;
}

//----------------------------------------------------------------------------
MotionTrajectoryDescriptorInterfaceABC* MotionTrajectoryExtractionTool::
GetDescriptorInterface(void)
{
        return m_DescriptorInterface;
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionTool::
SetDescriptorInterface(MotionTrajectoryDescriptorInterfaceABC
		       *aMotionTrajectoryDescriptorInterface)

{
  if (!aMotionTrajectoryDescriptorInterface)
     return -1;

  /* check if new value is different from old value */
  if (m_DescriptorInterface == aMotionTrajectoryDescriptorInterface)
     return 0;

  /* release old interface if exists */
  if (m_DescriptorInterface)
     m_DescriptorInterface->release();

  /* add new interface*/
  /*********************/

  m_DescriptorInterface = aMotionTrajectoryDescriptorInterface;

  if (m_DescriptorInterface) {
     m_DescriptorInterface->addref();
	  /* connect sub descriptors with sub extraction tools here if needed */
     if ((m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface())
       && m_TemporalInterpolationExtractionInterface) {
            m_TemporalInterpolationExtractionInterface->SetDescriptorInterface(
                m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface());
     }
  }
  else {
	  /* if no descriptor, release also descriptor interfaces from sub extraction tools schemes*/
     m_TemporalInterpolationExtractionInterface->SetDescriptorInterface(0);
     return -1;
  }
  return 0;
}

//----------------------------------------------------------------------------
TemporalInterpolationExtractionInterfaceABC *
   MotionTrajectoryExtractionTool::GetTemporalInterpolationExtractionInterface(void)

{
        return m_TemporalInterpolationExtractionInterface;
}

//----------------------------------------------------------------------------
int MotionTrajectoryExtractionTool::SetTemporalInterpolationExtractionInterface
           (TemporalInterpolationExtractionInterfaceABC *aTemporalInterpolationExtractionInterface)

{
        if (!aTemporalInterpolationExtractionInterface) return -1;

        m_TemporalInterpolationExtractionInterface = aTemporalInterpolationExtractionInterface;
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionTool::InitExtracting(void)

{
	return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionTool::StartExtracting(int KeyPointsNumber)

{
  //   unsigned long 	c_kp;
   unsigned long uL_check;
   double whole_duration;

   /* check for descriptor*/
   if (!m_DescriptorInterface) return (unsigned long)-1;
   if (strcmp(m_DescriptorInterface->GetName(),
		   "MotionTrajectoryDescriptorInterface") != 0) {
			   std::cerr << m_DescriptorInterface->GetName() << "\n";
	return (unsigned long)-1;
   }

   /* check for media */
   if (!m_Media) return (unsigned long)-1;

   T4DKeyPoint *PointerOnKeyPoints;
   //TnDKeyPoint *PointerOnKeyPoints;
   PointerOnKeyPoints = m_Media->GetKeyPointPointer();
   //PointerOnKeyPoints = m_Media->GetKeyPoint();

   //cerr << "Print tests in extraction\n";
   //for (int bibi=0; bibi<KeyPointsNumber; bibi++)
   //cerr << PointerOnKeyPoints[bibi].time << "  " << PointerOnKeyPoints[bibi].x_position << "  " << PointerOnKeyPoints[bibi].y_position << "  " << PointerOnKeyPoints[bibi].z_position << "\n";

   /* get the TemporalInterpolation interface */
   TemporalInterpolationDescriptorInterfaceABC *interpInterface;
   interpInterface =  m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface();
   if (!interpInterface) return (unsigned long)-1;
   if ((strcmp(interpInterface->GetName(),
	       "TemporalInterpolationDescriptorInterface") != 0) &&
       (strcmp(interpInterface->GetName(),
	      "Temporal Interpolation Descriptor Interface") !=0)) {
			  std::cerr << interpInterface->GetName() << "\n";
     return (unsigned long)-1;
   }

   /* INITIALIZE WHAT CAN BE DIRECTLY DONE */
   /****************************************/

   /* set keypoint number */
   uL_check = interpInterface->SetNumOfKeyPoint(KeyPointsNumber);

   /* put positions to the correct Units */
   //for the moment it is assume that positions are given in the Units the user whishes to store them
   // thus nothing is done
   
   /* extract keypoint data */
   int i;
   T4DKeyPoint *c_pt; 
   //TnDKeyPoint *c_pt;
   for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {
     uL_check = interpInterface->SetMediaTimePoint(i,(double)(c_pt->time));
     uL_check = interpInterface->SetKeyValue(0,i,(double)(c_pt->x_position));
     uL_check = interpInterface->SetKeyValue(1,i,(double)(c_pt->y_position));
     uL_check = interpInterface->SetKeyValue(2,i,(double)(c_pt->z_position));
     //uL_check = interpInterface->SetKeyValue(0,i,(double)(c_pt->p[0]));
     //uL_check = interpInterface->SetKeyValue(1,i,(double)(c_pt->p[1]));
     //uL_check = interpInterface->SetKeyValue(2,i,(double)(c_pt->p[2]));
     c_pt++;
   }

   /* set whole interval duration */
   whole_duration = interpInterface->GetMediaTimePoint(KeyPointsNumber-1) - interpInterface->GetMediaTimePoint(0);
   uL_check = interpInterface->SetMediaDuration(whole_duration);

   /* extract dimension: 2D-3D Coords */
   interpInterface->SetDimension(2); 		// initialize as 2-dimensional
   for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {
     if ( c_pt->z_position != (-1.0) )  {
     //if ( c_pt->p[2] != (-1.0) )  {
       interpInterface->SetDimension(3);	// change if at least one z
       break;
     }
     c_pt++;
   }

   /* camera follows: for the moment assume not specified -> to put SUN latest routine */
   m_DescriptorInterface->SetCameraFollows(0);

   /* COMPLETE TO OBTAIN A COMPLETE DESCRIPTOR: ROUGH FROM DATA, WITHOUT KEYPOINTS SELECTION */

   /* Contant Time Interval Initialization */
   int ref_time_interval, new_time_interval;
   int constant_time_interval = 1;
   // check if time intervals are constant 
   ref_time_interval = PointerOnKeyPoints[1].time - PointerOnKeyPoints[0].time;
   for ( i=1, c_pt=(PointerOnKeyPoints+1); i < (KeyPointsNumber-1); i++ ) {
     new_time_interval = ((c_pt+1)->time) - (c_pt->time);
     if (new_time_interval != ref_time_interval) {  // time intervals are not constant
        constant_time_interval = 0;
        break;
     }
   }
   /* initialize the field */
   interpInterface->SetConstantTimeInterval(constant_time_interval);
   // change back when bug in Temporal Interpolation coding corrected
   /*interpInterface->SetConstantTimeInterval(0);				// patch for bug in TemporalInterpolation coding
   interpInterface->SetMediaTimePointDataType(0);*/			//SJ-L

   /* Default Interp Initialization: at this point YES as no further processing is done */
//  for ( i=0; i<interpInterface->GetDimension(); i++ )
//		interpInterface->SetDefaultFunction(i,1);
	interpInterface->SetDefaultFunction(0,1);
	interpInterface->SetDefaultFunction(1,1);
   /*for ( i=0; i<interpInterface->GetDimension(); i++ ) {
		//interpInterface->SetType(i,0,0);
		for ( j=0; j < KeyPointsNumber-1; j++ ) {	//SJ-L
			interpInterface->SetType(i,j,1);
		}
   }*/


   /*========================*/
   /* SET FunctionIDs values */
   /*========================*/

   /* X-DIMENSION */
   /*=============*/

   /*if (interpInterface->GetDefaultFunction(0) == 1) {	// use always default interp 
     // set first all Types to correspond to default interp 
     interpInterface->SetType(0,0,0); // type = start point	//SJ-L
     for ( i=1; i < KeyPointsNumber; i++ ) {
			//interpInterface->SetType(0,i,0);	//SJ-L
			interpInterface->SetType(0,i,1);	//SJ-L
		}
		// then handle the case of points where value could not be calculated:
		//set Type to "-1 or 3" on the 2 intervals surrounding those points 
//		for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
		for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
			if (c_pt->x_position == (-1.0) ) {
//				if (c_pt->p[0] == (-1.0) ) {
				if (i != 0) {  			 // not first keypoint - no interval before
					interpInterface->SetType(0,i-1,3);
				}
				if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
					interpInterface->SetType(0,i,3);
				}
			}
			c_pt++;
		}
   }
   else { // choose interpolation depending on traj 
     // right now do the same -> will have to be updated 
     // set first all Types to correspond to default interp 
		interpInterface->SetType(0,0,0); // type = start point	//SJ-L
	    for ( i=1; i < KeyPointsNumber; i++ ) {
        //interpInterface->SetType(0,i,0);	//SJ-L
        interpInterface->SetType(0,i,1);	//SJ-L
     }
     // then handle the case of points where value could not be calculated:
     // set Type to "-1 or 3" on the 2 intervals surrounding those points 
//     for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
       for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
        if (c_pt->x_position == (-1.0) ) {
//        if (c_pt->p[0] == (-1.0) ) {
           if (i != 0) {  			 // not first keypoint - no interval before
	      interpInterface->SetType(0,i-1,3);
           }
           if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
	      interpInterface->SetType(0,i,3);
           }
        }
        c_pt++;
     }
   }*/

   /* Y-DIMENSION */
   /*=============*/

   /*if (interpInterface->GetDefaultFunction(1) == 1) {
     // set first all types to correspond to default interp 
     interpInterface->SetType(1,0,0); // type = start point			// SJ-L
     //for ( i=0; i < KeyPointsNumber; i++ ) {	// SJ-L
     for ( i=1; i < KeyPointsNumber; i++ ) {	// SJ-L
     //   interpInterface->SetType(1,i,0);		// SJ-L
        interpInterface->SetType(1,i,1);		// SJ-L
     }
     // then handle the case of points where value could not be calculated:
     // set Type to "-1 or 3" on the 2 intervals surrounding those points 
//     for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	// SJ-L
     for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	// SJ-L
        if (c_pt->y_position == (-1.0) ) {
//        if (c_pt->p[1] == (-1.0) ) {
           if (i != 0) {  			 // not first keypoint - no interval before
	      interpInterface->SetType(1,i-1,3);
           }
           if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
	      interpInterface->SetType(1,i,3);
           }
        }
        c_pt++;
     }
   }
   else { // choose interpolation depending on traj 
     // right now do the same -> will have to be updated 
     interpInterface->SetType(1,0,0); // type = start point			// SJ-L
     //for ( i=0; i < KeyPointsNumber; i++ ) {	// SJ-L
     for ( i=1; i < KeyPointsNumber; i++ ) {	// SJ-L
     //   interpInterface->SetType(1,i,0);		// SJ-L
        interpInterface->SetType(1,i,1);		// SJ-L
     }
     // then handle the case of points where value could not be calculated:
     // set Type to "-1 or 3" on the 2 intervals surrounding those points 
//     for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	// SJ-L
     for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	// SJ-L
        if (c_pt->y_position == (-1.0) ) {
//        if (c_pt->p[1] == (-1.0) ) {
           if (i != 0) {  			 // not first keypoint - no interval before
	      interpInterface->SetType(1,i-1,3);
           }
           if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
	      interpInterface->SetType(1,i,3);
           }
        }
        c_pt++;
     }
   }*/

   /* Z-DIMENSION */
   /*=============*/

/* if (interpInterface->GetDimension() == 3) {
     if (interpInterface->GetDefaultFunction(2) == 1) {
		// set first all Types to correspond to default interp 
		interpInterface->SetType(2,0,0); // type = start point	// SJ-L
		//for ( i=0; i < KeyPointsNumber; i++ ) {	//SJ-L
		for ( i=1; i < KeyPointsNumber; i++ ) {	//SJ-L
			//interpInterface->SetType(2,i,0);		//SJ-L
			interpInterface->SetType(2,i,1);		//SJ-L
		}
		// then handle the case of points where value could not be calculated:
		// set Type to "-1 or 3" on the 2 intervals surrounding those points 
		//for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
		for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {	//SJ-L
			if (c_pt->z_position == (-1.0) ) {
				//if (c_pt->p[2] == (-1.0) ) {
				if (i != 0) {  			 // not first keypoint - no interval before
					interpInterface->SetType(2,i-1,3);
				}
				if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
					interpInterface->SetType(2,i,3);
				}
			 }
			 c_pt++;
		}
	  }
	  else { // choose interpolation depending on traj 
		// right now do the same -> will have to be updated 
		interpInterface->SetType(2,0,0); // type = start point	// SJ-L
		//for ( i=0; i < KeyPointsNumber; i++ ) {	//SJ-L
		for ( i=1; i < KeyPointsNumber; i++ ) {	//SJ-L
			//interpInterface->SetType(2,i,0);		//SJ-L
			interpInterface->SetType(2,i,1);		//SJ-L
		}
		// then handle the case of points where value could not be calculated:
		// set Type to "-1 or 3" on the 2 intervals surrounding those points 
		// for ( i=0, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {		//SJ-L
		for ( i=1, c_pt=PointerOnKeyPoints; i < KeyPointsNumber; i++ ) {		//SJ-L
			if (c_pt->z_position == (-1.0) ) {
				//if (c_pt->p[2] == (-1.0) ) {
				if (i != 0) {  			 // not first keypoint - no interval before
					interpInterface->SetType(2,i-1,3);
				}
				if (i != (KeyPointsNumber-1)) { 	 // not last keypoint - no interval after
					interpInterface->SetType(2,i,3);
				}
			}
			c_pt++;
		}
	  }
   }*/

//    cout << "check End of MotionTrajectoryExtraction.cpp\n";
//    for ( i=0; i < KeyPointsNumber; i++ ) {
// 	     cout << interpInterface->GetMediaTimePoint(i) << "  "
// 	          << interpInterface->GetKeyValue(0,i) << "  "
// 	          << interpInterface->GetKeyValue(1,i) << "  "
// 	          << interpInterface->GetKeyValue(2,i) << "\n";
//    }

   return 0;
}

//----------------------------------------------------------------------------
unsigned long MotionTrajectoryExtractionTool::InterpolateKeypoints(int KeyPointsNumber)

{

   TemporalInterpolationDescriptorInterfaceABC *interpInterface;
   interpInterface =  m_DescriptorInterface->GetTemporalInterpolationDescriptorInterface();
   if (!interpInterface) return (unsigned long)-1;
   if ((strcmp(interpInterface->GetName(),
	       "TemporalInterpolationDescriptorInterface") != 0) 
       && (strcmp(interpInterface->GetName(),
		 "Temporal Interpolation Descriptor Interface") != 0)) {
			 std::cerr << interpInterface->GetName() << "\n";
     return (unsigned long)-1;
   }

  int dim;
  MultiMediaInterfaceABC *MediaI;

  MultiMedia *Media = new MultiMedia;
  MediaI=Media->GetInterface();
  TnDKeyPoint *keypoint;
  TnDKeyPoint c_kpt;
  keypoint = new TnDKeyPoint [KeyPointsNumber];
  for (m_counter=0; m_counter<KeyPointsNumber; m_counter++) {
    m_TemporalInterpolationExtractionInterface->SetDescriptorInterface(interpInterface);
	c_kpt = keypoint[m_counter];
    c_kpt.p = new float [3];
    c_kpt.dim = interpInterface->GetDimension();
    c_kpt.time = (int)(interpInterface->GetMediaTimePoint(m_counter));
    for (dim=0; dim<interpInterface->GetDimension(); dim++) {
      c_kpt.p[dim]  =  interpInterface->GetKeyValue(dim,m_counter);
 	}
    MediaI->AddKeyPointsList(m_counter);
    MediaI->AddKeyPoint(&c_kpt);
    m_TemporalInterpolationExtractionInterface->SetSourceMedia(MediaI);
    m_TemporalInterpolationExtractionInterface->StartExtracting();

    delete [] c_kpt.p;
   }
    MediaI->destroy();
	delete [] keypoint;
  //    m_TemporalInterpolationExtractionInterface->SetDescriptorInterface(interpInterface);
//    m_TemporalInterpolationExtractionInterface->ExTool()->GetInputPoints().Npoint = m_counter+1;
//    m_TemporalInterpolationExtractionInterface->ExTool()->GetInputPoints().Dimension = interpInterface->GetDimension();
//    m_TemporalInterpolationExtractionInterface->ExTool()->GetInputPoints().Point[m_counter].PointT = interpInterface->GetMediaTimePoint(m_counter);
//    for (dim=0; dim<interpInterface->GetDimension(); dim++) {
//      m_TemporalInterpolationExtractionInterface->ExTool()->GetInputPoints().Point[m_counter].Point[dim] = interpInterface->GetKeyValue(dim,m_counter);
//    }
  //m_TemporalInterpolationExtractionInterface->StartExtracting();
  //}

  //for (dim=0; dim<interpInterface->GetDimension(); dim++) {
    m_TemporalInterpolationExtractionInterface->SetDescriptorInterface(interpInterface);
    //cerr << "Start Post Extraction\n";
    m_TemporalInterpolationExtractionInterface->PostExtraction();
    //cerr << "Post Extraction done\n";
 // }

  // write TI D content for checking
  //cerr << "Final KeyPointNumber: " << interpInterface->GetNumOfKeyPoint() << "\n";
  //int i;
  //for ( i=0; i < interpInterface->GetNumOfKeyPoint(); i++ ) {
  //  cerr << interpInterface->GetMediaTimePoint(i) << "  " << interpInterface->GetKeyValue(0,i) << "  "
  //       << interpInterface->GetKeyValue(1,i) << "  " << interpInterface->GetKeyValue(2,i) << "\n";
  //}

  //interpInterface->release(); BUG ! - fix -
  return 0;
}

//----------------------------------------------------------------------------
const UUID& MotionTrajectoryExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MotionTrajectoryExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
MotionTrajectoryExtractionInterfaceABC *MotionTrajectoryExtractionTool::GetInterface(void)
{ return &m_Interface; }

