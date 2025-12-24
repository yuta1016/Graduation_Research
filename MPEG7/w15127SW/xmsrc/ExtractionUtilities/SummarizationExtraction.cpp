//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// Yousri Abdeljaoued (EPFL Switzerland)
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
// Applicable File Name:  SummarizationExtraction.cpp
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifndef _VISUALC_
#include <unistd.h>
#include <netinet/in.h>
#else
#include <io.h>
#endif

#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
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

extern "C" void SZfreeListIntegerObject(void *ListObject,void *unused);
extern "C" void SZdeleteDoubleArrayElement(void *object, void *par);
//=============================================================================

void SZdeleteDoubleArrayElement(void *object, void *par)
{
  doubleArrayDestroy((DoubleArray *)object);
}

//----------------------------------------------------------------------------
void SZfreeListIntegerObject(void *ListObject,void * unused)
{
  if (ListObject) free(ListObject);
}

//=============================================================================

using namespace XM;


const UUID SummarizationExtractionInterface::myID = UUID("");
const char *SummarizationExtractionInterface::myName = "SummarizationExtractionInterface";

const UUID SummarizationExtractionTool::myID = UUID("");
const char *SummarizationExtractionTool::myName = "SummarizationExtraction";

const UUID SummarizationExtractionInterfaceABC::myID = UUID();

//=============================================================================
SummarizationExtractionInterfaceABC::SummarizationExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
SummarizationExtractionInterface::SummarizationExtractionInterface(SummarizationExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
SummarizationExtractionInterface::~SummarizationExtractionInterface()
{
}

//----------------------------------------------------------------------------
void SummarizationExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int SummarizationExtractionInterface::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
int SummarizationExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionInterface::PostExtracting(void)
{
  return m_ExtractionTool->PostExtracting();
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionInterface::NewSequence(void)

{
  return m_ExtractionTool->NewSequence();
}

//----------------------------------------------------------------------------
const UUID& SummarizationExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationExtractionInterface::GetName(void)
{ return myName; }


//=============================================================================
SummarizationExtractionTool::SummarizationExtractionTool():
m_Interface(this),
m_distthresh(0.25),
m_nnfraction(30),
m_nn(1),
m_frameduration((float)m_nn/(float)m_nnfraction),
m_accelnoise(1500.0),
m_alpha(0.01),
m_measurenoise(10.0),
m_stddeviation(0.4),
m_writeall(0),
m_DescriptorInterface(0),
m_SequentialSummaryInterface(0),
m_Media(0),
m_Vop(0),
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
SummarizationExtractionTool::
SummarizationExtractionTool(GenericDSInterfaceABC *theParent):
m_Interface(this),
m_distthresh(0.25),
m_nnfraction(30),
m_nn(1),
m_frameduration((float)m_nn/(float)m_nnfraction),
m_accelnoise(1500.0),
m_alpha(0.01),
m_measurenoise(10.0),
m_stddeviation(0.4),
m_writeall(0),
m_DescriptorInterface(0),
m_SequentialSummaryInterface(0),
m_Media(0),
m_Vop(0),
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
SummarizationExtractionTool::~SummarizationExtractionTool()
{
        if (!m_Vop) freevop(m_Vop);
	if (F) doubleArrayDestroy(F);
	if (Q) doubleArrayDestroy(Q);
	if (H)doubleArrayDestroy(H);
	if (R) doubleArrayDestroy(R);
	if (kalState.P) doubleArrayDestroy(kalState.P);
}

//----------------------------------------------------------------------------
bool SummarizationExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool SummarizationExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int SummarizationExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
{
  if (!media) return -1;

  m_Media=media;

  if (!m_Vop || (media->GetImage()->width != m_Vop->width) ||
      (media->GetImage()->height != m_Vop->height)) {
    if (m_Vop) freevop(m_Vop);
    m_Vop=initvop(media->GetImage()->width,media->GetImage()->height,_444_,
		    UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE,UCHAR_TYPE);
    if (!m_Vop) return -1;
  }

  /* convert to apropriate color space*/
  convertRGB_To_YUV(m_Vop,media->GetImage());

#ifdef XYUVDISP
  xyuv_show_debug(FPWIN,(unsigned short *)m_Vop->y_chan->data->s,
		  (unsigned short *)m_Vop->u_chan->data->s,
		  (unsigned short *)m_Vop->v_chan->data->s,
		  datasize(m_Vop->y_chan)&1,
		  m_Vop->y_chan->x,m_Vop->y_chan->y,
		  m_Vop->u_chan->x,m_Vop->u_chan->y,
		  "YUV-Image",0,1.0,0,0,0,0,0,0);
#endif

  return 0;
}

//----------------------------------------------------------------------------
int SummarizationExtractionTool::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionTool::InitExtracting(void)

{
#define MAXFPFILENAMELEN 256
	char filename[MAXFPFILENAMELEN];
#undef MAXFPFILENAMELEN

        if (!m_Vop) return (unsigned long)-1;
	if (!m_DescriptorInterface)  return (unsigned long)-1;


        /* read parameters from parameter file*/
        m_distthresh=atof(getpara("distance_threshold","0.25"));
        m_nnfraction=atoi(getpara("nnfraction","30"));
        m_nn=atoi(getpara("nn","1"));
        m_frameduration=(float)m_nn/(float)m_nnfraction;
        m_accelnoise=atof(getpara("acceleration_noise","1500.0"));
        m_alpha=atof(getpara("alpha","0.01"));
        m_measurenoise=atof(getpara("measurement_noise","10.0"));
        m_stddeviation=atof(getpara("standard_deviation","0.4"));
	m_writeall=atoi(getpara("WriteAllFramesToDDL","0"));

	/* for scene cut and key frame detection*/
	m_FirstFrame=1;
	m_SecondFrame=0;
	Q = setProcessNoise(m_frameduration, m_accelnoise);
	R = setMeasureNoise(m_measurenoise);


	/* create basic structure of description*/
	NewSequence();

	/* compute directory name from media filename*/
	m_KLTFeaturePoints.SetMediaFilename(m_Media->GetVideoFileName());

	return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionTool::StartExtracting(void)

{
        int listsize;
	TKLTFeature *l_finalfeaturelist;
	MomImageData mapval;
	DoubleArray *C;    /* feature poit matrix*/
	DoubleArray *CT;
	DoubleArray *FT;   /* transposed transition matrix*/
	DoubleArray *HT;   /* transposed measure matrix*/
	DoubleArray *S;    /* covariance of innovation*/
	DoubleArray *S_1;  /* inverted S*/
	DoubleArray *W;
	DoubleArray *WT;
	DoubleArray *work,*work1; /* tmporary matrices*/
	unsigned long returnval=0;

	/* check for descriptor*/
        if (!m_SequentialSummaryInterface) return (unsigned long)-1;

// 	if (strcmp(m_SequentialSummaryInterface->GetDSName(),
// 		   "SequentialSummary") != 0) 
#ifdef USEXML
	if (m_SequentialSummaryInterface->GetDSName()!= "SequentialSummary") {
	  return (unsigned long)-1;
	}
#endif

	/* check for media*/
	if (!m_Vop) return (unsigned long)-1;

	/* perform extraction of feature from media
	1. call KLTFeaturePoints GetKLTFeaturePoints
	2. perform scene cut and key frame detection*/

	/* 1 */

	/* compute feature points*/
	l_finalfeaturelist=
	 m_KLTFeaturePoints.GetKLTFeaturePoints(m_Vop,&listsize);

	/* 2 */

	if (!listsize) {
	  fprintf(stderr,"Warning: Empty feature point list\n");
	  return 0;
	}

#ifdef VERBOSE
	fprintf(stderr,"k");
	fflush(stderr);
#endif

	/* write feature points to matrix C*/
	C=doubleArrayCreate(1, listsize, 1, FEATURENBR);
	{
	  int i;
	  for (i=0; i<listsize; i++) {
	    ARRAY(C->array,i+1,1) = (double) l_finalfeaturelist[i].x; 
	    ARRAY(C->array,i+1,2) = (double) l_finalfeaturelist[i].y; 
	    ARRAY(C->array,i+1,3) = (double) 0.0; 
	    ARRAY(C->array,i+1,4) = (double) 0.0; 
	    ARRAY(C->array,i+1,5) = (double) l_finalfeaturelist[i].x; 
	    ARRAY(C->array,i+1,6) = (double) l_finalfeaturelist[i].y; 
	    ARRAY(C->array,i+1,7) = (double) l_finalfeaturelist[i].val; 
	  }
	}
	CT = doubleArrayTranspose(C);
	doubleArrayDestroy(C);

	if (m_FirstFrame) {

	  m_FirstFrame = 0;
	  m_SecondFrame = 1;  // check if this value can be
	                      // treated equally to range
	  m_stdDeviation =  m_stddeviation;
	  m_Range =0;
 
	  if (F) doubleArrayTranspose(F);
	  F = setTransitionMatrix(m_frameduration);
//	  Q = setProcessNoise(FRAMEDURATION, ACCELNOISE); = const
	  kalState.P = Q;
	  if (H) doubleArrayTranspose(H);
	  H = setMeasureMatrix();
//	  R = setMeasureNoise(MEASURENOISE); = const
	  kalState.X = linkedListCreate();
	
	  {
	    int i;
	    LinkedList *trackList;
	    for (i = CT->loLimY; i <= CT->upLimY; i++) {
	      trackList = linkedListCreate();
	      work = doubleArraySubArray(CT, 1, 7, i, i);
	      doubleArrayOrigin(work,1,1);
	      linkedListAddLast(trackList, work); /* Initiate a track list */
	      linkedListAddLast(kalState.X, trackList);
	      /* Create a list of track lists */
	    }
	  }
	  doubleArrayDestroy(CT);
	  return 1;
	}


	/* not first frame*/
	kalState.Init = 0;
	kalState.Term = 0;
	kalState.activity = 0;

	/*state prediction cov. */
	FT = doubleArrayTranspose(F);
	work = doubleArrayMultiply(FT,kalState.P);
	work1 = doubleArrayMultiply(work,F);
	kalState.P = doubleArrayAdd(work1, Q);
	doubleArrayDestroy(FT);
	doubleArrayDestroy(work);
	doubleArrayDestroy(work1);
      
	/* Covariance of Innovation */
	HT = doubleArrayTranspose(H);
	work = doubleArrayMultiply(HT, kalState.P);
	work1 = doubleArrayMultiply(work,H);
	S = doubleArrayAdd(R, work1);

	doubleArrayDestroy(work);
	doubleArrayDestroy(work1);
      
	/* Gain matrix */
	doubleArrayOrigin(S, 0, 0);
	S_1 = doubleArrayInverse(S);
	doubleArrayOrigin(S_1, 1, 1);
	doubleArrayOrigin(S, 1, 1);
	work = doubleArrayMultiply(S_1, HT);
	doubleArrayDestroy(HT);
	W = doubleArrayMultiply(work, kalState.P);
	doubleArrayDestroy(work);

	/* Update of state Covariance */
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
     
	/*-----*/
      
	kalmanOneStep(&kalState, CT, F, H, W, S_1,
		      m_distthresh, m_frameduration);
	doubleArrayDestroy(S_1);
	doubleArrayDestroy(W);

	/* Shot boundarie detection and key frame extraction */
	if (m_SecondFrame) {

	  m_SecondFrame = 0;
	  returnval=0;

	  /*-- Gaussian process initialisation --*/
	  /* Initialisation of the stationary state (change rate) */ 
	  m_Mean = kalState.activity;
//	  m_stdDeviation =  m_stdDeviation;
//	  m_stdDeviation = kalState.activity;

	  /* has to be modified for XML usage*/
	  //	    m_DescriptorInterface->AddKeyFrame(m_FrameNumber);

	  /* create color histogram for frame*/
	  //	    ExtractColorHistogram();
	}

#ifdef VERBOSE
	fprintf(stderr,"acitvity : %f (%f, %f, %f)",kalState.activity,
		m_Mean, m_stdDeviation, m_Mean + (m_stdDeviation*2));
#endif

	/* test scene cut */
	if ((kalState.activity > (m_Mean + (m_stdDeviation*2)))) {


	  /*-- Detection of shot boundaries --*/
	  /* has to be modified for XML usage*/
	  if (m_Range && (m_Range < 16)) { /* ignore gradual change*/
#ifdef VERBOSE
	    fprintf(stderr,"\tRange: %d\n",m_Range);
#endif
	    m_Range++;
// 	    doubleArrayDestroy(CT);
	    returnval=0;
	  }
	  else {
#ifdef VERBOSE
	    fprintf(stderr,"\tScene Cut\n");
#endif
	    /* The max. duration of a  gradual change is 16 frames */ 
	    m_SecondFrame=1;
	    m_Range=0;

	    /* add scene cut frame*/
// 	    doubleArrayDestroy(CT);
	    returnval=1;
	  }
	}
	else {
#ifdef VERBOSE
	fprintf(stderr,"\tNormal\n");
#endif
	  /*-- Gaussian process update --*/
	  m_Mean = (1 - m_alpha) * m_Mean + m_alpha * kalState.activity;
	  m_stdDeviation = sqrt((1-m_alpha) * SQUARE(m_stdDeviation) +
				m_alpha * SQUARE(m_Mean - kalState.activity));
	  returnval=0;
	}

#ifdef USEXML
	if (returnval || m_writeall) {

	  /* write activity for a stationary or weak non-stationary state */
	  GenericDS 
	    l_FrameProperty=m_SequentialSummaryInterface->
	    CreateChild("FrameProperty");
	  GenericDSInterfaceABC
	    *l_FramePropertyInterface=l_FrameProperty.GetInterface();

	  GenericDS l_MediaTime=l_FramePropertyInterface->
	    CreateChild("MediaTime");
	  GenericDSInterfaceABC 
	    *l_MediaTimeInterface=l_MediaTime.GetInterface();

	  GenericDS l_RelIncTime=l_MediaTimeInterface->
	    CreateChild("RelIncTime");
	  GenericDSInterfaceABC
	    *l_RelIncTimeInterface=l_RelIncTime.GetInterface();

	  GenericDS l_FrameChangeValue=l_FramePropertyInterface->
	    CreateChild("FrameChangeValue");
	  GenericDSInterfaceABC 
	    *l_FrameChangeValueInterface=
	    l_FrameChangeValue.GetInterface();


	  /*-- Computation of the significance value --*/
	  // kf = m_FrameNumber - (m_Range/2);
	  /* add FrameProperty*/
	  l_FrameChangeValueInterface->SetValue(kalState.activity);

	  /* add time reference*/
	  l_RelIncTimeInterface->SetAttribute("nnFraction",m_nnfraction);
	  l_RelIncTimeInterface->SetAttribute("nn",m_nn);
	  l_RelIncTimeInterface->SetValue((int)m_Media->GetImageTime());
	}
#endif

	doubleArrayDestroy(CT);

	return returnval;
}


//----------------------------------------------------------------------------
DoubleArray *SummarizationExtractionTool::setTransitionMatrix(double dt)
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
DoubleArray *SummarizationExtractionTool::setProcessNoise(double dt, double accelNoise)
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
DoubleArray *SummarizationExtractionTool::setMeasureMatrix(void )
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
DoubleArray *SummarizationExtractionTool::setMeasureNoise(double measureNoise)

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
int SummarizationExtractionTool::kalmanOneStep(
		   KalmanState *kalState,
		   DoubleArray *C,
		   DoubleArray *F,
		   DoubleArray *H,
		   DoubleArray *W,
		   DoubleArray *S_1,
		   double distThresh,
		   double dt)
{
  
  DoubleArray *xhat; /*Predicted state vector */ 
  DoubleArray *zhat;  /* Measurement prediction */
  DoubleArray *z;  /* Measurement vector*/
  DoubleArray *snew; /* New state vector */ 
  DoubleArray *c; /* Last state vector + measurement vector + feature: (xhat,yhat, dxhat/dt, dyhat/dt,x,y,f) */
  DoubleArray *f;
  DoubleArray *fm;
  DoubleArray *v; /*Innovation*/
  DoubleArray *vt; /*Transpose innovation*/
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
//   int count_l = 0;
  double term = 0; /* terminated tracks */
  double init = 0; /*initiated tracks */
  int j ,i, k, kmin;
  double gateThresh = 1;
  double d1, d2;
  int h = 0;

  
  count_p = linkedListCountElement(kalState->X); /* previous track list */
  
  e = kalState->X->firstElement;
  
 
  while(e != (Element*) NULL) /* traverse track lists */ 
    {       
      LinkedList *gateList = linkedListCreate();
      c = (DoubleArray *)linkedListGetLast((LinkedList *)e->object);
      arr_c = c->array;
      xhat = doubleArraySubArray(c, 1, 4, 1, 1);
      f = doubleArraySubArray(c, 7, 7, 1, 1);
      x1 = ARRAY(arr_c,5,1);
      y1 = ARRAY(arr_c,6,1);
      arr_f = f->array;
      /* State prediction */
      work1 = doubleArrayMultiply(xhat, F);
      doubleArrayDestroy(xhat);
      xhat = work1;

      /* Meaurement prediction */
      zhat = doubleArrayMultiply(xhat, H);
      arr_zhat = zhat->array;
  
      /*----- Data Association -----*/  
      for (j = C->loLimY; j <= C->upLimY; j++) /* Select measurements which are situated within the gate */
	{
	  if (ARRAY(arr_C,1,j) != -1.0)  /* This condition allows us to assign only once a measurement to a track */
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
	  
	} /* end for */

      count_g = linkedListCountElement(gateList);
      g = gateList->firstElement;

      {
	if (count_g > 0) /* Select the nearest measurement in the feature space */ 
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
		  }/* Determine the minimum */ 
		
		doubleArrayDestroy(fm);
		g = g->nextElement;	
	      } /* end while */
	    
	    
	    {
	      if (dfmin < distThresh) /* Track list updating */
		{ 
		  int count_l = linkedListCountElement((LinkedList*)e->object);
		  snew = doubleArrayCreate(1, 7, 1, 1);
		  arr_snew = snew->array;
		  
		  if (count_l == 1) /* Track list initiation */ 
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
		      
		    }

		  else   /* Track list maintaining */
		    
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
			    }
			  else
			    {
			      ARRAY(arr_snew,i,1) = ARRAY(arr_C,i,kmin);
			    }
			}
		      
		      doubleArrayDestroy((DoubleArray *)linkedListRemoveLast
					 ((LinkedList *)(e->object)));
		      linkedListAddLast((LinkedList*)e->object, snew);
		      doubleArrayDestroy(z); 
		      doubleArrayDestroy(work);
		      doubleArrayDestroy(v);
		    }
		  
		  ARRAY(arr_C,1,kmin) = -1.0;  /*Mark this line: already assigned */ 
		  e = e->nextElement;
		} /* end (if dmin < distThresh) */
	      
	      else /* Track list termination */
		{
		  if (e->previousElement == (Element *)NULL)
		    {  
		      rmLinkedList = (LinkedList*)linkedListRemoveFirst
			((LinkedList*)(kalState->X));
		      linkedListDestroy(rmLinkedList,
					&SZdeleteDoubleArrayElement);
		      e = kalState->X->firstElement;
		      term ++;
		    }
		  else
		    {

		      if (e->nextElement == (Element *)NULL)
			{
			  rmLinkedList = (LinkedList*)linkedListRemoveLast
			    ((LinkedList*)kalState->X);
			  linkedListDestroy(rmLinkedList,
					    &SZdeleteDoubleArrayElement);
			  e = (Element*) NULL;
			  term ++;
			}
		      else
			
			{ 
			  etmp = e->nextElement;
			  e->previousElement->nextElement = e->nextElement;
			  e->nextElement->previousElement = e->previousElement;
			  rmLinkedList = (LinkedList*)e->object;
			  linkedListDestroy(rmLinkedList,
					    &SZdeleteDoubleArrayElement);
			  elementDestroy(e);
			  e = etmp;
			  term ++;
			}
		      
		    }
		  
		  
		} /* end else (if dmin < distThresh) */
	    }
	  }
	/* end if Count > 0 */
	
	else
	  {
	    
	    if (e->previousElement == (Element *)NULL)
	      {
		rmLinkedList = (LinkedList*)linkedListRemoveFirst
		  ((LinkedList*)kalState->X);
		e = kalState->X->firstElement;
		linkedListDestroy(rmLinkedList,&SZdeleteDoubleArrayElement);
		term ++;
	      }
	    else
	      {

		if (e->nextElement == (Element *)NULL)
		  {
		    rmLinkedList = (LinkedList*)linkedListRemoveLast
		      ((LinkedList*)kalState->X);
		    linkedListDestroy(rmLinkedList,&SZdeleteDoubleArrayElement);
		    e = (Element*) NULL;
		    term ++;
		  }
		else
		  
		  {
		    etmp = e->nextElement;
		    e->previousElement->nextElement = e->nextElement;
		    e->nextElement->previousElement = e->previousElement;
		    rmLinkedList = (LinkedList*)e->object;
		    linkedListDestroy(rmLinkedList,&SZdeleteDoubleArrayElement);
		    elementDestroy(e);
		    e = etmp;
		    term ++;
		  }
		
	      }
	   
	  } /* end else (if Count > 0) */
	
      }
      
      doubleArrayDestroy(xhat);
      doubleArrayDestroy(zhat);
      doubleArrayDestroy(f);
      linkedListDestroy(gateList,&SZfreeListIntegerObject);
      h++;

    } /* end while */
  
 
  count_c = linkedListCountElement(kalState->X);


	{
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
	  
	}
     
     count_c = linkedListCountElement(kalState->X); /* current track list number */
     
     if ( count_c != 0)
       {
	 if (count_p != 0)
	   {
	     d1 = init/count_c;
	     d2 = term/count_p;

	     kalState->Init = d1;
	     kalState->Term = d2;
	     kalState->activity = (d1 > d2) ? d1:d2; /* max. */
	   }
	 else
	   {
	     kalState->activity = 1;
	   }
       }
     else
       {
	 if (count_p == 0)
	   {
	     kalState->activity = 0;
	   }
	 else
	   {
	     kalState->activity = 1;
	   }
       }
 
     /*linkedListTraverse(kalState->X,action,NULL);*/
     return (0);
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionTool::NewSequence(void)

{
	if (!m_DescriptorInterface)  return (unsigned long)-1;

	/* remove SequentialSummary if DescriptorInterface is document*/
	if (m_DescriptorInterface->GetDSName()=="#document") {
	  m_SequentialSummary=m_DescriptorInterface->GetFirstChild();
	  if (!m_SequentialSummary.isNull()) {
	    if (m_SequentialSummary.GetDSName()=="SequentialSummary") {
	      m_DescriptorInterface->RemoveChild(m_SequentialSummary);
	    }
	  }
	}

	/* create sequential description*/
	m_SequentialSummary=m_DescriptorInterface->
	  CreateChild("SequentialSummary");
	m_SequentialSummaryInterface=m_SequentialSummary.GetInterface();

	/* create VideoSegmentLocator if file name is available*/
	if (m_Media->GetVideoFileName()) {
	  GenericDS l_VideoSegmentLocator=m_SequentialSummaryInterface->
	    CreateChild("VideoSegmentLocator");
	  GenericDSInterfaceABC *m_VideoSegmentLocatorInterface=
	    l_VideoSegmentLocator.GetInterface();

	  GenericDS l_MediaURL=m_VideoSegmentLocatorInterface->
	    CreateChild("MediaURL");
	  GenericDSInterfaceABC *l_MediaURLInterface=
	    l_MediaURL.GetInterface();

	  l_MediaURLInterface->SetAttribute("URI",m_Media->GetVideoFileName());
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long SummarizationExtractionTool::PostExtracting(void)
{
  return 0;
}

//----------------------------------------------------------------------------
const UUID& SummarizationExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *SummarizationExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
SummarizationExtractionInterfaceABC *SummarizationExtractionTool::GetInterface(void)
{ return &m_Interface; }

