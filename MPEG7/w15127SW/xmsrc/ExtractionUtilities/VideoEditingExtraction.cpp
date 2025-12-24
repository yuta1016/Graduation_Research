//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Rosa Ruiloba (LIP-6, Paris, France)
// Philippe Joly (LIP-6, Paris, France)
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
// Applicable File Name:  VideoEditingExtraction.cpp
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
#include "Descriptors/Descriptors.h"                     /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"              /* Naming Convention */
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

#define M_SUB 1
#define M_SUB_ABS 2

#define FEATURENBR 7
#define VERBOSE 1 

extern "C" void VEfreeListIntegerObject(void *ListObject,void *unused);
extern "C" void VEdeleteDoubleArrayElement(void *object, void *par);
//=============================================================================

void VEdeleteDoubleArrayElement(void *object, void *par)
{
  doubleArrayDestroy((DoubleArray *)object);
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void VEfreeListIntegerObject(void *ListObject,void * unused)
{
  if (ListObject) free(ListObject);
}

//=============================================================================
	

using namespace XM;


const UUID VideoEditingExtractionInterface::myID = UUID("");
const char *VideoEditingExtractionInterface::myName = "VideoEditingExtractionInterface";

const UUID VideoEditingExtractionTool::myID = UUID("");
const char *VideoEditingExtractionTool::myName = "VideoEditingExtraction";

const UUID VideoEditingExtractionInterfaceABC::myID = UUID();

//=============================================================================
VideoEditingExtractionInterfaceABC::VideoEditingExtractionInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
VideoEditingExtractionInterface::VideoEditingExtractionInterface(VideoEditingExtractionTool* aTool):
m_ExtractionTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
VideoEditingExtractionInterface::~VideoEditingExtractionInterface()
{
}

//----------------------------------------------------------------------------
int VideoEditingExtractionInterface::
SetSourceMedia(MultiMediaInterfaceABC *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------

void VideoEditingExtractionInterface::destroy(void)
{ delete m_ExtractionTool; }

//----------------------------------------------------------------------------
int VideoEditingExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(ParentDSInterface);
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionInterface::InitExtracting(void)

{
  return m_ExtractionTool->InitExtracting();
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionInterface::NewSequence(void)

{
  return m_ExtractionTool->NewSequence();
}

//----------------------------------------------------------------------------
const UUID& VideoEditingExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoEditingExtractionInterface::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionInterface::PostExtracting(void)

{
  return m_ExtractionTool->PostExtracting();
}

//=============================================================================
VideoEditingExtractionTool::VideoEditingExtractionTool():
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
VideoEditingInterfaceGen(0),
MainSegmentDecompositionInterfaceGen(0),
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
VideoEditingExtractionTool::
VideoEditingExtractionTool(GenericDSInterfaceABC *theParent):
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
VideoEditingInterfaceGen(0),
MainSegmentDecompositionInterfaceGen(0),
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
VideoEditingExtractionTool::~VideoEditingExtractionTool()
{
        if (!m_Vop) freevop(m_Vop);

	if (F) doubleArrayDestroy(F);
	if (Q) doubleArrayDestroy(Q);
	if (H)doubleArrayDestroy(H);
	if (R) doubleArrayDestroy(R);
	if (kalState.P) doubleArrayDestroy(kalState.P);
	/*FADE detection*/
	if(last_lum_image) free(last_lum_image);	
	if(signe_variation) free(signe_variation);	
	if(milImageResize) free(milImageResize);	
	if(buffer1) free(buffer1);	
}

//----------------------------------------------------------------------------
bool VideoEditingExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool VideoEditingExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int VideoEditingExtractionTool::SetSourceMedia(MultiMediaInterfaceABC *media)
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

  return 0;
}

//----------------------------------------------------------------------------
int VideoEditingExtractionTool::
SetDescriptorInterface(GenericDSInterfaceABC
		       *ParentDSInterface)

{
  if (!ParentDSInterface) return -1;

  m_DescriptorInterface=ParentDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionTool::InitExtracting(void)

{
//#define MAXFPFILENAMELEN 256
//	char filename[MAXFPFILENAMELEN];
	

	/*To write description*/
	idT=0;idEVS=0;


	/*Transition classification: Fade*/
	
	transition_detected=0;
	frame_transition=0;
	startEVS=startT=(long)0;
	durationEVS=durationT=(long)0;

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

	/*read parameter for transition classification: Fade */
	waitFadeDetection=atoi(getpara("waitFadeDetection","15"));	
	
	/* for video processing part = extraction of feature points*/
	/* allocate frame buffers*/

//#undef MAXFPFILENAMELEN

	/* for scene cut and key frame detection*/
	m_FirstFrame=1;
	m_SecondFrame=0;
	Q = setProcessNoise(m_frameduration, m_accelnoise);
	R = setMeasureNoise(m_measurenoise);


	/*Fade Extracting*/
	InitExtractingFade();
	/* create basic structure of description*/
	NewSequence();

	/* compute directory name from media filename*/
	m_KLTFeaturePoints.SetMediaFilename(m_Media->GetVideoFileName());

	return 0;
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionTool::InitExtractingFade(void)

{
	int x;
	
	/*Init detection values*/
	fade_beginning_detected=0;
	frame_fade_beginning=(long)0;
	frame_fade_ending=(long)0;

	/*Init correlation*/	
	correlation = 0; 
        
	/* read parameters from parameter file*/
	fade_max_threshold=atoi(getpara("fade_max_threshold","29"));
	fade_min_threshold=atoi(getpara("fade_min_threshold","57"));
	width_zoom_factor=atoi(getpara("width_zoom_factor","29"));
	height_zoom_factor=atoi(getpara("height_zoom_factor","57"));

	   col=m_Vop->width;
	   line=m_Vop->height;
	   colresize=(int)(col/width_zoom_factor);
	   lineresize=(int)(line/height_zoom_factor);
	   nbpix=colresize*lineresize; /*Nombre de pixels de l'image reduite*/
	   if(last_lum_image) free(last_lum_image);	
	   last_lum_image=(unsigned char *)calloc(nbpix,sizeof(unsigned char));
	   
	   if(signe_variation) free(signe_variation);	
	   signe_variation=(unsigned char *)calloc(nbpix,sizeof(unsigned char));
	   for (x=0;x<nbpix;x++){ 
	     last_lum_image[x]=(unsigned char)0;
	     signe_variation[x]=(unsigned char)2; 
	  }

	  if(milImageResize) free(milImageResize);	
	  milImageResize=(unsigned char *)calloc(nbpix,sizeof(unsigned char));

	  if(buffer1) free(buffer1);	
	  buffer1=(int *)malloc(nbpix*sizeof(int));
	  
	  /* Valeur des seuils de detection*/ 
	  thresfade1=nbpix/fade_max_threshold; 
	  thresfade2=nbpix/fade_min_threshold;	
	  
	  
	return 0;
}


//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionTool::StartExtractingFade(void){
		int x;
		fade_detected=0;

		/*Variation de la luminance*/  //sth not everyone can read this
		mimResize(m_Vop,milImageResize,width_zoom_factor,height_zoom_factor);
		mimArithSigne(milImageResize, last_lum_image, buffer1 ,nbpix, M_SUB); 
		mbufCopy(milImageResize,last_lum_image,nbpix);
		
		/* Memorisation du signe et comptage des valeurs a 0 ou a 255*/ 
		signe = (unsigned char)2; 
	       	correlation=0;
		for (x=0;x<nbpix;x++)  
		  { 
		    /* variation negative -> Signe = 0 
		     * variation positive -> Signe = 1 
		     * variation nulle    -> Signe = 2 */
		   
		    if ((*(buffer1+x))>0) signe = (unsigned char)1; 
		    else if ((*(buffer1+x))<0) signe = (unsigned char)0; 
		    val8=(*(signe_variation+x));
		    
		    /* on memorise la variation sur un octet si elle est non-nulle*/  
		    if (signe != (unsigned char)2) signe = (unsigned char)(signe + (unsigned char)(val8<<1)); 
		    
		    /* on inverse l'ancienne variation*/ 
		    
		    else signe = (unsigned char)((((val8<<7)>>7)+(unsigned char)1)%(unsigned char)2 + (val8<<1)); 
		    
		    /* estimation de la correlation de la variation de signe */	
		    if ((signe == (unsigned char)255)||(signe == (unsigned char)0)) correlation++; 
		    /* memorisation de la nouvelle valeur*/ 
		    (*(signe_variation+x))=signe; 
		     signe = (unsigned char)2; 
		    
		  }
		
	    
	    /*Detection du fondu*/ 
	    if (correlation > thresfade1)  
	      { 
		fade_beginning_detected = 1; 
#ifdef VERBOSE 
		printf("Starting Fade\n");

#endif /*VERBOSE*/ 
		frame_fade_beginning=m_Media->GetImageTime();

			
	      } 
	    else if ((fade_beginning_detected == 1) && (correlation < thresfade2)) 
	      { 
		
		    fade_detected=1;

#ifdef VERBOSE 
		    printf("Ending Fade fr:%d\n",m_Media->GetImageTime());

#endif /*VERBOSE*/ 
		    fade_beginning_detected = 0;
		    frame_fade_ending=m_Media->GetImageTime();
	      }

	 
	return (unsigned long)fade_detected;
	    
}
//----------------------------------------------------------------------------

unsigned long VideoEditingExtractionTool::StartExtracting(void)

{
        int listsize;
	TKLTFeature *l_finalfeaturelist;
//	MomImageData mapval;
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
        if (!VideoEditingInterfaceGen) return (unsigned long)-1;
// 	if (strcmp(VideoEditingInterfaceGen->GetDSName(),
// 		   "SequentialSummary") != 0) 
#ifdef USEXML
	if (VideoEditingInterfaceGen->GetDSName()!= "VideoEditing") {
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
	  if (m_Range && (m_Range < 16)) { 
	  /* ignore gradual change*/

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

	/*Updating transition detection*/

	char type[10];

	if(returnval){
		
		if(!transition_detected){
			/*No other transition detected since wating for fade datection*/
			/*So, wait for fade detection*/
			transition_detected=(int)returnval;
			frame_transition=m_Media->GetImageTime();
			}
		else{
			/*Already waiting for fade because a tansition was detected*/
			/*So, because a new transition was detected, the previous one was a cut and not a fade */
			/*Stop the old fade detection*/
			/*and  wait for another fade*/ 

			/*write cut*/
			durationEVS=(frame_transition -1)-startEVS;
			CreateEditedVideoSegment(MainSegmentDecompositionInterfaceGen,startEVS,durationEVS,idEVS);
			startT=(frame_transition-1);
			durationT=1;
			strcpy(type,"cut");
			CreateTransition(MainSegmentDecompositionInterfaceGen,startT,durationT,type,idT);
			/*the next EVS starts at:*/
			startEVS=(frame_transition);
			idT++;
			idEVS++;
			
			/*New transition detected*/
			transition_detected=(int)returnval;
			frame_transition=m_Media->GetImageTime();
			fade_beginning_detected=0;
		}
	}
#ifdef VERBOSE
	printf("transition detected:%ld\n",transition_detected);

#endif /*VERBOSE*/
	 StartExtractingFade();
	
#ifdef USEXML

	if(transition_detected){

		if(fade_detected && ((frame_fade_ending-frame_transition)<=waitFadeDetection )){

		/*writeFade*/
				
		durationEVS=(frame_fade_beginning-5)-startEVS;
		CreateEditedVideoSegment(MainSegmentDecompositionInterfaceGen,startEVS,durationEVS,idEVS);
		startT=(frame_fade_beginning-4);
		durationT=frame_fade_ending-startT;
		strcpy(type,"grad");
		CreateTransition(MainSegmentDecompositionInterfaceGen,startT,durationT,type,idT);
		/*the next EVS starts at:*/
		startEVS=(frame_fade_ending+1);
		idT++;
		idEVS++;
		transition_detected=0;
		fade_beginning_detected=0;

		}

		if(((m_Media->GetImageTime())-frame_transition)>waitFadeDetection){

		/*write cut*/

		durationEVS=(frame_transition -1)-startEVS;
		CreateEditedVideoSegment(MainSegmentDecompositionInterfaceGen,startEVS,durationEVS,idEVS);
		startT=(frame_transition-1);
		durationT=1;
		strcpy(type,"cut");
		CreateTransition(MainSegmentDecompositionInterfaceGen,startT,durationT,type,idT);
		/*the next EVS starts at:*/
		startEVS=(frame_transition);
		idT++;
		idEVS++;

		transition_detected=0;
		fade_beginning_detected=0;

		}
	}
#endif
	doubleArrayDestroy(CT);
	
	return returnval;
}

//----------------------------------------------------------------------------
unsigned long VideoEditingExtractionTool::PostExtracting(void)

{

	char type[10];
	/*write cut*/
	if(transition_detected){
		durationEVS=(frame_transition -1)-startEVS;
		CreateEditedVideoSegment(MainSegmentDecompositionInterfaceGen,startEVS,durationEVS,idEVS);
		startT=(frame_transition-1);
		durationT=1;
		strcpy(type,"cut");
		CreateTransition(MainSegmentDecompositionInterfaceGen,startT,durationT,type,idT);
		/*the next EVS starts at:*/
		startEVS=(frame_transition);
		idT++;
		idEVS++;

		transition_detected=0;
		fade_beginning_detected=0;

	}

	/*the last EVS*/
	durationEVS=(m_Media->GetImageTime())-startEVS;
	CreateEditedVideoSegment(MainSegmentDecompositionInterfaceGen,startEVS,durationEVS,idEVS);
  
  return(0UL);
}
//----------------------------------------------------------------------------
void VideoEditingExtractionTool::mimResize(MomVop *procFrame,unsigned char *framebufResize,int width_factor,int height_factor){ 
  int i,j;
  int col;
  int line;
  int colresize;int lineresize;
   
  col=procFrame->width;line=procFrame->height;
  colresize=(int)(col/width_factor);
  lineresize=(int)(line/height_factor);
  for(i=0;i<lineresize;i++){
    for(j=0;j<colresize;j++){
      framebufResize[i*colresize+j]=*(procFrame->y_chan->data->u+(i*height_factor+4)*col+(j*width_factor+4));
      
    }}
}

//----------------------------------------------------------------------------
 void VideoEditingExtractionTool::mbufCopy(unsigned char *input,unsigned char *output,int size){
   int i;
   for(i=0;i<size;i++) output[i]=input[i];
}

//----------------------------------------------------------------------------
void VideoEditingExtractionTool:: mimArithSigne(unsigned char *Image0, unsigned char *Image1, int *Image2,int size, int op)
{
  int i;
  if(op==M_SUB_ABS){
    for(i=0;i<size;i++) Image2[i]=abs((int)Image1[i]-(int)Image0[i]);
  }
   if(op==M_SUB){
    for(i=0;i<size;i++) Image2[i]=(int)((int)Image1[i]-(int)Image0[i]);
  }
}


//----------------------------------------------------------------------------
DoubleArray *VideoEditingExtractionTool::setTransitionMatrix(double dt)
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
DoubleArray *VideoEditingExtractionTool::setProcessNoise(double dt, double accelNoise)
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
DoubleArray *VideoEditingExtractionTool::setMeasureMatrix(void )
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
DoubleArray *VideoEditingExtractionTool::setMeasureNoise(double measureNoise)

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
int VideoEditingExtractionTool::kalmanOneStep(
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
					&VEdeleteDoubleArrayElement);
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
					    &VEdeleteDoubleArrayElement);
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
					    &VEdeleteDoubleArrayElement);
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
		linkedListDestroy(rmLinkedList,&VEdeleteDoubleArrayElement);
		term ++;
	      }
	    else
	      {

		if (e->nextElement == (Element *)NULL)
		  {
		    rmLinkedList = (LinkedList*)linkedListRemoveLast
		      ((LinkedList*)kalState->X);
		    linkedListDestroy(rmLinkedList,&VEdeleteDoubleArrayElement);
		    e = (Element*) NULL;
		    term ++;
		  }
		else
		  
		  {
		    etmp = e->nextElement;
		    e->previousElement->nextElement = e->nextElement;
		    e->nextElement->previousElement = e->previousElement;
		    rmLinkedList = (LinkedList*)e->object;
		    linkedListDestroy(rmLinkedList,&VEdeleteDoubleArrayElement);
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
      linkedListDestroy(gateList,&VEfreeListIntegerObject);
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
unsigned long VideoEditingExtractionTool::NewSequence(void)

{	/*Fade Detection*/


	if (!m_DescriptorInterface)  return (unsigned long)-1;

	if (m_DescriptorInterface->GetDSName()=="#document") {
	  VideoEditingGen=m_DescriptorInterface->GetFirstChild();
	  if (!VideoEditingGen.isNull()) {
	    if (VideoEditingGen.GetDSName()=="VideoEditing") {
	      m_DescriptorInterface->RemoveChild(VideoEditingGen);
	    }
	  }
	}

	/* create sequential description*/
	VideoEditingGen=m_DescriptorInterface->
	  CreateChild("VideoEditing");
	VideoEditingInterfaceGen=VideoEditingGen.GetInterface();
	VideoEditingGen.SetAttribute("use","analytic");
	CreateMainMediaInformation();
	CreateMainSegmentDecomposition();

	return 0;
}

//----------------------------------------------------------------------------
void VideoEditingExtractionTool:: CreateEditedVideoSegment(GenericDSInterfaceABC *SegDecompInterface,long start,long duration,long id){
	
	char ID[10];
	sprintf(ID,"EVS:%ld",id);

	  GenericDS EVSGen=SegDecompInterface->CreateChild("EditedVideoSegment");

        GenericDSInterfaceABC *EVSInterfaceGen=EVSGen.GetInterface();

	EVSInterfaceGen->SetAttribute("id",ID);	
	EVSInterfaceGen->SetAttribute("editingLevel","shot");	
	EVSInterfaceGen->SetAttribute("use","analytic");	
        CreateMediaTime(EVSInterfaceGen,start,duration);	
}
//----------------------------------------------------------------------------
void VideoEditingExtractionTool::CreateMediaTime(GenericDSInterfaceABC *theSegmentInterface,long start, long duration){
	
	GenericDS  MediaTimeGen=theSegmentInterface->CreateChild("MediaTime");

        GenericDSInterfaceABC *MediaTimeInterfaceGen=MediaTimeGen.GetInterface();

	GenericDS MediaRelIncrTimePointGen=MediaTimeInterfaceGen->CreateChild("MediaRelIncrTimePoint");	
	char timeUnit[15];
	sprintf(timeUnit,"PT1N%d",m_nnfraction);
	MediaRelIncrTimePointGen.SetAttribute("timeUnit",timeUnit);
	MediaRelIncrTimePointGen.SetValue((int)start);

	GenericDS MediaIncrDurationGen=MediaTimeInterfaceGen->CreateChild("MediaIncrDuration");
	
	MediaIncrDurationGen.SetAttribute("timeUnit",timeUnit);
	MediaIncrDurationGen.SetValue((int)duration);


}
//----------------------------------------------------------------------------

void VideoEditingExtractionTool:: CreateGradualEvolution(GenericDSInterfaceABC *SegDecompInterface){

	  GenericDS GradualGen=SegDecompInterface->
CreateChild("GradualEvolution");
	   GenericDSInterfaceABC *GradualInterfaceGen=GradualGen.GetInterface();
	   GenericDS TypeGen=GradualInterfaceGen->CreateChild("Type");
	   GenericDSInterfaceABC *TypeInterfaceGen=TypeGen.GetInterface();
	   GenericDS FreeTermGen=TypeInterfaceGen->CreateChild("FreeTerm");
	   GenericDSInterfaceABC *FreeTermInterfaceGen=FreeTermGen.GetInterface();
	   FreeTermInterfaceGen->SetAttribute("xml:lang","en");
	   FreeTermInterfaceGen->SetValue("Fade");

		}

//----------------------------------------------------------------------------

void VideoEditingExtractionTool:: CreateTransition(GenericDSInterfaceABC *SegDecompInterface,long start,long duration,char *type,long id){

	char ID[10];
	sprintf(ID,"Trans:%ld",id);
	GenericDS TransitionGen=SegDecompInterface->CreateChild("Transition");
	GenericDSInterfaceABC *TransitionInterfaceGen=TransitionGen.GetInterface();
		
	TransitionInterfaceGen->SetAttribute("id",ID);	
        if(strcmp(type,"cut")==0){
	TransitionInterfaceGen->SetAttribute("evolution","cut");	
	}
	if(strcmp(type,"grad")==0){
	TransitionInterfaceGen->SetAttribute("evolution","gradual");	
		CreateGradualEvolution(TransitionInterfaceGen);
	}

	TransitionInterfaceGen->SetAttribute("editingLevel","global");	
	TransitionInterfaceGen->SetAttribute("use","analytic");	
        CreateMediaTime(TransitionInterfaceGen,start,duration);	


		}
//----------------------------------------------------------------------------

void VideoEditingExtractionTool:: CreateMainSegmentDecomposition(){

		MainSegmentDecompositionGen=VideoEditingInterfaceGen->CreateChild("SegmentDecomposition");
	MainSegmentDecompositionInterfaceGen=MainSegmentDecompositionGen.GetInterface();
		// Set Decomposition Type
		MainSegmentDecompositionGen.SetAttribute("type", "temporal");
		MainSegmentDecompositionGen.SetAttribute("overlap", "true");
		/*the video segments representing a cut will contain the last frame of the previous VS and the first of the next one*/
		MainSegmentDecompositionGen.SetAttribute("gap", "false");
		MainSegmentDecompositionGen.SetAttribute("temporalConnectivity", "true");

}

//----------------------------------------------------------------------------

void VideoEditingExtractionTool:: CreateSegmentDecomposition(GenericDSInterfaceABC *parentInterface){

		GenericDS SegmentDecompositionGen;
		SegmentDecompositionGen=parentInterface->CreateChild("SegmentDecomposition");

		// Set Decomposition Type
		SegmentDecompositionGen.SetAttribute("type", "temporal");
		SegmentDecompositionGen.SetAttribute("overlap", "true");
		/*the video segments representing a cut will contain the last frame of the previous VS and the first of the next one*/
		SegmentDecompositionGen.SetAttribute("gap", "false");
		SegmentDecompositionGen.SetAttribute("temporalConnectivity", "true");


}

//----------------------------------------------------------------------------

void VideoEditingExtractionTool:: CreateMainMediaInformation(void){

	if (m_Media->GetVideoFileName()) {
	GenericDS MediaInformationGen= VideoEditingInterfaceGen->CreateChild("MediaInformation");
	  GenericDSInterfaceABC *MediaInformationInterfaceGen=
	    MediaInformationGen.GetInterface();

	  GenericDS MediaProfileGen=MediaInformationInterfaceGen->
	    CreateChild("MediaProfile");
	  GenericDSInterfaceABC *MediaProfileInterfaceGen=
	     MediaProfileGen.GetInterface();
	     
	  GenericDS MediaFormatGen=MediaProfileInterfaceGen->
	    CreateChild("MediaFormat");

	  GenericDSInterfaceABC *MediaFormatInterfaceGen=
	     MediaFormatGen.GetInterface();
	GenericDS ContentGen=MediaFormatInterfaceGen->
	    CreateChild("Content");
	GenericDSInterfaceABC *ContentInterfaceGen=ContentGen.GetInterface();
	ContentInterfaceGen->SetValue("audiovisual");
	GenericDS FileFormatGen=MediaFormatInterfaceGen->
	    CreateChild("FileFormat");
	GenericDSInterfaceABC *FileFormatInterfaceGen=FileFormatGen.GetInterface();
	FileFormatInterfaceGen->SetAttribute("scheme","MPEG7FileFormatCS");;
	FileFormatInterfaceGen->SetAttribute("term",3);
	GenericDS LabelGen=FileFormatInterfaceGen->
	    CreateChild("Label");

	GenericDSInterfaceABC *LabelInterfaceGen=LabelGen.GetInterface();

	LabelInterfaceGen->SetAttribute("xml:lang","en");
	LabelInterfaceGen->SetValue("mpeg");
	
	GenericDS VisualCodingGen=MediaFormatInterfaceGen->
	    CreateChild("VisualCoding");
	GenericDSInterfaceABC *VisualCodingInterfaceGen=VisualCodingGen.GetInterface();
	GenericDS FrameGen=VisualCodingInterfaceGen->
	    CreateChild("Frame");
	   GenericDSInterfaceABC *FrameInterfaceGen=FrameGen.GetInterface();

	  FrameInterfaceGen->SetAttribute("height",m_Vop->height);
	  FrameInterfaceGen->SetAttribute("width",m_Vop->width);
	  FrameInterfaceGen->SetAttribute("rate",m_nnfraction);
	

	GenericDS MediaInstanceGen=MediaProfileInterfaceGen->
	    CreateChild("MediaInstance");
	GenericDSInterfaceABC *MediaInstanceInterfaceGen=MediaInstanceGen.GetInterface(); 
	GenericDS InstanceIdentifierGen=MediaProfileInterfaceGen->
	    CreateChild("InstanceIdentifier");
 GenericDSInterfaceABC *InstanceIdentifierInterfaceGen=InstanceIdentifierGen.GetInterface(); 

GenericDS IDNameGen=InstanceIdentifierInterfaceGen->
	    CreateChild("IDName");
 GenericDSInterfaceABC *IDNameInterfaceGen=IDNameGen.GetInterface(); 
 
 GenericDS FreeTermGen=IDNameInterfaceGen->
	    CreateChild("FreeTerm");
 GenericDSInterfaceABC *FreeTermInterfaceGen=FreeTermGen.GetInterface(); 
	FreeTermInterfaceGen->SetValue("MPEG");
	
GenericDS IDOrganizationGen=InstanceIdentifierInterfaceGen->
	    CreateChild("IDOrganization");
 GenericDSInterfaceABC *IDOrganizationInterfaceGen=IDOrganizationGen.GetInterface(); 
 
 GenericDS FreeTerm2Gen=IDOrganizationInterfaceGen->
	    CreateChild("FreeTerm");
 GenericDSInterfaceABC *FreeTerm2InterfaceGen=FreeTerm2Gen.GetInterface(); 
	FreeTerm2InterfaceGen->SetValue("MPEG-7 ContentSet");
 GenericDS OffLineLocatorGen=MediaInstanceInterfaceGen->
	    CreateChild("OffLineLocator");
 GenericDSInterfaceABC *OffLineLocatorInterfaceGen=OffLineLocatorGen.GetInterface(); 
OffLineLocatorInterfaceGen->SetValue(m_Media->GetVideoFileName());

	}
		}
//----------------------------------------------------------------------------
const UUID& VideoEditingExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *VideoEditingExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
VideoEditingExtractionInterfaceABC *VideoEditingExtractionTool::GetInterface(void)
{ return &m_Interface; }

