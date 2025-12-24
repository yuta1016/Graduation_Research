///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
//	Rosa Ruiloba (LIP-6, France) 
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
// Applicable File Name:  VideoEditingExtraction.h
//
#ifndef __VIDEOEDITINGEXTRACTION_H__
#define __VIDEOEDITINGEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"
#include "Media/Media.h"
#include "Utilities/DoubleArray.h"
#include "Utilities/LinkedList.h"
#include "ExtractionUtilities/KLTFeaturePoints.h"

//=============================================================================

#ifndef _KALMANSTATEDEFIEND_
#define _KALMANSTATEDEFIEND_

typedef struct
{
  LinkedList *X; /* List of track lists */
  DoubleArray *P; /* State  covariance */
  double activity;
  double Term;
  double Init;
  
} KalmanState;
#endif //_KALMANSTATEDEFIEND_

namespace XM
{

// Forward Declarations:
class VideoEditingExtractionTool;

//=============================================================================
class VideoEditingExtractionInterfaceABC: public I_InterfaceABC
{
public:
	VideoEditingExtractionInterfaceABC();
	virtual ~VideoEditingExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long NewSequence(void) = 0;
	virtual unsigned long PostExtracting(void) = 0;

	static const UUID myID;
};

//=============================================================================
class VideoEditingExtractionInterface: 
  public VideoEditingExtractionInterfaceABC
{
public:
	VideoEditingExtractionInterface(VideoEditingExtractionTool* aTool);
	virtual ~VideoEditingExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long NewSequence(void);
	virtual unsigned long PostExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	VideoEditingExtractionTool *m_ExtractionTool;
};

//=============================================================================
class VideoEditingExtractionTool: public DescriptorExtractor
{
friend class VideoEditingExtractionInterface;
public:
	VideoEditingExtractionTool();
	VideoEditingExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long InitExtractingFade(void);//sth maybe private?
	virtual unsigned long StartExtracting(void);
	virtual unsigned long StartExtractingFade(void);
	virtual unsigned long NewSequence(void);
	virtual unsigned long PostExtracting(void);
/*	virtual unsigned long ExtractColorHistogram(void);*/

	virtual VideoEditingExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~VideoEditingExtractionTool();

	/* private functions*/
	DoubleArray *setTransitionMatrix(double dt);
	DoubleArray *setProcessNoise(double dt, double accelNoise);
	DoubleArray *setMeasureMatrix(void );
	DoubleArray *setMeasureNoise(double measureNoise);
	int kalmanOneStep(KalmanState *kal_State, DoubleArray *C,
			  DoubleArray *F, DoubleArray *H, DoubleArray *W,
			  DoubleArray *S_1, double distThresh, double dt);

	/* private functions Fade detection*/
	void mimResize( MomVop *procFrame,unsigned char *framebufResize,int width_factor,int height_factor);
	void mbufCopy(unsigned char *input,unsigned char *output,int size);
	void mimArithSigne(unsigned char *Image0, unsigned char *Image1, int *Image2,int size, int op);
       	void CreateMainMediaInformation(void);	
	void CreateSegmentDecomposition(GenericDSInterfaceABC *parentInterface);
	void CreateMainSegmentDecomposition();
	void CreateMediaTime(GenericDSInterfaceABC *theSegmentInterface,long start, long duration);
	void  CreateEditedVideoSegment(GenericDSInterfaceABC *SegDecompInterface,long start,long duration,long id);
	void CreateTransition(GenericDSInterfaceABC *SegDecompInterface,long start,long duration,char *type,long id);
	void CreateGradualEvolution(GenericDSInterfaceABC *SegDecompInterface);
 
	/* interface of this*/
	VideoEditingExtractionInterface m_Interface;

	/* extraction parameters*/
	KLTFeaturePoints m_KLTFeaturePoints /* interface to KLT library*/;
        float m_distthresh;  /* distance thres hold in kalman filter*/
        int m_nnfraction;  /* time base fraction*/
        int m_nn;          /* time base fraction multiplier*/
        float m_frameduration;/* time difference between frames*/
        float m_accelnoise;/* for process noise*/
        float m_alpha;     /* wheight for updating mean value*/
        float m_measurenoise;/* measurement noise*/
        float m_stddeviation;/* initial value for standard deviation*/
	int m_writeall;      /* write frame change value for every frame*/

	/* parameters for dissolve detection*/
	unsigned long transition_detected;
	unsigned long fade_detected;		

	int fade_beginning_detected;
 	long frame_fade_beginning, frame_fade_ending;	
	int frame_transition;
	int waitFadeDetection;
	
	int col;
	int line;
	int colresize;
	int lineresize;
	int nbpix;
	int correlation;
	unsigned char signe;
	char val8;
        int fade_max_threshold;
        int fade_min_threshold;
	int width_zoom_factor; 
	int height_zoom_factor; 

 
	int thresfade1;
	int thresfade2;
	unsigned char	*milImageResize;     /*Buffer for resized image*/ 
	unsigned char *last_lum_image;		  /*Buffer for resized image*/  
	int *buffer1;			   
	unsigned char *signe_variation;
 	long startEVS, durationEVS;	
 	long startT, durationT;	

	/* description*/
	GenericDSInterfaceABC *m_DescriptorInterface;
	GenericDS VideoEditingGen;
	GenericDSInterfaceABC *VideoEditingInterfaceGen;
	GenericDS MainSegmentDecompositionGen;
	GenericDSInterfaceABC *MainSegmentDecompositionInterfaceGen;

	long idT;
	long idEVS;


	/* input media*/
	MultiMediaInterfaceABC *m_Media;
	MomVop *m_Vop;

	/* inputs and output of  kalman filtering*/
	int m_FirstFrame;
	int m_SecondFrame;
	int m_Range;
	double m_Mean;
	double m_stdDeviation;

	/* global matrices for kalman filtering*/
	DoubleArray *F;    /* transition matrix*/
	DoubleArray *Q;    /* process noise matrix*/
	DoubleArray *H;    /* measure matrix*/
	DoubleArray *R;    /* measure noise matrix*/
	
	KalmanState kalState;
};

};


#endif //__VIDEOEDITINGEXTRACTION_H__
