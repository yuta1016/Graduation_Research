//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// K.Masukura (R&D Center Toshiba, Japan)
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
// Copyright (c) 1998-2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SpatioTemporalLocatorSearch.h
//
#ifndef __SpatioTemporalLocatorSEARCH_H__
#define __SpatioTemporalLocatorSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "Media/Media.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class SpatioTemporalLocatorSearchTool;

//=============================================================================
class SpatioTemporalLocatorSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	SpatioTemporalLocatorSearchInterfaceABC();
	virtual ~SpatioTemporalLocatorSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface) = 0;
	virtual SpatioTemporalLocatorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;
	virtual int SetQueryPoint(MultiMediaInterfaceABC *media) = 0;

	static const UUID myID;
};

//=============================================================================
class SpatioTemporalLocatorSearchInterface: 
  public SpatioTemporalLocatorSearchInterfaceABC
{
public:
	SpatioTemporalLocatorSearchInterface(SpatioTemporalLocatorSearchTool* aTool);
	virtual ~SpatioTemporalLocatorSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface);
	virtual SpatioTemporalLocatorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface);
	virtual double GetDistance(void);
	virtual int SetQueryPoint(MultiMediaInterfaceABC *media);


	static const UUID myID;
	static const char *myName;

private:
	SpatioTemporalLocatorSearchTool *m_SearchTool;
};

//=============================================================================
class SpatioTemporalLocatorSearchTool: public Search
{
friend class	SpatioTemporalLocatorSearchInterface;
public:
	SpatioTemporalLocatorSearchTool();
	SpatioTemporalLocatorSearchTool(
			 SpatioTemporalLocatorDescriptorInterfaceABC
			 *aQueryDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual SpatioTemporalLocatorSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface);
	virtual SpatioTemporalLocatorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (SpatioTemporalLocatorDescriptorInterfaceABC
	   *aSpatioTemporalLocatorDescriptorInterface);
	virtual double GetDistance(void);
	virtual int SetQueryPoint(MultiMediaInterfaceABC *media);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~SpatioTemporalLocatorSearchTool(); 

	double GetAtT(double ta,double tb,double fa,double fb,double a,double t );

	SpatioTemporalLocatorSearchInterface m_Interface;
	SpatioTemporalLocatorDescriptorInterfaceABC *m_RefDescriptorInterface;
	SpatioTemporalLocatorDescriptorInterfaceABC *m_QueryDescriptorInterface;

	double m_Time, m_X, m_Y;
	int m_FlagDepth;
	double m_NearDepth, m_FarDepth;
};

};


#endif //__SpatioTemporalLocatorSEARCH_H__
