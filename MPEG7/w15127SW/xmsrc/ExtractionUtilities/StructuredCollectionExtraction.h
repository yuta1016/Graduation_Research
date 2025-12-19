///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
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
// Applicable File Name:  StructuredCollectionExtraction.h
//
#ifndef __StructuredCollectionEXTRACTION_H__
#define __StructuredCollectionEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "ExtractionUtilities/ExtractionUtilities.h"
#include "momusys.h"
#include "intra.h"

#include <stdio.h>
#include <math.h>
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
#endif

#include <iostream>
#include <fstream>
#include <math.h>


//=============================================================================

namespace XM
{

// Forward Declarations:
class StructuredCollectionExtractionTool;

//=============================================================================
class StructuredCollectionExtractionInterfaceABC: public I_InterfaceABC
{
public:
	StructuredCollectionExtractionInterfaceABC();
	virtual ~StructuredCollectionExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(char *pcClassName, char *pcImageName) = 0;
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class StructuredCollectionExtractionInterface: 
  public StructuredCollectionExtractionInterfaceABC
{
public:
	StructuredCollectionExtractionInterface(StructuredCollectionExtractionTool* aTool);
	virtual ~StructuredCollectionExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(char *pcClassName, char *pcImageName);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	StructuredCollectionExtractionTool *m_ExtractionTool;
};

//=============================================================================
class StructuredCollectionExtractionTool: public DescriptorExtractor
{
friend class StructuredCollectionExtractionInterface;
public:
	StructuredCollectionExtractionTool();
	StructuredCollectionExtractionTool(GenericDSInterfaceABC *theParent); 

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(char *pcClassName, char *pcImageName);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual StructuredCollectionExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~StructuredCollectionExtractionTool();

	StructuredCollectionExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

	ScalableColorDescriptorInterfaceABC *m_ScalableColorDescriptor;
	ScalableColorExtractionInterfaceABC *m_ScalableColorExtraction;
	MultiMedia *m_Media;
	MultiMediaInterfaceABC *m_MediaInterface;
	MediaDB m_RamDB;

	char *m_pcClassName;
	char *m_pcImageName;

	// Constant variables
	// A more elegant solution involving the 
	// use of dinamic memory might be found later.
	static const int	STRING_MAXSIZE;		// maximum size of the strings

	// Extract StructuredCollection 
	void ExtractStructuredCollection(GenericDSInterfaceABC *parentDSInterface);

	// Read file with list of classes for collection
	int ReadClassFile(int *piNumClasses, int **ppiNumImgsPerClass, 
		char ***pppcClassLabel, char ****ppppcImagesPerClass);

	// Generate mean/variance scalable color for a class
	int ScalableColorStatisticsExtraction(
		int iNumClasses, int *piNumImgsPerClass, char ***pppcImagesPerClass, 
		VectorR **ppvrMeanSclColPerClass, VectorR **ppvrVarSclColPerClass);//sth maybe this is the GoFGoP

	// Extract scalable color for an image
	int ScalableColorExtraction(char *image, VectorR *pvrSclCol);

	// Encode StructuredCollection DS
	int StructuredCollectionEncoding(GenericDSInterfaceABC *pDSInt,
		int iNumClasses, int *piNumImgsPerClass, 
		char **ppcClassLabel, char ***pppcImagesPerClass,
		VectorR *pvrMeanSclColPerClass, VectorR *pvrVarSclColPerClass);

};

};

#endif //__StructuredCollectionEXTRACTION_H__
