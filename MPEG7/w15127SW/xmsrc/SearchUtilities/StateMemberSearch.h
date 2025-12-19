///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Hawley K. Rising III, Mohammed Zubair Visharam
// Company: Sony Corp.
// From a basis (SemanticSearch) by Ana Belen Benitez Jimenez, Columbia University.
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
// purpose, assign or donate the code to a third party and inhibit third
// parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2001.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  StateMemberSearch.h
//


// Redefine the StateMemberSearch files to do SemanticState based MemberFunction
// searches against a property or set of properties in a concept.

#ifndef __STATEMEMBER_SEARCH_H__
#define __STATEMEMBER_SEARCH_H__

#include <map>
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "Descriptors/Descriptors.h"
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "Media/Media.h"


//=============================================================================

namespace XM
{

#ifdef WIN32
// Make the map and functional datatype available from the STL std namespace.
using std::map;
#endif

// Forward Declarations:
class StateMemberSearchTool;

//=============================================================================
class StateMemberSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  StateMemberSearchInterfaceABC();
  virtual ~StateMemberSearchInterfaceABC() {};

  virtual void destroy() = 0;

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface) = 0;
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void) = 0;
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface) = 0;

  virtual double GetDistance(void) = 0;
	
  static const UUID myID;
};

//=============================================================================
class StateMemberSearchInterface: 
public StateMemberSearchInterfaceABC
{
public:
  StateMemberSearchInterface(StateMemberSearchTool* aTool);
  virtual ~StateMemberSearchInterface();
	
  virtual const UUID& GetInterfaceID(void);
  virtual const char *GetName(void);

  virtual void destroy();

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char *myName;

private:
  StateMemberSearchTool *m_SearchTool;
};

//=============================================================================
class StateMemberSearchTool: public Search
{
friend class StateMemberSearchInterface;
public:
  StateMemberSearchTool();
  StateMemberSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual StateMemberSearchInterfaceABC *GetInterface(void);

  virtual bool SupportsPush(void);
  virtual bool SupportsPull(void);

  virtual int SetRefDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);
  virtual GenericDSInterfaceABC* GetQueryDescriptionInterface(void);
  virtual int SetQueryDescriptionInterface
    (GenericDSInterfaceABC *aSemanticDescriptionInterface);

  virtual double GetDistance(void);

  static const UUID myID;
  static const char * myName;

private:
  virtual ~StateMemberSearchTool();

  StateMemberSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;


  // Constant variables
  // A more elegant solution involving the 
  // use of dinamic memory might be found later.
  static const int	STRING_MAXSIZE;		// maximum size of the strings

  // Names of types of segment entities
  static const string SEMANTIC_ENTITY_TYPES[10];

  // Enumerations and structures for the semantic entities and relations
  // This has been updated to include AgentObject, and change to new names
  // Added SEMANTICBASE for indeterminate types.
  // Also removed PROPERTIES since this cannot occur as a semantic entity
  enum STM_SemanticEntityType {
	SEMANTIC,
	OBJECT,
	PERSONOBJECT,
	SEMANTICPLACE,
	EVENT,
	SEMANTICBASE,
	CONCEPT,
	SEMANTICSTATE,
	SEMANTICTIME,
	AGENTOBJECT
  };

//  typedef map< string, string> StrMapType;

  
  // Map of strings for semantic entity relations
  // We adopt a more complex version of cataloging
  // relations:
  // Still stored under the "source" field,
  // still keyed there by target, but this is then
  // another map, which is keyed on id.  Since not
  // all relations are assigned id's, we assign them
  // when they do not exist by id="sourcetargetnumber"
  // where number is equal to size() on the target
  // when the relation is stored.

  // Structure for a relation
  
  typedef struct
  {
	  string sId;
	  string sName;
	  string sType;

  } RelationDS;

  // Map of relations, keyed by id, used as target fields.

  typedef map<string,RelationDS> RelMapType;

  // Map used for targets in the Semantic Entities

  typedef map<string,RelMapType> TargetMapType;


  // Structure for a semantic entity
  typedef struct {
	string sId;
	int abstractionLevel;
	STM_SemanticEntityType setType;
	string sLabel;
	string sDefinition;
	string sProperty;
	// The following two fields are solely for using membership functions.
	// In a general tool, this would be in a derived class.
	string sMemberFunction;
	double dMemberFunctionValue;
	TargetMapType msEntRelations;
  } STM_SemanticEntity;

  // Map of semantic entities
  // The semantic entity ids are the keys of the map
  typedef map< string, STM_SemanticEntity> STM_SEMapType;

  // Reads the semantic descriptions and extracts the labels and relations
  // for/between the semantic entities 
  int SemanticDecoding(
			GenericDSInterfaceABC *pdsiDSInt, 
	  		STM_SEMapType& mseSemanticEntities);

  // Reads the semantic base portion of a description scheme
  // interprets everything but the MediaOccurrence.
  int ReadSemanticBase(GenericDS& dsSemEntity,STM_SemanticEntity& seSemEntity, GenericDS& dsSubSemEntity );

  
  // Reads any relations coming directly from semantic objects.
  // Essentially puts these relations into the composite graph.
  int ReadSemanticRelation(GenericDS dsEdge,STM_SemanticEntity& seSemEntity);

// New functions to cover the various semantic entities
int ReadSemanticObject(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string &sSubId/*=string("") //sth*/);
int ReadSemanticEvent(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string &sSubId/*=string("") //sth*/);
int ReadSemanticTime(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities);
int ReadSemanticPlace(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities);
int ReadSemanticState(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities);
int ReadSemanticConcept(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities);
int ReadSemanticSemantic(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities,string &sSubId/*=string("") //sth*/);
int ReadSemanticAgentObject(GenericDS dsSemEntity,STM_SEMapType& mseSemEntities);

  // Read one semantic entity and sub-semantic entities
  // putting them into a vector of semantic entities
  int ReadSemanticEntityTree(GenericDS dsSemEntity,
						 STM_SEMapType& vseSemEntities, 
						 STM_SemanticEntity& seSemEntity);

  // added to separate out graph reading tasks (hkr)
int ReadGraphEdge(GenericDS dsEdge,STM_SEMapType& mseSemEntities);
int ReadGraphNode(GenericDS dsNode,STM_SEMapType& mseSemEntities);
int ReadGraphProperty(GenericDS dsProperty,STM_SEMapType& mseSemEntities);



  // Read graph of relations among semantic entities
  int ReadSemanticGraph(GenericDS dsSemGraph, STM_SEMapType& mseSemEntities);

  // Type of semantic entity
  int TypeOfSemanticEntity(GenericDS dsSemEntity, STM_SemanticEntityType& setType);

  // Matches the semantic descriptions based on the labels and relations
  // for/between the semantic entities 
  double SemanticMatching(
			STM_SEMapType mseSemanticEntitiesRef,
			STM_SEMapType mseSemanticEntitiesQuery);

  // Remove dummy words and signs from two strings and returns
  // and score on how similar the two string are
  double MatchStrings(string str1, string str2);
  // Replaces multiple sub-string for another sub-string in a string
  void ReplaceStrings(string& string, char *strListTypes, ...);
  // Replaces a sub-string for another sub-string in a string
  void ReplaceInString(string& string, char *str1, char *str2);
double GetMembershipDistance(STM_SEMapType mseSemEntitiesRef,STM_SEMapType mseSemEntitiesQuery);

};

};

#endif //__SEMANTIC_SEARCH_H__
