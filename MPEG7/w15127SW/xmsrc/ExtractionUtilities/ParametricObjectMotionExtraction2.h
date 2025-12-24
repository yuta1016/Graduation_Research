///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden
// (contributing organizations names)
//
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - update according to LaBaule CD-text (duration)
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
// Applicable File Name:  ParametricObjectMotionExtraction2.h
//
#ifndef __PARAMETRICOBJECTMOTIONEXTRACTION2_H__
#define __PARAMETRICOBJECTMOTIONEXTRACTION2_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "momusys.h"
#include "coortrans.h"

#define POM_TRANSLATIONAL  2
#define POM_ROTATIONAL     4
#define POM_AFFINE         6
#define POM_PERSPECTIVE    8
#define POM_QUADRATIC     12
#define POM_MAXPARAM POM_QUADRATIC

//#define INTARITH
#ifdef INTARITH
#define POMHISTBINS 256
#else
#define POMHISTBINS 128
#endif


namespace XM
{


// Forward Declarations:
class ParametricObjectMotionExtractionTool2;

//=============================================================================
class ParametricObjectMotionExtractionInterfaceABC2: public I_InterfaceABC
{
public:
	ParametricObjectMotionExtractionInterfaceABC2();
	virtual ~ParametricObjectMotionExtractionInterfaceABC2() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MomVop *VOP) = 0;
	virtual int SetCurrentSourceMedia(MomVop *VOP) = 0;
	virtual int SetNextSourceMedia(MomVop *VOP) = 0;
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(
                         ParametricObjectMotionDescriptorInterfaceABC
			 *aParametricObjectMotionDescriptorInterface) = 0;

        virtual unsigned long SetOrigins(float *) = 0;
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ParametricObjectMotionExtractionInterface2:
	public ParametricObjectMotionExtractionInterfaceABC2
{
public:
	ParametricObjectMotionExtractionInterface2(
                        ParametricObjectMotionExtractionTool2* aTool);
	virtual ~ParametricObjectMotionExtractionInterface2();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *VOP);
	virtual int SetCurrentSourceMedia(MomVop *VOP);
	virtual int SetNextSourceMedia(MomVop *VOP);
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
			 ParametricObjectMotionDescriptorInterfaceABC
			 *aParametricObjectMotionDescriptorInterface);

        virtual unsigned long SetOrigins(float *);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ParametricObjectMotionExtractionTool2 *m_ExtractionTool;
};

//=============================================================================
class ParametricObjectMotionExtractionTool2: public DescriptorExtractor
{
friend class ParametricObjectMotionExtractionInterface2;
public:
	ParametricObjectMotionExtractionTool2();
	ParametricObjectMotionExtractionTool2(
                             ParametricObjectMotionDescriptorInterfaceABC
			     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *VOP);
	virtual int SetCurrentSourceMedia(MomVop *VOP);
	virtual int SetNextSourceMedia(MomVop *VOP);
	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	     /* frame number must be set AFTER Media was set!!!*/
 
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
                         ParametricObjectMotionDescriptorInterfaceABC
			 *aParametricObjectMotionDescriptorInterface);
	
	virtual unsigned long SetOrigins(float *);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual ParametricObjectMotionExtractionInterfaceABC2
	  *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ParametricObjectMotionExtractionTool2();

	virtual int CreateLevels(int newlevels);
	virtual int DiffEstimateParameters(int motionmodel,
					   int modifyparameters,
					   int usehessian,float *parameters,
					   int origintype,
					   float alphastart,float alphalimit,
					   MomVop *current, MomVop *next,
					   MomVop *transdiff, MomVop *grad,
					   float *origins);

	virtual int invert_hessian(float inverse[POM_MAXPARAM][POM_MAXPARAM],
				   float matrix[POM_MAXPARAM][POM_MAXPARAM],
				   int dimension);
	virtual float try_cholesky(float res[POM_MAXPARAM][POM_MAXPARAM],
				   float matrix[POM_MAXPARAM][POM_MAXPARAM],
				   float delta,int dimension);
	virtual int
	  calc_inverse_udiagonal(float inverse[POM_MAXPARAM][POM_MAXPARAM],
				 float matrix[POM_MAXPARAM][POM_MAXPARAM],
				 int dimension);

	ParametricObjectMotionExtractionInterface2 m_Interface;
	ParametricObjectMotionDescriptorInterfaceABC *m_DescriptorInterface;

	float m_Param[POM_MAXPARAM];
	int m_CurrTime;
	int m_NextTime;
	int m_Duration;			// Aljoscha 12/13/00

	MomVop **m_CurrScale;
        MomVop **m_NextScale;
#ifdef INTARITH
        MomVop **m_Filtered;
#endif
        MomVop **m_Trans;
        MomVop **m_Grad;

        float *m_origins;
	int m_Origintype;
	int m_Resolutions;
	float m_ResolutionsScale;
	float *m_alphastartt;
	float *m_alphalimitt;
	float *m_alphastarta;
	float *m_alphalimita;
	int m_maxiterations;
	int m_param_init_mode;
	float m_tx, m_ty;
	int m_ErrorHist[POMHISTBINS];
};

};


//============================================================================
// pixel processing

//inter:
extern "C" void scalepix(TPixel *,TPixel *, TPixel*);
extern "C" void trans_diff_histpix(TPixel *,TPixel *, TPixel*);
extern "C" void trans_diffpix(TPixel *,TPixel *, TPixel*);

//intra:
extern "C" void abs_pix(TPixel *,TPixel *, TCoor, TCoor);
#ifndef INTARITH
extern "C" void floathistpix(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void floathistrangepix(TPixel *res, TPixel *in, TCoor x, TCoor y);
extern "C" void int_to_float(TPixel *,TPixel *, TCoor, TCoor);
#ifdef DEBUG
#ifndef NODISP
extern "C" void float_to_int(TPixel *,TPixel *, TCoor, TCoor);
#endif
#endif
extern "C" void ninetapfilter(TPixel *,TPixel *, TCoor, TCoor);
#else
extern "C" void antialiaspix(TPixel *,TPixel *, TCoor, TCoor);
#endif
extern "C" void abssumoverdiffpix(TPixel *,TPixel *, TCoor, TCoor);
extern "C" void gradientpix(TPixel *,TPixel *, TCoor, TCoor);

//error gradient for parameter variation:
extern "C" void graderrtrans(TPixel *,TPixel *, TCoor, TCoor);
extern "C" void graderrrot(TPixel *,TPixel *, TCoor, TCoor);
extern "C" void graderraffine(TPixel *,TPixel *, TCoor, TCoor);
extern "C" void graderrpersp(TPixel *,TPixel *, TCoor, TCoor);
extern "C" void graderrparabo(TPixel *,TPixel *, TCoor, TCoor);


#endif //__PARAMETRICOBJECTMOTIONEXTRACTION2_H__
