///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ModelExtraction.h
//
#ifndef __MODELEXTRACTION_H__
#define __MODELEXTRACTION_H__
#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"

#include <stdio.h>
#include <math.h>
#ifdef XYUVDISP
#include <xyuv.h>
#define FPWIN 1
#endif

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */

#include <iostream>
#include <fstream>

//#ifdef USEXML
//#include "XML4Cutils.h" // sth not permitted here
//#endif

#define NIX 0.0

namespace XM
{

// Forward Declarations:
class ModelExtractionTool;

//=============================================================================
class ModelExtractionInterfaceABC: public I_InterfaceABC
{
public:
	ModelExtractionInterfaceABC();
	virtual ~ModelExtractionInterfaceABC() {};

	virtual void destroy() = 0; // sth

	virtual int SetSourceMedia(std::ifstream& inf) = 0;
	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface) = 0;

	virtual unsigned long InitExtracting(void) = 0;
	virtual unsigned long StartExtracting(void) = 0;
	virtual unsigned long PostExtraction(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ModelExtractionInterface: 
  public ModelExtractionInterfaceABC
{
public:
	ModelExtractionInterface(ModelExtractionTool* aTool);
	virtual ~ModelExtractionInterface();
	
	virtual void destroy();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetSourceMedia(std::ifstream& inf);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC
					    *aGenericDSInterface);

	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	static const UUID myID;
	static const char *myName;

private:
	ModelExtractionTool *m_ExtractionTool;
};


//=============================================================================
class ModelExtractionTool: public DescriptorExtractor
{
friend class ModelExtractionInterface;
public:
	ModelExtractionTool();
	ModelExtractionTool(GenericDSInterfaceABC *theParent); // sth

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetSourceMedia(std::ifstream& inf);//sth should give MultiMedia

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterfaceABC);
	
	virtual unsigned long InitExtracting(void);
	virtual unsigned long StartExtracting(void);
	virtual unsigned long PostExtraction(void);

	virtual ModelExtractionInterfaceABC *GetInterface(void);

	// access is allowed only by class factories for this
	// object.  This avoids having to duplicate the
	// ID definition in multiple locations.  In the future, we may
	// have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
	// IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
	static const UUID myID;
	static const char * myName;
private:
	virtual ~ModelExtractionTool(); // sth

	ModelExtractionInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

	std::ifstream* m_Inf;

	GenericDSInterfaceABC *ExtractProbabilityModelClass(int iModelID, 
										GenericDSInterfaceABC *parentInterface);

	GenericDSInterfaceABC *ExtractDescriptorCollection(int iModelID, 
										GenericDSInterfaceABC *parentInterface);

	GenericDSInterfaceABC *ExtractContentCollection(int iModelID, 
										GenericDSInterfaceABC *parentInterface);



};

};

#endif //__MODELEXTRACTION_H__
