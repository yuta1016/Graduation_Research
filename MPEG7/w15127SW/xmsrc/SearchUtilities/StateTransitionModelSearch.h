///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
//
// in the course of development of the MPEG-7 Experimentation StateTransitionModel.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation StateTransitionModel tools as specified by the MPEG-7 Requirements.
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
// Applicable File Name:  StateTransitionModelSearch.h
//
#ifndef __STATETRANSITIONMODELSEARCH_H__
#define __STATETRANSITIONMODELSEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"


//=============================================================================

namespace XM
{

// Forward Declarations:
class StateTransitionModelSearchTool;

//=============================================================================
class StateTransitionModelSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	StateTransitionModelSearchInterfaceABC();
	virtual ~StateTransitionModelSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget) = 0;
	virtual int SetStateTransitionModelParms(const char* queryfile, const char* targetfile) = 0;
	virtual unsigned long StartMatching(void) = 0;

	static const UUID myID;
};

//=============================================================================
class StateTransitionModelSearchInterface: 
  public StateTransitionModelSearchInterfaceABC
{
public:
	StateTransitionModelSearchInterface(StateTransitionModelSearchTool* aTool);
	virtual ~StateTransitionModelSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget);
	virtual int SetStateTransitionModelParms(const char* queryfile, const char* targetfile);
	virtual unsigned long StartMatching(void);

	static const UUID myID;
	static const char *myName;

private:
	StateTransitionModelSearchTool *m_SearchTool;
};

//=============================================================================
class StateTransitionModelSearchTool: public Search
{
friend class StateTransitionModelSearchInterface;
public:
        
	StateTransitionModelSearchTool();
	StateTransitionModelSearchTool(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual StateTransitionModelSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetDescriptionInterface(GenericDSInterfaceABC *theQuery, GenericDSInterfaceABC *theTarget);
	virtual int SetStateTransitionModelParms(const char* queryfile, const char* targetfile);
	virtual unsigned long StartMatching(void);

	static const UUID myID;
	static const char * myName;


private:
	virtual ~StateTransitionModelSearchTool();

	char *m_QueryFile;
	char *m_TargetFile;
	StateTransitionModelSearchInterface m_Interface;
	GenericDSInterfaceABC *m_QueryInterface;
	GenericDSInterfaceABC *m_TargetInterface;

};

};


#endif //__STATETRANSITIONMODELSEARCH_H__
