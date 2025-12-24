/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee : LG-Elite
//
// (contributing organizations names)

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
// Copyright (c) 2000
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchingHintExtraction.h
//
#ifndef __MATCHINGHINTEXTRACTION_H__
#define __MATCHINGHINTEXTRACTION_H__
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
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

#include <iostream>
#include <fstream>

//=============================================================================

namespace XM
{

// Forward Declarations:
class MatchingHintExtractionTool;

//=============================================================================
class MatchingHintExtractionInterfaceABC: public I_InterfaceABC
{
public:
	MatchingHintExtractionInterfaceABC();
	virtual ~MatchingHintExtractionInterfaceABC() {};

	virtual void destroy(void) = 0;

	//sth interface does not look like required
	//sth who is child of whom? 
	virtual int SetFeedbackDSInterface 
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual double StartExtract(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MatchingHintExtractionInterface: 
  public MatchingHintExtractionInterfaceABC
{
public:
	MatchingHintExtractionInterface(MatchingHintExtractionTool*
					   aTool);
	virtual ~MatchingHintExtractionInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetFeedbackDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double StartExtract(void);

	static const UUID myID;
	static const char *myName;

private:
	MatchingHintExtractionTool *m_ExtractionTool;
};

//=============================================================================
class MatchingHintExtractionTool: public DescriptorExtractor
{
friend class MatchingHintExtractionInterface;
public:
	MatchingHintExtractionTool();
	MatchingHintExtractionTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual int SetFeedbackDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double StartExtract(void);
	void GetParameters(double distanceSet[], double maxDistanceSet[]);
	double Updating(GenericDS segDSRef,	GenericDS segDSQuery);
	double StillRegionsUpdating(GenericDS DSRef, GenericDS DSQuery);
	double UpdatingMatchingHintUsingOneFeedback(double distanceSet[], double maxDistanceSet[], GenericDS MatchingHing);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual MatchingHintExtractionInterfaceABC *GetInterface(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~MatchingHintExtractionTool();

	MatchingHintExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_FeedbackDSInterface;
	GenericDSInterfaceABC *m_QueryDSInterface;


};

};

#endif //__MATCHINGHINTEXTRACTION_H__
