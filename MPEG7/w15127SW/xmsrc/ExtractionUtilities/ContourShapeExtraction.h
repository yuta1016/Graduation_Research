///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric
// ITE-VIL, W.Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeExtraction.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CONTOURSHAPEEXTRACTION_H__
#define __CONTOURSHAPEEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>

//=============================================================================
namespace XM
{


// Forward Declarations:
class ContourShapeExtractionTool;

//=============================================================================
class ContourShapeExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ContourShapeExtractionInterfaceABC();
	virtual ~ContourShapeExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
        virtual ContourShapeDescriptorInterfaceABC *
          GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(ContourShapeDescriptorInterfaceABC
				    *aContourShapeDescriptorInterface) = 0;
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class ContourShapeExtractionInterface: 
  public ContourShapeExtractionInterfaceABC
{
public:
	ContourShapeExtractionInterface(ContourShapeExtractionTool* aTool);
	virtual ~ContourShapeExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
        virtual ContourShapeDescriptorInterfaceABC *
          GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ContourShapeDescriptorInterfaceABC
					  *aContourShapeDescriptorInterface);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ContourShapeExtractionTool *m_ExtractionTool;
};

//=============================================================================
class ContourShapeExtractionTool: public DescriptorExtractor
{
friend class ContourShapeExtractionInterface;
public:
	ContourShapeExtractionTool();
	ContourShapeExtractionTool(ContourShapeDescriptorInterfaceABC
                                   *contourshape);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

/*	virtual unsigned long SetSourceMedia(MediaDescriptor&,
	                                     XMStream& mediaitself);*/
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

/*	virtual unsigned long ExtractFeature(ContourShapeDescriptor* aDesc,
	             I_ExtractionInterfaceABC::ExtractionResult& aResult);*/
        virtual ContourShapeDescriptorInterfaceABC *
          GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ContourShapeDescriptorInterfaceABC
				   * aContourShapeDescriptorInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual ContourShapeExtractionInterfaceABC *GetInterface(void);

/*	"C" void pixeltohistogram(TPixel *res, TPixel *in,TCoor x, TCoor y);*/

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

private:
	virtual ~ContourShapeExtractionTool();

	ContourShapeExtractionInterface m_Interface;
	ContourShapeDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC *m_Media;
	MomVop *m_ImageMedia;

	// Private functions used for extracting the contour shape and the
	// peaks from the contour...

	unsigned long ExtractContour(int n, Point2 * const &coords);
	unsigned long ExtractPeaks(int n, const Point2 * const &coords);

	// Private function to extract the curvature values (eccentricity and
	// circularity) from the contour...

	void ExtractCurvature(int n, const Point2 * const &coords, unsigned long &cir, unsigned long &ecc);
};

};


#endif //__COLORSHAPEEXTRACTION_H__
