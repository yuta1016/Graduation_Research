///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// University of California Santa Barbara, Kapil Chhabra, Yining Deng.
// Mitsubishi Electric ITE-VIL, Leszek Cieplinski, LG-Elite.
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DominantColorSearch.h
//
#ifndef __DOMINANTCOLORSEARCH_H__
#define __DOMINANTCOLORSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/imgutil.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class DominantColorDescriptor;
class DominantColorSearchTool;

//=============================================================================
class DominantColorSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  DominantColorSearchInterfaceABC();
  virtual ~DominantColorSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface) = 0;
  virtual DominantColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface) = 0;
  virtual double GetDistance(void) = 0;

  static const UUID myID;
};

//=============================================================================
class DominantColorSearchInterface: 
  public DominantColorSearchInterfaceABC
{
public:
  DominantColorSearchInterface(DominantColorSearchTool* aTool);
  virtual ~DominantColorSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface);
  virtual DominantColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface);
  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  DominantColorSearchTool *m_SearchTool;
};

//=============================================================================
class DominantColorSearchTool: public Search
{
friend class DominantColorSearchInterface;
public:
  DominantColorSearchTool();
  DominantColorSearchTool(DominantColorDescriptorInterfaceABC
                          *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual DominantColorSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface);
  virtual DominantColorDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (DominantColorDescriptorInterfaceABC
     *aDominantColorDescriptorInterface);
  virtual double GetDistance(void);
  virtual double GetDistanceVariance(
    float *per1, float **color1, float **var1, int size1,
    float *per2, float **color2, float **var2, int size2);

  static const UUID myID;
  static const char * myName;

private:
  virtual ~DominantColorSearchTool();

  DominantColorSearchInterface m_Interface;
  DominantColorDescriptorInterfaceABC *m_RefDescriptorInterface;
  DominantColorDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};


#endif //__DOMINANTCOLORSEARCH_H__
