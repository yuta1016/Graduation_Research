//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshio Kamei, NEC Corporation.
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
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  AdvancedFaceRecognitionSearch.h
//

#ifndef __AdvancedFaceRecognitionSearch_H__
#define __AdvancedFaceRecognitionSearch_H__
#include <stdio.h>
#include <stdlib.h>
#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "SearchUtilities/SearchUtilities.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class AdvancedFaceRecognitionSearchTool;

//=============================================================================
class AdvancedFaceRecognitionSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	AdvancedFaceRecognitionSearchInterfaceABC();
	virtual ~AdvancedFaceRecognitionSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface) = 0;

	virtual AdvancedFaceRecognitionDescriptorInterfaceABC
	  *GetQueryDescriptorInterface(void) = 0;

	virtual int SetQueryDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class AdvancedFaceRecognitionSearchInterface: 
  public AdvancedFaceRecognitionSearchInterfaceABC
{
public:
	AdvancedFaceRecognitionSearchInterface(AdvancedFaceRecognitionSearchTool* aTool);
	virtual ~AdvancedFaceRecognitionSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface);
	virtual AdvancedFaceRecognitionDescriptorInterfaceABC*
	  GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	AdvancedFaceRecognitionSearchTool *m_SearchTool;
};

//=============================================================================
class AdvancedFaceRecognitionSearchTool: public Search
{
friend class AdvancedFaceRecognitionSearchInterface;
public:
	AdvancedFaceRecognitionSearchTool();
	AdvancedFaceRecognitionSearchTool(AdvancedFaceRecognitionDescriptorInterfaceABC
					  *aDescriptorInteAdvancedFaceRecognition);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual AdvancedFaceRecognitionSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface);
	virtual AdvancedFaceRecognitionDescriptorInterfaceABC
 	   *GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
	  (AdvancedFaceRecognitionDescriptorInterfaceABC
	   *aAdvancedFaceRecognitionDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char *myName;
private:
	virtual ~AdvancedFaceRecognitionSearchTool();
	AdvancedFaceRecognitionSearchInterface m_Interface;
	AdvancedFaceRecognitionDescriptorInterfaceABC *m_RefDescriptorInterface;
	AdvancedFaceRecognitionDescriptorInterfaceABC *m_QueryDescriptorInterface;

#define FACE_PARAM_FILE
#ifdef FACE_PARAM_FILE
       int mf_read_param1D( FILE *fp, void *buffer );
       int mf_read_param2D( FILE *fp, void *buffer );
       int mf_read_FaceParamFile( char * path );
#endif 
	
       // Added for AdvancedFaceRecognition parameters
        static double FourierFeatureWeight[96];
        static double CentralFourierFeatureWeight[96];
        static double CompositeFeatureWeight[96];
        static double CentralCompositeFeatureWeight[96];

      };

};


#endif //__AdvancedFaceRecognitionSearch_H__
