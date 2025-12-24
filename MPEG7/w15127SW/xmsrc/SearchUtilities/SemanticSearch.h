///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Author: Ana B. Benitez
// Company: Columbia University
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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  SemanticSearch.h
//

#ifndef __SEMANTIC_SEARCH_H__
#define __SEMANTIC_SEARCH_H__

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
class SemanticSearchTool;

//=============================================================================
class SemanticSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
  SemanticSearchInterfaceABC();
  virtual ~SemanticSearchInterfaceABC() {};

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
class SemanticSearchInterface: 
public SemanticSearchInterfaceABC
{
public:
  SemanticSearchInterface(SemanticSearchTool* aTool);
  virtual ~SemanticSearchInterface();
	
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
  SemanticSearchTool *m_SearchTool;
};

//=============================================================================
class SemanticSearchTool: public Search
{
friend class SemanticSearchInterface;
public:
  SemanticSearchTool();
  SemanticSearchTool(GenericDSInterfaceABC
			*aQueryDescriptionInterface);

  virtual const UUID& GetObjectID(void);
  virtual const char *GetName(void);

  virtual SemanticSearchInterfaceABC *GetInterface(void);

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
  virtual ~SemanticSearchTool();

  SemanticSearchInterface m_Interface;
  GenericDSInterfaceABC *m_RefDescriptionInterface;
  GenericDSInterfaceABC *m_QueryDescriptionInterface;


  // Constant variables
  // A more elegant solution involving the 
  // use of dinamic memory might be found later.
  static const int	STRING_MAXSIZE;		// maximum size of the strings

  // Names of types of segment entities
  static const string SEMANTIC_ENTITY_TYPES[8];

  // Enumerations and structures for the semantic entities and relations
  enum SEM_SemanticEntityType {
	SEMANTIC,
	OBJECT,
	AGENTOBJECT,
	SEMANTICPLACE,
	EVENT,
	CONCEPT,
	SEMANTICSTATE,
	SEMANTICTIME
  };

  // Map of strings for semantic entity relations
  typedef map< string, string> SEM_StrMapType;

  // Structure for a semantic entity
  typedef struct {
	string sId;
	SEM_SemanticEntityType setType;
	string sLabel;
	string sDefinition;
	SEM_StrMapType msEntRelations;
  } SEM_SemanticEntity;

  // Map of semantic entities
  // The semantic entity ids are the keys of the map
  typedef map< string, SEM_SemanticEntity> SEM_SEMapType;

  // Reads the semantic descriptions and extracts the labels and relations
  // for/between the semantic entities 
  int SemanticDecoding(
			GenericDSInterfaceABC *pdsiDSInt, 
	  		SEM_SEMapType& mseSemanticEntities);

  // Read one semantic entity and sub-semantic entities
  // putting them into a vector of semantic entities
  int ReadSemanticEntityTree(GenericDS dsSemEntity,
						 SEM_SEMapType& vseSemEntities, 
						 SEM_SemanticEntity& seSemEntity);

  // Read graph of relations among semantic entities
  int ReadSemanticGraph(GenericDS dsSemGraph, SEM_SEMapType& mseSemEntities);

  // Type of semantic entity
  int TypeOfSemanticEntity(GenericDS dsSemEntity, SEM_SemanticEntityType& setType);

  // Matches the semantic descriptions based on the labels and relations
  // for/between the semantic entities 
  double SemanticMatching(
			SEM_SEMapType mseSemanticEntitiesRef,
			SEM_SEMapType mseSemanticEntitiesQuery);

  // Remove dummy words and signs from two strings and returns
  // and score on how similar the two string are
  double MatchStrings(string str1, string str2);
  // Replaces multiple sub-string for another sub-string in a string
  void ReplaceStrings(string& string, char *strListTypes, ...);
  // Replaces a sub-string for another sub-string in a string
  void ReplaceInString(string& string, char *str1, char *str2);
};

};

#endif //__SEMANTIC_SEARCH_H__
