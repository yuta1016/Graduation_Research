///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// (contributing organizations names)
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrumSearch.h
//
#ifndef __SHAPESPECTRUMSEARCH_H__
#define __SHAPESPECTRUMSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class ShapeSpectrumSearchTool;

//=============================================================================
class ShapeSpectrumSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ShapeSpectrumSearchInterfaceABC();
	virtual ~ShapeSpectrumSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface) = 0;
	virtual ShapeSpectrumDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface) = 0;
	virtual double GetDistance(int MetricChoice) = 0;

	static const UUID myID;
};

//=============================================================================
class ShapeSpectrumSearchInterface: 
  public ShapeSpectrumSearchInterfaceABC
{
public:
	ShapeSpectrumSearchInterface(ShapeSpectrumSearchTool* aTool);
	virtual ~ShapeSpectrumSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface);
	virtual ShapeSpectrumDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface);
	virtual double GetDistance(int MetricChoice);


	static const UUID myID;
	static const char *myName;

private:
	ShapeSpectrumSearchTool *m_SearchTool;
};

//=============================================================================
class ShapeSpectrumSearchTool: public Search
{
friend class ShapeSpectrumSearchInterface;
public:
	ShapeSpectrumSearchTool();
	ShapeSpectrumSearchTool(ShapeSpectrumDescriptorInterfaceABC
				 *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual ShapeSpectrumSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface);
	virtual ShapeSpectrumDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ShapeSpectrumDescriptorInterfaceABC
	   *aShapeSpectrumDescriptorInterface);
	virtual double GetDistance(int MetricChoice);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ShapeSpectrumSearchTool();

	ShapeSpectrumSearchInterface m_Interface;
	ShapeSpectrumDescriptorInterfaceABC *m_RefDescriptorInterface;
	ShapeSpectrumDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

int Dist3DSSD(unsigned long *dreful, unsigned long *dqueryul, int n, int NoBits, int MetricChoice, double &err);
double dist_L1(double *dref, double *dq, int n);
double dist_L2(double *dref, double *dq, int n);
double dist_pond(double *dref, double *dq, int n);
#endif //__SHAPESPECTRUMSEARCH_H__
