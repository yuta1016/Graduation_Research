///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
//  Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// Applicable File Name:  MosaicExtraction.h
//
#ifndef __MOSAICEXTRACTION_H__
#define __MOSAICEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "coortrans.h"

#define POM_TRANSLATIONAL  2
#define POM_ROTATIONAL     4
#define POM_AFFINE         6
#define POM_PERSPECTIVE    8
#define POM_QUADRATIC     12

#define POM_MAXPARAM POM_QUADRATIC

namespace XM
{


// Forward Declarations:
class MosaicExtractionTool;

//=============================================================================
class MosaicExtractionInterfaceABC: public I_InterfaceABC
{
public:
	MosaicExtractionInterfaceABC();
	virtual ~MosaicExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *Media) = 0;
	//virtual int SetSourceMediaFrameNumber(unsigned long FrameNo) = 0;
	virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *ParentDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long PostExtracting(void) = 0;
	virtual MomVop* GetMosaic(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MosaicExtractionInterface:
	public MosaicExtractionInterfaceABC
{
public:
	MosaicExtractionInterface(
                        MosaicExtractionTool* aTool);
	virtual ~MosaicExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *Media);
	//	virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	virtual int SetDescriptorInterface(
			 GenericDSInterfaceABC
			 *ParentDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);
	virtual MomVop* GetMosaic(void);

	static const UUID myID;
	static const char *myName;

private:
	MosaicExtractionTool *m_ExtractionTool;
};

//=============================================================================
class MosaicExtractionTool: public DescriptorExtractor
{
friend class MosaicExtractionInterface;
public:
	MosaicExtractionTool();
	MosaicExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *Media);
	//virtual int SetSourceMediaFrameNumber(unsigned long FrameNo);
	     /* frame number must be set AFTER Media was set!!!*/
 
	virtual int SetDescriptorInterface(
                         GenericDSInterfaceABC
			 *ParentDSInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);
	virtual MomVop* GetMosaic(void);

	virtual MosaicExtractionInterfaceABC
	  *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~MosaicExtractionTool();

	MosaicExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptorInterface;

	/* extarction parameters*/
	int m_CreateMosaic;
	int m_MosaicX;
	int m_MosaicY;
	int m_SetBackground;
	int m_MosaicType;
	int m_Origiontype;
	int m_NoOfParams;
	int m_nnfraction;
	int m_nn;
	int m_SkipBoundary;

	/* internal variables*/
	GenericDS m_MosaicD;
	GenericDSInterfaceABC *m_MosaicInterface;
	ParametricObjectMotionDescriptorInterfaceABC *m_MotionD;
	ParametricObjectMotionExtractionInterfaceABC2 *m_MotionExtraction;
	int m_CurrTime;
	int m_NextTime;
	float m_Param[12];

	MomVop *m_Mosaic;
        MomVop *m_Next;
	int m_NextXSize;
	int m_NextYSize;

	int m_Sets;
	float m_Origins[4];
	int m_OffsetX;
	int m_OffsetY;
};
};

//============================================================================
// defines for image processing

#define copy_image(outvop,invop,neworiginx,neworiginy) \
            intra_proc(outvop,invop,0,\
                       Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
                       CON_0,SCAN_HO, \
                       -1,-1,-1,-1,-1, \
                       invop->width,invop->height,neworiginx,neworiginy,0,0, \
		       size1to1,cp_apix);
#define getboundingbox(invop,maxx,minx,maxy,miny) \
            _intra_regout1=0; \
            _intra_regout2=invop->width; \
            _intra_regout3=0; \
            _intra_regout4=invop->height; \
            intra_proc(0,invop,0,\
                       0,Y_CH|U_CH|V_CH|A_CH, \
                       CON_0,SCAN_HO, \
                       -1,-1,-1,-1,-1, \
                       -1,-1,0,0,0,0, \
		       size1to1,bb_apix); \
            (*(maxx))=_intra_regout1; \
            (*(minx))=_intra_regout2; \
            (*(maxy))=_intra_regout3; \
            (*(miny))=_intra_regout4;
#define crop_image(outvop,invop,maxx,minx,maxy,miny) \
            intra_proc(outvop,invop,0,\
                       Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
                       CON_0,SCAN_HO, \
                       -1,-1,-1,-1,-1, \
                       maxx-minx,maxy-miny,0,0,minx,miny, \
		       size1to1,cp_apix);
#define erosion_skip_boundary(outvop,invop,skipwidth) \
            _intra_regin1=skipwidth; \
            _intra_regin2=invop->width-skipwidth; \
            _intra_regin3=invop->height-skipwidth; \
            intra_proc(outvop,invop,0,\
                       Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
                       CON_8,SCAN_HO, \
                       -1,-1,-1,-1,-1, \
                       -1,-1,0,0,0,0, \
		       size1to1,aclean_pix);

#define trans_past(outvop,in2vop,transformation) \
            intertrans_proc(outvop,outvop,in2vop,\
                            Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
			    Y_CH|U_CH|V_CH|A_CH, \
			    SCAN_HO|TRANS_INT_INTERPOLATE, \
                            -1,-1,0,0,0,0, \
                            transformation,&trans_pastpix);
#define trans_extend(outvop,in2vop,transformation) \
            intertrans_proc(outvop,outvop,in2vop,\
                            Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
			    Y_CH|U_CH|V_CH|A_CH, \
			    SCAN_HO|TRANS_INT_INTERPOLATE, \
                            -1,-1,0,0,0,0, \
                            transformation,&trans_extendpix);
#define trans_merge(outvop,in2vop,transformation) \
            intertrans_proc(outvop,outvop,in2vop,\
                            Y_CH|U_CH|V_CH|A_CH,Y_CH|U_CH|V_CH|A_CH, \
			    Y_CH|U_CH|V_CH|A_CH, \
			    SCAN_HO|TRANS_INT_INTERPOLATE, \
                            -1,-1,0,0,0,0, \
                            transformation,&trans_mergepix);


//============================================================================
// pixel processing

//inter:
extern "C" void cp_apix(TPixel *,TPixel *, TCoor x, TCoor y);
extern "C" void bb_apix(TPixel *,TPixel *, TCoor x, TCoor y);
extern "C" void aclean_pix(TPixel *,TPixel *, TCoor x, TCoor y);
extern "C" void trans_pastpix(TPixel *,TPixel *, TPixel*);
extern "C" void trans_extendpix(TPixel *,TPixel *, TPixel*);
extern "C" void trans_mergepix(TPixel *,TPixel *, TPixel*);


#endif //__MOSAICEXTRACTION2_H__
