///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// HHI
// K. Mueller
//
// Mitsubishi Electric - Infomation Technology Europe - Visual Information Laboratory
// James Cooper
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  MultiViewExtraction.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MultiViewExtraction_H__
#define __MultiViewExtraction_H__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include <momusys.h>
#include <intra.h>
#include <math.h>

//=============================================================================
namespace XM
{


// Forward Declarations:
class MultiViewExtractionTool;
//=============================================================================
class MultiViewExtractionInterfaceABC: public I_InterfaceABC
{
public:
	MultiViewExtractionInterfaceABC();
	virtual ~MultiViewExtractionInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetSourceMedia(MultiMediaInterfaceABC **media) = 0;
	virtual MultiViewDescriptorInterfaceABC *GetDescriptorInterface(void) = 0;
	virtual int SetDescriptorInterface(MultiViewDescriptorInterfaceABC 
		        *aMultiViewDescriptorInterface) = 0;

	//sth use real name instead of SUB_EXTRACTION_INTERFACE_ABC
	virtual SUB_EXTRACTION_INTERFACE_ABC* Get2DExtractionInterface() = 0;
	virtual void Set2DExtractionInterface( SUB_EXTRACTION_INTERFACE_ABC* 
		                                     a2DExtractionInterface ) = 0;

	virtual unsigned long StartExtracting(void) = 0;


	static const UUID myID;
};

//=============================================================================
class MultiViewExtractionInterface: 
  public MultiViewExtractionInterfaceABC
{
public:
	MultiViewExtractionInterface(MultiViewExtractionTool* aTool);
	virtual ~MultiViewExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC **media);

	virtual MultiViewDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MultiViewDescriptorInterfaceABC
					  *aMultiViewDescriptorInterface);

	virtual SUB_EXTRACTION_INTERFACE_ABC* Get2DExtractionInterface();
	virtual void Set2DExtractionInterface( SUB_EXTRACTION_INTERFACE_ABC* 
		                                     a2DExtractionInterface );

	static const UUID myID;
	static const char *myName;

	virtual unsigned long StartExtracting(void);


private:
	MultiViewExtractionTool *m_ExtractionTool;
};

//=============================================================================
class MultiViewExtractionTool: public DescriptorExtractor
{
friend class MultiViewExtractionInterface;
public:
	MultiViewExtractionTool();
	MultiViewExtractionTool(MultiViewDescriptorInterfaceABC *MultiView);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual MultiViewDescriptorInterfaceABC *GetDescriptorInterface(void);
	virtual int SetDescriptorInterface(MultiViewDescriptorInterfaceABC
				   * aMultiViewDescriptorInterface);
	
	virtual MultiViewExtractionInterfaceABC *GetInterface(void);

	virtual int SetSourceMedia(MultiMediaInterfaceABC **media);


	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DON'T USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;

	virtual unsigned long StartExtracting(void);

	// accessor methods for subdescriptor
	virtual SUB_EXTRACTION_INTERFACE_ABC* Get2DExtractionInterface();
	virtual void Set2DExtractionInterface( SUB_EXTRACTION_INTERFACE_ABC* 
		                                     a2DExtractionInterface );

private:
	virtual ~MultiViewExtractionTool();

	MultiViewExtractionInterface m_Interface;
	MultiViewDescriptorInterfaceABC *m_pMultiViewDescriptorInterface;

	// Private functions used for shape extraction in view images 
	
	SUB_EXTRACTION_INTERFACE_ABC* m_p2DExtractionInterface;

	MultiMediaInterfaceABC **m_Media;

};

};


#endif //__MultiViewExtraction_H__
