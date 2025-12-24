// This software module was originally developed by
//
// Gandhimathi Vaithilingam, Philips Research Laboratories, U.S.A.
// 
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
// Copyright is not released for non MPEG-7 conforming products. Philips
// Research Laboratories retains the full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MediaReviewSearch.h
//


#ifndef __MEDIA_REVIEW_SEARCH_H__
#define __MEDIA_REVIEW_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

/*#ifdef USEXML //sth use function of GenericDS for this
#include "dom/DOM.hpp"
#include "util/PlatformUtils.hpp"		// Initiliase XML.
#endif*/

#define MAX_NUM_REVIEWERS_IN_QUERY 10

//=============================================================================

namespace XM
{

// Forward Declarations:
class MediaReviewSearchTool;

//=============================================================================
class MediaReviewSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	MediaReviewSearchInterfaceABC();
	virtual ~MediaReviewSearchInterfaceABC() {};

	virtual void destroy(void) = 0;
	virtual int SetRefDSInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;
	virtual int SetQueryDSInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MediaReviewSearchInterface: 
  public MediaReviewSearchInterfaceABC
{
public:
	MediaReviewSearchInterface(MediaReviewSearchTool* aTool);
	virtual ~MediaReviewSearchInterface();
	
	virtual void destroy(void);
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetRefDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	virtual int SetQueryDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	MediaReviewSearchTool *m_SearchTool;
};

//=============================================================================
class MediaReviewSearchTool: public Search
{
friend class MediaReviewSearchInterface;
public:
	MediaReviewSearchTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual MediaReviewSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	virtual int SetQueryDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~MediaReviewSearchTool();

	MediaReviewSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDSInterface;
	GenericDSInterfaceABC *m_QueryDSInterface;

	//Added by Vincent
	double  SimilarityMatch();
	string  getReviewerName(GenericDSInterfaceABC *aDescription);
	int     getIntegerValue(GenericDSInterfaceABC *aDescription,
				char* tagName);
	string  getTextValue(GenericDSInterfaceABC *aDescription,
			     char* tagname);
	void    getRatingSchemeIntegerValue(
	           GenericDSInterfaceABC *aDescription,
		   int &worst, int &best);
};

};
#endif //__MEDIA_REVIEW_SEARCH_H__
