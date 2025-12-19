///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S. Herrmann
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zahari (Media3D)
// (contributing organizations names)
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
// Applicable File Name:  MultiMedia.h

#ifndef __MultiMedia__
#define __MultiMedia__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"


// define media types
/* audio media -------------------------*/
#include "Media/AudioFrame.h"
#ifdef AFLIB
#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include <libtsp/AFmsg.h>
#endif

/* image media -------------------------*/
#include "vopio.h"
#include "momusys.h"
#include "intra.h"

/* 3D mesh media -----------------------*/
typedef struct three_d_mesh {
  int NoPoints;
  int NoTriangl;
 
  float **PObj;
  int **TObj;
} MediaThreeD;                                                                   
// keypoint media
#include "Utilities/LinkedList.h"

/* sth to be removed use nD types instead*/
struct fourD_key_point_struct {
  int time;
  float x_position;
  float y_position;
  float z_position;
};

typedef struct fourD_key_point_struct T4DKeyPoint;

//struct key_point_list_struct {
//  int Time;
//  LinkedList *KeyPoints;
//};
//
//typedef struct key_point_list_struct KeyPointList;

struct nD_key_point_struct {
  int time;
  int dim;
  float *p;
};
typedef struct nD_key_point_struct TnDKeyPoint;


struct key_point_list_struct {
  int Time;
  LinkedList *KeyPoints;
};

typedef struct key_point_list_struct KeyPointList;




/* defines for alpha layer selection--------------*/
#define BackgroundFromBinary (-1)
#define BackgroundFromAlpha (-2)
#define AllForegroundFromBinary (-3)
#define ForegroundFromAlpha (-4)


namespace XM
{

//=============================================================================
class MultiMediaInterfaceABC: public I_InterfaceABC
{
public:
	MultiMediaInterfaceABC();
	virtual ~MultiMediaInterfaceABC() {}

	virtual void destroy() = 0;

	//audio media
	virtual CAudioFrame *GetAudioFrame(void) = 0;
	virtual int SetAudioFrame(CAudioFrame) = 0;

	// video media
	virtual char *GetVideoFileName(void) = 0;
	virtual int SetVideoFileName(char *) = 0;
	// 2009.05.18 - opens video, and loads video frames
	virtual int OpenVideoFile() = 0;
	virtual MomVop *LoadNextVideoFrame() = 0;
	virtual void CloseVideoFile() = 0;
	virtual double GetVideoFrameRate() = 0;
	virtual int IsVideoInterlaced() = 0;

	// image media
	virtual char *GetImageFileName(void) = 0;
	virtual int SetImageFileName(char *) = 0;
	virtual char *GetImageComment(void) = 0;
	virtual int SetImageComment(char *) = 0;
	virtual int GetImageTime(void) = 0;
	virtual int SetImageTime(int) = 0;
	virtual MomVop *GetImage(void) = 0;
	virtual MomVop *GetSubImage(int x, int y, int w, int h) = 0;
	virtual int SetImage(MomVop *) = 0;
	virtual int FreeImage(void) = 0;
	virtual int GetMinAlpha() = 0;
	virtual void SetMinAlpha(int value) = 0;
	virtual int GetMaxAlpha() = 0;
	virtual void SetMaxAlpha(int value) = 0;
	virtual int SelectVopAlphaLayer(int selectedlayer) = 0;


	//sth functions for loading audio media missing (SoundProperty)
	//usage of AFsp library directly is not permitted

	// keypoint media = hierarchical linked list
	/* 1. linked list of key points = key point list
	   2. linked list of key point lists (one key point list for
	                                      each time point)*/
	virtual char *GetKeyPointFileName(void) = 0;
	virtual int SetKeyPointFileName(char *) = 0;


	/* lists of key point lists*/
	virtual int GetKeyPointTime(void) = 0; /* get time of current
						  key point list*/
	virtual int GotoKeyPointList(int index) = 0;
	                                       /* select the n-th key point
						  list (for one time point)*/
	virtual int NextKeyPointList(void) = 0;/* select next key point list
						  (for next time point)*/
	virtual int AddKeyPointsList(int time) = 0;
	                                       /* add a new list for a new
						  time point*/
	virtual int FreeKeyPointLists() = 0;   /* delete all lists of
						  key point lists*/

	/* key point lists*/
	virtual TnDKeyPoint *GetKeyPoint(void) = 0;
                                               /* Get current Key point
						  at current time*/
	virtual int GotoKeyPoint(int index) = 0;
	                                       /* Goto n-th key point at
						  current time*/
	virtual int NextKeyPoint(void) = 0;    /* Goto next key point at
						  current time*/
	virtual int AddKeyPoint(TnDKeyPoint*) = 0;
	                                       /* append a key point to list
						  at current time*/

	//sth to be removed
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPointNo in multimedia class
	virtual int SetKeyPointNumber(int Number) = 0;
//--- added by Sylvie Jeannin, August 23, to get m_KeyPointNo in multimedia class
	virtual int GetKeyPointNumber(void) = 0;
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPoint in multimedia class
	virtual int SetKeyPointPointer(T4DKeyPoint *KeyPointPointer) = 0;
//--- added by Sylvie Jeannin, August 22, to get m_KeyPoint in multimedia class
	virtual T4DKeyPoint *GetKeyPointPointer(void) = 0;


	static const UUID myID;
};

class MultiMedia;

//=============================================================================
class MultiMediaInterface: public MultiMediaInterfaceABC
{
public:
	MultiMediaInterface(MultiMedia *aMultiMedia);
	virtual ~MultiMediaInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	static const UUID myID;
	static const char *myName;

	// audio media
	virtual CAudioFrame *GetAudioFrame(void);
	virtual int SetAudioFrame(CAudioFrame);

	// video media
	virtual char *GetVideoFileName(void);
	virtual int SetVideoFileName(char *);
	// 2009.05.18 - opens video, and loads video frames
	virtual int OpenVideoFile();
	virtual MomVop *LoadNextVideoFrame();
	virtual void CloseVideoFile();
	virtual double GetVideoFrameRate();
	virtual int IsVideoInterlaced();

	// image media
	virtual char *GetImageFileName(void);
	virtual int SetImageFileName(char *);
	virtual char *GetImageComment(void);
	virtual int SetImageComment(char *);
	virtual int GetImageTime(void);
	virtual int SetImageTime(int);
	virtual MomVop *GetImage(void);
	virtual MomVop *GetSubImage(int x, int y, int w, int h);
	virtual int SetImage(MomVop *);
	virtual int FreeImage(void);
	virtual int GetMinAlpha();
	virtual void SetMinAlpha(int value);
	virtual int GetMaxAlpha();
	virtual void SetMaxAlpha(int value);
	virtual int SelectVopAlphaLayer(int selectedlayer);

	// keypoint media = hierarchical linked list
	virtual char *GetKeyPointFileName(void);
	virtual int SetKeyPointFileName(char *);


	virtual int GetKeyPointTime(void);
	virtual int GotoKeyPointList(int index);
	virtual int NextKeyPointList(void);
	virtual int AddKeyPointsList(int time);
	virtual int FreeKeyPointLists();

	virtual TnDKeyPoint *GetKeyPoint(void);
	virtual int GotoKeyPoint(int index);
	virtual int NextKeyPoint(void);
	virtual int AddKeyPoint(TnDKeyPoint *);

	//sth to be removed
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPointNo in multimedia class
	virtual int SetKeyPointNumber(int Number);
//--- added by Sylvie Jeannin, August 23, to get m_KeyPointNo in multimedia class
	virtual int GetKeyPointNumber(void);
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPoint in multimedia class
	virtual int SetKeyPointPointer(T4DKeyPoint *KeyPointPointer);
//--- added by Sylvie Jeannin, August 22, to get m_KeyPoint in multimedia class
	virtual T4DKeyPoint *GetKeyPointPointer(void);


private:
	MultiMedia *m_MultiMedia;	
};

//=============================================================================
class MultiMedia
{
friend class MultiMediaInterface;
public:
	MultiMedia();

	// actual descriptor methods
	virtual bool IsProprietary(void);
	virtual const UUID& GetValueID(void);
	virtual const char* GetValueName(void);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual MultiMediaInterfaceABC *GetInterface(void);

	// accessor methods
	// audio media
	virtual CAudioFrame *GetAudioFrame(void);
	virtual int SetAudioFrame(CAudioFrame);

	// video media
	virtual char *GetVideoFileName(void);
	virtual int SetVideoFileName(char *);
	// 2009.05.18 - opens video, and loads video frames
	virtual int OpenVideoFile();
	virtual MomVop *LoadNextVideoFrame();
	virtual void CloseVideoFile();
	virtual double GetVideoFrameRate();
	virtual int IsVideoInterlaced();


	// image media
	virtual char *GetImageFileName(void);
	virtual int SetImageFileName(char *);
	virtual char *GetImageComment(void);
	virtual int SetImageComment(char *);
	virtual int GetImageTime(void);
	virtual int SetImageTime(int);
	virtual MomVop *GetImage(void);
	virtual MomVop *GetSubImage(int x, int y, int w, int h);
	virtual int SetImage(MomVop *);
	virtual int FreeImage(void);
	virtual int GetMinAlpha();
	virtual void SetMinAlpha(int value);
	virtual int GetMaxAlpha();
	virtual void SetMaxAlpha(int value);
	virtual int SelectVopAlphaLayer(int selectedlayer);
	/* select a layer from the binary segment mask
	   -1 = background layer (=0)
	   -2 = alpha < 128
	   -3 = all forground layers (!= 0)
	   -4 = alpha >= 128
	   n = layer number*/

	// keypoint media = hierarchical linked list
	virtual char *GetKeyPointFileName(void);
	virtual int SetKeyPointFileName(char *);

	               // first dimension has lists of key points
	virtual int GetKeyPointTime(void);     /* get current time of list*/
	virtual int GotoKeyPointList(int index);/* select key point list*/
	virtual int NextKeyPointList(void);    /* select next list of key
						  points
						  0 != no more key point
						  lists*/
	virtual int AddKeyPointsList(int time);/* add a new list for specified
						  timetime point */
	virtual int FreeKeyPointLists();       /* delete all key points in
						  all lists*/ 

                       // second dimension, key points in list
	virtual TnDKeyPoint *GetKeyPoint(void); /* get current key point*/
	virtual int GotoKeyPoint(int index);   /* select keypoint from list*/
	virtual int NextKeyPoint(void);        /* select next key point from
						  list
						  0 != no more key points*/
	virtual int AddKeyPoint(TnDKeyPoint *);/* add key point to list*/

	//sth to be removed
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPointNo in multimedia class
	virtual int SetKeyPointNumber(int Number);
//--- added by Sylvie Jeannin, August 23, to fill m_KeyPointNo in multimedia class
	virtual int GetKeyPointNumber(void);
//--- added by Sylvie Jeannin, August 22, to fill m_KeyPoint in multimedia class
	virtual int SetKeyPointPointer(T4DKeyPoint *KeyPointPointer);
//--- added by Sylvie Jeannin, August 22, to get m_KeyPoint in multimedia class
	virtual T4DKeyPoint *GetKeyPointPointer(void);

private:
	virtual ~MultiMedia();

	MultiMediaInterface m_Interface;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;


	// all types
	int m_Time;

	// audio
	CAudioFrame m_AudioFrame;
	CAudioFrame * m_pAudioFrame;

	// video
	char *m_VideoFileName;

	// images
	char *m_ImageFileName;
	char *m_ImageComment;
	MomVop *m_Image;
	MomImage *m_OrigAlpha;
	int m_MinAlpha;
	int m_MaxAlpha;

	// key points
	char *m_KeyPointFileName;
	LinkedList *m_KeyPointLists; /* list includeing key poit list
					for each time point */
	int m_KeyPointListNo;        /* number (time) of current
					key point list*/
	KeyPointList *m_KeyPoints;   /* list of key point for one time point*/
	TnDKeyPoint *m_KeyPoint;     /* current key point*/
	int m_KeyPointNo;            /* number of current key point
					at current time*/
	T4DKeyPoint *m_oldKeyPoint;  /* old 4D key point */
	int m_oldKeyPointNo;            /* total number of 4D key points
					(old structure)*/
};

};


extern "C" void extractalpha_pix(TPixel *res,TPixel *in, TCoor x, TCoor y);
extern "C" void DeleteKeyPointList(void *List,void *unused);
extern "C" void DeleteKeyPoint(void *KeyPoint,void *unused);

#endif //__MultiMedia__
