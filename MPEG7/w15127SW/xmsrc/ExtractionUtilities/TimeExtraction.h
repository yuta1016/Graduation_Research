///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang	Siemens AG	12/05/2000
// Joerg Heuer          Siemens AG      30/06/2000
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeExtraction.h
//
#ifndef __TIMEEXTRACTION_H__
#define __TIMEEXTRACTION_H__
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

#define RELTIMEPOINT	        0
#define RELINCRTIMEPOINT	1 
#define TIMEPOINT	        2

#define DURATION	0
#define INCRDURATION	1

//=============================================================================

namespace XM
{

// Forward Declarations:
class TimeExtractionTool;

//=============================================================================
class TimeExtractionInterfaceABC: public I_InterfaceABC
{
public:
	TimeExtractionInterfaceABC();
	virtual ~TimeExtractionInterfaceABC() {};
	virtual void destroy()=0;

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fpTree, FILE *fpTime, char *pcMediaName) = 0;
	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class TimeExtractionInterface: 
  public TimeExtractionInterfaceABC
{
public:
	TimeExtractionInterface(TimeExtractionTool* aTool);
	virtual ~TimeExtractionInterface();
	virtual void destroy();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fpTree, FILE *fpTime, char *pcMediaName);
	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	TimeExtractionTool *m_ExtractionTool;
};

//=============================================================================
class TimeExtractionTool: public DescriptorExtractor
{
        friend class TimeExtractionInterface;

public:
	TimeExtractionTool();
	TimeExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MomVop *mvFS, FILE *fpTree, FILE *fpTime, char *pcMediaName);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	virtual TimeExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:

	virtual ~TimeExtractionTool();

	TimeExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

	MomVop	*m_mvFS;				// FS file
	FILE	*m_fpTree;				// tree.txt file
	FILE	*m_fpTime;				// time.txt file	
	char	*m_pcMediaName;
	int m_bRootNode, m_bTimeUnit, m_iRefTime, m_iChoiceStart, m_iChoiceDuration;
	// Constant variables

	// Constant for strings. A more elegant solution involving the
	// use of dynamic memory might be found later.
	static const int	STRING_MAXSIZE;		// maximum size of the strings


	// Parse: Extract Relationships and creates the DOM Tree for a
	// temporal segment relationship graph
	void RecursiveParse(int iSegmentID, GenericDSInterfaceABC *pDomParentInterface, int *piFrameOccurenceList, 
		char **ppcGOPTimeCodeList, int iFrameRate_n, int iFrameRate_d);
	//ReadTimeID: Reads a segment info form a tree.txt
	int ReadTimeModesFromTreeFile(FILE *fp);
	//ReadTimeID: Reads a segment info form a tree.txt
	int ReadSegmentInfoTree( FILE *fpTree, int iSegmentID, char *pcSegmentType, 
		int*piSegmentNumChildren, int **ppiSegmentChildIDList, char *pcDecompType, char *pcOverlap, 
		char *pcGap, char *pcSegmentAnno);
	
	int ReadTimeInfo( FILE *fpTime, int **ppiFrameOccurenceList, char ***pppcGOPTimeCodeList, 
		int **ppiGOPNumFrameList, int *iFrameRate_n, int *iFrameRate_d);
	
	// ReadTimeInfo: Reads media FS file for info 
	void ReadSegmentInfoFS(MomVop *mediaFS, int iSegmentID, 
		int *piSegmentNumFrames, int **ppiSegmentFramesList);

	GenericDS MediaTimeDOMExtraction(GenericDSInterfaceABC *pSegmentInterface, int iStartFrame, int iFrameDuration, 
		char **ppcGOPTimeCodeList, int iFrameRate_n, int iFrameRate_d);

	GenericDS DOMEncodeMediaTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, char **ppcGOPTimeCodeList, 
		int iFrameRate_n, int iFrameRate_d);

	GenericDS DOMEncodeMediaRelTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, 
		int iFrameRate_n, int iFrameRate_d);

	GenericDS DOMEncodeMediaRelIncrTimePoint(GenericDSInterfaceABC *pTimeInterface, int iStartFrame, 
		int iFrameRate_n, int iFrameRate_d);

	GenericDS DOMEncodeMediaDuration(GenericDSInterfaceABC *pTimeInterface, int iFrameDuration, 
		int iFrameRate_n, int iFrameRate_d);

	GenericDS DOMEncodeMediaIncrDuration(GenericDSInterfaceABC *pTimeInterface, int iFrameDuration, 
		int iFrameRate_n, int iFrameRate_d);

	int DOMEncodeTimePeriod(GenericDSInterfaceABC *pTimeInterface, int iTimePeriod, 
		int iFrameRate_n, int iFrameRate_d);
	
	int DOMEncodeTimeUnit(GenericDSInterfaceABC *pTimeInterface, int iFrameRate_n, int iFrameRate_d);

};

};

#endif //__TIMEEXTRACTION_H__
