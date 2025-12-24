///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ShapeVariationExtraction.h
//

#ifndef __SHAPE_SEQUENCE_EXTRACTION_H__
#define __SHAPE_SEQUENCE_EXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

namespace XM
{

#ifndef M_PI
#define M_PI	3.141592653
#endif

#define HYPOT(x,y)		sqrt((x)*(x)+(y)*(y))

#define	LUT_RADIUS 50		// ART basis function radius
#define	LUT_SIZE (LUT_RADIUS*2+1)

// Forward Declarations:
class ShapeVariationExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class ShapeVariationExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ShapeVariationExtractionInterfaceABC();
	virtual ~ShapeVariationExtractionInterfaceABC() {};
	
	virtual void destroy()=0;
	
	// This informs the extractor where the source data will come from
	virtual int SetSourceMedia(MultiMediaInterfaceABC** media, int frame) = 0;
	
	// This is used to inform the top level module where the results
	// are written to
	virtual ShapeVariationDescriptorInterfaceABC*
		GetDescriptorInterface(void) = 0;
	// This informs the extraction were to write the results
	virtual int SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface) = 0;
	
	// This is executed before frame based /time point) extraction, e.g.,
	// reset description or read extraction parameters
	virtual unsigned long InitExtracting(void) = 0;
	// This is the per frame (time point/ period) extraction
	virtual unsigned long StartExtracting(void) = 0;
	// This is executed after the frame based (time point) extraction
	virtual unsigned long PostExtracting(void) = 0;
	
	static const UUID myID;
};

class ShapeVariationxtractionTool;

//=============================================================================
class ShapeVariationExtractionInterface: 
public ShapeVariationExtractionInterfaceABC
{
public:
	// used when DummyExtraction tool is created
	ShapeVariationExtractionInterface(ShapeVariationExtractionTool* aTool);
	// desctructor
	virtual ~ShapeVariationExtractionInterface();
	
	// UUID for this extraction class
	virtual const UUID& GetInterfaceID(void);
	// for checking for right element type
	virtual const char *GetName(void);
	
	// destructs ShapeVariationExtractionTool
	virtual void destroy();
	
	virtual int SetSourceMedia(MultiMediaInterfaceABC** media, int frame);
	
	virtual ShapeVariationDescriptorInterfaceABC*
		GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);
	
	static const UUID myID;
	static const char *myName;
	
private:
	ShapeVariationExtractionTool *m_ExtractionTool;
};
//=============================================================================
class ShapeVariationExtractionTool: public DescriptorExtractor
{
friend class ShapeVariationExtractionInterface;
public:
	// Null constructor
	ShapeVariationExtractionTool();
	
	// Also connects the Descriptor (result memnory) to the extraction
	// If set to "0" it automatically creates the descriptor
	ShapeVariationExtractionTool(ShapeVariationDescriptorInterfaceABC
		*ShapeVariation);
	
	// ID of object type
	virtual const UUID& GetObjectID(void);
	// Object type name
	virtual const char *GetName(void);
	
	// Don't know what this does
	virtual bool SupportsPush(void);
	// Don't know what this does
	virtual bool SupportsPull(void);
	
	// This informs the extractor where the source data comes from
	virtual int SetSourceMedia(MultiMediaInterfaceABC** media, int frame);
	
	virtual ShapeVariationDescriptorInterfaceABC*
		GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);
	
	// Extraction object must no be used, only its interface is allowd to
	// to be used. This function is to get the interface
	virtual ShapeVariationExtractionInterfaceABC *GetInterface(void);
	
	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	// Destructor is private to allow creation of
	// object only by using "new"
	virtual ~ShapeVariationExtractionTool();

	void GenerateBasisLUT();
	void Init();
	void Reset();
	void MakeLVM();
	void MakeHVM();
	void AddFrame();
	void NormalizeMap();
	void FindCenter();
	void ExtractLVMCoefficients();
	void ExtractHVMCoefficients();
	void ExtractSTDCoefficients();
	void ExtractBinaryCoefficients();
	void Save();

	double GetReal(int p, int r, double dx, double dy);
	double GetImg(int p, int r, double dx, double dy);
	
	ShapeVariationExtractionInterface m_Interface;
	ShapeVariationDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC** m_Media;
	MultiMediaInterfaceABC* m_currentMedia;
	unsigned long m_FrameCnt;
	
	static bool m_bLUTInit;
	static double m_BasisR[ANGULAR][RADIAL][LUT_SIZE][LUT_SIZE];	// real-value of RegionShape basis function
	static double m_BasisI[ANGULAR][RADIAL][LUT_SIZE][LUT_SIZE];	// imaginary-value of RegionShape basis function
	
	double m_LVMCoeffR[ANGULAR][RADIAL];
	double m_LVMCoeffI[ANGULAR][RADIAL];
	double m_HVMCoeffR[ANGULAR][RADIAL];
	double m_HVMCoeffI[ANGULAR][RADIAL];
	double m_STDCoeffR[ANGULAR][RADIAL];
	double m_STDCoeffI[ANGULAR][RADIAL];
	double m_STDCoeff[ANGULAR][RADIAL];

	double m_centerX, m_centerY;
	double m_radius;
	int m_lmass, m_hmass, m_mass;
	
	int	m_mapWidth;
	int m_mapHeight;
	short**	m_pMap;

}; // End class
	
}; // End namespace


#endif // __SHAPE_SEQUENCE_EXTRACTION_H__
