///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Ana B. Benitez - Columbia University
// Javier Ruiz Hidalgo - University Politechnic of Catalonia
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
// Applicable File Name:  SegmentSearch.h
//
#ifndef __SEGMENTSEARCH_H__
#define __SEGMENTSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#ifdef USEXML
#include "dom/DOM.hpp"
#include "util/PlatformUtils.hpp"		// Initiliase XML.
#endif
//=============================================================================

namespace XM
{

// Forward Declarations:
class SegmentSearchTool;

//=============================================================================
class SegmentSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	SegmentSearchInterfaceABC();
	virtual ~SegmentSearchInterfaceABC() {};

	virtual int SetRefDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class SegmentSearchInterface: 
  public SegmentSearchInterfaceABC
{
public:
	SegmentSearchInterface(SegmentSearchTool* aTool);
	virtual ~SegmentSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetRefDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	SegmentSearchTool *m_SearchTool;
};

//=============================================================================
class SegmentSearchTool: public Search
{
public:
	SegmentSearchTool();
	virtual ~SegmentSearchTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual SegmentSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDSInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;


private:
	SegmentSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDSInterface;
	GenericDSInterfaceABC *m_QueryDSInterface;

	// Recursive matching of segments of the same type
#ifdef USEXML
	double RecursiveMatching(DOM_Element segElemRef, DOM_Element segElemQuery);

	// Matching of features for a video segment
	double VideoSegmentMatching(DOM_Element elemRef, DOM_Element elemQuery);

	// Matching of GoFGoPColorHistogramDescriptors
	double GoFGoPColorHistogramDescriptorMatching(DOM_Element elemRef, DOM_Element elemQuery);

	// Decode DOM into descriptor class for GoFGoPColorHistogramDescriptor
	void GoFGoPColorDescriptorDOMDecoding(DOM_Element elem, 
		GoFGoPColorDescriptorInterfaceABC *theGoFGoPColorDescriptorInterface);//vin

	// Decode DOM into descriptor class for ColorHistogramDescriptor
//	void ColorHistogramDescriptorDOMDecoding(DOM_Element elem, 
//		ColorHistogramDescriptorInterfaceABC *theColorHistogramDescriptorInterface);//vin

	// Decode DOM into descriptor class for ColorSpaceDescriptor
	void ColorSpaceDescriptorDOMDecoding(DOM_Element elem, 
		ColorSpaceDescriptorInterfaceABC *theColorSpaceDescriptorInterface);

	// Decode DOM into descriptor class for ColorQuantizerDescriptor
	void ColorQuantizerDescriptorDOMDecoding(DOM_Element elem, 
		ColorQuantizerDescriptorInterfaceABC *theColorQuantizerDescriptorInterface);

	// Decode DOM into descriptor class for HistogramDescriptor
	void HistogramDescriptorDOMDecoding(DOM_Element elem, 
		HistogramDescriptorInterfaceABC *theHistogramDescriptorInterface);

	// Utility functions
	DOM_Element GetFirstChildElement(DOM_Node node);
	DOM_Element GetNextSiblingElement(DOM_Node node);
	DOM_Element GetChildByTagName(DOM_Element elem, DOMString name);
	int ReadIntegerElement(DOM_Element elem, DOMString elemName);
#endif
};

};


#endif //__SEGMENTSEARCH_H__
