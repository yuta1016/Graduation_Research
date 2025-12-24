////////////////////////////////////////////////////////////////////////
//
// UserPreferenceSearch.h
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


#ifndef __USER_PREFERENCE_SEARCH_H__
#define __USER_PREFERENCE_SEARCH_H__
#include <string>
#include <vector>
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "MediaMetaSearch.h"		// DOMQuery classes

//=============================================================================

namespace XM
{

// Forward Declarations:
class UserPreferenceSearchTool;
class FilterManager;

//=============================================================================
class UserPreferenceSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	UserPreferenceSearchInterfaceABC();
	virtual ~UserPreferenceSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int  SetValuesFromParameterFile(void) = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class UserPreferenceSearchInterface: 
  public UserPreferenceSearchInterfaceABC
{
public:
	UserPreferenceSearchInterface(UserPreferenceSearchTool* aTool);
	virtual ~UserPreferenceSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy(void);

	virtual int  SetValuesFromParameterFile(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	UserPreferenceSearchTool *m_SearchTool;
};

//=============================================================================
class UserPreferenceSearchTool: public Search
{
friend class UserPreferenceSearchInterface;
public:
	UserPreferenceSearchTool();
	UserPreferenceSearchTool(GenericDSInterfaceABC *aDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual UserPreferenceSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int  SetValuesFromParameterFile(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~UserPreferenceSearchTool();

	UserPreferenceSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;
	FilterManager *m_FilterManager;
};

// Forward declarations
class FilterAgentDOM;
class PreferenceCondition;
class UsagePreferenceMap;
class UsagePreferenceList;
//=============================================================================
struct UserPreferenceEnvironment
{
};

//=============================================================================
class PreferenceQuery
{
public:
	PreferenceQuery();
	PreferenceQuery(const PreferenceQuery &pq) : m_DOMQuery(pq.m_DOMQuery) {}
	~PreferenceQuery();

	double CheckSimilarity(GenericDS DOMDocNode);
	double BuildQueryFromString(string aQuery) {return m_DOMQuery.BuildQueryFromString(aQuery.c_str());}

private:
	DOMQuery m_DOMQuery;
};

//=============================================================================
class FilterAgentDOM
{
public:
	FilterAgentDOM();
	~FilterAgentDOM();

	double CheckSimilarity(GenericDS DOMDocNode);

	double m_MaxScore;

private:
	PreferenceQuery m_PreferenceQuery;

	friend class FilterAgentFactory;
};

//=============================================================================
// The XM implementation of FilterManager does not persist a list of sorted ProgramDS
// TODO add overloaded methods for other non-DOM platforms
class FilterManager
{
public:
	FilterManager() : m_FilterAgentDOM(0) {};
	~FilterManager() {};

	int SetUserPreference(GenericDS userPreferenceRoot);
	//int AddProgram(DOM_Document programDoc)
	double Filter(GenericDS DOMDocNode);
	//void Filter();	// (Re)Filter all programs
	double GetMaxScore() {return m_FilterAgentDOM ? m_FilterAgentDOM->m_MaxScore : 1.0;}

private:
	FilterAgentDOM *m_FilterAgentDOM;	// Made by factory
	UserPreferenceEnvironment m_Environment;	// Keep a copy in the manager
};

//=============================================================================
class UsagePreferenceMap
{
public:
	enum GroupOp
	{
		NA_GO,
		OR,
		HOR,
		AND,
		HAND,
		ORAND
	};

	enum TestOp
	{
		NA_TO,
		STR_ISUB,
		STR_SUB,
		STR_IEQ,
		STR_EQ,
		STR_CNT,
		VAL_EQ,
		VAL_LT,
		VAL_GT,
		VAL_GT_LT,
		VAL_LTE,
		VAL_GTE,
		VAL_GTE_LTE,
		VAL_GTE_LT
	};

	virtual int FindPreferenceMap(string &nodeName, bool bCheckAncestry = true, int iStartMap = 0, int nCountMap = 0) = 0;
	//virtual int FindPreferenceMap(string nodeName) = 0;
	virtual const char *GetName(int iMap) = 0;
	virtual const char *GetMappedLocation(int iMap) = 0;
	virtual const char *GetMappedTestOperator(int iMap) = 0;
	virtual const char *GetMappedTestOptions(int iMap) = 0;
	virtual GroupOp GetMappedIntraOp(int iMap, bool bToggle = false) = 0;
	virtual GroupOp GetMappedInterOp(int iMap) = 0;
	virtual TestOp  GetMappedTestOp(int iMap) = 0;	// TODO too similar to GetMappedTestOperator
	static const char *GetOperator(GroupOp iMap);	// Calls DOMQuery::GetOperator()
	static GroupOp StringToGroupOp(const char *s);
	static TestOp  StringToTestOp(const char *s);
};

//=============================================================================
class FilterAgentFactory
{
public:
	FilterAgentFactory();
	~FilterAgentFactory();

	FilterAgentDOM *MakeFilterAgentDOM(GenericDS userPreferenceRoot);
	// TODO should this be in UsagePreferenceMap class
	UsagePreferenceList *m_UPList;
	UsagePreferenceMap  *m_UPMap;

private:
	string BuildMappedQuery( GenericDS &node, string progPath, string prefPath, bool bRecursToggleIntra = false );
	PreferenceCondition BuildPreferenceCondition( GenericDS &prefConditionNode );
	string CombineExpressions( string &baseExp, string &intraExp, 
								const string &newExp, 
								UsagePreferenceMap::GroupOp intraOp,
								UsagePreferenceMap::GroupOp interOp,
								UsagePreferenceMap::GroupOp prevInter,
								bool inIntraGroup );
};

//=============================================================================
// Derived classes must provide appropriate constructor (e.g. DOM_Node)
// TODO scrub UsagePreference vs UserPreference
class UsagePreferenceList
{
public:
	virtual bool GetNextPreferenceNode(GenericDS &parent, GenericDS &node) = 0;
	virtual bool GetNextPreferenceType(GenericDS &parent, GenericDS &node) = 0;
//	virtual int GetPreferenceValue(GenericDS &node) = 0;
};

//=============================================================================
class MPEG7UPList : public UsagePreferenceList
{
public:
	MPEG7UPList(GenericDS userPreferenceRoot);
	virtual ~MPEG7UPList();

	virtual bool GetNextPreferenceNode(GenericDS &parent, GenericDS &node);
	virtual bool GetNextPreferenceType(GenericDS &parent, GenericDS &node);
//	virtual int GetPreferenceValue(GenericDS &node);

	GenericDS GetUsagePreferenceNode();

private:
	GenericDS m_UserPreferenceRoot;

	bool GetNextNode(GenericDS &parent, GenericDS &node, int iStartMap, int nCountMap);
};

//=============================================================================
struct MPEG7UPMapEntry
{
	MPEG7UPMapEntry() {};
	MPEG7UPMapEntry(UsagePreferenceMap::GroupOp _intraOp, UsagePreferenceMap::GroupOp _interOp,
			UsagePreferenceMap::TestOp _testOp, char *_name, char *_location) : 
			intraOp(_intraOp),
			interOp(_interOp),
			testOp(_testOp),
			name(_name),
			location(_location) {};

	UsagePreferenceMap::GroupOp intraOp;
	UsagePreferenceMap::GroupOp interOp;
	UsagePreferenceMap::TestOp testOp;
	char *name;
	char *location;
};

//=============================================================================
const int MPEG7UP_MAX_NMAP = 1000;
class MPEG7UPMap : public UsagePreferenceMap
{
public:
	int FindPreferenceMap(string &nodeName, bool bCheckAncestry = true, int iStartMap = 0, int nCountMap = 0);
	//virtual int FindPreferenceMap(string nodeName);
	virtual const char *GetName(int iMap);
	virtual const char *GetMappedLocation(int iMap);
	virtual const char *GetMappedTestOperator(int iMap);
	virtual const char *GetMappedTestOptions(int iMap);
	virtual GroupOp GetMappedIntraOp(int iMap, bool bToggle = false);
	virtual GroupOp GetMappedInterOp(int iMap) {return map[iMap].interOp;}
	virtual TestOp  GetMappedTestOp(int iMap) {return map[iMap].testOp;}
//	static const char *GetOperator(GroupOp op);	// Calls DOMQuery::GetOperator()

	static MPEG7UPMapEntry map[MPEG7UP_MAX_NMAP];
	static int nMap;
};

};
#endif //__USER_PREFERENCE_SEARCH_H__

