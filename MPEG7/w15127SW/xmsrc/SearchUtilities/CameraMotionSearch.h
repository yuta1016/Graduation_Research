///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Benoit MORY - Laboratoires d'Electronique Philips, France
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
// Applicable File Name:  ExtractionUtilities.h
//
#ifndef __CAMERAMOTIONSEARCH_H__
#define __CAMERAMOTIONSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class CameraMotionSearchTool;

//=============================================================================
class CameraMotionSearchInterfaceABC: public I_SearchInterfaceABC 
{
public:
	CameraMotionSearchInterfaceABC();
	virtual ~CameraMotionSearchInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetRefDescriptorInterface (CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface) = 0;
	virtual int SetQueryDescriptorInterface (CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class CameraMotionSearchInterface: public CameraMotionSearchInterfaceABC
{
public:
	CameraMotionSearchInterface(CameraMotionSearchTool* aTool);
	virtual ~CameraMotionSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy() ;

	virtual int SetRefDescriptorInterface (CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);
	virtual int SetQueryDescriptorInterface (CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char *myName;

private:
	CameraMotionSearchTool *m_SearchTool;
};

//=============================================================================
class CameraMotionSearchTool: public Search
{

	friend class CameraMotionSearchInterface ;

public:
	CameraMotionSearchTool();
	CameraMotionSearchTool(CameraMotionDescriptorInterfaceABC *aQueryDescriptorInterface) ;

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual CameraMotionSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);
	virtual int SetQueryDescriptorInterface(CameraMotionDescriptorInterfaceABC *aCameraMotionDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;

private:
	virtual ~CameraMotionSearchTool();

	CameraMotionSearchInterface m_Interface;
	CameraMotionDescriptorInterfaceABC *m_RefDescriptorInterface;
	CameraMotionDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};


#endif //__CAMERAMOTIONSEARCH_H__
