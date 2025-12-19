///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// ICU(Information Communication University), Yong-Ju Jung,Ki Won Yoo,Yong Man Ro.
// ETRI, Mun Churl Kim
// Samsung, Yun Ju Yu, Yang Lim Choi.
// in cooperation with UCSB, Hyundai, HHI.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  HomoTextureSearch.h
//
#ifndef _HOMO_TEXTURE_Search_H_
#define _HOMO_TEXTURE_Search_H_

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class HomogeneousTextureSearchTool;

//=============================================================================
class HomogeneousTextureSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  HomogeneousTextureSearchInterfaceABC();
  virtual ~HomogeneousTextureSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface) = 0;
  virtual HomogeneousTextureDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class HomogeneousTextureSearchInterface: 
public HomogeneousTextureSearchInterfaceABC
{
public:
  HomogeneousTextureSearchInterface(HomogeneousTextureSearchTool* aTool);
  virtual ~HomogeneousTextureSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface);
  virtual HomogeneousTextureDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface);

  virtual double GetDistance(void);


  static const UUID myID;
  static const char *myName;

private:
  HomogeneousTextureSearchTool *m_SearchTool;
};

//=============================================================================
class HomogeneousTextureSearchTool: public Search
{
friend class HomogeneousTextureSearchInterface;
public:
  HomogeneousTextureSearchTool();
  HomogeneousTextureSearchTool(HomogeneousTextureDescriptorInterfaceABC
		      *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual HomogeneousTextureSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface);
  virtual HomogeneousTextureDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (HomogeneousTextureDescriptorInterfaceABC
     *aHomogeneousTextureDescriptorInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~HomogeneousTextureSearchTool();

  HomogeneousTextureSearchInterface m_Interface;
  HomogeneousTextureDescriptorInterfaceABC *m_RefDescriptorInterface;
  HomogeneousTextureDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

#endif //_HOMO_TEXTURE_Search_H_
