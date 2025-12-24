////////////////////////////////////////////////////////////////////////
//
// UserPreferenceSearch.cpp
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


//#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"                     /* Naming Convention */
#include "SearchUtilities/SearchUtilities.h"
#include "UserPreferenceSearch.h"	//TODO remove after development is stable
#include "Utilities/readparam.h"

//=============================================================================
	

using namespace XM;
using namespace std;

const UUID UserPreferenceSearchInterface::myID = UUID("");
const char *UserPreferenceSearchInterface::myName = "User Preference Search Interface";

const UUID UserPreferenceSearchTool::myID = UUID("");
const char *UserPreferenceSearchTool::myName = "User Preference Search Tool";

const UUID UserPreferenceSearchInterfaceABC::myID = UUID();

//=============================================================================
// The MPEG7UPMap::map is the basis for converting UserPreferenceDS into queries
// against the ProgramDS.  Each entry is composed of:
// name		A single element name from the UserPreferenceDS. (Case sensitive)
// 			Each element along a path in the UP must be listed separately.
// location	A partial path specifier for mapping in the ProgramDS.
// 			While decomposing the UP path, these locations are appended to build the
// 			ProgramDS path.
// intraOp	Operator used to combine adjacent similar preferences of this mapping.
// interOp	Operator used to combine this mapping (singly or in intraGroup) with 
// 			other different mappings within the parent preference.
// testOp	Match operator used to compare the content of this UP with the
// 			content of the associated location in the ProgramDS.  A UP should have
// 			either content or children.  It is an error to have both or neither.
// Entries that have maximum cardinality 1 may use NA_GO for intraOp because they 
// should never be adjacent to the same mapping.
// Top level entries (e.g. BrowsingPreferences, BrowsingPreferenceType) may use NA_GO
// for interOp because their inter-processing is hard-coded.
// Entries that have children may use NA_TO for testOp because should not have content.
// 
// Several map functions use a range of the entries in this array.  For that purpose, 
// all the preferences and preference types should be kept together.  Preferences at
// the beginning, preference types (just 2) at the end.

MPEG7UPMapEntry MPEG7UPMap::map[MPEG7UP_MAX_NMAP];
int MPEG7UPMap::nMap = 0;


#ifdef VERBOSE
#define vout cout	// VERBOSE output is directed to stdout for capture by redirection (unable to redirect stderr in DOS) 
#define eout cout << "Error: "	// Error output is redirected to stdout when VERBOSE is defined
#else
#define vout cerr
#define eout cerr
#endif

//=============================================================================
UserPreferenceSearchInterfaceABC::UserPreferenceSearchInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
UserPreferenceSearchInterface::UserPreferenceSearchInterface(UserPreferenceSearchTool* aTool):
m_SearchTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
UserPreferenceSearchInterface::~UserPreferenceSearchInterface()
{
}

//----------------------------------------------------------------------------
void UserPreferenceSearchInterface::destroy()
{
  delete m_SearchTool;
}

//----------------------------------------------------------------------------
int UserPreferenceSearchInterface::SetValuesFromParameterFile()
{
	return m_SearchTool->SetValuesFromParameterFile();
}

//----------------------------------------------------------------------------
int UserPreferenceSearchInterface::
SetRefDescriptionInterface(GenericDSInterfaceABC
		       *aRefGenericDSInterface)

{
  return m_SearchTool->
    SetRefDescriptionInterface(aRefGenericDSInterface);
}

//----------------------------------------------------------------------------
int UserPreferenceSearchInterface::
SetQueryDescriptionInterface(GenericDSInterfaceABC
		       *aQueryGenericDSInterface)

{
  return m_SearchTool->
    SetQueryDescriptionInterface(aQueryGenericDSInterface);
}

//----------------------------------------------------------------------------
double UserPreferenceSearchInterface::GetDistance(void)

{
  return m_SearchTool->GetDistance();
}

//----------------------------------------------------------------------------
const UUID& UserPreferenceSearchInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *UserPreferenceSearchInterface::GetName(void)
{ return myName; }


//=============================================================================
UserPreferenceSearchTool::UserPreferenceSearchTool():
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
	SetValuesFromParameterFile();

	m_FilterManager = new FilterManager();
	assert( m_FilterManager);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
UserPreferenceSearchTool::UserPreferenceSearchTool(GenericDSInterfaceABC
						   *aDescription):
m_Interface(this),
m_RefDescriptionInterface(0),
m_QueryDescriptionInterface(0)

{
	SetValuesFromParameterFile();

	m_FilterManager = new FilterManager();
	assert( m_FilterManager);
	
	SetQueryDescriptionInterface(aDescription);

	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
UserPreferenceSearchTool::~UserPreferenceSearchTool()
{
}

//----------------------------------------------------------------------------
const UUID& UserPreferenceSearchTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *UserPreferenceSearchTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
bool UserPreferenceSearchTool::SupportsPush(void)
{ return true; }

//----------------------------------------------------------------------------
bool UserPreferenceSearchTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
UserPreferenceSearchInterfaceABC *UserPreferenceSearchTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
int UserPreferenceSearchTool::SetValuesFromParameterFile(void)
{
	char *mapFileName = getpara("MapFile",NULL);

	if (!mapFileName)
	{
		eout << "Unable to read MapFile name from param file" << endl;
		exit(1);
	}

	ifstream mapFile(mapFileName);
	if (!mapFile.is_open())
	{
		eout << "Unable to open \"" << mapFileName << "\"" << endl;
		exit (1);
	}

	// Clear memory, and read the file in, line by line
	memset(MPEG7UPMap::map, 0, sizeof(MPEG7UPMap::map));

	char intra[MAXMEDIANAMELEN], inter[MAXMEDIANAMELEN],
	  test[MAXMEDIANAMELEN], name[MAXMEDIANAMELEN],
	  location[MAXMEDIANAMELEN];
	{
	  int i;
	  for (i=0; i<MPEG7UP_MAX_NMAP; i++)
	    {
		mapFile >> intra;
		mapFile >> inter;
		mapFile >> test;
		mapFile >> name;
		mapFile >> location;

		MPEG7UPMap::map[i].intraOp  = UsagePreferenceMap::StringToGroupOp(intra);
		MPEG7UPMap::map[i].interOp  = UsagePreferenceMap::StringToGroupOp(inter);
		MPEG7UPMap::map[i].testOp   = UsagePreferenceMap::StringToTestOp(test);
		MPEG7UPMap::map[i].name     = strdup(name);
		if (strcmp(location,"0"))
			MPEG7UPMap::map[i].location = strdup(location);

		if(mapFile.eof())
			break;
	    }
	  assert((i<MPEG7UP_MAX_NMAP));
	  MPEG7UPMap::nMap = i+1;
	}

	mapFile.close();

	return 0;
}

//----------------------------------------------------------------------------
// Set the reference Program interface into the Search Tool and the Filter Manager
//----------------------------------------------------------------------------
int UserPreferenceSearchTool::SetRefDescriptionInterface(GenericDSInterfaceABC
												*aRefGenericDSInterface)
{
	m_RefDescriptionInterface = aRefGenericDSInterface;

	//return m_FilterManager->SetProgram( 
	//	m_RefDSInterface->GetDOMDocument() );
	return 0;
}

//----------------------------------------------------------------------------
// Set the UserPreference interface into the Search Tool and the Filter Manager
//----------------------------------------------------------------------------
int  UserPreferenceSearchTool::SetQueryDescriptionInterface(GenericDSInterfaceABC
												   *aQueryGenericDSInterface)
{
	m_QueryDescriptionInterface = aQueryGenericDSInterface;

	GenericDS DOMRoot;
	GenericDS child = m_QueryDescriptionInterface->GetChild((int)0);	// How do you get the GenericDS from the interface???

	if (child.isNull() || (DOMRoot = child.GetParent()).isNull())
	{
		eout << "Unable to parse Query description" << endl;
		exit(-1);
	}

	return m_FilterManager->SetUserPreference( DOMRoot );
}

//----------------------------------------------------------------------------
// Evaluate the query UserPreferenceDS upon the reference ProgramDS 
//----------------------------------------------------------------------------
double UserPreferenceSearchTool::GetDistance(void)
{
	double similarity, distance;
	// Assumptions
	assert(m_RefDescriptionInterface);

	// Filter this one program
	GenericDS testDS = m_RefDescriptionInterface->GetChild((int)0);	// How do you get the GenericDS from the interface???
	GenericDS DOMDocNode = testDS.GetParent();
	while( ! (testDS = DOMDocNode.GetParent()).isNull() )
		DOMDocNode = testDS;

	if (DOMDocNode.isNull())
	{
		eout << "Unable to parse Reference description" << endl;
		exit(-1);
	}

	similarity = m_FilterManager->Filter( DOMDocNode );

	// Convert similarity measure to distance
	distance = (m_FilterManager->GetMaxScore() - similarity) /
				m_FilterManager->GetMaxScore() * 
				DOMQueryOperation::NOMINAL_PREFERENCE_VALUE;

	return distance;
}

//=============================================================================
//  Invoke a Factory to build the FilterAgent for this UserPreference document
//----------------------------------------------------------------------------
int FilterManager::SetUserPreference(GenericDS userPreferenceRoot)
{
	FilterAgentFactory theFactory;

	m_FilterAgentDOM = theFactory.MakeFilterAgentDOM( userPreferenceRoot );

	return m_FilterAgentDOM ? 1 : 0;
}

//----------------------------------------------------------------------------
// Evaluate the FilterAgent on a single Program
//----------------------------------------------------------------------------
double FilterManager::Filter(GenericDS DOMDocNode)
{
	return m_FilterAgentDOM->CheckSimilarity( DOMDocNode );
}

//=============================================================================
FilterAgentDOM::FilterAgentDOM() :
m_MaxScore(1.0)
{
}

//----------------------------------------------------------------------------
FilterAgentDOM::~FilterAgentDOM()
{
}

//----------------------------------------------------------------------------
// Evaluate all FASPs and BPs, taking maximum similarity
//----------------------------------------------------------------------------
double FilterAgentDOM::CheckSimilarity(GenericDS DOMDocNode)
{
	return m_PreferenceQuery.CheckSimilarity( DOMDocNode );
}

//=============================================================================
PreferenceQuery::PreferenceQuery()
{
}

//----------------------------------------------------------------------------
PreferenceQuery::~PreferenceQuery()
{
}

//----------------------------------------------------------------------------
double PreferenceQuery::CheckSimilarity(GenericDS DOMDocNode)
{
	return m_DOMQuery.CheckSimilarity(DOMDocNode);
}

//=============================================================================
FilterAgentFactory::FilterAgentFactory():
m_UPList(0),
m_UPMap(0)
{
}

//----------------------------------------------------------------------------
FilterAgentFactory::~FilterAgentFactory()
{
	delete m_UPList;
	delete m_UPMap;
}

//----------------------------------------------------------------------------
// Traverse the UsagePreferenceList and use UsagePreferenceMap to build the FilterAgent
// for a XML based implementation
//----------------------------------------------------------------------------
FilterAgentDOM *FilterAgentFactory::MakeFilterAgentDOM(GenericDS userPreferenceRoot)
{
	GenericDS rootPreferenceNode;
	double maxScore;
	FilterAgentDOM *theFilterAgentDOM = new FilterAgentDOM();
	string mappedQuery;

	// Assumptions
	assert( !userPreferenceRoot.isNull() );

	// Instantiate (load) the list and map
	m_UPList = new MPEG7UPList( userPreferenceRoot );
	m_UPMap = new MPEG7UPMap;

	// Get root UsagePreference node
	rootPreferenceNode = static_cast<MPEG7UPList*>(m_UPList)->GetUsagePreferenceNode();

	// Process all the queries for this root preference
	mappedQuery = BuildMappedQuery( rootPreferenceNode, string(), string() );
	// Post process the recursive BuildMappedQuery: convert ' to "
	int iFind = 0;
	while ((iFind = mappedQuery.find("'",iFind)) != string::npos)
		mappedQuery.replace(iFind,1,"\"");

	if ( !mappedQuery.empty() )
	{
#ifdef VERBOSE
		vout << "MappedQuery: \"" << mappedQuery.c_str() << "\"" << endl;
		// TODO incorporate RemoveRedundantGroupings on actual query
		vout << "==\"" << RemoveRedundantGroupings(mappedQuery.c_str()).c_str() << "\"" << endl;
#endif
		// Set this Mapped QueryString into the PreferenceQuery
		maxScore = theFilterAgentDOM->m_PreferenceQuery.BuildQueryFromString( mappedQuery );
		theFilterAgentDOM->m_MaxScore = maxScore;
#ifdef VERBOSE
		vout << "MappedQuery has MaxScore = " << maxScore << endl;
#endif
	}
	else // Error
	{
		eout << "Could not build mapped query. Exiting" << endl;
		exit(1);
	}

	return theFilterAgentDOM;
}

//----------------------------------------------------------------------------
// This is the core of the Factory, retrieves each child node and either appends
// a query with the child's mapped location & testOp & content, or recurses into
// the child node to build the query.
//----------------------------------------------------------------------------
string FilterAgentFactory::BuildMappedQuery( GenericDS &node, string progPath, 
											 string prefPath, bool bRecursToggleIntra )
{
	DOMNavigator DOMNav;
	GenericDS childPrefNode;
	string childPrefTag, prevChildPrefTag, childPrefTargValue;
	string childPrefPath, childProgPath;
	string singleExp;

	// Preferences with the same element tag are grouped into attribute and multi-map groups:
	//		prefPath		progPath
	//		/X/Y/Z%a1		loc1
	//		/X/Y/Z			loc2
	//		/X/Y/Z%a2		loc3
	//		/X/Y/Z%a3		loc4
	//		/X/Y/Z			loc5
	//		/X/Y/Z			loc6
	// The first two mappings are in one attrGroup, the next three are in a second attrGroup, and the last is a third
	// All three attrGroups are included in the multi-map group for /X/Y/Z

	// attrBaseExp, multiMapBaseExp and baseExp contain completed portions of the expressions being built
	// attrIntrExp and intrExp contain the current intra expressions
	// attrGroupExp, multiMapGroupExp and resultExpression contain the result for each pass, assembled out of the other expressions

	string attrBaseExp, attrIntraExp, attrGroupExp;
	string multiMapBaseExp, multiMapGroupExp;
	string baseExp, intraExp, resultExpression;
	int iFindMap, iMap, prevMap = -1;	// index of current and previous mapping in m_UPMap
	UsagePreferenceMap::GroupOp prevIntraOp = UsagePreferenceMap::NA_GO;
	UsagePreferenceMap::GroupOp prevInterOp = UsagePreferenceMap::NA_GO;
	bool inIntraGroup, inAttrGroup/*, inMultiMapGroup*/;
	string preferenceValue;
	bool childToggleIntra;

	// Determine if recursive children should toggle their intra-group operators.
	// If parent and all children are the same type, and input flag was false
	childToggleIntra = !bRecursToggleIntra;
	while (childToggleIntra && m_UPList->GetNextPreferenceNode(node, childPrefNode))
	{
		childToggleIntra = prefPath.compare( childPrefNode.GetDSName() ) == 0;
	}


	// Process all the child preference nodes for this node
	childPrefNode = GenericDS();
	while( m_UPList->GetNextPreferenceNode(node, childPrefNode) )
	{
		// Get this child preference tag
		childPrefTag = childPrefNode.GetDSName();
		// Build the child preference path
		if (!prefPath.empty())
			childPrefPath = prefPath + '/' + childPrefTag;
		else
			childPrefPath = childPrefTag;

		// Get child preference value
		if (childPrefNode.GetTextAttribute("preferenceValue", preferenceValue) != 0)
			preferenceValue.erase();

		// Initialize attribute group and multi-mapped group expressions
		inAttrGroup = false;
		attrBaseExp.erase();
		attrGroupExp.erase();
		multiMapBaseExp.erase();
		multiMapGroupExp.erase();
		
		// Loop through all the multiple mappings for this one preference name (e.g. Keyword => Title, Keyword => Description...)
		// Instruct FindPreferenceMap to check the ancestry on the first pass only (iStart == 0)
		int iStartMap = 0;
		while ( (iFindMap = m_UPMap->FindPreferenceMap(childPrefPath, iStartMap==0, iStartMap)) >= 0 )
		{
			// Prevent non-continuous map listings from forming attribute group
			if (iFindMap != iMap + 1)
				inAttrGroup = false;

			iMap = iFindMap;
			// Loop through the attributes (and the one content preference) of this preference node
			// Assumes that attributes precede content listings in the mapping table
			// Note - this could be expanded to include preference nodes that have content and children...
			{
				// Build the child program path (i.e. target)
				childProgPath = progPath.empty() ? "''" : progPath;		// Always quote the progPath
				const char *pLoc = m_UPMap->GetMappedLocation( iMap );
				if ( pLoc )
				{
					int nLocLen = strlen(pLoc);
					if (*pLoc == '\'' || *pLoc == '\"')
					{
						assert(pLoc[strlen(pLoc)-1] == '\'' ||
							pLoc[strlen(pLoc)-1] == '"');
						pLoc++;
						nLocLen -= 2;
					}
					childProgPath.insert(childProgPath.length()-1, pLoc, nLocLen);
				}

				// Each preference mapping refers to a preference node that contains children or has some test value
				// Examine the test operator to determine
				if ( m_UPMap->GetMappedTestOp(iMap) != m_UPMap->NA_TO )
				{
					// Grab the preference value, If this is a composite node, grab the composite value
					char *compositeLocation;
					if ((compositeLocation = (char*)strchr(m_UPMap->GetName(iMap), '=')) )
					{
						string attrName;
						DOMNav.GetNode(childPrefNode, compositeLocation, attrName);	// TODO use dbgPath
						if (childPrefNode.GetTextAttribute(attrName,childPrefTargValue) != 0)
						{
							// It is possible (esp. in the case of attributes) that the preference composite value doesn't exist for this node
							//eout << "Could not create composite for \"" << childPrefPath << "\" using composite location: " << compositeLocation << endl;
							iStartMap = iMap + 1;
							continue;
						}
					}
					else
						// Just grab the (element content) value
						childPrefTargValue = DOMNav.GetText(childPrefNode,false);

					// Leaf preferences should have value
					if ( !childPrefTargValue.empty() )
					{
						// Build the single mapped query test
						singleExp = childProgPath;
						singleExp += m_UPMap->GetMappedTestOperator( iMap );
						singleExp += "'" + childPrefTargValue + "'";	// Use ' temporarily, to prevent breaking up this string (when inserting PVs)
						// add preference value and other options
						// TODO insert PV for attribute groups at the highest containing operator, not at each test
						if (!preferenceValue.empty())
							singleExp += ":pv" + preferenceValue + m_UPMap->GetMappedTestOptions( iMap );
						else if ( m_UPMap->GetMappedTestOptions( iMap )[0] )
						{
							singleExp += ":"; 
							singleExp += m_UPMap->GetMappedTestOptions( iMap );
						}
					}
					else // Error condition in preference input
					{
						eout << "Preference path \"" << childPrefPath.c_str() << "\" is missing test value. Discarding test." << endl;
						// Search for next mapping after this one
						iStartMap = iMap + 1;
						continue;
					}
				}

				// Preference mapping with NA_TO test operator should have children nodes
				else
				{
					// Recurse and build any children
					singleExp = BuildMappedQuery( childPrefNode, childProgPath, childPrefPath, childToggleIntra );

					// Insert preference value into this sub-mapped query
					if (!preferenceValue.empty() && !singleExp.empty())
					{
						char *copySingleExp = new char[singleExp.length() + 1];
						char *left, *right;
						int  nCmp;
						strcpy(copySingleExp, singleExp.c_str());
						// Searh for operator
						if (FindLeftmostHighestOperator(copySingleExp, left, right, DOMQuery::BINARY_OPERATORS, 0) > 0)
						{
							// Insert pv after operator
							nCmp = singleExp.find(right);	assert(nCmp > 0);
							singleExp.insert(nCmp, "pv" + preferenceValue);
						}
						else
						{
							// Insert pv as option of testop
							// Peel off layers of grouping parens
							strcpy(copySingleExp, singleExp.c_str());	// recopy
							right = copySingleExp + strlen(copySingleExp) - 1;
							while( (right > copySingleExp) && 
								   (RevSkipOverPair(copySingleExp, right) < right - 1))
								right --;
							// right is pointing at last non-paren char
							// check for existing :pvNN, e.g. /creator pv="70"/firstname pv="90"
							assert(singleExp.find(":pv") == string::npos);	// TODO
							singleExp.insert( right - copySingleExp + 1, ":pv" + preferenceValue);
						}
						delete copySingleExp;
					}
				}

				if ( singleExp.empty() )
				{
					eout << "Empty expression from BuildMappedQuery() for \"" << childPrefPath.c_str() << "\".";
					//if ( m_UPMap->GetMappedTestOp(iMap) == m_UPMap->NA_TO )	//TODO Some set of tests should permit the test of presence (e.g. STR_ISUB_DEFAULT_EMPTY)
					//{
						eout << "  Discarding test" << endl;
						// Search for next mapping after this one
						iStartMap = iMap + 1;
						continue;
					//}
					//else
					//{
					//	singleExp = childProgPath + ":";
					//	eout << "  Building empty test: \"" << singleExp.c_str() << "\"" << endl;
					//}
				}


				// Changed attribute groups, so insert attrGroupExp into multiMapGroupExp, and reset
				if (!inAttrGroup && !attrGroupExp.empty())
				{
					if (!multiMapBaseExp.empty())
						multiMapBaseExp += m_UPMap->GetOperator(MPEG7UPMap::OR);	// TODO use GetMappedCombOp...
					multiMapBaseExp += "(" + attrGroupExp + ")";
					attrBaseExp.erase();
					attrIntraExp.erase();
				}

				// Assemble attributes (and the one content preference) into attrGroupExp (always valid expression)
				// This becomes a AND combination when we are within an attribute group
				UsagePreferenceMap::GroupOp interOp		    = UsagePreferenceMap::HAND;	// TODO use GetMappedCombOp and add AttrCombOp to mapping table
				UsagePreferenceMap::GroupOp attrPrevInterOp = UsagePreferenceMap::NA_GO;
				if (inAttrGroup)
					attrPrevInterOp = interOp;
					
				// If we are in attribute group, attrBaseExp, attrIntraExp will hold the pending component expressions
				// Continue to build these components into running attrGroupExp
				attrGroupExp = CombineExpressions(attrBaseExp, attrIntraExp, singleExp, 
													UsagePreferenceMap::NA_GO, interOp, 
													attrPrevInterOp, false);

				// Reset inAttrGroup flag
				if (strchr(m_UPMap->GetName(iMap), '%'))	// TODO better way of detecting attribute composite value??
					inAttrGroup = true;
				else
					inAttrGroup = false;

			}	// Loop preference names for this one node


			// Assemble multi-mapped preferences into multiMapGroupExp using OR
			assert(!attrGroupExp.empty());	// In all cases there should be some valid attrGroupExp expression

			// Assemble running multi-mapped group expression
			if (multiMapBaseExp.empty())
				multiMapGroupExp = "(" + attrGroupExp + ")";
			else
			{
				multiMapGroupExp = multiMapBaseExp + m_UPMap->GetOperator(MPEG7UPMap::OR);	// TODO use GetMappedCombOp...
				multiMapGroupExp += "(" + attrGroupExp + ")";
			}


			// Search for next mapping after this one
			iStartMap = iMap + 1;
		}	// Loop thru possible mapping for one node

		if ( iStartMap == 0 && iFindMap < 0 )
		{
			eout << "Could not find mapping for \"" << childPrefPath.c_str() << "\" in UsagePreferenceMap!" << endl;
			continue;
		}

		// Test if this was empty (for valid reason or error)
		if ( multiMapGroupExp.empty() )
			continue;

		// At the bottom of this inAttrGroup and inMultiMapGroup processing, multiMapGroupExp should be the result


		inIntraGroup = iMap == prevMap;


		// Combine with other resolved preferences at this level
		resultExpression = CombineExpressions( baseExp, intraExp, "("+multiMapGroupExp+")", 
												m_UPMap->GetMappedIntraOp(iMap, bRecursToggleIntra), 
												m_UPMap->GetMappedInterOp(iMap),
												prevInterOp, 
												inIntraGroup );

		prevIntraOp = m_UPMap->GetMappedIntraOp(iMap);
		prevInterOp = m_UPMap->GetMappedInterOp(iMap);
		prevMap = iMap;
		prevChildPrefTag = childPrefTag;
	}

	return resultExpression;
}



//----------------------------------------------------------------------------
string FilterAgentFactory::CombineExpressions( string &baseExp, string &intraExp, 
												const string &newExp, 
												UsagePreferenceMap::GroupOp intraOp,
												UsagePreferenceMap::GroupOp interOp,
												UsagePreferenceMap::GroupOp prevInter, 
												bool inIntraGroup )
{
	string resultExp;

	// Within an intra group, Continue intra
	if ( inIntraGroup )
	{
		if ( intraOp == UsagePreferenceMap::OR )
			intraExp += m_UPMap->GetOperator(MPEG7UPMap::OR); // TODO use GetMappedCombOp...
		else if ( intraOp == UsagePreferenceMap::HOR )
			intraExp += m_UPMap->GetOperator(MPEG7UPMap::HOR); // TODO use GetMappedCombOp...
		else if ( intraOp == UsagePreferenceMap::AND )
			intraExp += m_UPMap->GetOperator(MPEG7UPMap::AND);	// TODO use GetMappedCombOp...
		else if ( intraOp == UsagePreferenceMap::HAND )
			intraExp += m_UPMap->GetOperator(MPEG7UPMap::HAND);	// TODO use GetMappedCombOp...
		intraExp += newExp;
	}
	else // Not within intra group, Close intra and start new intra
	{
		switch (prevInter)
		{
		case UsagePreferenceMap::OR:
			// Prepend OR expressions
			if ( !baseExp.empty() )
				baseExp.insert(0, m_UPMap->GetOperator(MPEG7UPMap::OR)); // TODO use GetMappedCombOp...
			baseExp.insert(0, intraExp);
			break;

		case UsagePreferenceMap::HOR:
			// Prepend OR expressions
			if ( !baseExp.empty() )
				baseExp.insert(0, m_UPMap->GetOperator(MPEG7UPMap::HOR)); // TODO use GetMappedCombOp...
			baseExp.insert(0, intraExp);
			break;

		case UsagePreferenceMap::AND:
			// Append AND expressions
			if ( !baseExp.empty() )
				baseExp += m_UPMap->GetOperator(MPEG7UPMap::AND); // TODO use GetMappedCombOp...
			baseExp += intraExp;
			break;

		case UsagePreferenceMap::HAND:
			// Append AND expressions
			if ( !baseExp.empty() )
				baseExp += m_UPMap->GetOperator(MPEG7UPMap::HAND); // TODO use GetMappedCombOp...
			baseExp += intraExp;
			break;

		default:	assert(baseExp.empty());
		}
		// Start a new intra
		intraExp = newExp;
	}


	// Build a result expression from the pending intra 
	// (Similar to Close above, except using resultExp, and testing current interOp)
	resultExp = baseExp;
	switch (interOp)
	{
	case UsagePreferenceMap::OR:
		// Prepend OR expressions
		if ( !resultExp.empty() )
			resultExp.insert(0, m_UPMap->GetOperator(MPEG7UPMap::OR)); // TODO use GetMappedCombOp...
		resultExp.insert(0, intraExp);
		break;
	
	case UsagePreferenceMap::HOR:
		// Prepend OR expressions
		if ( !resultExp.empty() )
			resultExp.insert(0, m_UPMap->GetOperator(MPEG7UPMap::HOR)); // TODO use GetMappedCombOp...
		resultExp.insert(0, intraExp);
		break;
	
	case UsagePreferenceMap::AND:
		// Append AND expressions
		if ( !resultExp.empty() )
			resultExp += m_UPMap->GetOperator(MPEG7UPMap::AND); // TODO use GetMappedCombOp...
		resultExp += intraExp;
		break;
	
	case UsagePreferenceMap::HAND:
		// Append AND expressions
		if ( !resultExp.empty() )
			resultExp += m_UPMap->GetOperator(MPEG7UPMap::HAND); // TODO use GetMappedCombOp...
		resultExp += intraExp;
		break;
	}

	return resultExp;
}


//=============================================================================
MPEG7UPList::MPEG7UPList( GenericDS userPreferenceRoot )
{
	m_UserPreferenceRoot = userPreferenceRoot;
	// Self knowledge is good
	// Determine the ranges for FASP, BP, Preferences, and PreferenceTypes
}

//----------------------------------------------------------------------------
MPEG7UPList::~MPEG7UPList()
{
}

//----------------------------------------------------------------------------
GenericDS MPEG7UPList::GetUsagePreferenceNode()
{
	if ( m_UserPreferenceRoot.isNull() )
		return m_UserPreferenceRoot;

	vector<GenericDS> nodes;
	vector<string> p1;
	p1.push_back("Mpeg7");
	p1.push_back("Description");
	p1.push_back("UserPreferences");
	if ( ((nodes = m_UserPreferenceRoot.GetDescription(p1)).size() < 1) )
	{
		eout << "Could not find UserPreferences element in Query" << endl;
		exit(-1);
	}

	return nodes[0];
}

//----------------------------------------------------------------------------
bool MPEG7UPList::GetNextPreferenceNode(GenericDS &parent, GenericDS &node)
{
	// Currently all but last two entries are preferences
	return GetNextNode(parent, node, 0, MPEG7UPMap::nMap - 2);	
}

//----------------------------------------------------------------------------
bool MPEG7UPList::GetNextPreferenceType(GenericDS &parent, GenericDS &node)
{
	// PrefTypes are last two entries
	return GetNextNode(parent, node, MPEG7UPMap::nMap - 2, 2 );	
}

//----------------------------------------------------------------------------
bool MPEG7UPList::GetNextNode(GenericDS &parent, GenericDS &child, int iStartMap, int nCountMap)
{
	DOMNavigator DOMNav;
	MPEG7UPMap UPMap;	// TODO	UPList must have knowledge of UPMap to find the next preference map (has to know what is and is not a preference)?			
	int iName;
	string testName;

	if ( parent.isNull() )
	{
		eout << "Null Node passed in to GetNextNode while seeking " << iStartMap << " through " << iStartMap + nCountMap << endl;
		return false;
	}

	// Get first child or next sibling
	if ( child.isNull() )
		child = parent.GetChild();
	else
		child = child.GetNextSibling();

	// Scan from current node to next, matching any of the names
	while( !child.isNull() )
	{
		// Get ancestry // TODO use GetAncestorPath()
		testName = child.GetDSName();
		GenericDS ancestor = parent; assert(parent == child.GetParent());
		while(!ancestor.isNull())
		{
			testName = ancestor.GetDSName() + "/" + testName;
			ancestor = ancestor.GetParent();
		}
		// Match this name in mapping table
		if (UPMap.FindPreferenceMap(testName,true,iStartMap,nCountMap) >= 0)
			break;

		// Next sibling
		eout << "Unmapped preference element: " << parent.GetDSName().c_str() << "/" << child.GetDSName().c_str() << endl;
		child = child.GetNextSibling();
	}

	return !child.isNull();
}

//=============================================================================
//int MPEG7UPMap::FindPreferenceMap(string nodeName)
int MPEG7UPMap::FindPreferenceMap(string &nodeName, bool bCheckAncestry, int iStartMap, int nCountMap)
{
	int iMap, nCmp;
	char *pComposite;
	bool bSearchTable = true;
	string testName = nodeName;

	// Get root, if we are testing the ancestry
	if (bCheckAncestry && 
		((nCmp = testName.find_last_of('/')) != string::npos) )
		testName.erase(0, nCmp+1);

	// Default range is remainder of map table
	if (nCountMap == 0)
		nCountMap = MPEG7UPMap::nMap - iStartMap;

	while (bSearchTable)
	{
		bSearchTable = false;

		for( iMap = iStartMap; iMap < iStartMap + nCountMap; iMap ++)
		{
			// Compare against mapname, up to the '=' CompositeNode delimiter
			pComposite = strchr(map[iMap].name, '=');
			if (pComposite)
			{
				if ( !strncmp(testName.c_str(), map[iMap].name, pComposite - map[iMap].name) )
					break;
			}
			else
				if ( !strcmp(testName.c_str(), map[iMap].name) )
					break;
		}
		
		// Did we not find it
		if ( iMap == iStartMap + nCountMap )
		{	
			// Grab more of ancestry
			if ( bCheckAncestry  && testName.compare(nodeName) )
			{
				bSearchTable = true;
				testName = nodeName;
				if ( (nCmp = testName.find_last_of('/', nCmp-1)) != string::npos )
					testName.erase(0, nCmp+1);
			}
			else
				// Failed, set iMap to undefined
				iMap = -1;
		}
	}

	//assert(iMap >= 0);	// TODO eout ??

	// Found a portion of the nodeName that matches in the table -- update nodeName
	if (iMap >= 0)
		nodeName = testName;

	return iMap;
}
	// Loop through ancestors building up the name until we find the mapping

//----------------------------------------------------------------------------
const char *MPEG7UPMap::GetMappedLocation(int iMap)
{
	return map[iMap].location;
}

//----------------------------------------------------------------------------
const char *MPEG7UPMap::GetName(int iMap)
{
	return map[iMap].name;
}

//----------------------------------------------------------------------------
const char *MPEG7UPMap::GetMappedTestOperator(int iMap)
{
	switch(map[iMap].testOp)
	{
		case STR_CNT:
		case STR_ISUB:	return ":strisub:";	// This is also the default match op

		case STR_SUB:	return ":strsub:";

		case STR_IEQ:	return ":strieq:";

		case STR_EQ:	return ":streq:";

		case VAL_EQ:	return ":=:";

		case VAL_LT:	return ":<:";

		case VAL_GT:	return ":>:";

		case VAL_GT_LT:	return ":><:";

		case VAL_LTE:	return ":<=:";

		case VAL_GTE:	return ":>=:";

		case VAL_GTE_LTE:	return ":>=<=:";

		case VAL_GTE_LT:	return ":>=<:";

		case NA_TO:
		default:
		{
			eout << "Test Operator \"" << map[iMap].testOp << "\" requested for map of " << map[iMap].name << endl;
		}
	}

	return "NA_ERROR";
}

//----------------------------------------------------------------------------
const char *MPEG7UPMap::GetMappedTestOptions(int iMap)
{
	switch(map[iMap].testOp)
	{
		case STR_CNT:	return "min1.25max2test4";
                default: break;
	}

	return "";
}


//----------------------------------------------------------------------------
UsagePreferenceMap::GroupOp MPEG7UPMap::GetMappedIntraOp(int iMap, bool bToggle)
{
	if (map[iMap].intraOp == ORAND)
	{
		if (!bToggle)
			return UsagePreferenceMap::OR;
		else
			return UsagePreferenceMap::AND;
	}
	else
		return map[iMap].intraOp;
}


//----------------------------------------------------------------------------
const char *UsagePreferenceMap::GetOperator(GroupOp op)
{
	switch(op)
	{
		case OR:	return DOMQuery::GetOperator(DOMQuery::SOFT_OR_OPERATOR);
		case HOR:	return DOMQuery::GetOperator(DOMQuery::HARD_OR_OPERATOR_tag);
		case AND:	return DOMQuery::GetOperator(DOMQuery::SOFT_AND_OPERATOR);
		case HAND:	return DOMQuery::GetOperator(DOMQuery::HARD_AND_OPERATOR_tag);
		default:	break;
	}
	assert(0);
	return "";
}

//----------------------------------------------------------------------------
UsagePreferenceMap::GroupOp UsagePreferenceMap::StringToGroupOp(const char *s)
{
	if		(!strcmp(s,"NA_GO"))	return NA_GO;
	else if	(!strcmp(s,"OR"))		return OR;
	else if	(!strcmp(s,"HOR"))		return HOR;
	else if	(!strcmp(s,"AND"))		return AND;
	else if	(!strcmp(s,"HAND"))		return HAND;
	else if	(!strcmp(s,"ORAND"))	return ORAND;
	else 
	{
		eout << "Invalid GroupOp \"" << s << "\"in MapFile" << endl;
		exit(1);
	}
}

//----------------------------------------------------------------------------
UsagePreferenceMap::TestOp UsagePreferenceMap::StringToTestOp(const char *s)
{
	if		(!strcmp(s,"NA_TO"))		return NA_TO;
	else if	(!strcmp(s,"STR_ISUB"))		return STR_ISUB;
	else if	(!strcmp(s,"STR_SUB"))		return STR_SUB;
	else if	(!strcmp(s,"STR_IEQ"))		return STR_IEQ;
	else if	(!strcmp(s,"STR_EQ"))		return STR_EQ;
	else if	(!strcmp(s,"STR_CNT"))		return STR_CNT;
	else if	(!strcmp(s,"VAL_EQ"))		return VAL_EQ;
	else if	(!strcmp(s,"VAL_GT"))		return VAL_GT;
	else if	(!strcmp(s,"VAL_GT_LT"))	return VAL_GT_LT;
	else if	(!strcmp(s,"VAL_LTE"))		return VAL_LTE;
	else if	(!strcmp(s,"VAL_GTE"))		return VAL_GTE;
	else if	(!strcmp(s,"VAL_GTE_LTE"))	return VAL_GTE_LTE;
	else if	(!strcmp(s,"VAL_GTE_LT"))	return VAL_GTE_LT;
	else 
	{
		eout << "Invalid TestOp \"" << s << "\"in MapFile" << endl;
		exit(1);
	}
}
