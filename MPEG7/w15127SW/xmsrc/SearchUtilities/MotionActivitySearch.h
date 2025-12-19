//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Nam Kyu Kim, Dong-Gye Sim, Hae-Kwang Kim - Hyundai
// Ajay Divakaran - Mitsubishi
// Xinding Sun, B. S. Manjunath -UCSB
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
// Applicable File Name:  MotionActivitySearch.h
//
#ifndef __MOTIONACTIVITYSEARCH_H__
#define __MOTIONACTIVITYSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class MotionActivitySearchTool;

//=============================================================================
class MotionActivitySearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	MotionActivitySearchInterfaceABC();
	virtual ~MotionActivitySearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface) = 0;
	virtual int SetQueryDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MotionActivitySearchInterface: 
  public MotionActivitySearchInterfaceABC
{
public:
	MotionActivitySearchInterface(MotionActivitySearchTool* aTool);
	virtual ~MotionActivitySearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	MotionActivitySearchTool *m_SearchTool;
};

//=============================================================================
class MotionActivitySearchTool: public Search
{
friend class MotionActivitySearchInterface;
public:
	MotionActivitySearchTool();
	MotionActivitySearchTool(MotionActivityDescriptorInterfaceABC
				 *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual MotionActivitySearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (MotionActivityDescriptorInterfaceABC
	   *aMotionActivityDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~MotionActivitySearchTool();

	MotionActivitySearchInterface m_Interface;
	MotionActivityDescriptorInterfaceABC *m_RefDescriptorInterface;
	MotionActivityDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};


#endif //__MOTIONACTIVITYSEARCH_H__
