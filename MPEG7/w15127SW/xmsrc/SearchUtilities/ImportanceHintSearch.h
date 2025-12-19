///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation ImportanceHint.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation ImportanceHint tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  ImportanceHintSearch.h
//
#ifndef __IMPORTANCEHINTSEARCH_H__
#define __IMPORTANCEHINTSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"


//=============================================================================

namespace XM
{

// Forward Declarations:
class ImportanceHintSearchTool;

//=============================================================================
class ImportanceHintSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ImportanceHintSearchInterfaceABC();
	virtual ~ImportanceHintSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface) = 0;
	//sth not the required interface
	virtual int SetImportanceHintParms(const char* mediafile, const char* hintfile) = 0;//sth I guess SetSourceMedia would be the right name
	virtual unsigned long StartTranscoding(void) = 0;
	//sth not the required name of the interface

	static const UUID myID;
};

//=============================================================================
class ImportanceHintSearchInterface: 
  public ImportanceHintSearchInterfaceABC
{
public:
	ImportanceHintSearchInterface(ImportanceHintSearchTool* aTool);
	virtual ~ImportanceHintSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface);
	virtual int SetImportanceHintParms(const char* mediafile, const char* hintfile);
	virtual unsigned long StartTranscoding(void);

	static const UUID myID;
	static const char *myName;

private:
	ImportanceHintSearchTool *m_SearchTool;
};

//=============================================================================
class ImportanceHintSearchTool: public Search
{
friend class ImportanceHintSearchInterface;
public:
        ImportanceHintSearchTool();
	ImportanceHintSearchTool(GenericDSInterfaceABC *theParent);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual ImportanceHintSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC 
		*aRefGenericDSInterface);
	virtual int SetImportanceHintParms(const char* mediafile, const char* hintfile);
	virtual unsigned long StartTranscoding(void);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~ImportanceHintSearchTool();

	char *m_MediaFile;
	char *m_HintFile;
	ImportanceHintSearchInterface m_Interface;
	GenericDSInterfaceABC *m_DescriptionInterface;

};

};


#endif //__IMPORTANCEHINTSEARCH_H__
