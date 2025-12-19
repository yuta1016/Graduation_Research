////////////////////////////////////////////////////////////////////////
//
// ColorStructureExtraction.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#ifndef __COLORSTRUCTUREEXTRACTION_H__
#define __COLORSTRUCTUREEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"      /* Naming Convention */

#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

namespace XM
{

// Forward Declarations:
class ColorStructureExtractionTool;

//=============================================================================
// This is an abstract base class to provide an interface to the extraction
// utilities.
class ColorStructureExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ColorStructureExtractionInterfaceABC();
	virtual ~ColorStructureExtractionInterfaceABC() {};

	virtual void destroy()=0;

	// This informs the extractor where the source data will come from
	virtual int SetSourceMedia(MultiMediaInterfaceABC* media) = 0;

	// This is used to inform the top level module where the results
	// are written to
	virtual ColorStructureDescriptorInterfaceABC*
		GetDescriptorInterface(void) = 0;
	// This informs the extraction were to write the results
	virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
					 *aColorStructureDescriptorInterface) = 0;

	// This is executed before frame based /time point) extraction, e.g.,
	// reset description or read extraction parameters
	virtual unsigned long InitExtracting(void) = 0;
	// This is the per frame (time point/ period) extraction
	virtual unsigned long StartExtracting(void) = 0;
	// This is executed after the frame based (time point) extraction
	virtual unsigned long PostExtracting(void) = 0;

	virtual int DownQuantHMMD(int nNewSize) = 0;

	static const UUID myID;
};

//=============================================================================
class ColorStructureExtractionInterface: 
public ColorStructureExtractionInterfaceABC
{
public:
	// used when DummyExtraction tool is created
	ColorStructureExtractionInterface(ColorStructureExtractionTool* aTool);
	// desctructor
	virtual ~ColorStructureExtractionInterface();

	// UUID for this extraction class
	virtual const UUID& GetInterfaceID(void);
	// for checking for right element type
	virtual const char *GetName(void);

	// destructs ColorStructureExtractionTool
	virtual void destroy();

	virtual int SetSourceMedia(MultiMediaInterfaceABC* media);

	virtual ColorStructureDescriptorInterfaceABC*
		GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
					 *aColorStructureDescriptorInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);

	virtual int DownQuantHMMD(int nNewSize);

	static const UUID myID;
	static const char *myName;

private:
	ColorStructureExtractionTool *m_ExtractionTool;

};
//=============================================================================
class ColorStructureExtractionTool: public DescriptorExtractor
{
friend class ColorStructureExtractionInterface;
public:
	// Null constructor
	ColorStructureExtractionTool();

	// Also connects the Descriptor (result memnory) to the extraction
	// If set to "0" it automatically creates the descriptor
	ColorStructureExtractionTool(ColorStructureDescriptorInterfaceABC
				*ColorStructure);

	// ID of object type
	virtual const UUID& GetObjectID(void);
	// Object type name
	virtual const char *GetName(void);

	// Don't know what this does
	virtual bool SupportsPush(void);
	// Don't know what this does
	virtual bool SupportsPull(void);

	// This informs the extractor where the source data comes from
	virtual int SetSourceMedia(MultiMediaInterfaceABC* media);

	virtual ColorStructureDescriptorInterfaceABC*
	GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(ColorStructureDescriptorInterfaceABC
					 *aColorStructureDescriptorInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtracting(void);

	// Extraction object must no be used, only its interface is allowd to
	// to be used. This function is to get the interface
	virtual ColorStructureExtractionInterfaceABC *GetInterface(void);

	virtual int DownQuantHMMD(int nNewSize);

	static int BuildColorQuantTable(int iColorQuantSpace = 3);

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
	virtual ~ColorStructureExtractionTool();

	static int GetColorQuantSpace(int size);
	static int GetBinSize(int iColorQuantSpace);

	// Single pixel/bin operations
	static void RGB2HMMD(int R, int G, int B, int &H, int &S, int &D);
	static int QuantHMMD(int H, int S, int D, int iColorQuantSpace);
	static int TransformBinIndex(int iOrig, int iOrigColorQuantSpace, 
											int iNewColorQuantSpace);

	// Full descriptor operations
	int QuantAmplNonLinear(unsigned long OldNorm);
	int ConvertAmplLinear(unsigned long NewNorm);
	int UnifyBins(unsigned long Norm, int nNewSize);

	// Three ways to transform color quant spaces:
	// 1) hard-coded transform table (define PRINT_TRANSFORM_TABLE to generate)
	// 2) build transform table from colorspace table dynamically
	// 3) build transform table using QuantHMMD() repeatedly (very inefficient)
	static int BuildTransformTable(int iOrigColorQuantSpace, int nNewSize);

	enum { NUM_COLOR_QUANT_SPACE = 4, 
				 MAX_SUB_SPACE = 5, 
				 BASE_QUANT_SPACE = 256,
				 BASE_QUANT_SPACE_INDEX = 3 };

	static const int diffThresh[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE+1];
	static const int nHueLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static const int nSumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static const int nCumLevels[NUM_COLOR_QUANT_SPACE][MAX_SUB_SPACE];
	static unsigned char *colorQuantTable[NUM_COLOR_QUANT_SPACE];
	static unsigned char *colorQuantTransform[NUM_COLOR_QUANT_SPACE]
											 [NUM_COLOR_QUANT_SPACE];

	static const double amplThresh[];
	static const int nAmplLevels[];

	ColorStructureExtractionInterface m_Interface;
	ColorStructureDescriptorInterfaceABC *m_DescriptorInterface;
	MultiMediaInterfaceABC* m_Media;
}; // End class

}; // End namespace


#endif //__COLORSTRUCTUREEXTRACTION_H__
