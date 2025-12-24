///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// NEC Corp., Akio Yamada and Eiji Kasutani
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
// Applicable File Name:  ExtractionUtilities.h
//
#ifndef __COLOR_LAYOUT_SEARCH_H__
#define __COLOR_LAYOUT_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
//#include "imgutil.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
//class ColorLayoutDescriptor;
class ColorLayoutSearchTool;

//=============================================================================
class ColorLayoutSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
    ColorLayoutSearchInterfaceABC();
    virtual ~ColorLayoutSearchInterfaceABC() {};

    // start modification for version 3.0
    virtual void destroy()=0;
    // end modification for version 3.0

    virtual int SetRefDescriptorInterface
      (ColorLayoutDescriptorInterfaceABC 
       *aColorLayoutDescriptorInterface) = 0;
    // start modification for version 3.0
    virtual ColorLayoutDescriptorInterfaceABC*
      GetQueryDescriptorInterface(void) = 0;
    virtual int SetQueryDescriptorInterface
      (ColorLayoutDescriptorInterfaceABC
       *aColorLayoutDescriptorInterface) = 0;
    virtual float GetDistance(void) = 0;
    virtual void SetWeightingValues(int weight[3][64]) = 0;
    
    static const UUID myID;
};

//=============================================================================
class ColorLayoutSearchInterface: 
  public ColorLayoutSearchInterfaceABC
{
public:
    ColorLayoutSearchInterface(ColorLayoutSearchTool* aTool);
    virtual ~ColorLayoutSearchInterface();

    virtual const UUID& GetInterfaceID(void);
    virtual const char *GetName(void);

    // start modification for version 3.0
    virtual void destroy();
    // end modification for version 3.0

    virtual int SetRefDescriptorInterface
	  (ColorLayoutDescriptorInterfaceABC
	   *aColorLayoutDescriptorInterface);
    // start modification for version 3.0
    virtual ColorLayoutDescriptorInterfaceABC*
      GetQueryDescriptorInterface(void);
    // end modification for version 3.0
    virtual int SetQueryDescriptorInterface
	  (ColorLayoutDescriptorInterfaceABC
	   *aColorLayoutDescriptorInterface);
    virtual float GetDistance(void);
    virtual void SetWeightingValues(int weight[3][64]);

    static const UUID myID;
    static const char *myName;

private:
    ColorLayoutSearchTool *m_SearchTool;
};

//=============================================================================
class ColorLayoutSearchTool: public Search
{
friend class ColorLayoutSearchInterface;
public:
    ColorLayoutSearchTool();
    // start modification for version 3.0
    ColorLayoutSearchTool(ColorLayoutDescriptorInterfaceABC
			  *aQueryDescriptorInterface);
    // end modification for version 3.0
//    virtual ~ColorLayoutSearchTool();
    // start modification for version 3.0
    virtual bool SupportsPush(void);
    virtual bool SupportsPull(void);
    // end modification for version 3.0

    virtual const UUID& GetObjectID(void);
    virtual const char *GetName(void);
    virtual int SetRefDescriptorInterface
	  (ColorLayoutDescriptorInterfaceABC
	   *aColorLayoutDescriptorInterface);
    // start modification for version 3.0
    virtual ColorLayoutDescriptorInterfaceABC*
      GetQueryDescriptorInterface(void);
    // end modification for version 3.0
    virtual int SetQueryDescriptorInterface
	  (ColorLayoutDescriptorInterfaceABC
	   *aColorLayoutDescriptorInterface);
    virtual float GetDistance(void);
    virtual void SetWeightingValues(int weight[3][64]);

    virtual ColorLayoutSearchInterfaceABC* GetInterface(void);
    static const UUID myID;
    static const char * myName;
private:
    // start modification for version 3.0
    virtual ~ColorLayoutSearchTool();
    // end modification for version 3.0
    ColorLayoutSearchInterface m_Interface;
    ColorLayoutDescriptorInterfaceABC *m_RefDescriptorInterface;
    ColorLayoutDescriptorInterfaceABC *m_QueryDescriptorInterface;
    int m_weight[3][64];
};

};


#endif //__COLOR_LAYOUT_SEARCH_H__
