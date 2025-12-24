////////////////////////////////////////////////////////////////////////
//
// MediaMetaSearch.cpp
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


#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifdef _VISUALC_
#include <sstream>
#endif
#include <cstring>

#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "MediaMetaSearch.h"	//TODO remove after development is stable

//=============================================================================
	

using namespace XM;
using namespace std;


const UUID MediaMetaSearchInterface::myID = UUID("");
const char *MediaMetaSearchInterface::myName = "Media Meta Search Interface";

const UUID MediaMetaSearchTool::myID = UUID("");
const char *MediaMetaSearchTool::myName = "Media Meta Search Tool";

const UUID MediaMetaSearchInterfaceABC::myID = UUID();

//const double DOMQueryOperation::RESOLUTION = 0.01;	// Granularity: 0.01 gives distances 0-100
const double DOMQueryOperation::NOMINAL_PREFERENCE_VALUE = 10.0;
const char   DOMQueryOperation::DECIMAL_CHARS[] = "0123456789.-";
/*
const char *DOMQuery::SINGLE_QUERY_OPERATOR = " ";
const char *DOMQuery::AND_OPERATOR = "&";
const char *DOMQuery::OR_OPERATOR = "|";
const char *DOMQuery::NOT_OPERATOR = "!";
const char *DOMQuery::UNARY_OPERATORS[] = { NOT_OPERATOR, 0 };
const char *DOMQuery::BINARY_OPERATORS[] = { AND_OPERATOR, OR_OPERATOR, 0 };
*/
//const char XM::OPEN_DELIM[] = "(";
//const char XM::CLOSE_DELIM[] = ")";
const char XM::OPEN_DELIM[] = "([\"";
const char XM::CLOSE_DELIM[] = ")]\"";
const char XM::QUOTE_DELIM[] = "\"";

const char DOMNavigator::COMPOSITE_NODE_ATTR_NAME[] = "CompositeNodeValue";
const char DOMNavigator::NOT_A_NUMBER[] = "NaN";
//const char *DOMNavigator::ADDITION_OPERATOR = "+";
//const char *DOMNavigator::SUBTRACTION_OPERATOR = "-";
//const char *DOMNavigator::MULTIPLICATION_OPERATOR = "*";
//const char *DOMNavigator::DIVISION_OPERATOR = "/";
//const char *DOMNavigator::CONCATENATION_OPERATOR = "&";
//const char *DOMNavigator::COMPOSITE_NODE_UNARY_OPERATORS[] = {0};
//const char *DOMNavigator::COMPOSITE_NODE_BINARY_OPERATORS[] = {ADDITION_OPERATOR, SUBTRACTION_OPERATOR, MULTIPLICATION_OPERATOR, DIVISION_OPERATOR, CONCATENATION_OPERATOR, 0};
const char DOMNavigator::WHITESPACE[] = "\n\t ";
const char DOMNavigator::ELEM_NUM = '#';
const char DOMNavigator::ELEM_NUM_DISABLED = '@';
const char DOMNavigator::ELEM_NUM_WILDCARD = '*';
const int  DOMNavigator::MAX_LOCATION_SIZE = 10240;
const int  DOMNavigator::ELEM_NUM_WIDTH = 3;


const char *DOMQuery::Operators[] = {OPERATORS};

// The OPERATOR_TAG and OPERATOR_INDEX must match, and the total should be less than 32
/*const char const*Operators[] = {"^SINGLE_QUERY^","&","|","!","^SAND^","^SOR^","+","-","*","/","&"};
const enum OperatorTypes
{
	SINGLE_QUERY_OPERATOR, HARD_AND_OPERATOR, HARD_OR_OPERATOR, NOT_OPERATOR, SOFT_AND_OPERATOR, SOFT_OR_OPERATOR,
	ADDITION_OPERATOR, SUBTRACTION_OPERATOR, MULTIPLICATION_OPERATOR, DIVISION_OPERATOR, CONCATENATION_OPERATOR
};
*/
const int DOMQuery::UNARY_OPERATORS = 1<<NOT_OPERATOR | 1<<NOT_OPERATOR_tag;
const int DOMQuery::BINARY_OPERATORS= 1<<SINGLE_QUERY_OPERATOR | 1<<HARD_AND_OPERATOR | 1<<HARD_OR_OPERATOR | 1<<SOFT_AND_OPERATOR | 1<<SOFT_OR_OPERATOR | 1<<HARD_AND_OPERATOR_tag | 1<<HARD_OR_OPERATOR_tag;
const int DOMNavigator::COMPOSITE_NODE_UNARY_OPERATORS = 0;
const int DOMNavigator::COMPOSITE_NODE_BINARY_OPERATORS = 1<<DOMQuery::ADDITION_OPERATOR | 1<<DOMQuery::SUBTRACTION_OPERATOR | 1<<DOMQuery::MULTIPLICATION_OPERATOR | 1<<DOMQuery::DIVISION_OPERATOR | 1<<DOMQuery::CONCATENATION_OPERATOR;

// These operation strings must match the operationtype enums
const char *MatchOperations[] =
{ "", "strisub", "strsub", "strieq", "streq", "=", "<", ">", "><", "<=", ">=", ">=<=", ">=<", "#=", "#<", "#>", "#><" };
enum MatchOperationType
{ 
	MATCH_DEFAULT, MATCH_STR_ISUB, MATCH_STR_SUB, MATCH_STR_IEQ, MATCH_STR_EQ,
	MATCH_VAL_EQ, MATCH_VAL_LT, MATCH_VAL_GT, MATCH_VAL_GT_LT, MATCH_VAL_LTE, MATCH_VAL_GTE, MATCH_VAL_GTE_LTE, MATCH_VAL_GTE_LT,
	MATCH_CNT_EQ, MATCH_CNT_LT, MATCH_CNT_GT, MATCH_CNT_GT_LT
};
const int MAX_MATCH_OPERATION_WIDTH = 7;	// strisub

#define vout cerr
#define eout cerr

//=============================================================================
MediaMetaSearchInterfaceABC::MediaMetaSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
MediaMetaSearchInterface::MediaMetaSearchInterface(MediaMetaSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
MediaMetaSearchInterface::~MediaMetaSearchInterface()
{
}

//----------------------------------------------------------------------------
void MediaMetaSearchInterface::destroy()
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int MediaMetaSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(aRefGenericDSInterface);
}

//----------------------------------------------------------------------------
int MediaMetaSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(aQueryGenericDSInterface);
}

//----------------------------------------------------------------------------
int MediaMetaSearchInterface::
SetQuery(char *aQuery)

{
  return m_SearchTool->SetQuery(aQuery);
}

//----------------------------------------------------------------------------
double MediaMetaSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& MediaMetaSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaMetaSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
MediaMetaSearchTool::MediaMetaSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
	SetExposedInterface(&m_Interface);

	m_Query = new DOMQuery();
	assert( m_Query );
}

//----------------------------------------------------------------------------
MediaMetaSearchTool::MediaMetaSearchTool(GenericDSInterfaceABC *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
        SetQueryDescriptionInterface(aDescription);

	SetExposedInterface(&m_Interface);

	m_Query = new DOMQuery();
	assert( m_Query );
}

//----------------------------------------------------------------------------
MediaMetaSearchTool::~MediaMetaSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& MediaMetaSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *MediaMetaSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool MediaMetaSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool MediaMetaSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
MediaMetaSearchInterfaceABC *MediaMetaSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int MediaMetaSearchTool::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
	m_RefDescriptionInterface = aRefGenericDSInterface;
	return 0;
}

//----------------------------------------------------------------------------
// Attempt to build a full query from the Query XML file input.
//----------------------------------------------------------------------------
int  MediaMetaSearchTool::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)
{
	DOMQueryDS theDOMQueryDS;

	theDOMQueryDS.SetDOMQuery( m_Query );
	
	return theDOMQueryDS.BuildQueryFromDOM( aQueryGenericDSInterface );
}

//----------------------------------------------------------------------------
// Build a full query from the Query string input.
//----------------------------------------------------------------------------
int  MediaMetaSearchTool::SetQuery(char *aQuery)
{
	assert(m_Query);

	// Build the query
	return m_Query->BuildQueryFromString( aQuery ) == 0.0 ? 0 : 1;
}

//----------------------------------------------------------------------------
// Evaluate the query object on the reference DOM document.
//----------------------------------------------------------------------------
double MediaMetaSearchTool::GetDistance(void)
{
	// Assumptions
	assert(m_Query && m_RefDescriptionInterface);

	// Retrieve the reference doc
	GenericDS testDS = m_RefDescriptionInterface->GetChild((int)0);	// How do you get the GenericDS from the interface???
	GenericDS DOMDocNode = testDS.GetParent();
	while( ! (testDS = DOMDocNode.GetParent()).isNull() )
		DOMDocNode = testDS;

	return m_Query->GetDistance( DOMDocNode );
}

//=============================================================================
DOMQuery::DOMQuery() :
m_Operation(0),
m_queryString(0)
{
}

//----------------------------------------------------------------------------
DOMQuery::~DOMQuery()
{
	delete m_Operation;
}

//----------------------------------------------------------------------------
// Build the root query operation from the full query string,
// and the sub-queries are built recursively.
//----------------------------------------------------------------------------
double DOMQuery::BuildQueryFromString(const char *aQuery)
{
	char *aQueryCopy;	
	assert(aQuery);

	// Copy the string, lower functions are going to modify it
	aQueryCopy = new char[strlen(aQuery)+1];
	assert(aQueryCopy);
	strcpy(aQueryCopy,aQuery);

	// Retain copy of string for posterity
	m_queryString = new char[strlen(aQuery)+1];
	assert(m_queryString);
	strcpy(m_queryString,aQuery);

	// Build the highest operation (recursive) and calculate maximum score
	m_MaxScore = 1.0;
	m_Operation = BuildOperationFromString(aQueryCopy, m_MaxScore);

	delete aQueryCopy;

	return m_MaxScore;
}

//----------------------------------------------------------------------------
// Build a single query operation from the given portion of the full query string.
// Recursively builds sub-portions.  Should always return a single DOMQueryOperation.
//----------------------------------------------------------------------------
DOMQueryOperation *DOMQuery::BuildOperationFromString(char *aQuery, double &maxScore)
{
	int iOp;
	double leftMax, rightMax, normPreferenceValue = 1.0;
	char *leftOperandString = 0, *rightOperandString = 0;

	// Example of aQuery:  ((/Program/Stuff:Hi&/Program%quality:good)|!/Program/Summary:)

	// Assumptions
	assert( aQuery );

	// Building left to right at the highest level, find the next operator
	// This returns 0,1 or 2 operand strings, depending on operator
	iOp = FindLeftmostHighestOperator( aQuery, leftOperandString, rightOperandString,
											 BINARY_OPERATORS, UNARY_OPERATORS );

	// Parse operator extensions (if present)
	if (rightOperandString)
	{
		// Parse the preference value
		if (!strncmp(rightOperandString,"pv",2))
		{
			rightOperandString += 2;
			normPreferenceValue = atof(rightOperandString) / 
									DOMQueryOperation::NOMINAL_PREFERENCE_VALUE;
			while (*rightOperandString && strchr(DOMQueryOperation::DECIMAL_CHARS, *rightOperandString))
				rightOperandString ++;
		}
	}

	// Preset left/right max score
	leftMax = rightMax = maxScore;


	// Create this operation
	switch( iOp )
	{
	case SINGLE_QUERY_OPERATOR:	// aQuery didn't decompose further, leftOperand is the single query
		{
			SingleQueryOperation *searchOp = new SingleQueryOperation();
			assert( searchOp );
			searchOp->SetSingleQuery( leftOperandString, maxScore );
			// Preference value for single queries will always be 1.0  The testop may have non-unit pref value
			searchOp->m_NormPreferenceValue = normPreferenceValue;
			maxScore *= fabs( searchOp->m_NormPreferenceValue );
			return searchOp;
		}

	case HARD_AND_OPERATOR:
	case HARD_AND_OPERATOR_tag:
		{
#ifdef VERBOSE
			vout << "HARD AND operation on \"" << leftOperandString << "\", \"" << rightOperandString << "\"" << endl;
#endif
			HardAndOperation *andOp = new HardAndOperation();
			assert( andOp );
			andOp->AddOperand( BuildOperationFromString( leftOperandString, leftMax ) );
			andOp->AddOperand( BuildOperationFromString( rightOperandString, rightMax ) );
			andOp->m_NormPreferenceValue = normPreferenceValue;
			// TODO more accurately, the lesser score is the max
			maxScore = (leftMax > rightMax ? leftMax : rightMax) * fabs( andOp->m_NormPreferenceValue );
			return andOp;
		}

	case SOFT_AND_OPERATOR:
		{
#ifdef VERBOSE
			vout << "SOFT AND operation on \"" << leftOperandString << "\", \"" << rightOperandString << "\"" << endl;
#endif
			SoftAndOperation *andOp = new SoftAndOperation();
			assert( andOp );
			andOp->AddOperand( BuildOperationFromString( leftOperandString, leftMax ) );
			andOp->AddOperand( BuildOperationFromString( rightOperandString, rightMax ) );
			andOp->m_NormPreferenceValue = normPreferenceValue;
			maxScore = ((leftMax + rightMax) / 2) * fabs( andOp->m_NormPreferenceValue );
			return andOp;
		}

	case HARD_OR_OPERATOR:
	case HARD_OR_OPERATOR_tag:
		{
#ifdef VERBOSE
			vout << "HARD OR operation on \"" << leftOperandString << "\", \"" << rightOperandString << "\"" << endl;
#endif
			HardOrOperation *orOp = new HardOrOperation();
			assert( orOp );
			orOp->AddOperand( BuildOperationFromString( leftOperandString, leftMax ) );
			orOp->AddOperand( BuildOperationFromString( rightOperandString, rightMax ) );
			orOp->m_NormPreferenceValue = normPreferenceValue;
			maxScore = (leftMax > rightMax ? leftMax : rightMax) * fabs( orOp->m_NormPreferenceValue );
			return orOp;
		}

	case SOFT_OR_OPERATOR:
		{
#ifdef VERBOSE
			vout << "SOFT OR operation on \"" << leftOperandString << "\", \"" << rightOperandString << "\"" << endl;
#endif
			SoftOrOperation *orOp = new SoftOrOperation();
			assert( orOp );
			orOp->AddOperand( BuildOperationFromString( leftOperandString, leftMax ) );
			orOp->AddOperand( BuildOperationFromString( rightOperandString, rightMax ) );
			orOp->m_NormPreferenceValue = normPreferenceValue;
			maxScore = (leftMax + rightMax) * fabs( orOp->m_NormPreferenceValue );
			return orOp;
		}

	case NOT_OPERATOR:
	case NOT_OPERATOR_tag:
		{
#ifdef VERBOSE
			vout << "NOT operation on \"" << rightOperandString << "\"" << endl;
#endif
			NotOperation *notOp = new NotOperation();
			assert( notOp );
			notOp->AddOperand( BuildOperationFromString( rightOperandString, maxScore ) );
			notOp->m_NormPreferenceValue = normPreferenceValue;
			maxScore *= fabs( notOp->m_NormPreferenceValue );
			return notOp;
		}

	default:
		eout << "Undefined operator (" << iOp << ") found in query: " << aQuery << endl;
		exit(-1);
	}

}

//----------------------------------------------------------------------------
// Evaluate the root operation (and its sub-operations) and convert similarity to distance.
//----------------------------------------------------------------------------
unsigned long DOMQuery::GetDistance(GenericDS DOMDocNode)
{
	double similarity = CheckSimilarity( DOMDocNode );

	// Convert similarity measure to distance
	double distance = (m_MaxScore - similarity) / m_MaxScore *
						DOMQueryOperation::NOMINAL_PREFERENCE_VALUE;
	assert(distance >= 0.0);

	// Round ?
	return (unsigned long) floor(distance + 0.5);
}

//----------------------------------------------------------------------------
// Evaluate the root operation (and its sub-operations)
//----------------------------------------------------------------------------
double DOMQuery::CheckSimilarity(GenericDS DOMDocNode)
{
	// NodeList default inactive
	NodeList evalList(DOMDocNode,"");
	NodeList emptyList;
	return m_Operation->Evaluate( evalList, emptyList );
}

/*
//----------------------------------------------------------------------------
// Parse the given query string, handle grouping pairs () [],
// return the operation type and the 0, 1 or 2 operand strings.
//----------------------------------------------------------------------------
char XM::FindLeftmostHighestOperator( char *aQuery, 
								char *&leftOperandString, char *&rightOperandString,
								const char *binaryOperators, const char *unaryOperators)
{
	char *operatorPlusRight;	// points to the operator followed by rightOperand
	char operation;	// result

	// The query can begin with a single query, a unary operator, or a grouping paren
	if ( (operatorPlusRight = SkipOverPair(aQuery)) != aQuery )
	{
		// Found a grouping pair, it either groups the beginning or the entire string
		if ( operatorPlusRight == &aQuery[strlen(aQuery)] )
		{
			// Groups the entire string, strip and recurse
			aQuery[strlen(aQuery) - 1] = '\0';
			return FindLeftmostHighestOperator( aQuery + 1, leftOperandString, rightOperandString,
												binaryOperators, unaryOperators );
		}
		else
		{
			// Groups a portion of the beginning, the operator must be on the other side
//			leftOperandString = aQuery;
//			operatorPlusRight = SkipOverPair(aQuery);


			//Groups a portion of the beginning, Assume this starts with one query or group of queries
			leftOperandString = aQuery;
			// Scan through query to find leftmost operator
			operatorPlusRight = SkipOverPair( aQuery );
			while( *operatorPlusRight && !strchr(binaryOperators, *operatorPlusRight) )
				operatorPlusRight = SkipOverPair( operatorPlusRight + 1 );

			if ( !*operatorPlusRight )
			{
				// No operators, this whole query must be a single query
				rightOperandString = 0;
				return ' ';	//Space indicates single query
			}

		}
	}
	else if ( strchr( unaryOperators, *aQuery ) )
	{
		// Unary operator:  there is no left operand string
		leftOperandString = 0;
		operatorPlusRight = aQuery;
	}
	else
	{
		// Assume this starts with a single query
		leftOperandString = aQuery;

		// Check for a binary operator linking this single query with next operation
		operatorPlusRight = aQuery;
		// Scan through query to find leftmost operator
		while( *operatorPlusRight && !strchr(binaryOperators, *operatorPlusRight) )
			operatorPlusRight = SkipOverPair( operatorPlusRight + 1 );

		if ( !*operatorPlusRight )
		{
			// No operators, this whole query must be a single query
			rightOperandString = 0;
			return ' ';	// Space indicates single query
		}
	}

	// Separate the operator from the right operand string
	if ( !*operatorPlusRight )
		return '\0'; // Null indicates undefined

	rightOperandString = operatorPlusRight + 1;
	operation = *operatorPlusRight;
	*operatorPlusRight = '\0';	// Terminate left operand string

	return operation;
}
*/

//----------------------------------------------------------------------------
// Parse the given query string, handle grouping pairs () [],
// return the operation type and the 0, 1 or 2 operand strings.
// If operator not found, returns default op type SINGLE_QUERY_OPERATOR == 0
//----------------------------------------------------------------------------
int XM::FindLeftmostHighestOperator( char *aQuery, 
								char *&leftOperandString, char *&rightOperandString,
								int binaryOperators, int unaryOperators )
{
	char *operatorPlusRight;	// points to the operator followed by rightOperand
	int iOp;					// result
	
	// Assumptions
	assert(aQuery && *aQuery);
	if (!aQuery || !*aQuery)
		return 0;


	// Start at the beginning
	operatorPlusRight = SkipOverPair( aQuery );

	// Test for grouping around entire string
	if ( operatorPlusRight == &aQuery[strlen(aQuery)] )
	{
		// Strip and recurse
		aQuery[strlen(aQuery) - 1] = '\0';
		return FindLeftmostHighestOperator( aQuery + 1, leftOperandString, rightOperandString,
											 binaryOperators, unaryOperators );
	}
	assert(aQuery);	// SkipOverPair should return non-null
	

	// Test for leading unary operator
	for (iOp=0; iOp < 32; iOp++)
	{
		if ((unaryOperators & 1 << iOp) &&
			!strncmp(DOMQuery::Operators[iOp], operatorPlusRight, strlen(DOMQuery::Operators[iOp])) )
			break;
	}

	// Leading unary operator ?
	if (iOp < 32)
	{
		rightOperandString = aQuery + strlen(DOMQuery::Operators[iOp]);
		leftOperandString = 0;
	}
	else
	{
		// Scan through query to find leftmost operator
		while( *operatorPlusRight )
		{
			for (iOp=0; iOp < 32; iOp++)
			{
				if ((binaryOperators & 1 << iOp) &&
					!strncmp(DOMQuery::Operators[iOp], operatorPlusRight, strlen(DOMQuery::Operators[iOp])) )
					break;
			}
			if (iOp < 32)
				break;
			else
				operatorPlusRight = SkipOverPair( operatorPlusRight + 1 );
		}

		// Test if found binary operator, separate the operator from the right operand string
		if ( iOp < 32 )
		{
			leftOperandString = aQuery;
			*operatorPlusRight = '\0';	// Terminate left operand string
			rightOperandString = operatorPlusRight + strlen(DOMQuery::Operators[iOp]);
		}

		// No operators, this whole query must be a single query
		else
		{
			iOp = DOMQuery::SINGLE_QUERY_OPERATOR;
			rightOperandString = 0;
			leftOperandString = aQuery;
		}
	}

	assert(iOp < 32 && iOp >= 0);
	return iOp;
}

//----------------------------------------------------------------------------
// Reverse parse the given query string, handle grouping pairs () [],
// return the operation type and the 0, 1 or 2 operand strings.
// If operator not found, returns default op type SINGLE_QUERY_OPERATOR == 0
//----------------------------------------------------------------------------
int XM::FindRightmostHighestOperator( char *aQuery, 
								char *&leftOperandString, char *&rightOperandString,
								int binaryOperators, int unaryOperators )
{
	char *operatorPlusRight;	// points to the operator followed by rightOperand
	int iOp;					// result
	
	// Assumptions
	assert(aQuery && *aQuery);
	if (!aQuery || !*aQuery)
		return 0;


	// Start at the end
	operatorPlusRight = RevSkipOverPair( aQuery, aQuery + strlen(aQuery) - 1 );

	// Test for grouping around entire string
	if ( operatorPlusRight == (aQuery - 1) )
	{
		// Strip and recurse
		aQuery[strlen(aQuery) - 1] = '\0';
		return FindRightmostHighestOperator( aQuery + 1, leftOperandString, rightOperandString,
											 binaryOperators, unaryOperators );
	}
	assert(aQuery);	// RevSkipOverPair should return non-null
	
	// Scan (reverse) through query to find rightmost operator
	while( operatorPlusRight > aQuery )
	{
		for (iOp=0; iOp < 32; iOp++)
		{
			if ((binaryOperators & 1 << iOp) &&
				!strncmp(DOMQuery::Operators[iOp], operatorPlusRight, strlen(DOMQuery::Operators[iOp])) )
				break;
		}
		if (iOp < 32)
			break;
		else
			operatorPlusRight = RevSkipOverPair( aQuery, operatorPlusRight - 1 );
	}

	// Test if found binary operator, separate the operator from the right operand string
	if ( iOp < 32 )
	{
		assert(operatorPlusRight > aQuery);	// binary should have left operand also
		leftOperandString = aQuery;
		*operatorPlusRight = '\0';	// Terminate left operand string
		rightOperandString = operatorPlusRight + strlen(DOMQuery::Operators[iOp]);
	}

	// No binary operators, test for leading unary operator
	else //if (strchr(unaryOperators, *aQuery))
	{
		for (iOp=0; iOp < 32; iOp++)
		{
			if ((unaryOperators & 1 << iOp) &&
				!strncmp(DOMQuery::Operators[iOp], operatorPlusRight, strlen(DOMQuery::Operators[iOp])) )
				break;
		}

		// Leading unary operator
		if (iOp < 32)
		{
			rightOperandString = aQuery + strlen(DOMQuery::Operators[iOp]);
			leftOperandString = 0;
		}

		// No operators, this whole query must be a single query
		else
		{
			iOp = DOMQuery::SINGLE_QUERY_OPERATOR;
			rightOperandString = 0;
			leftOperandString = aQuery;
		}
	}

	assert(iOp < 32 && iOp >= 0);
	return iOp;
}

//----------------------------------------------------------------------------
// Detect the opening delimiter, seek to the matching closing delimiter, and advance past.
// Return pointer to just past the closing delim, or the same input if it is not opening delim.
//----------------------------------------------------------------------------
//char *DOMQuery::SkipOverPair( char *string )
char *XM::SkipOverPair( char *string )
{
	char *openDelim, closeDelim;
	char *pClose = string + 1;

	assert( string );


	// Test if this begins with an opening delimiter
	if ( !*string || !(openDelim = (char*)strchr(OPEN_DELIM,*string)) )
		return string;

	// Find close delim
	closeDelim = CLOSE_DELIM[openDelim - OPEN_DELIM];

	// Scan through string testing for close
	while( *pClose && *pClose != closeDelim )
	{
		// Recurse if find another opening delim
		if ( !strchr(OPEN_DELIM,*pClose) ||	// Test opening delim
			 (pClose[-1] == '\\') ||		// Test escaped delim
			 strchr(QUOTE_DELIM,*pClose) )	// Test quoted delim	(ignore quotes?)
			pClose ++;
		else
		{
			char *pTmp = SkipOverPair( pClose );
			// Test for successful recursion
			if (pTmp == pClose)
				return string;	// failed recursion
			else
				pClose = pTmp;
		}
	}

	if ( !*pClose )
	{
		eout << "string \"" << string << "\" is missing closing delimiter: \'" << closeDelim << "\'" << endl;
//		exit(-1);
		pClose = string;
	}
	else
		// Found the closing delim, advance past
		pClose ++;
	

	return pClose;
}

//----------------------------------------------------------------------------
// Starting at the string end (or provided char*)
// Detect the closing delimiter, reverse seek to the matching opening delimiter, and decrement before.
// Return pointer to just before the opening delim, or the same input if it is not closing delim.
// Note: SkipOverPair returns valid char*, possibly terminating null, 
// but RevSkipOverPair returns char* that may point to illegal location before string
//----------------------------------------------------------------------------
char *XM::RevSkipOverPair( char *string, char *pClose )
{
	char openDelim, *closeDelim;
	char *pOpen;

	assert( string );
	assert( !pClose || (pClose >= string && pClose < (string + strlen(string))));

	// Set pClose
	if (!pClose)
		pClose = string + strlen(string) - 1;
	if (pClose < string)
		pClose = string;

	// Test if this ends with closing delimiter
	if (!*pClose || !(closeDelim = (char*)strchr(CLOSE_DELIM,*pClose)) )
		return pClose;

	// Find open delim
	openDelim = OPEN_DELIM[closeDelim - CLOSE_DELIM];

	// Scan through string testing for open
	pOpen = pClose - 1;
	while( pOpen >= string && *pOpen != openDelim )
	{
		// Test for another closing delim
		if ( !strchr(CLOSE_DELIM,*pOpen) ||				// Test closing delim
			 (pOpen > string && pOpen[-1] == '\\') ||	// Test escaped delim
			 strchr(QUOTE_DELIM,*pOpen) )				// Test quoted delim	(ignore quotes?)
			pOpen --;
		else
			// Recurse with this closing delim
			pOpen = RevSkipOverPair( string, pOpen );
	}

#ifdef _DEBUG
	if ( pOpen < string )
	{
		eout << "string \"" << string << "\" is missing opening delimiter: \'" << openDelim << "\'" << endl;
		exit(-1);
	}
#endif

	// Found the closing delim, decrement before
	pOpen --;
	

	return pOpen;
}

//----------------------------------------------------------------------------
// Modifies the input string, removing redundant grouping pairs, and shifts left
//----------------------------------------------------------------------------
string XM::RemoveRedundantGroupings( const char *s )
{
	char *pClose, *pOpen;
	char *pShift;

	// Helper functions based on char*
	char *tmp = new char[strlen(s) + 1];
	strcpy(tmp,s);

	// Loop through input
	pOpen = tmp;
	while(*pOpen)
	{
		// Test if these two chars have adjacent closing delimiters
		if (pOpen[0] == pOpen[1] &&
			strchr("([",*pOpen) &&		// TODO
			((pClose = SkipOverPair(pOpen)) > pOpen + 1) &&
			(SkipOverPair(pOpen+1) == pClose - 1))
		{
			// Point pClose to the char before closing delim
			pClose -= 2; assert(pClose >= pOpen);

			// Shift input between grouping down one
			for(pShift = pOpen; pShift < pClose; pShift++)
				*pShift = pShift[1];

			// Shift remaining input down two
			while(pShift[1])
				*pShift++ = pShift[2];
		}
		else
			// Advance to next char
			pOpen++;
	}
	return string(tmp);
}


//=============================================================================
DOMQueryOperation::DOMQueryOperation() :
m_NumOperands(0),
m_NormPreferenceValue(1.0)
{
}

//----------------------------------------------------------------------------
DOMQueryOperation::~DOMQueryOperation()
{
	for ( int i = 0; i < m_NumOperands; i++ )
		delete m_Operands[i];
}

//----------------------------------------------------------------------------
void DOMQueryOperation::AddOperand( DOMQueryOperation *operand )
{
	assert(m_NumOperands < DOMQueryOperation_MAX_OPERANDS);
	m_Operands[m_NumOperands++] = operand;
}

//=============================================================================
// Evaluate the left and right sub-operations, AND the result
// Filter first, then score (MINIMUM)
//----------------------------------------------------------------------------
double HardAndOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	double leftEval, rightEval;
	NodeList tempList;
	assert( m_NumOperands == 2 );

	// Don't bother savinging nodes if the parent list doesn't want them
	tempList.Active( resultList.Active() );

	// Evaluate left operand first
	leftEval = m_Operands[0]->Evaluate( evalList, tempList );
	
	// If left operand has already failed, resultList will not be updated 
	// and MIN function will filter.
	// But right operand may bring score even lower.

	// Evaluate right
	rightEval = m_Operands[1]->Evaluate( evalList, tempList );

	// AND the node results
	if ( resultList.Active() &&  
		leftEval > 0.0 &&
		rightEval > 0.0 )
		resultList.Append( tempList );

#ifdef VERY_VERBOSE
	vout << "HAND:" << (leftEval < rightEval ? leftEval : rightEval) * m_NormPreferenceValue << endl;
#endif

	// AND the similarity results
	return (leftEval < rightEval ? leftEval : rightEval) * m_NormPreferenceValue;
}

//=============================================================================
// Evaluate the left and right sub-operations, AND the result
// Score first (AVERAGE), then filter
//----------------------------------------------------------------------------
double SoftAndOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	double leftEval, rightEval, result;
	NodeList tempList;
	assert( m_NumOperands == 2 );

	// Don't bother savinging nodes if the parent list doesn't want them
	tempList.Active( resultList.Active() );

	// Evaluate left operand first
	leftEval = m_Operands[0]->Evaluate( evalList, tempList );

	// Evaluate right
	rightEval = m_Operands[1]->Evaluate( evalList, tempList );

	// AND the similarity results
	// Negative preferences/scores can only detract from the combined score in an AND
	result = (leftEval + rightEval) / 2;

	// AND the node results
	if ( resultList.Active() &&  
		result > 0.0 )
		resultList.Append( tempList );

#ifdef VERY_VERBOSE
	vout << "SAND:" << result * m_NormPreferenceValue << endl;
#endif
	return result * m_NormPreferenceValue;
}

//=============================================================================
// Evaluate the left and right sub-operations, OR the result
// Score (MAXIMUM) with translated negative scores
//----------------------------------------------------------------------------
double HardOrOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	double leftEval, rightEval;
	assert( m_NumOperands == 2 );

	// Evaluate left operand first
	leftEval = m_Operands[0]->Evaluate( evalList, resultList );
	// Negative preferences/scores can only improve the combined score in an OR
	if (m_Operands[0]->m_NormPreferenceValue < 0)
		leftEval -= m_Operands[0]->m_NormPreferenceValue;

	// Don't Bypass right operand - allows to raise the similarity above 1.0

	// Evaluate right
	rightEval = m_Operands[1]->Evaluate( evalList, resultList );
	if (m_Operands[1]->m_NormPreferenceValue < 0)
		rightEval -= m_Operands[1]->m_NormPreferenceValue;

#ifdef VERY_VERBOSE
	vout << "HOR:" << (leftEval > rightEval ? leftEval : rightEval) * m_NormPreferenceValue << endl;
#endif
	// OR the similarity results
	return (leftEval > rightEval ? leftEval : rightEval) * m_NormPreferenceValue;
}

//=============================================================================
// Evaluate the left and right sub-operations, OR the result
// Score (SUM) with translated negative scores
//----------------------------------------------------------------------------
double SoftOrOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	double leftEval, rightEval;
	SingleQueryOperation *pSingleQueryOp;
	assert( m_NumOperands == 2 );

	// Evaluate left operand first
	leftEval = m_Operands[0]->Evaluate( evalList, resultList );
	// Negative preferences/scores can only improve the combined score in an OR
	// Determine (dynamically) if this operand is a single query or an operation
	// TODO - there's got to be a better way
	// TODO - repeat for other operations
	// TODO create family of translation and combining functions for reuse, and for SingleQueryOperation::Evaluate() to use the overriden method
//	if (pSingleQueryOp = dynamic_cast <SingleQueryOperation*> (m_Operands[0]))
	if (m_Operands[0]->m_NumOperands == 0)
	{
		pSingleQueryOp = static_cast <SingleQueryOperation*> (m_Operands[0]);
		if (pSingleQueryOp->m_NodeTest->m_NormPreferenceValue < 0)
			leftEval -= pSingleQueryOp->m_NodeTest->m_NormPreferenceValue;
	}
	else
	{
		if (m_Operands[0]->m_NormPreferenceValue < 0)
			leftEval -= m_Operands[0]->m_NormPreferenceValue;
	}

	// Don't Bypass right operand - allows to raise the similarity above 1.0

	// Evaluate right
	rightEval = m_Operands[1]->Evaluate( evalList, resultList );
//	if (pSingleQueryOp = dynamic_cast <SingleQueryOperation*> (m_Operands[1]))
	if (m_Operands[1]->m_NumOperands == 0)
	{
		pSingleQueryOp = static_cast <SingleQueryOperation*> (m_Operands[1]);
		if (pSingleQueryOp->m_NodeTest->m_NormPreferenceValue < 0)
			rightEval -= pSingleQueryOp->m_NodeTest->m_NormPreferenceValue;
	}
	else
	{
		if (m_Operands[1]->m_NormPreferenceValue < 0)
			rightEval -= m_Operands[1]->m_NormPreferenceValue;
	}

#ifdef VERY_VERBOSE
	vout << "SOR:" << (leftEval + rightEval) * m_NormPreferenceValue << endl;
#endif
	// OR the similarity results
	return (leftEval + rightEval) * m_NormPreferenceValue;
}

//=============================================================================
// Evaluate the sub-operation, Negate the result
//----------------------------------------------------------------------------
double NotOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	double eval;
	assert( m_NumOperands == 1 );

#ifdef _DEBUG
	// Saving evaluated nodes in a NOT operation doesn't make sense.
	if ( resultList.Active() )
	{
		eout << "\nSaving nodes in a NOT operation doesn't make sense!\n" << endl;
	}
#endif
	// Evaluate operand
	eval = m_Operands[0]->Evaluate( evalList, resultList );
	
	// Negate the similarity results
	if (m_NormPreferenceValue > 0)
		return m_NormPreferenceValue - eval;
	else
		// Double negative, just apply weighting
		return m_NormPreferenceValue * eval;
}

//=============================================================================
SingleQueryOperation::SingleQueryOperation() :
DOMQueryOperation(),
m_LocationSpecifier(0),
m_NodeTest(0),
m_saveNode(0)
{
}

//----------------------------------------------------------------------------
SingleQueryOperation::~SingleQueryOperation()
{
	delete m_LocationSpecifier;
	delete m_NodeTest;
}

//----------------------------------------------------------------------------
// Parse the single query string into location specifier, query text and options.
// Initilize the location specifier, and build the SingleStringMatch object.
// Example of singleQuery:  /Program/Creation/Creator%Role:"Producer:Assistant":ce
//----------------------------------------------------------------------------
void SingleQueryOperation::SetSingleQuery(char *singleQuery, double &maxScore)
{
	char *queryString;
	char *quoteEnd, *optionString, optionBuf[33] = {'\0'};
	char *pTmp, *pTmp2;
	bool caseSensitive = false;
	bool exactMatch = false;
	char *locationBuf;
	DOMNavigator DOMNav;
	int  iMatchOp = 0;	// Single String Match

	// Assumptions
	assert( singleQuery );

	// Release existing matcher
	delete m_NodeTest;
	m_NodeTest = 0;

	// Copy location (plus query string and options)
	locationBuf = new char[strlen(singleQuery) + 1];
	assert( locationBuf );
	strcpy(locationBuf,singleQuery);

	// Extract query string, skip over quoted/grouped location
	queryString = locationBuf;
	while( (pTmp = SkipOverPair(queryString)) != queryString )
		queryString = pTmp;
	//assert(queryString[0] != '(');
	//queryString = SkipOverPair(queryString);
	queryString = strchr(queryString, ':');
	if ( !queryString )
	{
		eout << "Invalid query string presented to SingleQueryOperation: " << locationBuf << endl;
		exit(-1);
	}
	*queryString++ = '\0';

	// Find (optional) close of match ':' within first few characters
	if ( (pTmp = strchr(queryString, ':')) && 
		 pTmp <= queryString + MAX_MATCH_OPERATION_WIDTH + 1 )
	{
		// Found a possible match operation (e.g. :<:), determine which MatchOperation
		int matchLength = pTmp - queryString;

		// Determine MatchOperation
		for( iMatchOp = 0; iMatchOp < sizeof(MatchOperations)/sizeof(char*); iMatchOp ++ )
		{
			if ( ! strncmp( queryString, MatchOperations[iMatchOp], matchLength ) &&
				 matchLength == strlen(MatchOperations[iMatchOp]) )
				break;
		}
		if ( iMatchOp < sizeof(MatchOperations)/sizeof(char*) )
		{
			// This is a valid match operation, update the queryString pointer
			//pTmp2 = queryString;
			queryString = pTmp;
			*queryString++ = '\0';
		}
		else
		{
			// eout << "Possible mis-interpretation of (first " << matchLength << " chars) Match Operation \"" << queryString << endl;
			iMatchOp = 0;	// Default Single String Match
		}
	}

	// Find any ending quote - which must precede the option string
	if ( ! (quoteEnd = strrchr( queryString, '"' )) )
		quoteEnd = queryString;

	// Extract options
	if ( optionString = strrchr( quoteEnd, ':' ) )
	{
		*optionString++ = '\0';
	}

	// Remove quotes around query string
//	if ( *queryString == '"' )
//	{
//		assert(queryString[strlen(queryString)-1] == '"' );
//		queryString[strlen(queryString)-1] = '\0';
//		queryString ++;
//	}
	// Strip enclosing quotes (and parens?) from query string
	while( *queryString && (*(pTmp = SkipOverPair(queryString)) == '\0') )
	{
		pTmp[-1] = '\0';
		queryString++;
	}

	// Strip enclosing quotes (and parens?) from location
	pTmp2 = locationBuf;
	while( *pTmp2 && (*(pTmp = SkipOverPair(pTmp2)) == '\0') )
	{
		pTmp[-1] = '\0';
		pTmp2++;
	}
	// Initialize the location specifier
	m_LocationSpecifier = DOMNav.InitLocation(pTmp2);


	// Construct the actual matcher
	switch (iMatchOp)
	{
	case MATCH_DEFAULT:
	case MATCH_STR_ISUB:	// Single String Match, pseudo-stristr
		m_NodeTest = new SingleStringMatch( queryString, optionString );
		maxScore *= fabs( ((SingleStringMatch*)m_NodeTest)->m_MaxScore );
		break;
	case MATCH_STR_SUB: // Single Custom Match, strstr
		strcpy(optionBuf,"c");
		if (optionString)
			strcat(optionBuf,optionString);
		m_NodeTest = new SingleStringMatch( queryString, optionBuf );
		maxScore *= fabs( ((SingleStringMatch*)m_NodeTest)->m_MaxScore );
		break;
	case MATCH_STR_IEQ: // Single Custom Match, stricmp
		strcpy(optionBuf,"e");
		if (optionString)
			strcat(optionBuf,optionString);
		m_NodeTest = new SingleStringMatch( queryString, optionBuf );
		maxScore *= fabs( ((SingleStringMatch*)m_NodeTest)->m_MaxScore );
		break;
	case MATCH_STR_EQ: // Single Custom Match, strcmp
		strcpy(optionBuf,"ce");
		if (optionString)
			strcat(optionBuf,optionString);
		m_NodeTest = new SingleStringMatch( queryString, optionBuf );
		maxScore *= fabs( ((SingleStringMatch*)m_NodeTest)->m_MaxScore );
		break;
	case MATCH_VAL_EQ: // Single Custom Match, Equal
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_Equal );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_LT: // Single Custom Match, Less Than
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_LessThan );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_GT: // Single Custom Match, Greater Than
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_GreaterThan );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_GT_LT: // Single Custom Match, Greater Than Less Than
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_GreaterThan_LessThan );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_LTE: // Single Custom Match, Less Than/Equal
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_LessThanEqual );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_GTE: // Single Custom Match, Greater Than/Equal
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_GreaterThanEqual );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_GTE_LTE: // Single Custom Match, Greater Than/Equal Less Than/Equal
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_GreaterThanEqual_LessThanEqual );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_VAL_GTE_LT: // Single Custom Match, Greater Than/Equal Less Than
		{
			SingleCustomMatch *CustomMatch = new SingleCustomMatch( queryString, optionString );
			if (atof(queryString) == 0.0 && queryString[0] != '0')
				eout << "Numerical value comparison using query \"" << queryString << "\" == 0.0" << endl;
			CustomMatch->SetMatchFunction( CustomMatchFunc_GreaterThanEqual_LessThan );
			m_NodeTest = CustomMatch;
			break;
		}
	case MATCH_CNT_EQ:	// Node Count Comparison - Specific Compare operator determined in constructor
		m_NodeTest = new NodeCountCompare( queryString, optionString, NodeCountCompare::EQUAL );
		break;
	case MATCH_CNT_LT:
		m_NodeTest = new NodeCountCompare( queryString, optionString, NodeCountCompare::LESS_THAN );
		break;
	case MATCH_CNT_GT:
		m_NodeTest = new NodeCountCompare( queryString, optionString, NodeCountCompare::GREATER_THAN );
		break;
	case MATCH_CNT_GT_LT:
		m_NodeTest = new NodeCountCompare( queryString, optionString, NodeCountCompare::LESS_THAN_GREATER_THAN );
		break;

	default:
		eout << "Match Operation number " << iMatchOp << " is not supported yet!" << endl;
		exit(-1);
	}

	assert( m_NodeTest );

	// Scale the maxScore
	maxScore *= fabs( m_NodeTest->m_NormPreferenceValue );


#ifdef VERBOSE
	vout << "SetSingleQuery on Location:\"" << m_LocationSpecifier << "\", MatchOp:\"" << MatchOperations[iMatchOp] << "\" Query:\"" << queryString << "\"" << endl;
#endif

	delete locationBuf;
}

//----------------------------------------------------------------------------
char *SingleQueryOperation::GetLocation()
{
	return m_LocationSpecifier;
}

//----------------------------------------------------------------------------
// Resolve the location specifier into 0,1 or multiple reference text strings, 
// and evaluate the single string search for each.  Return 0.0 if location is not present
// OR all locations at all given nodes
//----------------------------------------------------------------------------
double SingleQueryOperation::Evaluate(NodeList &evalList, NodeList &resultList)
{
	DOMNavigator DOMNav;
	double newSimilarity, similarity = 0.0;
	bool bLocationNotFound = true;
	char *location;	// expanded copy of location specifier, gets modified
	GenericDS locationNode;
	int iEval;
	string attrName, newAttrName;
	int ancestorLevel = m_saveNode;
	int newLevel;
	NodeList aggregateList;

	// Assumptions
	assert(m_NodeTest);

	// Loop through all the nodes in the eval list
	for ( iEval = 0; iEval < evalList.GetLength(); iEval ++ )
	{
		// Copy the location for modification
		location = new char[strlen(GetLocation())+1];
		assert(location);
		strcpy(location,GetLocation());

		// The location specifier may resolve to multiple actual locations
		// Iterate through all the possible locations, ORing the results
		// Retrieve the node 
		locationNode = DOMNav.GetNode( evalList.Item(iEval), location , attrName);
		while ( ! locationNode.isNull() )
		{
			// Check for single node testing
			if ( !m_NodeTest->Aggregate() )
			{
				// Perform the Match on a single node
				NodeList matchNodeList(locationNode, attrName);
				newSimilarity = m_NodeTest->Match( matchNodeList );

				// OR the similarity results
				if ( bLocationNotFound ||
					 newSimilarity > similarity )
				{
					bLocationNotFound = false;	// TODO Handle other types of ORing (e.g.Soft) when list length > 1
					similarity = newSimilarity;
				}

				// Saving any nodes?
				if ( resultList.Active() && ancestorLevel )
				{
					// Determine how far to back up in ancestry
					newLevel = ancestorLevel - 1;
					newAttrName.erase();
					if ( !attrName.empty() )	
					{
						newLevel --;		// Attr and Element occupy same GenericDS
						if ( newLevel == 0 )	// Only this leaf node can be an attribute
							newAttrName = attrName;
					}
					assert( newLevel >= 0 );
					// Append the correct "ancestor" node
					if ( newSimilarity > 0.0 )
						resultList.Append( DOMNav.GetAncestorNode(locationNode, newLevel), newAttrName );
				}
				// Don't Bypass remaining nodes - allows to raise the similarity above 1.0

				// A location that is just an attribute %Attr should only retrieve once
				// TODO put this into GetNode (how to persist?)
				if ( *location == '%' )
					break;
			}
			else
			{
				// Append nodes to evaluate in aggregation
				aggregateList.Append( locationNode, attrName );
			}

			locationNode = DOMNav.GetNode( evalList.Item(iEval), location , attrName);
		}	// while GetNode

		// Evaluate aggregate testing
		if ( m_NodeTest->Aggregate() )
		{
			// Evaluate, then clear for next eval node
			newSimilarity = m_NodeTest->Match( aggregateList );
			aggregateList.Clear();
		

			// OR the similarity results
			if ( bLocationNotFound ||
				 newSimilarity > similarity )
			{
				bLocationNotFound = false;	// TODO Handle other types of ORing (e.g.Soft) when list length > 1
				similarity = newSimilarity;
			}

			// Don't Bypass remaining nodes - allows to raise the similarity above 1.0
		}

		delete location;
	}	// for iEval

#ifdef VERY_VERBOSE
	vout << "SingleQueryOperation:" << similarity * m_NormPreferenceValue << endl;
#endif
	return similarity * m_NormPreferenceValue;
}

//=============================================================================
NodeTest::NodeTest(char *queryString, const char *optionString, int type)
{
	// Assumptions
	assert( queryString );

	// Copy the query string
	m_QueryString = new char[strlen(queryString) + 1];
	assert( m_QueryString );
	strcpy(m_QueryString,queryString);

	// Default normalized pref value
	m_NormPreferenceValue = 1.0;

	// Grab the preference value from the optionString, delimited by "pv"
	if (optionString)
	{
		const char *pTmp;
		if (pTmp = strstr(optionString,"pv"))
		{
			pTmp += 2;
			m_NormPreferenceValue = atof(pTmp) / DOMQueryOperation::NOMINAL_PREFERENCE_VALUE;
			//TODO this atof() function will interpret a trailing 'e' as exponent.
		}

//		*pTmp = '\0';
	}

}

//----------------------------------------------------------------------------
NodeTest::~NodeTest()
{
	delete m_QueryString;
}

//=============================================================================
SingleStringMatch::SingleStringMatch(char *queryString, const char *optionString, int type) :
	// Base constructor	// TODO
	NodeTest(queryString, optionString)
{
	char *pTmp;

	// Default max count
	m_TestCnt = 1;
	m_MaxScore = 1.0;
	m_MinScore = 1.0;

	// Set the options
	m_CaseSensitive = false;
	m_ExactMatch = false;
	while ( optionString && *optionString )
	{
		switch( *optionString )
		{
		case 'c':
			m_CaseSensitive = true;
			break;
		case 'e':
			m_ExactMatch = true;
			break;
		case 'm':
			if (!strncmp(optionString,"max",3))
			{
				optionString += 3;
				m_MaxScore = atof(optionString);
				while (*optionString && strchr(DOMQueryOperation::DECIMAL_CHARS, *optionString))
					optionString ++;
				optionString --;	// point to last decimal char
			}
			else if (!strncmp(optionString,"min",3))
			{
				optionString += 3;
				m_MinScore = atof(optionString);
				while (*optionString && strchr(DOMQueryOperation::DECIMAL_CHARS, *optionString))
					optionString ++;
				optionString --;	// point to last decimal char
			}
			break;
		case 't':
			if (!strncmp(optionString,"test",4))
			{
				optionString += 4;
				m_TestCnt = atoi(optionString);
				while (*optionString && strchr(DOMQueryOperation::DECIMAL_CHARS, *optionString))
					optionString ++;
				optionString --;	// point to last decimal char
			}
			break;
		}
		optionString++;
	}


	// Convert to lower case for case insensitive
	if ( ! m_CaseSensitive )
		for( int i=0; i < strlen(m_QueryString); i++)
			m_QueryString[i] = tolower(m_QueryString[i]);
}

//----------------------------------------------------------------------------
// Single String Match engine == strcmp, stricmp, or strstr.
// Return 0.0 or 1.0.  Empty reference string, return 0.0.
//----------------------------------------------------------------------------
double SingleStringMatch::Match(NodeList &testList)
{
	double similarity = 0.0;
	const char *pTmp;

	assert( m_QueryString );
	assert( testList.GetLength() == 1);

	// Get the text from this single node
	DOMNavigator DOMNav;
	string refString;
	string attrName = testList.ItemAttrName(0);
	GenericDS testNode = testList.Item(0);
	if ( testNode.isNull() )
		return 0.0;
	refString = DOMNav.GetText(testNode,true,attrName);


	if ( refString.empty() )
	{
		if ( m_QueryString[0] )
			similarity = 0.0;	// empty ref can't match non-empty query
		else
			similarity = 1.0;	// empty ref matches empty query
	}

	else if ( m_ExactMatch )
	{
		if ( !m_CaseSensitive )
			for( int i=0; i < refString.size(); i++)
				refString.at(i) = tolower(refString.at(i));	// TODO
		similarity = refString.compare( m_QueryString ) ? 0.0 : 1.0;
	}

	else
	{
		if ( !m_CaseSensitive )
			for( int i=0; i < refString.size(); i++)
				refString.at(i) = tolower(refString.at(i));	// TODO
		//similarity = strstr( refString.c_str(), m_QueryString ) ? 1.0 : 0.0;	// TODO
		// Count up to m_TestCnt occurrences of this query string
		pTmp = refString.c_str();
		while (similarity < m_MaxScore && (pTmp = strstr(pTmp, m_QueryString)))
		{
			if (similarity == 0.0)
				similarity = m_MinScore;
			else
				similarity += (m_MaxScore - m_MinScore) / (m_TestCnt - 1);
			pTmp += strlen(m_QueryString);
		}

	}

#ifdef VERBOSE
	vout << "SingleStringMatch::Match " << similarity << 
		" (" << similarity * m_NormPreferenceValue << ") " <<
		"\t<= Query:\"" << m_QueryString << 
		"\" Reference:\"" << (refString.empty()?"":refString.c_str()) << "\"" << endl;
#endif

//	delete refString;
	return similarity * m_NormPreferenceValue;
}

//=============================================================================
SingleCustomMatch::SingleCustomMatch(char *queryString, const char *optionString, int type) :
	// Base constructor	// TODO
	NodeTest(queryString, optionString)
{
	// Set the options ?
}

//----------------------------------------------------------------------------
double SingleCustomMatch::Match(NodeList &testList)
{
	double similarity = 0.0;

	assert( m_QueryString );
	assert( testList.GetLength() == 1);

	// Get the text from this single node
	DOMNavigator DOMNav;
	string refString;
	string attrName = testList.ItemAttrName(0);
	refString = DOMNav.GetText(testList.Item(0),true,attrName);

	if ( refString.empty() )
		similarity = 0.0;

	else
		similarity = m_MatchFunc( refString.c_str(), m_QueryString, 0 );

#ifdef VERBOSE
	vout << "SingleCustomMatch::Match " << similarity << "\t<= Query:\"" << m_QueryString << 
		"\" Reference:\"" << (refString.empty()?"":refString.c_str()) << "\"" << endl;
#endif

//	delete refString;
	return similarity * m_NormPreferenceValue;
}

//=============================================================================
NodeCountCompare::NodeCountCompare(char *queryString, const char *optionString, NodeCountComparisonType type) :
	// Base constructor	// TODO
	NodeTest(queryString, optionString),
	m_Compare1(0),
	m_Compare2(0),
	m_CompareType(type)
{
	char *pTmp;

	// Set the comparison values
	m_Compare1 = atoi(queryString);
	if ( m_CompareType == LESS_THAN_GREATER_THAN )
	{
		if ( pTmp = strchr(queryString,',') )
			m_Compare2 = atoi(++pTmp);
		else
			eout << "NodeCountCompare #>< requires two compare values, not found in (" << queryString << ")" << "\nUsing zero for comparison!" << endl;
	}
}

//----------------------------------------------------------------------------
double NodeCountCompare::Match(NodeList &testList)
{
	double similarity = 0.0;
	int count = testList.GetLength();

	switch(m_CompareType)
	{
	case EQUAL:
		if ( count == m_Compare1 )
			similarity = 1.0;
		break;

	case LESS_THAN:
		if ( count < m_Compare1 )
			similarity = 1.0;
		break;

	case GREATER_THAN:
		if ( count > m_Compare1 )
			similarity = 1.0;
		break;

	case LESS_THAN_GREATER_THAN:
		if ( (count > m_Compare1) && (count < m_Compare2) )
			similarity = 1.0;
		break;
	}

#ifdef VERBOSE
	vout << "NodeCountCompare::Match " << similarity << "\t<= CompareType:\"" << MatchOperations[MATCH_CNT_EQ + m_CompareType - 1] << 
		"\" Reference: " << count << " Query: " << m_QueryString << endl;
#endif

	return similarity * m_NormPreferenceValue;
}

//=============================================================================
// Expand element number and wildcard designators (#,*) to have fixed-length leading-zero 
// format.  Insert at least one explicit number format in the specifier, after any wildcards.
// Return a new buffer that must be freed by the caller.
//----------------------------------------------------------------------------
char *DOMNavigator::InitLocation(char *string)
{
	char *pDest, *pSrc = string;
	char *location, *locationBuf;
	char *pEndMultiple;	// Points to element just after the lowest multiple element
	int i;
	bool bTag = false;

	// Allocate oversized buffer for location
	locationBuf = new char[MAX_LOCATION_SIZE];
	assert( locationBuf );
	memset( locationBuf, 0, MAX_LOCATION_SIZE );	// Null terminate locationBuf

	// Expand  Element Number special characters
	pDest = pEndMultiple = locationBuf;
	pSrc = string;
	while( *pSrc )
	{
		// Only expand tags (preceded by '/' or '%')
		if (!bTag)
		{
			bTag =  strchr("/%", *pSrc) != NULL;
			*pDest++ = *pSrc++;
		}
		else
		{
			bTag =  strchr("/%", *pSrc) != NULL;
			switch( *pSrc )
			{
			case ELEM_NUM_WILDCARD:
				strncat( pDest, "*000000000", ELEM_NUM_WIDTH+1 );
				pDest += ELEM_NUM_WIDTH+1;
				pEndMultiple = pDest;		// The last wildcare number must be followed by explicit number
				pSrc++;
				break;
			case ELEM_NUM:
				pSrc++;
				// Find the extent of this number, less than ELEM_NUM_WIDTH digits
				for( i=0; i < ELEM_NUM_WIDTH; i++ )
					if ( pSrc[i] < '0' || pSrc[i] > '9' )
						break;
				strncat( pDest, "#000000000", ELEM_NUM_WIDTH-i+1 );
				strncat( pDest, pSrc, i );
				pDest += ELEM_NUM_WIDTH+1;
				//TODO this used to work (e.g. test_11.par
//				pEndMultiple = 0;	// This takes the place of the end-multiple explicit number
				pEndMultiple = pDest;		// The last wildcard number must be followed by explicit number
				pSrc += i;
				break;
			default:	// regular character
				*pDest++ = *pSrc++;
			}
		}
	}

	// TODO Insert the #0000 at every point, instead of all this end-multiple mess
	// Insert the end-multiple delimiter
	if ( pEndMultiple )
	{
		if (!*pEndMultiple)
		{
			// Special case of empty string location
			strncpy(pEndMultiple, "#000000000", ELEM_NUM_WIDTH+1);
			assert(!*string);
		}
		else
		{
			// pEndMultiple is pointing at the element name of the last wildcard number, fast forward to next element
			while ( pEndMultiple = strchr(pEndMultiple,'/') )
			{
				// Don't expand a DIVISION_OPERATOR, known to precede '('
				if (pEndMultiple[1] == '(')
				{
					pEndMultiple++;
					continue;
				}

				// scan past '/'
				while( *pEndMultiple == '/' )
					pEndMultiple ++;
				// shift right ELEM_NUM_WIDTH + 1, start from ending termination
				// pDest should be pointing at the null termination
				pDest = pEndMultiple + strlen(pEndMultiple);
				while( pDest >= pEndMultiple )
				{
					*(pDest+ELEM_NUM_WIDTH+1) = *pDest;
					pDest--;
				}
				strncpy(pEndMultiple, "#000000000", ELEM_NUM_WIDTH+1);
	//break;	// TODO forces only the end one
			}
		}
	}

	// Create and return a new buffer
	location = new char[strlen(locationBuf)+1];
	assert( location );
	strcpy(location, locationBuf);

	delete locationBuf;

	return location;
}

//----------------------------------------------------------------------------
// If attrName is empty, retrieve the element content, else retrieve the attribute text value
// Return null if the node cannot be found, 
// return empty string if the text is missing, empty, or all whitespace.
//----------------------------------------------------------------------------
string DOMNavigator::GetText(GenericDS node, bool preserveWhiteSpace, const string &attrName)
{
	string text;

	// Test for good node
	if ( node.isNull() )
		return text;

	if (attrName.empty())
		node.GetTextValue( text );
	else
		node.GetTextAttribute(attrName, text);

	// Exclude whitespace only strings
	if ( !preserveWhiteSpace )
	{					
		while( text.size() && strchr( DOMNavigator::WHITESPACE, text.at(0)) )
			text.erase(0,1);
		while( text.size() && strchr( DOMNavigator::WHITESPACE, text.at(text.size()-1)) )
			text.erase(text.size()-1,1);
//		for( int i=0; i<text.size(); i++ )
//			if ( !strchr( DOMNavigator::WHITESPACE, text.at(i) ) )
//				break;
//		if ( i==text.size() )
//			text.erase();
	}

	return text;
}

//----------------------------------------------------------------------------
// Find the leading path of this location and recurse for trailing path locations.
// Allow trailing path recursions to increment their element numbers until exhausted.
// Then increment this leading element number (if present), or reset to zero if location 
// not found. Always return node & incremented location, or null node & reset location.
// Handles a parent that is DOM_Element or DOM_Document.
// Sets attrName if the location specifies an attribute
// Location may be /Elem..., /#xxxxElem..., /@xxxxElem..., /*xxxxElem..., or %Attr
//----------------------------------------------------------------------------
GenericDS DOMNavigator::GetNode(GenericDS parent, char *location, string &attrName, char *dbgPath)
{
	char locationName[MAX_LOCATION_SIZE];
	char *pTailLocation, *pLocation;
	GenericDS nextNode;
	string nextAttrName;
	string value;
	int elementNumber = 0;
	char *pElementNumberLocation = 0;

	// Check for valid node and location
	assert(location);
	assert(*location);
	if ( parent.isNull() )// || !*location )	// Empty location returns the same node
		return parent;

#ifdef _DEBUG
	string ancestorPath = GetAncestorPath(parent);
#endif

	// Derive node value if required
	if (*location == '=')
		return GetCompositeNode(parent, location+1, attrName, dbgPath);

	// Preserve debug path (recall that location is modified in place)
	if ( !dbgPath )
		dbgPath = location;

	// Parse the location, scoot past leading slashes
	while( *location == '/' )
		location++;


	// Check for element number. Advances the pLocation past number stuff
	pLocation = location;
	if ( (elementNumber = CheckElementNumber(pLocation)) < 0 )
		// Element number is out of range (number already reset), so return
		return GenericDS();	// Null node

	if ( !*pLocation )	// Empty location returns the same node
	{
		SetElementNumber(location,1);
		return parent;
	}


	// pLocation should now be pointing at Elem... or %Attr...

	// Determine if there are more than one path in this location
	pTailLocation = pLocation + 1;
	while( *pTailLocation && !strchr("/%=",*pTailLocation) )
		pTailLocation++;
	if (*pTailLocation)
	{
		// We are not at the end of the location
		strncpy(locationName, pLocation, pTailLocation - pLocation);
		locationName[pTailLocation - pLocation] = '\0';
	}
	else
	{
		pTailLocation = 0;
		strcpy(locationName, pLocation);
	}



	// Test if this location is an attribute
	if ( *locationName == '%' )
	{
		if ( !parent.GetTextAttribute( locationName+1, value ) )
		{
			attrName = locationName+1;

			// Derive attribute node value of if required
			if (pTailLocation && (*pTailLocation == '='))
				nextNode = GetCompositeNode(parent, pTailLocation+1, attrName, dbgPath);
			else
				nextNode = parent;
		}

#ifdef VERBOSE
		if ( !nextNode.isNull() )
			vout << "GetNode (OK)   location = " << dbgPath << endl;
		else
			vout << "GetNode (FAIL) location = " << dbgPath << endl;
#endif
		return nextNode;
	}


	// This must be an element, find it (either the parent, this node, or a child)
	if (!strcmp(locationName,".."))
		nextNode = parent.GetParent();
	else if (!strcmp(locationName,"."))
		nextNode = parent;
	else
		// find it by name and number
		nextNode = GetChildByTagName( parent, locationName, elementNumber );

	if ( nextNode.isNull() )
	{
		// Did not find this element name & number
		// reset element numbers and return
		// assume that lower element numbers have been previously reset
#ifdef VERBOSE
		vout << "GetNode (FAIL) location = " << dbgPath << endl;
		vout << "Parent = " << GetAncestorPath(parent, 2).c_str() << endl;
#endif
		SetElementNumber(location, 0);
		return nextNode;
	}


	// If this is the leaf location, increment for next pass and return
	if ( ! pTailLocation )
	{
#ifdef VERBOSE
		vout << "GetNode (OK)   location = " << dbgPath << endl;
#endif
		SetElementNumber(location, elementNumber + 1);

		attrName.erase();
		return nextNode;
	}

	else
	{
		// Recurse with tail nodes if there is a tail location
		nextNode = GetNode( nextNode, pTailLocation, nextAttrName, dbgPath );

		// Test if we exhausted all the tail node locations for this element number
		if ( nextNode.isNull() )
		{
/*			// Continue at this level, increment and reinvoke
			if ( SetElementNumber(location, elementNumber + 1) >= 0 )
				return GetNode(parent, location);
			else
				return nextNode;	// can't increment, just return the null node
TODO*/
			// Continue at this level, increment and reinvoke
			if ( SetElementNumber(location, elementNumber + 1) >= 0 )
				nextNode = GetNode(parent, location, nextAttrName, dbgPath);

			if ( nextNode.isNull() )
			{
				// Can't do any more at this level, reset and return the null node
				SetElementNumber(location, 0);
				return nextNode;	
			}
		}

		// Tail node returned was good, continue with tail nodes, unless it is an attribute or composite
		if ( *pTailLocation != '%' &&
			 *pTailLocation != '=' )
		{
			attrName = nextAttrName;
			return nextNode;
		}
	}

	// Tail node was an attribute, increment for next pass and return
	assert( *pTailLocation == '%' || *pTailLocation == '=' );	// check assumption
	assert( ! nextAttrName.empty() );		// check assumption
	SetElementNumber(location, elementNumber + 1);
	attrName = nextAttrName;
	return nextNode;
}

//----------------------------------------------------------------------------
// Retrieve the element number at the given string, and move past element number stuff
// Return 0,1,... for valid numbers, or -1 if element numbers have been exhausted.
// If numbers have been exhausted, go ahead and reset number for next pass
//----------------------------------------------------------------------------
int DOMNavigator::CheckElementNumber(char *&element)
{
	char numString[ELEM_NUM_WIDTH+1];
	int retval = 0;

	if ( *element == ELEM_NUM ||
		 *element == ELEM_NUM_WILDCARD )
	{
		// Retrieve the number
		strncpy( numString, element+1, ELEM_NUM_WIDTH );
		numString[ELEM_NUM_WIDTH] = '\0';
		retval = atoi(numString);

		// Move past element number
		element += ELEM_NUM_WIDTH+1;
	}
	else if ( *element == ELEM_NUM_DISABLED )
	{
		// We've been here before, reset and return
		*element = ELEM_NUM;
		retval = -1;
		element += ELEM_NUM_WIDTH+1;
	}

	return retval;
}

//----------------------------------------------------------------------------
// Set the element number (if present) to the given value
//----------------------------------------------------------------------------
int DOMNavigator::SetElementNumber(char *element, int num)
{
	char numString[ELEM_NUM_WIDTH+1];

	if ( *element == ELEM_NUM ||
		 *element == ELEM_NUM_DISABLED )
	{
		// Disregard the num, just toggle the SPCL_CHAR
//		*element = *element == ELEM_NUM ? ELEM_NUM_DISABLED : ELEM_NUM;
		// Regard the num!
		*element = num ? ELEM_NUM_DISABLED : ELEM_NUM;
		return -1;
	}

	else if ( *element == ELEM_NUM_WILDCARD )
	{
		assert( num < 10000 );
		sprintf(numString, "%0*d", ELEM_NUM_WIDTH, num);
		strncpy(element+1, numString, ELEM_NUM_WIDTH);
		return num;
	}

	else
	{
		// Can do nothing except signal an out of range
		return num ? -1 : 0;
	}
}

//----------------------------------------------------------------------------
GenericDS DOMNavigator::GetChildByTagName(GenericDS node, string name, unsigned int iInst) {

	GenericDS child = node.GetFirstChild();

 	while ( ! child.isNull() ) {
		string childName = child.GetDSName();
		if ( !childName.compare(name) ) {
			if ( !iInst-- ) {
				return child;
			}
		}
		child = child.GetNextSibling();
	}

	return child;
}

//----------------------------------------------------------------------------
// Build a location specifier for the given node, by walking back up the tree.
// Return a new buffer that must be freed by the caller, or 0 if the node is null.
//----------------------------------------------------------------------------
char *DOMNavigator::GetLocation(GenericDS node, string attrName)
{
	string location;

	if ( node.isNull() )
		return 0;

	// Attribute node (leaf only) 
	if ( ! attrName.empty() )
	{
		location = attrName;
		location.insert( 0, "%" );
	}


	// Build location from this point up
	while ( !node.isNull() && 
			node.GetDSName().compare("#document") )	// TODO GenericDS has a hidden #document node at top
	{
		location.insert( 0, node.GetDSName() );
		location.insert( 0, "/" );
		node = node.GetParent();
	}

	char *retval = new char[location.size()+1];	// TODO change everything to string
	assert(retval);
	if ( !location.empty() )
		strcpy(retval,location.c_str());

	return retval;
}
	
//----------------------------------------------------------------------------
GenericDS DOMNavigator::GetAncestorNode(GenericDS node, int ancestorLevel)
{
	// Retrieve from this point up
	while ( !node.isNull() && ancestorLevel-- )
	{
		node = node.GetParent();
	}

	return node;
}

//----------------------------------------------------------------------------
// Combine the relative nodes using arithmetic or string operations into a single node value.
// Insert this as an attribute into the parent, which will then be used as the parent's value
// For simplicity (for recognizing DIVISION_OPERATOR) requires every operand to be enclosed in parens.
// If the location does not exist (e.g. no more instances), returns null node.
// If the location exists, the attribute may be empty, a numbertext, or NaN.
//----------------------------------------------------------------------------
GenericDS DOMNavigator::GetCompositeNode(GenericDS parent, char *location, string &attrName, char *dbgPath)
{
	// e.g. "/Program/*CreationMetaInformation/*Classification/*MediaReview/Rating=((/RatingValue)-(/WorstRating)+(#1))/((/BestRating)-(/WorstRating)+(#1))

	char *locationBuf, *pGetLocation, *pDefault;
	char *leftOperand, *rightOperand;
	char numBuf[32];
	int  iOp;
	GenericDS childNode;
	string childAttrName;
	string refString, leftString, rightString;

	// Assumptions
	assert(!parent.isNull());
	assert(location);
	//assert(*location == '(');	// TODO

	// Copy location, we will insert updated location after each GetNode()
	locationBuf = new char[strlen(location)+1];
	assert(locationBuf);
	strcpy(locationBuf, location);
	// Perform all operations with pointer into copy buffer
	pGetLocation = locationBuf;

	// Strip off outer grouping pairs
	while( SkipOverPair(pGetLocation) == &pGetLocation[strlen(pGetLocation)])
	{
		pGetLocation++;
		pGetLocation[strlen(pGetLocation)-1] = '\0';
	}

	// If there isn't a grouping pair remaining, this is a single node or constant, so get it
	if (*pGetLocation != '(')
	{
		if (*pGetLocation == '#')
			refString = pGetLocation + 1;	// Constant
		else
		{
			// Separate the optional trailing default value "(node1,(node2,#0))"
			if (pDefault = strchr(pGetLocation, ','))
				*pDefault++ = '\0';

			// Attempt to get built-in composite value
			childAttrName = attrName;
			childNode = GetTimeNode(parent, pGetLocation, childAttrName, dbgPath);

			// If not a built-in, just get this node
			if (childNode.isNull())
				childNode = GetNode(parent, pGetLocation, childAttrName, dbgPath);	// TODO dbgPath?
			// Update location
			memcpy(&location[pGetLocation-locationBuf], pGetLocation, strlen(pGetLocation));

			// Test if we didn't find the node
			if (childNode.isNull())
			{
				childAttrName.erase();

				if (pDefault)
				{
					// Get the default node
					childNode = GetCompositeNode(parent, pDefault, childAttrName, dbgPath);

					attrName = childAttrName;	//TODO

					// Update location
					memcpy(&location[pDefault-locationBuf], pDefault, strlen(pDefault));
				}
				return childNode;
			}

			// Retrieve this node's text
			refString = GetText(childNode,true,childAttrName);
		}
		// Set the value into the CompositeNodeValue attribute
		parent.SetAttribute(COMPOSITE_NODE_ATTR_NAME,refString);	// TODO
		attrName = COMPOSITE_NODE_ATTR_NAME;
		return parent;
	}

	// Parse the location to find its operators
	iOp = FindRightmostHighestOperator(pGetLocation, leftOperand, rightOperand, 
											COMPOSITE_NODE_BINARY_OPERATORS, COMPOSITE_NODE_UNARY_OPERATORS);

	// Get right operand
	childAttrName.erase();
	childNode = GetCompositeNode(parent, rightOperand, childAttrName, dbgPath);
	// Update location
	memcpy(&location[rightOperand-locationBuf], rightOperand, strlen(rightOperand));
	if (childNode.isNull())
		return childNode;
	// Retrieve this node's composite value
	if (childNode.GetTextAttribute(COMPOSITE_NODE_ATTR_NAME, rightString) != 0)
		return GenericDS();  // Null node

	// Get left operand, for binary operations only
	if (COMPOSITE_NODE_BINARY_OPERATORS & 1<<iOp)
	{
		childAttrName.erase();
		childNode = GetCompositeNode(parent, leftOperand, childAttrName, dbgPath);
		// Update location
		memcpy(&location[leftOperand-locationBuf], leftOperand, strlen(leftOperand));
		if (childNode.isNull())
			return childNode;
		// Retrieve this node's composite value
		if (childNode.GetTextAttribute(COMPOSITE_NODE_ATTR_NAME,leftString) != 0)
			return GenericDS();  // Null node
	}


	switch( iOp )
	{
	case DOMQuery::ADDITION_OPERATOR:
		{
			if (!leftString.compare(NOT_A_NUMBER) ||
				!rightString.compare(NOT_A_NUMBER) )
				refString = NOT_A_NUMBER;
			else
			{
				sprintf(numBuf,"%f",atof(leftString.c_str()) + atof(rightString.c_str()));
				refString = numBuf;
			}
			break;
		}

	case DOMQuery::SUBTRACTION_OPERATOR:
		{
			if (!leftString.compare(NOT_A_NUMBER) ||
				!rightString.compare(NOT_A_NUMBER) )
				refString = NOT_A_NUMBER;
			else
			{
				sprintf(numBuf,"%f",atof(leftString.c_str()) - atof(rightString.c_str()));
				refString = numBuf;
			}
			break;
		}

	case DOMQuery::MULTIPLICATION_OPERATOR:
		{
			if (!leftString.compare(NOT_A_NUMBER) ||
				!rightString.compare(NOT_A_NUMBER) )
				refString = NOT_A_NUMBER;
			else
			{
				sprintf(numBuf,"%f",atof(leftString.c_str()) * atof(rightString.c_str()));
				refString = numBuf;
			}
			break;
		}

	case DOMQuery::DIVISION_OPERATOR:
		{
			if (!leftString.compare(NOT_A_NUMBER) ||
				!rightString.compare(NOT_A_NUMBER) ||
				atof(rightString.c_str()) == 0 )
				refString = NOT_A_NUMBER;
			else
			{
				sprintf(numBuf,"%f",atof(leftString.c_str()) / atof(rightString.c_str()));
				refString = numBuf;
			}
			break;
		}

	case DOMQuery::CONCATENATION_OPERATOR:
		{
			refString = leftString + rightString;
			break;
		}

	default:
		eout << "Undefined operator (" << iOp << ") found in composite node location: " << location << endl;
		exit(-1);
	}

	// Set the value into the CompositeNodeValue attribute
	parent.SetAttribute(COMPOSITE_NODE_ATTR_NAME,refString);
	attrName = COMPOSITE_NODE_ATTR_NAME;
	return parent;
}

//----------------------------------------------------------------------------
// Get predefined Time D's and DS's
//
// If the location is not one of the Time D's, returns null node.
// TODO This changed...If the node does not contain the required data for the given Time D, the composite attribute string is empty
//----------------------------------------------------------------------------
GenericDS DOMNavigator::GetTimeNode(GenericDS node, char *location, string &attrName, char *dbgPath)
{
	double timeUnit, timeBase;
	GenericDS tempNode, retNode = node;
	double timePointValue, timeDurationValue = 0.0;
	string timeText, uri, refString, unusedAttrName, tempAttrName = attrName;
	char valBuf[100];

#ifdef _DEBUG
	string ancestorPath = GetAncestorPath(node);
#endif

	// There are three levels for specifying location string

	// Level 1: Explicit D typecast (e.g. the node is a TimePointD element)
	if		(!strcmp(location, "TimePointD"))
	{
		timeText = GetText(node, false, tempAttrName);
		if (!timeText.empty())
			refString = ConvertFractionalTime( timeText , node );
		else
			retNode = GenericDS();
	}
	else if (!strcmp(location, "DurationD") ||
			 !strcmp(location, "MediaDurationD"))
	{
		timeText = GetText(node, false, tempAttrName);
		if (timeText.c_str() && *timeText.c_str() && (timeText.c_str()[0] == 'P' || timeText.c_str()[1] == 'P'))	// simple test for Duration
			refString = ConvertFractionalDuration( timeText, node );
		else
			retNode = GenericDS();
	}
	else if (!strcmp(location, "IncrDurationD"))
	{
		timeText = GetText(node, true, tempAttrName);
		timeUnit = GetTimeUnit(node);
		if (!timeText.empty() && (timeUnit != 0.0))
		{
			sprintf(valBuf, "%f",timeUnit * atof(timeText.c_str()));
			refString = valBuf;
		}
		else
			retNode = GenericDS();

	}
/*
	else if (!strcmp(location, "RelTimePointD"))
	{
		timeText = ConvertFractionalDuration( GetText(node, true, tempAttrName), GenericDS() );
		timeBase = GetTimeBase(node);
		if (!timeText.IsEmpty() && (timeBase != 0.0))
		{
			sprintf(valBuf, "%f",timeBase + atof(timeText.c_str()));
			refString = valBuf;
		}
		else
			retNode = GenericDS();
	}
	else if (!strcmp(location, "RelIncrTimePointD"))
	{
		timeText = GetText(node, true, tempAttrName);
		timeUnit = GetTimeUnit(node);
		timeBase = GetTimeBase(node);
		if (!timeText.IsEmpty() && (timeUnit != 0.0) && (timeBase != 0.0))
		{
			sprintf(valBuf, "%f",timeBase + (timeUnit * atof(timeText.c_str())));
			refString = valBuf;
		}
		else
			retNode = GenericDS();
	}
*/

	// Level 2: PointInTime or DurationOfTime typecast specification (e.g. the node is 1 of several types of elements)
	else if (!strcmp(location, "PointInTime"))
	{
/*
		tempNode = GetTimeNode(node,"RelIncrTimePointD",tempAttrName);
		if (!tempNode.isNull())
			tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
		else
		{
			tempNode = GetTimeNode(node,"RelTimePointD",tempAttrName);
			if (!tempNode.isNull())
				tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
			else
			{
*/
				tempNode = GetTimeNode(node,"TimePointD",tempAttrName);
				if (!tempNode.isNull())
					tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
/*
			}
		}
*/
	}
	else if (!strcmp(location, "DurationOfTime"))
	{
		tempNode = GetTimeNode(node,"IncrDurationD",tempAttrName);
		if (!tempNode.isNull())
			tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
		else
		{
			tempNode = GetTimeNode(node,"DurationD",tempAttrName);
			if (!tempNode.isNull())
				tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
		}
	}

	// Level 3: Variety of ways of composing TimeDS
	else if (!strcmp(location, "TimeDS_StartEnd"))
	{
		// the node is a container of a time point and a time duration

		tempNode = GetNode(node,"TimePoint=TimePointD",unusedAttrName);
/*
		if (tempNode.isNull())
		{
			unusedAttrName.erase();
			tempNode = GetNode(node,"RelTimePoint=RelTimePointD",unusedAttrName);
			if (tempNode.isNull())
			{
				unusedAttrName.erase();
				tempNode = GetNode(node,"RelIncrTimePoint=RelIncrTimePointD",unusedAttrName);
			}
		}			
*/
		if (tempNode.isNull())
			retNode = GenericDS();
		else
		{
			tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
			timePointValue = atof(refString.c_str());

			unusedAttrName.erase();
			tempNode = GetNode(node,"Duration=DurationD",unusedAttrName);
			if (tempNode.isNull())
			{
				unusedAttrName.erase();
				tempNode = GetNode(node,"IncrDuration=IncrDurationD",unusedAttrName);
			}
//			if (tempNode.isNull())
//				retNode = GenericDS();
//			else
			if (!tempNode.isNull())
			{
				tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
				timeDurationValue = atof(refString.c_str());
			}

			// Build start,end pair
			sprintf(valBuf,"%f,%f", timePointValue, timePointValue + timeDurationValue);
			refString = valBuf;
	
		}
	}
	else if (!strcmp(location, "TimeDS_Start"))
	{
		// the node is a container of a time point

		tempNode = GetNode(node,"TimePoint=TimePointD",unusedAttrName);
/*
		if (tempNode.isNull())
		{
			unusedAttrName.erase();
			tempNode = GetNode(node,"RelTimePoint=RelTimePointD",unusedAttrName);
			if (tempNode.isNull())
			{
				unusedAttrName.erase();
				tempNode = GetNode(node,"RelIncrTimePoint=RelIncrTimePointD",unusedAttrName);
			}
		}			
*/
		if (tempNode.isNull())
			retNode = GenericDS();
		else
		{
			tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
		}
	}
	else if (!strcmp(location, "TimeDS_End"))
	{
		// the node is a container of a time point and a time duration

		tempNode = GetNode(node,"TimePoint=TimePointD",unusedAttrName);
/*
		if (tempNode.isNull())
		{
			unusedAttrName.erase();
			tempNode = GetNode(node,"RelTimePoint=RelTimePointD",unusedAttrName);
			if (tempNode.isNull())
			{
				unusedAttrName.erase();
				tempNode = GetNode(node,"RelIncrTimePoint=RelIncrTimePointD",unusedAttrName);
			}
		}			
*/
		if (tempNode.isNull())
			retNode = GenericDS();
		else
		{
			tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
			timePointValue = atof(refString.c_str());

			unusedAttrName.erase();
			tempNode = GetNode(node,"Duration=DurationD",unusedAttrName);
			if (tempNode.isNull())
			{
				unusedAttrName.erase();
				tempNode = GetNode(node,"IncrDuration=IncrDurationD",unusedAttrName);
			}
//			if (tempNode.isNull())
//				retNode = GenericDS();
//			else
			if (!tempNode.isNull())
			{
				tempNode.GetTextAttribute( COMPOSITE_NODE_ATTR_NAME, refString );
				timeDurationValue = atof(refString.c_str());
			}

			// Build start,end pair
			sprintf(valBuf,"%f", timePointValue + timeDurationValue);
			refString = valBuf;
	
		}
	}
	else
		// This was not a Time node
		retNode = GenericDS();	// Null node

	// Set the value into the CompositeNodeValue attribute
	if (!retNode.isNull())
	{
		retNode.SetAttribute(COMPOSITE_NODE_ATTR_NAME,refString);
		attrName = COMPOSITE_NODE_ATTR_NAME;
	}
	return retNode;
}

//----------------------------------------------------------------------------
// Convert the TimePoint D string into Julian seconds
// Use optional node to resolve FractionalSize if not supplied in TimePoint string
//----------------------------------------------------------------------------
string DOMNavigator::ConvertFractionalTime(string tp, GenericDS node)
{
	char valBuf[100];
	int i = 0;
	double valSign = 1, tzdSign = 1;
	double Y=0, M=1, D=1, h=0, m=0, s=0, zh=0, zm=0, N=0;	// Year, Month, Day, Hour, Minute, Second, TimeZoneHour, TimeZoneMinute, FractionalSize
	double n=0.0, value = 0.0;	// FractionalCount, resultant value
		
	// Test empty string
	if (tp.empty())
		return string();

	// Parse the string left to right
	// Negative
	if (tp[i] == '-')
	{
		i++;
		valSign = true;
	}

	// Date
	if (isdigit(tp[i]) && tp[i+2] != ':')	// Legal (but unlikely) case of only providing a TZD "-06:00"
	{
		Y = atof(&tp[i]);
		while(isdigit(tp[i]))	i++;	// Scan up to next delimiter
		if (tp[i] == '-' && tp[i+2] != ':')	// Legal (but unlikely) case of only providing a Year plus TZD "1989-06:00"
		{
			M = atof(&tp[++i]);
			while(isdigit(tp[i]))	i++;	// Scan up to next delimiter
			if (tp[i] == '-' && tp[i+2] != ':')	// Legal (but unlikely) case of only providing a Year, Month plus TZD "1989-07-06:00"
			{
				D = atof(&tp[++i]);
				while(isdigit(tp[i]))	i++;	// Scan up to next delimiter
			}
		}
	}

	// Time
	if (tp[i] == 'T')
	{
		h = atof(&tp[i+1]);
		i += 3;
		if (tp[i] == ':')
		{
			m = atof(&tp[++i]);
			i += 2;
			if (tp[i] == ':')
			{
				s = atof(&tp[++i]);
				i += 2;
				if (tp[i] == ':')
				{
					n = atof(&tp[++i]);
					while(isdigit(tp[i]) || tp[i] =='.')	i++;	// Scan up to next delimiter
					//assert(...) TODO exactly two decimal places
				}
			}
		}
	}

	// TimeZone
	if (tp[i] == '+' || tp[i] == '-')
	{
		assert(tp[i+3] == ':');
		tzdSign = (tp[i] == '-') ? -1 : 1;
		zh = atof(&tp[i+1]);
		zm = atof(&tp[i+4]);
		i+= 6;
	}

	// FractionalSize
	if (tp[i] == 'F')
		N = atof(&tp[i+1]);

	// Get default FractionalSize if not specified
	if (n != 0.0 && N == 0)
	{
		assert(!node.isNull());
		//N = GetDefaultFractionOfSecond(node);
	}

	value = valSign * (((ConvertYMDToJulian(Y,M,D) * 24 + h + tzdSign * zh) * 60 + m + tzdSign * zm) * 60 + s + (N?n/N:0));

	sprintf(valBuf, "%f", value);

	return valBuf;
}


//----------------------------------------------------------------------------
// Convert the Duration D string into seconds
// Use optional node to resolve FractionalSize if not supplied in TimePoint string
//----------------------------------------------------------------------------
string DOMNavigator::ConvertFractionalDuration(string dur, GenericDS node)
{
	char valBuf[100];
	int iNum = 0, iDelim = 0;
	double valSign = 1;
	double D=0, h=0, m=0, s=0, N=0;	// Day, Hour, Minute, Second, FractionalSize
	double n=0.0, value = 0.0;	// FractionalCount, resultant value
		
	// Test empty string
	if (dur.empty())
		return string();

	// Parse the string left to right
	while( dur[iDelim] )
	{
		if (isdigit(dur[iDelim]))
		{
			iDelim++;
			continue;
		}

		switch( dur[iDelim] )
		{
		case '-':
			if (iDelim+6 < dur.size() && dur[iDelim+6] == 'Z')
				iDelim += 6;	// Don't handle TZD in Duration D
			else
				valSign = -1;
			break;
		case 'P':
		case 'T':
			break;
		case 'D':
			D = atof(&dur[iNum]);
			break;
		case 'H':
			h = atof(&dur[iNum]);
			break;
		case 'M':
			m = atof(&dur[iNum]);
			break;
		case 'S':
			s = atof(&dur[iNum]);
			break;
		case 'N':
			n = atof(&dur[iNum]);
			break;
		case 'f':
			assert(iNum == iDelim-2);	// Fixed fractional size
			n += atof(&dur[iNum]) / 100;
			break;
		case 'F':
			N = atof(&dur[iNum]);
			break;
		default:
			eout << "Illegal delimiter \"" << dur[iDelim] << "\" found in DurationD \"" << dur.c_str() << "\"" << endl;
			assert(0);
		}

		// Advance
		iDelim++;
		iNum = iDelim;
	}

	// Get default FractionalSize if not specified
	if (n != 0.0 && N == 0)
	{
		assert(!node.isNull());
		//N = GetDefaultFractionOfSecond(node);
	}

	value = valSign * (((D * 24 + h) * 60 + m) * 60 + s + (N?n/N:0));

	sprintf(valBuf, "%f", value);

	return valBuf;
}


//----------------------------------------------------------------------------
// Get the timeunit for this node, search ancestors if necessary
//----------------------------------------------------------------------------
double DOMNavigator::GetTimeUnit(GenericDS node)
{
	string refString;

	// Retrieve from this node's attribute
	if (node.GetTextAttribute("timeUnit", refString) == 0)
		return atof( ConvertFractionalDuration(refString, node).c_str() );		

	// Retrieve from ancestors
	//assert(0);	// Not implemented
	return 0.0;
}


//----------------------------------------------------------------------------
// Get the ancestor path for a node
//----------------------------------------------------------------------------
string DOMNavigator::GetAncestorPath(const GenericDS node, int depth)
{
	string ancestorPath;
	GenericDS ancestorNode = node;
	for(int i=0; i < depth && !ancestorNode.isNull(); i++)
	{
		if(i)
			ancestorPath.insert(0,"/");
		ancestorPath.insert(0,ancestorNode.GetDSName());
		ancestorNode = ancestorNode.GetParent();
	}
	return ancestorPath;
}


//=============================================================================
double XM::ConvertYMDToJulian(int year, int month, int day)
{
	int ta,tb,tc;

	if( year  <=0 || year <1900){return(0);}
	assert(year  >=1900);
	assert(month >0 && month <= 12);
	assert(day   >0 && day   <= 31);

	if(month>2){month-=3;}
	else {month+=9,year--;}
	ta = 146097* (year/100)/4;
	tb = 1461L * (year%100)/4;
	tc = (153L * month+2)  /5+day+1721119;
	return( ((double)ta+tb+tc) );
}


//=============================================================================
int DOMQueryDS::SetDOMQuery(DOMQuery *aDOMQuery)
{
	m_DOMQuery = aDOMQuery;
	return 0;
}

//----------------------------------------------------------------------------
// Build a query string from the given DOM Document and build a query from this query string.
// Return true/false of success.
//----------------------------------------------------------------------------
int DOMQueryDS::BuildQueryFromDOM(GenericDSInterfaceABC *DOMDoc)
{
	char *queryString = 0;

	// Test for a valid XML document 
	if ( DOMDoc->isNull() || DOMDoc->GetChild((int)0).isNull() )
		return 0;

	// Build the query string
	queryString = BuildQueryStringFromNode( DOMDoc->GetChild((int)0) );

	// Set the query into the DOMQuery
	return m_DOMQuery->BuildQueryFromString( queryString ) == 0.0 ? 0 : 1;
}

//----------------------------------------------------------------------------
// Build a query string that mimics the given node, recurse for child elment nodes.
// Return a new buffer that must be freed by caller, or null if no text is found.
//----------------------------------------------------------------------------
char *DOMQueryDS::BuildQueryStringFromNode(GenericDS node)
{
	GenericDS aChild;
	int iChild;
	string text;
	char *location= 0;
	char *queryString = 0;
	DOMNavigator DOMNav;	//DOMNavigator does not use xerces directly.  All DOM interation is through GenericDS

	// Examine child attributes
	typedef vector<string> VectString;	// convenience defn
	VectString attributeList = node.GetAttributeNames();
	VectString::iterator pAttr;
	for(pAttr = attributeList.begin(); pAttr != attributeList.end(); pAttr++)
	{
		node.GetTextAttribute( pAttr->c_str(), text );
		location = DOMNav.GetLocation( node, pAttr->c_str() );
		AppendQueryString( queryString, location, text.empty()?0:text.c_str() );
	}

	// Examine this node's text
	if ( !(text = DOMNav.GetText(node,false,string())).empty() &&
		 (location = DOMNav.GetLocation(node, "")) )
	{
		 // Only add non-null text at this point
		 AppendQueryString( queryString, location, text.empty()?0:text.c_str() );
	}

	// Examine child element nodes
	for( iChild = 0; iChild < node.GetNumberOfChildren(); iChild ++ )
	{
		AppendQueryString( queryString, BuildQueryStringFromNode(node.GetChild(iChild)) );
	}


	// Test if this was an empty leaf, create an empty queryText
	if ( text.empty() && !*queryString )
		AppendQueryString( queryString, location, 0 );

	// Delete any local strings
	delete location;

	return queryString;
}

//----------------------------------------------------------------------------
// Append the given text and optional query text to the queryString, using AND operator
// Replace the given queryString with a new buffer, and free the old.
//----------------------------------------------------------------------------
char *DOMQueryDS::AppendQueryString(char *&queryString, char *text1, const char *queryText)
{
	char *newQueryString;

	// Increase allocation, allow for colon, two quotes, operator &, and termination
	newQueryString = new char[strlen(text1) + 
							  (queryText?strlen(queryText)+3:0) +
							  (queryString?strlen(queryString)+1:0) + 1];
	assert( newQueryString );
	newQueryString[0] = '\0';

	// Update queryString, append operator
	if ( queryString )
	{
		strcpy( newQueryString, queryString );
		delete queryString;
		strcat( newQueryString, "&" );
	}
	queryString = newQueryString;

	// Append text1 (this is either a complete queryString or a location)
	strcat( queryString, text1 );

	// queryText present means that text1 is a location
	if ( queryText )
	{
		strcat( queryString, ":\"" );
		strcat( queryString, queryText );
		strcat( queryString, "\"" );
	}

	return queryString;
}

//=============================================================================
NodeList::NodeList() : 
m_active(false), 
m_numNodes(0)
{
	m_nodes = m_fastNodes;
	m_attrNames = m_fastAttrNames;
	m_nodeListLength = NodeList_INIT_NODE_LIST;
}

//----------------------------------------------------------------------------
NodeList::NodeList(GenericDS node, const string &attrNode) :
m_active(false), 
m_numNodes(0)
{
	m_nodes = m_fastNodes;
	m_attrNames = m_fastAttrNames;
	m_nodeListLength = NodeList_INIT_NODE_LIST;
	Append(node, attrNode);	//TODO use default constructor
}

//----------------------------------------------------------------------------
NodeList::~NodeList()
{
	Clear();
}

//----------------------------------------------------------------------------
void NodeList::Clear()
{
	for( int i=0; i<m_numNodes; i++)
		delete m_nodes[i];
	if ( m_nodeListLength > NodeList_INIT_NODE_LIST)
	{
		delete [] m_nodes;
		delete [] m_attrNames;
	}

	m_active = false;
	m_nodes = m_fastNodes;
	m_attrNames = m_fastAttrNames;
	m_nodeListLength = NodeList_INIT_NODE_LIST;
	m_numNodes = 0;
}

//----------------------------------------------------------------------------
void NodeList::Append(GenericDS node, const string &attrName)
{
	// Add space for pointer to NodeWrapper 
	if ( m_numNodes >= m_nodeListLength )
	{
		GenericDS **newList = new GenericDS *[m_nodeListLength * 2];
		string *newAttrNames = new string[m_nodeListLength * 2];
		assert( newList && newAttrNames );
		memcpy( newList, m_nodes, m_numNodes * sizeof(GenericDS*) );
		for( int i=0; i<m_numNodes; i++ )	// Can you memcpy array of strings?
			newAttrNames[i] = m_attrNames[i];
		if ( m_nodeListLength > NodeList_INIT_NODE_LIST)
		{
			delete [] m_nodes;
			delete [] m_attrNames;
		}
		m_nodes = newList;
		m_attrNames = newAttrNames;
		m_nodeListLength *= 2;
	}

	// Add NodeWrapper
	m_nodes[m_numNodes] = new GenericDS();
	assert( m_nodes[m_numNodes] );

	// Assignment
//	m_nodes[m_numNodes++]->UnWrap() = node;
	m_attrNames[m_numNodes] = attrName;
	(*m_nodes[m_numNodes++]) = node;
}

//----------------------------------------------------------------------------
void NodeList::Append(NodeList nodeList)
{
	for( int i=0; i<nodeList.GetLength(); i++ )
		Append( nodeList.Item(i), nodeList.ItemAttrName(i) );
}

//=============================================================================
double XM::CustomMatchFunc_Equal(							const char *refString, const char *queryString, const char *funcOptionString)
{ return atof(refString) == atof(queryString) ? 1.0 : 0.0; }

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_LessThan(						const char *refString, const char *queryString, const char *funcOptionString)
{ return atof(refString) < atof(queryString) ? 1.0 : 0.0; }

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_GreaterThan(						const char *refString, const char *queryString, const char *funcOptionString)
{ return atof(refString) > atof(queryString) ? 1.0 : 0.0; }

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_GreaterThan_LessThan(			const char *refString, const char *queryString, const char *funcOptionString)
{ 
	double right;
	double ref = atof(refString);
	double left = atof(queryString);
	char *comma = (char*)strchr(queryString,',');
	if ( comma )
		right = atof(++comma);
	return (ref > left && ref < right) ? 1.0 : 0.0; 
}

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_LessThanEqual(					const char *refString, const char *queryString, const char *funcOptionString)
{ return atof(refString) <= atof(queryString) ? 1.0 : 0.0; }

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_GreaterThanEqual(				const char *refString, const char *queryString, const char *funcOptionString)
{ return atof(refString) >= atof(queryString) ? 1.0 : 0.0; }

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_GreaterThanEqual_LessThanEqual(	const char *refString, const char *queryString, const char *funcOptionString)
{ 
	double right;
	double ref = atof(refString);
	double left = atof(queryString);
	char *comma = (char*)strchr(queryString,',');
	if ( comma )
		right = atof(++comma);
	return (ref >= left && ref <= right) ? 1.0 : 0.0; 
}

//----------------------------------------------------------------------------
double XM::CustomMatchFunc_GreaterThanEqual_LessThan(		const char *refString, const char *queryString, const char *funcOptionString)
{ 
	double right;
	double ref = atof(refString);
	double left = atof(queryString);
	const char *comma = strchr(queryString,',');
	if ( comma )
		right = atof(++comma);
	return (ref >= left && ref < right) ? 1.0 : 0.0; 
}

