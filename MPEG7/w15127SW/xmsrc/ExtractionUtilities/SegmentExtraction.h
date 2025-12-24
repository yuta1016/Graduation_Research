///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Javier Ruiz Hidalgo - University Politechnic of Catalonia
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SegmentExtraction.h
//
#ifndef __SEGMENTEXTRACTION_H__
#define __SEGMENTEXTRACTION_H__
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

#include <iostream.h>
#include <fstream.h>
#include <math.h>

#ifdef USEXML
#include "dom/DOM.hpp"
#endif
#include "ParametricObjectMotionUtilities.h"

namespace XM
{

// Forward Declarations:
class SegmentExtractionTool;

//=============================================================================
class SegmentExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SegmentExtractionInterfaceABC();
	virtual ~SegmentExtractionInterfaceABC() {};

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, char *pcMasksName) = 0;
	virtual int SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class SegmentExtractionInterface: 
  public SegmentExtractionInterfaceABC
{
public:
	SegmentExtractionInterface(SegmentExtractionTool* aTool);
	virtual ~SegmentExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, char *pcMasksName);
	virtual int SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	SegmentExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SegmentExtractionTool: public DescriptorExtractor
{
public:
	SegmentExtractionTool();
	virtual ~SegmentExtractionTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp, char *pcFramesName, char *pcMasksName);

	virtual int SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual SegmentExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	SegmentExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DSInterface;

	char	*m_pcFramesName;		// Name of the frames on disk (f_%frameID.ppm)
	char	*m_pcMasksName;			// Name of the masks on disk (m_%maskID_%frameID.ras)
	MomVop	*m_mvFS;				// FS file
	FILE	*m_fp;					// tree.txt file

	// Constant variables

	// Constant for strings. A more elegant solution involving the
	// use of dinamic memory might be found later.
	static const int	STRING_MAXSIZE;		// maximum size of the strings


	// RecursiveParse: Extract Descriptors and creates the DOM Tree for a
	// given segment
#ifdef USEXML
	void RecursiveParse(int iSegmentID, DOM_Element &pdomNode);
#endif	
	//ReadSegmentID: Reads a segment info form a tree.txt
	int ReadSegmentInfoTree( FILE *fp, int iSegmentID, char *pcSegmentType, 
		int*piSegmentNumChildren, int **ppiSegmentChildIDList, 
		char *pcDecompType, char *pcOverlap, char *pcGap, char *pcSegmentAnno);
	
	// ReadSegmentInfo: Reads media FS file for info 
	void ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, char *pcSegmentType, 
		int *piSegmentNumFrames, int **ppiSegmentFramesList, int **ppiSegmentMasksList);

	// Descriptor extraction for different types of Segment DSs
#ifdef USEXML
	void ExtractionOfVideoSegmentDescriptors (DOM_Element &domElement,
						  int id, int nframes, 
						  int *frames, char* path);

	void ExtractionOfMovingRegionDescriptors(DOM_Element &domElement,
						 int id, int nframes, 
						 int *frames, int *masks,
						 char *framespath,
						 char *maskspath);
	void ExtractionOfStillRegionDescriptors(DOM_Element &domElement,
						int id, int frame, int mask,
						char *framesname,
						char *masksname);

	// Functions that implement the extraction (and population of DOMtree) of specific Descriptors
	// They should be compared with the corresponding server Applications
	void GoFGoPColorHistogramDescriptorDOMExtraction(int iNFrames,
						      int *piFramesList, 
						      int *piMasksList,
						      char *pcFramesName,
						      char *pcMasksName,
						      DOM_Element &domElement);
	void ColorHistogramDescriptorDOMExtraction(MomVop *media,
						   DOM_Element &domElement);
	void ContourShapeDescriptorDOMExtraction(MomVop *media,
						 DOM_Element &domElement);

	void ParametricObjectMotionDescriptorDOMExtraction(
		int nframes,int *frames,int *masks,char *framespath,
		char *maskspath, DOM_Element &domElement);


	// Functions that implement the DOM CodingScheme for each Descriptor.
	// This should be moved into specific DOMCodingSchemes classes for Descriptors
	// (like the actual binary Coding Schemes implemented).
	DOM_Element GoFGoPColorDescriptorDOMElement(DOM_Document doc, 
		GoFGoPColorDescriptorInterfaceABC *theGoFGoPColorDescriptorInterface);//vin
//	DOM_Element ColorHistogramDescriptorDOMElement(DOM_Document doc, 
//		ColorHistogramDescriptorInterfaceABC *theColorHistogramDescriptorInterface);//vin
	DOM_Element ColorSpaceDescriptorDOMElement(DOM_Document doc, 
		ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface);
	DOM_Element ColorQuantizerDescriptorDOMElement(DOM_Document doc, 
		ColorQuantizerDescriptorInterfaceABC *theColorQuantizerDescriptorInterface);
	DOM_Element HistogramDescriptorDOMElement(DOM_Document doc, 
		HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface);
	DOM_Element ParametricObjectMotionDescriptorDOMElement(DOM_Document doc, 
		ParametricObjectMotionDescriptorInterfaceABC *theParametricObjectMotionDescriptorInterface);
	DOM_Element ContourShapeDescriptorDOMElement(DOM_Document doc, 
		ContourShapeDescriptorInterfaceABC *theContourShapeDescriptorInterface);
#endif

	
	// Some util functions the code above uses	
	bool SpatialConnectivity4(MomVop *mvMedia);
#ifdef USEXML
	DOM_Element CreateIntegerElement(DOM_Document &doc, char* elemName, int value);
#endif
	char *Str(double d, char *pc);
	char *Str(int i, char *pc);
	char *Str(unsigned int ui, char *pc);
	char *Str(unsigned long ul, char *pc);
	char *Str(long l, char *pc);

};

};

#endif //__SEGMENTEXTRACTION_H__
