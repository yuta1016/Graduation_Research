///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yong-Sung Kim - Hanyang University, Korea
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  RegionShapeExtraction.h
//
#ifndef __REGIONSHAPEEXTRACTION_H__
#define __REGIONSHAPEEXTRACTION_H__


#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>


//=============================================================================
namespace XM
{

#ifndef M_PI
#define M_PI	3.141592653
#endif

#define HYPOT(x,y)		sqrt((x)*(x)+(y)*(y))

#define	ART_LUT_RADIUS 50		// Zernike basis function radius
#define	ART_LUT_SIZE (ART_LUT_RADIUS*2+1)

// Forward Declarations:
class RegionShapeExtractionTool;


//=============================================================================
class RegionShapeExtractionInterfaceABC: public I_InterfaceABC
{
public:
	RegionShapeExtractionInterfaceABC();
	virtual ~RegionShapeExtractionInterfaceABC() {};

	virtual void destroy() = 0;

//	virtual int SetSourceMedia(MomVop *media) = 0;
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual RegionShapeDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
				    *aRegionShapeDescriptorInterface) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

class RegionShapeExtractionTool;

//=============================================================================
class RegionShapeExtractionInterface: 
  public RegionShapeExtractionInterfaceABC
{
public:
	RegionShapeExtractionInterface(RegionShapeExtractionTool* aTool);
	virtual ~RegionShapeExtractionInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

//	virtual int SetSourceMedia(MomVop *media);
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual RegionShapeDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(RegionShapeDescriptorInterfaceABC
					  *aRegionShapeDescriptorInterface);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	RegionShapeExtractionTool *m_ExtractionTool;
};

//=============================================================================
class RegionShapeExtractionTool: public DescriptorExtractor
{
friend class RegionShapeExtractionInterface;
public:
	RegionShapeExtractionTool();
	RegionShapeExtractionTool(RegionShapeDescriptorInterfaceABC *art);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	//	virtual int SetSourceMedia(MomVop *media);
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual RegionShapeDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(RegionShapeDescriptorInterfaceABC*
					   aRegionShapeDescriptorInterface);
	
	virtual unsigned long StartExtracting(void);

	virtual RegionShapeExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~RegionShapeExtractionTool();

	void Reset();				
	void GenerateBasisLUT();	
	void FindCenterOfMass();	
	void FindRadius();			
	void ExtractCoefficients();	
	void Save();				

	double GetReal(int p, int r, double dx, double dy);
	double GetImg(int p, int r, double dx, double dy);

	RegionShapeExtractionInterface m_Interface;
	RegionShapeDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC *m_MultiMedia;	
	MomVop *m_Media;	

	static bool m_bLUTInit;
	static double m_pBasisR[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// real-value of RegionShape basis function
	static double m_pBasisI[ART_ANGULAR][ART_RADIAL][ART_LUT_SIZE][ART_LUT_SIZE];	// imaginary-value of RegionShape basis function
	
	double m_pCoeffR[ART_ANGULAR][ART_RADIAL];
	double m_pCoeffI[ART_ANGULAR][ART_RADIAL];

	double m_centerX, m_centerY;
	double m_radius;
	int m_mass;
};

};

#endif //__REGIONSHAPEEXTRACTION_H__
