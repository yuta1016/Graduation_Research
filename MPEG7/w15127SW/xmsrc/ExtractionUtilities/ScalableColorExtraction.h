///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai
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
// Applicable File Name:  ScalableColorExtraction.h
//
#ifndef __SCALCOLOREXTRACTION_H__
#define __SCALCOLOREXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "momusys.h"
#include "intra.h"
#include "Media/Media.h"

//=============================================================================
//Macro for extraction process



namespace XM
{


// Forward Declarations:
class ScalableColorExtractionTool;
class GoFGoPColorExtractionTool;

//=============================================================================
class ScalableColorExtractionInterfaceABC: public I_InterfaceABC
{
friend class GoFGoPColorExtractionTool;
public:
	ScalableColorExtractionInterfaceABC();
	virtual ~ScalableColorExtractionInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual ScalableColorDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(ScalableColorDescriptorInterfaceABC
				    *aScalableColorDescriptorInterface) = 0;
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long PostExtracting(void) = 0;

	static const UUID myID;

private:
	/*parts of StartExtracting for usage in GoFGoPColor*/
	virtual unsigned long *ComputeNormalizedHistogram()=0;
	virtual unsigned long *QuantizeHistogram(unsigned long *aHist)=0;
	virtual unsigned long *HaarTransform(unsigned long *aHist)=0;
};

//=============================================================================
class ScalableColorExtractionInterface: 
  public ScalableColorExtractionInterfaceABC
{
public:
	ScalableColorExtractionInterface(ScalableColorExtractionTool* aTool);
	virtual ~ScalableColorExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual ScalableColorDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ScalableColorDescriptorInterfaceABC
					  *aScalableColorDescriptorInterface);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ScalableColorExtractionTool *m_ExtractionTool;

	/*parts of StartExtracting for usage in GoFGoPColor*/
	virtual unsigned long *ComputeNormalizedHistogram();
	virtual unsigned long *QuantizeHistogram(unsigned long *aHist);
	virtual unsigned long *HaarTransform(unsigned long *aHist);
};

//=============================================================================
class ScalableColorExtractionTool: public DescriptorExtractor
{
friend class ScalableColorExtractionInterface;
public:
	ScalableColorExtractionTool();
	ScalableColorExtractionTool(ScalableColorDescriptorInterfaceABC
				     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

/*	virtual unsigned long SetSourceMedia(MediaDescriptor&,
	                                     XMStream& mediaitself);*/
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

/*	virtual unsigned long ExtractFeature(ScalableColorDescriptor* aDesc,
	             I_ExtractionInterfaceABC::ExtractionResult& aResult);*/
	virtual ScalableColorDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ScalableColorDescriptorInterfaceABC
				   * aScalableColorDescriptorInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);

	virtual ScalableColorExtractionInterfaceABC *GetInterface(void);

/*	"C" void pixeltocolorhist(TPixel *res, TPixel *in,TCoor x, TCoor y);*/

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ScalableColorExtractionTool();

	ScalableColorExtractionInterface m_Interface;
	ScalableColorDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC *m_Media;

	/*parts of StartExtracting for usage in GoFGoPColor*/
	virtual unsigned long *ComputeNormalizedHistogram();
	virtual unsigned long *QuantizeHistogram(unsigned long *aHist);
	virtual unsigned long *HaarTransform(unsigned long *aHist);
};

};

extern "C" void pixeltohistogram1(TPixel *res, TPixel *in,TCoor x, TCoor y);
extern "C" void pixeltocolorhist1(TPixel *res, TPixel *in,TCoor x, TCoor y);
//void pixeltocolorhist1(TPixel *res, TPixel *in,TCoor x, TCoor y);

#endif //__SCALCOLOREXTRACTION_H__
