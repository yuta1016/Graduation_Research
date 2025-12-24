///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Heinrich-Hertz-Institute (HHI), Bela Makai
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
// Applicable File Name:  ScalableColorSearch.h
//
#ifndef __SCALABLECOLORSEARCH_H__
#define __SCALABLECOLORSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class ScalableColorSearchTool;

//=============================================================================
class ScalableColorSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ScalableColorSearchInterfaceABC();
	virtual ~ScalableColorSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface) = 0;
	virtual ScalableColorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ScalableColorSearchInterface: 
  public ScalableColorSearchInterfaceABC
{
public:
	ScalableColorSearchInterface(ScalableColorSearchTool* aTool);
	virtual ~ScalableColorSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface);
	virtual ScalableColorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	ScalableColorSearchTool *m_SearchTool;
};

//=============================================================================
class ScalableColorSearchTool: public Search
{
friend class ScalableColorSearchInterface;
public:
	ScalableColorSearchTool();
	ScalableColorSearchTool(ScalableColorDescriptorInterfaceABC
				 *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual ScalableColorSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface);
	virtual ScalableColorDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ScalableColorDescriptorInterfaceABC
	   *aScalableColorDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ScalableColorSearchTool();

	ScalableColorSearchInterface m_Interface;
	ScalableColorDescriptorInterfaceABC *m_RefDescriptorInterface;
	ScalableColorDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};


#endif //__SCALABLECOLORSEARCH_H__
