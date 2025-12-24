///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation SpaceFrequencyView.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation SpaceFrequencyView tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  SpaceFrequencyViewExtraction.h
//
#ifndef __SPACEFREQUENCYVIEWEXTRACTION_H__
#define __SPACEFREQUENCYVIEWEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"

#include <stdio.h>
#include <math.h>
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
#endif

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>
#include <math.h>

//#ifdef USEXML
//#include "XML4Cutils.h" // sth not allwed here
//#endif

namespace XM
{

enum SFVIEWDECOMPTYPES { 
		SFV_VIEW,			// <--- Abstract
		SFV_SPACEVIEW,
		SFV_FREQUENCYVIEW,
		SFV_SPACEFREQUENCYVIEW,
		SFV_RESOLUTIONVIEW,
		SFV_SPACERESOLUTIONVIEW,
		SFV_VIEWSET,
		SFV_SPACETREE,
		SFV_FREQUENCYTREE,
		SFV_SPACEFREQUENCYGRAPH,
		SFV_VIDEOVIEWGRAPH,
		SFV_MULTIRESOLUTIONPYRAMID
  };


// Forward Declarations:
class SpaceFrequencyViewExtractionTool;

//=============================================================================
class SpaceFrequencyViewExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SpaceFrequencyViewExtractionInterfaceABC();
	virtual ~SpaceFrequencyViewExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	//sth looks like proprietary replacement for SetSourceMedia
	virtual int SetViewParms(const char* imname, const char* ext, 
		int depth, SFVIEWDECOMPTYPES viewtype) = 0;
	
	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long PostExtraction(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SpaceFrequencyViewExtractionInterface: 
  public SpaceFrequencyViewExtractionInterfaceABC
{
public:
	SpaceFrequencyViewExtractionInterface(SpaceFrequencyViewExtractionTool* aTool);
	virtual ~SpaceFrequencyViewExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetViewParms(const char* imname, const char* ext, 
		int depth, SFVIEWDECOMPTYPES viewtype);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	static const UUID myID;
	static const char *myName;

private:
	SpaceFrequencyViewExtractionTool *m_ExtractionTool;
};


//=============================================================================
class SpaceFrequencyViewExtractionTool: public DescriptorExtractor
{
friend class SpaceFrequencyViewExtractionInterface;
public:
	SpaceFrequencyViewExtractionTool();
	SpaceFrequencyViewExtractionTool(GenericDSInterfaceABC *theParent); 

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetViewParms(const char* imname, const char* ext, 
		int depth, SFVIEWDECOMPTYPES viewtype);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	virtual SpaceFrequencyViewExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~SpaceFrequencyViewExtractionTool();
	SpaceFrequencyViewExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

	int m_Depth;
	SFVIEWDECOMPTYPES m_ViewType;
	char* m_ImName;
	char* m_Ext;
	int m_ViewCount;

	// sth the rest  is not printable!!

	GenericDSInterfaceABC *ExtractSpaceFrequencyViews(int iSpaceFrequencyViewID, 
										GenericDSInterfaceABC *parentInterface);

	GenericDSInterfaceABC *ExtractPartition(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h);

	GenericDSInterfaceABC *ExtractPartition(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float t,
									float w, float h, float d);


	GenericDSInterfaceABC *ExtractView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface);


	GenericDSInterfaceABC *ExtractSpaceView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h);

	GenericDSInterfaceABC *ExtractFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h);

	GenericDSInterfaceABC *ExtractFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float t,
									float w, float h, float d);

	GenericDSInterfaceABC *ExtractResolutionView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, float r);

	GenericDSInterfaceABC *ExtractSpaceResolutionView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name, 
									float x, float y, float w, float h, float r);

	GenericDSInterfaceABC *ExtractSpaceFrequencyView(int iSpaceFrequencyViewID, 
									GenericDSInterfaceABC *parentInterface,
									const char* name,
									float sx, float sy, float sw, float sh,
									float fx, float fy, float fw, float fh);
	
	GenericDSInterfaceABC *ExtractSpaceTree(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float x, float y, float w, float h);

	GenericDSInterfaceABC *ExtractFrequencyTree(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float x, float y, float w, float h);

	GenericDSInterfaceABC *ExtractSpaceFrequencyGraph(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float sx, float sy, float sw, float sh,
						  float fx, float fy, float fw, float fh);

	GenericDSInterfaceABC *ExtractMultiResolutionPyramid(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth, float r);

	GenericDSInterfaceABC *ExtractVideoViewGraph(int iSpaceFrequencyViewID, 
						  GenericDSInterfaceABC *parentInterface,
						  const char* name, 
						  int atLevel, int maxDepth,
						  float fx, float fy, float fz,
						  float fw, float fh, float fd);

};

};

#endif //__SPACEFREQUENCYVIEWEXTRACTION_H__
