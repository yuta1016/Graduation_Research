///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Kapil Chhabra, Peng Wu, Xinding Sun, Dr. Hyundoo Shin, Prof. B.S. Manjunath
// University of California,Santa Barbara.
// Samsung, Yun Ju Yu, Yang Lim Choi.
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
// Applicable File Name:  TextureBrowsingSearch.h
//
#ifndef __TEXTUREBROWSINGSEARCH_H__
#define __TEXTUREBROWSINGSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class TextureBrowsingDescriptor;
class TextureBrowsingSearchTool;

//=============================================================================
class TextureBrowsingSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  TextureBrowsingSearchInterfaceABC();
  virtual ~TextureBrowsingSearchInterfaceABC() {};

  virtual void destroy()=0;

  virtual int SetRefDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface) = 0;
  virtual TextureBrowsingDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void) = 0;
  virtual int SetQueryDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class TextureBrowsingSearchInterface: 
public TextureBrowsingSearchInterfaceABC
{
public:
  TextureBrowsingSearchInterface(TextureBrowsingSearchTool* aTool);
  virtual ~TextureBrowsingSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface);
  virtual TextureBrowsingDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface);

  virtual double GetDistance(void);


  static const UUID myID;
  static const char *myName;

private:
  TextureBrowsingSearchTool *m_SearchTool;
};

//=============================================================================
class TextureBrowsingSearchTool: public Search
{
friend class TextureBrowsingSearchInterface;
public:
  TextureBrowsingSearchTool();
  TextureBrowsingSearchTool(TextureBrowsingDescriptorInterfaceABC
		      *aQueryDescriptorInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual TextureBrowsingSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface);
  virtual TextureBrowsingDescriptorInterfaceABC*
    GetQueryDescriptorInterface(void);
  virtual int SetQueryDescriptorInterface
    (TextureBrowsingDescriptorInterfaceABC
     *aTextureBrowsingDescriptorInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;
private:
  virtual ~TextureBrowsingSearchTool();

  TextureBrowsingSearchInterface m_Interface;
  TextureBrowsingDescriptorInterfaceABC *m_RefDescriptorInterface;
  TextureBrowsingDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

#endif //__TEXTUREBROWSINGSEARCH_H__

