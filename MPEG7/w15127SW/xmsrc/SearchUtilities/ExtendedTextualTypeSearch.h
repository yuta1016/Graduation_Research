////////////////////////////////////////////////////////////////////////
//
// ExtendedTextualTypeSearch.h
//
// This software module was originally developed by
//
// Andre Mengel, Bosch GmbH, Hildesheim
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
// Copyright (c) 2001
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#ifndef __EXTENDED_TEXTUAL_TYPE_SEARCH_H__
#define __EXTENDED_TEXTUAL_TYPE_SEARCH_H__

#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "MediaMetaSearch.h"


//=============================================================================

namespace XM
{


// Forward Declarations:
class ExtendedTextualTypeSearchTool;

//=============================================================================
class ExtendedTextualTypeSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ExtendedTextualTypeSearchInterfaceABC();
	virtual ~ExtendedTextualTypeSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQuery(char *aQuery) = 0;
	//sth std interface functions are missing
	virtual void GetNodes(NodeList &m_NodeList) {};

	static const UUID myID;
};

//=============================================================================
class ExtendedTextualTypeSearchInterface:public ExtendedTextualTypeSearchInterfaceABC
{
public:
	ExtendedTextualTypeSearchInterface(ExtendedTextualTypeSearchTool* aTool);
	virtual ~ExtendedTextualTypeSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQuery(char *aQuery);
	virtual void GetNodes(NodeList &m_NodeList);


	static const UUID myID;
	static const char *myName;

private:
	ExtendedTextualTypeSearchTool *m_SearchTool;
};

//=============================================================================
class ExtendedTextualTypeSearchTool: public Search
{
friend class ExtendedTextualTypeSearchInterface;
public:
	ExtendedTextualTypeSearchTool();
	ExtendedTextualTypeSearchTool(GenericDSInterfaceABC *aDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual ExtendedTextualTypeSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQuery(char *aQuery);
	virtual void GetNodes(NodeList &m_NodeList);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ExtendedTextualTypeSearchTool();

	ExtendedTextualTypeSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;
	string m_tagname;
	string m_phtranscription;
};


};

#endif //__EXTENDED_TEXTUAL_TYPE_SEARCH_H__
