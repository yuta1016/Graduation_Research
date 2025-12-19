///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Håkan Wallin and Charilaos Christopoulos (MediaLab, Ericsson Radio Systems AB,Sweden)
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
// Applicable File Name:  MosaicSearch.h
//

#ifndef __MOSAICSEARCH_H__
#define __MOSAICSEARCH_H__


#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class MosaicSearchTool;

//=============================================================================
class MosaicSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  MosaicSearchInterfaceABC();
  virtual ~MosaicSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface) = 0;

  virtual int SetMedia(MultiMediaInterfaceABC* media) = 0;
  virtual double InitSearch(void) = 0;
  virtual double StartSearch(void) = 0;
  virtual double PostSearch(void) = 0;
  virtual MultiMediaInterfaceABC *GetOutMedia() = 0;

  /* sub-components do not have get and set functions because
     there is only one output value*/

  static const UUID myID;
};

//=============================================================================
class MosaicSearchInterface: 
public MosaicSearchInterfaceABC
{
public:
  MosaicSearchInterface(MosaicSearchTool* aTool);
  virtual ~MosaicSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface);

  virtual int SetMedia(MultiMediaInterfaceABC* media);
  virtual double InitSearch(void);
  virtual double StartSearch(void);
  virtual double PostSearch(void);
  virtual MultiMediaInterfaceABC *GetOutMedia();

  static const UUID myID;
  static const char *myName;

private:
  MosaicSearchTool *m_SearchTool;
};

//=============================================================================
class MosaicSearchTool: public Search
{
friend class MosaicSearchInterface;
public:
  MosaicSearchTool();
  MosaicSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual MosaicSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aMosaicDescriptionInterface);

  virtual int SetMedia(MultiMediaInterfaceABC* media); /* needed ,e.g.,
	     to read the time of the image*/
  virtual double InitSearch(void);
  virtual double StartSearch(void);
  virtual double PostSearch(void);
  virtual MultiMediaInterfaceABC *GetOutMedia();

  static const UUID myID;
  static const char * myName;
private:
  virtual ~MosaicSearchTool();

  MosaicSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;
  GenericDS m_Mosaic;
  GenericDSInterfaceABC *m_MosaicInterface;
  GenericDS m_MediaTime;



  MultiMediaInterfaceABC* m_Media;
  MultiMediaInterfaceABC* m_OutMedia;
  TCoor xsize,ysize;
  float m_Origins[4];
  int m_OffsetX;
  int m_OffsetY;
  float m_Param[12];
  int m_CurrTime;
  int m_NoOfParams;


/* 	int m_CreateMosaic; */
/* 	int m_MosaicX; */
/* 	int m_MosaicY; */
/* 	int m_SetBackground; */
/* 	int m_MosaicType; */
/* 	int m_Origiontype; */
/* 	int m_NoOfParams; */
/* 	int m_nnfraction; */
/* 	int m_nn; */
/* 	int m_SkipBoundary; */

	/* internal variables*/
  
/*   MomVop *m_Mosaic; */
/*   MomVop *m_Next; */
/*   int m_NextXSize; */
/*   int m_NextYSize; */

/*   int m_Sets; */
/*   int m_OffsetX; */
/*   int m_OffsetY; */

};

};

#endif //__MOSIACSEARCH_H__
