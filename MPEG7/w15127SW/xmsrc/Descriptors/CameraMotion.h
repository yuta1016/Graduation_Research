///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// Applicable File Name:  CameraMotion.h
#ifndef __CameraMotion__
#define __CameraMotion__

#define NB_CAMERA_OPERATIONS 15
// This represents the different possible camera operations, namely :  
// Track left, Track right, Boom down, Boom up, Dolly forward, Dolly backward, Pan left, Pan right
// Tilt up, Tilt down, Roll clockwise, Roll anti-clockwise, Zoom in, Zoom out, Fixed

#define MIXTURE_MODE true 
#define NON_MIXTURE_MODE false 
// Binary representation semantics

typedef double FractionalPresence_t[NB_CAMERA_OPERATIONS] ;
typedef double AmountOfMotion_t[NB_CAMERA_OPERATIONS-1] ;

typedef enum {
	TRACK_LEFT = 0,
	TRACK_RIGHT = 1,
	BOOM_DOWN = 2,
	BOOM_UP = 3,
	DOLLY_FORWARD = 4,
	DOLLY_BACKWARD = 5,
	PAN_LEFT = 6,
	PAN_RIGHT = 7,
	TILT_UP = 8,
	TILT_DOWN = 9,
	ROLL_CLOCKWISE = 10,
	ROLL_ANTICLOCKWISE = 11,
	ZOOM_IN = 12,
	ZOOM_OUT = 13,
	FIXED = 14
} CMType;

typedef struct {
	long startTime ;
	long duration ;
	FractionalPresence_t presence ;
	AmountOfMotion_t speeds ;
	bool FocusOfExpansionFlag ;
	double x_FoeFoc ;
	double y_FoeFoc ;
} SegmentedCameraMotion_t ;

namespace XM
{

//=============================================================================
class CameraMotionDescriptorInterfaceABC: public I_InterfaceABC
{
public:
	CameraMotionDescriptorInterfaceABC();
	virtual ~CameraMotionDescriptorInterfaceABC() {}
   
	virtual void addref()=0;
	virtual void release()=0;

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent)=0;		//0719-Sylvie
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription)=0;	//0719-Sylvie

	virtual long GetNumSegments(void) = 0;
	virtual unsigned long SetNumSegments(long) = 0;
	virtual bool GetDescriptionMode(void) = 0;
	virtual unsigned long SetDescriptionMode(bool) = 0;
	virtual SegmentedCameraMotion_t * GetCameraMotionInfo(void) = 0 ;
	virtual SegmentedCameraMotion_t GetCameraMotionSegmentInfo(long) = 0 ;
	virtual unsigned long SetCameraMotionSegmentInfo(SegmentedCameraMotion_t,long) = 0 ;
	virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t ) = 0 ;
	virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t, double, double ) = 0;

	static const UUID myID;

};

class CameraMotionDescriptor;

//=============================================================================
class CameraMotionDescriptorInterface: public CameraMotionDescriptorInterfaceABC
{
public:
	CameraMotionDescriptorInterface(CameraMotionDescriptor* aCameraMotion );
	virtual ~CameraMotionDescriptorInterface();
   
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void addref();
	virtual void release();

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);		//0719-Sylvie
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);	//0719-Sylvie

	virtual long GetNumSegments(void) ;
	virtual unsigned long SetNumSegments(long) ;
	virtual bool GetDescriptionMode(void) ;
	virtual unsigned long SetDescriptionMode(bool) ;
	virtual SegmentedCameraMotion_t * GetCameraMotionInfo(void) ;
	virtual SegmentedCameraMotion_t GetCameraMotionSegmentInfo(long) ;
	virtual unsigned long SetCameraMotionSegmentInfo(SegmentedCameraMotion_t,long) ;
	virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t ) ;
    virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t, double, double ) ;

	static const UUID myID;
	static const char *myName;


private:
	CameraMotionDescriptor *m_CameraMotionDescriptor;
};

//=============================================================================
class CameraMotionDescriptor: public Descriptor
{

	friend class CameraMotionDescriptorInterface;

public:
	CameraMotionDescriptor();

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

	virtual CameraMotionDescriptorInterfaceABC *GetInterface(void);

	virtual unsigned long ExportDDL(GenericDSInterfaceABC *theParent);		//0719-Sylvie
	virtual unsigned long ImportDDL(GenericDSInterfaceABC *theDescription);	//0719-Sylvie

	virtual void addref();
	virtual void release();

	// Accesssor methods
	virtual long GetNumSegments(void) ;
	virtual unsigned long SetNumSegments(long) ;
	virtual bool GetDescriptionMode(void) ;
	virtual unsigned long SetDescriptionMode(bool) ;
	virtual SegmentedCameraMotion_t * GetCameraMotionInfo(void) ;
	virtual SegmentedCameraMotion_t GetCameraMotionSegmentInfo(long) ;
	virtual unsigned long SetCameraMotionSegmentInfo(SegmentedCameraMotion_t,long) ;
	virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t ) ;
   virtual SegmentedCameraMotion_t CreateCameraMotionInfo(long, long, FractionalPresence_t, AmountOfMotion_t, double, double ) ;

private:
	virtual ~CameraMotionDescriptor();

	CameraMotionDescriptorInterface m_Interface;

	unsigned long m_refcount;

	const bool m_isProprietary;
	static const char * valName;
	static const UUID valID;

	// actual values:
	long  m_NumSegments;      // number of elementary temporal segments being described through the union operation
	bool m_DescriptionMode;  // 0 for non-mixture mode, 1 for mixture mode
	
	SegmentedCameraMotion_t *m_CameraMotionInfo ; // Actual description, which consists of NumSegments SegmentedCameraMotion structures

	// Copied from SpatioTemporalLocator
	unsigned long  ExtractHours(unsigned long second);
	unsigned long  ExtractMinutes(unsigned long second);
	unsigned long  ExtractSeconds(unsigned long second);

};

};

#endif //__CameraMotion__
