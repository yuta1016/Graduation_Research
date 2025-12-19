///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Marius PREDA, Titus ZAHARIA
// Institut National des Telecommunications
// ARTEMIS Project Unit
//
// (contributing organizations names)
//
// Updated 12/13/00:
// Heinrich-Hertz-Institute (HHI), Aljoscha Smolic
// - usage of parameter file to set search parameters
//   declaration of new menber function SetValuesFromParameterFile()
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
#ifndef __PARAMETRICOBJECTMOTIONSEARCH_H__
#define __PARAMETRICOBJECTMOTIONSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "ExtractionUtilities/imgutil.h"
#include "momusys.h"


//=============================================================================

namespace XM
{


// Forward Declarations:
class ParametricObjectMotionDescriptor;
class ParametricObjectMotionSearchTool;

//=============================================================================
class ParametricObjectMotionSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
        ParametricObjectMotionSearchInterfaceABC();
	virtual ~ParametricObjectMotionSearchInterfaceABC() {};
  
	virtual void destroy(void) = 0;

	virtual void  SetValuesFromParameterFile(void) = 0;

	virtual int SetRefDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface) = 0;
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface) = 0;
	virtual double GetDistance(MomVop *rvop,MomVop *qvop,
				   int qm, int qn,int sd) = 0;

	virtual unsigned long SetQueryMethod(int queryMeth) = 0;
	virtual int GetQueryMethod(void) = 0;
	virtual unsigned long SetQueryNorm(int queryNorm) = 0;
	virtual int GetQueryNorm(void) = 0;
	virtual unsigned long SetSquareDim(int squareDim) = 0;
	virtual int GetSquareDim(void) = 0;

	//  virtual void SetRefVop(MomVop *rvop); //used for AssociatedRegion
	//  virtual void SetQueryVop(MomVop *rvop);//used for AssociatedRegion
	static const UUID myID;
  
  
};

//=============================================================================
class ParametricObjectMotionSearchInterface: 
  public ParametricObjectMotionSearchInterfaceABC
{
public:
	ParametricObjectMotionSearchInterface(ParametricObjectMotionSearchTool* aTool);
	virtual ~ParametricObjectMotionSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface);
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface);
	virtual double GetDistance(MomVop *rvop,MomVop *qvop,
				   int qm, int qn, int sd);
//	virtual void SetRefVop(MomVop *rvop);//used for AssociatedRegion
//	virtual void SetQueryVop(MomVop *rvop);//used for AssociatedRegion

	virtual void  SetValuesFromParameterFile(void);

	virtual unsigned long SetQueryMethod(int queryMeth);
	virtual int GetQueryMethod(void);
	virtual unsigned long SetQueryNorm(int queryNorm);
	virtual int GetQueryNorm(void);
	virtual unsigned long SetSquareDim(int squareDim);
	virtual int GetSquareDim(void);

	static const UUID myID;
	static const char *myName;

private:
	ParametricObjectMotionSearchTool *m_SearchTool;
};

//=============================================================================
class ParametricObjectMotionSearchTool: public Search
{
friend class ParametricObjectMotionSearchInterface;
public:
	ParametricObjectMotionSearchTool();
	ParametricObjectMotionSearchTool(
			ParametricObjectMotionDescriptorInterfaceABC
			*aDescriptorInteface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual int SetRefDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface);
	virtual ParametricObjectMotionDescriptorInterfaceABC
	  *GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (ParametricObjectMotionDescriptorInterfaceABC
	   *aParametricObjectMotionDescriptorInterface);
	virtual double GetDistance(MomVop *rvop,MomVop *qvop,
				   int qm, int qn, int sd);
//	virtual void SetRefVop(MomVop *rvop);//used for AssociatedRegion
// 	virtual void SetQueryVop(MomVop *rvop);//used for AssociatedRegion

	virtual unsigned long SetQueryMethod(int queryMeth);
	virtual int GetQueryMethod();
	virtual unsigned long SetQueryNorm(int queryNorm);
	virtual int GetQueryNorm();
	virtual unsigned long SetSquareDim(int squareDim);
	virtual int GetSquareDim();

	virtual ParametricObjectMotionSearchInterfaceABC* GetInterface(void);
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ParametricObjectMotionSearchTool();

	ParametricObjectMotionSearchInterface m_Interface;
	ParametricObjectMotionDescriptorInterfaceABC
	  *m_RefDescriptorInterface;
	ParametricObjectMotionDescriptorInterfaceABC
	  *m_QueryDescriptorInterface;
//	MomVop *m_RefVop;//used for AssociatedRegion
//	MomVop *m_QueryVop;//used for AssociatedRegion

	int m_QueryMethod;
	int m_QueryNorm;
	int m_SquareDim;
};

};


#endif //__PARAMETRICOBJECTMOTIONSEARCH_H__
