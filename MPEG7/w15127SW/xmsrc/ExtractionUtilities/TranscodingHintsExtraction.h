///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Peter Kuhn, Teruhiko Suzuki (SONY Corporation Japan)
//
// and modified by
//
// Anthony Vetro (Mitsubishi)
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
// Applicable File Name:  TranscodingExtraction.h
//
#ifndef __TRANSCODINGEXTRACTION_H__
#define __TRANSCODINGEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"                  /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                     /* Naming Convention */
#include "Descriptors/Descriptors.h"                   /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"            /* Naming Convention */
#include "momusys.h"
#include "intra.h"
#include "coortrans.h"

// ############################################################################
// PK declarations

//sth #include "../lib/kltlib/klt.h"


// void feature_point_extraction(int framenum);

// ############################################################################



namespace XM
{


// Forward Declarations:
class TranscodingExtractionTool;

//=============================================================================
class TranscodingExtractionInterfaceABC: public I_InterfaceABC
{
public:
	TranscodingExtractionInterfaceABC();
	virtual ~TranscodingExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MomVop *media, MomVop *prevmedia) = 0;
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
	virtual int SetSourceMediaFilename(char *aFilename) = 0;
	
	virtual int SetDSInterface(GenericDSInterfaceABC *ParentDSInterface) = 0;
	
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting() = 0;
	virtual unsigned long NewSequence(void) = 0;
	virtual unsigned long PostExtracting(int startframe, int k) = 0;
	virtual unsigned long CloseSession(int lastframe)=0;
	virtual unsigned long GetCalculateDifficulty(void)=0;
	virtual int GetUseDifficulty(void)=0;
	virtual int GetUseDDL(void)=0;
	virtual int GetStartFrame(void)=0;
	virtual int GetFrames(void)=0;
	virtual char *GetSimDir(void)=0;
	
	virtual char *GetReencodingParamfilename(void)=0;
	virtual char *GetDifficultyParamfilename(void)=0;

	virtual unsigned long GetCalculateObjectDifficulty(void)=0;
	virtual unsigned long GetCalculateMotionIntensity(void)=0;
	virtual unsigned long GetCalculateShape(void)=0;
	virtual unsigned long GetMediaCodingParams(void)=0;
	virtual int GetUseMESearchRange(void)=0;
	virtual int GetUseMotionUncompensability(void)=0;
	virtual int GetUseMotionIntensity(void)=0;
	virtual int GetUseShape(void)=0;
	virtual int GetBitRate(void)=0;
	virtual int GetCompressionFormat(void)=0;
	virtual int GetNumVO(void)=0;
	virtual char *GetDifficultyCtlfilename(void)=0;
	virtual char *GetMotionIntensityfilename(void)=0;
	virtual char *GetShapeDistfilename(void)=0;

	static const UUID myID;
};

//=============================================================================
class TranscodingExtractionInterface:
	public TranscodingExtractionInterfaceABC
{
public:
	TranscodingExtractionInterface(TranscodingExtractionTool* aTool);
	virtual ~TranscodingExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *media, MomVop *prevmedia);
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual int SetSourceMediaFilename(char *aFilename);
	virtual int SetDSInterface(GenericDSInterfaceABC *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting();
	virtual unsigned long NewSequence(void);
	virtual unsigned long PostExtracting(int startframe, int k);
	virtual unsigned long CloseSession(int lastframe);
	virtual int GetUseDifficulty(void);
	virtual int GetUseDDL(void);
	virtual unsigned long GetCalculateDifficulty(void);
	virtual int GetStartFrame(void);
	virtual int GetFrames(void);
	virtual char *GetSimDir(void);
	
	virtual char *GetReencodingParamfilename(void);
	virtual char *GetDifficultyParamfilename(void);

	virtual unsigned long GetCalculateObjectDifficulty(void);
	virtual unsigned long GetCalculateMotionIntensity(void);
	virtual unsigned long GetCalculateShape(void);
	virtual unsigned long GetMediaCodingParams(void);
	virtual int GetUseMESearchRange(void);
	virtual int GetUseMotionUncompensability(void);
	virtual int GetUseMotionIntensity(void);
	virtual int GetUseShape(void);
	virtual int GetBitRate(void);
	virtual int GetCompressionFormat(void);
	virtual int GetNumVO(void);
	virtual char *GetDifficultyCtlfilename(void);
	virtual char *GetMotionIntensityfilename(void);
	virtual char *GetShapeDistfilename(void);
		
	static const UUID myID;
	static const char *myName;

private:
	TranscodingExtractionTool *m_ExtractionTool;
};

//=============================================================================
class TranscodingExtractionTool: public DescriptorExtractor
{
friend class TranscodingExtractionInterface;
public:
	TranscodingExtractionTool();
	TranscodingExtractionTool(GenericDSInterface *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *media, MomVop *prevmedia);
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	     /* frame number must be set AFTER Media was set!!!*/
 	virtual int SetSourceMediaFilename(char *aFilename);
                           // for fileIO with preprossessed preliminary results
	virtual int SetDSInterface(GenericDSInterfaceABC *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting();
	virtual unsigned long NewSequence(void);
	virtual unsigned long PostExtracting(int startframe, int k);
	virtual unsigned long CloseSession(int lastframe);
	virtual int GetUseDifficulty(void);
	virtual int GetUseDDL(void);
	virtual int GetStartFrame(void);
	virtual int GetFrames(void);
	virtual char *GetSimDir(void);

	virtual unsigned long GetCalculateDifficulty(void);
	virtual char *GetReencodingParamfilename(void);
        virtual char *GetDifficultyParamfilename(void);

	virtual unsigned long GetCalculateObjectDifficulty(void);
	virtual unsigned long GetCalculateMotionIntensity(void);
	virtual unsigned long GetCalculateShape(void);
	virtual unsigned long GetMediaCodingParams(void);
	virtual int GetUseMESearchRange(void);
	virtual int GetUseMotionUncompensability(void);
	virtual int GetUseMotionIntensity(void);
	virtual int GetUseShape(void);
	virtual int GetBitRate(void);
	virtual int GetCompressionFormat(void);
	virtual int GetNumVO(void);
	virtual char *GetDifficultyCtlfilename(void);
	virtual char *GetMotionIntensityfilename(void);
	virtual char *GetShapeDistfilename(void);

	virtual TranscodingExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~TranscodingExtractionTool();
	
	/* private functions*/
	//int calc_fp_range(int framenum, int nfeatures);
	
	/* interface of this */	 	 
	TranscodingExtractionInterface m_Interface;
		
	/* extraction parameters*/
	int m_seg_frames; 	/* 0: */
	int m_fp_new_me_seg;    /* 100: number of featurepoints for
	                           which a new segment is initialized */
	int m_segmin;           /* 15: minimum length of segment */
	int m_me_segmax;        /* 128: maximum lenght of segment */
	int m_me_fix;		/* 0: */
	int m_me_max;           /* 63: maximum search range */
	int m_me_min;           /* 0:  minimum search range */
 
	int m_motion_range_x_left;	/* Motion_range_x_left */
	int m_motion_range_x_right;	/* Motion_range_x_right */
	int m_motion_range_y_left;	/* Motion_range_y_left */
	int m_motion_range_y_right;	/* Motion_range_y_right */
	int m_motion_uncompensability;	/* Motion Uncompensability */	
	int m_difficulty; 		/* Difficulty */
	
	char *m_Sim_Dir_filename;	/* directory for simulation */
	char *m_Encoding_par_filename; /* param file for MPEG-1/2 Reencoding */
	char *m_Difficulty_par_filename; /* param file for difficulty hints exytraction */
	char *m_Difficulty_bits_filename; /*stat.out.bits location */
        int m_startframe;		/* decoding startframe */
	int m_frames; 			/* number of frames to be decoded */
	int m_use_me_search_range; 		/* use me_search_range DS */
	int m_use_motion_uncompensability;  /* use motion_uncompensability DS */
	int m_use_difficulty;         	/* use m_difficulty DS */
	int m_use_ddl;

	char *m_Difficulty_ctl_filename;      /* ctl file for difficulty hints extraction (MPEG4) */
	char *m_Difficulty_objbits_filename;  /* bits.dat location */
	char *m_Motion_intensity_filename;    /* intensity.dat location */
	char *m_Shape_dist_filename;          /* sh_dist.dat location */
	int m_use_motion_intensity;           /* use m_motion_intensity DS */
	int m_use_shape;                      /* use m_shape DS */
	int m_compression_format;             /* 0: MPEG-2, 1: MPEG-4 */
	int m_numvos;                         /* number of MPEG-4 objects */
	int m_bitrate;
	float m_framerate;
	int m_framewidth;
	int m_frameheight;
	int m_mediacoding;
		
	/* description */
	GenericDSInterfaceABC *m_DSInterface;
	GenericDS m_Transcoding;
	
	GenericDS m_Comment1;
	GenericDS m_Mpeg7;
	GenericDSInterfaceABC *m_Mpeg7Interface;
	GenericDS m_ContentDescription;
	GenericDSInterfaceABC *m_ContentDescriptionInterface;
	GenericDS m_MultimediaContent;
	GenericDSInterfaceABC *m_MultimediaContentInterface;
	GenericDS m_Video;
	GenericDSInterfaceABC *m_VideoInterface;
	GenericDS m_Header;
	GenericDSInterfaceABC *m_HeaderInterface;
	GenericDS m_MediaLocator;
	GenericDSInterfaceABC *m_MediaLocatorInterface;
	GenericDS m_MediaUri;
	GenericDSInterfaceABC *m_MediaUriInterface;
	
	GenericDS m_Difficulty;
   	GenericDSInterfaceABC *m_TranscodingHintsInterface;
	
//	GenericDS m_MovingRegion;
//	GenericDSInterfaceABC *m_MovingRegionInterface;
//	GenericDS m_SpatioTemporalLocator;
//	GenericDSInterfaceABC *m_SpatioTemporalLocatorInterface;

	
	/* input media*/
	MomVop *m_Media;
	MomVop *m_Media_prev;
	char *m_MediaFilename;
	unsigned long m_FrameNumber;
	int m_nnfraction;
	int m_nn;																
};
};

//============================================================================

extern "C" void klt_if(MomVop *m_Media, MomVop *m_Media_last, int ncols, int nrows, int framenum);
extern "C" void fp_range (int framenum, int nfeatures);
extern "C" int fp_calc_me_segment (int framenum, int is_last_frame, int compression_format);
extern "C" void calc_search_range (int start_frame, int end_frame);
extern "C" void calc_seg_difficulty(int seg, unsigned long difficulty_bitsum, int max);
extern "C" unsigned long  init_difficulty(int maxframes);
extern "C" int read_bitratefile(char *fname);
extern "C" char getstring(FILE *fp, char *buf, int buflen, char *delim, char *ignlead, char *igntrail);
extern "C" char e_getstring(FILE *fp, char *buf, int buflen, char *delim, char *ignlead, char *igntrail);
extern "C" long atoix(char *str);
	    		    	    
#endif //__TRANSCODINGEXTRACTION_H__
