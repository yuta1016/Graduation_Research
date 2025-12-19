///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
// Yousri Abdeljaoued (EPFL Switzerland)
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
// Applicable File Name:  SummarizationSearch.h
//
#ifndef __SUMMARIZATION_SEARCH_H__
#define __SUMMARIZATION_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{

// Forward Declarations:
class SummarizationSearchTool;

//=============================================================================
class SummarizationSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	SummarizationSearchInterfaceABC();
	virtual ~SummarizationSearchInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aDescription) = 0;
	virtual GenericDSInterfaceABC
	  *GetQueryDescriptionInterface(void) = 0;
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aDescription) = 0;
	virtual int SetMedia(MultiMediaInterfaceABC *aMedia) = 0;
	virtual int InitSearch(void) = 0;
	virtual int StartSearch(void) = 0;
	virtual double PostSearch(void) = 0;
	virtual double ActivityTreshold(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SummarizationSearchInterface: 
  public SummarizationSearchInterfaceABC
{
public:
	SummarizationSearchInterface(SummarizationSearchTool* aTool);
	virtual ~SummarizationSearchInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aDescription);
	virtual GenericDSInterfaceABC *GetQueryDescriptionInterface(void);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aDescription);
	virtual int SetMedia(MultiMediaInterfaceABC *aMedia);
	virtual int InitSearch(void);
	virtual int StartSearch(void);
	virtual double PostSearch(void);
	virtual double ActivityTreshold(void);

	static const UUID myID;
	static const char *myName;

private:
	SummarizationSearchTool *m_SearchTool;
};

//=============================================================================
class SummarizationSearchTool: public Search
{
friend class SummarizationSearchInterface;
public:
	SummarizationSearchTool();
	SummarizationSearchTool(GenericDSInterfaceABC *aDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual SummarizationSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC *aDescription);
	virtual GenericDSInterfaceABC *GetQueryDescriptionInterface(void);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC *aDescription);
	virtual int SetMedia(MultiMediaInterfaceABC *aMedia);
	virtual int InitSearch(void);
	virtual int StartSearch(void);
	virtual double PostSearch(void);
	virtual double ActivityTreshold(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~SummarizationSearchTool();

	SummarizationSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;
	GenericDS m_Summarization;
	GenericDSInterfaceABC *m_SummarizationInterface;
	GenericDS m_FrameProperty;
	MultiMediaInterfaceABC *m_Media;
	int m_writeall; /* parameter of extraction process*/
	double m_ActivityThreshold;
	double m_SumActivity;
	double m_Activity;
};

};


#endif //__COLORHISTSEARCH_H__
