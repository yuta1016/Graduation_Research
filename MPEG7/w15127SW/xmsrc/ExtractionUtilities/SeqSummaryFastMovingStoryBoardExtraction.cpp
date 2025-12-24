///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// 
// Modifications to this base code have been done to enable TimeStamp and
// FastAudio generation by
// Dulce Ponceleon, Jan H. Pieper (Almaden Research Center, IBM, USA) and 
// Gilad Cohen (IBM Research, Haifa ISRAEL)
// 
// dulce@almaden.ibm.com
// jhpieper@almaden.ibm.com
// giladc@il.ibm.com
// 
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
// Applicable File Name:  SeqSummaryFastMovingStoryBoardExtraction.cpp
//

//=============================================================================
/**SeqSummaryFastMovingStoryBoardExtractionTool
 *
 * The SeqSummaryFastMovingStoryBoardExtractionTool extracts keyframes from
 * a sequence of frames and creates a Description Scheme (DS) as XML document 
 * object model (DOM).
 *
 * Furthermore a fast audio file is generated from an audio file belonging to
 * the sequence of frames. FASTMSB_AUDIO_FACTOR defines the speedup factor as 0.66
 * Feel free to change this value to get different speeds and hear what you get!!
 *
 * A FastMovingStoryBoard will be created as a .SMIL file synchronizing the 
 * keyframes with the fast audio file. This file is playable with
 * e.g. RealPlayer and QuickTime (RealPlayer is a trademark of RealNetworks and
 * QuickTime is a trademark of Apple Computer, Inc).
 *
 * SeqSummaryFastMovingStoryBoardExtractionTool operates in three steps:
 *
 * -Initialisation: * create root node and header nodes of DS DOM
 *                  * create .smil file and write header data
 *
 * -{Extraction}:   (called for every frame)
 *                  * determine whether frame is a keyframe or not
 *                  * if frame is keyframe:
 *                    * save frame as .jpg
 *                    * add FrameProperty node to the DS DOM
 *                    * write reference to the keyframe.jpg into the .smil file
 *
 * -PostExtraction: * add MediaDuration to DS DOM
 *                  * write footer and close .smil file
 *                  * generate fast audio file
 *
 *
 * The Description Scheme DOM is organized as a tree of nodes. Every node 
 * can have a value, none, one or more attributes and none, one or more children
 * nodes. 
 *
 * The structure of the SeqSummaryFastMovingStoryBoardExtractionDS is as follows:
 *
 *   SequentialSummary
 *       RefLocator
 *           MediaURL
 *       AudioSegmentLocator
 *           MediaURL
 *           MediaDuration
 *       {FrameProperty}
 *           ImageLocator
 *               MediaURL
 *           RefTime
 *               MediaRelTimePoint
 *           SyncTime
 *               MediaRelTimePoint
 *               MediaDuration
 *
 * SequentialSummary has three types of children nodes:
 *  RefLocator and AudioSegmentLocator contain header information and appear only once
 *  FrameProperty contains data about one keyframe and may be used several times
 *
 * The reference below presents differen video summaries and a user study on 
 * FastMovingStoryBoards. The study shows that using a speed up version of the video
 * does not affect comprehension of the content:
 * A. Amir, D. Ponceleon, B. Blanchard, D. Petkovic, S. Srinivasan, and G. Cohen,
 * "Using Audio Time Scale Modification for Video Browsing", Proc. of Hawaii Int. Conf. 
 * on System Sciences, HICSS-33, Hawaii, Jan. 2000. 
 *
 * The generation of the fast audio file is done by the WSOLA algorithm.
 * Beware!! No attempts were made to optimize this code in any way in terms 
 * of memory consumption and CPU time.
 * For more information see comments within this file.
 *
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <limits.h>
#include <float.h>


#ifndef _VISUALC_
	#include <unistd.h>
	#include <netinet/in.h>
#else
	#include <io.h>
#endif

#include "momusys.h"
#include "inter.h"
#include "intra.h"
#include "Utilities/readparam.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Utilities/DoubleArray.h"
#include "Utilities/DoubleArrayMatrix.h"
#include "Utilities/LinkedList.h"
#include "ExtractionUtilities/KLTFeaturePoints.h"
#include "Media/Media.h"

#define SW_DENOISE 1
#define MAXINDEX 299
#define MINDIST 19
#define SETMARK 127

#define FEATURENBR 7
#define DISTTHRESH 0.25
#define FRAMEDURATION (1.0/30.0)  
#define ACCELNOISE 1500.0
#define ALPHA 0.01
#define MEASURENOISE 10.0
#define STDDEVIATION 0.4
#define INTERMEDIARYIO
#define MAXFPFILENAMELEN 256
#define FASTMSB_AUDIO_FACTOR 0.66	//speedup factor for audio file

#define USESMIL //works only if USEXML has been defined too.

using namespace std;

extern "C" void SSZfreeListIntegerObject(void *ListObject,void *unused);
extern "C" void SSZdeleteDoubleArrayElement(void *object, void *par);

//=============================================================================
void SSZdeleteDoubleArrayElement(void *object, void *par)
{
  doubleArrayDestroy((DoubleArray *)object);
}

//----------------------------------------------------------------------------
void SSZfreeListIntegerObject(void *ListObject,void * unused)
{
  if (ListObject) free(ListObject);
}

//=============================================================================

using namespace XM;

const UUID SeqSummaryFastMovingStoryBoardExtractionInterface::myID = UUID("");
const char *SeqSummaryFastMovingStoryBoardExtractionInterface::myName = "SeqSummaryFastMovingStoryBoardExtractionInterface";

const UUID SeqSummaryFastMovingStoryBoardExtractionTool::myID = UUID("");
const char *SeqSummaryFastMovingStoryBoardExtractionTool::myName = "SeqSummaryFastMovingStoryBoardExtraction";

const UUID SeqSummaryFastMovingStoryBoardExtractionInterfaceABC::myID = UUID();

//=============================================================================
SeqSummaryFastMovingStoryBoardExtractionInterfaceABC::SeqSummaryFastMovingStoryBoardExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SeqSummaryFastMovingStoryBoardExtractionInterface::SeqSummaryFastMovingStoryBoardExtractionInterface(SeqSummaryFastMovingStoryBoardExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SeqSummaryFastMovingStoryBoardExtractionInterface::~SeqSummaryFastMovingStoryBoardExtractionInterface()
{

}

//----------------------------------------------------------------------------
void SeqSummaryFastMovingStoryBoardExtractionInterface::destroy(void)
{
	delete m_ExtractionTool; 
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionInterface::
SetSourceMedia(MomVop *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionInterface::SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  return m_ExtractionTool->SetSourceMediaFrameNumber(FrameNo);
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionInterface::SetSourceMediaFilename(char *aFilename)
{
  return m_ExtractionTool->SetSourceMediaFilename(aFilename);
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionInterface::SetSourceWavFilename(char *aFilename)
{
  return m_ExtractionTool->SetSourceWavFilename(aFilename);
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionInterface::SetDescriptorInterface(GenericDSInterfaceABC *ParentDSInterface)
{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionInterface::InitExtracting(void)
{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionInterface::StartExtracting(void)
{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionInterface::NewSequence(void)
{
  return m_ExtractionTool->NewSequence();
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionInterface::PostExtraction(void)
{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& SeqSummaryFastMovingStoryBoardExtractionInterface::GetInterfaceID(void)
{
	return myID; 
}

//----------------------------------------------------------------------------
const char *SeqSummaryFastMovingStoryBoardExtractionInterface::GetName(void)
{
	return myName; 
}


//=============================================================================
SeqSummaryFastMovingStoryBoardExtractionTool::
SeqSummaryFastMovingStoryBoardExtractionTool():
m_Interface(this),
m_distthresh(0.25),
m_nnfraction(30),
m_nn(1),
m_frameduration((float)m_nn/(float)m_nnfraction),
m_accelnoise(1500.0),
m_alpha(0.01),
m_measurenoise(10.0),
m_stddeviation(0.4),
m_lastKeyFrameTime(0),
m_DescriptorInterface(0),
m_SequentialSummaryInterface(0),
m_SyncTimeInterface(0),
m_AudioDurationInterface(0),
m_WavFilename(0),
m_Media(0),
//m_Vop(0),
m_FrameNumber(0),
m_FirstFrame(1),
m_SecondFrame(0),
F(0),
Q(0),
H(0),
R(0)
{
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SeqSummaryFastMovingStoryBoardExtractionTool::
SeqSummaryFastMovingStoryBoardExtractionTool(GenericDSInterface *theParent):
m_Interface(this),
m_distthresh(0.25),
m_nnfraction(30),
m_nn(1),
m_frameduration((float)m_nn/(float)m_nnfraction),
m_accelnoise(1500.0),
m_alpha(0.01),
m_measurenoise(10.0),
m_stddeviation(0.4),
m_lastKeyFrameTime(0),
m_DescriptorInterface(0),
m_SequentialSummaryInterface(0),
m_SyncTimeInterface(0),
m_AudioDurationInterface(0),
m_WavFilename(0),
m_Media(0),
//m_Vop(0),
m_FrameNumber(0),
m_FirstFrame(1),
m_SecondFrame(0),
F(0),
Q(0),
H(0),
R(0)
{
  SetDescriptorInterface(theParent);
  SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
SeqSummaryFastMovingStoryBoardExtractionTool::
~SeqSummaryFastMovingStoryBoardExtractionTool()
{
//        if (!m_Vop) freevop(m_Vop);
	if (F) doubleArrayDestroy(F);
	if (Q) doubleArrayDestroy(Q);
	if (H)doubleArrayDestroy(H);
	if (R) doubleArrayDestroy(R);
	if (kalState.P) doubleArrayDestroy(kalState.P);
}

//----------------------------------------------------------------------------
bool SeqSummaryFastMovingStoryBoardExtractionTool::SupportsPush(void)
{
	return true; 
}

//----------------------------------------------------------------------------
bool SeqSummaryFastMovingStoryBoardExtractionTool::SupportsPull(void)
{
	return false; 
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::SetSourceMedia(MomVop *media)
{
  if (!media) return -1;

  m_Media=media;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::SetSourceMediaFrameNumber(unsigned long FrameNo)
{
  m_FrameNumber=FrameNo;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::SetSourceMediaFilename(char *aFilename)
{
  m_MediaFilename=aFilename;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::SetSourceWavFilename(char *aFilename)
{
  m_WavFilename=aFilename;
  return 0;
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::SetDescriptorInterface(GenericDSInterfaceABC *ParentDSInterface)
{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionTool::InitExtracting(void)
{
  if (!m_Media) return (unsigned long)-1;
  //this is not a very good error value for an usigned long...
  if (!m_DescriptorInterface) return (unsigned long)-1;
  //this is not a very good error value for an usigned long...

  //read parameters from parameter file
  m_distthresh=atof(getpara("distance_threshold","0.25"));
  m_nnfraction=atoi(getpara("nnfraction","30"));
  m_nn=atoi(getpara("nn","1"));
  m_frameduration=(float)m_nn/(float)m_nnfraction;
  m_accelnoise=atof(getpara("accelleration_noise","1500.0"));
  m_alpha=atof(getpara("alpha","0.01"));
  m_measurenoise=atof(getpara("measurement_noise","10.0"));
  m_stddeviation=atof(getpara("standard_deviation","0.4"));
	


	//for scene cut and key frame detection
	m_FirstFrame=1;
	m_SecondFrame=0;
	Q = setProcessNoise(m_frameduration, m_accelnoise);
	R = setMeasureNoise(m_measurenoise);


	//create basic structure of description
	NewSequence();

	/* compute directory name from media filename*/
//	m_KLTFeaturePoints.SetMediaFilename(m_Media->GetVideoFileName());
	m_KLTFeaturePoints.SetMediaFilename(m_MediaFilename);
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SeqSummaryFastMovingStoryBoardExtractionTool::StartExtracting(void)
{
        int listsize;
	TKLTFeature *l_finalfeaturelist;
	DoubleArray *C;    //feature poit matrix
	DoubleArray *CT;
	DoubleArray *FT;   //transposed transition matrix
	DoubleArray *HT;   //transposed measure matrix
	DoubleArray *S;    //covariance of innovation
	DoubleArray *S_1;  //inverted S
	DoubleArray *W;
	DoubleArray *WT;
	DoubleArray *work,*work1; //tmporary matrices
	unsigned long returnval=0;

	//check for descriptor
	if (!m_SequentialSummaryInterface) return (unsigned long)-1;

#ifdef USEXML
	if (m_SequentialSummaryInterface->GetDSName()!= "SequentialSummary")
	{
	  return (unsigned long)-1;	//this is not a very good error value for an usigned long...
	}
#endif //USEXML

	//check for media
	if (!m_Media) return (unsigned long)-1;	//this is not a very good error value for an usigned long...

	/* perform extraction of feature from media
	1. call KLTFeaturePoints GetKLTFeaturePoints
	2. perform scene cut and key frame detection*/

	/* 1 */
	/* compute feature points*/
	l_finalfeaturelist=
	  m_KLTFeaturePoints.GetKLTFeaturePoints(m_Media,&listsize);
	//	  m_KLTFeaturePoints.GetKLTFeaturePoints(m_Vop,&listsize);

	/* 2 */

#ifdef VERBOSE
	fprintf(stderr,"k");
	fflush(stderr);
#endif //VERBOSE

	if (!listsize)
	{
	  fprintf(stderr,"Warning: Empty feature point list\n");
	  return 0;
	}

	//write feature points to matrix C
	C=doubleArrayCreate(1, listsize, 1, FEATURENBR);

	int i;
	for (i=0; i<listsize; i++)
	{
	  ARRAY(C->array,i+1,1) = (double) l_finalfeaturelist[i].x; 
	  ARRAY(C->array,i+1,2) = (double) l_finalfeaturelist[i].y; 
	  ARRAY(C->array,i+1,3) = (double) 0.0; 
	  ARRAY(C->array,i+1,4) = (double) 0.0; 
	  ARRAY(C->array,i+1,5) = (double) l_finalfeaturelist[i].x; 
	  ARRAY(C->array,i+1,6) = (double) l_finalfeaturelist[i].y; 
	  ARRAY(C->array,i+1,7) = (double) l_finalfeaturelist[i].val; 
	}

	CT = doubleArrayTranspose(C);
	doubleArrayDestroy(C);

	if (m_FirstFrame)
	{
	  m_FirstFrame = 0;
	  m_SecondFrame = 1;  //check if this value can be treated equally to range
	  m_Range =1;

		returnval=2; //extract first frame as keyframe
 
	  if (F) doubleArrayTranspose(F);
	  F = setTransitionMatrix(FRAMEDURATION);
		//Q = setProcessNoise(FRAMEDURATION, ACCELNOISE); = const
	  kalState.P = Q;
	  if (H) doubleArrayTranspose(H);
	  H = setMeasureMatrix();
		//R = setMeasureNoise(MEASURENOISE); = const
	  kalState.X = linkedListCreate();
	
	  int i;
	  LinkedList *trackList;
	  for (i = CT->loLimY; i <= CT->upLimY; i++)
		{
	    trackList = linkedListCreate();
	    work = doubleArraySubArray(CT, 1, 7, i, i);
	    doubleArrayOrigin(work,1,1);
	    linkedListAddLast(trackList, work);		//Initiate a track list 
	    linkedListAddLast(kalState.X, trackList);
	    //Create a list of track lists 
	  }

#ifdef USEXML
//		char time[200];

		//this is the first frame, so this always creates an entry in the DS

		//create SequentialSummary->FrameProperty node
		GenericDS l_FrameProperty=m_SequentialSummaryInterface->CreateChild("FrameProperty");
		GenericDSInterfaceABC *l_FramePropertyInterface=l_FrameProperty.GetInterface();

		//create FrameProperty->ImageLocator node
		GenericDS l_ImageLocator=l_FramePropertyInterface->CreateChild("ImageLocator");
		GenericDSInterfaceABC *l_ImageLocatorInterface=l_ImageLocator.GetInterface();

		//create ImageLocator->MediaURL node
		GenericDS l_MediaURL=l_ImageLocatorInterface->CreateChild("MediaURL");
		GenericDSInterfaceABC *l_MediaURLInterface=l_MediaURL.GetInterface();

		//create FrameProperty->RefTime node
		GenericDS l_RefTime=l_FramePropertyInterface->CreateChild("RefTime");
		GenericDSInterfaceABC *l_RefTimeInterface=l_RefTime.GetInterface();

		//create FrameProperty->SyncTime node
		m_SyncTime=l_FramePropertyInterface->CreateChild("SyncTime");
		m_SyncTimeInterface=m_SyncTime.GetInterface();

		//create SyncTime->MediaRelTimePoint node
		GenericDS l_MediaRelTimePointSync=m_SyncTimeInterface->CreateChild("MediaRelTimePoint");
		GenericDSInterfaceABC *l_MediaRelTimePointSyncInterface=l_MediaRelTimePointSync.GetInterface();

		//create RefTime->MediaRelTimePoint
		GenericDS l_MediaRelTimePointRef=l_RefTimeInterface->CreateChild("MediaRelTimePoint");
		GenericDSInterfaceABC *l_MediaRelTimePointRefInterface=l_MediaRelTimePointRef.GetInterface();

		//calculate data
		m_lastKeyFrameTime= (int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR);

		//set SyncTime->MediaRelTimePoint
		l_MediaRelTimePointSyncInterface->SetValue("PT0S1000F");	

		//set RefTime->MediaRelTimePoint
		l_MediaRelTimePointRefInterface->SetValue("PT0S1000F");
		l_MediaRelTimePointRefInterface->SetAttribute("TimeBase", "RefLocator (1)");


		char keyfilename[MAXFPFILENAMELEN];
		char filename[MAXFPFILENAMELEN];

		//compute path and filename of keyframe
		sprintf(keyfilename,"key%05ld.jpg",m_FrameNumber);
//		if (m_intermediaryIO)
//		{
			if (m_MediaFilename)
			{
				//compute directory name from media filename
				strncpy(filename,m_MediaFilename,MAXFPFILENAMELEN-1);
				strncat(filename,".dir/",MAXFPFILENAMELEN-1-strlen(filename));
				strcat(filename,keyfilename);
				strcpy(keyfilename, filename);
			}
//		}

		//set ImageLocator->MediaURL
		l_MediaURLInterface->SetAttribute("URI", keyfilename);	

#ifdef USESMIL
//		char time2[200];
		char keyfilename2[MAXFPFILENAMELEN];

		//write first frame to .smil file
		//Note that the duration (to next frame) is not yet known.
		//This will be written when the next keyframe has been detected
		sprintf(keyfilename2,"key%05ld.jpg",m_FrameNumber);
		smilout<<"			<img src=\""<<keyfilename2<<"\" region=\"pic_region\" begin=\"0.0s\" dur=\"";
#endif //USESMIL
#endif //USEXML

	}	else //not first frame
	{ 
	  kalState.Init = 0;
	  kalState.Term = 0;
	  kalState.activity = 0;

	  //state prediction cov. 
	  FT = doubleArrayTranspose(F);
	  work = doubleArrayMultiply(FT,kalState.P);
	  work1 = doubleArrayMultiply(work,F);
	  kalState.P = doubleArrayAdd(work1, Q);
	  doubleArrayDestroy(FT);
	  doubleArrayDestroy(work);
	  doubleArrayDestroy(work1);
      
	  //Covariance of Innovation 
	  HT = doubleArrayTranspose(H);
	  work = doubleArrayMultiply(HT, kalState.P);
	  work1 = doubleArrayMultiply(work,H);
	  S = doubleArrayAdd(R, work1);

	  doubleArrayDestroy(work);
	  doubleArrayDestroy(work1);
      
	  //Gain matrix 
	  doubleArrayOrigin(S, 0, 0);
	  S_1 = doubleArrayInverse(S);
	  doubleArrayOrigin(S_1, 1, 1);
	  doubleArrayOrigin(S, 1, 1);
	  work = doubleArrayMultiply(S_1, HT);
	  doubleArrayDestroy(HT);
	  W = doubleArrayMultiply(work, kalState.P);
 	  doubleArrayDestroy(work);

	  //Update of state Covariance 
	  WT = doubleArrayTranspose(W);
	  work = doubleArrayMultiply(WT, S);
	  doubleArrayDestroy(S);
	  doubleArrayDestroy(WT);
	  work1 = doubleArrayMultiply(work, W);
	  doubleArrayDestroy(work);
	  work=kalState.P;
	  kalState.P = doubleArraySub(work, work1);
     
	  doubleArrayDestroy(work);
	  doubleArrayDestroy(work1);
     
	  //-----
	  kalmanOneStep(&kalState, CT, F, H, W, S_1, DISTTHRESH, FRAMEDURATION);
	  doubleArrayDestroy(S_1);
    doubleArrayDestroy(W);

	  //Shot boundarie detection and key frame extraction 
	  if (m_SecondFrame)
		{
	    m_stdDeviation = STDDEVIATION;
	    
			//Initialisation of the stationary state (change rate)  
	    m_Mean = kalState.activity;
	    m_SecondFrame = 0;

	    //add key frame
	    //has to be modified for XML usage
	    //	    m_DescriptorInterface->AddKeyFrame(m_FrameNumber);

	    //create color histogram for frame
	    //	    ExtractColorHistogram();
	    returnval=1;
	  } else 
		{
	    if ((kalState.activity > (m_Mean + m_stdDeviation))	|| (kalState.activity < (m_Mean - m_stdDeviation)))
			{
	      if (m_Range < 16) //The max. duration of a  gradual change is 16 frames  
				{
					//-- Detection of shot boundaries --
					//has to be modified for XML usage
					returnval=1;
				} else  //Detection of a stationary or weak non-stationary state 
				{
					//int kf;
					//kf = m_FrameNumber - (m_Range/2);

#ifdef USEXML

#ifdef USESMIL
					char time2[200];
					char keyfilename2[MAXFPFILENAMELEN];

					time2[0] = 0;
					ms2realtime(((int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR)-m_lastKeyFrameTime), time2);
					sprintf(keyfilename2,"key%05ld.jpg",m_FrameNumber);

					//write duration between last frame and this frame to .smil file
					smilout<<time2<<"\"/>\n";

					//write next keyframe name to .smil file
					//Note that the duration (to next frame) is not yet known.
					//This will be written when the next keyframe has been detected
					smilout<<"			<img src=\""<<keyfilename2<<"\" region=\"pic_region\" dur=\"";
#endif //USESMIL

					char time[200];

					//create SequentialSummary->FrameProperty node					
					GenericDS l_FrameProperty=m_SequentialSummaryInterface->CreateChild("FrameProperty");
					GenericDSInterfaceABC *l_FramePropertyInterface=l_FrameProperty.GetInterface();

					//create FrameProperty->ImageLocator node
					GenericDS l_ImageLocator=l_FramePropertyInterface->CreateChild("ImageLocator");
					GenericDSInterfaceABC *l_ImageLocatorInterface=l_ImageLocator.GetInterface();

					//create ImageLocator->MediaURL node
					GenericDS l_MediaURL=l_ImageLocatorInterface->CreateChild("MediaURL");
					GenericDSInterfaceABC *l_MediaURLInterface=l_MediaURL.GetInterface();

					//create FrameProperty->RefTime node
					GenericDS l_RefTime=l_FramePropertyInterface->CreateChild("RefTime");
					GenericDSInterfaceABC *l_RefTimeInterface=l_RefTime.GetInterface();

					if (m_SyncTimeInterface) //do we have the FrameProperty->SyncTime node from the last frame?
					{
						//now we can calculate the duration between the last frame and this frame
						//and assign this to the FrameProperty->SyncTime node
						strcpy(time, "D");
						ms2time(((int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR)-m_lastKeyFrameTime), time);

						//create SyncTime->MediaDuration and set data
						m_SyncTimeInterface->CreateChild("MediaDuration").GetInterface()->SetValue(time);
					}

					//create new FrameProperty->SyncTime node
					m_SyncTime=l_FramePropertyInterface->CreateChild("SyncTime");
					m_SyncTimeInterface=m_SyncTime.GetInterface();

					//create SyncTime->MediaRelTimePoint node
					GenericDS l_MediaRelTimePointSync=m_SyncTimeInterface->CreateChild("MediaRelTimePoint");
					GenericDSInterfaceABC *l_MediaRelTimePointSyncInterface=l_MediaRelTimePointSync.GetInterface();

					//create RefTime->MediaRelTimePoint
					GenericDS l_MediaRelTimePointRef=l_RefTimeInterface->CreateChild("MediaRelTimePoint");
					GenericDSInterfaceABC *l_MediaRelTimePointRefInterface=l_MediaRelTimePointRef.GetInterface();

					//calculate data
					m_lastKeyFrameTime= (int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR);

					strcpy(time, "PT");
					ms2time(m_lastKeyFrameTime, time);

					//set SyncTime->MediaRelTimePoint
					l_MediaRelTimePointSyncInterface->SetValue(time);	

					m_lastKeyFrameTime= (int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR);

					strcpy(time, "PT");
					ms2time((int)((m_FrameNumber*1000)/(m_nnfraction)), time);

					//set RefTime->MediaRelTimePoint
					l_MediaRelTimePointRefInterface->SetValue(time);

					char keyfilename[MAXFPFILENAMELEN];
					char filename[MAXFPFILENAMELEN];

					//compute path and filename of keyframe
					sprintf(keyfilename,"key%05ld.jpg",m_FrameNumber);
//					if (m_intermediaryIO)
//					{
						if (m_MediaFilename)
						{
							//compute directory name from media filename
							strncpy(filename,m_MediaFilename,MAXFPFILENAMELEN-1);
							strncat(filename,".dir/",MAXFPFILENAMELEN-1-strlen(filename));
							strcat(filename,keyfilename);
							strcpy(keyfilename, filename);
						}
//					}

					//set ImageLocator->MediaURL
					l_MediaURLInterface->SetAttribute("URI", keyfilename);	
#endif //USEXML

					returnval=2;	//indicate that the frame is a keyframe
				}

	      //-- Gaussian process initialisation --
	      m_Mean = kalState.activity;
	      m_stdDeviation = STDDEVIATION;
	      m_Range = 1;
	    } else
			{
	      //-- Gaussian process update --
	      m_Mean = (1 - ALPHA) * m_Mean + ALPHA * kalState.activity;
	      m_stdDeviation = sqrt((1-ALPHA) * SQUARE(m_stdDeviation) +
				    ALPHA * SQUARE(m_Mean - 
						   kalState.activity));
	      m_Range ++;
	    }
	  }
	}
	doubleArrayDestroy(CT);
	return returnval;
}

//----------------------------------------------------------------------------
//called by InitExtracting
unsigned long SeqSummaryFastMovingStoryBoardExtractionTool::NewSequence(void)
{
#ifdef USEXML
	if (!m_DescriptorInterface)  return (unsigned long)-1;

	//create SequentialSummary node (root node of the DS)
	m_SequentialSummary=m_DescriptorInterface->CreateChild("SequentialSummary");
	m_SequentialSummaryInterface=m_SequentialSummary.GetInterface();

	//create RefLocator and AudioSegmentLocator if file name is available
	if (m_MediaFilename)
	{
		//create SequentialSummary->RefLocator
	  GenericDS l_VideoSegmentLocator=m_SequentialSummaryInterface->CreateChild("RefLocator");
	  GenericDSInterfaceABC *m_VideoSegmentLocatorInterface=l_VideoSegmentLocator.GetInterface();

	  //create RefLocator->MediaURL
		GenericDS l_MediaURL=m_VideoSegmentLocatorInterface->CreateChild("MediaURL");
	  GenericDSInterfaceABC *l_MediaURLInterface=l_MediaURL.GetInterface();

		//set RefLocator->MediaURL
	  l_MediaURLInterface->SetAttribute("URI",m_MediaFilename);

		//create SequentialSummary->AudioSegmentLocator
	  GenericDS l_AudioSegmentLocator=m_SequentialSummaryInterface->CreateChild("AudioSegmentLocator");
	  GenericDSInterfaceABC *l_AudioSegmentLocatorInterface=l_AudioSegmentLocator.GetInterface();

	  //create AudioSegmentLocator->MediaURL
		GenericDS l_AudioURL=l_AudioSegmentLocatorInterface->CreateChild("MediaURL");
	  GenericDSInterfaceABC *l_AudioURLInterface=l_AudioURL.GetInterface();

		//create AudioSegmentLocator->MediaDuration
		//note: this node is stored as a data member because the MediaDuration
		//      is unknown till the end of the extraction. The value will
		//      be assigned after the extraction finished.
	  m_AudioDuration=l_AudioSegmentLocatorInterface->CreateChild("MediaDuration");
	  m_AudioDurationInterface=m_AudioDuration.GetInterface();

		char filename[MAXFPFILENAMELEN];
		char tmpfilename[MAXFPFILENAMELEN];

		filename[0] = 0;

		//compute path and filename of fast audio file
//		if (m_intermediaryIO)
//		{
			if (m_MediaFilename)
			{
				strncpy(filename, m_MediaFilename,MAXFPFILENAMELEN-1);
				*(strrchr(filename, '.')) = 0;
				sprintf(tmpfilename, "%s", (strrchr(filename, '/') + sizeof(char)));
				sprintf(filename, "%s.dir/%s%f.wav", m_MediaFilename, tmpfilename, FASTMSB_AUDIO_FACTOR);
			}
//		}

		//set AudioSegmentLocator->MediaURL
	  l_AudioURLInterface->SetAttribute("URI",filename);
	}

#ifdef USESMIL
		char filename2[MAXFPFILENAMELEN];
		char filename3[MAXFPFILENAMELEN];

		//compute path and filename of .smil file
		sprintf(filename2, "%s", m_MediaFilename);
		*(strrchr(filename2, '.')) = 0;
		sprintf(filename3, "%s%f", (strrchr(filename2, '/') + sizeof(char)), FASTMSB_AUDIO_FACTOR);
		sprintf(filename2, "%s.dir/%s.smil", m_MediaFilename, filename3);

		//create .smil file
		smilout.open(filename2, ios::out | ios::trunc);

		//write header information
		smilout<<"<smil>\n <head>\n		<meta name=\"title\" content=\"FMSB\" />\n		<meta name=\"author\" content=\"generated by XM SeqSummaryMovingStoryBoard\" />\n		<meta name=\"copyright\" content=\"IBM Almaden Research\" />\n		<meta name=\"keywords\" content=\"MSB\" />\n		<meta name=\"description\" content=\"keyframes synchronized with fast audio\" />\n		<layout type=\"text/smil-basic-layout\">\n";
		smilout<<"			<root-layout background-color=\"black\"/>\n";
		smilout<<"			<region id=\"pic_region\" left=\"0\" top=\"0\" z-index=\"1\" />\n		</layout>\n	</head>\n	<body>\n		<par>\n";
		//write refernce to fast audio file
		smilout<<"		<audio src=\""<<filename3<<".wav\" type=\"audio/x-wav\"/>\n		<seq>\n";
#endif //USESMIL
#endif //USEXML

	return 0;
}

//----------------------------------------------------------------------------
//called after extraction is complete
unsigned long SeqSummaryFastMovingStoryBoardExtractionTool::PostExtraction(void)
{
	char time[200];
	char wavout[MAXFPFILENAMELEN];
	char tmpstr[MAXFPFILENAMELEN];

#ifdef USEXML
	//calculate and set SequentialSummary->AudioSegmentLocator->MediaDuration
	strcpy(time, "D");
	ms2time((int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR), time);
	m_AudioDurationInterface->SetValue(time);

	if (m_SyncTimeInterface)
	{
		//calculate and set MediaDuration of last keyframe
		strcpy(time, "D");
		ms2time(((int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR)-m_lastKeyFrameTime), time);

		m_SyncTimeInterface->CreateChild("MediaDuration").GetInterface()->SetValue(time);
	}

#ifdef USESMIL
		char time2[200];

		//calculate duration of last frame
		time2[0] = 0;
		ms2realtime(((int)(((m_FrameNumber*1000)/(m_nnfraction))*FASTMSB_AUDIO_FACTOR)-m_lastKeyFrameTime), time2);
		
		//write duration and closing tags to .smil file
		smilout<<time2<<"\"/>\n			</seq>\n		</par>\n	</body>\n</smil>\n"<<endl;
		smilout.close();
#endif //USESMIL
#endif //USEXML

	//compute path and name of fast audio file
	sprintf(wavout, "%s", m_MediaFilename);
	*(strrchr(wavout, '.')) = '\0';
	sprintf(tmpstr, "%s", (strrchr(wavout, '/') + sizeof(char)));
	sprintf(wavout, "%s.dir/%s%f.wav", m_MediaFilename, tmpstr, FASTMSB_AUDIO_FACTOR);
	
	//create fast audio file using wsola algorithm
	wsola(m_WavFilename, wavout, FASTMSB_AUDIO_FACTOR);

	return 0;
}

//----------------------------------------------------------------------------
/** convert a given time from milliseconds to a MediaTime string (xxxHxxMxxSxxxN).
 *	The result will be appended (!) to the given result string. This method
 *  uses milliseconds as unit for N.
 *
 *  @author Jan H. Pieper
 *
 *  @param ms  time in milliseconds
 *  @param res char* to receive the result of the method. The MediaTime string will be appended to the given string.
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::ms2time(int ms, char* res)
{
	char time[200];
	char str[200];

	int temp = ms;
	int msec, sec, min, h;

	msec=temp%1000;
	temp=(int) temp/1000;
	sec=temp%60;
	temp=(int) temp/60;
	min=temp%60;
	h=(int) temp/60;

	strcpy(time, res);

	if (h>0)
	{
		sprintf(str,"%iH", h);
		strcat(time, str);
	}

	if (min>0)
	{
		sprintf(str,"%iM", min);
		strcat(time, str);
	}

	if (sec>0)
	{
		sprintf(str,"%iS", sec);
		strcat(time, str);
	}
	
	if (ms>0)
	{
		sprintf(str,"%iN", msec);
		strcat(time, str);
	}

	strcpy(res, time);
}

//----------------------------------------------------------------------------

/** convert a given time from milliseconds to a RealTime string (dd:hh:mm:ss.xyz).
 *	The result will be appended (!) to the given result string. This method
 *  uses milliseconds as unit for xyz.
 *
 *  @author Jan H. Pieper
 *
 *  @param ms  time in milliseconds
 *  @param res char* to receive the result of the method. The RealTime string will be appended to the given string.
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::ms2realtime(int ms, char* res)
{
	int temp = ms;
	int msec, sec, min, h;

	msec=temp%1000;
	temp=(int) temp/1000;
	sec=temp%60;
	temp=(int) temp/60;
	min=temp%60;
	h=(int) temp/60;

	sprintf(res,"%s%02i:%02i:%02i.%03i", res, h, min, sec, msec);
}


//----------------------------------------------------------------------------
/**
 *
 *  Time Scale Modification for MPEG-7
 *
 *  Written by:  Gilad Cohen (giladc@il.ibm.com)
 *               IBM Research, Haifa ISRAEL
 *
 *  Last update: July 31, 2000
 * 
 *  Version:     1.0
 *
 *  Description:
 *
 *  This file includes functions for reading and writing WAV
 *  audio files.
 *
 */

//sth reading audio-files (e.g, .wav) must be part of MediaIO
//----------------------------------------------------------------------------
//Read unsigned long, little-endian 
unsigned long SeqSummaryFastMovingStoryBoardExtractionTool::ReadUnsignedLong(FILE *fp)
{
 unsigned char uc1, uc2, uc3, uc4;

 uc1 = getc(fp);
 uc2 = getc(fp);
 uc3 = getc(fp);
 uc4 = getc(fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error reading WAV header !\n\n");
  exit(1);
 }

 return ( ((unsigned long)uc4 << 24) | 
          ((unsigned long)uc3 << 16) |
          ((unsigned long)uc2 <<  8) |
           (unsigned long)uc1          );
}                                

//----------------------------------------------------------------------------
//Read unsigned short, little-endian 
unsigned short SeqSummaryFastMovingStoryBoardExtractionTool::ReadUnsignedShort(FILE *fp)
{
 unsigned char uc1, uc2;

 uc1 = getc(fp);
 uc2 = getc(fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error reading WAV header !\n\n");
  exit(1);
 }

 return ( ((unsigned short)uc2 << 8) | 
           (unsigned short)uc1        );
}

//----------------------------------------------------------------------------
//Read short, little-endian 
short SeqSummaryFastMovingStoryBoardExtractionTool::ReadShort(FILE *fp)
{
 unsigned char uc1, uc2;

 uc1 = getc(fp);
 uc2 = getc(fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error reading audio data from WAV file !\n\n");
  exit(1);
 }

 return ( ((short)uc2 << 8) | 
           (short)uc1        );
}

//----------------------------------------------------------------------------
//Write unsigned short, little-endian 
void SeqSummaryFastMovingStoryBoardExtractionTool::WriteUnsignedShort(FILE *fp, unsigned short us)
{
 putc(us   , fp);
 putc(us>>8, fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error writing header to WAV file !\n\n");
  exit(1);
 }
}

//----------------------------------------------------------------------------
//Write unsigned long, little-endian 
void SeqSummaryFastMovingStoryBoardExtractionTool::WriteUnsignedLong(FILE *fp, unsigned long ul)
{
 long datum;

 datum =(long)( (ul) & 0xff);
 putc(datum, fp);
 datum = (long)((ul >> 8) & 0xff);
 putc(datum, fp);
 datum = (long)((ul >> 16) & 0xff);
 putc(datum, fp);
 datum = (long)((ul >> 24) & 0xff);
 putc(datum, fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error writing header to WAV file !\n\n");
	exit(1);
 }
}

//----------------------------------------------------------------------------
//Write signed short, little-endian 
void SeqSummaryFastMovingStoryBoardExtractionTool::WriteShort(FILE *fp, short s)
{
 putc(s   , fp);
 putc(s>>8, fp);

 if (ferror(fp))
 {
	fprintf(stderr,"\n\n Error writing audio data to WAV file !\n\n");
  exit(1);
 }
}

//----------------------------------------------------------------------------
/**InPlaceFloatToShortConversion
 *
 * This function converts a block of PCM samples in floating
 * point format to 16 bit short integer point format. The
 * conversion is done "in-place", such that the input buffer
 * of floating points will hold on output the same data
 * converted to short integers.
 *
 * Arguments:
 *
 *   pvBuffer -  On input: A buffer containing the floating
 *               point samples.
 *               On output: The buffer will hold the samples
 *               converted to short integer.
 *   iLen     -  Number of floating point samples in the
 *               buffer on input (this will be the number
 *               of short integer samples in the buffer
 *               on output).
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::InPlaceFloatToShortConversion(void* pvBuffer, const int iLen)
{
 int n;
 float *pfInput  = (float *)pvBuffer;
 float fInput;
 short *psOutput = (short *)pvBuffer;

 //Loop over all input samples 
 for(n=0;n<iLen;n++)
 {
   //Load input samples 
   fInput = pfInput[n];

   //Convert to short (while checking overflow & underflow) 
	 if (fInput < (float)SHRT_MIN )
	 {
		 psOutput[n]= SHRT_MIN;
	 } else if (fInput > (float)SHRT_MAX )
	 {
		 psOutput[n]= SHRT_MAX;
	 } else if (fInput >= 0.0f)
	 {
	   psOutput[n]=(short)(fInput+0.5f);
	 } else
	 {
		 psOutput[n]=(short)(fInput-0.5f);
	 }
 }
}

//----------------------------------------------------------------------------
/**ParseWavHeader
 *
 * This function parses a buffer which holds a WAV header
 * read from a WAV file.
 *
 * Input arguments:
 *
 *   header - A buffer containing the WAV header. It must
 *            hold at least the first 36 (=READ_WAV_HEADER_SIZE)
 *            bytes of the WAV header.
 *
 * Output arguments:
 *
 *   pWavStruct - On output this structure will hold
 *                all the WAV header field values
 *
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::ParseWavHeader(FILE *fp, WavStruct *pWavStruct)
{
 //Parse "RIFF" string (chars 0-3)
 if(4!=fread(pWavStruct->riff,1,4, fp))
 {
	fprintf(stderr,"\n\n Error reading WAV file header !\n\n");
  exit(1);
 }
 if (strncmp("RIFF",pWavStruct->riff, 4))
 {
	fprintf(stderr,"\n\n The input file is not a RIFF file !\n\n");
  exit(1);
 }

 //Parse RIFF chunk length (chars 4-7) 
 pWavStruct->riffLen = ReadUnsignedLong(fp);

 //Parse "WAVE" string (chars 8-11)
 if(4!=fread(pWavStruct->wave,1,4,fp))
 {
	fprintf(stderr,"\n\n Error reading WAV file header !\n\n");
  exit(1);
 }

 if (strncmp("WAVE",pWavStruct->wave, 4))
 {
	fprintf(stderr,"\n\n The input file is not a WAVE file !\n\n");
  exit(1);
 }

 //Parse the "fmt" tag (chars 12-15) 
 if(4!=fread(pWavStruct->fmt,1,4,fp))
 {
	fprintf(stderr,"\n\n Error reading WAV file header !\n\n");
  exit(1);
 }
 
 if (strncmp("fmt ", pWavStruct->fmt, 4))
 {
	fprintf(stderr,"\n\n The input WAV file does not have a ""fmt"" chunk !\n\n");
  exit(1);
 }

 //Parse size of format chunk (chars 16-19) 
 pWavStruct->fmtLen=ReadUnsignedLong(fp);
 
 //Parse audio format tag (chars 20-21) 
 pWavStruct->format=ReadUnsignedShort(fp);
 
 //Only PCM format supported 
 if(WAVE_FORMAT_PCM!=pWavStruct->format)
 {
  fprintf(stderr,"\n\n Only PCM WAV format supported !\n\n");
  exit(1);
 }

 //Parse number of channels (chars 22-23) 
 pWavStruct->numChannels=ReadUnsignedShort(fp);
 
 //Check number of channels 
 if(pWavStruct->numChannels>2 || pWavStruct->numChannels<=0)
 {
   fprintf(stderr,"\n\n Only mono or stereo WAV files are supported !\n\n");
   exit(1);
 }

 //Parse samples per secods (chars 24-27) 
 pWavStruct->samplesPerSecond=ReadUnsignedLong(fp);
 
 //Parse average bytes per sec (chars 28-31) 
 pWavStruct->avgBytes=ReadUnsignedLong(fp);
 
 //Parse block align (chars 32-33) 
 pWavStruct->blockAlign=ReadUnsignedShort(fp);
 
 //Parse bits Per sample (chars 34-35) 
 pWavStruct->bitsPerSample=ReadUnsignedShort(fp);
 
 //Only 16 bit per sample supported 
 if(16!=pWavStruct->bitsPerSample)
 {
  fprintf(stderr,"\n\n Only 16 bit per sample WAV supported !\n\n");
	exit(1);
 }

 //Parse "data" string (usually in chars 36-39). However,
 //Since some WAV files have other chunks between the "fmt" chunk
 //and the "data" cunk, we must move ahead in the file until
 //we read the "data" string.

 strcpy(pWavStruct->data,"\0"); 

	while(strncmp("data",pWavStruct->data, 4))
	{
		//Read new char to top of string and move other backwards 
		pWavStruct->data[0] = pWavStruct->data[1];
		pWavStruct->data[1] = pWavStruct->data[2];
		pWavStruct->data[2] = pWavStruct->data[3];
		pWavStruct->data[3] = getc(fp);        

    if(feof(fp))
		{
 			fprintf(stderr,"\n\n The input WAV header does not have a data chunk !\n");
			exit(1);
    }

    if(ferror(fp))
		{
     fprintf(stderr,"\n\n Error reading WAV file header !\n\n");
     exit(1);
    }
	}

	//Parse data length (usually chars 40-43) 
	pWavStruct->dataLen = ReadUnsignedLong(fp);
}

//----------------------------------------------------------------------------
/**WriteWavHeader
 *
 * This function writes a WAV header to a file, given the
 * description of the audio format.
 *
 * Input arguments:
 *   numChannles  - Number of channels (1-mono,2-stereo,etc.)
 *   frequency    - Sampling rate frequency (in Hz)
 *   bitPerSample - Number of bits per sample (i.e., 16 bit)
 *   DataSize     - Number of audio bytes to be written
 *   fp           - An opened file pointer. The WAV header
 *                  will be written to this file.
 *
 * Return:
 *
 *   The number of bytes written into the header buffer
 *   which is WRITE_WAV_HEADER_SIZE=44 bytes.
 */
int SeqSummaryFastMovingStoryBoardExtractionTool::WriteWavHeader(const int numChannels, const int frequency, const int bitPerSample, const int DataSize, FILE *fp)
{
 int index = 0;
 WavStruct wave;

 //Prepare WAV header structue  
 strcpy(wave.riff,"RIFF");
 wave.riffLen        = DataSize + 36;
 strcpy(wave.wave,"WAVE");
 strcpy(wave.fmt, "fmt ");
 wave.fmtLen         = 16;
 wave.format         = WAVE_FORMAT_PCM;
 wave.numChannels    = numChannels;
 wave.samplesPerSecond = frequency ;
 wave.bitsPerSample  = bitPerSample;
 wave.avgBytes       = wave.numChannels*wave.samplesPerSecond*wave.bitsPerSample/8;
 wave.blockAlign     = (wave.numChannels * wave.bitsPerSample) / 8;
 strcpy(wave.data,"data");
 wave.dataLen        = DataSize;

 //Write "RIFF" string (chars 0-3)
 if(0>fputs(wave.riff,fp))
 {
	fprintf(stderr,"\n\n Error writing WAV file header !\n\n");
  exit(1);
 }
 index+=4;

 //Write size of waveform chunk (chars 4-7) 
 WriteUnsignedLong(fp,wave.riffLen);
 index+=sizeof(long);

 //Write "WAVE" string (chars 8-11) 
 if(0>fputs(wave.wave,fp))
 {
	fprintf(stderr,"\n\n Error writing WAV file header !\n\n");
  exit(1);
 }
 index+=4;

 //Write "fmt " string (chars 12-15) 
 if(0>fputs(wave.fmt,fp))
 {
	fprintf(stderr,"\n\n Error writing WAV file header !\n\n");
  exit(1);
 }
 index+=4;

 //Write size of format chunk (chars 16-19) 
 WriteUnsignedLong(fp,wave.fmtLen); 
 index+=sizeof(long);

 //Write format tag (chars 20-21) 
 WriteUnsignedShort(fp,wave.format);    
 index+=sizeof(short);
  
 //Write number of channels (chars 22-23) 
 WriteUnsignedShort(fp,wave.numChannels);                  
 index+=sizeof(short);

 //Write samples per second (chars 24-27) 
 WriteUnsignedLong(fp,wave.samplesPerSecond); 
 index+=sizeof(long);

 //Write average bytes per second (chars 28-31) 
 WriteUnsignedLong(fp,wave.avgBytes); 
 index+=sizeof(long);

 //Write block alignment (chars 32-33) 
 WriteUnsignedShort(fp,wave.blockAlign);                  
 index+=sizeof(short);

 //Write bits per sample (chars 34-35) 
 WriteUnsignedShort(fp,wave.bitsPerSample);                  
 index+=sizeof(short);

 //Write "data" string (chars 36-39) 
 if(0>fputs(wave.data,fp))
 {
	fprintf(stderr,"\n\n Error writing WAV file header !\n\n");
  exit(1);
 }
 index+=4;

 //Write size of waveform data in bytes (chars 40-43)   
 WriteUnsignedLong(fp,wave.dataLen); 
 index+=sizeof(long);

 //Return the number of bytes written (should be WAV_HEADER_SIZE !)
 return(index);
}

//----------------------------------------------------------------------------
/**WriteWavFile
 *
 * This function writes a mono, 16 bit linear PCM audio
 * as a WAV file.
 *
 * Input arguments:
 *
 *   file_name - The WAV file name to be written
 *   x         - A buffer containing the mono audio
 *               data samples in floating point to be written to the WAV
 *               file (conversion to 16 bit fixed point will be done
 *               internally in this function)
 *   len       - The number of samples in the buffer
 *   freq      - The sampling rate in Hz of the audio data
 *
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::WriteWavFile(const char *file_name, const float x[], const long len, const int freq)
{
 int n;
 short *sx;
 FILE *fp = (FILE *)NULL;       //Output WAV file pointer 

 //Open WAV file for writing 
 if ( (FILE *)NULL == (fp = fopen(file_name,"wb")) )
 {
	fprintf(stderr,"\n\n Can't open %s WAV file for writing !\n\n",file_name);
	exit(1);
 }

 //Create a WAV header 
 WriteWavHeader(1,freq,16,len*2,fp);

 //In place conversion of output from floating point to short integer 
 InPlaceFloatToShortConversion((void *)x,len);

 //Write audio data to WAV file, as 16 bit signed short integer, little-endian 
 sx = (short *)x;
 for(n=0;n<len;n++)
 {
  WriteShort(fp,*sx);
  sx++;
 }

 //Close WAV file 
 fclose(fp);
}

//----------------------------------------------------------------------------
/**ReadWavFile
 *
 * This function reads the audio data from a given
 * WAV file, converts it to floating point and
 * saves it in a buffer. If the WAV file is stereo,
 * downmixig to mono is also performed.
 *
 * Input Arguments:
 *
 *    file_name   - The full name (including path) of the input file.
 *    len         - The total number of audio samples in the file
 *                  (in all channels combined).
 *    numChannles - The number of channels (1 for mono, 2 for stereo).
 *    headerSize  - WAV Header size (in bytes) in the input file to be skipped
 *                  when reading from the file.
 *
 * Output Arguments:
 *    x           - On output will hold the data read from the file, converted
 *                  to floating point format in mono. Memory will be allocated
 *                  internally by this function.
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::ReadWavFile(char *file_name, float **x, long *len, int *freq)
{
 int n;
 FILE *fp = (FILE *)NULL;
 WavStruct wavFmt;

 *x = (float *)NULL;

 //Open input WAV file 
 if ( (FILE *)NULL == (fp = fopen(file_name,"rb")) )
 {
	fprintf(stderr,"\n\n Can't open input %s WAV file !\n\n",file_name);
	exit(1);
 }

 //Parse WAV header 
 ParseWavHeader(fp,&wavFmt);

 //Check positive sampling rate 
 *freq=wavFmt.samplesPerSecond;
 
 //Number of audio samples per channel 
 *len=(wavFmt.dataLen>>1)/wavFmt.numChannels;

 //Allocate Memory 
 if ( (float *)NULL == (*x = (float*)malloc((int)(*len)*sizeof(float))) )
 {
  fprintf(stderr, "\n\n Can't allocate memory while Loading %s !\n\n",file_name) ;
  exit(1) ;
 }

 //Read samples from file and downmix stereo to mono if needed 
 n=0;
 if (1==wavFmt.numChannels) 
 {
   while (!feof(fp) && n<*len) 
   {
     *(*x+n)=(float)ReadShort(fp);    
     n++;
   } 
 } else
 {
   while (!feof(fp) && n<*len) 
   {
     short xl = ReadShort(fp);
     short xr = ReadShort(fp);

     *(*x+n)=((float)xl+(float)xr)/2.0f;    
     n++;
   } 
 }

 //Close input WAV file 
 fclose(fp);
}

//----------------------------------------------------------------------------
/**Time Scale Modification for MPEG-7
 *
 *  Written by:  Gilad Cohen (giladc@il.ibm.com)
 *               IBM Research, Haifa ISRAEL
 *
 *  Last update: July 31, 2000
 *
 *  Version:     1.0
 *
 *  Description:
 * 
 *  The following program performs time scale modification (TSM) of
 *  audio signals, i.e. speed-up or slow-down of the audio without
 *  changing pitch or timbre. It is written in portable ANSI-C, and
 *  was tested on Windows and IBM AIX platforms.
 *
 *  Time scaling is done using the WSOLA algorithm, presented in
 *  the paper: 
 *  W. Verhelst, M. Roenlands, "An overlap-add technique based on
 *  waveform similarity (WSOLA) for high quality time-scale modification
 *  of speech", Proc. Int. Conf. Acoustics, Speech and Signal Processing
 *  (ICASSP), 1993.
 *
 *  The input audio file must be a WAV file in the following format:
 *  - Mono or stereo,
 *  - Linear PCM (compressed WAV audio is not supported),
 *  - 16 bit per sample (signed),
 *  - At any sampling rate.
 *  The program will not run if the input WAV is not in this format.
 *  Use an external audio editing/conversion tool to save your data
 *  in this format.
 *
 *  The output audio file will be a WAV file in the same format as
 *  the input WAV file, only in mono.
 *
 *  Beware !! this is a straight forward implementation of the WSOLA
 *  algorithm. No attempts were made to optimize this code in any
 *  way in terms of memory consumption and CPU time. For example,
 *  memory is allocate once for all the input and output audio, and
 *  *all* the input file is loaded into memory during initialization.
 *  Take this into consideration when processing huge audio files.
 *  Also, processing time is *very* long when the input sampling rate
 *  is high (44 Khz for example).
 *
 *  Highly optimized, real-time buffer-to-buffer APIs for audio time
 *  scaling is available from IBM, suitable for integration into
 *  audio playback applications. Using this library, 44 Khz audio time
 *  scaling is done at up to 8% real time, as measured on a Pentium 333 Mhz.
 *  More information is available on the IBM Intranet URL site:
 *  http://ppc034.haifa.ibm.com/audiovideo/technology/tsm/tsm.htm
 *
 */

//Maximum IO file name length, including path 
#define MAX_FILE_NAME_LENGTH  1024

//Default min pitch frequency (or dominant lowest harmonic) in audio 
#define MIN_PITCH_FREQ_HZ     50.0f

//Default half window length in msec 
#define DEFAULT_WIN_SPAN_MSEC 10.0f

//----------------------------------------------------------------------------
/**InitWindow
 *
 * Initialize analysis/synthesis/overlap-add window.
 *
 * The window:
 *
 *   w[n], n = -win_span,...,0,...,+win_span;
 *
 * Should have the following characteristics:
 *
 * 1. w[-win_span] = w[win_span] = 0; (decay to zero)
 * 2. w[0] = 1; (normalized peak at center)
 * 3. w[k] = w[-k] ; k=0,1,...,win_span (symmetry)
 * 4. w[-win_span+k]+w[k] = 1.0 ; k = 0,1,...,win_span (summes up to 1.0);
 *
 * Here we use an odd length, raised cosine window.
 *
 * Arguments:
 *
 *   w - On output will hold the window. Memory will be
 *       allocated internally. 
 *   win_span - The size of the window will be 2*win_span+1
 *
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::InitWindow(float **w, const int win_span)
{
 int win_len = 2*win_span+1;  //Define window size 
 int k;
 float pi = 3.14159265358979f ;

 //Allocate window memory 
 if ( (float *)NULL == (*w = (float *)malloc(win_len*sizeof(float))))
 {
	 fprintf(stderr, "\n\n Can't allocate memory for window buffer !\n\n");
	 exit(1);
 }

 //Calculate window 
 for (k = -win_span ; k<= win_span  ; k++)
 {
	 (*w)[k+win_span] = 0.5f*(1.0f-(float)cos(2.f*pi*(k+win_span)/(win_len-1)));
 }
}

//----------------------------------------------------------------------------
/**EvalCorr
 *
 * This function calculates the cross correlation
 * between two buffers.
 *
 * Input arguments:
 *
 *   x - A pointer to the middle of the first audio buffer
 *   y - A pointer to the middle of the second audio buffer
 *   win_span - The correlation will be done between samples
 *        located in a distance of "win_span" or less on both
 *        sides of the input pointer.
 *
 * Result:
 *
 *   The cross correlation (norm 2) between the two buffers.
 *   The result is only normalized by the energy of second
 *   input buffer (so the return value is NOT limited to the
 *   range -1 to 1 as usual).
 *
 */
float SeqSummaryFastMovingStoryBoardExtractionTool::EvalCorr(float *x, float *y, const int win_span)
{
 float correlation, sum_sqr_y;
 int n;

 correlation = sum_sqr_y = 0.f;

 //Calculate cross correlation between buffers and enery of one buffer 
 for(n= -win_span;n<=win_span;n++)
 {
  correlation+=(*(x+n)) * (*(y+n));
  sum_sqr_y+=  (*(y+n)) * (*(y+n));
 };

 //Normalize result 
 if (sum_sqr_y == 0.0f)
 {
  correlation = 0.0;
 } else
 {
  correlation /= (float)sqrt(sum_sqr_y);
 }

 return(correlation);
}

//----------------------------------------------------------------------------
/**ActivePrompt
 *
 * This function just displays a text prompt
 * to indicate that the TSM is in progress.
 * A percentage (0-100%) will also be displayed.
 * 
 * Arguments:
 *  iTotalCount - The 100% count value
 *  iCount      - The current count (iCount<=iTotalCount)
 *
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::ActivePrompt(long iTotalCount, long iCount)
{
 static int  k=0;
 static char *prompt[] = {
  " .       ",
  " ..      ",
  " ...     ",
  " ....    ",
  " .....   ",
  " ......  ",
  " ....... ",
  " ........"
 };
  
 printf("%3ld%% %s\r",100*iCount/iTotalCount,prompt[k&0x7]);
 k++;
}
  
//----------------------------------------------------------------------------
/**wsola
 *
 * the WSOLA algorithm
 *
 * Input arguments:
 *
 *   input_file_name  - path&filename of input file (.WAV)
 *   output_file_name - path&filename of output file
 *   alfa             - time scale factor
 */
void SeqSummaryFastMovingStoryBoardExtractionTool::wsola(char* input_file_name, char* output_file_name, float alfa)
{
 int n,k;
 float *input_sig,*output_sig;  //Float IO data buffers 
 float *win, *win1;             //Overlap add weight window 
 float win_span_in_msec;        //Half window size in msec 
 float delta_in_msec;           //Half search interval in msec 

 long input_len;   //Total number of samples in input 
 long output_len;  //Total number of samles in output 
 int freq;         //Sampling rate of processed audio (in Hz) 
 int delta;        //Correlation Search Span 
 int optim_k;      //Max Correlation window location 
 int win_span;     //Overlap-add half window size 

 int   iSyStep;    //Synthesis (output) window advance step size (in integer samples) 
 float fAnStep;    //Analysis (input) window advance step size (non integer samples) 
 float fAnWinCen;  //location of current analysis (input) window center (non integer samples) 
 long  iAnWinCen;  //location of current analysis (input) window center (rounded to integer samples) 
 long  iPrAnWinCen;//Location of previously copied window 
 long  iSyWinCen;  //location of current synthesis (ouput) window center (in integer samples) 

 long  iCount,iTotalCount; //Count number of processed windows 

 //use these default values
 win_span_in_msec = DEFAULT_WIN_SPAN_MSEC ;
 delta_in_msec    = 1.0f/(MIN_PITCH_FREQ_HZ)*500.0f ;

 printf(" 1. Reading input WAV file.\n");

 //Parse WAV file and read audio from input WAV file 
 ReadWavFile(input_file_name, &input_sig, &input_len, &freq);

 //Calculate window width and synthesis step size in samples 
 win_span = iSyStep = (int)(0.5f + win_span_in_msec/1000.0f * (float)(freq) );

 //Calculate search interval width in samples 
 delta = (int)(0.5f + delta_in_msec/1000.0f * (float)(freq) );
 
 //Calculate number of output samples 
 output_len = (unsigned long)(0.5f + alfa*input_len);

 //Calculate number of synthesis windows in the input signal 
 iTotalCount = output_len/iSyStep;

 //Allocate and reset floating point buffer for output samples 
 if ( (float *)NULL == (output_sig = (float *)malloc(output_len*sizeof(float))))
 {
   fprintf(stderr, "\n\n Can't allocate memory for output buffer !\n\n") ;
	 exit(1) ;
 }
 memset(output_sig,0,output_len*sizeof(float));

 //Calculate overlap-add window 
 InitWindow(&win1,win_span);

 //Create a pointer to the middle of the window, such that it can be 
 //accessed with negative indeces (the range [-win_span,..,0,..+win_span])
 win = win1 + win_span;

 //Calculate analysis pointed step size in integer samples 
 fAnStep = (float)iSyStep/alfa;

 //Initialize first analysis window center in input signal 
 fAnWinCen = fAnStep;
 iAnWinCen = (int)(0.5f + fAnWinCen); 
 iPrAnWinCen = 0;

 //Initialize first synthesis window center in input signal 
 iSyWinCen = iSyStep;

 //Init TSM by Copy of first half window from input to output 
 for(n=0;n<=win_span;n++)
 {
   output_sig[n]=input_sig[n]*win[n];
 }

 iCount = 0;

 printf(" 2. Performing time scaling.\n");

 //Loop until analysis window center axceed input signal length 
 while ( iAnWinCen + delta + win_span < input_len)
 {
  //Check if the search interval includes the referece window 
  if ( iPrAnWinCen+iSyStep >= iAnWinCen-delta && iPrAnWinCen+iSyStep <= iAnWinCen+delta    ) 
	{
    //No window search is needed, copy reference winodow to output 
    optim_k= iPrAnWinCen + iSyStep - iAnWinCen;
  } else //Do a maximum correlation search to find the best window to copy 
  {
    //Initialize search 
    float max_xcorr = -FLT_MAX; 
    float xcorr;

    optim_k=0;

    //Go over all allowed window offsets and find the best offset 
    for(k= -delta; k<=delta; k++)
		{
      //Special care at the beggining of the file 
			if (iAnWinCen+k>=win_span)
			{
        //Correlate cadidate window with reference window 
				xcorr=EvalCorr( input_sig + iPrAnWinCen+iSyStep,  //reference 
												input_sig + iAnWinCen+k,          //candidate 
												win_span);

        //Update current maxima 
        if (xcorr>max_xcorr) 
				{ 
					max_xcorr=xcorr; optim_k=k; 
				};
			}
    } //of for(k=...
  }   //of else 

  //Copy best window from input to output (using overlap-add method) 
  for(n= -win_span;n<=win_span;n++)
	{
   output_sig[iSyWinCen+n]+=win[n]*input_sig[iAnWinCen+optim_k+n];
  }

  //Save location of window copied from input to output 
  iPrAnWinCen = iAnWinCen + optim_k;

  //Calculate exact locatin of new analysis window center 
  fAnWinCen += fAnStep;
  iAnWinCen = (int)(0.5f+fAnWinCen); 

  //Move synthesis window center ahead 
  iSyWinCen += iSyStep; 

  //Display active prompt during run time 
  iCount++;
  if (0==(iCount&0x3F)) ActivePrompt(iTotalCount,iCount);
 }

 //Free input signal and window buffer 
 free(input_sig);
 free(win1);
 
 printf(" 3. Writing output WAV file.\n");

 //Write output audio as WAV file 
 WriteWavFile(output_file_name,output_sig,output_len,freq);

 //Free output buffer 
 free(output_sig);
 
 printf(" 4. Done.                                 \n\n");
} //of main() 


//----------------------------------------------------------------------------
DoubleArray *SeqSummaryFastMovingStoryBoardExtractionTool::setTransitionMatrix(double dt)
{
  DoubleArray *F = doubleArrayCreate(1, 4, 1, 4);
  double **arr_F;

  doubleArrayInit(F);
  arr_F = F->array;

  ARRAY(arr_F, 1, 1) = 1;
  ARRAY(arr_F, 1, 3) = dt;
  ARRAY(arr_F, 2, 2) = 1;
  ARRAY(arr_F, 2, 4) = dt;
  ARRAY(arr_F, 3, 3) = 1;
  ARRAY(arr_F, 4, 4) = 1;
  
  return(F);
}

//----------------------------------------------------------------------------
DoubleArray *SeqSummaryFastMovingStoryBoardExtractionTool::setProcessNoise(double dt, double accelNoise)
{
  DoubleArray *Q = doubleArrayCreate(1, 4, 1, 4);
  double **arr_Q;
  double scale;

  doubleArrayInit(Q);
  arr_Q = Q->array;

  ARRAY(arr_Q, 1, 1) = (SQUARE(dt) * SQUARE(dt)) / 4;
  ARRAY(arr_Q, 1, 3) = (SQUARE(dt) * dt) / 2;
 
  ARRAY(arr_Q, 2, 2) = (SQUARE(dt) * SQUARE(dt)) / 4;
  ARRAY(arr_Q, 2, 4) = (SQUARE(dt) * dt) / 2;

  ARRAY(arr_Q, 3, 1) = (SQUARE(dt) * dt) / 2; 
  ARRAY(arr_Q, 3, 3) = SQUARE(dt);
  
  ARRAY(arr_Q, 4, 2) = (SQUARE(dt) * dt) / 2;
  ARRAY(arr_Q, 4, 4) = SQUARE(dt);

  scale = SQUARE(accelNoise);
  doubleArrayScale(Q, scale);

  return(Q);
}

//----------------------------------------------------------------------------
DoubleArray *SeqSummaryFastMovingStoryBoardExtractionTool::setMeasureMatrix(void )
{
  DoubleArray *H = doubleArrayCreate(1, 2, 1, 4);
  double **arr_H;

  doubleArrayInit(H);
  arr_H = H->array;

  ARRAY(arr_H, 1, 1) = 1;
  ARRAY(arr_H, 2, 2) = 1;

  return(H);
}

//----------------------------------------------------------------------------
DoubleArray *SeqSummaryFastMovingStoryBoardExtractionTool::setMeasureNoise(double measureNoise)
{
  DoubleArray *R = doubleArrayCreate(1, 2, 1, 2);
  double **arr_R;

  doubleArrayInit(R);

  arr_R = R->array;

  ARRAY(arr_R, 1, 1) = SQUARE(measureNoise);
  ARRAY(arr_R, 2, 2) = SQUARE(measureNoise);

  return(R);
}

//----------------------------------------------------------------------------
int SeqSummaryFastMovingStoryBoardExtractionTool::kalmanOneStep(KalmanState *kalState, DoubleArray *C, DoubleArray *F, DoubleArray *H, DoubleArray *W, DoubleArray *S_1, double distThresh, double dt)
{
  DoubleArray *xhat;  //Predicted state vector  
  DoubleArray *zhat;  //Measurement prediction 
  DoubleArray *z;     //Measurement vector
  DoubleArray *snew;  //New state vector  
  DoubleArray *c;     //Last state vector + measurement vector + feature: (xhat,yhat, dxhat/dt, dyhat/dt,x,y,f) 
  DoubleArray *f;
  DoubleArray *fm;
  DoubleArray *v;     //Innovation
  DoubleArray *vt;    //Transpose innovation
  DoubleArray *work;
  DoubleArray *work1;
  double **arr_snew;
  double **arr_C = C->array;
  double **arr_zhat;
  double **arr_f;
  double **arr_fm;
  double **arr_z;
  double **arr_xhat;
  double **arr_c;
  double x1,y1;
  Element *e;
  Element *g;
  Element *etmp;
  LinkedList *trackList;
  LinkedList *rmLinkedList;
  int count_p = 0;
  int count_c = 0;
  int count_g = 0;
	//int count_l = 0;
  double term = 0;     //terminated tracks 
  double init = 0;     //initiated tracks 
  int j ,i, k, kmin;
  double gateThresh = 1;
  double d1, d2;
  int h = 0;
  
  count_p = linkedListCountElement(kalState->X); //previous track list 
  e = kalState->X->firstElement;
 
  while(e != (Element*) NULL) //traverse track lists  
  {       
    LinkedList *gateList = linkedListCreate();
    c = (DoubleArray *)linkedListGetLast((LinkedList *)e->object);
    arr_c = c->array;
    xhat = doubleArraySubArray(c, 1, 4, 1, 1);
    f = doubleArraySubArray(c, 7, 7, 1, 1);
    x1 = ARRAY(arr_c,5,1);
    y1 = ARRAY(arr_c,6,1);
    arr_f = f->array;

    //State prediction 
    work1 = doubleArrayMultiply(xhat, F);
    doubleArrayDestroy(xhat);
    xhat = work1;

    //Meaurement prediction 
    zhat = doubleArrayMultiply(xhat, H);
    arr_zhat = zhat->array;

    //----- Data Association -----  
    for (j = C->loLimY; j <= C->upLimY; j++) //Select measurements which are situated within the gate 
		{
		  if (ARRAY(arr_C,1,j) != -1.0)  //This condition allows us to assign only once a measurement to a track 
	    {
	      DoubleArray *d;
	      double D = 0;
	      z = doubleArraySubArray(C, 5, 6, j, j);
	      doubleArrayOrigin(z, 1, 1);
	      arr_z = z->array;
	      
	      v = doubleArraySub(z, zhat);
	      vt = doubleArrayTranspose(v);
	      
	      work = doubleArrayMultiply(v, S_1);
	      d = doubleArrayMultiply(work, vt);
	      D = d->array[1][1];
 
	      if (D < gateThresh)
				{
					int *p = (int*) malloc(sizeof(int));
					*p = j;
					linkedListAddLast(gateList, p);
				}
	      
	      doubleArrayDestroy(z);
	      doubleArrayDestroy(work);
	      doubleArrayDestroy(v);
	      doubleArrayDestroy(vt);
	      doubleArrayDestroy(d);
	    }
		} //end for 

    count_g = linkedListCountElement(gateList);
    g = gateList->firstElement;

		if (count_g > 0) //Select the nearest measurement in the feature space  
	  {
	    double dfmin = 100000;
	    while ( g != (Element *)NULL)
	    {
				double df = 0;
				k = *(int *)(g->object);
				fm = doubleArraySubArray(C, 7, 7, k, k);
				arr_fm = fm->array;
				df = fabs((ARRAY(arr_f,7,1) - ARRAY(arr_fm,7,k)) / ARRAY(arr_f,7,1));
					
				if (df < dfmin) 
			  {
					dfmin = df;
					kmin = k;
				} //Determine the minimum  
		
				doubleArrayDestroy(fm);
				g = g->nextElement;	
      } //end while 

      if (dfmin < distThresh) //Track list updating 
			{ 
				int count_l = linkedListCountElement((LinkedList*)e->object);
				snew = doubleArrayCreate(1, 7, 1, 1);
				arr_snew = snew->array;
		  
			  if (count_l == 1) //Track list initiation  
		    {
		      ARRAY(arr_snew,1,1) = ARRAY(arr_C,5,kmin);
		      ARRAY(arr_snew,2,1) = ARRAY(arr_C,6,kmin);
		      ARRAY(arr_snew,3,1) = (ARRAY(arr_C,5,kmin) - x1)/ dt;
		      ARRAY(arr_snew,4,1) = (ARRAY(arr_C,6,kmin) - y1)/ dt;
		      ARRAY(arr_snew,5,1) = ARRAY(arr_C,5,kmin);
		      ARRAY(arr_snew,6,1) = ARRAY(arr_C,6,kmin);

					for (i = 7; i <= snew->upLimX; i++)
					{
					  ARRAY(arr_snew,i,1) =  ARRAY(arr_C,i,kmin);
					}
		      
		      linkedListAddLast((LinkedList*)e->object, snew);
		    } else   //Track list maintaining 
		    {
		      z = doubleArraySubArray(C, 5, 6, kmin, kmin);
		      doubleArrayOrigin(z, 1, 1);
		      v = doubleArraySub(z, zhat);
    
		      work = doubleArrayMultiply(v,W);
		      work1 = doubleArrayAdd(work,xhat);
		      doubleArrayDestroy(xhat);
		      xhat = work1;

		      arr_xhat = xhat->array;
		      
		      for (i = snew->loLimX; i <= snew->upLimX; i++)
					{
					  if (i <= xhat->upLimX)
					  {
						  ARRAY(arr_snew,i,1) = ARRAY(arr_xhat,i,1);
						} else
						{
							ARRAY(arr_snew,i,1) = ARRAY(arr_C,i,kmin);
						}
					}
		      
		      doubleArrayDestroy((DoubleArray *)linkedListRemoveLast((LinkedList *)(e->object)));
		      linkedListAddLast((LinkedList*)e->object, snew);
		      doubleArrayDestroy(z); 
		      doubleArrayDestroy(work);
		      doubleArrayDestroy(v);
		    }
		  
				ARRAY(arr_C,1,kmin) = -1.0;  //Mark this line: already assigned  
				e = e->nextElement;
			} //end (if dmin < distThresh) 
		  else //Track list termination 
			{
			  if (e->previousElement == (Element *)NULL)
		    {  
		      rmLinkedList = (LinkedList*)linkedListRemoveFirst((LinkedList*)(kalState->X));
		      linkedListDestroy(rmLinkedList,	&SSZdeleteDoubleArrayElement);
		      e = kalState->X->firstElement;
		      term ++;
		    } else
		    {
			    if (e->nextElement == (Element *)NULL)
					{
						rmLinkedList = (LinkedList*)linkedListRemoveLast((LinkedList*)kalState->X);
						linkedListDestroy(rmLinkedList, &SSZdeleteDoubleArrayElement);
						e = (Element*) NULL;
						term ++;
					} else
					{ 
						etmp = e->nextElement;
						e->previousElement->nextElement = e->nextElement;
						e->nextElement->previousElement = e->previousElement;
						rmLinkedList = (LinkedList*)e->object;
						linkedListDestroy(rmLinkedList, &SSZdeleteDoubleArrayElement);
						elementDestroy(e);
						e = etmp;
						term ++;
					}
		    }
			} //end else (if dmin < distThresh) 
		}	//end if Count > 0 
		else
		{
			if (e->previousElement == (Element *)NULL)
			{
				rmLinkedList = (LinkedList*)linkedListRemoveFirst((LinkedList*)kalState->X);
				e = kalState->X->firstElement;
				linkedListDestroy(rmLinkedList,&SSZdeleteDoubleArrayElement);
				term ++;
			} else
			{
				if (e->nextElement == (Element *)NULL)
				{
					rmLinkedList = (LinkedList*)linkedListRemoveLast((LinkedList*)kalState->X);
					linkedListDestroy(rmLinkedList,&SSZdeleteDoubleArrayElement);
					e = (Element*) NULL;
					term ++;
				} else
				{
					etmp = e->nextElement;
					e->previousElement->nextElement = e->nextElement;
					e->nextElement->previousElement = e->previousElement;
					rmLinkedList = (LinkedList*)e->object;
					linkedListDestroy(rmLinkedList,&SSZdeleteDoubleArrayElement);
					elementDestroy(e);
					e = etmp;
					term ++;
				}
			}
		} //end else (if Count > 0) 

		doubleArrayDestroy(xhat);
		doubleArrayDestroy(zhat);
		doubleArrayDestroy(f);
		linkedListDestroy(gateList,&SSZfreeListIntegerObject);
		h++;

  } //end while 

  count_c = linkedListCountElement(kalState->X);

  for (i = C->loLimY; i <= C->upLimY; i++)
  {
    if (ARRAY(arr_C,1,i) != -1.0)
		{
		  snew = doubleArrayCreate(1, 7, 1, 1);
		  arr_snew = snew->array;
		  
		  for (j = C->loLimX; j <= C->upLimX; j++)
	    {
	      ARRAY(arr_snew,j,1) = ARRAY(arr_C,j,i);
	    }
		 
		  init++;
		  trackList = linkedListCreate();
		  linkedListAddLast(trackList, snew);
		  linkedListAddLast(kalState->X, trackList);
		}
  }
     
  count_c = linkedListCountElement(kalState->X); //current track list number 
     
	d1 = init/count_c;
	d2 = term/count_p;

	kalState->Init = d1;
	kalState->Term = d2;
	kalState->activity = (d1 > d2) ? d1:d2; //max. 
     
  //linkedListTraverse(kalState->X,action,NULL);
  return (0);
}

//----------------------------------------------------------------------------
const UUID& SeqSummaryFastMovingStoryBoardExtractionTool::GetObjectID(void)
{
	return myID; 
}

//----------------------------------------------------------------------------
const char *SeqSummaryFastMovingStoryBoardExtractionTool::GetName(void)
{
	return myName;
}

//----------------------------------------------------------------------------
SeqSummaryFastMovingStoryBoardExtractionInterfaceABC *SeqSummaryFastMovingStoryBoardExtractionTool::GetInterface(void)
{
	return &m_Interface; 
}

