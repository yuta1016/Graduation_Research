////////////////////////////////////////////////////////////////////////
//
// ColorStructureSearch.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////

#ifndef __COLORSTRUCTURESEARCH_H__
#define __COLORSTRUCTURESEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class ColorStructureSearchTool;

//=============================================================================
class ColorStructureSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ColorStructureSearchInterfaceABC();
	virtual ~ColorStructureSearchInterfaceABC() {};

	virtual void destroy()=0;

	virtual int SetRefDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface) = 0;
	virtual ColorStructureDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void) = 0;
	virtual int SetQueryDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	/* sub-components do not have get and set functions because
	there is only one output value*/

	static const UUID myID;
};

//=============================================================================
class ColorStructureSearchInterface: 
public ColorStructureSearchInterfaceABC
{
public:
	ColorStructureSearchInterface(ColorStructureSearchTool* aTool);
	virtual ~ColorStructureSearchInterface();

	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface);
	virtual ColorStructureDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	ColorStructureSearchTool *m_SearchTool;
};

//=============================================================================
class ColorStructureSearchTool: public Search
{
friend class ColorStructureSearchInterface;
public:
	ColorStructureSearchTool();
	ColorStructureSearchTool(ColorStructureDescriptorInterfaceABC
							*aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);

	virtual ColorStructureSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface);
	virtual ColorStructureDescriptorInterfaceABC*
		GetQueryDescriptorInterface(void);
	virtual int SetQueryDescriptorInterface
		(ColorStructureDescriptorInterfaceABC
			*aColorStructureDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ColorStructureSearchTool();

	ColorStructureSearchInterface m_Interface;
	ColorStructureDescriptorInterfaceABC *m_RefDescriptorInterface;
	ColorStructureDescriptorInterfaceABC *m_QueryDescriptorInterface;
};

};

#endif //__COLORSTRUCTURESEARCH_H__
