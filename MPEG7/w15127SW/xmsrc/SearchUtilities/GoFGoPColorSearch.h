///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Stephan Herrmann
// Munich University of technology
// based on the code from
// A. Mufit Ferman - University of Rochester
// Vincent Hsu, June 11, 2001
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  GoFGoPColorSearch.h
//
#ifndef __GOFCOLORSEARCH_H__
#define __GOFCOLORSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class GoFGoPColorSearchTool;
class ScalableColorSearchInterfaceABC;

//=============================================================================
class GoFGoPColorSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  GoFGoPColorSearchInterfaceABC();
  virtual ~GoFGoPColorSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface) = 0;
  virtual GoFGoPColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class GoFGoPColorSearchInterface: 
public GoFGoPColorSearchInterfaceABC
{
public:
  GoFGoPColorSearchInterface(GoFGoPColorSearchTool* aTool);
  virtual ~GoFGoPColorSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface);
  virtual GoFGoPColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface);

  virtual double GetDistance(void);


  static const UUID myID;
  static const char *myName;

private:
  GoFGoPColorSearchTool *m_SearchTool;
};

//=============================================================================
class GoFGoPColorSearchTool: public Search
{
friend class GoFGoPColorSearchInterface;
public:
  GoFGoPColorSearchTool();
  GoFGoPColorSearchTool(GoFGoPColorDescriptorInterfaceABC
		      *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual GoFGoPColorSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface);
  virtual GoFGoPColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (GoFGoPColorDescriptorInterfaceABC
     *aGoFGoPColorDescriptorInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~GoFGoPColorSearchTool();

  GoFGoPColorSearchInterface m_Interface;
  GoFGoPColorDescriptorInterfaceABC *m_RefDescriptorInterface;
  GoFGoPColorDescriptorInterfaceABC *m_QueryDescriptorInterface;

  ScalableColorSearchInterfaceABC *m_ScalableColorSearch;
};

};

#endif //__GOFCOLORHISTSEARCH_H__
