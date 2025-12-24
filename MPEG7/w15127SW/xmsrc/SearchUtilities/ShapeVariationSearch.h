///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Minseok Choi, Hanyang University, Korea
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
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ShapeVariationSearch.h
//

#ifndef __SHAPE_VARIATION_SEARCH_H__
#define __SHAPE_VARIATION_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"
#include "BasicArchitecture/XMObjects.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class ShapeVariationSearchTool;

//=============================================================================
class ShapeVariationSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ShapeVariationSearchInterfaceABC();
	virtual ~ShapeVariationSearchInterfaceABC() {};
	
	virtual void destroy()=0;
	
	virtual int SetRefDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface) = 0;
	virtual ShapeVariationDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface) = 0;
	
	virtual double GetDistance(void) = 0;
	
	/* sub-components do not have get and set functions because
	there is only one output value*/
	
	static const UUID myID;
};

//=============================================================================
class ShapeVariationSearchInterface: 
public ShapeVariationSearchInterfaceABC
{
public:
	ShapeVariationSearchInterface(ShapeVariationSearchTool* aTool);
	virtual ~ShapeVariationSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);
	
	virtual void destroy();
	
	virtual int SetRefDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	virtual ShapeVariationDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	
	virtual double GetDistance(void);
		
	static const UUID myID;
	static const char *myName;
	
private:
	ShapeVariationSearchTool *m_SearchTool;
};

//=============================================================================
class ShapeVariationSearchTool: public Search
{
friend class ShapeVariationSearchInterface;
public:
	ShapeVariationSearchTool();
	ShapeVariationSearchTool(ShapeVariationDescriptorInterfaceABC
		*aQueryDescriptorInterface);
	
	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	
	virtual ShapeVariationSearchInterfaceABC *GetInterface(void);
	
	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);
	
	virtual int SetRefDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	virtual ShapeVariationDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
		(ShapeVariationDescriptorInterfaceABC
		*aShapeVariationDescriptorInterface);
	
	virtual double GetDistance(void);
		
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ShapeVariationSearchTool();
	
	ShapeVariationSearchInterface m_Interface;
	ShapeVariationDescriptorInterfaceABC *m_RefDescriptorInterface;
	ShapeVariationDescriptorInterfaceABC *m_QueryDescriptorInterface;
	
};

};

#endif //__SHAPE_VARIATION_SEARCH_H__
