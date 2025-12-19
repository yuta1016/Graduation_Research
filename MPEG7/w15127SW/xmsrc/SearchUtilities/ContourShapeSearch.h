///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric ITE-VIL
// W.Price
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ContourShapeSearch.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CONTOURSHAPESEARCH_H__
#define __CONTOURSHAPESEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class ContourShapeSearchTool;

//=============================================================================
class ContourShapeSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ContourShapeSearchInterfaceABC();
	virtual ~ContourShapeSearchInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetRefDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface) = 0;
	virtual int SetQueryDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ContourShapeSearchInterface: 
  public ContourShapeSearchInterfaceABC
{
public:
	ContourShapeSearchInterface(ContourShapeSearchTool* aTool);
	virtual ~ContourShapeSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	ContourShapeSearchTool *m_SearchTool;
};

//=============================================================================
class ContourShapeSearchTool: public Search
{
friend class ContourShapeSearchInterface;
public:
	ContourShapeSearchTool();
	ContourShapeSearchTool(ContourShapeDescriptorInterfaceABC
                               *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual ContourShapeSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (ContourShapeDescriptorInterfaceABC
	   *aContourShapeDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ContourShapeSearchTool();

        class Node
        {
        public:
          double cost;
          int    nr;
          int    nq;
          Point2 rPeaks[(CONTOURSHAPE_CSSPEAKMASK)+1];
          Point2 qPeaks[(CONTOURSHAPE_CSSPEAKMASK)+1];
        };

	ContourShapeSearchInterface m_Interface;
	ContourShapeDescriptorInterfaceABC *m_RefDescriptorInterface;
	ContourShapeDescriptorInterfaceABC *m_QueryDescriptorInterface;

        float *m_rPeaksX, *m_rPeaksY, *m_qPeaksX, *m_qPeaksY;
        Node *m_nodeList;

};

};


#endif //__CONTOURSHAPESEARCH_H__
