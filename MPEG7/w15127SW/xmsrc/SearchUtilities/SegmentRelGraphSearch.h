///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
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
// Applicable File Name:  SegmentRelGraphSearch.h
//
#ifndef __SEGMENTRELGRAPHSEARCH_H__
#define __SEGMENTRELGRAPHSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"


//=============================================================================

namespace XM
{

// Forward Declarations:
class SegmentRelGraphSearchTool;

//=============================================================================
class SegmentRelGraphSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	SegmentRelGraphSearchInterfaceABC();
	virtual ~SegmentRelGraphSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SegmentRelGraphSearchInterface: 
  public SegmentRelGraphSearchInterfaceABC
{
public:
	SegmentRelGraphSearchInterface(SegmentRelGraphSearchTool* aTool);
	virtual ~SegmentRelGraphSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	SegmentRelGraphSearchTool *m_SearchTool;
};

//=============================================================================
class SegmentRelGraphSearchTool: public Search
{
friend class SegmentRelGraphSearchInterface;
public:
        SegmentRelGraphSearchTool();
	SegmentRelGraphSearchTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual SegmentRelGraphSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~SegmentRelGraphSearchTool();

	SegmentRelGraphSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;


	// Matching of relations for a SegmentRelationshipGraph DS
	double SegmentRelGraphMatching(GenericDSInterfaceABC *pRefDSInterface, 
		GenericDSInterfaceABC *pQueryDSInterface);
	double L1Dist(double *piSegRelListRef, double *piSegRelListQuery, int dim);

	// Decodes the Segment Graph DS
	int SegmentRelGraphDecoding(GenericDSInterfaceABC *parentDSInterface, 
		char ***pppcSegmentRelationsList, int *piNumRelationships);
};

};


#endif //__SEGMENTRELGRAPHSEARCH_H__
