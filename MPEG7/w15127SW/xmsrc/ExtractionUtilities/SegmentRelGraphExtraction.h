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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SegmentRelGraphExtraction.h
//
#ifndef __SEGMENTRELGRAPHEXTRACTION_H__
#define __SEGMENTRELGRAPHEXTRACTION_H__
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

//=============================================================================

namespace XM
{

// Forward Declarations:
class SegmentRelGraphExtractionTool;

//=============================================================================
class SegmentRelGraphExtractionInterfaceABC: public I_InterfaceABC
{
public:
	SegmentRelGraphExtractionInterfaceABC();
	virtual ~SegmentRelGraphExtractionInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp,
				   char *pcFramesName, char *pcMasksName) = 0;
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class SegmentRelGraphExtractionInterface: 
  public SegmentRelGraphExtractionInterfaceABC
{
public:
	SegmentRelGraphExtractionInterface(SegmentRelGraphExtractionTool*
					   aTool);
	virtual ~SegmentRelGraphExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp,
				   char *pcFramesName, char *pcMasksName);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	SegmentRelGraphExtractionTool *m_ExtractionTool;
};

//=============================================================================
class SegmentRelGraphExtractionTool: public DescriptorExtractor
{
friend class SegmentRelGraphExtractionInterface;
public:
	SegmentRelGraphExtractionTool();
	SegmentRelGraphExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fp,
				   char *pcFramesName, char *pcMasksName);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual SegmentRelGraphExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~SegmentRelGraphExtractionTool();

	SegmentRelGraphExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

	char *m_pcFramesName; // Name of the frames on disk (f_%frameID.ppm)
	char *m_pcMasksName;  // Name of the masks on disk (m_%maskID_%frameID.ras)
	MomVop	*m_mvFS;				// FS file
	FILE	*m_fp;					// tree.txt file

	// Constant variables

	// Constant for strings. A more elegant solution involving the
	// use of dinamic memory might be found later.
	static const int	STRING_MAXSIZE;		// maximum size of the strings


	// Parse: Extract relations and creates the DS for a
	// temporal segment graph
	void ExtractSegmentRelGraph(GenericDSInterfaceABC *parentDSInterface);

	//ReadSegmentRelGraphID: Reads a segment info form a tree.txt
	int ReadSegmentInfoTree( FILE *fp, int iSegmentID, char *pcSegmentType,
				 int*piSegmentNumChildren,
				 int **ppiSegmentChildIDList, 
				 char *pcDecompType, char *pcOverlap,
				 char *pcGap, char *pcSegmentAnno);
	
	// ReadSegmentRelGraphInfo: Reads media FS file for info 
	void ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID,
			       char *pcSegmentType, int *piSegmentNumFrames,
			       int **ppiSegmentFramesList,
			       int **ppiSegmentMasksList);

	// Relation extraction and creation of Segment Graph DS
	void SegmentRelationsExtraction(int NumSegments,
					    int **ppiSegmentFramesLists, 
					    char ***pppcSegRelsList,
					    int *piNumRels);
	void SegmentRelGraphEncoding(GenericDSInterfaceABC *parentDSInterface,
				     int NumSegments,
				     char **ppcSegmentRelationsList,
				     int iNumRelationships);

};

};

#endif //__SEGMENTRELGRAPHEXTRACTION_H__
