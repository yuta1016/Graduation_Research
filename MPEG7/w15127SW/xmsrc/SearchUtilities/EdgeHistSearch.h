///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ETRI / Dongguk University, Seoul.
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
// Applicable File Name:  EdgeHistSearch.h
//

#ifndef __EDGEHISTSEARCH_H__
#define __EDGEHISTSEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class EdgeHistogramSearchTool;

//=============================================================================
class EdgeHistogramSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  EdgeHistogramSearchInterfaceABC();
  virtual ~EdgeHistogramSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface) = 0;
  virtual EdgeHistogramDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface) = 0;

  virtual double GetDistance(void) = 0;

  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class EdgeHistogramSearchInterface: 
  public EdgeHistogramSearchInterfaceABC
{
public:
  EdgeHistogramSearchInterface(EdgeHistogramSearchTool* aTool);
  virtual ~EdgeHistogramSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy(void);

  virtual int SetRefDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface);
  virtual EdgeHistogramDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface);

  virtual double GetDistance(void);


  static const UUID myID;
  static const char *myName;

private:
  EdgeHistogramSearchTool *m_SearchTool;
};

//=============================================================================
class EdgeHistogramSearchTool: public Search
{
friend class EdgeHistogramSearchInterface;
public:
  EdgeHistogramSearchTool();
  EdgeHistogramSearchTool(EdgeHistogramDescriptorInterfaceABC
			  *aQueryDescriptorInteface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual EdgeHistogramSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface);
  virtual EdgeHistogramDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (EdgeHistogramDescriptorInterfaceABC
     *aEdgeHistogramDescriptorInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;
private:
  virtual ~EdgeHistogramSearchTool();

  EdgeHistogramSearchInterface m_Interface;
  EdgeHistogramDescriptorInterfaceABC *m_RefDescriptorInterface;
  EdgeHistogramDescriptorInterfaceABC *m_QueryDescriptorInterface;

  virtual void Make_Global_SemiGlobal(double *LocalHistogramOnly,
					    double *TotalHistogram);

};

};


#endif //__EDGEHISTSEARCH_H__
