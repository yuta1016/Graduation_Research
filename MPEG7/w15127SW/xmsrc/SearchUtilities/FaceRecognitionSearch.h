///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Lei Wang, Zhongyang Huang
// Panasonic Singapore Laboratories Pte Ltd
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
// Applicable File Name:  FaceRecognitionSearch.h
//
#ifndef __FaceRecognitionSEARCH_H__
#define __FaceRecognitionSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
//#include "imgutil.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class FaceRecognitionSearchTool;

//=============================================================================
class FaceRecognitionSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	FaceRecognitionSearchInterfaceABC();
	virtual ~FaceRecognitionSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface) = 0;

	virtual FaceRecognitionDescriptorInterfaceABC
	  *GetQueryDescriptorInterface(void) = 0;

	virtual int SetQueryDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class FaceRecognitionSearchInterface: 
  public FaceRecognitionSearchInterfaceABC
{
public:
	FaceRecognitionSearchInterface(FaceRecognitionSearchTool* aTool);
	virtual ~FaceRecognitionSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface);
	virtual FaceRecognitionDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	FaceRecognitionSearchTool *m_SearchTool;
};

//=============================================================================
class FaceRecognitionSearchTool: public Search
{
friend class FaceRecognitionSearchInterface;
public:
	// Modified according to ISO/IEC 15938-3/FCD and XM visual part 10.0 (2001.05)
	int qstep[48];
	double weight1[48];
//	int qstep[49];
//	double weight1[49];
	FaceRecognitionSearchTool();
	FaceRecognitionSearchTool(
			FaceRecognitionDescriptorInterfaceABC
			*aDescriptorInteFaceRecognition);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual FaceRecognitionSearchInterfaceABC *GetInterface(void);

	virtual int SetRefDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface);
	virtual FaceRecognitionDescriptorInterfaceABC
 	   *GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (FaceRecognitionDescriptorInterfaceABC
	   *aFaceRecognitionDescriptorInterface);
	virtual double GetDistance(void);
	


	static const UUID myID;
	static const char *myName;
private:
	virtual ~FaceRecognitionSearchTool();
	FaceRecognitionSearchInterface m_Interface;
	FaceRecognitionDescriptorInterfaceABC *m_RefDescriptorInterface;
	FaceRecognitionDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};


#endif //__FaceRecognitionSEARCH_H__
