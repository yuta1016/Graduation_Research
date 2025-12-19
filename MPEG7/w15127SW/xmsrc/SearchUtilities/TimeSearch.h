///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Patricia Chiang 	Siemens AG	12/05/2000
// Joerg Heuer          Siemens AG      30/06/2000
//
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  TimeSearch.h
//
#ifndef __TIMESEARCH_H__
#define __TIMESEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include <time.h>
#include <stdio.h>

#define RELATIVE 0
#define ABSOLUTE 1

#define EXCLUSIVE 0
#define EXACT 1
#define SUBSET 2
#define SUPERSET 3
#define OVERLAP 4

//#define DISTFACTOR 1000000000000.0  // sth use DBL_MAX

//=============================================================================

namespace XM
{

// Forward Declarations:
class TimeSearchTool;

// DataClass
class TimePointDC
{
public:
	int m_dateY;
	int m_dateM;
	int m_dateD;
	int m_time_h;
	int m_time_m;
	int m_time_s;
	int m_time_nn;
	int m_time_nnFraction;
	int m_timeTZD;
public:	
	TimePointDC() {};
	virtual ~TimePointDC() {};
	int SetAttributes(int Y, int M, int D, int h, int m, int s, int nn, int nnFraction, int TZD);	
};

class TimeUnitDC
{
public:
	int m_negative;
	int m_time_D;
	int m_time_h;
	int m_time_m;
	int m_time_s;
	int m_time_nn;
	int m_time_nnFraction;
	int m_timeTZD;
public:	
	TimeUnitDC(){};
	virtual ~TimeUnitDC(){};
	int SetAttributes(int neg, int No_D, int h, int m, int s, int nn, int nnFraction, int TZD);
};	
	
//=============================================================================
class TimeSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	TimeSearchInterfaceABC();
	virtual ~TimeSearchInterfaceABC() {};
	virtual void destroy()=0;
	
	virtual int SetDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetSourceQuery(FILE *fpQuery) = 0;	   
	virtual double GetDistance(void) = 0;
	virtual int GetSegmentID(char *pcSegmentID) = 0;

	static const UUID myID;
};

//=============================================================================
class TimeSearchInterface: 
  public TimeSearchInterfaceABC
{
public:
	TimeSearchInterface(TimeSearchTool* aTool);
	virtual ~TimeSearchInterface();
	void destroy(void);

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetSourceQuery(FILE *fpQuery);	   
	virtual double GetDistance(void);
	virtual int GetSegmentID(char *pcSegmentID);


	static const UUID myID;
	static const char *myName;

private:
	TimeSearchTool *m_SearchTool;
};

//=============================================================================
class TimeSearchTool: public Search
{
friend class TimeSearchInterface;
public:
	TimeSearchTool();
	TimeSearchTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual TimeSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetSourceQuery(FILE *fpQuery);  //sth please use GenericDSInterfaceABC  of SetMedia
	virtual double GetDistance(void);
	virtual int GetSegmentID(char *pcSegmentID);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~TimeSearchTool();

	TimeSearchInterface m_Interface;
	GenericDSInterfaceABC *m_DSInterface;
	
	FILE *m_fpQuery;
	
	int m_QueryStartIncrTime, m_QueryIncrDuration, m_Query_time_nnFraction;
	double m_distance;
	char *m_pcSegmentID;

	TimePointDC *m_pRefTimePoint;
	TimeUnitDC *m_pRefTimeUnit;
	int m_time_nnFraction;
	int m_FlagAbsRel, m_FlagTimeUnit, m_FlagRootNode, m_FlagnnFraction;
	
	static const int	STRING_MAXSIZE;	
	
	// Read Query File 
	int ReadQueryFile();

	// Matching of features for a video segment
	int RecursiveTimeMatching(GenericDSInterfaceABC *pDomInterface);
	int CalculateDistance(int *piStartIncrTime, int *piIncrDuration, GenericDSInterfaceABC *pTimeInterface);

	// Decodes the SegmentRelationshipGraph DOM tree
	int MediaTimeDOMDecode(GenericDSInterfaceABC *pTimeInterface, int *piStartIncrTime, int *piIncrDuration);
	int DOMDecodeTimePoint(GenericDSInterfaceABC *pTimePointInterface, int *piStartIncrTime);
	int DOMDecodeRelTimePoint(GenericDSInterfaceABC *pRelTimeInterface, int *piStartIncrTime);
	int DOMDecodeRelIncrTimePoint(GenericDSInterfaceABC *pRelIncrTimeInterface, int *piStartIncrTime);
	int DOMDecodeDuration(GenericDSInterfaceABC *pDurationInterface, int *piIncrDuration);
	int DOMDecodeIncrDuration(GenericDSInterfaceABC *pIncrDurationInterface, int *piIncrDuration);
	int DOMDecodeTimePeriod(GenericDSInterfaceABC *pTimePeriodInterface, int *piTimePeriod);
	int DOMDecodeIncrTime(GenericDSInterfaceABC *pIncrTimeInterface, int *piIncrTime);	
	
	// Utility functions
	int DecodeDurationLexical(char *timeperiod, int *ti_element, int *neg, int *flagnnFraction);
	GenericDS GetChild(GenericDSInterfaceABC *pDomInterface, const string& aName);
	int findLCM(int num1, int num2);
};

};


#endif //__TIMESEARCH_H__
