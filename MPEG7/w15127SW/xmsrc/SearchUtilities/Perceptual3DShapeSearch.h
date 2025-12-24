///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Hui Zhang, In Kyu Park, *Duck Hoon Kim, **Il Dong Yun
// Samsung Advanced Institute of Technology, *Seoul National University, **Hankuk University of Foreign Studies
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  Perceptual3DShapeSearch.h
//

#ifndef __PERCEPTUAL3DSHAPESEARCH_H__
#define __PERCEPTUAL3DSHAPESEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class Perceptual3DShapeSearchTool;

//=============================================================================
class Perceptual3DShapeSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  Perceptual3DShapeSearchInterfaceABC();
  virtual ~Perceptual3DShapeSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface) = 0;
  virtual Perceptual3DShapeDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class Perceptual3DShapeSearchInterface: 
public Perceptual3DShapeSearchInterfaceABC
{
public:
  Perceptual3DShapeSearchInterface(Perceptual3DShapeSearchTool* aTool);
  virtual ~Perceptual3DShapeSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface);
  virtual Perceptual3DShapeDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface);

  virtual double GetDistance(void);


  static const UUID myID;
  static const char *myName;

private:
  Perceptual3DShapeSearchTool *m_SearchTool;
};

//=============================================================================
class Perceptual3DShapeSearchTool: public Search
{
friend class Perceptual3DShapeSearchInterface;
public:
  Perceptual3DShapeSearchTool();
  Perceptual3DShapeSearchTool(Perceptual3DShapeDescriptorInterfaceABC
		      *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual Perceptual3DShapeSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface);
  virtual Perceptual3DShapeDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (Perceptual3DShapeDescriptorInterfaceABC
     *aPerceptual3DShapeDescriptorInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~Perceptual3DShapeSearchTool();

  Perceptual3DShapeSearchInterface m_Interface;
  Perceptual3DShapeDescriptorInterfaceABC *m_RefDescriptorInterface;
  Perceptual3DShapeDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

#endif //__PERCEPTUAL3DSHAPESEARCH_H__
