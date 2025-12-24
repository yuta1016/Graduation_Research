///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Ana B. Benitez
// Company: Columbia University
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  PyramidSearch.h
//

#ifndef __PYRAMID_SEARCH_H__
#define __PYRAMID_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class PyramidSearchTool;

//=============================================================================
class PyramidSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  PyramidSearchInterfaceABC();
  virtual ~PyramidSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class PyramidSearchInterface: 
public PyramidSearchInterfaceABC
{
public:
  PyramidSearchInterface(PyramidSearchTool* aTool);
  virtual ~PyramidSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  PyramidSearchTool *m_SearchTool;
};

//=============================================================================
class PyramidSearchTool: public Search
{
friend class PyramidSearchInterface;
public:
  PyramidSearchTool();
  PyramidSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual PyramidSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aPyramidDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~PyramidSearchTool();

  PyramidSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;

  int GetAnnotationsPerLevel(GenericDSInterfaceABC *pdsiDSInt, string *pstrTextAnnotations);
  double AnnotationsPerLevelMatching(string *pstrTextAnnotsRef, string *pstrTextAnnotsQuery);
  
  // Replaces multiple sub-string for another sub-string in a string
  void ReplaceStrings(string& string, char *strListTypes, ...);
  // Replaces a sub-string for another sub-string in a string
  void ReplaceInString(string& string, char *str1, char *str2);
};

};

#endif //__PYRAMID_SEARCH_H__
