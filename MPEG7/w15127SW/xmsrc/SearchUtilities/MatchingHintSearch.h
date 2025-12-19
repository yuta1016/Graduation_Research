/////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Jin-Soo Lee : LG-Elite
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
// Copyright (c) 2000-
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MatchingHintSearch.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATCHING_HINT_SEARCH_H__
#define __MATCHING_HINT_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"


//=============================================================================

namespace XM
{


// Forward Declarations:
class MatchingHintSearchTool;

//=============================================================================
class MatchingHintSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	MatchingHintSearchInterfaceABC();
	virtual ~MatchingHintSearchInterfaceABC() {};

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
class MatchingHintSearchInterface: 
  public MatchingHintSearchInterfaceABC
{
public:
	MatchingHintSearchInterface(MatchingHintSearchTool* aTool);
	virtual ~MatchingHintSearchInterface();
	
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
	MatchingHintSearchTool *m_SearchTool;
};

//=============================================================================
class MatchingHintSearchTool: public Search
{
friend class MatchingHintSearchInterface;
public:
    MatchingHintSearchTool();
	MatchingHintSearchTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual MatchingHintSearchInterfaceABC *GetInterface(void);

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

	double Matching(GenericDS segDSRef,	GenericDS segDSQuery);
	double StillRegionsMatching(GenericDS DSRef, GenericDS DSQuery);
	void GetParameters(double distanceSet[], double maxDistanceSet[]);	
	double GetDistanceUsingMatchingHint(double distanceSet[],
										double maxDistanceSet[],
										GenericDS MatchingHint);


private:
	virtual ~MatchingHintSearchTool();

	MatchingHintSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;

};

};


#endif //__MATCHINGHINTSEARCH_H__
