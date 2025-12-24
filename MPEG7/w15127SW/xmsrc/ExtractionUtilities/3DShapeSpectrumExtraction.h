///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
// Institut National des Telecommunications - ARTEMIS Project Unit, Titus Zaharia
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
// Applicable File Name:  3DShapeSpectrumExtraction.h
//
#ifndef __SHAPESPECTRUMEXTRACTION_H__
#define __SHAPESPECTRUMEXTRACTION_H__

#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */

namespace XM
{


// Forward Declarations:
class ShapeSpectrumExtractionTool;

//=============================================================================
class ShapeSpectrumExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ShapeSpectrumExtractionInterfaceABC();
	virtual ~ShapeSpectrumExtractionInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetSourceMedia(MediaThreeD *Media) = 0;
	virtual ShapeSpectrumDescriptorInterfaceABC
	  *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(
                            ShapeSpectrumDescriptorInterfaceABC
			    *aShapeSpectrumDescriptorInterface) = 0;
	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class ShapeSpectrumExtractionInterface:
	public ShapeSpectrumExtractionInterfaceABC
{
public:
	ShapeSpectrumExtractionInterface(
                        ShapeSpectrumExtractionTool* aTool);
	virtual ~ShapeSpectrumExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MediaThreeD *Media);
	virtual ShapeSpectrumDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
			 ShapeSpectrumDescriptorInterfaceABC
			 *aShapeSpectrumDescriptorInterface);
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);

	static const UUID myID;
	static const char *myName;

private:
	ShapeSpectrumExtractionTool *m_ExtractionTool;
};

//=============================================================================
class ShapeSpectrumExtractionTool: public DescriptorExtractor
{
friend class ShapeSpectrumExtractionInterface;
public:
	ShapeSpectrumExtractionTool();
	ShapeSpectrumExtractionTool(
                             ShapeSpectrumDescriptorInterfaceABC
			     *aDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(MediaThreeD *Media);
	virtual ShapeSpectrumDescriptorInterfaceABC
	  *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(
                         ShapeSpectrumDescriptorInterfaceABC
			 *aShapeSpectrumDescriptorInterface);
	
	virtual unsigned long StartExtracting(void);

	virtual ShapeSpectrumExtractionInterfaceABC
	  *GetInterface(void);
	virtual unsigned long InitExtracting(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ShapeSpectrumExtractionTool();

	ShapeSpectrumExtractionInterface m_Interface;
	ShapeSpectrumDescriptorInterfaceABC *m_DescriptorInterface;

	MediaThreeD *m_Media3D;
	unsigned long m_FrameCnt;
	unsigned long m_ShapeSpectrumSize;
	unsigned long m_ShapeSpectrumNoOfBits;
};

};

#endif //__SHAPESPECTRUMEXTRACTION_H__
