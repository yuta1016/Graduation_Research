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
#ifndef __SEQSUMMARYFASTMOVINGSTORYBOARDEXTRACTION_H__
#define __SEQSUMMARYFASTMOVINGSTORYBOARDEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"               //Naming Convention 
#include "BasicArchitecture/XMObjects.h"                  //Naming Convention 
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"
#include "Media/Media.h"
#include "Utilities/DoubleArray.h"
#include "Utilities/LinkedList.h"
#include "ExtractionUtilities/KLTFeaturePoints.h"
#include "ExtractionUtilities/SummarizationExtraction.h"

//----------------------------------------------------------------------------
/** Time Scale Modification for MPEG-7
 *
 *  Written by:  Gilad Cohen (giladc@il.ibm.com)
 *               IBM Research, Haifa ISRAEL
 * 
 *  Last update: July 31, 2000
 * 
 *  Version:     1.0
 *
 *	WAV read and write function prototypes and definitions
 */

//sth use MultiMedia class and afsp library instead
//Minimal WAV header size for writing 
#define WRITE_WAV_HEADER_SIZE 44 

//PCM packing code in WAV  
#define	WAVE_FORMAT_PCM	 (0x0001) 

//WAV header structure
typedef struct WAV_STRUCT  
{
  char           riff[5];
  unsigned long  riffLen;
  char           wave[5];
  char           fmt[5];
  unsigned long  fmtLen;
  unsigned short format;
  unsigned short numChannels;
  unsigned long  samplesPerSecond;
  unsigned long  avgBytes;
  unsigned short blockAlign;
  unsigned short bitsPerSample;
  char           data[5]; 
  unsigned long  dataLen;
} WavStruct;


//=============================================================================
//Macro for extraction process

#ifndef _KALMANSTATEDEFIEND_
#define _KALMANSTATEDEFIEND_

typedef struct
{
  LinkedList *X; // List of track lists
  DoubleArray *P; // State  covariance 
  double activity;
  double Term;
  double Init;
  
} KalmanState;
#endif //_KALMANSTATEDEFIEND_

//=============================================================================

namespace XM
{

	// Forward Declarations:
	class SeqSummaryFastMovingStoryBoardExtractionTool;

	//=============================================================================
	class SeqSummaryFastMovingStoryBoardExtractionInterfaceABC: public I_InterfaceABC
	{
		public:
			SeqSummaryFastMovingStoryBoardExtractionInterfaceABC();
			virtual ~SeqSummaryFastMovingStoryBoardExtractionInterfaceABC() {};

			virtual void destroy() = 0;

			virtual int SetSourceMedia(MomVop *media) = 0;
			virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
			virtual int SetSourceMediaFilename(char *aFilename) = 0;
			virtual int SetSourceWavFilename(char *aFilename) = 0;

			virtual int SetDescriptorInterface(GenericDSInterfaceABC *ParentDSInterface) = 0;

			virtual unsigned long InitExtracting(void) = 0;
			virtual unsigned long StartExtracting(void) = 0;
			virtual unsigned long NewSequence(void) = 0;
			virtual unsigned long PostExtraction(void) = 0;

			static const UUID myID;
	};

	//=============================================================================
	class SeqSummaryFastMovingStoryBoardExtractionInterface: public SeqSummaryFastMovingStoryBoardExtractionInterfaceABC
	{
		public:
			SeqSummaryFastMovingStoryBoardExtractionInterface(SeqSummaryFastMovingStoryBoardExtractionTool* aTool);
			virtual ~SeqSummaryFastMovingStoryBoardExtractionInterface();
			
			virtual const UUID& GetInterfaceID(void);
			virtual const char *GetName(void);

			virtual void destroy();

			virtual int SetSourceMedia(MomVop *media);
			virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
			virtual int SetSourceMediaFilename(char *aFilename);
			virtual int SetSourceWavFilename(char *aFilename);

			virtual int SetDescriptorInterface(GenericDSInterfaceABC
								 *ParentDSInterface);

			virtual unsigned long InitExtracting(void);
			virtual unsigned long StartExtracting(void);
			virtual unsigned long NewSequence(void);
			virtual unsigned long PostExtraction(void);

			static const UUID myID;
			static const char *myName;

		private:
			SeqSummaryFastMovingStoryBoardExtractionTool *m_ExtractionTool;
	};

	//=============================================================================
	class SeqSummaryFastMovingStoryBoardExtractionTool: public DescriptorExtractor
	{
		friend class SeqSummaryFastMovingStoryBoardExtractionInterface;
		public:
			SeqSummaryFastMovingStoryBoardExtractionTool();
			SeqSummaryFastMovingStoryBoardExtractionTool(GenericDSInterface *theParent);

			virtual const UUID& GetObjectID(void);
			virtual const char *GetName(void);

			virtual bool SupportsPush(void);
			virtual bool SupportsPull(void);

			virtual int SetSourceMedia(MomVop *media);
			virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
			virtual int SetSourceMediaFilename(char *aFilename);
			virtual int SetSourceWavFilename(char *aFilename);
			//for fileIO with preprossessed preliminary results
			virtual int SetDescriptorInterface(GenericDSInterfaceABC *ParentDSInterface);
			
			virtual unsigned long InitExtracting(void);
			virtual unsigned long StartExtracting(void);
			virtual unsigned long NewSequence(void);
			virtual unsigned long PostExtraction(void);

			virtual SeqSummaryFastMovingStoryBoardExtractionInterfaceABC *GetInterface(void);

			// access is allowed only by class factories for this
			// object.  This avoids having to duplicate the
			// ID definition in multiple locations.  In the future, we may
			// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
			// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
			static const UUID myID;
			static const char * myName;
		private:
			virtual ~SeqSummaryFastMovingStoryBoardExtractionTool();

			//private functions
			DoubleArray *setTransitionMatrix(double dt);
			DoubleArray *setProcessNoise(double dt, double accelNoise);
			DoubleArray *setMeasureMatrix(void );
			DoubleArray *setMeasureNoise(double measureNoise);
			int kalmanOneStep(KalmanState *kal_State, DoubleArray *C, DoubleArray *F, DoubleArray *H, DoubleArray *W,	DoubleArray *S_1, double distThresh, double dt);

			//time format function prototypes
			void ms2time(int ms, char* res);
			void ms2realtime(int ms, char* res);

			//WAV IO function prototypes
			void WriteWavFile(const char *file_name, const float x[], const long len, const int freq);
			void ReadWavFile(char *file_name, float **x, long *len, int *freq);

			unsigned long ReadUnsignedLong(FILE *fp);
			unsigned short ReadUnsignedShort(FILE *fp);
			short ReadShort(FILE *fp);
			void WriteUnsignedShort(FILE *fp, unsigned short us);
			void WriteUnsignedLong(FILE *fp, unsigned long ul);
			void WriteShort(FILE *fp, short s);
			void InPlaceFloatToShortConversion(void* pvBuffer, const int iLen);
			void ParseWavHeader(FILE *fp, WavStruct *pWavStruct);
			int WriteWavHeader(const int numChannels, const int frequency, const int bitPerSample, const int DataSize, FILE *fp);
			void InitWindow(float **w, const int win_span);
			float EvalCorr(float *x, float *y, const int win_span);
			void ActivePrompt(long iTotalCount, long iCount);
			void wsola(char* input_file_name, char* output_file_name, float alfa);

			// interface of this
			KLTFeaturePoints m_KLTFeaturePoints; /* interface to KLT library*/
			SeqSummaryFastMovingStoryBoardExtractionInterface m_Interface;

			// extraction parameters
			float m_distthresh;				//distance thres hold in kalman filter
			int m_nnfraction;					//time base fraction
			int m_nn;									//time base fraction multiplier
			float m_frameduration;		//time difference between frames
			float m_accelnoise;				//for process noise
			float m_alpha;						//wheight for updating mean value
			float m_measurenoise;			//measurement noise
			float m_stddeviation;			//initial value for standard deviation
			int m_lastKeyFrameTime;

			//description
			GenericDSInterfaceABC *m_DescriptorInterface;
			GenericDS m_SequentialSummary;
			GenericDSInterfaceABC *m_SequentialSummaryInterface;

			GenericDS m_SyncTime;
			GenericDSInterfaceABC *m_SyncTimeInterface;
			GenericDS m_AudioDuration;
			GenericDSInterfaceABC *m_AudioDurationInterface;

			//smil output
			std::fstream smilout;
			char *m_WavFilename;

			//input media
			MomVop *m_Media;
			char *m_MediaFilename;
			unsigned long m_FrameNumber;

			//inputs and output of  kalman filtering
			int m_FirstFrame;
			int m_SecondFrame;
			int m_Range;
			double m_Mean;
			double m_stdDeviation;
			
			//global matrices for kalman filtering
			DoubleArray *F;    //transition matrix
			DoubleArray *Q;    //process noise matrix
			DoubleArray *H;    //measure matrix
			DoubleArray *R;    //measure noise matrix
			
			KalmanState kalState;
	};
};

extern "C" void freeListIntegerObject(void *ListObject,void *unused);
extern "C" void SMSpix_median(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void SMSpix_grad_matirx(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void SMSpix_corner_to_list(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void SMSpix_set_value(TPixel *res, TPixel *in, TCoor x, TCoor y);

#endif //__SEQSUMMARYFASTMOVINGSTORYBOARDEXTRACTION_H__
