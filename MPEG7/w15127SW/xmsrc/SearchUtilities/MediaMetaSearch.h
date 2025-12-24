////////////////////////////////////////////////////////////////////////
//
// MediaMetaSearch.h
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


#ifndef __MEDIA_META_SEARCH_H__
#define __MEDIA_META_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include <assert.h>

//=============================================================================

namespace XM
{


// Non-member functions
char *SkipOverPair(char *string);
char *RevSkipOverPair( char *string, char *pClose = 0 );
string RemoveRedundantGroupings( const char *s );
int FindLeftmostHighestOperator(char *aQuery, char *&leftOperand, char *&rightOperand,
								 int binaryOperators, int unaryOperators );
int FindRightmostHighestOperator(char *aQuery, char *&leftOperand, char *&rightOperand,
								 int binaryOperators, int unaryOperators );
double ConvertYMDToJulian(int Y, int M, int D);
extern const char OPEN_DELIM[];
extern const char CLOSE_DELIM[];
extern const char QUOTE_DELIM[];


// Forward Declarations:
class MediaMetaSearchTool;
class DOMQuery;
class DOMQueryOperation;
class NodeTest;

//=============================================================================
class MediaMetaSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	MediaMetaSearchInterfaceABC();
	virtual ~MediaMetaSearchInterfaceABC() {};

	virtual void destroy(void) = 0;

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface) = 0;
	virtual int SetQuery(char *aQuery) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class MediaMetaSearchInterface:public MediaMetaSearchInterfaceABC
{
public:
	MediaMetaSearchInterface(MediaMetaSearchTool* aTool);
	virtual ~MediaMetaSearchInterface();
	
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
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	MediaMetaSearchTool *m_SearchTool;
};

//=============================================================================
class MediaMetaSearchTool: public Search
{
friend class MediaMetaSearchInterface;
public:
	MediaMetaSearchTool();
	MediaMetaSearchTool(GenericDSInterfaceABC *aDescription);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual MediaMetaSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQueryDescriptionInterface
	  (GenericDSInterfaceABC
	   *aGenericDSInterface);
	virtual int SetQuery(char *aQuery);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~MediaMetaSearchTool();

	MediaMetaSearchInterface m_Interface;
	GenericDSInterfaceABC *m_RefDescriptionInterface;
	GenericDSInterfaceABC *m_QueryDescriptionInterface;
	DOMQuery *m_Query;
};

class DOMQueryOperation;
class NodeList;
//=============================================================================
class DOMQuery
{
public:
	DOMQuery();
	DOMQuery(const DOMQuery &dq) {assert(dq.m_queryString);BuildQueryFromString(dq.m_queryString);}
	~DOMQuery();

	double BuildQueryFromString(const char *aQuery);
	DOMQueryOperation *BuildOperationFromString(char *aQuery, double &maxScore);
//	int AddOperation(DOMQueryOperation &);
	unsigned long GetDistance(GenericDS DOMRoot);
	double CheckSimilarity(GenericDS DOMRoot);

	double m_MaxScore;

private:
	DOMQueryOperation *m_Operation;
	char *m_queryString;

public:
	static const int UNARY_OPERATORS;
	static const int BINARY_OPERATORS;

	// The OPERATOR_TAG and OPERATOR_INDEX must match, and the total should be less than 32
	#define OPERATORS "^SINGLE_QUERY^","&","|","!","^SAND^","^SOR^","^HAND^","^HOR^","^NOT^","+","-","*","/","&"
	static const char* Operators[];
	enum OperatorTypes
	{
		SINGLE_QUERY_OPERATOR, HARD_AND_OPERATOR, HARD_OR_OPERATOR, NOT_OPERATOR, SOFT_AND_OPERATOR, SOFT_OR_OPERATOR, HARD_AND_OPERATOR_tag, HARD_OR_OPERATOR_tag, NOT_OPERATOR_tag,
		ADDITION_OPERATOR, SUBTRACTION_OPERATOR, MULTIPLICATION_OPERATOR, DIVISION_OPERATOR, CONCATENATION_OPERATOR, NUM_OPERATORS
	};
	static char* GetOperator(OperatorTypes op) {assert(op >= 0); assert(op < NUM_OPERATORS); return (char *)Operators[op];}

};

//=============================================================================
const unsigned int DOMQueryOperation_MAX_OPERANDS = 2;
class DOMQueryOperation
{
public:
	DOMQueryOperation();
	virtual ~DOMQueryOperation();

	virtual double Evaluate(NodeList &evalList, NodeList &resultList) = 0;
	void AddOperand( DOMQueryOperation *operand );
	double m_NormPreferenceValue;
	unsigned int m_NumOperands;
	
	static const double NOMINAL_PREFERENCE_VALUE;
	static const char   DECIMAL_CHARS[];

protected:
	DOMQueryOperation *m_Operands[DOMQueryOperation_MAX_OPERANDS];
};

//=============================================================================
class HardAndOperation : public DOMQueryOperation
{
public:
	virtual double Evaluate(NodeList &evalList, NodeList &resultList);
};

//=============================================================================
class SoftAndOperation : public DOMQueryOperation
{
public:
	virtual double Evaluate(NodeList &evalList, NodeList &resultList);
};

//=============================================================================
class HardOrOperation : public DOMQueryOperation
{
public:
	virtual double Evaluate(NodeList &evalList, NodeList &resultList);
};

//=============================================================================
class SoftOrOperation : public DOMQueryOperation
{
public:
	virtual double Evaluate(NodeList &evalList, NodeList &resultList);
};

//=============================================================================
class NotOperation : public DOMQueryOperation
{
public:
	virtual double Evaluate(NodeList &evalList, NodeList &resultLis);
};

//=============================================================================
class SingleQueryOperation : public DOMQueryOperation
{
public:
	SingleQueryOperation();
	virtual ~SingleQueryOperation();

	virtual double Evaluate(NodeList &evalList, NodeList &resultList);
	void SetSingleQuery(char *singleQuery, double &maxScore);
	char *GetLocation();

	int  SaveNodes() {return m_saveNode;}	// TODO SaveNode singular?
	void SaveNodes(int a) {m_saveNode = a;}
	NodeTest *m_NodeTest;

private:
	char *m_LocationSpecifier;

	int m_saveNode;	//  level (back from leaf) to save, starting at 1, 0 indicates do not
};

//=============================================================================
class NodeTest
{
public:
	NodeTest(char *queryString, const char *optionString = 0, int type = 0);
	virtual ~NodeTest();

	virtual double Match(NodeList &testList) = 0;
	virtual bool Aggregate() {return false;}

	double m_NormPreferenceValue;

protected:
	char *m_QueryString;
};

//=============================================================================
class SingleStringMatch : public NodeTest
{
public:
	SingleStringMatch(char *queryString, const char *optionString = 0, int type = 0);

	virtual double Match(NodeList &testList);

	int  m_TestCnt;	// Default 1, count of occurrences tested for
	double  m_MaxScore;	// Default 1.0
	double  m_MinScore;	// Default 1.0

private:
	bool m_CaseSensitive;
	bool m_ExactMatch;
};

//=============================================================================
class SingleCustomMatch : public NodeTest
{
public:
	SingleCustomMatch(char *queryString, const char *optionString = 0, int type = 0);

	virtual double Match(NodeList &testList);

	void SetMatchFunction( double (*f)(const char *, const char *, const char *) ) {m_MatchFunc = f;}

private:
	double (*m_MatchFunc)(const char *refString, const char *queryString, const char *funcOptionString);
};

//=============================================================================
class NodeCountCompare : public NodeTest
{
public:
	enum NodeCountComparisonType
	{
		EQUAL,
		LESS_THAN,
		GREATER_THAN,
		LESS_THAN_GREATER_THAN
	};

	NodeCountCompare(char *queryString, const char *optionString = 0, NodeCountComparisonType type = EQUAL );

	virtual double Match(NodeList &testList);
	virtual bool Aggregate() {return true;}

private:
	NodeCountComparisonType	m_CompareType;
	int	m_Compare1;
	int	m_Compare2;
};

//=============================================================================
// TODO many of these methods should be made into global functions
class DOMNavigator
{
public:
	GenericDS GetNode(GenericDS parent, char *location, string &attrName, char *dbgPath = 0);
	string GetText(GenericDS node, bool preserveWhiteSpace, const string &attrName = "");
	char *InitLocation(char *location);
	GenericDS GetChildByTagName(GenericDS node, string name, unsigned int iInst = 0);
	char *GetLocation(GenericDS node, string attrName);
	GenericDS GetAncestorNode(GenericDS node, int ancestorLevel);

	static string GetAncestorPath(const GenericDS node, int depth = 20);

	static const char WHITESPACE[];

private:
	GenericDS GetCompositeNode(GenericDS parent, char *location, string &attrName, char *dbgPath = 0);
	GenericDS GetTimeNode(GenericDS node, char *location, string &attrName, char *dbgPath = 0);
	string ConvertFractionalTime(string tp, GenericDS node);
	string ConvertFractionalDuration(string dur, GenericDS node);
	double GetTimeUnit(GenericDS node);
	//double GetTimeBase(GenericDS node);
	//long GetDefaultFractionOfSecond(GenericDS node);
	//double GetDefaultTimeBase(GenericDS node);
	int  CheckElementNumber(char *&element);
	int  SetElementNumber(char *element, int num);

	static const char ELEM_NUM;
	static const char ELEM_NUM_DISABLED;
	static const char ELEM_NUM_WILDCARD;
	static const int  MAX_LOCATION_SIZE;
	static const int  ELEM_NUM_WIDTH;

public:
	static const char NOT_A_NUMBER[];
	static const char COMPOSITE_NODE_ATTR_NAME[];
	static const int COMPOSITE_NODE_UNARY_OPERATORS;
	static const int COMPOSITE_NODE_BINARY_OPERATORS;
};

//=============================================================================
class DOMQueryDS
{
public:
	int SetDOMQuery(DOMQuery *aDOMQuery);
	int BuildQueryFromDOM(GenericDSInterfaceABC *DOMDoc);

private:
	char *BuildQueryStringFromNode(GenericDS node);
	char *AppendQueryString(char *&queryString, char *text1, const char *QueryText = 0);

	DOMQuery *m_DOMQuery;
};

//=============================================================================
const unsigned int NodeList_INIT_NODE_LIST = 16;
class NodeList
{
public:
	NodeList();
	NodeList(GenericDS node, const string &attrName );	// TODO default attrName ?
	~NodeList();

	bool Active() {return m_active;}
	void Active(bool a) {m_active = a;}
	unsigned int GetLength() {return m_numNodes;}
	GenericDS Item(unsigned int iNode) {return *m_nodes[iNode];}	// Returns copy
	string ItemAttrName(unsigned int iNode) {return m_attrNames[iNode];}	// Returns copy
	void Clear();

	void Append(GenericDS node, const string &attrName);
	void Append(NodeList nodeList);

private:
	bool m_active;	// indicates whether calling function wants nodes

	unsigned int m_numNodes;	// Number of valid node pointers
	GenericDS *m_fastNodes[NodeList_INIT_NODE_LIST];
	GenericDS **m_nodes;	// the xml4c doc says not to new DOM_Nodes directly!
	string m_fastAttrNames[NodeList_INIT_NODE_LIST];
	string *m_attrNames;
	unsigned int m_nodeListLength;	// Buffer space for node pointers
};
//=============================================================================
double CustomMatchFunc_Equal(							const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_LessThan(						const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_GreaterThan(						const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_GreaterThan_LessThan(			const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_LessThanEqual(					const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_GreaterThanEqual(				const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_GreaterThanEqual_LessThanEqual(	const char *refString, const char *queryString, const char *funcOptionString);
double CustomMatchFunc_GreaterThanEqual_LessThan(		const char *refString, const char *queryString, const char *funcOptionString);

};

#endif //__MEDIA_META_SEARCH_H__
