///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatioTemporalLocatorExtraction.h
//
#ifndef __SpatioTemporalLocatorEXTRACTION_H__
#define __SpatioTemporalLocatorEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>


#include "spatio_temporal_locator_extraction.h"									 

//#ifndef MAX
//#define MAX __max
//#endif


//=============================================================================
//Macro for extraction process

namespace XM
{

// Forward Declarations:
class SpatioTemporalLocatoroExtractionTool;

//=============================================================================
class SpatioTemporalLocatorExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SpatioTemporalLocatorExtractionInterfaceABC();
	virtual ~SpatioTemporalLocatorExtractionInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media) = 0;
	virtual SpatioTemporalLocatorDescriptorInterfaceABC 
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(
                              SpatioTemporalLocatorDescriptorInterfaceABC
			      * aSpatioTemporalLocatorDescriptorInterface) = 0;

	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long PostExtraction(void) = 0;


	static const UUID myID;
};

class SpatioTemporalLocatorExtractionTool;

//=============================================================================
class SpatioTemporalLocatorExtractionInterface: 
  public SpatioTemporalLocatorExtractionInterfaceABC
{
public:
	SpatioTemporalLocatorExtractionInterface(SpatioTemporalLocatorExtractionTool* aTool);
	virtual ~SpatioTemporalLocatorExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);
	virtual SpatioTemporalLocatorDescriptorInterfaceABC 
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
                                SpatioTemporalLocatorDescriptorInterfaceABC
				* aSpatioTemporalLocatorDescriptorInterface);

	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	static const UUID myID;
	static const char *myName;

private:
	SpatioTemporalLocatorExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SpatioTemporalLocatorExtractionTool: public DescriptorExtractor
{
friend class SpatioTemporalLocatorExtractionInterface;
public:
	SpatioTemporalLocatorExtractionTool();
	SpatioTemporalLocatorExtractionTool(SpatioTemporalLocatorDescriptorInterfaceABC *SpatioTemporalLocator);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

/*	virtual unsigned long SetSourceMedia(MediaDescriptor&,
	                                     XMStream& mediaitself);*/
	virtual int SetSourceMedia(MultiMediaInterfaceABC *media);

/*	virtual unsigned long ExtractFeature(SpatioTemporalLocatorDescriptor* aDesc,
	             I_ExtractionInterfaceABC::ExtractionResult& aResult);*/
	virtual SpatioTemporalLocatorDescriptorInterfaceABC 
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
                                SpatioTemporalLocatorDescriptorInterfaceABC
				* aSpatioTemporalLocatorDescriptorInterface);
	
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	virtual SpatioTemporalLocatorExtractionInterfaceABC *GetInterface(void);


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~SpatioTemporalLocatorExtractionTool(); 

	SpatioTemporalLocatorExtractionInterface m_Interface;
	SpatioTemporalLocatorDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC *m_Media;

	CPolygon *Pmedia, Pmedia_back;
	CPolygons Pdb;
	CExtractMP ExtractMP;
	int m_FP, m_sf, m_ef;
	int m_counter;

	TemporalInterpolationExtractionTool *TIEx[MAXNRefrence*MAXNVertices+2];
	TemporalInterpolationExtractionInterfaceABC	  *TIExI;
};

};



#endif //__SpatioTemporalLocatorEXTRACTION_H__
